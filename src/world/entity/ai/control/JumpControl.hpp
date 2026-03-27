#pragma once

//package net.minecraft.world.entity.ai.control;

#include "Control.hpp"
#include "world/entity/Mob.hpp"

class JumpControl: public Control
{
public:
    JumpControl(Mob* mob)
    :   mob(mob),
        _jump(false)
    {
    }

    void jump() {
        _jump = true;
    }

    void tick() {
        mob->setJumping(_jump);
        _jump = false;
    }
private:
    Mob* mob;
    bool _jump;
};

