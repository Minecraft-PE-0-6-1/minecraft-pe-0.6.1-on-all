#pragma once
#include "client/gui/GuiComponent.hpp"

class Tesselator;
class MinecraftClient;

class GuiElement : public GuiComponent {
public:
	GuiElement(bool active=false, bool visible=true, int x = 0, int y = 0, int width=24, int height=24);
    virtual ~GuiElement() {}
    
	virtual void tick(MinecraftClient& minecraft) {}
    virtual void render(MinecraftClient& minecraft, int xm, int ym) { }
	virtual void setupPositions() {}

	virtual void mouseClicked(MinecraftClient& minecraft, int x, int y, int buttonNum) {}
	virtual void mouseReleased(MinecraftClient& minecraft, int x, int y, int buttonNum) {}
	virtual void keyPressed(MinecraftClient& minecraft, int key) {}
	virtual void charPressed(MinecraftClient& minecraft, char key) {}

	virtual bool pointInside(int x, int y);
	
	void setVisible(bool visible);
	
	bool active;
	bool visible;
	int x;
	int y;
	int width;
	int height;
};

