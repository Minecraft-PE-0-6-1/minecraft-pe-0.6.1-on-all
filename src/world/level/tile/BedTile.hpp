#pragma once

#include "DirectionalTile.hpp"
#include "world/level/material/Material.hpp"

class Pos;
class BedTile : public DirectionalTile
{
	typedef DirectionalTile super;
public:
	static const int HEAD_PIECE_DATA = 0x8;
	static const int OCCUPIED_DATA = 0x4;
	static const int HEAD_DIRECTION_OFFSETS[4][2];

	BedTile(int id);

	bool use(Level* level, int x, int y, int z, Player* player);

	int getTexture(int face, int data);
	int getRenderShape();
	int getRenderLayer();
	bool isCubeShaped();
	bool isSolidRender();

	void updateShape(LevelSource* level, int x, int y, int z);
	void setShape();

	void neighborChanged(Level* level, int x, int y, int z, int type);

	int getResource(int data, Random* random);
	void spawnResources(Level* level, int x, int y, int z, int data, float odds);

	static bool isHeadPiece( int data );
	static bool isOccupied(int data);
	static void setOccupied( Level* level, int x, int y, int z, bool occupied );
	static bool findStandUpPosition( Level* level, int x, int y, int z, int skipCount, Pos& position);
};

