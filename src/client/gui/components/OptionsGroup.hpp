#pragma once

//package net.minecraft.client.gui;

#include <string>
#include "GuiElementContainer.hpp"
#include "ScrollingPane.hpp"
#include "client/Options.hpp"

class Font;
class MinecraftClient;

class OptionsGroup: public GuiElementContainer {
	typedef GuiElementContainer super;
public:
	OptionsGroup(std::string labelID);
	virtual void setupPositions();
	virtual void render(MinecraftClient& minecraft, int xm, int ym);
	OptionsGroup& addOptionItem(OptionId optId, MinecraftClient& minecraft);
protected:

	void createToggle(OptionId optId, MinecraftClient& minecraft);
	void createProgressSlider(OptionId optId, MinecraftClient& minecraft);
	void createStepSlider(OptionId optId, MinecraftClient& minecraft);
	void createTextbox(OptionId optId, MinecraftClient& minecraft);
	void createKey(OptionId optId, MinecraftClient& minecraft);

	std::string label;
};

