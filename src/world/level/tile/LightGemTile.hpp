#pragma once
#include "Tile.hpp"
class LightGemTile : public Tile {
	typedef Tile super;
public:
	LightGemTile(int id, int tex, const Material* material);
	int getResourceCount(Random* random);
	int getResource(int data, Random* random);
};

