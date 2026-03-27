#pragma once
#include "Tile.hpp"
class MelonTile : public Tile {
	typedef Tile super;
public:
	MelonTile(int id);
	int getTexture(LevelSource* level, int x, int y, int z, int face);
	int getTexture(int face);
	int getResource(int data, Random* random);
	int getResourceCount(Random* random);
private:
	static const int TEX = 8 + 8 * 16;
	static const int TEX_TOP = 9 + 8 * 16;
};

