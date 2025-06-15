#pragma once
#include <string>
#include <vector>

enum class Command {
    MSG,
    JOIN,
    PART,
    UNKNOWN
};

struct ParsedMessage {
    Command command = Command::UNKNOWN;
    std::vector<std::string> params;
};