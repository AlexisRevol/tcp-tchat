#include <iostream>
#include "Server.hpp"

int main() {
    const int PORT = 8080;
    Server server(PORT);

    if (server.start()) {
        std::cout << "Serveur demarre avec succes. Appuyez sur Entree pour arreter." << std::endl;
        std::cin.get();
        server.stop();
    } else {
        std::cerr << "Impossible de demarrer le serveur." << std::endl;
    }
    
    std::cout << "Application serveur terminee." << std::endl;
    return 0;
}