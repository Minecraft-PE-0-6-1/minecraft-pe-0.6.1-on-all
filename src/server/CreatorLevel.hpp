#pragma once

#include "world/level/Level.hpp"

class CreatorLevel: public Level
{
	typedef Level super;
public:
	CreatorLevel(LevelStorage* levelStorage, const std::string& levelName, const LevelSettings& settings, int generatorVersion, Dimension* fixedDimension = NULL);

	void tick();
};

