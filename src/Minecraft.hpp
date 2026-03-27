#pragma once

#include "client/Options.hpp"
#include "client/Timer.hpp"

//#include "../network/RakNetInstance.hpp"
#include "world/phys/HitResult.hpp"

#include "App.hpp"
#include <cstddef>

class Level;
class LocalPlayer;
class IInputHolder;
class Mob;
class Player;
class Entity;
class ICreator;
class GameMode;
class CThread;
class LevelStorageSource;
class BuildActionIntention;
class PerfRenderer;
class LevelSettings;
class IRakNetInstance;
class NetEventCallback;
class CommandServer;
struct PingedCompatibleServer;

class Minecraft: public App {
public:
	using App::App;
	virtual ~Minecraft();

	virtual void init();
	virtual void update();
	virtual void restartServer();
	virtual void tick(int nTick, int maxTick);

	virtual bool isOnlineClient();
	virtual bool isOnline();

	virtual void setIsCreativeMode(bool isCreative);

	virtual void optionUpdated(OptionId option, bool value);
	virtual void optionUpdated(OptionId option, float value);
	virtual void optionUpdated(OptionId option, int value);

	/**
	 * @brief Get public name that will be listed in JoinGame menu
	 */
	virtual std::string getServerName() { return "Unknown"; }

	void toggleDimension() {}
	bool isCreativeMode();

	virtual void selectLevel(const std::string& levelId, const std::string& levelName, const LevelSettings& settings);
	virtual void setLevel(Level* level, const std::string& message = "", Player* forceInsertPlayer = NULL);

	virtual void onBlockDestroyed(Player* player, int x, int y, int z, int face) {}

	void generateLevel( const std::string& message, Level* level );
	LevelStorageSource* getLevelSource();

	virtual void hostMultiplayer(int port=19132);
	Player* respawnPlayer(int playerId);
	void respawnPlayer();
	void resetPlayer(Player* player);
	void doActuallyRespawnPlayer();

	void prepareLevel(const std::string& message);

	int getProgressStatusId();
	const char* getProgressMessage();

	ICreator* getCreator();

	bool isLevelGenerated();

#ifdef __APPLE__
    bool _isSuperFast = false;
    bool isSuperFast() { return _isSuperFast; }
#endif

protected:
	virtual void _levelGenerated();

private:
	static void* prepareLevel_tspawn(void *p_param);
public:
	Level* level = nullptr;
	CommandServer* _commandServer = nullptr;
	GameMode* gameMode = nullptr;
	IRakNetInstance*  raknetInstance = nullptr;
	NetEventCallback* netCallback = nullptr;

	int commandPort = 4711;

	int lastTime = 0;
	int lastTickTime = -1;
	int missTime = 0;
	int ticks = 0;

	CThread* generateLevelThread = nullptr;
	// static int customDebugId;

	HitResult hitResult;
	volatile int progressStagePercentage = 0;

	// This field is initialized in main()
	// It sets the base path to where worlds can be written (sdcard on android)
	std::string externalStoragePath;
	std::string externalCacheStoragePath;
protected:
	Timer timer{20};
    // @note @attn @warn: this is dangerous as fuck!
	volatile bool isGeneratingLevel = false;
	bool _hasSignaledGeneratingLevelFinished = true;

	LevelStorageSource* storageSource;
	bool _running;

protected:
	volatile int progressStageStatusId = 0;
	static const char* progressMessages[];

	int _licenseId;

	bool _isCreativeMode;
	Player* _pendingRemovePlayer; // @attn @todo @fix: remove this shait and fix the respawn behaviour

	// from NinecraftApp
	bool _verbose = true;
	int _lastTickMs = 0;

	static bool _hasInitedStatics;
};

