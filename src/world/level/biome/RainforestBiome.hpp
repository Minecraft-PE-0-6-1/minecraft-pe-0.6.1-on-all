#pragma once

//package net.minecraft.world.level.biome;

#include "Biome.hpp"
#include "util/Random.hpp"
#include "world/level/levelgen/feature/TreeFeature.hpp"

class RainforestBiome: public Biome
{
public:
    Feature* getTreeFeature(Random* random) {
        if (random->nextInt(3) == 0) {
            //return new BasicTree();
        }
        return new TreeFeature(false);
    }
};

