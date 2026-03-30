#pragma once
#include <string>
#include <vector>

enum CommandFlags {
    COMMAND_FLAG_SINGLEPLAYER_ONLY = (1 << 1),
    COMMAND_FLAG_NO_ARGS = (1 << 2),
};

class Minecraft;

class Command {
public:
    const std::string& getName() { return m_name; }
    const CommandFlags getFlags() { return m_flags; }

    virtual void execute(Minecraft& mc, const std::vector<std::string>& args) = 0;
    virtual void printHelp(Minecraft& mc) = 0;

protected:
    Command(const std::string& name, CommandFlags flags = (CommandFlags)0) : m_name(name), m_flags(flags) {}

    const std::string m_name;
    const CommandFlags m_flags;
};