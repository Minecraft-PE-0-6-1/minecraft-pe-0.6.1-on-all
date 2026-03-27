#pragma once

//package net.minecraft.world.entity.item;

#include "world/entity/Entity.hpp"
#include "world/level/Level.hpp"

class CompoundTag;


class PrimedTnt: public Entity
{
	typedef Entity super;
public:
    PrimedTnt(Level* level);
    PrimedTnt(Level* level, float x, float y, float z);

	void tick();
	
	bool isPickable();

	int getEntityTypeId() const;

	float getShadowHeightOffs();
protected:
    void addAdditonalSaveData(CompoundTag* entityTag);
	void readAdditionalSaveData(CompoundTag* tag);
private:
    void explode();

public:
	int life;
};

