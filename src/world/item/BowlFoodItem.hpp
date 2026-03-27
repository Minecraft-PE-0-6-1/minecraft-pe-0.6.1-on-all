#pragma once

//package net.minecraft.world.item;

#include "FoodItem.hpp"

class BowlFoodItem: public FoodItem
{
	typedef FoodItem super;
public:
	BowlFoodItem(int id, int nutrition)
	:   super(id, nutrition, false)
	{
		setMaxStackSize(1);
	}

	ItemInstance useTimeDepleted(ItemInstance* instance, Level* level, Player* player) {
		super::useTimeDepleted(instance, level, player);
		*instance = ItemInstance(Item::bowl);
		return *instance;
	}
};

