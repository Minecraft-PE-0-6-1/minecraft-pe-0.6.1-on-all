#pragma once

//package net.minecraft.world.level.material;
#include "Material.hpp"

class WebMaterial: public Material
{
public:
    WebMaterial() {
        notAlwaysDestroyable();
    }

    bool blocksMotion() const {
        return false;
    }
};

