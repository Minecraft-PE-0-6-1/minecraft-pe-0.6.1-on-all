#pragma once

//package net.minecraft.client.renderer.tileentity;

#include "TileEntityRenderer.hpp"

#include "client/gui/Font.hpp"
#include "client/model/SignModel.hpp"

class SignRenderer: public TileEntityRenderer
{
public:
    /*@Override*/
    void render(TileEntity* te, float x, float y, float z, float a);
    void onGraphicsReset();
private:
    SignModel signModel;
};

