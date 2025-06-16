#include <iostream>
#include <string>
#include <thread>
#include <QCoreApplication>
#include <QObject> 
#include "Client.hpp"
#include "ParsedMessage.hpp" 

std::atomic<bool> isAppRunning = true;

/**
 * @brief Point d'entrée pour la version console du client de tchat.

 */
int main(int argc, char *argv[]) {
    // Il faut une instance de QApplication pour que les signaux/slots fonctionnent,
    // même sans interface graphique.
    QCoreApplication app(argc, argv);

    Client client;

    // --- Connexion des signaux du client à des handlers console ---

    // Gère les changements de statut de la connexion
    QObject::connect(&client, &Client::connectionStatusChanged, 
        [](bool isConnected, const QString& message) {
            std::cout << "[STATUS] " << message.toStdString() << std::endl;
            if (!isConnected) {
                isAppRunning = false; // Arrête l'application si déconnecté
            }
        });

    // Gère la réception de nouveaux messages
    QObject::connect(&client, &Client::newMessageReceived,
        [](const ParsedMessage& msg) {
            if (msg.command == Command::MSG && msg.params.size() >= 2) {
                std::cout << msg.params[0] << ": " << msg.params[1] << std::endl;
            } else if (msg.command == Command::JOIN && !msg.params.empty()) {
                std::cout << "--- " << msg.params[0] << " a rejoint le tchat. ---" << std::endl;
            } else if (msg.command == Command::PART && !msg.params.empty()) {
                std::cout << "--- " << msg.params[0] << " a quitte le tchat. ---" << std::endl;
            }
        });
    
    // Gère la mise à jour de la liste des utilisateurs
    QObject::connect(&client, &Client::userListUpdated, 
        [](const QStringList& users) {
            std::cout << "[USERS] En ligne (" << users.count() << "): " << users.join(", ").toStdString() << std::endl;
        });

    // --- Démarrage de la connexion ---
    const std::string SERVER_IP = "127.0.0.1";
    const int SERVER_PORT = 8080;
    
    // La connexion se fait de manière asynchrone.
    QMetaObject::invokeMethod(&client, "connectToServer", Qt::QueuedConnection,
                              Q_ARG(std::string, SERVER_IP),
                              Q_ARG(int, SERVER_PORT));

    // --- Boucle de saisie utilisateur ---
    std::cout << "Tentative de connexion... Entrez votre pseudo une fois connecte." << std::endl;
    
    std::thread inputThread([](Client& c){
        std::string line;
        while (isAppRunning.load()) {
            std::getline(std::cin, line);
            if (!isAppRunning.load()) break;

            if (!line.empty()) {
                // On envoie le message via un appel asynchrone.
                 QMetaObject::invokeMethod(&c, "sendMsg", Qt::QueuedConnection,
                                          Q_ARG(std::string, line));
            }
        }
    }, std::ref(client));

    int exec_result = app.exec();

    // Nettoyage
    isAppRunning = false;
    std::cout << "Fermeture de l'application. Appuyez sur ENTRER pour quitter." << std::endl;
    inputThread.join();

    return exec_result;
}