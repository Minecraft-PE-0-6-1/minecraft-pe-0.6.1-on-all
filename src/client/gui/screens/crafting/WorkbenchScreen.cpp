#include "WorkbenchScreen.hpp"
#include "CraftingFilters.hpp"
#include "world/level/material/Material.hpp"

WorkbenchScreen::WorkbenchScreen(int craftingSize)
:	super(craftingSize)
{
}

WorkbenchScreen::~WorkbenchScreen() {
}

bool WorkbenchScreen::filterRecipe(const Recipe& r) {
	return !CraftingFilters::isStonecutterItem(r.getResultItem());
}
