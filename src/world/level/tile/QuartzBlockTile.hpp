#pragma once

//package net.minecraft.world.level.tile;

#include "util/Random.hpp"
#include "world/Facing.hpp"
#include "world/level/material/Material.hpp"
#include "world/level/Level.hpp"

#include "Tile.hpp"

class QuartzBlockTile: public Tile
{
	typedef Tile super;
public:

	static const int TYPE_DEFAULT = 0;
	static const int TYPE_CHISELED  = 1;
	static const int TYPE_LINES  = 2;

	QuartzBlockTile(int id)
	:	super(id, 4 + 13 * 16, Material::stone)
	{}

	int getTexture(int face, int data) {
		if (face == Facing::UP || face == Facing::DOWN) {
			if (data == TYPE_CHISELED)
			{
				return 6 + 12 * 16;
			}
			if (data == TYPE_LINES)
			{
				return 5 + 12 * 16;
			}
			if (face == Facing::DOWN) {
				return 3 + 13 * 16;
			}
			return 4 + 12 * 16;
		}
		if (data == TYPE_CHISELED)
		{
			return 6 + 13 * 16;
		}
		if (data == TYPE_LINES)
		{
			return 5 + 13 * 16;
		}
		return tex;
	}

	static int getTileDataForItemAuxValue(int auxValue) {
		return (auxValue & 0xf);
	}

protected:

	int getSpawnResourcesAuxValue(int data) {
		return data;
	}

};

