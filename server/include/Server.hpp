#pragma once

#include <string>
#include <vector>
#include <thread>

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
    // Le constructeur prend le port en argument
    Server(int port);
    // Le destructeur s'assurera de tout nettoyer proprement
    ~Server();

    // Méthode pour démarrer le serveur
    bool start();

private:
    void initializeWinsock(); // Spécifique à Windows
    void acceptConnections();
    void handleClient(socket_t clientSocket);
    void cleanup();

    int m_port;
    socket_t m_listenSocket;
    std::vector<std::thread> m_clientThreads;
};