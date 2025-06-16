#include "Client.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <vector>

Client::Client(QObject *parent)
    : QObject(parent), m_socket(INVALID_SOCKET), m_isConnected(false) {
    initializeWinsock();
    // Enregistrer le type ParsedMessage pour qu'il puisse être utilisé dans les signaux/slots
    qRegisterMetaType<ParsedMessage>("ParsedMessage");
}

Client::~Client() {
    disconnectFromServer();
    cleanup();
}

void Client::connectToServer(const std::string& ip, int port) {
    // Si déjà connecté, ne rien faire
    if (m_isConnected.load()) {
        return;
    }

    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket == INVALID_SOCKET) {
        emit connectionStatusChanged(false, "Erreur : Impossible de créer le socket.");
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(static_cast<u_short>(port));
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

    if (::connect(m_socket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        emit connectionStatusChanged(false, "Erreur : Connexion au serveur échouée.");
        cleanup(); // Nettoie le socket qui a été créé
        return;
    }

    m_isConnected.store(true);
    m_receiveThread = std::thread(&Client::receiveMessages, this);

    emit connectionStatusChanged(true, "Connecté");
}

void Client::sendMsg(const std::string& message) {
    if (m_isConnected.load() && m_socket != INVALID_SOCKET) {
        send(m_socket, message.c_str(), static_cast<int>(message.length()), 0);
    }
}

void Client::receiveMessages() {
    char buffer[4096];
    
    // Boucle de réception tant que la connexion est active.
    // .load() lis la valeur atomique de manière sécurisée.
    while (m_isConnected.load()) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(m_socket, buffer, sizeof(buffer), 0);

        if (bytesReceived > 0) {
            std::string received_data(buffer, bytesReceived);

            size_t start = 0;
            size_t end;
            while ((end = received_data.find('\n', start)) != std::string::npos) {
                processRawMessage(received_data.substr(start, end - start));
                start = end + 1;
            }
            if (start < received_data.length()) {
                processRawMessage(received_data.substr(start));
            }
            
        } else {
            m_isConnected.store(false);
        }
    }

    // Une fois sorti de la boucle, on notifie l'UI.
    emit connectionStatusChanged(false, "Déconnecté du serveur");
}

void Client::processRawMessage(const std::string& raw_message) {
    if (raw_message.empty()) return;

    ParsedMessage parsed_msg = m_messageHandler.decode(raw_message);

    if (parsed_msg.command == Command::USERLIST) {
        QStringList userList;
        for(const auto& param : parsed_msg.params) {
            userList.append(QString::fromStdString(param));
        }
        emit userListUpdated(userList);
    } else {
        emit newMessageReceived(parsed_msg);
    }
}


void Client::disconnectFromServer() {
    if (!m_isConnected.load()) {
        return;
    }
    m_isConnected.store(false);
    
#ifdef _WIN32
    closesocket(m_socket);
#else
    close(m_socket);
#endif
    m_socket = INVALID_SOCKET;

}


void Client::initializeWinsock() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        exit(1);
    }
#endif
}

void Client::cleanup() {
    if (m_receiveThread.joinable()) {
        m_receiveThread.join(); // Attend que le thread se termine.
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