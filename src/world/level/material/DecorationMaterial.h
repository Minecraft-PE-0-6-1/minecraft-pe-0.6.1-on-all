#pragma once

//package net.minecraft.world.level.material;
#include "Material.h"

class DecorationMaterial: public Material
{
public:
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

