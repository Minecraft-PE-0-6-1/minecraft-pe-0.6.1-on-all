#include "CommandKick.hpp"
#include "commands/Command.hpp"
#include "network/RakNetInstance.h"
#include "raknet/RakPeerInterface.h"
#include "world/level/Level.h"
#include <algorithm>
#include <client/Minecraft.h>

CommandKick::CommandKick() : Command("kick") {}

std::string CommandKick::execute(Minecraft& mc, Player& player, const std::vector<std::string>& args) {
    if (!isPlayerOp(mc, player)) {
        return "You aren't enough priveleged to run this command";
    }
    
    if (args.empty()) {
        return help(mc);
    }

    std::string nicknameLower = args[0];
    std::transform(nicknameLower.begin(), nicknameLower.end(), nicknameLower.begin(), ::tolower);

    auto it = std::find_if(mc.level->players.begin(), mc.level->players.end(), [args, nicknameLower] (auto& it) -> bool {
        std::string lower = it->name;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

        return lower == nicknameLower;
    });

    if (it == mc.level->players.end()) {
        return "kick: can't find player with name " + args[0];
    }

    if (*it == (Player*)mc.player) {
        return "kick: you can't kick urself lol";
    }

    auto sourceId = (*it)->owner;

    (*it)->reallyRemoveIfPlayer = true;
    mc.level->removeEntity((*it));
    mc.raknetInstance->getPeer()->CloseConnection(sourceId, true);
    return "kick: successfully kicked player " + args[0];
}

std::string CommandKick::help(Minecraft& mc) {
    return "Usage: /kick <player>";
}