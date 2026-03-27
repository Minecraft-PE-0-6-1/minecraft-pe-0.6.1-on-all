#pragma once

//package net.minecraft.world.item;

#include "TileItem.hpp"
#include "world/level/tile/Tile.hpp"

class SaplingTileItem: public TileItem
{
    typedef TileItem super;
public:
    SaplingTileItem(int id)
    :   super(id)
    {
        setMaxDamage(0);
        setStackedByData(true);
    }

    int getLevelDataForAuxValue(int auxValue) {
        return auxValue;
    }

    int getIcon(int itemAuxValue) {
        return Tile::sapling->getTexture(0, itemAuxValue);
    }
};

