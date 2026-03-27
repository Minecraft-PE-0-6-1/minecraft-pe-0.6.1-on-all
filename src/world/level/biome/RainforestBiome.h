#pragma once

//package net.minecraft.world.level.biome;

#include "Biome.h"
#include "../../../util/Random.h"
#include "../levelgen/feature/TreeFeature.h"

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

