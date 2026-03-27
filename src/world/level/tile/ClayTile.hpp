#pragma once

//package net.minecraft.world.level.tile;

#include "util/Random.hpp"
#include "world/level/material/Material.hpp"

#include "Tile.hpp"

class ClayTile: public Tile {
public:
    ClayTile(int id, int tex)
		: Tile(id, tex, Material::clay)
	{}
    
    int getResource(int data, Random* random) {
        return Item::clay->id;
    }

    int getResourceCount(Random* random) {
        return 4;
    }
};

