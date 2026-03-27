#pragma once

#include "gles.hpp"
#include "world/level/Region.hpp"

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

