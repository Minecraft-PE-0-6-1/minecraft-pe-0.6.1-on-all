#include "CreativeMode.h"
#include <Minecraft.h>
#include <world/level/Level.h>
#include <world/entity/player/Abilities.h>

static const int DestructionTickDelay = 5;

CreativeMode::CreativeMode(Minecraft& minecraft)
: 	super(minecraft)
{
}

void CreativeMode::startDestroyBlock(Player* player, int x, int y, int z, int face) {
	if(player->getCarriedItem() != NULL && player->getCarriedItem()->id == Item::bow->id)
		return;

	creativeDestroyBlock(player, x, y, z, face);
	destroyDelay = DestructionTickDelay;
}

void CreativeMode::creativeDestroyBlock(Player* player, int x, int y, int z, int face) {
	minecraft.level->extinguishFire(x, y, z, face);
	destroyBlock(player, x, y, z, face);
}

void CreativeMode::continueDestroyBlock(Player* player, int x, int y, int z, int face) {
	destroyDelay--;
	if (destroyDelay <= 0) {
		destroyDelay = DestructionTickDelay;
		creativeDestroyBlock(player, x, y, z, face);
	}
}

void CreativeMode::stopDestroyBlock(Player* player) {
	destroyDelay = 0;
}

void CreativeMode::initAbilities( Abilities& abilities ) {
	abilities.mayfly = true;
	abilities.instabuild = true;
	abilities.invulnerable = true;
}

bool CreativeMode::isCreativeType() {
	return true;
}

void CreativeMode::releaseUsingItem( Player* player ) {
	if(player->getCarriedItem() != NULL) {
		int oldItemId = player->getCarriedItem()->id;
		int oldAux = player->getAuxData();
		super::releaseUsingItem(player);
		if(player->getCarriedItem() != NULL && player->getCarriedItem()->id == oldItemId) {
			player->getCarriedItem()->setAuxValue(oldAux);
		}
	} else {
		super::releaseUsingItem(player);
	}
}
