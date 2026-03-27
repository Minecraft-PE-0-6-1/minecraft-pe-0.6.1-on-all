#pragma once

//package net.minecraft.client.gui;

#include "client/gui/Screen.hpp"
#include <string>
#include "client/gui/components/ImageButton.hpp"
class SignTileEntity;
class Button;
class TextEditScreen: public Screen
{
    typedef Screen super;
public:
	TextEditScreen(SignTileEntity* signEntity);
	~TextEditScreen();
    void init();
	void tick();
	bool handleBackEvent(bool isDown);
	void render(int xm, int ym, float a);
	virtual void lostFocus();
	virtual void keyPressed(int eventKey);
	virtual void charPressed(char inputChar);
	void setupPositions();
	void buttonClicked(Button* button);
protected:
	bool isShowingKeyboard;
	SignTileEntity* sign;
	int frame;
	int line;
private:
	ImageButton btnClose;
};

