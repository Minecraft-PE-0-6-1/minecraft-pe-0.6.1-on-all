#pragma once

//package net.minecraft.world.level.levelgen.feature;

#include "Feature.hpp"
#include "world/level/Level.hpp"
#include "world/level/tile/Tile.hpp"
#include "world/level/material/Material.hpp"
#include "util/Random.hpp"

class SpringFeature: public Feature
{
    int tile;

public:
    SpringFeature(int tile) {
        this->tile = tile;
    }

    bool place(Level* level, Random* random, int x, int y, int z) {
        if (level->getTile(x, y + 1, z) != Tile::rock->id) return false;
        if (level->getTile(x, y - 1, z) != Tile::rock->id) return false;

        if (level->getTile(x, y, z) != 0 && level->getTile(x, y, z) != Tile::rock->id) return false;

        int rockCount = 0;
        if (level->getTile(x - 1, y, z) == Tile::rock->id) rockCount++;
        if (level->getTile(x + 1, y, z) == Tile::rock->id) rockCount++;
        if (level->getTile(x, y, z - 1) == Tile::rock->id) rockCount++;
        if (level->getTile(x, y, z + 1) == Tile::rock->id) rockCount++;

        int holeCount = 0;
        if (level->isEmptyTile(x - 1, y, z)) holeCount++;
        if (level->isEmptyTile(x + 1, y, z)) holeCount++;
        if (level->isEmptyTile(x, y, z - 1)) holeCount++;
        if (level->isEmptyTile(x, y, z + 1)) holeCount++;

        if (rockCount == 3 && holeCount == 1) {
            level->setTile(x, y, z, tile);
            level->instaTick = true;
            Tile::tiles[tile]->tick(level, x, y, z, random);
            level->instaTick = false;
        }

        return true;
    }
};

