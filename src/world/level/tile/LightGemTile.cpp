#include "LightGemTile.hpp"
#include "util/Random.hpp"
#include "world/item/Item.hpp"
LightGemTile::LightGemTile( int id, int tex, const Material* material )
: super(id, tex, material) 
{ }

int LightGemTile::getResourceCount( Random* random ) {
	return 2 + random->nextInt(3);
}

int LightGemTile::getResource( int data, Random* random ) {
	return Item::yellowDust->id;
}

