#include "SurvivalMode.hpp"
#include <Minecraft.hpp>
#include <world/level/Level.hpp>
#include <world/entity/player/Abilities.hpp>

SurvivalMode::SurvivalMode( Minecraft& minecraft )
:	super(minecraft),
	xDestroyBlock(-1),
	yDestroyBlock(-1),
	zDestroyBlock(-1)
{
}

void SurvivalMode::continueDestroyBlock(Player* player, int x, int y, int z, int face ) {
	if (destroyDelay > 0) {
		destroyDelay--;
		return;
	}

	if (x == xDestroyBlock && y == yDestroyBlock && z == zDestroyBlock) {
		int t = minecraft.level->getTile(x, y, z);
		if (t == 0) return;
		Tile* tile = Tile::tiles[t];

		destroyProgress += tile->getDestroyProgress(player);

		if ((++destroyTicks & 3) == 1) {
#ifndef STANDALONE_SERVER
			if (tile != NULL) {
				minecraft.soundEngine()->play(tile->soundType->getStepSound(), x + 0.5f, y + 0.5f, z + 0.5f, (tile->soundType->getVolume() + 1) / 8, tile->soundType->getPitch() * 0.5f);
			}
#endif
		}

		if (destroyProgress >= 1) {
			destroyBlock(player, x, y, z, face);
			destroyProgress = 0;
			oDestroyProgress = 0;
			destroyTicks = 0;
			destroyDelay = 5;
		}
	} else {
		destroyProgress = 0;
		oDestroyProgress = 0;
		destroyTicks = 0;
		xDestroyBlock = x;
		yDestroyBlock = y;
		zDestroyBlock = z;
	}
}

bool SurvivalMode::destroyBlock(Player* player, int x, int y, int z, int face ) {
	int t = minecraft.level->getTile(x, y, z);
	int data = minecraft.level->getData(x, y, z);
	bool changed = GameMode::destroyBlock(player, x, y, z, face);
	bool couldDestroy = player->canDestroy(Tile::tiles[t]);

	ItemInstance* item = player->inventory->getSelected();
	if (item != NULL) {
		item->mineBlock(t, x, y, z);
		if (item->count == 0) {
			//item->snap(minecraft.player());
			player->inventory->clearSlot(player->inventory->selected);
		}
	}
	if (changed && couldDestroy) {
		ItemInstance instance(t, 1, data);
		Tile::tiles[t]->playerDestroy(minecraft.level, player, x, y, z, data);
	}
	return changed;
}

void SurvivalMode::stopDestroyBlock(Player* player) {
	destroyProgress = 0;
	destroyDelay = 0;
}

void SurvivalMode::initAbilities( Abilities& abilities ) {
	abilities.flying = false;
	abilities.mayfly = false;
	abilities.instabuild = false;
	abilities.invulnerable = false;
}

void SurvivalMode::startDestroyBlock(Player* player, int x, int y, int z, int face ) {
	if(player->getCarriedItem() != NULL && player->getCarriedItem()->id == Item::bow->id)
		return;

	int t = minecraft.level->getTile(x, y, z);
	if (t > 0 && destroyProgress == 0) Tile::tiles[t]->attack(minecraft.level, x, y, z, player);
	if (t > 0 && Tile::tiles[t]->getDestroyProgress(player) >= 1)
		destroyBlock(player, x, y, z, face);
}
