#pragma once

#include "Item.hpp"
#include "ItemInstance.hpp"
#include "world/level/Level.hpp"
#include "world/entity/player/Player.hpp"
#include "world/entity/item/TripodCamera.hpp"

class CameraItem: public Item
{
	typedef Item super;

public:
    CameraItem(int id)
    :	super(id)
	{
    }

	ItemInstance* use(ItemInstance* itemInstance, Level* level, Player* player) {
		level->addEntity( new TripodCamera(level, player, player->x, player->y, player->z) );
		return itemInstance;
	}
};

