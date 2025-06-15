#pragma once
#include <string>

class MessageHandler {
public:
    std::string format_join_message(const std::string& pseudo);
    std::string format_chat_message(const std::string& pseudo, const std::string& msg);
    std::string format_leave_message(const std::string& pseudo);
};