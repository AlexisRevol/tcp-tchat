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

    // La boucle principale pour écouter le serveur
    while (m_isConnected) {
        memset(buffer, 0, 4096);
        int bytesReceived = recv(m_socket, buffer, 4096, 0);

        if (bytesReceived > 0) {
            // 1. On reçoit des données brutes et on les décode
            std::string raw_msg(buffer, 0, bytesReceived);
            ParsedMessage parsed_msg = m_messageHandler.decode(raw_msg);

            // 2. On émet simplement l'objet ParsedMessage.
            //    C'est maintenant la responsabilité de la MainWindow de l'interpréter.
            //    Cela s'applique à TOUS les types de commandes (MSG, JOIN, PART...).
            emit newMessageReceived(parsed_msg);
            
            // 3. On gère le cas spécial de la liste d'utilisateurs.
            //    On pourrait aussi inclure ça dans le signal newMessageReceived,
            //    mais le séparer est un peu plus propre car la donnée est différente.
            if (parsed_msg.command == Command::USERLIST) {
                QStringList userList;
                // On ignore le premier paramètre qui est la commande elle-même.
                // Note: Mon implémentation de decode était légèrement imparfaite,
                // elle devrait peut-être ne pas inclure la commande dans les params.
                // On va corriger ça en faisant une boucle sur parsed_msg.params.
                for(const auto& param : parsed_msg.params) {
                    userList.append(QString::fromStdString(param));
                }
                emit userListUpdated(userList);
            }

        } else {
            // Le serveur s'est déconnecté ou une erreur est survenue
            m_isConnected = false;
        }
    }

    // Une fois sorti de la boucle, on notifie l'UI que la connexion est terminée.
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