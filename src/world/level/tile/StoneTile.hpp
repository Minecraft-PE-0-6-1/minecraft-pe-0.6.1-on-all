#pragma once

//package net.minecraft.world.level.tile;

#include "util/Random.hpp"
#include "world/level/material/Material.hpp"

#include "Tile.hpp"

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

