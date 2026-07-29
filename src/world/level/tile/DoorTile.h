#ifndef NET_MINECRAFT_WORLD_LEVEL_TILE__DoorTile_H__
#define NET_MINECRAFT_WORLD_LEVEL_TILE__DoorTile_H__

//package net.minecraft.world.level->tile;

#include "Tile.h"
#include "../material/Material.h"
#include "../../../util/Random.h"

class Item;
class Level;
class LevelSource;
class Player;


class DoorTile: public Tile
{
	typedef Tile super;
public:
	static const int C_DIR_MASK = 3;
	static const int C_OPEN_MASK = 4;
	static const int C_LOWER_DATA_MASK = 7;
	static const int C_IS_UPPER_MASK = 8;
	static const int C_RIGHT_HINGE_MASK = 16;

	DoorTile(int id, const Material* material);
	int getTexture(LevelSource* level, int x, int y, int z, int face) override;

	bool blocksLight();
    bool isSolidRender() override;
    bool isCubeShaped() override;
    int getRenderShape() override;
	int getRenderLayer() override;

	AABB getTileAABB(Level* level, int x, int y, int z) override;
    AABB* getAABB(Level* level, int x, int y, int z) override;

    void updateShape(LevelSource* level, int x, int y, int z) override;
    void setShape(int compositeData);

    void attack(Level* level, int x, int y, int z, Player* player) override;
    bool use(Level* level, int x, int y, int z, Player* player) override;

    void setOpen(Level* level, int x, int y, int z, bool shouldOpen);
	static bool isOpen(LevelSource* level, int x, int y, int z);

	void neighborChanged(Level* level, int x, int y, int z, int type) override;

    int getResource(int data, Random* random) override;

    // override to avoid duplicate drops when upper half is mined directly
    void playerDestroy(Level* level, Player* player, int x, int y, int z, int data) override;

    HitResult clip(Level* level, int xt, int yt, int zt, const Vec3& a, const Vec3& b) override;

    int getDir(LevelSource* level, int x, int y, int z);

    bool mayPlace(Level* level, int x, int y, int z, unsigned char face) override;

	static int getCompositeData(LevelSource* level, int x, int y, int z);
};

#endif /*NET_MINECRAFT_WORLD_LEVEL_TILE__DoorTile_H__*/
