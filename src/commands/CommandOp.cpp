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

    std::string nicknameLower = args[0];
    std::transform(nicknameLower.begin(), nicknameLower.end(), nicknameLower.begin(), ::tolower);
    
    if (mc.level->ops.find(nicknameLower) != mc.level->ops.end()) {
        return "op: player " + args[0] + " already opped";
    }

    mc.level->ops.emplace(nicknameLower);
    return "op: successfully opped player " + args[0];
}

std::string CommandOp::help(Minecraft& mc) {
    return "Usage: /op <player>";
}