#pragma once

#include "../Screen.h"
#include "../../../world/level/storage/LevelStorageSource.h"

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

