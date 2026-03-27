#pragma once

//package net.minecraft.world.entity.animal;

#include "world/entity/AgableMob.hpp"
#include "world/entity/Creature.hpp"

class Level;
class Entity;
class CompoundTag;

class Animal:   public AgableMob,
                public Creature
{
	typedef AgableMob super;

public:
    Animal(Level* level);

	//@Override
	bool hurt(Entity* source, int dmg);

    bool canSpawn();

    int getAmbientSoundInterval();

	int getCreatureBaseType() const;

	bool removeWhenFarAway();

protected:
	float getWalkTargetValue(int x, int y, int z);
	Entity* findAttackTarget();
private:
	int inLove;
};

