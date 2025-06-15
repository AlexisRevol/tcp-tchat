#pragma once
#include <string>
#include "ParsedMessage.hpp"

class MessageHandler {
public:
    // --- Méthodes pour le SERVEUR (Encoder) ---
    std::string encode_chat_message(const std::string& pseudo, const std::string& msg);
    std::string encode_join_message(const std::string& pseudo);
    std::string encode_leave_message(const std::string& pseudo);
    std::string encode_userlist_message(const std::vector<std::string>& users);

    // --- Méthode pour le CLIENT (Décoder) ---
    ParsedMessage decode(const std::string& raw_message);
};