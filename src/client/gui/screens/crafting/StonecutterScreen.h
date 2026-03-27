#pragma once

#include "PaneCraftingScreen.h"

class StonecutterScreen: public PaneCraftingScreen
{
	typedef PaneCraftingScreen super;
public:
    StonecutterScreen();
	~StonecutterScreen();

private:
	bool filterRecipe(const Recipe& r);
};

