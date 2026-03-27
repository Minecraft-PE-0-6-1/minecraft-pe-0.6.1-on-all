#include "StonecutterScreen.hpp"
#include "CraftingFilters.hpp"
#include "world/level/material/Material.hpp"
#include "world/item/ItemCategory.hpp"

StonecutterScreen::StonecutterScreen()
:	super(Recipe::SIZE_3X3)
{
	setSingleCategoryAndIcon(ItemCategory::Structures, 5);
}

StonecutterScreen::~StonecutterScreen() {
}

bool StonecutterScreen::filterRecipe(const Recipe& r) {
	return CraftingFilters::isStonecutterItem(r.getResultItem());
}
