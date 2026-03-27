#pragma once

//package net.minecraft.world.entity;

#include "Mob.hpp"

class Level;


class FlyingMob: public Mob
{
    typedef Mob super;
public:
    FlyingMob(Level* level);

    void travel(float xa, float ya);

    bool onLadder();
protected:
    void causeFallDamage(float distance);
};

