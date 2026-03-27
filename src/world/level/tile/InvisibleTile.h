#pragma once

#include "Tile.h"

class InvisibleTile : public Tile
{
	typedef Tile super;
public:
	InvisibleTile(int id, int tex, const Material* material)
	:	super(id, tex, material)
	{
	}

	virtual int getRenderShape()
	{
		return SHAPE_INVISIBLE;
	}
};

