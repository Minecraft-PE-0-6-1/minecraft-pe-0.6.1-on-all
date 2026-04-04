#pragma once

//package net.minecraft.client.gui;

#include <vector>
#include "GuiComponent.hpp"

class Font;
class MinecraftClient;
class Button;
class TextBox;
struct IntRectangle;

class Screen: public GuiComponent
{
public:
	Screen(MinecraftClient& minecraft);

    virtual void render(int xm, int ym, float a);

    void init(int width, int height);
	virtual void init();

    void setSize(int width, int height);
	virtual void setupPositions() {};

	virtual void updateEvents();
    virtual void mouseEvent();
    virtual void keyboardEvent();
	virtual void keyboardTextEvent();
	virtual bool handleBackEvent(bool isDown);

    virtual void tick() {}

    virtual void removed() {}

    virtual void renderBackground();
    virtual void renderBackground(int vo);
    virtual void renderDirtBackground(int vo);
	// query
	virtual bool renderGameBehind();
	virtual bool hasClippingArea(IntRectangle& out);

    virtual bool isPauseScreen();
	virtual bool isErrorScreen();
	virtual bool isInGameScreen();
    virtual bool closeOnPlayerHurt();

    virtual void confirmResult(bool result, int id) {}
	virtual void lostFocus();
	virtual void toGUICoordinate(int& x, int& y);
protected:
	void updateTabButtonSelection();

	virtual void buttonClicked(Button* button) {}
	virtual void mouseClicked(int x, int y, int buttonNum);
	virtual void mouseReleased(int x, int y, int buttonNum);

	// mouse wheel movement (dx/dy are wheel deltas, xm/ym are GUI coords)
	virtual void mouseWheel(int dx, int dy, int xm, int ym) {}

	virtual void keyPressed(int eventKey);
	virtual void charPressed(char inputChar);
public:
	int width;
	int height;
	bool passEvents;
	//GuiParticles* particles;
protected:
	MinecraftClient& minecraft;
	std::vector<Button*> buttons;
	std::vector<TextBox*> textBoxes;

	std::vector<Button*> tabButtons;
	int tabButtonIndex;

	Font* font;
private:
	Button* clickedButton;
};

