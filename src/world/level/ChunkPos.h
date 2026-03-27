#pragma once

//package net.minecraft.world.level;

#include "../entity/Entity.h"

class ChunkPos {
public:
    const int x, z;

	ChunkPos()
	:	x(0),
		z(0)
	{}

    ChunkPos(int x_, int z_)
	:	x(x_),
		z(z_)
	{}

    static int hashCode(int x, int z) {
        return (x < 0 ? 0x80000000 : 0) | ((x & 0x7fff) << 16) | (z < 0 ? 0x00008000 : 0) | (z & 0x7fff);
    }

    int hashCode() const {
        return hashCode(x, z);
    }

	bool operator==(const ChunkPos& rhs) const {
		return x == rhs.x && z == rhs.z;
	}

	bool operator<(const ChunkPos& rhs) const {
		return hashCode() < rhs.hashCode();
	}

    float distanceToSqr(const Entity* e) const {
        float xPos = (float) (x * 16 + 8);
        float zPos = (float) (z * 16 + 8);

        float xd = xPos - e->x;
        float zd = zPos - e->z;

        return xd * xd + zd * zd;
    }
};

