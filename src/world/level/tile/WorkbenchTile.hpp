#pragma once

//package net.minecraft.world.level.tile;

#include "Tile.hpp"
#include "world/level/Level.hpp"
#include "world/level/material/Material.hpp"
#include "world/entity/player/Player.hpp"

class WorkbenchTile: public Tile
{
	typedef Tile super;
public:
    WorkbenchTile(int id)
	:	super(id, Material::wood)
	{
        tex = 11 + 16 * 3;
    }

    int getTexture(int face) {
        if (face == 1) return tex - 16;
        if (face == 0) return Tile::wood->getTexture(0);
        if (face == 2 || face == 4) return tex + 1;
        return tex;
    }

	bool use(Level* level, int x, int y, int z, Player* player) {
		player->startCrafting(x, y, z, Recipe::SIZE_3X3);
        return true;
    }
};

