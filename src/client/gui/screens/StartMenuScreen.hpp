#pragma once

#include "client/gui/Screen.hpp"
#include "client/gui/components/Button.hpp"
#include "client/gui/components/ImageButton.hpp"

class StartMenuScreen: public Screen
{
public:
	StartMenuScreen();
	virtual ~StartMenuScreen();

	void init();
	void setupPositions();

	void tick();
	void render(int xm, int ym, float a);

	void buttonClicked(Button* button);
	virtual void mouseClicked(int x, int y, int buttonNum);
	bool handleBackEvent(bool isDown);
	bool isInGameScreen();
private:

	Button bHost;
	Button bJoin;
	Button bOptions;
	ImageButton bQuit; // X button in top-right corner

	std::string copyright;
	int copyrightPosX;

	std::string version;
	int versionPosX;

	std::string username;
};

