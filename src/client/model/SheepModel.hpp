#pragma once

//package net.minecraft.client.model;

#include "QuadrupedModel.hpp"

class SheepModel: public QuadrupedModel
{
    typedef QuadrupedModel super;
public:
	SheepModel();

    /*@Override*/
    void prepareMobModel(Mob* mob, float time, float r, float a);

    /*@Override*/
    void setupAnim(float time, float r, float bob, float yRot, float xRot, float scale);
private:
    float headXRot;
};

