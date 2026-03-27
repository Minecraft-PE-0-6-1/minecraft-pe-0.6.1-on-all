#pragma once
#include "EntityRenderer.hpp"

class ArrowRenderer : public EntityRenderer {
	void render(Entity* arrow, float x, float y, float z, float rot, float a);
};

