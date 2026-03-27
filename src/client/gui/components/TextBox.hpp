#pragma once

//package net.minecraft.client.gui;

#include <string>
#include "GuiElement.hpp"
#include "client/Options.hpp"
#include "platform/input/Mouse.hpp"
#include "platform/input/Keyboard.hpp"

class Font;
class Minecraft;

class TextBox: public GuiElement
{
public:
	TextBox(int id, const std::string& msg);
	TextBox(int id, int x, int y, const std::string& msg);
	TextBox(int id, int x, int y, int w, int h, const std::string& msg);

	virtual void mouseClicked(Minecraft* minecraft, int x, int y, int buttonNum);

	virtual void setFocus(Minecraft* minecraft);
	virtual bool loseFocus(Minecraft* minecraft);

	virtual void render(Minecraft* minecraft, int xm, int ym);

	virtual void keyPressed(Minecraft* minecraft, int key);
	virtual void charPressed(Minecraft* minecraft, char c);
	virtual void tick(Minecraft* minecraft);
	
public:
	std::string hint;
	std::string text;
	int id;

	int blinkTicks;

	bool focused;
	bool blink;
};

