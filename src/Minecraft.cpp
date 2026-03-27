#include <Minecraft.hpp>
#include "gamemode/CreativeMode.hpp"
#include "gamemode/SurvivalMode.hpp"
#include "gamemode/CreatorMode.hpp"
#include "world/entity/player/Player.hpp"
#include "world/item/Item.hpp"
#include "world/item/ItemInstance.hpp"
#include "world/item/crafting/Recipes.hpp"
#include "world/level/Level.hpp"
#include "world/level/tile/entity/TileEntity.hpp"
#include <string>
#include <cstdlib>
#include "client/gui/Screen.hpp"
#include "world/level/storage/ExternalFileLevelStorageSource.hpp"

#if defined(APPLE_DEMO_PROMOTION)
    #define NO_NETWORK
#endif

#if defined(RPI)
	#define CREATORMODE
#endif
#include "network/RakNetInstance.hpp"
#include "network/ClientSideNetworkHandler.hpp"
#include "network/ServerSideNetworkHandler.hpp"
//#include "network/Packet.hpp"
#include "world/entity/player/Inventory.hpp"
#include "world/level/tile/Tile.hpp"
#include "world/level/storage/LevelStorageSource.hpp"
#include "world/level/storage/LevelStorage.hpp"
#include "world/level/chunk/ChunkSource.hpp"

#include "platform/CThread.hpp"
#include <IPlatform.hpp>
#include "util/PerfTimer.hpp"
#include "util/PerfRenderer.hpp"

#include "world/entity/MobFactory.hpp"
#include "world/level/MobSpawner.hpp"
#include "util/Mth.hpp"
#include "world/entity/MobCategory.hpp"
#include "server/ServerLevel.hpp"

#ifdef CREATORMODE
#include "server/CreatorLevel.hpp"
#endif

#include "network/command/CommandServer.hpp"

/*static*/
const char* Minecraft::progressMessages[] = {
	"Locating server",
	"Building terrain",
	"Preparing",
	"Saving chunks"
};

// int Minecraft::customDebugId = Minecraft::CDI_NONE;
bool Minecraft::_hasInitedStatics = false;

#if defined(_MSC_VER)
	#pragma warning( disable : 4355 ) // 'this' pointer in initialization list which is perfectly legal
#endif

// Minecraft::Minecraft() :
// #ifdef __APPLE__
//     _isSuperFast(false),
// #endif


// #if defined(NO_NETWORK)
//     raknetInstance = new IRakNetInstance();
// #else
// 	raknetInstance = new RakNetInstance();
// #endif
// #ifndef STANDALONE_SERVER
// 	soundEngine = new SoundEngine(20.0f);
// 	soundEngine->init(this, &options);
// #endif
// 	//setupPieces();

// 	#if defined(ANDROID) || defined(__APPLE__) || defined(RPI)
//         signal(SIGPIPE, SIG_IGN);
//     #endif

// 	externalCacheStoragePath = '.';
// 	externalCacheStoragePath = '.';
// }

Minecraft::~Minecraft() {
	delete netCallback;
	delete raknetInstance;
	delete gameMode;

	if (level != NULL) {
		level->saveGame();
		if (level->getChunkSource())
			level->getChunkSource()->saveAll(true);
		delete level->getLevelStorage();
		delete level;
		level = NULL;
	}

	//delete player;
	delete storageSource;
	delete _commandServer;

	MobFactory::clearStaticTestMobs();

	// Note: Don't tear down statics if we run on Android
	// (we might change this in the future)
#ifndef ANDROID
	Biome::teardownBiomes();
	Item ::teardownItems();
	Tile ::teardownTiles();
	Material::teardownMaterials();
	Recipes ::teardownRecipes();
	TileEntity::teardownTileEntities();
#endif
}

// Only called by server
void Minecraft::selectLevel( const std::string& levelId, const std::string& levelName, const LevelSettings& settings ) {
	level = (Level*)new ServerLevel(
		storageSource->selectLevel(levelId, false),
		levelName,
		settings,
		SharedConstants::GeneratorVersion
	);

	// note: settings is useless beyond this point, since it's
	//       either copied to LevelData (or LevelData read from file)
	setLevel(level, "Generating level");
	setIsCreativeMode(level->getLevelData()->getGameType() == GameType::Creative);
	_running = true;
}

void Minecraft::setLevel(Level* level, const std::string& message, Player* forceInsertPlayer) {
	LOGI("Seed is %ld\n", level->getSeed());
	
	if (level != NULL) {
		level->raknetInstance = raknetInstance;
        gameMode->initLevel(level);

		this->level = level;
		_hasSignaledGeneratingLevelFinished = false;
#ifdef STANDALONE_SERVER
		const bool threadedLevelCreation = false;
#else
		const bool threadedLevelCreation = true;
#endif

		if (threadedLevelCreation) {
			// Threaded
			// "Lock"
			isGeneratingLevel = true;
			generateLevelThread = new CThread(Minecraft::prepareLevel_tspawn, this);
		} else {
			// Non-threaded
			generateLevel("Currently not used", level);
		}
    }

    this->lastTickTime = 0;
	this->_running = true;
}

void Minecraft::prepareLevel(const std::string& title) {
	LOGI("status: 1\n");
	progressStageStatusId = 1;

	Stopwatch A, B, C, D;
	A.start();

	Stopwatch L;

	// Dont update lights if we load the level (ok, actually just with leveldata version=1.+(?))
	if (!level->isNew())
		level->setUpdateLights(false);

	int Max = CHUNK_CACHE_WIDTH * CHUNK_CACHE_WIDTH;
	int pp = 0;
	for (int x = 8; x < (CHUNK_CACHE_WIDTH * CHUNK_WIDTH); x += CHUNK_WIDTH) {
        for (int z = 8; z < (CHUNK_CACHE_WIDTH * CHUNK_WIDTH); z += CHUNK_WIDTH) {
            progressStagePercentage = 100 * pp++ / Max;
            //printf("level generation progress %d\n", progressStagePercentage);
			B.start();
            level->getTile(x, 64, z);
			B.stop();
			L.start();
			if (level->isNew())
				while (level->updateLights())
					;
			L.stop();
        }
    }
	A.stop();
	level->setUpdateLights(true);

	C.start();
	for (int x = 0; x < CHUNK_CACHE_WIDTH; x++)
	{
		for (int z = 0; z < CHUNK_CACHE_WIDTH; z++)
		{
			LevelChunk* chunk = level->getChunk(x, z);
			if (chunk && !chunk->createdFromSave)
			{
				chunk->unsaved = false;
				chunk->clearUpdateMap();
			}
		}
	}
	C.stop();

	LOGI("status: 3\n");
	progressStageStatusId = 3;
	if (level->isNew()) {
		level->setInitialSpawn(); // @note: should obviously be called from Level itself
		level->saveLevelData();
		level->getChunkSource()->saveAll(false);
		level->saveGame();
	} else {
		level->saveLevelData();
		level->loadEntities();
	}

	progressStagePercentage = -1;
	progressStageStatusId = 2;
	LOGI("status: 2\n");

	D.start();
	level->prepare();
	D.stop();

	A.print("Generate level: ");
	L.print(" - light: ");
	B.print(" - getTl: ");
	C.print(" - clear: ");
	D.print(" - prepr: ");
	progressStageStatusId = 0;
}

void Minecraft::update() {

	timer.advanceTime();
	raknetInstance->runEvents(netCallback);

	TIMER_PUSH("tick");
	int toTick = timer.ticks;
	timer.ticks = 0;
	for (int i = 0; i < toTick; ++i, ++ticks) tick(i, toTick-1);

	TIMER_POP_PUSH("updatelights");
	{
		if (level && !isGeneratingLevel) {
			level->updateLights();
		}
	}
	TIMER_POP();


    // Restart the server if (our modded) RakNet reports an error
    if (level && raknetInstance->isProbablyBroken() && raknetInstance->isServer()) {
        restartServer();
    }
}

void Minecraft::restartServer() {
    if (!level) return;

	raknetInstance->resetIsBroken();

    hostMultiplayer();
    if (netCallback) netCallback->levelGenerated(level);
}

void Minecraft::tick(int nTick, int maxTick) {
	if (missTime > 0) missTime--;

	TIMER_PUSH("gameMode");
	if (level) {
		gameMode->tick();
	}

	TIMER_POP_PUSH("commandServer");
	if (level && _commandServer) {
		_commandServer->tick();
	}
	
	//
	// Ongoing level generation in a (perhaps) different thread. When it's
	// ready, _levelGenerated() is called once and any threads are deleted.
	//
	if (isGeneratingLevel) {
		return;
	} else if (!_hasSignaledGeneratingLevelFinished) {
		if (generateLevelThread) {
			delete generateLevelThread;
			generateLevelThread = NULL;
		}
		_levelGenerated();
	}

	//
	// Normal game loop, run before or efter level generation
	//
	if (level != NULL) {
		TIMER_POP_PUSH("level");
		level->tickEntities();
		level->tick();
	}

	TIMER_POP();
}

bool Minecraft::isOnlineClient() {
    return false;
}

bool Minecraft::isOnline() {
	return netCallback != NULL;
}

void Minecraft::init()
{
	// WHY DO WE NEED THIS ON MODERN PLATFORMS :sob:
	// Global initialization goes here
	Mth::initMth();

	if (raknetInstance != nullptr) {
		delete raknetInstance;
	}

	raknetInstance = new RakNetInstance();

	// If we're running Android, only initialize
	// the first time class is instanced
#ifdef ANDROID
	if (!_hasInitedStatics) {
		_hasInitedStatics = true;
#endif
		Material::initMaterials();
		MobCategory::initMobCategories();
		Tile::initTiles();
		Item::initItems();
		Biome::initBiomes();
		TileEntity::initTileEntities();

#ifdef ANDROID
	}
#endif

	setIsCreativeMode(false); // false means it's Survival Mode

#if !defined(NO_STORAGE)
	storageSource = new ExternalFileLevelStorageSource(externalStoragePath, externalCacheStoragePath);
#else
	storageSource = new MemoryLevelStorageSource();
#endif

	// Server-only featire @todo server class app
	hostMultiplayer();
}


//
// Multiplayer
//

void Minecraft::hostMultiplayer(int port) {
    // Tear down last instance
    raknetInstance->disconnect();
    delete netCallback;
    netCallback = nullptr;

#if !defined(NO_NETWORK)
	netCallback = new ServerSideNetworkHandler(this, raknetInstance);
#endif
}

//
// Level generation
//
/*static*/

void* Minecraft::prepareLevel_tspawn(void *p_param) {
	Minecraft* mc = (Minecraft*) p_param;
	mc->generateLevel("Currently not used", mc->level);
	return 0;
}

void Minecraft::generateLevel( const std::string& message, Level* level ) {
	Stopwatch s;
	s.start();
	prepareLevel(message);
	s.stop();
	s.print("Level generated: ");

	// "Unlock"
	isGeneratingLevel = false;
}

void Minecraft::_levelGenerated() {
	level->validateSpawn();

	if (raknetInstance->isServer())
		raknetInstance->announceServer(getServerName());

	if (netCallback) {
		netCallback->levelGenerated(level);
	}

	_hasSignaledGeneratingLevelFinished = true;
}

Player* Minecraft::respawnPlayer(int playerId) {
	for (unsigned int i = 0; i < level->players.size(); ++i) {
		if (level->players[i]->entityId == playerId) {
			resetPlayer(level->players[i]);
			return level->players[i];
		}
	}
	return NULL;
}

void Minecraft::resetPlayer(Player* player) {
	level->validateSpawn();
	player->reset();

	Pos p;
	if(player->hasRespawnPosition()) {
		p = player->getRespawnPosition();
	}
	else {
		p = level->getSharedSpawnPos();
	}
	player->setPos((float)p.x + 0.5f, (float)p.y + 1.0f, (float)p.z + 0.5f);
	player->resetPos(true);

	if (isCreativeMode())
		player->inventory->clearInventoryWithDefault();
}

int Minecraft::getProgressStatusId() {
	return progressStageStatusId;
}

const char* Minecraft::getProgressMessage()
{
	return progressMessages[progressStageStatusId];
}

bool Minecraft::isLevelGenerated()
{
	return level != NULL && !isGeneratingLevel;
}

LevelStorageSource* Minecraft::getLevelSource()
{
	return storageSource;
}

void Minecraft::setIsCreativeMode(bool isCreative)
{
#ifdef CREATORMODE
	delete gameMode;
	gameMode = new CreatorMode(this);
	_isCreativeMode = true;
#else
	if (!gameMode || isCreative != _isCreativeMode)
	{
		delete gameMode;
		if (isCreative) gameMode = new CreativeMode(*this);
		else			gameMode = new SurvivalMode(*this);
		_isCreativeMode = isCreative;
	}
#endif
}

bool Minecraft::isCreativeMode() {
	return _isCreativeMode;
}

ICreator* Minecraft::getCreator()
{
#ifdef CREATORMODE
	return ((CreatorMode*)gameMode)->getCreator();
#else
	return NULL;
#endif
}

void Minecraft::optionUpdated(OptionId option, bool value ) {
	if(netCallback != NULL && option == OPTIONS_SERVER_VISIBLE) {
		ServerSideNetworkHandler* ss = (ServerSideNetworkHandler*) netCallback;
		ss->allowIncomingConnections(value);
	}
}

void Minecraft::optionUpdated(OptionId option, float value ) {}

void Minecraft::optionUpdated(OptionId option, int value ) {}
