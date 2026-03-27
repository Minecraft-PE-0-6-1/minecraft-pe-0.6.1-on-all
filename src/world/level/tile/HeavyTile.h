#pragma once

//package net.minecraft.world.level.tile;

#include "Tile.h"

class FallingTile;

class HeavyTile: public Tile {
    typedef Tile super;
public:
    static bool instaFall;

    HeavyTile(int id, int tex);
    HeavyTile(int id, int tex, const Material* material);

    void onPlace(Level* level, int x, int y, int z);
    void neighborChanged(Level* level, int x, int y, int z, int type);

    void tick(Level* level, int x, int y, int z, Random* random);
    int getTickDelay(Level* level);

    static bool isFree(Level* level, int x, int y, int z);

    virtual void falling(FallingTile* entity);
    virtual void onLand(Level* level, int xt, int yt, int zt, int data);
private:
    void checkSlide(Level* level, int x, int y, int z);
};

