#include "OreRecipes.hpp"
#include "Recipes.hpp"
#include "world/level/tile/Tile.hpp"
#include "world/item/DyePowderItem.hpp"
#include <utility>

void OreRecipes::addRecipes(Recipes* r)
{
	typedef std::pair<Tile*, ItemInstance> Pair;
	Pair map[] = {
		std::make_pair(Tile::goldBlock,    ItemInstance(Item::goldIngot, 9)),
		std::make_pair(Tile::ironBlock,    ItemInstance(Item::ironIngot, 9)),
		std::make_pair(Tile::emeraldBlock, ItemInstance(Item::emerald, 9)),
		std::make_pair(Tile::lapisBlock,   ItemInstance(Item::dye_powder, 9, DyePowderItem::BLUE))
	};
	const int NumItems = sizeof(map) / sizeof(Pair);

	for (int i = 0; i < NumItems; i++) {
		Tile* from = map[i].first;
		ItemInstance to = map[i].second;

		r->addShapedRecipe(ItemInstance(from), //
			"###", //
			"###", //
			"###", //

			definition('#', to));

		r->addShapedRecipe(to, //
			"#", //

			definition('#', from));
	}

	//r->addShapedRecipe(ItemInstance(Item::goldIngot), //
	//	"###", //
	//	"###", //
	//	"###", //

	//	definition('#', Item::goldNugget));
	//r->addShapedRecipe(ItemInstance(Item::goldNugget, 9), //
	//    "#", //

	//  definition('#', Item::goldIngot));
}
