#pragma once

//package net.minecraft.world.level.tile;

#include "../material/Material.h"

#include "Tile.h"

class DirtTile: public Tile
{
public:
    /*protected*/
	DirtTile(int id, int tex)
	:	Tile(id, tex, Material::dirt)
    {
	}
};

