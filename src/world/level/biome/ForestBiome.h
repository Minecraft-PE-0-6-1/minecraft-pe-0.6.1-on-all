#pragma once

//package net.minecraft.world.level.biome;

#include "Biome.h"
#include "../levelgen/feature/TreeFeature.h"
#include "../levelgen/feature/BirchFeature.h"

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

