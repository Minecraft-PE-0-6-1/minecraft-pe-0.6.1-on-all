#pragma once

//package net.minecraft.world.entity.animal;

#include "world/entity/PathfinderMob.hpp"
#include "world/entity/Creature.hpp"

class Level;
class Player;
class CompoundTag;

/**
 * The purpose of this class is to offer a new base class for MobCategory.
 * {@link MobCategory}
 *
 * Note: Can't extend Animal because then water animals would prevent normal
 * animals to spawn, due to MobCategory.getMaxInstancesPerChunk(). This class is
 * otherwise similar to Animal
 *
 */
class WaterAnimal: public PathfinderMob, public Creature
{
    typedef PathfinderMob super;
public:
    WaterAnimal(Level* level);

    /*@Override*/
    bool isWaterMob();

    void addAdditonalSaveData(CompoundTag* entityTag);
    void readAdditionalSaveData(CompoundTag* tag);

    bool canSpawn();

    int getAmbientSoundInterval();
	int getCreatureBaseType() const;

protected:
    bool removeWhenFarAway();

    /*@Override*/
    int getExperienceReward(Player* killedBy);
};

