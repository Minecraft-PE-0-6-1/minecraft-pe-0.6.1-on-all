#include "ScreenChooser.hpp"
#include "StartMenuScreen.hpp"
#include "MinecraftClient.hpp"
#include "SelectWorldScreen.hpp"
#include "JoinGameScreen.hpp"
#include "PauseScreen.hpp"
#include "RenameMPLevelScreen.hpp"
#include "ConsoleScreen.hpp"
#include "IngameBlockSelectionScreen.hpp"
#include "JoinByIPScreen.hpp"
#include "touch/TouchStartMenuScreen.hpp"
#include "touch/TouchSelectWorldScreen.hpp"
#include "touch/TouchJoinGameScreen.hpp"
#include "touch/TouchIngameBlockSelectionScreen.hpp"

#include "client/Minecraft.hpp"

#include <client/gui/screens/UsernameScreen.hpp>

Screen* ScreenChooser::createScreen( ScreenId id )
{
	Screen* screen = NULL;

	// :sob:
	if (/* _mc->useTouchscreen() */ true) {
		switch (id) {
		case SCREEN_STARTMENU:	     screen = new Touch::StartMenuScreen();	break;
		case SCREEN_SELECTWORLD:     screen = new Touch::SelectWorldScreen();break;
		case SCREEN_JOINGAME:	     screen = new Touch::JoinGameScreen();	break;
		case SCREEN_PAUSE:	         screen = new PauseScreen(false); break;
		case SCREEN_PAUSEPREV:	     screen = new PauseScreen(true);	 break;
		case SCREEN_BLOCKSELECTION:	 screen = new Touch::IngameBlockSelectionScreen();	break;
		case SCREEN_JOINBYIP:        screen = new JoinByIPScreen(); break;
		case SCREEN_CONSOLE:		 screen = new ConsoleScreen(); break;
		case SCREEN_NONE:
		default:
			// Do nothing
			break;
		}
	} else {
		switch (id) {
		case SCREEN_STARTMENU:	     screen = new StartMenuScreen();  break;
		case SCREEN_SELECTWORLD:     screen = new SelectWorldScreen();break;
		case SCREEN_JOINGAME:	     screen = new JoinGameScreen();   break;
		case SCREEN_PAUSE:	         screen = new PauseScreen(false); break;
		case SCREEN_PAUSEPREV:	     screen = new PauseScreen(true);	 break;
		case SCREEN_BLOCKSELECTION:	 screen = new IngameBlockSelectionScreen();	break;
		case SCREEN_JOINBYIP:        screen = new JoinByIPScreen(); break;
		case SCREEN_CONSOLE:		 screen = new ConsoleScreen(); break;
		case SCREEN_NONE:
		default:
			// Do nothing
			break;
		}
	}
	return screen;
}

Screen* ScreenChooser::setScreen(ScreenId id)
{
	Screen* screen = createScreen(id);
	_mc.setScreen(screen);
	return screen;
}
