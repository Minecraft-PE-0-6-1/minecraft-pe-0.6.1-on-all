#pragma once

//package net.minecraft.client.gui;

#include <string>
#include "GuiElement.hpp"
#include "client/Options.hpp"

class Font;
class MinecraftClient;

class Button: public GuiElement
{
public:
	Button(int id, const std::string& msg);
    Button(int id, int x, int y, const std::string& msg);
    Button(int id, int x, int y, int w, int h, const std::string& msg);
	virtual ~Button() {}
    virtual void render(MinecraftClient& minecraft, int xm, int ym);

	virtual bool clicked(MinecraftClient& minecraft, int mx, int my);
    virtual void released(int mx, int my);
    virtual void setPressed();

	bool isInside(int xm, int ym);
protected:
	virtual int getYImage(bool hovered);
	virtual void renderBg(MinecraftClient& minecraft, int xm, int ym);

	virtual void renderFace(MinecraftClient& minecraft, int xm, int ym);
	bool hovered(MinecraftClient& minecraft, int xm, int ym);	
public:
	std::string msg;
	int id;

	bool selected;
protected:
    bool _currentlyDown;
};

// @note: A bit backwards, but this is a button that
//        only reacts to clicks, but isn't rendered.
class BlankButton: public Button
{
	typedef Button super;
public:
	BlankButton(int id);
	BlankButton(int id, int x, int y, int w, int h);
};


namespace Touch {
class TButton: public Button
{
	typedef Button super;
public:
	TButton(int id, const std::string& msg);
	TButton(int id, int x, int y, const std::string& msg);
	TButton(int id, int x, int y, int w, int h, const std::string& msg);
protected:
	virtual void renderBg(MinecraftClient& minecraft, int xm, int ym);
};

// "Header" in Touchscreen mode
class THeader: public Button {
	typedef Button super;
public:
	THeader(int id, const std::string& msg);
	THeader(int id, int x, int y, const std::string& msg);
	THeader(int id, int x, int y, int w, int h, const std::string& msg);
protected:
	virtual void renderBg(MinecraftClient& minecraft, int xm, int ym);
	void render( MinecraftClient& minecraft, int xm, int ym );
public:
	int xText;
};
}

