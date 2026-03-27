#pragma once

//package net.minecraft.world.level;

namespace GameType {
	const int Undefined = -1;
	const int Survival = 0;
	const int Creative = 1;

	const int Default = Creative;
}

class LevelSettings
{
public:
    LevelSettings(long seed, int gameType, bool allowCheats = false)
    :   seed(seed),
        gameType(gameType),
        allowCheats(allowCheats)
    {
    }
	static LevelSettings None() {
		return LevelSettings(-1,-1,false);
	}

    long getSeed() const {
        return seed;
    }

    int getGameType() const {
        return gameType;
    }

    bool getAllowCheats() const {
        return allowCheats;
    }

	//
	// Those two should actually not be here
	// @todo: Move out when we add LevelSettings.cpp :p
	//
	static int validateGameType(int gameType) {
        switch (gameType) {
		case GameType::Creative:
		case GameType::Survival:
            return gameType;
        }
        return GameType::Default;
    }

	static std::string gameTypeToString(int gameType) {
		if (gameType == GameType::Survival) return "Survival";
		if (gameType == GameType::Creative) return "Creative";
		return "Undefined";
	}

private:
    const long seed;
    const int gameType;
    const bool allowCheats;
};

