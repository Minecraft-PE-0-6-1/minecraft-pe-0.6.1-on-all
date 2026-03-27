#pragma once

//package net.minecraft.world.level.tile;

#include "../../../util/Random.h"
#include "../material/Material.h"

#include "Tile.h"

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

