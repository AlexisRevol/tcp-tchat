#include "MessageHandler.hpp"
#include <sstream> 

// --- ENCODAGE (pour le serveur) ---

std::string MessageHandler::encode_chat_message(const std::string& pseudo, const std::string& msg) {
    return "MSG|" + pseudo + "|" + msg;
}

std::string MessageHandler::encode_join_message(const std::string& pseudo) {
    return "JOIN|" + pseudo;
}

std::string MessageHandler::encode_leave_message(const std::string& pseudo) {
    return "PART|" + pseudo;
}

std::string MessageHandler::encode_userlist_message(const std::vector<std::string>& users) {
    std::string message = "USERLIST";
    for (const auto& user : users) {
        message += "|" + user;
    }
    return message;
}

// --- DÉCODAGE (pour le client) ---

ParsedMessage MessageHandler::decode(const std::string& raw_message) {
    ParsedMessage result;
    std::stringstream ss(raw_message);
    std::string segment;
    std::vector<std::string> segments;

    while(std::getline(ss, segment, '|')) {
       segments.push_back(segment);
    }

    if (segments.empty()) {
        result.command = Command::UNKNOWN;
        return result;
    }

    // Décoder la commande
    if (segments[0] == "MSG") result.command = Command::MSG;
    else if (segments[0] == "JOIN") result.command = Command::JOIN;
    else if (segments[0] == "PART") result.command = Command::PART;
    else if (segments[0] == "USERLIST") result.command = Command::USERLIST;
    else result.command = Command::UNKNOWN;
    
    // Copier les paramètres
    if (segments.size() > 1) {
        result.params.assign(segments.begin() + 1, segments.end());
    }

    return result;
}