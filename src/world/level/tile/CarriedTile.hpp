#pragma once

//package net.minecraft.world.level.tile;

#include "util/Random.hpp"
#include "world/level/material/Material.hpp"

#include "Tile.hpp"

class CarriedTile: public Tile
{
    typedef Tile super;
public:
    CarriedTile(int id, int texDefault, int texTop = -1)
	:	super(id, tex, Material::dirt),
		texDefault(texDefault),
		texTop(texTop >= 0? texTop : texDefault)
	{
    }

    int getTexture(int face, int data) {
        if (face == 1) return texTop;
        return texDefault;
    }

    int getResource(int data, Random* random) {
        return 0;
    }

    int getResourceCount(Random* random) {
        return 0;
    }

private:
    int texDefault;
    int texTop;
};

