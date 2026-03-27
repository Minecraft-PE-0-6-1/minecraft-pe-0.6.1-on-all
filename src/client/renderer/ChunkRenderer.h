#pragma once

#include "gles.h"
#include "../../world/level/Region.h"

class ChunkRenderer {
public:
	ChunkRenderer(const Region& region)
		: r(region)
	{
	}

	void render() {
		//glTranslate(r.xc1, 
	}

private:
	const Region& r;
};

