#include <iostream>
#include "Server.hpp"

int main() {
    const int PORT = 8080;

    Server server(PORT);

    if (server.start()) {
        std::cout << "Serveur démarré avec succès. Appuyez sur Entrée pour arrêter." << std::endl;
        std::cin.get();
    } else {
        std::cerr << "Impossible de démarrer le serveur." << std::endl;
    }

    return 0;
}