#pragma once

#include <string>
#include <vector>
#include "GuiElementContainer.hpp"
#include "world/item/ItemInstance.hpp"
#include "client/Options.hpp"
class Font; 
class Textures;
class NinePatchLayer;
class ItemPane;

class OptionsItem: public GuiElementContainer
{
	typedef GuiElementContainer super;
public:
	OptionsItem(OptionId optionId, std::string label, GuiElement* element);
	virtual void render(Minecraft* minecraft, int xm, int ym);
	void setupPositions();

private:
	OptionId m_optionId;
	std::string m_label;
};

