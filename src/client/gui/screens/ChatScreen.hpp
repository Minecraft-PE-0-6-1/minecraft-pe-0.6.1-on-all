#pragma once

#include "client/gui/Screen.hpp"
#include "client/gui/components/TextBox.hpp"

class ChatScreen: public Screen
{
public:
	ChatScreen();

	void init();
	
	void render(int xm, int ym, float a);

	void buttonClicked(Button* button) {};

	bool handleBackEvent(bool isDown);
	void keyPressed(int eventKey);
private:
	TextBox m_input;
};

