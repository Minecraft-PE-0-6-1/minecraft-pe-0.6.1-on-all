#pragma once

//package net.minecraft.world.level.tile;

#include "util/Random.hpp"
#include "world/Facing.hpp"
#include "world/level/material/Material.hpp"
#include "world/level/Level.hpp"

#include "Tile.hpp"
#include "MultiTextureTile.hpp"

class SandStoneTile: public MultiTextureTile
{
	typedef MultiTextureTile super;
public:
	SandStoneTile(int id, const int* textures, int texCount)
	:	super(id, textures, texCount, Material::stone)
	{}

	int getTexture(int face, int data) {
		if (face == Facing::UP || (face == Facing::DOWN && data > 0)) {
			return 11 * 16;//tex - 16;
		}
		if (face == Facing::DOWN) {
			return 13 * 16;//tex + 16;
		}
		return super::getTexture(face, data);
	}

	static const int TYPE_DEFAULT     = 0;
	static const int TYPE_HEIROGLYPHS = 1;
	static const int TYPE_SMOOTHSIDE  = 2;
};

