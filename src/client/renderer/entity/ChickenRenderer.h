#pragma once

//package net.minecraft.client.renderer.entity;

#include "MobRenderer.h"

class Mob;

class ChickenRenderer: public MobRenderer
{
    typedef MobRenderer super;
public:
    ChickenRenderer(Model* model, float shadow);

    void render(Entity* mob, float x, float y, float z, float rot, float a);

protected:
    float getBob(Mob* mob_, float a);
};

