#include "CommandManager.hpp"
#include <algorithm>
#include <string>
#include <vector>
#include "client/Minecraft.h"
#include "commands/Command.hpp"
#include "commands/CommandHelp.hpp"
#include "commands/CommandKick.hpp"
#include "commands/CommandOp.hpp"
#include "commands/CommandBan.hpp"
#include "commands/CommandGimmieItems.hpp"
#include "network/packet/ChatPacket.h"
#include "network/RakNetInstance.h"
#include "world/level/Level.h"

CommandManager::CommandManager() {
    registerAllCommands();
}

void CommandManager::registerAllCommands() {
    m_commands.push_back(new CommandHelp());
    m_commands.push_back(new CommandKick());
    m_commands.push_back(new CommandOp());
    m_commands.push_back(new CommandBan());
    // m_commands.push_back(new CommandGimmieItems());
}

std::vector<std::string> CommandManager::getListAllCommands() {
    std::vector<std::string> ret;

    for (auto& cmd : m_commands) {
        ret.push_back(cmd->getName());
    }

    return ret;
}

std::string CommandManager::execute(Minecraft& mc, Player& player, const std::string& input) {
    std::istringstream ss(input);
    std::string cmd;

    ss >> cmd;

    auto it = std::find_if(m_commands.begin(), m_commands.end(), [cmd](auto& it) -> bool {
        return it->getName() == cmd;
    });

    if (it == m_commands.end()) {
        return "Command /" + cmd + " not found";
    }

    std::vector<std::string> args;

    std::string tok;
    while (ss >> tok) args.push_back(tok);

    return (*it)->execute(mc, player, args);
}

Command* CommandManager::getCommand(const std::string& name) {
    auto it = std::find_if(m_commands.begin(), m_commands.end(), [name](auto& it) -> bool {
        return it->getName() == name;
    });

    if (it == m_commands.end()) {
        return *it;
    }

    return nullptr;
}