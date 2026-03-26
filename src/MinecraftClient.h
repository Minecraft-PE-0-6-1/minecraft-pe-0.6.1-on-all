#pragma once
#include "client/gui/Font.h"
#include "client/gui/Screen.h"
#include "client/particle/ParticleEngine.h"
#include "client/player/LocalPlayer.h"
#include "client/renderer/GameRenderer.h"
#include "client/renderer/Textures.h"
#include "client/sound/SoundEngine.h"
#include <Minecraft.h>

#include <client/MouseHandler.h>
#include <client/gui/Gui.h>
#include <client/gui/screens/ScreenChooser.h>
#include <client/PixelCalc.h>
#include <client/renderer/LevelRenderer.h>

class MinecraftClient : public Minecraft {
public:
    using Minecraft::Minecraft;
    ~MinecraftClient();

    void init() override;

    void update() override;

    void setSize(int width, int height);
    void reloadOptions();
    
    bool supportNonTouchScreen();
	bool useTouchscreen();
	void grabMouse();
	void releaseMouse();

    void setScreen(Screen*);
    void leaveGame(bool renameLevel = false);

	void setLevel(Level* level, const std::string& message = "", Player* forceInsertPlayer = NULL) override;

    void updateStats();

    void restartServer() override;

    bool handleBack(bool isDown) override;

    void onGraphicsReset();
    void initGLStates();

    void tick(int nTick, int maxTick) override;
    void tickInput();

    void handleBuildAction(BuildActionIntention* action);

    bool isOnlineClient() override;

    void pauseGame(bool isBackPaused);
    void gameLostFocus();

    void respawnPlayer();

    void audioEngineOn() override;
    void audioEngineOff() override;

    void setIsCreativeMode(bool isCreative) override;

    void optionUpdated(OptionId option, bool value) override;
	void optionUpdated(OptionId option, float value) override;
	void optionUpdated(OptionId option, int value) override;

    LocalPlayer* getPlayer() { return player; }
    Font* getFont() { return font; }
    Textures& textures() { return m_textures; }
    Options& options() { return m_options;}
    Screen* getScreen() { return m_screen; }
    Gui& gui() { return m_gui; }
    ParticleEngine* getParticleEngine() {return particleEngine; }

    int getScreenWidth() { return width; }
    int getScreenHeigth() { return height; }

    virtual void hostMultiplayer(int port) override;

    bool isPowerVR() { return _powerVr; }
	bool isKindleFire(int kindleVersion);
	bool transformResolution(int* w, int* h);

    virtual std::string getServerName() override;

    void locateMultiplayer();
	void cancelLocateMultiplayer();
	bool joinMultiplayer(const PingedCompatibleServer& server);
	bool joinMultiplayerFromString(const std::string& server);

    void onBlockDestroyed(Player* player, int x, int y, int z, int face) override;

protected:
    void _reloadInput();
    void _levelGenerated() override;

    int width = 1, height = 1;

	Font* font = nullptr;
    // @warn This is unsafe cuz Gui may call some MinecraftClient method while MinecraftClient is not ready 
    MouseHandler mouseHandler;

    LevelRenderer*  levelRenderer = nullptr;
	GameRenderer*   gameRenderer = nullptr;
	ParticleEngine* particleEngine = nullptr;
	SoundEngine*    soundEngine = nullptr;
    PerfRenderer* _perfRenderer = nullptr;

	bool mouseGrabbed = false;

    PixelCalc pixelCalc;
    PixelCalc pixelCalcUi;

    Screen* m_screen = nullptr;

    bool screenMutex = false;
	bool hasScheduledScreen = false;
	Screen* scheduledScreen = nullptr;

    int m_frames = 0;

    volatile bool pause = false;

    // @todo make static
    LocalPlayer*	player = nullptr;
	IInputHolder*	inputHolder = nullptr;
	Mob*			cameraTargetPlayer = nullptr;

    bool _supportsNonTouchscreen = false;
    bool isLookingForMultiplayer = false;
    bool _powerVr = false;

    Options m_options{*this};

    Textures m_textures{m_options, *m_platform};

    ScreenChooser screenChooser{*this};

    Gui m_gui{*this};
};