#pragma once

#include "../world/level/Level.h"

class ServerLevel: public Level
{
	typedef Level super;
public:
	ServerLevel(LevelStorage* levelStorage, const std::string& levelName, const LevelSettings& settings, int generatorVersion, Dimension* fixedDimension = NULL);

	void updateSleepingPlayerList();
	void awakenAllPlayers();
	bool allPlayersSleeping();
	void tick();
protected:
	bool allPlayersAreSleeping;
};

