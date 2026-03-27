#pragma once

enum ScreenId {
	SCREEN_NONE,
	SCREEN_STARTMENU,
	SCREEN_JOINGAME,
	SCREEN_PAUSE,
	SCREEN_PAUSEPREV,
	SCREEN_SELECTWORLD,
	SCREEN_BLOCKSELECTION,
	SCREEN_JOINBYIP,
	SCREEN_CONSOLE
};

class Screen;
class MinecraftClient;

class ScreenChooser
{
public:
	ScreenChooser(MinecraftClient& mc) : _mc(mc) {}

	Screen* createScreen(ScreenId id);
	Screen* setScreen(ScreenId id);
private:
	MinecraftClient& _mc;
};

