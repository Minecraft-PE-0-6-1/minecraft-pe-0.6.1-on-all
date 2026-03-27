#pragma once

#include "PaneCraftingScreen.hpp"

class StonecutterScreen: public PaneCraftingScreen
{
	typedef PaneCraftingScreen super;
public:
    StonecutterScreen();
	~StonecutterScreen();

private:
	bool filterRecipe(const Recipe& r);
};

