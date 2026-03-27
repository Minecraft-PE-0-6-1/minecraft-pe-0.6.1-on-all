#pragma once

//package net.minecraft.world.level.tile;

#include "HalfTransparentTile.h"
#include "../material/Material.h"
#include "../../../util/Random.h"

class GlassTile: public HalfTransparentTile
{
	typedef HalfTransparentTile super;
public:
    GlassTile(int id, int tex, const Material* material, bool allowSame)
    :	super(id, tex, material, allowSame)
	{}

    int getResourceCount(Random* random) {
        return 0;
    }

    int getRenderLayer() {
        return Tile::RENDERLAYER_ALPHATEST;
    }
};

