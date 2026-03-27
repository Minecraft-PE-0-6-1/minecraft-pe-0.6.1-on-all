#pragma once

//package net.minecraft.world.item;

#include "Item.hpp"

#include "world/entity/player/Player.hpp"
#include "world/entity/projectile/ThrownEgg.hpp"
#include "world/level/Level.hpp"

class EggItem: public Item
{
    typedef Item super;
public:
    EggItem(int id)
	:	super(id)
	{
        maxStackSize = 16;
    }

    ItemInstance* use(ItemInstance* instance, Level* level, Player* player) {
        if (!player->abilities.instabuild)
            instance->count--;

		level->playSound(player, "random.bow", 0.5f, 0.4f / (random.nextFloat() * 0.4f + 0.8f));
        if (!level->isClientSide) level->addEntity(new ThrownEgg(level, player));
        return instance;
    }
};

