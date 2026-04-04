#pragma once

#include "ImageButton.hpp"

class LargeImageButton: public ImageButton
{
	typedef ImageButton super;
public:
	LargeImageButton(int id, const std::string& msg);
	LargeImageButton(int id, const std::string& msg, ImageDef& imageDef);

	void render(MinecraftClient& minecraft, int xm, int ym);

private:
	void setupDefault();

	float _buttonScale;
};

