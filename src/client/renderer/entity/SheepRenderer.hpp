#pragma once

//package net.minecraft.client.renderer.entity;

#include "MobRenderer.hpp"
class Mob;

class SheepRenderer: public MobRenderer
{
    typedef MobRenderer super;

public:
    SheepRenderer(Model* model, Model* armor, float shadow);
	~SheepRenderer();
protected:
	int prepareArmor(Mob* sheep, int layer, float a);
};

