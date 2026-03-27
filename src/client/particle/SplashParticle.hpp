#pragma once

//package net.minecraft.client.particle;

#include "WaterDropParticle.hpp"
#include "world/level/Level.hpp"

class SplashParticle: public WaterDropParticle
{
	typedef WaterDropParticle super;
public:
    SplashParticle(Level* level, float x, float y, float z, float xa, float ya, float za)
	:	super(level, x, y, z)
	{
        gravity = 0.04f;
        tex += 1;
        if (ya == 0 && (xa != 0 || za != 0)) {
            xd = xa;
            yd = ya + 0.1;
            zd = za;
        }
    }
};

