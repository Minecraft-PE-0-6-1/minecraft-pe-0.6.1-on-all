#pragma once

//package net.minecraft.client.gamemode;

#include <world/level/tile/Tile.hpp>

class ItemInstance;
class Minecraft;
class Level;
class Player;
class Abilities;

class GameMode {
protected:
	Minecraft& minecraft;

public:
    GameMode(Minecraft& minecraft) : minecraft(minecraft) {}
	virtual ~GameMode() {}

    virtual void initLevel(Level* level) {}

    virtual void startDestroyBlock(Player* player, int x, int y, int z, int face);
    virtual bool destroyBlock(Player* player, int x, int y, int z, int face);
    virtual void continueDestroyBlock(Player* player, int x, int y, int z, int face) = 0;
    virtual void stopDestroyBlock(Player* player) {}

    virtual void tick();

    virtual float getPickRange();
    /* void postLevelGen(LevelGen levelGen, Level level) {} */

    virtual bool useItem(Player* player, Level* level, ItemInstance* item);
    virtual bool useItemOn(Player* player, Level* level, ItemInstance* item, int x, int y, int z, int face, const Vec3& hit);

    virtual void initPlayer(Player* player);
    virtual void adjustPlayer(Player* player) {}
    virtual bool canHurtPlayer() { return false; }

	virtual void interact(Player* player, Entity* entity);
    virtual void attack(Player* player, Entity* entity);

    virtual ItemInstance* handleInventoryMouseClick(int containerId, int slotNum, int buttonNum, Player* player);
    virtual void handleCloseInventory(int containerId, Player* player);

	virtual bool isCreativeType() { return false; }
	virtual bool isSurvivalType() { return false; }

	virtual void initAbilities(Abilities& abilities) {}

	virtual void releaseUsingItem(Player* player);

	float oDestroyProgress = 0;
	float destroyProgress = 0;
protected:
	int destroyTicks = 0;
	int destroyDelay = 0;
};

