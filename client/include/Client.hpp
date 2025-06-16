#pragma once

#include <QObject>
#include <QStringList>
#include <string>
#include <thread>
#include <atomic>

#include "ParsedMessage.hpp"
#include "MessageHandler.hpp"

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
    constexpr int INVALID_SOCKET = -1; // Utiliser constexpr pour les constantes compilées
    constexpr int SOCKET_ERROR = -1;
#endif

/**
 * @class Client
 * @brief Gère la connexion réseau, l'envoi et la réception de messages avec le serveur.
 *
 * Cette classe est conçue pour être le "backend" réseau de l'application.
 * Elle s'exécute dans un thread séparé de l'interface utilisateur pour ne pas
 * la bloquer avec des opérations réseau. Elle communique avec l'UI (ex: MainWindow)
 * exclusivement via des signaux et des slots Qt.
 */
class Client : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construit un objet Client.
     * @param parent Le QObject parent, nullptr par défaut.
     */
    explicit Client(QObject *parent = nullptr);

    /**
     * @brief Destructeur. Assure la déconnexion et le nettoyage des ressources.
     */
    ~Client();

public slots:
    /**
     * @brief Tente de se connecter au serveur à l'adresse et au port spécifiés.
     *
     * Lance le thread de réception des messages en cas de succès.
     * Émet le signal connectionStatusChanged() pour notifier le résultat.
     * @param ip L'adresse IP du serveur.
     * @param port Le port du serveur.
     */
    void connectToServer(const std::string& ip, int port);

    /**
     * @brief Envoie un message texte brut au serveur.
     * @param message Le message à envoyer.
     */
    void sendMsg(const std::string& message);

    /**
     * @brief Déclenche la procédure de déconnexion du serveur.
     */
    void disconnectFromServer();

signals:
    /**
     * @brief Émis lorsque l'état de la connexion change (connexion, déconnexion, erreur).
     * @param isConnected Vrai si connecté, faux sinon.
     * @param message Message décrivant le statut.
     */
    void connectionStatusChanged(bool isConnected, const QString& message);

    /**
     * @brief Émis lorsqu'un nouveau message (MSG, JOIN, PART, etc.) est reçu du serveur.
     * @param msg L'objet contenant les données du message parsé.
     */
    void newMessageReceived(const ParsedMessage& msg);

    /**
     * @brief Émis lorsque le serveur envoie une liste mise à jour des utilisateurs.
     * @param users La liste des pseudonymes des utilisateurs connectés.
     */
    void userListUpdated(const QStringList& users);

private:
    /**
     * @brief Initialise la bibliothèque Winsock (pour Windows uniquement).
     */
    void initializeWinsock();

    /**
     * @brief Fonction exécutée dans un thread séparé pour écouter en continu les messages du serveur.
     *
     * Boucle tant que le client est connecté. C'est une fonction bloquante.
     */
    void receiveMessages();

    /**
     * @brief Nettoie les ressources (ferme le socket, joint le thread).
     */
    void cleanup();
    
    /**
     * @brief Gère la logique de traitement d'un message brut reçu du serveur.
     * @param raw_message Le message brut sous forme de chaîne de caractères.
     */
    void processRawMessage(const std::string& raw_message);

    socket_t m_socket;
    std::atomic<bool> m_isConnected; // atomic pour un accès sécurisé entre les threads
    std::thread m_receiveThread;
    MessageHandler m_messageHandler;
};