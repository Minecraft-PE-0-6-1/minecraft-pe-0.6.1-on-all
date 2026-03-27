#pragma once

//package net.minecraft.world.level.material;
#include "Material.h"

class GasMaterial: public Material
{
public:
	GasMaterial() {
		replaceable();
	}

	bool isSolid() const {
        return false;
    }

    bool blocksLight() const {
        return false;
    }
    
    bool blocksMotion() const {
        return false;
    }
};

