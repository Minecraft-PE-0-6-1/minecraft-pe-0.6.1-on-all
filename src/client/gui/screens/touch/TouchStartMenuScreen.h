#pragma once

#include "../../Screen.h"
#include "../../components/LargeImageButton.h"
#include "../../components/ImageButton.h"
#include "../../components/TextBox.h"

namespace Touch {

class StartMenuScreen: public Screen
{
public:
	StartMenuScreen();
	virtual ~StartMenuScreen();
	
	void init();
	void setupPositions();

	void render(int xm, int ym, float a);

	void buttonClicked(Button* button);
	virtual void mouseClicked(int x, int y, int buttonNum);
	bool handleBackEvent(bool isDown);
	bool isInGameScreen();
private:
	
	LargeImageButton bHost;
	LargeImageButton bJoin;
	LargeImageButton bOptions;
	ImageButton bQuit; // X close icon

	std::string copyright;
	int copyrightPosX;

	std::string version;
	int versionPosX;

	std::string username;
};
};

