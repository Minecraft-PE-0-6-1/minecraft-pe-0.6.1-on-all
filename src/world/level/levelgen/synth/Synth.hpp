#pragma once

//package net.minecraft.world.level.levelgen.synth;

class Synth
{
public:
	virtual ~Synth();

	int getDataSize(int width, int height);

	virtual float getValue(float x, float y) = 0;

    void create(int width, int height, float* result);
};

