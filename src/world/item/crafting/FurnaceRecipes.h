#pragma once

//package net.minecraft.world.item.crafting;

#include "../ItemInstance.h"
#include <map>

class FurnaceRecipes
{
public:
    typedef std::map<int, ItemInstance> Map;

	static void teardownFurnaceRecipes();
    static const FurnaceRecipes* getInstance();

    bool isFurnaceItem(int itemId) const;

	ItemInstance getResult(int itemId) const;

    const Map& getRecipes() const;

private:
    FurnaceRecipes();

	void addFurnaceRecipe(int itemId, const ItemInstance& result);

    static FurnaceRecipes* instance;

	Map recipes;
};

