#pragma once

//package net.minecraft.world.level.tile;

#include "../../../util/Random.h"
#include "../material/Material.h"

#include "Tile.h"

class StoneTile: public Tile
{
public:
    StoneTile(int id, int tex)
	:	Tile(id, tex, Material::stone)
	{
    }

    int getResource(int data, Random* random) {
        return Tile::stoneBrick->id;
    }
};

