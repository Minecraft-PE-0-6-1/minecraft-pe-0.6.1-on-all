#pragma once

//package net.minecraft.world.entity.monster;

#include "Monster.hpp"
#include <string>

class Entity;

class Creeper: public Monster
{
	typedef Monster super;
public:
    Creeper(Level* level);

    /*@Override*/
    int getMaxHealth();

    void tick();

    float getSwelling(float a);

	virtual int getEntityTypeId() const;
protected:
    int getDeathLoot();

	void checkCantSeeTarget(Entity* target, float d);
	void checkHurtTarget(Entity* target, float d);

	std::string getHurtSound();
	std::string getDeathSound();
private:
    int getSwellDir();
    void setSwellDir(int dir);

	int swell;
	int oldSwell;

	int swellDir;

	static const int DATA_SWELL_DIR = 16;
	static const int MAX_SWELL = 30;
};

