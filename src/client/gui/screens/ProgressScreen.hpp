#pragma once

#include "client/gui/Screen.hpp"

class ProgressScreen: public Screen
{
public:
	ProgressScreen();

	void render(int xm, int ym, float a);
	bool isInGameScreen();

	virtual void keyPressed(int eventKey) {}

	void tick();
private:
	int ticks;
};

