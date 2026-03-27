#pragma once

//package net.minecraft.world.item;

#include "DiggerItem.h"
#include <vector>

class Tile;
class Tier;

class ShovelItem: public DiggerItem
{
    typedef DiggerItem super;
public:
    ShovelItem(int id, const Tier& tier);

    bool canDestroySpecial(const Tile* tile) const;
};

