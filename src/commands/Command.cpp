#include "Command.hpp"
#include "world/level/Level.h"
#include <client/Minecraft.h>

bool Command::isPlayerOp(Minecraft& mc, Player& player) {
    return mc.level->ops.find(player.name) != mc.level->ops.end();
}