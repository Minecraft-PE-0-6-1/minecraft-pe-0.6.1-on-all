#include "CommandOp.hpp"
#include "commands/Command.hpp"
#include "network/RakNetInstance.h"
#include "raknet/RakPeer.h"
#include "world/level/Level.h"
#include <algorithm>
#include <client/Minecraft.h>


CommandOp::CommandOp() : Command("op") {}

void CommandOp::execute(Minecraft& mc, Player& player, const std::vector<std::string>& args) {
    if (!isPlayerOp(mc, player)) {
        return mc.addMessage("You aren't enough priveleged to run this command");
    }
    
    if (args.empty()) {
        return printHelp(mc);
    }

    auto it = std::find_if(mc.level->players.begin(), mc.level->players.end(), [args] (auto& it) -> bool {
        return it->name == args[0];
    });

    if (mc.level->ops.find(args[0]) != mc.level->ops.end()) {
        return mc.addMessage("op: player " + args[0] + " already opped");
    }

    mc.level->ops.emplace((*it)->name);
    mc.addMessage("op: successfully opped player " + args[0]);
}

void CommandOp::printHelp(Minecraft& mc) {
    mc.addMessage("Usage: /op <player>");
}