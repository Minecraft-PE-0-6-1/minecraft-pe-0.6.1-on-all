#pragma once

//package net.minecraft.world.level.tile;

#include "world/entity/Entity.hpp"
#include "world/item/Item.hpp"
#include "world/level/Level.hpp"
#include "world/level/material/Material.hpp"
#include "world/phys/AABB.hpp"

class WebTile: public Tile
{
    typedef Tile super;
public:
    WebTile(int id, int tex)
	:	super(id, tex, Material::web)
	{
    }

	int getRenderLayer(){
		return RENDERLAYER_ALPHATEST;
	}

    /*@Override*/
    void entityInside(Level* level, int x, int y, int z, Entity* entity) {
        entity->makeStuckInWeb();
    }

    /*@Override*/
    bool isSolidRender() {
        return false;
    }

    /*@Override*/
    AABB* getAABB(Level* level, int x, int y, int z) {
        return NULL;
    }

    /*@Override*/
    int getRenderShape() {
        return Tile::SHAPE_CROSS_TEXTURE;
    }

    bool blocksLight() {
        return false;
    }

    bool isCubeShaped() {
        return false;
    }

    /*@Override*/
    int getResource(int data, Random* random) {
        return Item::string->id;
    }
};

