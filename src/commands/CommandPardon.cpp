#include "CommandPardon.hpp"
#include "commands/Command.hpp"
#include "network/RakNetInstance.h"
#include "raknet/RakPeerInterface.h"
#include "world/level/Level.h"
#include <algorithm>
#include <client/Minecraft.h>

CommandPardon::CommandPardon() : Command("pardon") {}

std::string CommandPardon::execute(Minecraft& mc, Player& player, const std::vector<std::string>& args) {
    if (!isPlayerOp(mc, player)) {
        return "You aren't enough priveleged to run this command";
    }
    
    if (args.empty()) {
        return help(mc);
    }

    std::string nicknameLower = args[0];
    std::transform(nicknameLower.begin(), nicknameLower.end(), nicknameLower.begin(), ::tolower);

    if (mc.level->bannedPpl.find(nicknameLower) != mc.level->bannedPpl.end()) {
        mc.level->bannedPpl.erase(nicknameLower);

        return "pardon: successfully unbanned player " + args[0];
    }

    if (mc.level->bannedIps.find(nicknameLower) != mc.level->bannedIps.end()) {
        mc.level->bannedIps.erase(nicknameLower);
        
        return "pardon: successfully unbanned ip " + args[0];
    }

    return "Player/Ip doesnt banned";
}

std::string CommandPardon::help(Minecraft& mc) {
    return "Usage: /pardon <player>";
}