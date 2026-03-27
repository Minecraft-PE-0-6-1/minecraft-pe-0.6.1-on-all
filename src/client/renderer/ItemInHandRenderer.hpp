#pragma once

//package net.minecraft.client.renderer;

#include "TileRenderer.hpp"
#include "client/renderer/RenderChunk.hpp"
#include "world/item/ItemInstance.hpp"

class Minecraft;

typedef struct RenderCall {
	int itemId;
	RenderChunk chunk;
	std::string	texture;
	bool		isFlat;
} RenderCall;


class ItemInHandRenderer
{
public:
    ItemInHandRenderer(Minecraft* mc);

	void tick();

	void render(float a);
    void renderItem(Mob* player, ItemInstance* item);
    void renderScreenEffect(float a);

	void itemPlaced();
	void itemUsed();

	void onGraphicsReset();

private:
	void renderTex(float a, int tex);
    void renderWater(float a);
    void renderFire(float a);

private:
	int lastIconRendered;
	int lastItemRendered;
	int lastSlot;
	ItemInstance item;

	Minecraft* mc;
	//ItemInstance* selectedItem;
	float height;
	float oHeight;
	TileRenderer tileRenderer;

	static const int MaxNumRenderObjects = 512;
	RenderCall renderObjects[MaxNumRenderObjects];
};

