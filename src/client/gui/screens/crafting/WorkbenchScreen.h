#pragma once

#include "PaneCraftingScreen.h"

class WorkbenchScreen: public PaneCraftingScreen
{
	typedef PaneCraftingScreen super;
public:
    WorkbenchScreen(int craftingSize);
	~WorkbenchScreen();

private:
	bool filterRecipe(const Recipe& r);
};

