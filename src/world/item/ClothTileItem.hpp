#pragma once

//package net.minecraft.world.item;

#include "TileItem.hpp"
#include "DyePowderItem.hpp"
#include "world/level/tile/ClothTile.hpp"

class ClothTileItem: public TileItem
{
    typedef TileItem super;
public:
    ClothTileItem(int id)
    :   super(id)
    {
        setMaxDamage(0);
        setStackedByData(true);
    }

    /*@Override*/
    int getIcon(int itemAuxValue) {
        return Tile::cloth->getTexture(2, ClothTile::getTileDataForItemAuxValue(itemAuxValue));
    }

    /*@Override*/
    int getLevelDataForAuxValue(int auxValue) {
        return auxValue;
    }

    /*@Override*/
    std::string getDescriptionId(const ItemInstance* instance) const {
        return super::getDescriptionId() + "." + DyePowderItem::COLOR_DESCS[ClothTile::getTileDataForItemAuxValue(instance->getAuxValue())];
    }
};

