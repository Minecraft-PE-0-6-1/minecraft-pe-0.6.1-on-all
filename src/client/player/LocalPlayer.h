#ifndef NET_MINECRAFT_CLIENT_PLAYER__LocalPlayer_H__
#define NET_MINECRAFT_CLIENT_PLAYER__LocalPlayer_H__

//package net.minecraft.client.player;

#include "input/IMoveInput.h"
#include "../../util/SmoothFloat.h"
#include "../../world/entity/player/Player.h"

class MinecraftClient;
class Stat;
class CompoundTag;

class LocalPlayer: public Player
{
	typedef Player super;
public:
	LocalPlayer(MinecraftClient& minecraft, Level* level, const std::string& username, int dimension, bool isCreative);
	~LocalPlayer();

	void _init();
	virtual void reset();

	void tick();
    void move(float xa, float ya, float za);

    void aiStep();
    void updateAi();

	void setKey(int eventKey, bool eventKeyState);
    void releaseAllKeys();

    void addAdditonalSaveData(CompoundTag* entityTag);
    void readAdditionalSaveData(CompoundTag* entityTag);

    void closeContainer();

	void drop(ItemInstance* item, bool randomly);
    void take(Entity* e, int orgCount);

	void startCrafting(int x, int y, int z, int tableSize);
	void startStonecutting(int x, int y, int z);

	void openContainer(ChestTileEntity* container);
	void openFurnace(FurnaceTileEntity* e);

    bool isSneaking();

    void actuallyHurt(int dmg);
    void hurtTo(int newHealth);
	void die(Entity* source);

    void respawn();

    void animateRespawn() {}
	float getFieldOfViewModifier();
	void chat(const std::string& message) {}
    void displayClientMessage(const std::string& messageId);

    void awardStat(Stat* stat, int count) {
        //minecraft->stats.award(stat, count);
        //minecraft->achievementPopup.popup("Achievement get!", stat.name);
    }
	void causeFallDamage( float distance );

	virtual int startSleepInBed(int x, int y, int z);
	virtual void stopSleepInBed(bool forcefulWakeUp, bool updateLevelList, bool saveRespawnPoint);

	void swing();
	virtual void openTextEdit( TileEntity* tileEntity );
	virtual float getWalkingSpeedModifier();
private:
	void calculateFlight(float xa, float ya, float za);
	bool isSolidTile(int x, int y, int z);
	void updateArmorTypeHash();
public:
	IMoveInput* input = nullptr;
	bool autoJumpEnabled = true;
protected:
	MinecraftClient& minecraft;
	int jumpTriggerTime;
	int ascendTriggerTime;
	int descendTriggerTime;
	bool ascending, descending;
private:
    // local player fly
    // -----------------------
    float flyX, flyY, flyZ;

    // smooth camera settings
    SmoothFloat smoothFlyX;
    SmoothFloat smoothFlyY;
    SmoothFloat smoothFlyZ;

	int autoJumpTime;

	int sentInventoryItemId = -1;
	int sentInventoryItemData = -1;

	int armorTypeHash = 0;

	// sprinting
	bool sprinting = false;
	int  sprintDoubleTapTimer = 0;
	bool prevForwardHeld = false;
public:
	void setSprinting(bool sprint) { sprinting = sprint; }
};

#endif /*NET_MINECRAFT_CLIENT_PLAYER__LocalPlayer_H__*/
