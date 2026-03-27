#pragma once

//package net.minecraft.world.level.levelgen.feature;

#include "util/Random.hpp"
#include "world/level/Level.hpp"

#include "world/level/tile/Bush.hpp"
#include "Feature.hpp"

class FlowerFeature: public Feature {
public:
	int tile;

    FlowerFeature(int tile) {
        this->tile = tile;
    }

    bool place(Level* level, Random* random, int x, int y, int z) {
        for (int i = 0; i < 64; i++) {
            int x2 = x + random->nextInt(8) - random->nextInt(8);
            int y2 = y + random->nextInt(4) - random->nextInt(4);
            int z2 = z + random->nextInt(8) - random->nextInt(8);
            if (level->isEmptyTile(x2, y2, z2)) {
                if (((Bush*) Tile::tiles[tile])->canSurvive(level, x2, y2, z2)) {
                    level->setTileNoUpdate(x2, y2, z2, tile);
                }
            }
        }
        return true;
    }
};

