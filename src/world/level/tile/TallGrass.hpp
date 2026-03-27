#pragma once
#include "Bush.hpp"
class Pos;
class TallGrass : public Bush {
public:
	typedef Bush super;

public:
	static const int DEAD_SHRUB = 0;
	static const int TALL_GRASS = 1;
	static const int FERN = 3;


	TallGrass(int id, int tex);
	int getTexture(int face, int data);
	int getColor();
	int getColor(int auxData);
	int getColor(LevelSource* level, int x, int y, int z);
	int getResource(int data, Random* random);
	void playerDestroy(Level* level, Player* player, int x, int y, int z, int data);
};

