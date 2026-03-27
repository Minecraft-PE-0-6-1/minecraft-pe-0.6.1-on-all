#pragma once

//package net.minecraft.client.renderer.entity;

#include "EntityRenderer.h"

class ItemSpriteRenderer: public EntityRenderer
{
public:
    ItemSpriteRenderer(int icon);

	void render(Entity* e, float x, float y, float z, float rot, float a);
private:
	int icon;
};

