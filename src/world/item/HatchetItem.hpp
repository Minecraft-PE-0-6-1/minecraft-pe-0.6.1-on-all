#pragma once

//package net.minecraft.world.item;

#include "DiggerItem.hpp"
#include <vector>

class Tile;
class Tier;
class Material;

class HatchetItem: public DiggerItem
{
    typedef DiggerItem super;
public:
    HatchetItem(int id, const Tier& tier);

    //@Override
    float getDestroySpeed(ItemInstance* itemInstance, Tile* tile);
};

