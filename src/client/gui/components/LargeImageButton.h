#pragma once

#include "ImageButton.h"

class LargeImageButton: public ImageButton
{
	typedef ImageButton super;
public:
	LargeImageButton(int id, const std::string& msg);
	LargeImageButton(int id, const std::string& msg, ImageDef& imageDef);

	void render(Minecraft* minecraft, int xm, int ym);

private:
	void setupDefault();

	float _buttonScale;
};

