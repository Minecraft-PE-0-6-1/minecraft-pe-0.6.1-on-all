#pragma once
#include "GuiElement.hpp"
#include <vector>
class Tesselator;
class MinecraftClient;

class GuiElementContainer : public GuiElement {
public:
	GuiElementContainer(bool active=false, bool visible=true, int x = 0, int y = 0, int width=24, int height=24);
    virtual ~GuiElementContainer();
    virtual void render(MinecraftClient& minecraft, int xm, int ym);
	virtual void setupPositions();
	virtual void addChild(GuiElement* element);
	virtual void removeChild(GuiElement* element);

	virtual void tick( MinecraftClient& minecraft );

	virtual void mouseClicked( MinecraftClient& minecraft, int x, int y, int buttonNum );
	virtual void mouseReleased( MinecraftClient& minecraft, int x, int y, int buttonNum );
	virtual void keyPressed(MinecraftClient& minecraft, int key);
	virtual void charPressed(MinecraftClient& minecraft, char key);

protected:
	std::vector<GuiElement*> children;
};

