#pragma once

//package net.minecraft.world.entity.animal;

#include "Animal.hpp"

class CompoundTag;
class Level;

class Chicken: public Animal
{
    typedef Animal super;
public:
    Chicken(Level* level);

	int getEntityTypeId() const;

    /*@Override*/
    int getMaxHealth();

    void aiStep();

    void addAdditonalSaveData(CompoundTag* tag);
    void readAdditionalSaveData(CompoundTag* tag);
protected:
    void causeFallDamage(float distance);

    const char* getAmbientSound();
    std::string getHurtSound();
    std::string getDeathSound();

    //int getDeathLoot();

    //@Override
    void dropDeathLoot(/*bool wasKilledByPlayer, int playerBonusLevel*/);

    //@Override
    Animal* getBreedOffspring(Animal* target);
public:
	bool sheared;
	float flap;
	float flapSpeed;
	float oFlapSpeed, oFlap;
	float flapping;
	int eggTime;
};

