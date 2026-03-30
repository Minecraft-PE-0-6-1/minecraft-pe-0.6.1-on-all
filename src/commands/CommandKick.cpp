#include "CommandKick.hpp"
#include "commands/Command.hpp"
#include "network/RakNetInstance.h"
#include "raknet/RakPeer.h"
#include "world/level/Level.h"
#include <algorithm>
#include <client/Minecraft.h>


CommandKick::CommandKick() : Command("kick") {}

void CommandKick::execute(Minecraft& mc, Player& player, const std::vector<std::string>& args) {
    if (!isPlayerOp(mc, player)) {
        return mc.addMessage("You aren't enough priveleged to run this command");
    }
    
    if (args.empty()) {
        return printHelp(mc);
    }

    auto it = std::find_if(mc.level->players.begin(), mc.level->players.end(), [args] (auto& it) -> bool {
        return it->name == args[0];
    });

    if (it == mc.level->players.end()) {
        return mc.addMessage("kick: can't find player with name " + args[0]);
    }

    if (*it == (Player*)mc.player) {
        return mc.addMessage("kick: you can't kick urself lol");
    }

    mc.level->removePlayer(*it);
    (*it)->remove();
    mc.raknetInstance->getPeer()->CloseConnection((*it)->owner, true);
    mc.addMessage("kick: successfully kicked player " + args[0]);
}

void CommandKick::printHelp(Minecraft& mc) {
    mc.addMessage("Usage: /kick <player>");
}