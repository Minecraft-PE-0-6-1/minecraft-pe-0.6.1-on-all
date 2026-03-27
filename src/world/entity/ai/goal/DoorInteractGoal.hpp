#pragma once

//package net.minecraft.world.entity.ai.goal;

#include "Goal.hpp"

#include "world/entity/ai/PathNavigation.hpp"
#include "world/entity/monster/Monster.hpp"
#include "world/level/pathfinder/Path.hpp"
#include "world/level/tile/DoorTile.hpp"

class DoorInteractGoal: public Goal
{
public:
    DoorInteractGoal(Monster* mob)
    :   mob(mob)
    {}

    bool canUse() {
        if (!mob->horizontalCollision) return false;
        PathNavigation* pathNav = mob->getNavigation();
        Path* path = pathNav->getPath();
        if (path == NULL || path->isDone() || !pathNav->canOpenDoors)
			return false;

        for (int i = 0; i < Mth::Min(path->getIndex() + 2, path->getSize()); ++i) {
            Node* n = path->get(i);
            doorX = n->x;
            doorY = n->y + 1;
            doorZ = n->z;
            if (mob->distanceToSqr((float)doorX, mob->y, (float)doorZ) > 1.5f * 1.5f) continue;
            doorTile = getDoorTile(doorX, doorY, doorZ);
            if (doorTile == NULL) continue;
            return true;
        }

        doorX = Mth::floor(mob->x);
        doorY = Mth::floor(mob->y + 1);
        doorZ = Mth::floor(mob->z);
        doorTile = getDoorTile(doorX, doorY, doorZ);
        return doorTile != NULL;
    }

    bool canContinueToUse() {
        return !passed;
    }

    void start() {
        passed = false;
        doorOpenDirX = doorX + 0.5f - mob->x;
        doorOpenDirZ = doorZ + 0.5f - mob->z;
    }

    void tick() {
        float newDoorDirX = doorX + 0.5f - mob->x;
        float newDoorDirZ = doorZ + 0.5f - mob->z;
        float dot = doorOpenDirX * newDoorDirX + doorOpenDirZ * newDoorDirZ;
        if (dot < 0) {
            passed = true;
        }
    }
private:
    DoorTile* getDoorTile(int x, int y, int z) {
        int tileId = mob->level->getTile(x, y, z);
        if (tileId != Tile::door_wood->id) return NULL;
        DoorTile* doorTile = (DoorTile*) Tile::tiles[tileId];
        return doorTile;
    }
protected:
    Monster* mob;
    int doorX, doorY, doorZ;
    DoorTile* doorTile;
private:
    bool passed;
    float doorOpenDirX, doorOpenDirZ;
};

