#pragma once

#include "client/gui/Screen.hpp"
#include "client/gui/components/Button.hpp"
#include "client/gui/components/OptionsGroup.hpp"

class ImageButton;
class OptionsPane;

class OptionsScreen: public Screen
{
	typedef Screen super;

	void init();
	void generateOptionScreens();

public:
	OptionsScreen();
	~OptionsScreen();

	void setupPositions();
	void buttonClicked(Button* button);
	void render(int xm, int ym, float a);
	void removed();
	void selectCategory(int index);

	virtual void mouseClicked(int x, int y, int buttonNum);
	virtual void mouseReleased(int x, int y, int buttonNum);
	virtual void keyPressed(int eventKey);
	virtual void charPressed(char inputChar);
	
	virtual void tick();

private:
	Touch::THeader* bHeader;
	ImageButton* btnClose;

	Button* btnCredits;   // <-- ADD THIS

	std::vector<Touch::TButton*> categoryButtons;
	std::vector<OptionsGroup*> optionPanes;

	OptionsGroup* currentOptionsGroup;

	int selectedCategory;
};

