#include "Client.hpp"
#include <iostream>
#include <string>

Client::Client() : m_socket(INVALID_SOCKET), m_isConnected(false) {
    initializeWinsock();
}

Client::~Client() {
    cleanup();
}

bool Client::connectToServer(const std::string& ip, int port) {
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket == INVALID_SOCKET) {
        std::cerr << "Erreur: Impossible de creer le socket." << std::endl;
        return false;
    }
    std::cout << "Socket client cree." << std::endl;

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    
    // Convertir l'adresse IP de texte à binaire
    if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Erreur: Adresse IP invalide." << std::endl;
        cleanup();
        return false;
    }

    // Tenter la connexion
    if (connect(m_socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Erreur: Connexion au serveur echouee." << std::endl;
        cleanup();
        return false;
    }

    m_isConnected = true;
    std::cout << "Connecte au serveur !" << std::endl;
    
    // Lancer un thread pour écouter les messages du serveur
    m_receiveThread = std::thread(&Client::receiveMessages, this);

    return true;
}

void Client::run() {
    // Boucle principale pour lire les entrées de l'utilisateur et les envoyer
    std::string line;
    while (m_isConnected) {
        std::getline(std::cin, line);
        if (!line.empty()) {
            sendMsg(line);
        }
    }
}

void Client::sendMsg(const std::string& message) {
    if (m_isConnected) {
        send(m_socket, message.c_str(), static_cast<int>(message.length()), 0);
    }
}

void Client::receiveMessages() {
    char buffer[4096];
    while (m_isConnected) {
        memset(buffer, 0, 4096);
        int bytesReceived = recv(m_socket, buffer, 4096, 0);

        if (bytesReceived > 0) {
            // Afficher le message reçu
            std::cout << "Reçu > " << std::string(buffer, 0, bytesReceived) << std::endl;
        } else {
            std::cout << "Deconnecte du serveur." << std::endl;
            m_isConnected = false;
        }
    }
}

void Client::initializeWinsock() {
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup a echoue: " << result << std::endl;
        exit(1);
    }
#endif
}

void Client::cleanup() {
    if (m_receiveThread.joinable()) {
        m_receiveThread.join();
    }
    if (m_socket != INVALID_SOCKET) {
#ifdef _WIN32
        closesocket(m_socket);
#else
        close(m_socket);
#endif
        m_socket = INVALID_SOCKET;
    }
#ifdef _WIN32
    WSACleanup();
#endif
}