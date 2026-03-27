#pragma once

#include "../Screen.h"
class Button;

class InBedScreen: public Screen
{
public:
	InBedScreen();

	virtual ~InBedScreen();

	void init();

	void setupPositions();

	void render(int xm, int ym, float a);

	void buttonClicked(Button* button);

private:
	Button* bWakeUp;
};

