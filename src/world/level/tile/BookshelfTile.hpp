#pragma once

//package net.minecraft.world.level.tile;

#include "world/item/Item.hpp"
#include "world/level/material/Material.hpp"

class BookshelfTile: public Tile
{
	typedef Tile super;
public:
    BookshelfTile(int id, int tex)
    :   super(id, tex, Material::wood)
    {
    }

    int getTexture(int face) {
        if (face <= 1) return 4;
        return tex;
    }

    int getResourceCount(Random* random) {
        return 3;
    }

    //@Override
    int getResource(int data, Random* random/*, int playerBonusLevel*/) {
        return Item::book->id;
    }
};

