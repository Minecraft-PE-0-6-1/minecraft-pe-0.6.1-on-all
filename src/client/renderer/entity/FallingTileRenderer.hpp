#pragma once

//package net.minecraft.client.renderer.entity;

#include "EntityRenderer.hpp"

class TileRenderer;

class FallingTileRenderer: public EntityRenderer
{
    typedef EntityRenderer super;
public:
    FallingTileRenderer();
    ~FallingTileRenderer();

    void render(Entity* e, float x, float y, float z, float rot, float a);
private:
    TileRenderer* tileRenderer;
};

