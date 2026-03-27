#pragma once
#include "EntityRenderer.h"

class ArrowRenderer : public EntityRenderer {
	void render(Entity* arrow, float x, float y, float z, float rot, float a);
};

