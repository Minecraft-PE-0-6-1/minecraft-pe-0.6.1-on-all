#pragma once

//package net.minecraft.client.gui;

#include <string>
#include "GuiElement.hpp"
#include "client/Options.hpp"
#include "platform/input/Mouse.hpp"
#include "platform/input/Keyboard.hpp"

class Font;
class MinecraftClient;

class TextBox: public GuiElement
{
public:
	TextBox(int id, const std::string& msg);
	TextBox(int id, int x, int y, const std::string& msg);
	TextBox(int id, int x, int y, int w, int h, const std::string& msg);

	virtual void mouseClicked(MinecraftClient& minecraft, int x, int y, int buttonNum);

	virtual void setFocus(MinecraftClient& minecraft);
	virtual bool loseFocus(MinecraftClient& minecraft);

	virtual void render(MinecraftClient& minecraft, int xm, int ym);

	virtual void keyPressed(MinecraftClient& minecraft, int key);
	virtual void charPressed(MinecraftClient& minecraft, char c);
	virtual void tick(MinecraftClient& minecraft);
	
public:
	std::string hint;
	std::string text;
	int id;

	int blinkTicks;

	bool focused;
	bool blink;
};

