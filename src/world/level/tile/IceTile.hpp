#pragma once

//package net.minecraft.world.level.tile;

#include "Tile.hpp"
#include "world/level/material/Material.hpp"
#include "world/level/LightLayer.hpp"
#include "util/Random.hpp"

class IceTile: public Tile
{
	typedef Tile super;
public:
    IceTile(int id, int tex)
	:	super(id, tex, Material::ice)
	{
        friction = 0.98f;
        setTicking(true);
    }

    void playerDestroy(Level* level, Player* player, int x, int y, int z, int data) {
        const Material* below = level->getMaterial(x, y - 1, z);
        if (below->blocksMotion() || below->isLiquid()) {
            level->setTile(x, y, z, Tile::water->id);
        }
    }

    int getResourceCount(Random* random) {
        return 0;
    }

    void tick(Level* level, int x, int y, int z, Random* random) {
        if (level->getBrightness(LightLayer::Block, x, y, z) > 11 - Tile::lightBlock[id]) {
            this->spawnResources(level, x, y, z, level->getData(x, y, z));
            level->setTile(x, y, z, Tile::calmWater->id);
        }
    }
};

