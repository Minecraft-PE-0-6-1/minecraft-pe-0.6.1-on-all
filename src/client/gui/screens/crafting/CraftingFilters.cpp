#include "CraftingFilters.hpp"
#include "world/item/ItemInstance.hpp"
#include "world/item/Item.hpp"
#include "world/level/tile/Tile.hpp"
#include "world/level/material/Material.hpp"
#include "world/level/tile/StoneSlabTile.hpp"

namespace CraftingFilters {

bool isStonecutterItem(const ItemInstance& ins) {
	Item* const item = ins.getItem();
	if (item->id < 0 || item->id >= 256)
		return false;

	Tile* const tile = Tile::tiles[item->id];
	if (!tile)
		return false;

	// Special stone/sand cases
	if (	tile == Tile::lapisBlock
		||	tile == Tile::furnace
		||	tile == Tile::stonecutterBench)
		return false;

	if (tile == Tile::stoneSlabHalf && ins.getAuxValue() == StoneSlabTile::WOOD_SLAB)
		return false;

	// Return everything stone or sand
	return (tile->material == Material::stone || tile->material == Material::sand);
}

}