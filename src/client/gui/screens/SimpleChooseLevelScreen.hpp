#pragma once

#include "ChooseLevelScreen.hpp"
#include "client/gui/components/TextBox.hpp"
#include "client/gui/components/Button.hpp"    // for Touch::THeader
class Button;
class ImageButton;

class SimpleChooseLevelScreen: public ChooseLevelScreen
{
public:
	SimpleChooseLevelScreen(const std::string& levelName);

	virtual ~SimpleChooseLevelScreen();

	void init();
	void setupPositions();
	void tick();

	void render(int xm, int ym, float a);

	void buttonClicked(Button* button);
	bool handleBackEvent(bool isDown);
	virtual void keyPressed(int eventKey);
	virtual void mouseClicked(int x, int y, int buttonNum);

private:
	Touch::THeader* bHeader;
	Button* bGamemode;
	Button* bCheats;
	ImageButton* bBack;
	Button* bCreate;
	bool hasChosen;

	std::string levelName;
	int gamemode;
	bool cheatsEnabled;

	TextBox tLevelName;
	TextBox tSeed;
};

