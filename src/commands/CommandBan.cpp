#include "CommandBan.hpp"
#include "commands/Command.hpp"
#include "network/RakNetInstance.h"
#include "raknet/RakPeerInterface.h"
#include "world/level/Level.h"
#include <algorithm>
#include <client/Minecraft.h>

CommandBan::CommandBan() : Command("ban") {}

std::string CommandBan::execute(Minecraft& mc, Player& player, const std::vector<std::string>& args) {
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

    if (*it == (Player*)mc.player) {
        return "ban: you can't ban urself lol";
    }
    
    if (it != mc.level->players.end()) {
        mc.level->removePlayer(*it);
        (*it)->remove();
        mc.raknetInstance->getPeer()->CloseConnection((*it)->owner, true);
    } else {
        for (auto& banned : mc.level->bannedPlayers) {
            if (nicknameLower == banned) {
                return args[0] + "already banned!";
            }
        }
    }

    mc.level->bannedPlayers.insert(nicknameLower);
    return "ban: successfully banned player " + args[0];
}

std::string CommandBan::help(Minecraft& mc) {
    return "Usage: /ban <player>";
}