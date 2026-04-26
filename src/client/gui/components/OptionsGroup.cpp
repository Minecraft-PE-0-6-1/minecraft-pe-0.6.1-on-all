#include "OptionsGroup.h"
#include "../../Minecraft.h"
#include "ImageButton.h"
#include "OptionsItem.h"
#include "Slider.h"
#include "../../../locale/I18n.h"
#include "TextOption.h"
#include "KeyOption.h"
#include <algorithm>
#include "../Gui.h"
#include "../Screen.h"
#include "../../../platform/input/Mouse.h"
#include "../../../util/Mth.h"

OptionsGroup::OptionsGroup( std::string labelID )
: contentHeight(0),
	scrollOffsetY(0.0f),
	maxScrollOffsetY(0.0f),
	trackingScrollGesture(false),
	scrollingGesture(false),
	touchDispatched(false),
	dragStartX(0),
	dragStartY(0),
	lastDragY(0),
	touchStartX(0),
	touchStartY(0) {
	label = I18n::get(labelID);
}

void OptionsGroup::setupPositions() {
	const int labelHeight = 18;
	const int bottomPadding = 36;
	const float requestedScroll = scrollOffsetY;
	const int scrollOffset = (int)requestedScroll;
	int curY = y + labelHeight - scrollOffset;
	const int contentStartY = y + labelHeight;

	// First we write the header and then we add the items
	for(std::vector<GuiElement*>::iterator it = children.begin(); it != children.end(); ++it) {
		(*it)->width = width - 5;
		
		(*it)->y = curY;
		(*it)->x = x + 10;
		(*it)->setupPositions();
		curY += (*it)->height + 3;
	}
	curY += bottomPadding;
	contentHeight = std::max(0, curY - contentStartY + scrollOffset);
	maxScrollOffsetY = std::max(0, contentHeight - (height - labelHeight));
	const float clampedScroll = Mth::clamp(requestedScroll, 0.0f, maxScrollOffsetY);
	if (clampedScroll != requestedScroll) {
		scrollOffsetY = clampedScroll;
		setupPositions();
	}
}

void OptionsGroup::render( Minecraft* minecraft, int xm, int ym ) {
	float padX = 10.0f;
	float padY = 5.0f;
	const int labelHeight = 18;
	
	minecraft->font->draw(label, (float)x + padX, (float)y + padY, 0xffffffff, false);

	glEnable2(GL_SCISSOR_TEST);
	glScissor(
		Gui::GuiScale * x,
		minecraft->height - Gui::GuiScale * (y + height),
		Gui::GuiScale * width,
		Gui::GuiScale * (height - labelHeight)
	);

	super::render(minecraft, xm, ym);
	glDisable2(GL_SCISSOR_TEST);
}

void OptionsGroup::tick(Minecraft* minecraft) {
	int xm = Mouse::getX();
	int ym = Mouse::getY();
	if (minecraft->screen != NULL) {
		minecraft->screen->toGUICoordinate(xm, ym);
	}

	bool leftDown = Mouse::isButtonDown(MouseAction::ACTION_LEFT);

	if (trackingScrollGesture && leftDown) {
		int dy = ym - lastDragY;
		int dx = xm - dragStartX;
		if (!scrollingGesture) {
			int totalDx = xm - dragStartX;
			int totalDy = ym - dragStartY;
			if (std::abs(totalDx) >= ScrollStartThreshold || std::abs(totalDy) >= ScrollStartThreshold) {
				if (std::abs(totalDy) >= std::abs(totalDx)) {
					scrollingGesture = true;
				} else if (!touchDispatched) {
					super::mouseClicked(minecraft, touchStartX, touchStartY, MouseAction::ACTION_LEFT);
					touchDispatched = true;
				}
			}
		}
		if (scrollingGesture && dy != 0) {
			scrollByPixels((float)dy);
		}
		lastDragY = ym;
	}
	super::tick(minecraft);
}

void OptionsGroup::mouseClicked(Minecraft* minecraft, int x, int y, int buttonNum) {
	trackingScrollGesture = false;
	scrollingGesture = false;
	touchDispatched = false;

	if (buttonNum == MouseAction::ACTION_LEFT && pointInside(x, y)) {
		trackingScrollGesture = true;
		dragStartX = x;
		dragStartY = y;
		lastDragY = y;
		touchStartX = x;
		touchStartY = y;
		return;
	}

	super::mouseClicked(minecraft, x, y, buttonNum);
}

void OptionsGroup::mouseReleased(Minecraft* minecraft, int x, int y, int buttonNum) {
	bool wasScrolling = scrollingGesture;
	bool wasTracking = trackingScrollGesture;
	trackingScrollGesture = false;
	scrollingGesture = false;
	if (buttonNum == MouseAction::ACTION_LEFT && wasTracking && !touchDispatched && pointInside(touchStartX, touchStartY)) {
		super::mouseClicked(minecraft, touchStartX, touchStartY, buttonNum);
		touchDispatched = true;
	}

	if (!wasScrolling) {
		super::mouseReleased(minecraft, x, y, buttonNum);
	}
}

void OptionsGroup::scrollByPixels(float deltaY) {
	if (deltaY == 0.0f || maxScrollOffsetY <= 0.0f) return;

	scrollOffsetY = Mth::clamp(scrollOffsetY - deltaY, 0.0f, maxScrollOffsetY);
	setupPositions();
}

bool OptionsGroup::isScrollingGestureActive() const {
	return trackingScrollGesture || scrollingGesture;
}

OptionsGroup& OptionsGroup::addOptionItem(OptionId optId, Minecraft* minecraft ) {
	auto option = minecraft->options.getOpt(optId);

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

void OptionsGroup::createToggle(OptionId optId, Minecraft* minecraft ) {
	ImageDef def;

	def.setSrc(IntRectangle(160, 206, 39, 20));
	def.name = "gui/touchgui.png";
	def.width = 39 * 0.7f;
	def.height = 20 * 0.7f;
	
	OptionButton* element = new OptionButton(optId);
	element->setImageDef(def, true);
	element->updateImage(&minecraft->options);
	
	std::string itemLabel = I18n::get(minecraft->options.getOpt(optId)->getStringId());
	
	OptionsItem* item = new OptionsItem(optId, itemLabel, element);
	
	addChild(item);
	setupPositions();
}

void OptionsGroup::createProgressSlider(OptionId optId, Minecraft* minecraft ) {
	Slider* element = new SliderFloat(minecraft, optId);
	element->width = 100;
	element->height = 20;

	std::string itemLabel = I18n::get(minecraft->options.getOpt(optId)->getStringId());
	OptionsItem* item = new OptionsItem(optId, itemLabel, element);
	addChild(item);
	setupPositions();
}

void OptionsGroup::createStepSlider(OptionId optId, Minecraft* minecraft ) {
	Slider* element = new SliderInt(minecraft, optId);
	element->width = 100;
	element->height = 20;
	std::string itemLabel = I18n::get(minecraft->options.getOpt(optId)->getStringId());
	OptionsItem* item = new OptionsItem(optId, itemLabel, element);
	addChild(item);
	setupPositions();
}

void OptionsGroup::createTextbox(OptionId optId, Minecraft* minecraft) {
	TextBox* element = new TextOption(minecraft, optId);
	element->width = 100;
	element->height = 20;

	std::string itemLabel = I18n::get(minecraft->options.getOpt(optId)->getStringId());
	OptionsItem* item = new OptionsItem(optId, itemLabel, element);
	addChild(item);
	setupPositions();
}

void OptionsGroup::createKey(OptionId optId, Minecraft* minecraft) {
	KeyOption* element = new KeyOption(minecraft, optId);
	element->width = 50;
	element->height = 20;

	std::string itemLabel = I18n::get(minecraft->options.getOpt(optId)->getStringId());
	OptionsItem* item = new OptionsItem(optId, itemLabel, element);
	addChild(item);
	setupPositions();
}
