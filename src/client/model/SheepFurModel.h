#pragma once

//package net.minecraft.client.model;

#include "QuadrupedModel.h"

class SheepFurModel: public QuadrupedModel
{
    typedef QuadrupedModel super;

public:
    SheepFurModel();

    /*@Override*/
    void prepareMobModel(Mob* mob, float time, float r, float a);

    /*@Override*/
    void setupAnim(float time, float r, float bob, float yRot, float xRot, float scale);
private:
    float headXRot;
};

