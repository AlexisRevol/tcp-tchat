// client/src/main_client.cpp
#include <iostream>
#include "Client.hpp"

int main() {
    const std::string SERVER_IP = "127.0.0.1";
    const int SERVER_PORT = 8080;

    Client client;

    if (client.connectToServer(SERVER_IP, SERVER_PORT)) {
        // Boucle du client pour envoyer des messages
        client.run();
    } else {
        std::cerr << "Impossible de se connecter au serveur." << std::endl;
    }

    return 0;
}