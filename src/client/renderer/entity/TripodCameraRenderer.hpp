#pragma once

#include "EntityRenderer.hpp"
#include "client/renderer/TileRenderer.hpp"
#include "client/model/geom/ModelPart.hpp"
#include "world/level/tile/Tile.hpp"


class TripodCamera;

class TripodCameraRenderer: public EntityRenderer
{
    TileRenderer tileRenderer;

public:
    TripodCameraRenderer();

	void render(Entity* cam_, float x, float y, float z, float rot, float a);
	//void render( Tesselator &t );

	static float getFlashTime(const TripodCamera* c, float a);

private:
	Tile tripod;
	ModelPart cameraCube;
};

