#pragma once

//package net.minecraft.client.renderer.tileentity;

#include "TileEntityRenderer.h"

#include "../../gui/Font.h"
#include "../../model/SignModel.h"

class SignRenderer: public TileEntityRenderer
{
public:
    /*@Override*/
    void render(TileEntity* te, float x, float y, float z, float a);
    void onGraphicsReset();
private:
    SignModel signModel;
};

