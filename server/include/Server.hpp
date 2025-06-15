#pragma once

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <map>

// Gérer les différences de Sockets entre Windows et POSIX (Linux, macOS)
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    // Link avec la bibliothèque Ws2_32.lib
    #pragma comment (lib, "Ws2_32.lib")
    // On définit un type plus simple pour les sockets, comme sur POSIX
    using socket_t = SOCKET;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    // On définit un type pour la compatibilité et on signale un socket invalide
    using socket_t = int;
    const int INVALID_SOCKET = -1;
#endif


class Server {
public:
    Server(int port);
    ~Server();
    bool start();

private:
    void initializeWinsock(); // Spécifique à Windows
    void acceptConnections();
    void handleClient(socket_t clientSocket);

    void broadcastMessage(const std::string& message);
    void broadcastMessage(const std::string& message, socket_t senderSocket);
    
    // NOUVELLE méthode pour retirer un client
    void removeClient(socket_t clientSocket);

    void cleanup();

    int m_port;
    socket_t m_listenSocket;
   
    //std::vector<socket_t> m_clientSockets; 
    std::map<socket_t, std::string> m_clients;
    std::mutex m_clientsMutex;      
};