#include "CommandGimmieItems.hpp"
#include <client/Minecraft.h>
#include <world/level/Level.h>
#include <world/item/DyePowderItem.h>

CommandGimmieItems::CommandGimmieItems() : Command("gimmieitems") {}

std::string CommandGimmieItems::execute(Minecraft& mc, Player& player, const std::vector<std::string>& args) {
    if (mc.level != nullptr) {
        if (mc.level->getLevelData()->getAllowCheats()) {
            player.inventory->add(new ItemInstance(Item::ironIngot, 64));
            player.inventory->add(new ItemInstance(Item::ironIngot, 34));
            player.inventory->add(new ItemInstance(Tile::stonecutterBench));
            player.inventory->add(new ItemInstance(Tile::workBench));
            player.inventory->add(new ItemInstance(Tile::furnace));
            player.inventory->add(new ItemInstance(Tile::wood, 54));
            player.inventory->add(new ItemInstance(Item::stick, 14));
            player.inventory->add(new ItemInstance(Item::coal, 31));
            player.inventory->add(new ItemInstance(Tile::sand, 6));
            player.inventory->add(new ItemInstance(Item::dye_powder, 23, DyePowderItem::PURPLE));
            
            return "Grant debug items. Have fun!";
        } else {
            return "Cheats are not allowed!";
        }
    }

    return "World not loaded";
}

std::string CommandGimmieItems::help(Minecraft& mc) {
    return "Usage: /gimmieitems";
}