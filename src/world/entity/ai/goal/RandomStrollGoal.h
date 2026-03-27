#pragma once

//package net.minecraft.world.entity.ai.goal;

#include "Goal.h"

#include "../../../../SharedConstants.h"
#include "../../PathfinderMob.h"
#include "../control/Control.h"
#include "../util/RandomPos.h"
#include "../../../phys/Vec3.h"
#include "../util/RandomPos.h"

class RandomStrollGoal: public Goal
{
public:
    RandomStrollGoal(PathfinderMob* mob, float speed)
    :   mob(mob),
        speed(speed)
    {
        setRequiredControlFlags(Control::MoveControlFlag);
    }

    /*@Override*/
    bool canUse() {
        if (mob->getNoActionTime() >= SharedConstants::TicksPerSecond * 5) return false;
        if (mob->random.nextInt(120) != 0) return false;

		Vec3 pos;
        if (!RandomPos::getPos(pos, mob, 10, 7)) return false;
        wantedX = pos.x;
        wantedY = pos.y;
        wantedZ = pos.z;
        return true;
    }

    /*@Override*/
    bool canContinueToUse() {
        return !mob->getNavigation()->isDone();
    }

    /*@Override*/
    void start() {
        mob->getNavigation()->moveTo(wantedX, wantedY, wantedZ, speed);
    }

private:
    PathfinderMob* mob;
    float wantedX, wantedY, wantedZ;
    float speed;
};

