#pragma once

//package net.minecraft.world.entity->ai.goal;

#include "DoorInteractGoal.hpp"

#include "world/entity/Entity.hpp"
#include "world/level/tile/LevelEvent.hpp"
#include "world/level/Level.hpp"
#include "world/level/tile/DoorTile.hpp"
#include "SharedConstants.hpp"

class BreakDoorGoal: public DoorInteractGoal
{
    typedef DoorInteractGoal super;
public:
    BreakDoorGoal(Monster* mob)
    :   super(mob)
    {}

    bool canUse() {
        if (!super::canUse()) return false;
        return !doorTile->isOpen(mob->level, doorX, doorY, doorZ);
    }

    void start() {
        super::start();
        breakTime = SharedConstants::TicksPerSecond * 12;
    }

    bool canContinueToUse() {
        float d = mob->distanceToSqr((float)doorX, (float)doorY, (float)doorZ);
        return breakTime >= 0 && !doorTile->isOpen(mob->level, doorX, doorY, doorZ) && d < 2 * 2;
    }

    void tick() {
        super::tick();
          if (mob->random.nextInt(20) == 0) {
              //mob->level->levelEvent(LevelEvent::SOUND_ZOMBIE_WOODEN_DOOR, doorX, doorY, doorZ, 0);
          }

          //LOGI("time: %d\n", breakTime);

         if (--breakTime == 0) {
             /*if (mob->level->difficulty == Difficulty.HARD)*/ {
                 mob->level->setTile(doorX, doorY, doorZ, 0);
                 //mob->level->levelEvent(LevelEvent::SOUND_ZOMBIE_DOOR_CRASH, doorX, doorY, doorZ, 0);
                 //mob->level->levelEvent(LevelEvent::PARTICLES_DESTROY_BLOCK, doorX, doorY, doorZ, doorTile->id);
             }
         }
    }
private:
    int breakTime;
};

