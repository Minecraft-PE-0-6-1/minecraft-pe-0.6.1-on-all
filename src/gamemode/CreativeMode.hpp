#pragma once

//package net.minecraft.client.gamemode;

#include "GameMode.hpp"

class CreativeMode: public GameMode
{
	typedef GameMode super;
public:
    CreativeMode(Minecraft& minecraft);

    void startDestroyBlock(Player* player, int x, int y, int z, int face);
    void continueDestroyBlock(Player* player, int x, int y, int z, int face);
    void stopDestroyBlock(Player* player);

	bool isCreativeType();

	void initAbilities(Abilities& abilities);

	void releaseUsingItem(Player* player);
private:
	void creativeDestroyBlock(Player* player, int x, int y, int z, int face);
};

