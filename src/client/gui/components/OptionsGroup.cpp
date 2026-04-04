#include "OptionsGroup.hpp"
#include <MinecraftClient.hpp>
#include "ImageButton.hpp"
#include "OptionsItem.hpp"
#include "Slider.hpp"
#include "locale/I18n.hpp"
#include "TextOption.hpp"
#include "KeyOption.hpp"

OptionsGroup::OptionsGroup( std::string labelID )  {
	label = I18n::get(labelID);
}

void OptionsGroup::setupPositions() {
	// First we write the header and then we add the items
	int curY = y + 18;
	for(auto it = children.begin(); it != children.end(); ++it) {
		(*it)->width = width - 5;
		
		(*it)->y = curY;
		(*it)->x = x + 10;
		(*it)->setupPositions();
		curY += (*it)->height + 3;
	}
	height = curY;
}

void OptionsGroup::render( MinecraftClient& minecraft, int xm, int ym ) {
	float padX = 10.0f;
	float padY = 5.0f;
	
	minecraft.font()->draw(label, (float)x + padX, (float)y + padY, 0xffffffff, false);

	super::render(minecraft, xm, ym);
}

OptionsGroup& OptionsGroup::addOptionItem(OptionId optId, MinecraftClient& minecraft ) {
	auto option = minecraft.options().getOpt(optId);

	if (option == nullptr) return *this;

	// TODO: do a options key class to check it faster via dynamic_cast
	if (option->getStringId().find("options.key") != std::string::npos) createKey(optId, minecraft);
	else if (dynamic_cast<OptionBool*>(option)) createToggle(optId, minecraft);
	else if (dynamic_cast<OptionFloat*>(option)) createProgressSlider(optId, minecraft);
	else if (dynamic_cast<OptionInt*>(option)) createStepSlider(optId, minecraft);
	else if (dynamic_cast<OptionString*>(option)) createTextbox(optId, minecraft);

	return *this;
}

// TODO: wrap this copypaste shit into templates

void OptionsGroup::createToggle(OptionId optId, MinecraftClient& minecraft ) {
	ImageDef def;

	def.setSrc(IntRectangle(160, 206, 39, 20));
	def.name = "gui/touchgui.png";
	def.width = 39 * 0.7f;
	def.height = 20 * 0.7f;
	
	OptionButton* element = new OptionButton(optId);
	element->setImageDef(def, true);
	element->updateImage(&minecraft.options);
	
	std::string itemLabel = I18n::get(minecraft.options().getOpt(optId)->getStringId());
	
	OptionsItem* item = new OptionsItem(optId, itemLabel, element);
	
	addChild(item);
	setupPositions();
}

void OptionsGroup::createProgressSlider(OptionId optId, MinecraftClient& minecraft ) {
	Slider* element = new SliderFloat(minecraft, optId);
	element->width = 100;
	element->height = 20;

	std::string itemLabel = I18n::get(minecraft.options().getOpt(optId)->getStringId());
	OptionsItem* item = new OptionsItem(optId, itemLabel, element);
	addChild(item);
	setupPositions();
}

void OptionsGroup::createStepSlider(OptionId optId, MinecraftClient& minecraft ) {
	Slider* element = new SliderInt(minecraft, optId);
	element->width = 100;
	element->height = 20;
	std::string itemLabel = I18n::get(minecraft.options().getOpt(optId)->getStringId());
	OptionsItem* item = new OptionsItem(optId, itemLabel, element);
	addChild(item);
	setupPositions();
}

void OptionsGroup::createTextbox(OptionId optId, MinecraftClient& minecraft) {
	TextBox* element = new TextOption(minecraft, optId);
	element->width = 100;
	element->height = 20;

	std::string itemLabel = I18n::get(minecraft.options().getOpt(optId)->getStringId());
	OptionsItem* item = new OptionsItem(optId, itemLabel, element);
	addChild(item);
	setupPositions();
}

void OptionsGroup::createKey(OptionId optId, MinecraftClient& minecraft) {
	KeyOption* element = new KeyOption(minecraft, optId);
	element->width = 50;
	element->height = 20;

	std::string itemLabel = I18n::get(minecraft.options().getOpt(optId)->getStringId());
	OptionsItem* item = new OptionsItem(optId, itemLabel, element);
	addChild(item);
	setupPositions();
}