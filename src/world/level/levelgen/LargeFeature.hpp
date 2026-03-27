#pragma once

//package net.minecraft.world.level.levelgen;

#include "util/Random.hpp"

class Random;
class Level;
class ChunkSource;

class LargeFeature
{
public:
	LargeFeature();
	virtual ~LargeFeature();

    virtual void apply(ChunkSource* chunkSource, Level* level, int xOffs, int zOffs, unsigned char* blocks, int blocksSize);

protected:
    virtual void addFeature(Level* level, int x, int z, int xOffs, int zOffs, unsigned char* blocks, int blocksSize) = 0;

	int radius;
    Random random;
};

