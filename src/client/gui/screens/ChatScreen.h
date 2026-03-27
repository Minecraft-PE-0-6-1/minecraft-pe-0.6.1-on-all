#pragma once

#include "../Screen.h"

class ChatScreen: public Screen
{
public:
	ChatScreen() {}
	virtual ~ChatScreen() {}

	void init();

	void render(int xm, int ym, float a);

	void buttonClicked(Button* button) {};

private:
};

