#include "GuiElementContainer.hpp"
#include <algorithm>
GuiElementContainer::GuiElementContainer( bool active/*=false*/, bool visible/*=true*/, int x /*= 0*/, int y /*= 0*/, int width/*=24*/, int height/*=24*/ )
: GuiElement(active, visible, x, y, width, height) {

}

GuiElementContainer::~GuiElementContainer() {
	while(!children.empty()) {
		GuiElement* element = children.back();
		children.pop_back();
		delete element;
	}
}

void GuiElementContainer::render( MinecraftClient& minecraft, int xm, int ym ) {
	for(auto it = children.begin(); it != children.end(); ++it) {
		(*it)->render(minecraft, xm, ym);
	}
}

void GuiElementContainer::setupPositions() {
	for(auto it = children.begin(); it != children.end(); ++it) {
		(*it)->setupPositions();
	}
}

void GuiElementContainer::addChild( GuiElement* element ) {
	children.push_back(element);
}

void GuiElementContainer::removeChild( GuiElement* element ) {
	auto it = std::find(children.begin(), children.end(), element);
	if(it != children.end())
		children.erase(it);
}

void GuiElementContainer::tick( MinecraftClient& minecraft ) {
	for(auto it = children.begin(); it != children.end(); ++it) {
		(*it)->tick(minecraft);
	}
}

void GuiElementContainer::mouseClicked( MinecraftClient& minecraft, int x, int y, int buttonNum ) {
	for(auto it = children.begin(); it != children.end(); ++it) {
		(*it)->mouseClicked(minecraft, x, y, buttonNum);
	}
}

void GuiElementContainer::mouseReleased( MinecraftClient& minecraft, int x, int y, int buttonNum ) {
	for(auto it = children.begin(); it != children.end(); ++it) {
		(*it)->mouseReleased(minecraft, x, y, buttonNum);
	}
}

void GuiElementContainer::keyPressed(MinecraftClient& minecraft, int key) {
	for(auto it = children.begin(); it != children.end(); ++it) {
		(*it)->keyPressed(minecraft, key);
	}
}

void GuiElementContainer::charPressed(MinecraftClient& minecraft, char key) {
	for(auto it = children.begin(); it != children.end(); ++it) {
		(*it)->charPressed(minecraft, key);
	}
}