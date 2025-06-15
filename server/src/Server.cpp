#include "Server.hpp"
#include <iostream>
#include <string>

Server::Server(int port) : m_port(port), m_listenSocket(INVALID_SOCKET) {
}

Server::~Server() {
    cleanup();
}

bool Server::start() {
    // Initialiser Winsock (uniquement pour Windows)
    initializeWinsock();

    // Créer le socket d'écoute
    m_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listenSocket == INVALID_SOCKET) {
        std::cerr << "Erreur: Impossible de creer le socket." << std::endl;
        return false;
    }
    std::cout << "Socket cree." << std::endl;

    // Lier le socket à une adresse IP et un port
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(m_port);
    serverAddr.sin_addr.s_addr = INADDR_ANY; 

    if (bind(m_listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Erreur: Bind a echoue." << std::endl;
        cleanup();
        return false;
    }
    std::cout << "Socket lie au port " << m_port << "." << std::endl;

    // Mettre le socket en écoute
    if (listen(m_listenSocket, 5) < 0) { // 5 = taille de la file d'attente des connexions
        std::cerr << "Erreur: Listen a echoue." << std::endl;
        cleanup();
        return false;
    }
    std::cout << "Serveur en ecoute..." << std::endl;

    // Accepter les connexions dans un thread dédié pour ne pas bloquer le main
    acceptConnections();

    return true;
}

void Server::acceptConnections() {
    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientAddrSize = sizeof(clientAddr);
        socket_t clientSocket = accept(m_listenSocket, (sockaddr*)&clientAddr, &clientAddrSize);

        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Erreur: Accept a echoue." << std::endl;
            continue;
        }

        char clientIp[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIp, INET_ADDRSTRLEN);
        std::cout << "Nouveau client connecte depuis " << clientIp << std::endl;

        // Verrouiller le mutex avant de modifier la liste des clients
        {
            std::lock_guard<std::mutex> lock(m_clientsMutex);
            m_clientSockets.push_back(clientSocket);
        }

        // Lancer un thread pour ce client
        std::thread(&Server::handleClient, this, clientSocket).detach();
    }
}

void Server::handleClient(socket_t clientSocket) {
    char buffer[4096];
    while (true) {
        memset(buffer, 0, 4096);
        int bytesReceived = recv(clientSocket, buffer, 4096, 0);

        if (bytesReceived <= 0) {
            std::cout << "Client deconnecte." << std::endl;
            removeClient(clientSocket);
            break; // Terminer le thread
        }
        
        std::string message = std::string(buffer, 0, bytesReceived);
        std::cout << "Recu: " << message << std::endl;

        // Diffuser le message
        broadcastMessage(message, clientSocket);
    }
}

void Server::broadcastMessage(const std::string& message, socket_t senderSocket) {
    std::lock_guard<std::mutex> lock(m_clientsMutex);

    for (socket_t client : m_clientSockets) {
        // Envoyer le message à tout le monde sauf à l'expéditeur
        if (client != senderSocket) {
            send(client, message.c_str(), static_cast<int>(message.length()), 0);
        }
    }
}

void Server::removeClient(socket_t clientSocket) {
    std::lock_guard<std::mutex> lock(m_clientsMutex);

    auto it = std::remove(m_clientSockets.begin(), m_clientSockets.end(), clientSocket);
    if (it != m_clientSockets.end()) {
        m_clientSockets.erase(it, m_clientSockets.end());
    }

    // Fermer le socket
#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif
}


void Server::initializeWinsock() {
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup a echoue: " << result << std::endl;
        exit(1);
    }
#endif
}

void Server::cleanup() {
    if (m_listenSocket != INVALID_SOCKET) {
#ifdef _WIN32
        closesocket(m_listenSocket);
#else
        close(m_listenSocket);
#endif
        m_listenSocket = INVALID_SOCKET;
    }

#ifdef _WIN32
    WSACleanup();
#endif

    // (Pour une version avancée) Attendre la fin de tous les threads clients
    // for (auto& th : m_clientThreads) {
    //     if (th.joinable()) {
    //         th.join();
    //     }
    // }
}