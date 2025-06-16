// server/include/Server.hpp
#pragma once

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include <atomic>

#include "MessageHandler.hpp"
#include "ParsedMessage.hpp"

// Définitions de sockets multi-plateformes
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
    constexpr int INVALID_SOCKET = -1;
    constexpr int SOCKET_ERROR = -1;
#endif

/**
 * @class Server
 * @brief Gère un serveur de tchat TCP multi-clients et multi-threadé.
 *
 * Le serveur écoute les connexions entrantes sur un port spécifié.
 * Chaque client connecté est géré dans un thread dédié.
 * La synchronisation pour l'accès aux données partagées (liste des clients)
 * est assurée par des mutex. L'arrêt du serveur est géré proprement
 * via un drapeau atomique et en joignant tous les threads actifs.
 */
class Server {
public:
    /**
     * @brief Construit un serveur qui écoutera sur le port donné.
     * @param port Le port réseau sur lequel le serveur écoutera.
     */
    Server(int port);

    /**
     * @brief Destructeur. Assure l'arrêt propre du serveur.
     */
    ~Server();

    /**
     * @brief Démarre le serveur.
     * Met en place le socket d'écoute et lance le thread d'acceptation des connexions.
     * @return Vrai si le démarrage a réussi, faux sinon.
     */
    bool start();

    /**
     * @brief Arrête le serveur de manière propre et ordonnée.
     * Ferme le socket d'écoute et attend que tous les threads clients se terminent.
     */
    void stop();

private:
    /**
     * @brief Initialise la bibliothèque Winsock (spécifique à Windows).
     */
    void initializeWinsock();

    /**
     * @brief Fonction exécutée dans le thread principal du serveur pour accepter les nouvelles connexions.
     * Boucle tant que le serveur est en cours d'exécution.
     */
    void acceptConnections();

    /**
     * @brief Fonction exécutée dans un thread dédié pour chaque client.
     * Gère la réception des messages d'un client et sa déconnexion.
     * @param clientSocket Le socket du client à gérer.
     */
    void handleClient(socket_t clientSocket);

    /**
     * @brief Diffuse un message à tous les clients connectés.
     * @param message Le message à diffuser.
     */
    void broadcastMessage(const std::string& message);

    /**
     * @brief Diffuse un message à tous les clients connectés, sauf à l'expéditeur.
     * @param message Le message à diffuser.
     * @param senderSocket Le socket de l'expéditeur à exclure.
     */
    void broadcastMessage(const std::string& message, socket_t senderSocket);
    
    /**
     * @brief Worker non thread-safe pour la diffusion. Doit être appelé avec le mutex déjà pris.
     */
    void _broadcastMessage_unsafe(const std::string& message, socket_t senderSocket);


    /**
     * @brief Met à jour et diffuse la liste des utilisateurs à tous les clients.
     */
    void broadcastUserList();

    /**
     * @brief Supprime un client de la liste et ferme son socket.
     * @param clientSocket Le socket du client à supprimer.
     * @return Le pseudonyme du client qui a été supprimé.
     */
    std::string removeClient(socket_t clientSocket);

    /**
     * @brief Nettoie les ressources réseau (sockets).
     */
    void cleanupSockets();

    // --- Configuration et état du serveur ---
    int m_port;
    socket_t m_listenSocket;
    std::atomic<bool> m_isRunning;

    // --- Gestion des threads ---
    std::thread m_acceptThread;
    std::vector<std::thread> m_clientThreads;
    std::mutex m_clientThreadsMutex;

    // --- Gestion des données clients ---
    std::map<socket_t, std::string> m_clients;
    std::mutex m_clientsMutex;

    // --- Utilitaires ---
    MessageHandler m_messageHandler;
};