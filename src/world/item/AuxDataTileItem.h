#pragma once

//package net.minecraft.world.item;

#include "TileItem.h"
#include "../level/tile/Tile.h"

class AuxDataTileItem: public TileItem
{
    typedef TileItem super;
public:
    AuxDataTileItem(int id, Tile* parentTile)
    :   super(id),
        parentTile(parentTile)
    {
        setMaxDamage(0);
        setStackedByData(true);
    }

    /*@Override*/
    int getIcon(int itemAuxValue) {
        return parentTile->getTexture(2, itemAuxValue);
    }

    /*@Override*/
    int getLevelDataForAuxValue(int auxValue) {
        return auxValue;
    }
private:
    Tile* parentTile;
};

