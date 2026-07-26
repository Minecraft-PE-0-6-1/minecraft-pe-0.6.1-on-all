#include "CommandBanIp.hpp"
#include "commands/Command.hpp"
#include "network/RakNetInstance.h"
#include "raknet/RakPeerInterface.h"
#include "world/level/Level.h"
#include <algorithm>
#include <client/Minecraft.h>

CommandBanIp::CommandBanIp() : Command("banip") {}

std::string CommandBanIp::execute(Minecraft& mc, Player& player, const std::vector<std::string>& args) {
    if (!isPlayerOp(mc, player)) {
        return "You aren't enough priveleged to run this command";
    }
    
    if (args.empty()) {
        return help(mc);
    }

    std::string nicknameLower = args[0];
    std::transform(nicknameLower.begin(), nicknameLower.end(), nicknameLower.begin(), ::tolower);

    auto it = std::find_if(mc.level->players.begin(), mc.level->players.end(), [nicknameLower] (auto& it) -> bool {
        std::string lower = it->name;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

        return lower == nicknameLower;
    });

    std::string clientIp;

    if (it != mc.level->players.end()) {
        if (*it == (Player*)mc.player) {
            return "banip: you can't ban urself lol";
        }
        
        RakNet::SystemAddress sysAddress = mc.raknetInstance->getPeer()->GetSystemAddressFromGuid((*it)->owner);

        char ip[32];
        sysAddress.ToString(false, ip);

        clientIp = ip;

        auto sourceId = (*it)->owner;

        (*it)->reallyRemoveIfPlayer = true;
        mc.level->removeEntity((*it));
        mc.raknetInstance->getPeer()->CloseConnection(sourceId, true);
    } else {
        for (auto& banned : mc.level->bannedIps) {
            if (clientIp == banned) {
                return args[0] + " already banned!";
            }
        }

        clientIp = args[0];
    }

    mc.level->bannedIps.insert(clientIp);
    return "banip: successfully banned player by ip " + args[0];
}

std::string CommandBanIp::help(Minecraft& mc) {
    return "Usage: /banip <player>";
}