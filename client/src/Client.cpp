#include "Client.hpp"
#include <iostream>
#include <string>
#include <cstring>

Client::Client(QObject *parent) 
    : QObject(parent), m_socket(INVALID_SOCKET), m_isConnected(false) {
    initializeWinsock();
}

Client::~Client() {
    cleanup();
}

bool Client::connectToServer(const std::string& ip, int port) {
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket == INVALID_SOCKET) {
        emit connectionStatusChanged(false, "Erreur: Impossible de creer le socket.");
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

    if (::connect(m_socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        emit connectionStatusChanged(false, "Erreur: Connexion au serveur echouee.");
        cleanup();
        return false;
    }

    m_isConnected = true;
    m_receiveThread = std::thread(&Client::receiveMessages, this);
    
    // On envoie le signal de succès !
    emit connectionStatusChanged(true, "Connecte au serveur !");

    return true;
}

void Client::start() {
    const std::string SERVER_IP = "127.0.0.1";
    const int SERVER_PORT = 8080;

    if (connectToServer(SERVER_IP, SERVER_PORT)) {
        run();
    } else {
        std::cerr << "Impossible de se connecter au serveur." << std::endl;
    }
}

void Client::run() {
    // Demander et envoyer le pseudo
    std::string pseudo;
    std::cout << "Entrez votre pseudo: ";
    std::getline(std::cin, pseudo);
    if (pseudo.empty()) {
        pseudo = "Anonyme";
    }
    sendMsg(pseudo);
    std::cout << "--- Tchat commence. Tapez vos messages. ---" << std::endl;

    // Boucle principale pour envoyer les messages
    std::string line;
    while (m_isConnected) {
        std::getline(std::cin, line);
        if (!line.empty() && m_isConnected) {
            sendMsg(line);
        } else if (!m_isConnected) {
            break; 
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
            std::string raw_msg(buffer, 0, bytesReceived);
            ParsedMessage parsed_msg = m_messageHandler.decode(raw_msg);
            QString display_message;

            switch(parsed_msg.command) {
                case Command::MSG:
                    if (parsed_msg.params.size() >= 2) {
                        display_message = QString::fromStdString(parsed_msg.params[0] + ": " + parsed_msg.params[1]);
                    }
                    break;
                case Command::JOIN:
                    if (!parsed_msg.params.empty()) {
                        display_message = QString("--- %1 a rejoint le tchat. ---").arg(QString::fromStdString(parsed_msg.params[0]));
                    }
                    break;
                case Command::PART:
                     if (!parsed_msg.params.empty()) {
                        display_message = QString("--- %1 a quitte le tchat. ---").arg(QString::fromStdString(parsed_msg.params[0]));
                    }
                    break;
                case Command::UNKNOWN:
                    display_message = "Message du serveur non reconnu.";
                    break;
            }
            if (!display_message.isEmpty()) {
                emit newMessageReceived(display_message);
            }
        } else {
            m_isConnected = false;
        }
    }
    emit connectionStatusChanged(false, "Deconnecte du serveur.");
}

void Client::disconnectFromServer() {
    if (m_isConnected) {
        m_isConnected = false; 
        cleanup(); 
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