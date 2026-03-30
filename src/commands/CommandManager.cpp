#include "CommandManager.hpp"
#include <algorithm>
#include <string>
#include <vector>
#include "client/Minecraft.h"
#include "commands/Command.hpp"
#include "commands/CommandHelp.hpp"
#include "commands/CommandKick.hpp"
#include "network/packet/ChatPacket.h"
#include "network/RakNetInstance.h"
#include "world/level/Level.h"



CommandManager::CommandManager() {
    registerAllCommands();
}

void CommandManager::registerAllCommands() {
    m_commands.push_back(new CommandHelp());
    m_commands.push_back(new CommandKick());
}

std::vector<std::string> CommandManager::getListAllCommands() {
    std::vector<std::string> ret;

    for (auto& cmd : m_commands) {
        ret.push_back(cmd->getName());
    }

    return ret;
}

void CommandManager::execute(Minecraft& mc, const std::string& input) {
    std::istringstream ss(input);
    std::string cmd;

    ss >> cmd;

    auto it = std::find_if(m_commands.begin(), m_commands.end(), [cmd](auto& it) -> bool {
        return it->getName() == cmd;
    });

    if (it == m_commands.end()) {
        return mc.addMessage("Command /" + cmd + " not found");
    }

    std::vector<std::string> args;

    std::string tok;
    while (ss >> tok) args.push_back(tok);
    
    if (!mc.level->isClientSide || (*it)->getFlags() & CommandFlags::COMMAND_FLAG_SINGLEPLAYER_ONLY) {
        (*it)->execute(mc, args);
    } else {
        ChatPacket packet("/" + input);
        mc.raknetInstance->send(packet);
    }
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