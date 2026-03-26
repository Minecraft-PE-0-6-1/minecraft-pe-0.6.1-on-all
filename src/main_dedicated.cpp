#include <iostream>
#include <IPlatform.h>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "MinecraftServer.h"
#include "platform/server/PlatformServer.h"
#include "world/level/LevelSettings.h"
#include "world/level/Level.h"
#include "server/ArgumentsSettings.h"
#include "platform/time.h"
#include "SharedConstants.h"

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

int main(int numArguments, char* pszArgs[]) {
	ArgumentsSettings aSettings(numArguments, pszArgs);

	if(aSettings.getShowHelp()) {
		ArgumentsSettings defaultSettings(0, NULL);
		printf("Minecraft Pockect Edition Server %s\n", Common::getGameVersionString("").c_str());
		printf("-------------------------------------------------------\n");
		printf("--cachepath - Path to where the server can store temp stuff (not sure if this is used) [default: \"%s\"]\n", defaultSettings.getCachePath().c_str());
		printf("--externalpath - The path to the place where the server should store the levels. [default: \"%s\"]\n", defaultSettings.getExternalPath().c_str());
		printf("--levelname - The name of the server [default: \"%s\"]\n", defaultSettings.getLevelName().c_str());
		printf("--leveldir - The name of the server [default: \"%s\"]\n", defaultSettings.getLevelDir().c_str());
		printf("--help - Shows this message.\n");
		printf("--port - The port to run the server on. [default: %d]\n", defaultSettings.getPort());
		printf("--serverkey - The key that the server should use for API calls. [default: \"%s\"]\n", defaultSettings.getServerKey().c_str());
		printf("-------------------------------------------------------\n");
		return 0;
	}

	printf("Level Name: %s\n", aSettings.getLevelName().c_str());

	MinecraftServer server(App::CreatePlatform());

	signal(SIGINT, signal_callback_handler);
	g_app = &server;

	server.externalStoragePath = aSettings.getExternalPath();
	server.externalCacheStoragePath = aSettings.getCachePath();

	server.init();
	LevelSettings settings(getEpochTimeS(), GameType::Creative);
	float startTime = getTimeS();
	server.selectLevel(aSettings.getLevelDir(), aSettings.getLevelName(),  settings);
	server.hostMultiplayer(aSettings.getPort());

	std::cout << "Level has been generated in " << getTimeS() - startTime << std::endl;
	server.level->saveLevelData();
	std::cout << "Level has been saved!" << std::endl;

	while(!server.wantToQuit()) {
		server.update();
		//pthread_yield();
		sleepMs(20);
	}

	server.level->saveLevelData();

	std::cout << "Quit correctly" << std::endl;
	return g_exitCode;
}