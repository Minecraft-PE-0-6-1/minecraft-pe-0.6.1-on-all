#pragma once

#include "Item.h"
#include "ItemInstance.h"
#include "../level/Level.h"
#include "../entity/player/Player.h"
#include "../entity/item/TripodCamera.h"

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

