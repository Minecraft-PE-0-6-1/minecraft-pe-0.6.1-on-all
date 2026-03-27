#pragma once

//package net.minecraft.world.level.biome;

#include "Biome.hpp"
#include "world/level/levelgen/feature/TreeFeature.hpp"
#include "world/level/levelgen/feature/BirchFeature.hpp"

class ForestBiome: public Biome
{
public:
    Feature* getTreeFeature(Random* random) {
        if (random->nextInt(5) == 0) {
            return new BirchFeature();
        }
        if (random->nextInt(3) == 0) {
            //return new BasicTree();
        }
        return new TreeFeature(false);
    }
};

