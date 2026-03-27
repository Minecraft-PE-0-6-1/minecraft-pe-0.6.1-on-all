#pragma once

//package net.minecraft.world.level;

class LightLayer
{
public:
    static const LightLayer Sky; // (15)
	static const LightLayer Block;// (0)

	const int surrounding;

private:
    LightLayer(int surrounding_)
	:	surrounding(surrounding_)
	{}
};

