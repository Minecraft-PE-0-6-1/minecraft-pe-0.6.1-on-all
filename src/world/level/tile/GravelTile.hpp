#pragma once

//package net.minecraft.world.level.tile;

#include "util/Random.hpp"
#include "world/level/material/Material.hpp"

#include "HeavyTile.hpp"

class GravelTile: public HeavyTile
{
    typedef HeavyTile super;
public:
    GravelTile(int type, int tex)
	:	super(type, tex)
	{
	}

    int getResource(int data, Random* random) {
        if (random->nextInt(10) == 0) return Item::flint->id;
        return id;
    }
};

