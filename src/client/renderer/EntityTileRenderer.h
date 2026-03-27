#pragma once

//package net.minecraft.client.renderer;

#include "../../world/level/tile/entity/ChestTileEntity.h"
#include "tileentity/TileEntityRenderDispatcher.h"

class Tile;

class EntityTileRenderer
{
public:
    static EntityTileRenderer* instance;

    void render(Tile* tile, int data, float brightness);

    ChestTileEntity chest;
};

