#pragma once
#include "Item.h"
class Player;
class ItemInstance;
class Level;
class BedItem : public Item{
	typedef Item super;
public:
	BedItem(int id) : super(id) {}
	bool useOn(ItemInstance* itemInstance, Player* player, Level* level, int x, int y, int z, int face, float clickX, float clickY, float clickZ);
};

