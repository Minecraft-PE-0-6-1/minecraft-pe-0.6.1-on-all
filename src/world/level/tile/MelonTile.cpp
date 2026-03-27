#include "MelonTile.hpp"
#include "world/level/material/Material.hpp"
#include "util/Random.hpp"
#include "world/Facing.hpp"
#include "world/item/Item.hpp"
MelonTile::MelonTile( int id )
:super(id, Material::vegetable){
	tex = TEX;
}

int MelonTile::getTexture( LevelSource* level, int x, int y, int z, int face ) {
	return getTexture(face);
}

int MelonTile::getTexture( int face ) {
	if(face == Facing::UP || face == Facing::DOWN) return TEX_TOP;
	return TEX;
}

int MelonTile::getResource( int data, Random* random ) {
	return Item::melon->id;
}

int MelonTile::getResourceCount( Random* random ) {
	return 3 + random->nextInt(5);
}

