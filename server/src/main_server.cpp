// server/src/main_server.cpp
#include <iostream>
#include <csignal> // Pour gérer les signaux d'arrêt (Ctrl+C)
#include "Server.hpp"

// Pointeur global vers le serveur pour y accéder depuis le gestionnaire de signal
Server* server_ptr = nullptr;

/**
 * @brief Gestionnaire de signal pour un arrêt propre sur Ctrl+C.
 * @param signum Le numéro du signal reçu.
 */
void handle_signal(int signum) {
    if (server_ptr) {
        std::cout << "\nSignal d'arrêt (" << signum << ") reçu. Arrêt du serveur..." << std::endl;
        server_ptr->stop();
    }
}

/**
 * @brief Point d'entrée de l'application serveur.
 *
 * Crée et démarre une instance du serveur. Attend une entrée utilisateur
 * ou un signal (comme Ctrl+C) pour arrêter proprement le serveur.
 * @return 0 en cas de succès, 1 en cas d'échec au démarrage.
 */
int main() {
    const int PORT = 8080;
    Server server(PORT);
    server_ptr = &server;

    // Intercepte le signal SIGINT (Ctrl+C) pour un arrêt propre
    signal(SIGINT, handle_signal);

    if (server.start()) {
        std::cout << "Serveur démarré avec succès. Appuyez sur Entrée ou faites Ctrl+C pour arrêter." << std::endl;
        std::cin.get(); // Attend que l'utilisateur appuie sur Entrée
        server.stop();
    } else {
        std::cerr << "Impossible de démarrer le serveur." << std::endl;
        return 1;
    }

    std::cout << "Application serveur terminée." << std::endl;
    return 0;
}