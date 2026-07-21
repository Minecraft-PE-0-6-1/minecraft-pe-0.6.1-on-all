#include "TallGrass.h"
#include "../FoliageColor.h"
#include "../GrassColor.h"
#include "../../entity/player/Player.h"
#include "../../item/Item.h"
#include "../../item/ShearsItem.h"
#include "../Level.h"
#include "../LevelSource.h"
#include "../biome/BiomeSource.h"

TallGrass::TallGrass( int id, int tex ) : super(id, tex, Material::replaceable_plant) {
	float ss = 0.4f;
	setShape(0.5f - ss, 0, 0.5f - ss, 0.5f + ss, 0.8f, 0.5f + ss);
}

int TallGrass::getTexture( int face, int data ) {
	if(data == TALL_GRASS) return tex;
	if(data == FERN) return tex + 16 + 1;
	if(data == DEAD_SHRUB) return tex + 16;
	return tex;
}

int TallGrass::getColor() {
	//double temp = 0.5;
	//double rain = 1.0;
	// converted to float for consistency - shredder
	float temp = 0.5;
	float rain = 1.0;
	if (GrassColor::useTint){
		return GrassColor::get(temp, rain);
	}
	return 0x339933;
}

int TallGrass::getColor( int auxData ) {
	if(auxData == DEAD_SHRUB);
	if (!FoliageColor::useTint && auxData == DEAD_SHRUB){
		return 0xffffff;
	}
	return FoliageColor::getDefaultColor();
}

int TallGrass::getColor( LevelSource* level, int x, int y, int z ) {
	int d = level->getData(x, y, z);
	if (d == DEAD_SHRUB); //return 0xffffff; // i removed this to make it accurate to beta 1.6.6 instead of early java release versions
	float temp = level->getBiomeSource()->temperatures[0]; // shredder added
	float rain = level->getBiomeSource()->downfalls != nullptr ? level->getBiomeSource()->downfalls[0] : 0; // shredder added
	if (!GrassColor::useTint && d == DEAD_SHRUB){
		return 0xffffff;
	}
	if (GrassColor::useTint){
		return GrassColor::get(temp, rain);
	}
	// @TODO port this function from beta 1.6.6 probably, for now im using biomesource to tint it directly above - shredder
	//if (GrassColor::useTint){ 
	//return level->getBiome(x, z)->getGrassColor();
	//}
	return 0x339933;//level->getBiome(x, z)->getGrassColor();
}

int TallGrass::getResource( int data, Random* random ) {
	if (random->nextInt(8) == 0) {
		return Item::seeds_wheat->id;
	}
	return -1;
}

void TallGrass::playerDestroy( Level* level, Player* player, int x, int y, int z, int data ) {
	if (!level->isClientSide && player->getSelectedItem() != NULL && player->getSelectedItem()->id == Item::shears->id) {
		//player->awardStat(Stats.blockMined[id], 1);

		// drop leaf block instead of sapling
		ItemInstance itemInstance(Tile::tallgrass, 1, data);
		popResource(level, x, y, z, itemInstance);
	} else {
		super::playerDestroy(level, player, x, y, z, data);
	}
}
