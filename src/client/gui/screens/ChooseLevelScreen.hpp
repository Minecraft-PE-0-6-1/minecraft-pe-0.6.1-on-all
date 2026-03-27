#pragma once

#include "client/gui/Screen.hpp"
#include "world/level/storage/LevelStorageSource.hpp"

class ChooseLevelScreen: public Screen
{
public:
	void init();

protected:
	std::string getUniqueLevelName(const std::string& level);

private:
	void loadLevelSource();

	LevelSummaryList levels;
};

