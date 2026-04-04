#pragma once

#include "GuiElement.hpp"
#include "client/Options.hpp"
#include <client/Option.hpp>

class Slider : public GuiElement {
	typedef GuiElement super;
public:
	virtual void render( MinecraftClient& minecraft, int xm, int ym );
	virtual void mouseClicked( MinecraftClient& minecraft, int x, int y, int buttonNum );
	virtual void mouseReleased( MinecraftClient& minecraft, int x, int y, int buttonNum );
	virtual void tick(MinecraftClient& minecraft);

protected:
	Slider(OptionId optId);

	OptionId m_optId;

	bool m_mouseDownOnElement;
	float m_percentage;
	int m_numSteps;
};

class SliderFloat : public Slider {
public:
	SliderFloat(MinecraftClient& minecraft, OptionId option);

	virtual void mouseReleased( MinecraftClient& minecraft, int x, int y, int buttonNum ) override;	

protected:
	OptionFloat* m_option;
};


class SliderInt : public Slider {
public:
	SliderInt(MinecraftClient& minecraft, OptionId option);

	virtual void render( MinecraftClient& minecraft, int xm, int ym ) override;
	virtual void mouseReleased( MinecraftClient& minecraft, int x, int y, int buttonNum ) override;

protected:
	OptionInt* m_option;
};

