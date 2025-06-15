#pragma once
#include <string>
#include <vector>
#include <QMetaType>

enum class Command {
    MSG,
    JOIN,
    PART,
    UNKNOWN,
    USERLIST
};

struct ParsedMessage {
    Command command = Command::UNKNOWN;
    std::vector<std::string> params;
};

Q_DECLARE_METATYPE(ParsedMessage);