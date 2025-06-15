// client/include/Client.hpp
#pragma once

#include <QObject>
#include <string>
#include <thread>

#include "ParsedMessage.hpp"
#include "MessageHandler.hpp"

// On réutilise les mêmes définitions de sockets que pour le serveur
// pour la compatibilité Windows / POSIX.
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment (lib, "Ws2_32.lib")
    using socket_t = SOCKET;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    using socket_t = int;
    const int INVALID_SOCKET = -1;
#endif

class Client : public QObject {
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);
    ~Client();

    void run();
    void start();

public slots:
    bool connectToServer(const std::string& ip, int port);
    void sendMsg(const std::string& message);
    void disconnectFromServer();
    
signals:
    // Les signaux que notre logique enverra à l'interface
    void connectionStatusChanged(bool isConnected, const QString& message);
    void newMessageReceived(const QString& message);

private:
    void initializeWinsock(); 
    void receiveMessages(); 
    void cleanup();

    socket_t m_socket;
    bool m_isConnected;
    std::thread m_receiveThread;

    MessageHandler m_messageHandler; 
};