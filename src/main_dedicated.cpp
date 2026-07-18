#include <fstream>
#include <iostream>
#include "NinecraftApp.h"
#include "AppPlatform.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#ifdef __linux__
#include <unistd.h>
#endif

#include "world/level/LevelSettings.h"
#include "world/level/Level.h"
#include "server/ArgumentsSettings.h"
#include "platform/time.h"
#include "SharedConstants.h"
#include "world/level/LevelConstants.h"

#define MAIN_CLASS NinecraftApp
static App* g_app = 0;
static int g_exitCode = 0;
void signal_callback_handler(int signum) {
	std::cout << "Signum caught: " << signum << std::endl;
	if(signum == 2 || signum == 3){ // SIGINT ||  SIGQUIT

		if(g_app != 0) {
			g_app->quit();
		} else {
			exit(g_exitCode);
		}
	}
}

std::string findStringInConfig(std::string line, std::string config) {
    auto valuePos = config.find(line);

    if (line.empty()) {
        throw std::runtime_error("Key cannot be empty");
    }

    if (valuePos == std::string::npos) {
        throw std::runtime_error("Cannot find value");
    }
 
    std::string valueStr = "";

    valuePos += 1 + line.size();

    while (true) {
        auto sym = config.at(valuePos);

        if (sym == '\n') {
            break;
        }

        if (valuePos == config.size()) {
            throw std::runtime_error("Cannot find end of the line");
        }

        valueStr += sym;
        valuePos++;
    }

    return valueStr;
}


int main(int numArguments, char* pszArgs[]) {	
	std::ifstream serverProperties("server.properties");

	int port = 0;
	int gamemode = 0;
	int worldSize = 0;

	std::string levelDir = "";
	std::string cachePath = "";
	std::string externalPath = "";
	std::string levelName = "";

	if (serverProperties.is_open()) {
		std::string propString;
        std::string line;
        
        while (std::getline(serverProperties, line)) {
            propString += line;
			propString += "\n";
        }

        if (propString.empty()) {
            throw std::runtime_error("Config cannot be empty");
        }

		port = std::stoi(findStringInConfig("port", propString));
		worldSize = std::stoi(findStringInConfig("world-size", propString));

		if (worldSize < 16 || worldSize > 32) {
			throw std::runtime_error("Incorrect world size.");
		}

		LevelConstants::CHUNK_CACHE_WIDTH = worldSize;
		LevelConstants::LEVEL_WIDTH = LevelConstants::CHUNK_CACHE_WIDTH * LevelConstants::CHUNK_WIDTH;
		LevelConstants::LEVEL_DEPTH = LevelConstants::CHUNK_CACHE_WIDTH * LevelConstants::CHUNK_DEPTH;
		
		cachePath = findStringInConfig("level-cachepath", propString);
		externalPath = findStringInConfig("level-externalpath", propString);
		levelName = findStringInConfig("level-name", propString);
		levelDir = findStringInConfig("level-dir", propString);

		std::string gamemodeStr = findStringInConfig("gamemode", propString);

		if (gamemodeStr == "survival") {
			gamemode = GameType::Survival;
		} else if (gamemodeStr == "creative") {
			gamemode = GameType::Creative;
		} else {
			gamemode = GameType::Survival;
		}
		
		printf("%d %s %s %d %d\n", port, cachePath.c_str(), externalPath.c_str(), gamemode, LevelConstants::CHUNK_CACHE_WIDTH);
	} else {
		std::ofstream defaultProperties("server.properties");

		if (defaultProperties.is_open()) {
			ArgumentsSettings defaultSettings(0, NULL);

			std::stringstream buffer;
			
			buffer << "// MCPE Server properties" << std::endl;
			buffer << "port=" << defaultSettings.getPort() << std::endl;
			buffer << "level-dir=" << defaultSettings.getLevelDir() << std::endl;
			buffer << "level-cachepath=" << defaultSettings.getCachePath() << std::endl;
			buffer << "level-externalpath=" << defaultSettings.getExternalPath() << std::endl;
			buffer << "gamemode=survival" << std::endl;
			buffer << "level-name=" << defaultSettings.getLevelName() << std::endl;
			buffer << "world-size=16" << std::endl;

			defaultProperties << buffer.str();

			printf("%s \n", buffer.str().c_str());

			printf("Edit a config file first!\n");

			defaultProperties.close();
			exit(0);
		} else {
			throw std::runtime_error("Cannot create default config.");
		}
	}

	printf("Level Name: %s\n", levelName.c_str());

	AppContext appContext;
	appContext.platform = new AppPlatform();
	App* app = new MAIN_CLASS();
	signal(SIGINT, signal_callback_handler);
	g_app = app;
	((MAIN_CLASS*)g_app)->externalStoragePath = externalPath;
	((MAIN_CLASS*)g_app)->externalCacheStoragePath = cachePath;

	g_app->init(appContext);
	LevelSettings settings(getEpochTimeS(), gamemode);
	float startTime = getTimeS();
	((MAIN_CLASS*)g_app)->selectLevel(levelDir, levelName, settings);
	((MAIN_CLASS*)g_app)->hostMultiplayer(port);

	// Reading ops
	std::ifstream ops("ops.txt");

	if (ops.is_open()) {
        std::string line;
        
        while (std::getline(ops, line)) {
			if (!line.empty()) {
            	((MAIN_CLASS*)g_app)->level->ops.insert(line);
			}
        }
	} else {
		std::ofstream opsDefault("ops.txt");

		if (!opsDefault.is_open()) {
			throw std::runtime_error("Cannot create ops list.");
		}
	}

	// Reading banned ppl
	std::ifstream banned("banned-players.txt");

	if (banned.is_open()) {
        std::string line;
        
        while (std::getline(banned, line)) {
			if (!line.empty()) {
            	((MAIN_CLASS*)g_app)->level->bannedPpl.insert(line);
			}
        }
	} else {
		std::ofstream bannedPpl("banned-players.txt");

		if (!bannedPpl.is_open()) {
			throw std::runtime_error("Cannot create banned players list.");
		}
	}

	std::cout << "Level has been generated in " << getTimeS() - startTime << std::endl;
	((MAIN_CLASS*)g_app)->level->saveLevelData();
	std::cout << "Level has been saved!" << std::endl;

	while(!app->wantToQuit()) {
		app->update();
		//pthread_yield();
		sleepMs(20);
	}
	
	std::ofstream opsWrite("ops.txt"); 

	if (opsWrite.is_open()) {
		for (auto& op : ((MAIN_CLASS*)g_app)->level->ops) {
			opsWrite << op << std::endl;
		}
		opsWrite.close();
	} else {
		throw std::runtime_error("Cannot open ops list.");
	}

	std::ofstream bannedWrite("banned-players.txt"); 

	if (bannedWrite.is_open()) {
		for (auto& banned : ((MAIN_CLASS*)g_app)->level->bannedPpl) {
			bannedWrite << banned << std::endl;
		}
		bannedWrite.close();
	} else {
		throw std::runtime_error("Cannot open banned players list.");
	}
	
	((MAIN_CLASS*)g_app)->level->saveLevelData();

	delete app;
	appContext.platform->finish();
	delete appContext.platform;

	std::cout << "Quit correctly" << std::endl;
	return g_exitCode;
}