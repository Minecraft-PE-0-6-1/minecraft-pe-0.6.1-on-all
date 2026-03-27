#pragma once

//package net.minecraft.world.level.tile;

#include "Tile.hpp"
#include "world/level/material/Material.hpp"

class MetalTile: public Tile
{
public:
	MetalTile(int id, int tex)
	:	Tile(id, Material::metal)
	{
		this->tex = tex;
	}

	int getTexture(int face) {
		return tex;
	}
};

