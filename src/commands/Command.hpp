#pragma once
#include <string>
#include <vector>

enum CommandFlags {
    COMMAND_FLAG_SINGLEPLAYER_ONLY = (1 << 1),
    COMMAND_FLAG_NO_ARGS = (1 << 2),
};

class Minecraft;
class Player;

class Command {
public:
    const std::string& getName() { return m_name; }
    const CommandFlags getFlags() { return m_flags; }

    bool isPlayerOp(Minecraft& mc, Player& player);

    virtual std::string execute(Minecraft& mc, Player& player, const std::vector<std::string>& args) = 0;
    virtual std::string help(Minecraft& mc) = 0;

protected:
    Command(const std::string& name, CommandFlags flags = (CommandFlags)0) : m_name(name), m_flags(flags) {}

    const std::string m_name;
    const CommandFlags m_flags;
};