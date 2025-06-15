#include "MessageHandler.hpp"

std::string MessageHandler::format_join_message(const std::string& pseudo) {
    return pseudo + " a rejoint le tchat.";
}

std::string MessageHandler::format_chat_message(const std::string& pseudo, const std::string& msg) {
    return pseudo + ": " + msg;
}

std::string MessageHandler::format_leave_message(const std::string& pseudo) {
    return pseudo + " a quitte le tchat.";
}