#pragma once

//package net.minecraft.world.level.tile;

#include "util/Random.hpp"
#include "world/level/material/Material.hpp"
#include "world/level/Level.hpp"
#include "world/level/LevelSource.hpp"

#include "Tile.hpp"

class GrassTile: public Tile
{
	typedef Tile super;
public:
	static const int MIN_BRIGHTNESS = 4;

	GrassTile(int id);

	int getTexture(LevelSource* level, int x, int y, int z, int face);
	int getTexture(int face, int data);
    int getColor(LevelSource* level, int x, int y, int z);

    void tick(Level* level, int x, int y, int z, Random* random);
    int getResource(int data, Random* random);
};

