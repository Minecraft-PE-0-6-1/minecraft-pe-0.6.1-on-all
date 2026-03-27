#pragma once

//package net.minecraft.world.level.biome;

#include "Biome.hpp"
#include "util/Random.hpp"
#include "world/level/levelgen/feature/PineFeature.hpp"
#include "world/level/levelgen/feature/SpruceFeature.hpp"

class TaigaBiome: public Biome
{
public:
    Feature* getTreeFeature(Random* random) {
        if (random->nextInt(3) == 0) {
            return new PineFeature();
        }
        return new SpruceFeature();
    }
};

