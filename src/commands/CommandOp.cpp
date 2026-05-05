#include "CommandOp.hpp"
#include "commands/Command.hpp"
#include "world/level/Level.h"
#include <algorithm>
#include <client/Minecraft.h>

CommandOp::CommandOp() : Command("op") {}

std::string CommandOp::execute(Minecraft& mc, Player& player, const std::vector<std::string>& args) {
    if (!isPlayerOp(mc, player)) {
        return "You aren't enough priveleged to run this command";
    }
    
    if (args.empty()) {
        return help(mc);
    }

    auto it = std::find_if(mc.level->players.begin(), mc.level->players.end(), [args] (auto& it) -> bool {
        return it->name == args[0];
    });

    if (mc.level->ops.find(args[0]) != mc.level->ops.end()) {
        return "op: player " + args[0] + " already opped";
    }

    mc.level->ops.emplace((*it)->name);
    return "op: successfully opped player " + args[0];
}

std::string CommandOp::help(Minecraft& mc) {
    return "Usage: /op <player>";
}