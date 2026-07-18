#include "Command.hpp"
#include "world/level/Level.h"
#include <client/Minecraft.h>

bool Command::isPlayerOp(Minecraft& mc, Player& player) {
    std::string nicknameLower = player.name;
    std::transform(nicknameLower.begin(), nicknameLower.end(), nicknameLower.begin(), ::tolower);

    return mc.level->ops.find(nicknameLower) != mc.level->ops.end();
}