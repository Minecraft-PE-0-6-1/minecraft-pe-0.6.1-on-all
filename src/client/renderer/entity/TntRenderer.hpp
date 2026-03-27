#pragma once

//package net.minecraft.client.renderer.entity;

#include "EntityRenderer.hpp"
#include "client/renderer/TileRenderer.hpp"

class TntRenderer: public EntityRenderer
{
public:
    TntRenderer();
	void render(Entity* tnt_, float x, float y, float z, float rot, float a);

	TileRenderer tileRenderer;
};

