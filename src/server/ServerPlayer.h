#ifndef ServerPlayer_H__
#define ServerPlayer_H__

#include "../world/entity/player/Player.h"
#include "../world/inventory/BaseContainerMenu.h"

class Minecraft;
class FurnaceTileEntity;
class ItemInstance;
class FillingContainer;
class ChestTileEntity;

class ServerPlayer: public Player,
					public IContainerListener
{
    typedef Player super;
public:
    ServerPlayer(Minecraft* minecraft, Level* level, bool proto);

	~ServerPlayer();

	void aiStep();
	void tick();
	void take(Entity* e, int orgCount);

	void hurtArmor(int dmg);

	void displayClientMessage(const std::string& messageId);

	void openContainer(ChestTileEntity* furnace);
	void openFurnace(FurnaceTileEntity* furnace);
	void closeContainer();
	void doCloseContainer();

	bool hasResource( int id );
	//
	// IContainerListener
	//
	void setContainerData(BaseContainerMenu* menu, int id, int value);
	void slotChanged(BaseContainerMenu* menu, int slot, const ItemInstance& item, bool isResultSlot);
	void refreshContainer(BaseContainerMenu* menu, const std::vector<ItemInstance>& items);

	virtual void stopSleepInBed(bool forcefulWakeUp, bool updateLevelList, bool saveRespawnPoint);

	void completeUsingItem();
	
	void setLastMoveTicks(int lastMoveTicks) { this->lastMoveTicks = lastMoveTicks; }
	int getLastMoveTicks() { return lastMoveTicks; }

	void setTicksInAir(int ticksInAir) { this->ticksInAir = ticksInAir; }
	int getTicksInAir() { return ticksInAir; }

	void setNewProto(bool proto) { isNewProto = proto; }
	bool getProto() { return isNewProto; }

	virtual void causeFallDamage(float distance);
private:
	void nextContainerCounter();
	void setContainerMenu( BaseContainerMenu* menu );

	Minecraft* _mc;
    int _prevHealth;
	int _containerCounter;

	int lastMoveTicks = 0;
	int ticksInAir = 0;

	bool isNewProto = false;
};

#endif /*ServerPlayer_H__*/
