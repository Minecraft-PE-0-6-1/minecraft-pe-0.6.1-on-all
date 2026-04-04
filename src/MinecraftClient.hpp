#pragma once
#include "client/gui/Font.hpp"
#include "client/gui/Screen.hpp"
#include "client/particle/ParticleEngine.hpp"
#include "client/player/LocalPlayer.hpp"
#include "client/renderer/GameRenderer.hpp"
#include "client/renderer/Textures.hpp"
#include "client/sound/SoundEngine.hpp"
#include <Minecraft.hpp>

#include <client/MouseHandler.hpp>
#include <client/gui/Gui.hpp>
#include <client/gui/screens/ScreenChooser.hpp>
#include <client/PixelCalc.hpp>
#include <client/renderer/LevelRenderer.hpp>

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

    LocalPlayer* player() { return m_player; }
    Font* font() { return m_font; }
    Textures& textures() { return m_textures; }
    Options& options() { return m_options;}
    Screen* getScreen() { return m_screen; }
    Gui& gui() { return m_gui; }
    ParticleEngine* getParticleEngine() {return particleEngine; }

    int getScreenWidth() { return m_width; }
    int getScreenHeight() { return m_height; }

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

    ScreenChooser& screenChooser() { return m_screenChooser; }

    PixelCalc& pixelCalc() { return m_pixelCalc; }
    PixelCalc& pixelCalcUi() { return m_pixelCalcUi; }

    IInputHolder* inputHolder() { return m_inputHolder; }
    SoundEngine* soundEngine() { return m_soundEngine; }

protected:
    void _reloadInput();
    void _levelGenerated() override;

    int m_width = 1, m_height = 1;

	Font* m_font = nullptr;
    // @warn This is unsafe cuz Gui may call some MinecraftClient method while MinecraftClient is not ready 
    MouseHandler mouseHandler;

    LevelRenderer*  levelRenderer = nullptr;
	GameRenderer*   gameRenderer = nullptr;
	ParticleEngine* particleEngine = nullptr;
	SoundEngine*    m_soundEngine = nullptr;
    PerfRenderer* _perfRenderer = nullptr;

	bool mouseGrabbed = false;

    PixelCalc m_pixelCalc;
    PixelCalc m_pixelCalcUi;

    Screen* m_screen = nullptr;

    bool screenMutex = false;
	bool hasScheduledScreen = false;
	Screen* scheduledScreen = nullptr;

    int m_frames = 0;

    volatile bool pause = false;

    LocalPlayer*	m_player = nullptr;
	IInputHolder*	m_inputHolder = nullptr;
	Mob*			cameraTargetPlayer = nullptr;

    bool _supportsNonTouchscreen = false;
    bool isLookingForMultiplayer = false;
    bool _powerVr = false;

    Options m_options{*this};

    Textures m_textures{m_options, *m_platform};

    ScreenChooser m_screenChooser{*this};

    Gui m_gui{*this};
};