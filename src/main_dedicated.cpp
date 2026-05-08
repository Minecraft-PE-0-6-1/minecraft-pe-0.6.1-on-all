#include <fstream>
#include <iostream>
#include "NinecraftApp.h"
#include "AppPlatform.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "world/level/LevelSettings.h"
#include "world/level/Level.h"
#include "server/ArgumentsSettings.h"
#include "platform/time.h"
#include "SharedConstants.h"

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
            throw std::runtime_error("Cannot end line");
        }

        valueStr += sym;
        valuePos++;
    }

    return valueStr;
}


int main(int numArguments, char* pszArgs[]) {
	ArgumentsSettings aSettings(numArguments, pszArgs);
	// if(aSettings.getShowHelp()) {
	// 	// TODO: Map with args and print it with std::cout and for loop
	// 	// TODO: World size setting

	// 	ArgumentsSettings defaultSettings(0, NULL);

	// 	printf("Minecraft Pockect Edition Server %s\n", Common::getGameVersionString("").c_str());
	// 	printf("-------------------------------------------------------\n");

	// 	printf("--cachepath - Path to where the server can store temp stuff (not sure if this is used) [default: \"%s\"]\n", defaultSettings.getCachePath().c_str());
	// 	printf("--externalpath - The path to the place where the server should store the levels. [default: \"%s\"]\n", defaultSettings.getExternalPath().c_str());
	// 	printf("--levelname - The name of the server [default: \"%s\"]\n", defaultSettings.getLevelName().c_str());
	// 	printf("--gamemode - The name of the gamemode [default: \"%s\"]\n", defaultSettings.getGamemode().c_str());
	// 	printf("--leveldir - The name of the server [default: \"%s\"]\n", defaultSettings.getLevelDir().c_str());
	// 	printf("--help - Shows this message.\n");
	// 	printf("--port - The port to run the server on. [default: %d]\n", defaultSettings.getPort());
	// 	printf("--serverkey - The key that the server should use for API calls. [default: \"%s\"]\n", defaultSettings.getServerKey().c_str());

	// 	printf("-------------------------------------------------------\n");

	// 	return 0;
	// }
	
	std::ifstream serverProperties("server.properties");

	int port = 0;
	int gamemode = 0;

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
		
		printf("%d %s %s %d \n", port, cachePath.c_str(), externalPath.c_str(), gamemode);
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

			printf("Edit a config file first!");

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

	std::cout << "Level has been generated in " << getTimeS() - startTime << std::endl;
	((MAIN_CLASS*)g_app)->level->saveLevelData();
	std::cout << "Level has been saved!" << std::endl;

	while(!app->wantToQuit()) {
		app->update();
		//pthread_yield();
		sleepMs(20);
	}
	
	((MAIN_CLASS*)g_app)->level->saveLevelData();
	delete app;
	appContext.platform->finish();
	delete appContext.platform;

	std::cout << "Quit correctly" << std::endl;
	return g_exitCode;
}