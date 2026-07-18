#include "CommandHelp.hpp"
#include "commands/Command.hpp"
#include "CommandManager.hpp"
#include <client/Minecraft.h>
#include <sstream>

CommandHelp::CommandHelp() : Command("help") {}

std::string CommandHelp::execute(Minecraft& mc, Player& player, const std::vector<std::string>& args) {
    if (args.empty()) {
        auto cmds = mc.commandManager().getListAllCommands();

        std::ostringstream output;

        output << "List of all commands:" << std::endl;
        
        for (auto& cmd : cmds) {
            output << " - " + cmd << std::endl;;
        }

        return output.str();
    }

    Command* cmd = mc.commandManager().getCommand(args[0]);

    if (cmd != nullptr) {
        return cmd->help(mc);
    }

    return "help: command " + args[0] + " not found";
}

std::string CommandHelp::help(Minecraft& mc) {
    return "Usage: /help <command>";
}