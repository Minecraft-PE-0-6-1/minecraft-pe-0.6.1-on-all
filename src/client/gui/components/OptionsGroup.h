#ifndef NET_MINECRAFT_CLIENT_GUI_COMPONENTS__OptionsGroup_H__
#define NET_MINECRAFT_CLIENT_GUI_COMPONENTS__OptionsGroup_H__

//package net.minecraft.client.gui;

#include <string>
#include "GuiElementContainer.h"
#include "ScrollingPane.h"
#include "../../Options.h"

class Font;
class Minecraft;

class OptionsGroup: public GuiElementContainer {
	typedef GuiElementContainer super;
public:
	OptionsGroup(std::string labelID);
	virtual void setupPositions();
	virtual void render(Minecraft* minecraft, int xm, int ym);
	virtual void tick(Minecraft* minecraft);
	virtual void mouseClicked(Minecraft* minecraft, int x, int y, int buttonNum);
	virtual void mouseReleased(Minecraft* minecraft, int x, int y, int buttonNum);
	OptionsGroup& addOptionItem(OptionId optId, Minecraft* minecraft);
	void scrollByPixels(float deltaY);
	bool isScrollingGestureActive() const;
protected:

	void createToggle(OptionId optId, Minecraft* minecraft);
	void createProgressSlider(OptionId optId, Minecraft* minecraft);
	void createStepSlider(OptionId optId, Minecraft* minecraft);
	void createTextbox(OptionId optId, Minecraft* minecraft);
	void createKey(OptionId optId, Minecraft* minecraft);

	std::string label;
	int contentHeight = 0;
	float scrollOffsetY = 0.f;
	float maxScrollOffsetY = 0.f;
	bool trackingScrollGesture = false;
	bool scrollingGesture = false;
	bool touchDispatched = false;
	int dragStartX = 0;
	int dragStartY = 0;
	int lastDragY = 0;
	int touchStartX = 0;
	int touchStartY = 0;
	static const int ScrollStartThreshold = 5;
};

#endif /*NET_MINECRAFT_CLIENT_GUI_COMPONENTS__OptionsGroup_H__*/
