#pragma once
#include "Item.h"
class HangingEntity;
class HangingEntityItem : public Item {
	typedef Item super;
public:
	HangingEntityItem(int id, int type);
	bool useOn(ItemInstance* itemInstance, Player* player, Level* level, int x, int y, int z, int face, float clickX, float clickY, float clickZ);
private:
	HangingEntity* createEntity(Level* level, int x, int y, int z, int dir);
private:
	int entityType;
};

