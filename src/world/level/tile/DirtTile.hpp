#pragma once

//package net.minecraft.world.level.tile;

#include "world/level/material/Material.hpp"

#include "Tile.hpp"

class DirtTile: public Tile
{
public:
    /*protected*/
	DirtTile(int id, int tex)
	:	Tile(id, tex, Material::dirt)
    {
	}
};

