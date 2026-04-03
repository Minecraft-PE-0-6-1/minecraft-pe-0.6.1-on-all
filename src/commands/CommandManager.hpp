#pragma once
#include <string>
#include <vector>

#include "Command.hpp"

class CommandManager {
public:
    CommandManager();

    std::vector<std::string> getListAllCommands();

    std::string execute(Minecraft& mc, Player& player, const std::string& input);

    Command* getCommand(const std::string& name);

private:
    void registerAllCommands();
    
    std::vector<Command*> m_commands;
};