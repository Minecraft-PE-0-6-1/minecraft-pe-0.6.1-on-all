#pragma once

//package net.minecraft.world.level.tile;

#include "util/Random.hpp"
#include "world/level/material/Material.hpp"

#include "Tile.hpp"
#include "world/item/DyePowderItem.hpp"

class OreTile: public Tile
{
public:
    OreTile(int id, int tex)
	:	Tile(id, tex, Material::stone)
	{
    }

    int getResource(int data, Random* random) {
        if (id == Tile::coalOre->id) return Item::coal->id;
        if (id == Tile::emeraldOre->id) return Item::emerald->id;
        if (id == Tile::lapisOre->id) return Item::dye_powder->id;
        return id;
    }

    int getResourceCount(Random* random) {
        if (id == Tile::lapisOre->id) return 4 + random->nextInt(5);
        return 1;
    }

protected:
    //@Override
    int getSpawnResourcesAuxValue(int data) {
        // lapis spawns blue dye
        if (id == Tile::lapisOre->id) return DyePowderItem::BLUE;
        return 0;
    }
};

