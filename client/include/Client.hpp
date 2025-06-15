// client/include/Client.hpp
#pragma once

#include <string>
#include <thread>

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

class Client {
public:
    Client();
    ~Client();

    bool connectToServer(const std::string& ip, int port);
    void sendMsg(const std::string& message);

    void run();
    void start();

private:
    void initializeWinsock(); // Spécifique à Windows
    void receiveMessages(); 
    void cleanup();

    socket_t m_socket;
    bool m_isConnected;
    std::thread m_receiveThread;
};