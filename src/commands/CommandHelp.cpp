#include "CommandHelp.hpp"
#include "commands/Command.hpp"
#include "CommandManager.hpp"
#include <client/Minecraft.h>

CommandHelp::CommandHelp() : Command("help") {}

void CommandHelp::execute(Minecraft& mc, Player& player, const std::vector<std::string>& args) {
    if (args.empty()) {
        auto cmds = mc.commandManager().getListAllCommands();

        mc.addMessage("Usage: /help <command>");
        mc.addMessage("List of all commands:");
        
        for (auto& cmd : cmds) {
            mc.addMessage(" - " + cmd);
        }
    } else {
        Command* cmd = mc.commandManager().getCommand(args[0]);

        if (cmd != nullptr) {
            cmd->printHelp(mc);
        }
    }
}

void CommandHelp::printHelp(Minecraft& mc) {}