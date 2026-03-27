#pragma once

//package net.minecraft.world.level;

#include "LightLayer.hpp"
#include "chunk/LevelChunk.hpp"

class Level;

class LightUpdate
{
public:
	const LightLayer* layer;
    int x0, y0, z0;
    int x1, y1, z1;

	//LightUpdate(); // for using std::set
	LightUpdate(const LightUpdate* t);
	LightUpdate(const LightUpdate& t);
	LightUpdate(const LightLayer& _layer, int _x0, int _y0, int _z0, int _x1, int _y1, int _z1);

	void operator=(const LightUpdate* t);

    void update(Level* level);

    bool expandToContain(int _x0, int _y0, int _z0, int _x1, int _y1, int _z1);

};

