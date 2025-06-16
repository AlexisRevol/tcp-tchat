#include "Server.hpp"
#include <iostream>
#include <string>
#include <cstring>

Server::Server(int port) : m_port(port), m_listenSocket(INVALID_SOCKET), m_isRunning(false) {
}

Server::~Server() {
    stop();
}

bool Server::start() {
    initializeWinsock();

    m_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listenSocket == INVALID_SOCKET) {
        std::cerr << "Erreur: Impossible de creer le socket." << std::endl;
        return false;
    }
    std::cout << "Socket cree." << std::endl;

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

    if (listen(m_listenSocket, 5) < 0) {
        std::cerr << "Erreur: Listen a echoue." << std::endl;
        cleanup();
        return false;
    }
    std::cout << "Serveur en ecoute..." << std::endl;

    m_isRunning = true;
    m_acceptThread = std::thread(&Server::acceptConnections, this);

    return true;
}

void Server::stop() {
    if (!m_isRunning.exchange(false)) {
        return; // Déjà arrêté
    }
    std::cout << "Arret du serveur..." << std::endl;

    // Fermer le socket d'écoute pour débloquer la fonction accept()
    cleanup();
    
    // Attendre la fin du thread d'acceptation
    if (m_acceptThread.joinable()) {
        m_acceptThread.join();
    }
    std::cout << "Thread d'acceptation termine." << std::endl;
}


void Server::acceptConnections() {
    while (m_isRunning) {
        socket_t clientSocket = accept(m_listenSocket, nullptr, nullptr);

        if (clientSocket == INVALID_SOCKET) {
            if (m_isRunning) {
                // Cette erreur est normale si le serveur s'arrête pendant qu'il attend sur accept()
                std::cout << "accept() a echoue ou a ete interrompu, arret de la boucle." << std::endl;
            }
            break; // Sortir de la boucle
        }

        std::cout << "Nouveau client connecte." << std::endl;
        std::thread(&Server::handleClient, this, clientSocket).detach();
    }
}

void Server::handleClient(socket_t clientSocket) {
    // Recevoir le pseudo du client en premier message
    char buffer[4096];
    memset(buffer, 0, 4096);
    int bytesReceived = recv(clientSocket, buffer, 4096, 0);
    
    if (bytesReceived <= 0) {
        std::cout << "Client deconnecte avant de fournir un pseudo." << std::endl;
#ifdef _WIN32
        closesocket(clientSocket);
#else
        close(clientSocket);
#endif
        return;
    }

    std::string pseudo = std::string(buffer, 0, bytesReceived);
    
    // Stocker le client et son pseudo, et informer les autres
    {
        std::lock_guard<std::mutex> lock(m_clientsMutex);
        m_clients[clientSocket] = pseudo;
    }

    broadcastUserList();

    std::cout << m_messageHandler.encode_join_message(pseudo) << std::endl;
    broadcastMessage(m_messageHandler.encode_join_message(pseudo));

    // Boucle pour recevoir les messages de tchat
    while (true) {
        memset(buffer, 0, 4096);
        bytesReceived = recv(clientSocket, buffer, 4096, 0);

        if (bytesReceived <= 0) {
            // Le client s'est déconnecté
            removeClient(clientSocket);
            
            broadcastMessage(m_messageHandler.encode_leave_message(pseudo));
            broadcastUserList();
            std::cout << m_messageHandler.encode_leave_message(pseudo) << std::endl;
            break;
        }
        
        std::string message = std::string(buffer, 0, bytesReceived);
        
        std::string formattedMessage = m_messageHandler.encode_chat_message(pseudo, message);
        std::cout << "Recu: " << formattedMessage << std::endl;

        // Diffuser le message formaté
        broadcastMessage(formattedMessage, clientSocket);
    }
}

void Server::broadcastUserList() {
    // std::lock_guard<std::mutex> lock(m_clientsMutex);
    
    std::vector<std::string> user_pseudos;
    for (const auto& pair : m_clients) {
        user_pseudos.push_back(pair.second);
    }
    
    std::string message = m_messageHandler.encode_userlist_message(user_pseudos);
    broadcastMessage(message); // On envoie à tout le monde
}

// Version pour diffuser à tout le monde
void Server::broadcastMessage(const std::string& message) {
    _broadcastMessage_unsafe(message, INVALID_SOCKET);
}

// Version pour diffuser à tout le monde sauf à l'expéditeur
void Server::broadcastMessage(const std::string& message, socket_t senderSocket) {
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    _broadcastMessage_unsafe(message, senderSocket);
}

void Server::_broadcastMessage_unsafe(const std::string& message, socket_t senderSocket) {
    for (const auto& pair : m_clients) {
        if (pair.first != senderSocket) {
            send(pair.first, message.c_str(), static_cast<int>(message.length()), 0);
        }
    }
}

void Server::removeClient(socket_t clientSocket) {
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    
    // Retirer de la map
    m_clients.erase(clientSocket);
    
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