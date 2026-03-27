#include "OptionsScreen.hpp"

#include "StartMenuScreen.hpp"
#include "UsernameScreen.hpp"
#include "DialogDefinitions.hpp"
#include "client/Minecraft.hpp"
#include "AppPlatform.hpp"
#include "CreditsScreen.hpp"

#include "client/gui/components/ImageButton.hpp"
#include "client/gui/components/OptionsGroup.hpp"
#include "platform/input/Keyboard.hpp"

OptionsScreen::OptionsScreen()
	: btnClose(NULL),
	bHeader(NULL),
	btnCredits(NULL),
	selectedCategory(0) {
}

OptionsScreen::~OptionsScreen() {
	if (btnClose != NULL) {
		delete btnClose;
		btnClose = NULL;
	}

	if (bHeader != NULL) {
		delete bHeader;
		bHeader = NULL;
	}

	if (btnCredits != NULL) {
		delete btnCredits;
		btnCredits = NULL;
	}

	for (std::vector<Touch::TButton*>::iterator it = categoryButtons.begin(); it != categoryButtons.end(); ++it) {
		if (*it != NULL) {
			delete* it;
			*it = NULL;
		}
	}

	for (std::vector<OptionsGroup*>::iterator it = optionPanes.begin(); it != optionPanes.end(); ++it) {
		if (*it != NULL) {
			delete* it;
			*it = NULL;
		}
	}

	categoryButtons.clear();
}

void OptionsScreen::init() {
	bHeader = new Touch::THeader(0, "Options");

	btnClose = new ImageButton(1, "");

	ImageDef def;
	def.name = "gui/touchgui.png";
	def.width = 34;
	def.height = 26;

	def.setSrc(IntRectangle(150, 0, (int)def.width, (int)def.height));
	btnClose->setImageDef(def, true);

	categoryButtons.push_back(new Touch::TButton(2, "General"));
	categoryButtons.push_back(new Touch::TButton(3, "Game"));
	categoryButtons.push_back(new Touch::TButton(4, "Controls"));
	categoryButtons.push_back(new Touch::TButton(5, "Graphics"));
	categoryButtons.push_back(new Touch::TButton(6, "Tweaks"));

	btnCredits = new Touch::TButton(11, "Credits");

	buttons.push_back(bHeader);
	buttons.push_back(btnClose);
	buttons.push_back(btnCredits);

	for (std::vector<Touch::TButton*>::iterator it = categoryButtons.begin(); it != categoryButtons.end(); ++it) {
		buttons.push_back(*it);
		tabButtons.push_back(*it);
	}

	generateOptionScreens();
	// start with first category selected
	selectCategory(0);
}

void OptionsScreen::setupPositions() {
	int buttonHeight = btnClose->height;

	btnClose->x = width - btnClose->width;
	btnClose->y = 0;

	int offsetNum = 1;

	for (std::vector<Touch::TButton*>::iterator it = categoryButtons.begin(); it != categoryButtons.end(); ++it) {

		(*it)->x = 0;
		(*it)->y = offsetNum * buttonHeight;
		(*it)->selected = false;

		offsetNum++;
	}

	bHeader->x = 0;
	bHeader->y = 0;
	bHeader->width = width - btnClose->width;
	bHeader->height = btnClose->height;

	// Credits button (bottom-right)
	if (btnCredits != NULL) {
		btnCredits->x = width - btnCredits->width;
		btnCredits->y = height - btnCredits->height;
	}

	for (std::vector<OptionsGroup*>::iterator it = optionPanes.begin(); it != optionPanes.end(); ++it) {

		if (categoryButtons.size() > 0 && categoryButtons[0] != NULL) {

			(*it)->x = categoryButtons[0]->width;
			(*it)->y = bHeader->height;
			(*it)->width = width - categoryButtons[0]->width;

			(*it)->setupPositions();
		}
	}

	// don't override user selection on resize
}


void OptionsScreen::render(int xm, int ym, float a) {
	renderBackground();

	int xmm = xm * width / minecraft->width;
	int ymm = ym * height / minecraft->height - 1;

	if (currentOptionsGroup != NULL)
		currentOptionsGroup->render(minecraft, xmm, ymm);
	
	super::render(xm, ym, a);
}

void OptionsScreen::removed() {
}

void OptionsScreen::buttonClicked(Button* button) {
	if (button == btnClose) {
		minecraft->options.save();
		if (minecraft->screen != NULL) {
			minecraft->setScreen(NULL);
		} else {
			minecraft->screenChooser.setScreen(SCREEN_STARTMENU);
		}
	}
	else if (button->id > 1 && button->id < 7) {
		int categoryButton = button->id - categoryButtons[0]->id;
		selectCategory(categoryButton);
	}
	else if (button == btnCredits) {
		minecraft->setScreen(new CreditsScreen());
	}
}

void OptionsScreen::selectCategory(int index) {
	int currentIndex = 0;

	for (std::vector<Touch::TButton*>::iterator it = categoryButtons.begin(); it != categoryButtons.end(); ++it) {

		if (index == currentIndex)
			(*it)->selected = true;
		else
			(*it)->selected = false;

		currentIndex++;
	}

	if (index < (int)optionPanes.size())
		currentOptionsGroup = optionPanes[index];
}

void OptionsScreen::generateOptionScreens() {
	// how the fuck it works

	optionPanes.push_back(new OptionsGroup("options.group.general"));
	optionPanes.push_back(new OptionsGroup("options.group.game"));
	optionPanes.push_back(new OptionsGroup("options.group.controls"));
	optionPanes.push_back(new OptionsGroup("options.group.graphics"));
	optionPanes.push_back(new OptionsGroup("options.group.tweaks"));

	// General Pane
	optionPanes[0]->addOptionItem(OPTIONS_USERNAME, minecraft)
		.addOptionItem(OPTIONS_SENSITIVITY, minecraft);

	// Game Pane
	optionPanes[1]->addOptionItem(OPTIONS_DIFFICULTY, minecraft)
		.addOptionItem(OPTIONS_SERVER_VISIBLE, minecraft)
		.addOptionItem(OPTIONS_THIRD_PERSON_VIEW, minecraft)
		.addOptionItem(OPTIONS_GUI_SCALE, minecraft)
		.addOptionItem(OPTIONS_SENSITIVITY, minecraft)
		.addOptionItem(OPTIONS_MUSIC_VOLUME, minecraft)
		.addOptionItem(OPTIONS_SOUND_VOLUME, minecraft)
		.addOptionItem(OPTIONS_SMOOTH_CAMERA, minecraft)
		.addOptionItem(OPTIONS_DESTROY_VIBRATION, minecraft)
		.addOptionItem(OPTIONS_IS_LEFT_HANDED, minecraft);

	// // Controls Pane
	optionPanes[2]->addOptionItem(OPTIONS_INVERT_Y_MOUSE, minecraft)
		.addOptionItem(OPTIONS_USE_TOUCHSCREEN, minecraft)
		.addOptionItem(OPTIONS_AUTOJUMP, minecraft);

	for (int i = OPTIONS_KEY_FORWARD; i <= OPTIONS_KEY_USE; i++) {
		optionPanes[2]->addOptionItem((OptionId)i, minecraft);
	}

	// // Graphics Pane
	optionPanes[3]->addOptionItem(OPTIONS_FANCY_GRAPHICS, minecraft)
		.addOptionItem(OPTIONS_LIMIT_FRAMERATE, minecraft)
		.addOptionItem(OPTIONS_VSYNC, minecraft)
		.addOptionItem(OPTIONS_RENDER_DEBUG, minecraft)
		.addOptionItem(OPTIONS_ANAGLYPH_3D, minecraft)
		.addOptionItem(OPTIONS_VIEW_BOBBING, minecraft)
		.addOptionItem(OPTIONS_AMBIENT_OCCLUSION, minecraft);
	
	optionPanes[4]->addOptionItem(OPTIONS_ALLOW_SPRINT, minecraft)
		.addOptionItem(OPTIONS_BAR_ON_TOP, minecraft)
		.addOptionItem(OPTIONS_RPI_CURSOR, minecraft);
}

void OptionsScreen::mouseClicked(int x, int y, int buttonNum) {
	if (currentOptionsGroup != NULL)
		currentOptionsGroup->mouseClicked(minecraft, x, y, buttonNum);

	super::mouseClicked(x, y, buttonNum);
}

void OptionsScreen::mouseReleased(int x, int y, int buttonNum) {
	if (currentOptionsGroup != NULL)
		currentOptionsGroup->mouseReleased(minecraft, x, y, buttonNum);

	super::mouseReleased(x, y, buttonNum);
}

void OptionsScreen::keyPressed(int eventKey) {
	if (currentOptionsGroup != NULL)
		currentOptionsGroup->keyPressed(minecraft, eventKey);
	if (eventKey == Keyboard::KEY_ESCAPE) 
		minecraft->options.save();
	
	super::keyPressed(eventKey);
}

void OptionsScreen::charPressed(char inputChar) {
	if (currentOptionsGroup != NULL)
		currentOptionsGroup->charPressed(minecraft, inputChar);

	super::keyPressed(inputChar);
}

void OptionsScreen::tick() {

	if (currentOptionsGroup != NULL)
		currentOptionsGroup->tick(minecraft);

	super::tick();
}