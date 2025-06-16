// server/src/Server.cpp
#include "Server.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <algorithm> // Pour std::remove_if

Server::Server(int port)
    : m_port(port), m_listenSocket(INVALID_SOCKET), m_isRunning(false) {}

Server::~Server() {
    stop();
}

bool Server::start() {
    initializeWinsock();

    m_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listenSocket == INVALID_SOCKET) {
        std::cerr << "Erreur : Impossible de créer le socket d'écoute." << std::endl;
        return false;
    }
    std::cout << "Socket d'écoute créé." << std::endl;

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(static_cast<u_short>(m_port));
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(m_listenSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Erreur : Bind a échoué." << std::endl;
        cleanupSockets();
        return false;
    }
    std::cout << "Socket lié au port " << m_port << "." << std::endl;

    if (listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Erreur : Listen a échoué." << std::endl;
        cleanupSockets();
        return false;
    }
    std::cout << "Serveur en écoute..." << std::endl;

    m_isRunning.store(true);
    m_acceptThread = std::thread(&Server::acceptConnections, this);

    return true;
}

void Server::stop() {
    // exchange() met la valeur à false et retourne l'ancienne valeur.
    // Si l'ancienne valeur était déjà false, le serveur est déjà en cours d'arrêt.
    if (!m_isRunning.exchange(false)) {
        return;
    }
    std::cout << "Arrêt du serveur en cours..." << std::endl;

    // Fermer le socket d'écoute pour débloquer la fonction `accept()`
    cleanupSockets();

    // Attendre la fin du thread d'acceptation
    if (m_acceptThread.joinable()) {
        m_acceptThread.join();
    }
    std::cout << "Thread d'acceptation terminé." << std::endl;
    
    // Attendre la fin de tous les threads clients pour un arrêt propre
    {
        std::lock_guard<std::mutex> lock(m_clientThreadsMutex);
        for (auto& th : m_clientThreads) {
            if (th.joinable()) {
                th.join();
            }
        }
        m_clientThreads.clear();
    }
    std::cout << "Tous les threads clients sont terminés." << std::endl;
}

void Server::acceptConnections() {
    while (m_isRunning.load()) {
        socket_t clientSocket = accept(m_listenSocket, nullptr, nullptr);

        if (clientSocket == INVALID_SOCKET) {
            // Cette condition est normale si le serveur s'arrête pendant qu'il attend sur accept()
            if (m_isRunning.load()) {
                 std::cerr << "Erreur lors de l'acceptation d'un client." << std::endl;
            }
            break; // Sortir de la boucle, le serveur s'arrête.
        }
        
        std::cout << "Nouveau client connecté (socket: " << clientSocket << ")." << std::endl;
        
        // CORRECTION MAJEURE: Stocker le thread au lieu de le détacher
        std::lock_guard<std::mutex> lock(m_clientThreadsMutex);
        m_clientThreads.emplace_back(&Server::handleClient, this, clientSocket);
    }
    std::cout << "Arrêt de la boucle d'acceptation des connexions." << std::endl;
}

void Server::handleClient(socket_t clientSocket) {
    // 1. Recevoir le pseudo du client
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    
    if (bytesReceived <= 0) {
        std::cerr << "Client (socket: " << clientSocket << ") déconnecté avant l'authentification." << std::endl;
        removeClient(clientSocket);
        return;
    }

    std::string pseudo(buffer, bytesReceived);
    
    // 2. Ajouter le client à la liste et informer tout le monde
    {
        std::lock_guard<std::mutex> lock(m_clientsMutex);
        m_clients[clientSocket] = pseudo;
    }
    
    std::string joinMsg = m_messageHandler.encode_join_message(pseudo);
    std::cout << "[LOG] " << pseudo << " a rejoint le tchat." << std::endl;
    broadcastMessage(joinMsg);
    broadcastUserList();

    // 3. Boucle principale pour recevoir les messages du client
    while (m_isRunning.load()) {
        memset(buffer, 0, sizeof(buffer));
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived <= 0) {
            // Le client s'est déconnecté (proprement ou non)
            break; // Sortir de la boucle pour nettoyer
        }
        
        std::string message(buffer, bytesReceived);
        std::string formattedMessage = m_messageHandler.encode_chat_message(pseudo, message);
        std::cout << "[MSG] De " << pseudo << ": " << message << std::endl;

        // Diffuser le message à tous les autres clients
        broadcastMessage(formattedMessage, clientSocket);
    }

    // 4. Nettoyage après la déconnexion du client
    std::string removedPseudo = removeClient(clientSocket);
    if (!removedPseudo.empty()) {
        std::cout << "[LOG] " << removedPseudo << " a quitté le tchat." << std::endl;
        std::string partMsg = m_messageHandler.encode_leave_message(removedPseudo);
        broadcastMessage(partMsg);
        broadcastUserList();
    }
    // Le thread se terminera ici, et sera joint lors de l'arrêt du serveur.
}

void Server::broadcastUserList() {
    std::vector<std::string> user_pseudos;
    
    // CORRECTION : Le verrou est nécessaire pour lire m_clients en toute sécurité.
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    for (const auto& pair : m_clients) {
        user_pseudos.push_back(pair.second);
    }
    
    std::string message = m_messageHandler.encode_userlist_message(user_pseudos);
    _broadcastMessage_unsafe(message, INVALID_SOCKET);
}


void Server::_broadcastMessage_unsafe(const std::string& message, socket_t senderSocket) {
    // Aucune prise de verrou ici, car on suppose que l'appelant l'a fait
    for (const auto& pair : m_clients) {
        if (pair.first != senderSocket) {
            send(pair.first, message.c_str(), static_cast<int>(message.length()), 0);
        }
    }
}

void Server::broadcastMessage(const std::string& message) {
    broadcastMessage(message, INVALID_SOCKET); // Délègue à l'autre surcharge
}

void Server::broadcastMessage(const std::string& message, socket_t senderSocket) {
    // CORRECTION : Le verrou est pris ici pour TOUTES les opérations de diffusion.
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    for (const auto& pair : m_clients) {
        if (pair.first != senderSocket) {
            send(pair.first, message.c_str(), static_cast<int>(message.length()), 0);
        }
    }
}

std::string Server::removeClient(socket_t clientSocket) {
    std::string pseudo = "";
    {
        std::lock_guard<std::mutex> lock(m_clientsMutex);
        auto it = m_clients.find(clientSocket);
        if (it != m_clients.end()) {
            pseudo = it->second;
            m_clients.erase(it);
        }
    }
    // Fermer le socket après avoir retiré le client de la liste
#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif
    return pseudo;
}

void Server::initializeWinsock() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup a échoué." << std::endl;
        exit(1);
    }
#endif
}

void Server::cleanupSockets() {
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
}