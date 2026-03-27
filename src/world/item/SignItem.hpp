#pragma once

//package net.minecraft.world.item;

#include "world/entity/player/Player.hpp"
#include "world/level/Level.hpp"
#include "world/level/tile/Tile.hpp"
#include "world/level/tile/entity/SignTileEntity.hpp"

class SignItem: public Item
{
	typedef Item super;
public:
    SignItem(int id)
    :   super(id)
    {
        maxStackSize = 16;
    }

    /*@Override*/
    bool useOn(ItemInstance* instance, Player* player, Level* level, int x, int y, int z, int face, float clickX, float clickY, float clickZ) {
        if (face == 0) return false;
        if (!level->getMaterial(x, y, z)->isSolid()) return false;

        if (face == 1) y++;

        if (face == 2) z--;
        if (face == 3) z++;
        if (face == 4) x--;
        if (face == 5) x++;

        //if (!player->mayUseItemAt(x, y, z, face, instance)) return false;
        if (!Tile::sign->mayPlace(level, x, y, z)) return false;

        if (face == 1) {
            int rot = Mth::floor(((player->yRot + 180) * 16) / 360 + 0.5f) & 15;
            level->setTileAndData(x, y, z, Tile::sign->id, rot);
        } else {
            level->setTileAndData(x, y, z, Tile::wallSign->id, face);
        }

        instance->count--;
        SignTileEntity* ste = (SignTileEntity*) level->getTileEntity(x, y, z);
        if (ste != NULL) player->openTextEdit(ste);
        return true;
    }
};

