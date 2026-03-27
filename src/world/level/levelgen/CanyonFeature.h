#pragma once

#if 0

//package net.minecraft.world.level.levelgen;

#include "LargeFeature.h"

class CanyonFeature: public LargeFeature {

	/*protected*/
	void addTunnel(int xOffs, int zOffs, unsigned char* blocks, float xCave, float yCave, float zCave, float thickness, float yRot, float xRot, int step, int dist, float yScale);
    /*protected*/
	void addFeature(Level level, int x, int z, int xOffs, int zOffs, char* blocks);
};

#endif
