#pragma once

//package net.minecraft.world.level.material;
#include "Material.hpp"

class LiquidMaterial: public Material
{
public:
	LiquidMaterial() {
		replaceable();
	}

	bool isLiquid() const {
        return true;
    }
    
    bool blocksMotion() const {
        return false;
    }    
    
    bool isSolid() const {
        return false;
    }
};

