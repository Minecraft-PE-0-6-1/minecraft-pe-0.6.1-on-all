#pragma once

//package net.minecraft.world.level.tile;

#include "world/level/LevelSource.hpp"
#include "world/level/material/Material.hpp"

class HalfTransparentTile: public Tile {
    bool allowSame;

public:
    /*protected*/
	HalfTransparentTile(int id, int tex, const Material* material, bool allowSame)
	:	Tile(id, tex, material)
	{
        this->allowSame = allowSame;
    }

	int getRenderLayer() {
        return Tile::RENDERLAYER_BLEND;
    }

    bool isSolidRender() {
        return false;
    }

    bool shouldRenderFace(LevelSource* level, int x, int y, int z, int face) {
        int id = level->getTile(x, y, z);
        if (!allowSame && id == this->id) return false;
        return Tile::shouldRenderFace(level, x, y, z, face);
    }

    bool blocksLight() {
        return false;
    }
};

