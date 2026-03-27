#pragma once

//package net.minecraft.client.renderer.tileentity;

#include "TileEntityRenderer.h"
#include "../../model/ChestModel.h"

class ChestTileEntity;

class ChestRenderer: public TileEntityRenderer
{
    /*@Override*/
    void render(TileEntity* chest, float x, float y, float z, float a);

private:
    ChestModel chestModel;
    //LargeChestModel* largeChestModel;
};

