#include "Options.hpp"
#include "world/Difficulty.hpp"
#include <MinecraftClient.hpp>

bool Options::debugGl = false;

// OPTIONS TABLE

OptionInt difficulty("difficulty", Difficulty::NORMAL, 0, Difficulty::COUNT);
OptionBool hidegui("hidegui", false);
OptionBool thirdPersonView("thirdperson", false);
OptionBool renderDebug("renderDebug", false);
OptionBool smoothCamera("smoothCamera", false);
OptionBool fixedCamera("fixedCamera", false);
OptionBool isFlying("isflying", false);
OptionBool barOnTop("barOnTop", false);
OptionBool allowSprint("allowSprint", true);
OptionBool rpiCursor("rpiCursor", false);
OptionBool autoJump("autoJump", true);


OptionFloat flySpeed("flySpeed", 1.f);
OptionFloat cameraSpeed("cameraSpeed", 1.f);

OptionInt guiScale("guiScale", 0, 0, 5);

OptionString skin("skin", "Default");

#ifdef RPI
OptionString username("username", "StevePi");
#else 
OptionString username("username", "Steve");
#endif

OptionBool destroyVibration("destroyVibration", true);
OptionBool isLeftHanded("isLeftHanded", false);
OptionBool isJoyTouchArea("isJoyTouchArea", false);

OptionFloat musicVolume("music", 1.f, MUSIC_MIN_VALUE, MUSIC_MAX_VALUE);
OptionFloat soundVolume("sound", 1.f, SOUND_MIN_VALUE, SOUND_MAX_VALUE);

OptionFloat sensitivityOpt("sensitivity", 0.5f, SENSITIVITY_MIN_VALUE, SENSITIVITY_MAX_VALUE);

OptionBool invertYMouse("invertMouse", false);
OptionInt viewDistance("renderDistance", 2, 0, 4);

OptionBool anaglyph3d("anaglyph3d", false);
OptionBool limitFramerate("limitFramerate", false);
OptionBool vsync("vsync", true);
OptionBool fancyGraphics("fancyGraphics", true);
OptionBool viewBobbing("viewBobbing", true);
OptionBool ambientOcclusion("ao", false);

OptionBool useTouchscreen("useTouchscreen", true);

OptionBool serverVisible("servervisible", true);

OptionInt keyForward("key.forward", Keyboard::KEY_W);
OptionInt keyLeft("key.left", Keyboard::KEY_A);
OptionInt keyBack("key.back", Keyboard::KEY_S);
OptionInt keyRight("key.right", Keyboard::KEY_D);
OptionInt keyJump("key.jump", Keyboard::KEY_SPACE);
OptionInt keyInventory("key.inventory", Keyboard::KEY_E);
OptionInt keySneak("key.sneak", Keyboard::KEY_LSHIFT);
OptionInt keyDrop("key.drop", Keyboard::KEY_Q);
OptionInt keyChat("key.chat", Keyboard::KEY_T);
OptionInt keyFog("key.fog", Keyboard::KEY_F);
OptionInt keyUse("key.use", Keyboard::KEY_U);

// TODO: make human readable keycodes here
OptionInt keyMenuNext("key.menu.next", 40);
OptionInt keyMenuPrev("key.menu.previous", 38);
OptionInt keyMenuOk("key.menu.ok", 13);
OptionInt keyMenuCancel("key.menu.cancel", 8);

OptionBool firstLaunch("firstLaunch", true);

OptionString lastIp("lastip");

void Options::initTable() {
    m_options[OPTIONS_DIFFICULTY] = &difficulty;
    m_options[OPTIONS_HIDEGUI] = &hidegui;
    m_options[OPTIONS_THIRD_PERSON_VIEW] = &thirdPersonView;
    m_options[OPTIONS_RENDER_DEBUG] = &renderDebug;
    m_options[OPTIONS_SMOOTH_CAMERA] = &smoothCamera;
    m_options[OPTIONS_FIXED_CAMERA] = &fixedCamera;
	m_options[OPTIONS_IS_FLYING] = &isFlying;

	m_options[OPTIONS_FLY_SPEED] = &flySpeed;
	m_options[OPTIONS_CAMERA_SPEED] = &cameraSpeed;

	m_options[OPTIONS_GUI_SCALE] = &guiScale;

	m_options[OPTIONS_DESTROY_VIBRATION] = &destroyVibration;

	m_options[OPTIONS_IS_LEFT_HANDED] = &isLeftHanded;
	m_options[OPTIONS_IS_JOY_TOUCH_AREA] = &isJoyTouchArea;

	m_options[OPTIONS_MUSIC_VOLUME] = &musicVolume;
	m_options[OPTIONS_SOUND_VOLUME] = &soundVolume;

	#if defined(PLATFORM_DESKTOP) || defined(RPI)
		float sensitivity = sensitivityOpt.get();
		sensitivity *= 0.4f;
		sensitivityOpt.set(sensitivity);
	#endif


    m_options[OPTIONS_GUI_SCALE] = &guiScale;

	m_options[OPTIONS_SKIN] = &skin;
	m_options[OPTIONS_USERNAME] = &username;

    m_options[OPTIONS_DESTROY_VIBRATION] = &destroyVibration;
    m_options[OPTIONS_IS_LEFT_HANDED] = &isLeftHanded;

    m_options[OPTIONS_MUSIC_VOLUME] = &musicVolume;
    m_options[OPTIONS_SOUND_VOLUME] = &soundVolume;

    m_options[OPTIONS_SENSITIVITY] = &sensitivityOpt;

    m_options[OPTIONS_INVERT_Y_MOUSE] = &invertYMouse;
    m_options[OPTIONS_VIEW_DISTANCE] = &viewDistance;

    m_options[OPTIONS_ANAGLYPH_3D] = &anaglyph3d;
    m_options[OPTIONS_LIMIT_FRAMERATE] = &limitFramerate;
    m_options[OPTIONS_VSYNC] = &vsync;
    m_options[OPTIONS_FANCY_GRAPHICS] = &fancyGraphics;
	m_options[OPTIONS_VIEW_BOBBING] = &viewBobbing;
	m_options[OPTIONS_AMBIENT_OCCLUSION] = &ambientOcclusion;

    m_options[OPTIONS_USE_TOUCHSCREEN] = &useTouchscreen;

    m_options[OPTIONS_SERVER_VISIBLE] = &serverVisible;

    m_options[OPTIONS_KEY_FORWARD] = &keyForward;
    m_options[OPTIONS_KEY_LEFT] = &keyLeft;
    m_options[OPTIONS_KEY_BACK] = &keyBack;
    m_options[OPTIONS_KEY_RIGHT] = &keyRight;
    m_options[OPTIONS_KEY_JUMP] = &keyJump;
    m_options[OPTIONS_KEY_INVENTORY] = &keyInventory;
    m_options[OPTIONS_KEY_SNEAK] = &keySneak;
    m_options[OPTIONS_KEY_DROP] = &keyDrop;
    m_options[OPTIONS_KEY_CHAT] = &keyChat;
    m_options[OPTIONS_KEY_FOG] = &keyFog;
    m_options[OPTIONS_KEY_USE] = &keyUse;

    m_options[OPTIONS_KEY_MENU_NEXT] = &keyMenuNext;
    m_options[OPTIONS_KEY_MENU_PREV] = &keyMenuPrev;
    m_options[OPTIONS_KEY_MENU_OK] = &keyMenuOk;
    m_options[OPTIONS_KEY_MENU_CANCEL] = &keyMenuCancel;

	m_options[OPTIONS_FIRST_LAUNCH] = &firstLaunch;

	m_options[OPTIONS_BAR_ON_TOP] = &barOnTop;
	m_options[OPTIONS_ALLOW_SPRINT] = &allowSprint;
	m_options[OPTIONS_RPI_CURSOR] = &rpiCursor;

	m_options[OPTIONS_AUTOJUMP] = &autoJump;
	m_options[OPTIONS_LAST_IP] = &lastIp;
}

void Options::set(OptionId key, const std::string& value) {
	auto option = opt<OptionString>(key);

	if (option) {
		option->set(value);
		notifyOptionUpdate(key, value);
	}
}

void Options::set(OptionId key, float value) {
	auto option = opt<OptionFloat>(key);

	if (option) {
		option->set(value);
		notifyOptionUpdate(key, value);
	}
}

void Options::set(OptionId key, int value) {
	auto option = opt<OptionInt>(key);

	if (option) {
		option->set(value);
		notifyOptionUpdate(key, value);
	}
}

void Options::toggle(OptionId key) {
	auto option = opt<OptionBool>(key);

	if (option) {
		option->toggle();
		notifyOptionUpdate(key, option->get());
	}
}

void Options::load() {
	StringVector optionStrings = optionsFile.getOptionStrings();

	for (auto i = 0; i < optionStrings.size(); i += 2) {
		const std::string& key = optionStrings[i];
		const std::string& value = optionStrings[i+1];

		// FIXME: woah this is so slow 
		auto opt = std::find_if(m_options.begin(), m_options.end(), [&](auto& it) {
			return it != nullptr && it->getStringId() == key;
		});

		if (opt == m_options.end()) continue;

		(*opt)->parse(value);
/*
        // //LOGI("reading key: %s (%s)\n", key.c_str(), value.c_str());
        
		// // Multiplayer
		// // if (key == OptionStrings::Multiplayer_Username) username = value;
		// if (key == OptionStrings::Multiplayer_ServerVisible) {
		// 	m_options[OPTIONS_SERVER_VISIBLE] = readBool(value);
		// }

		// // Controls
        // if (key == OptionStrings::Controls_Sensitivity) {
		// 	float sens = readFloat(value);

		// 	// sens is in range [0,1] with default/center at 0.5 (for aesthetics)
        //     // We wanna map it to something like [0.3, 0.9] BUT keep 0.5 @ ~0.5...
        //     m_options[OPTIONS_SENSITIVITY] = 0.3f + std::pow(1.1f * sens, 1.3f) * 0.42f;
        // }

		// if (key == OptionStrings::Controls_InvertMouse) {
		// 	m_options[OPTIONS_INVERT_Y_MOUSE] = readBool(value);
		// }

		// if (key == OptionStrings::Controls_IsLefthanded) {
		// 	m_options[OPTIONS_IS_LEFT_HANDED] = readBool(value);
		// }
		
		// if (key == OptionStrings::Controls_UseTouchJoypad) {
		// 	m_options[OPTIONS_IS_JOY_TOUCH_AREA] = readBool(value) && minecraft.useTouchscreen();
		// }

		// // Feedback
		// if (key == OptionStrings::Controls_FeedbackVibration) {
		// 	m_options[OPTIONS_DESTROY_VIBRATION] = readBool(value);
		// }

		// // Graphics
		// if (key == OptionStrings::Graphics_Fancy) {
		// 	m_options[OPTIONS_FANCY_GRAPHICS] = readBool(value);
		// }

		// // Graphics extras
		// if (key == OptionStrings::Graphics_Vsync) {
		// 	m_options[OPTIONS_VSYNC] = readBool(value);
		// }

		// if (key == OptionStrings::Graphics_GUIScale) {
		// 	m_options[OPTIONS_GUI_SCALE] = readInt(value) % 5;
		// }

		// // Game
		// if (key == OptionStrings::Game_DifficultyLevel) {
		// 	readInt(value, difficulty);
		// 	// Only support peaceful and normal right now
		// 	if (difficulty != Difficulty::PEACEFUL && difficulty != Difficulty::NORMAL)
		// 		difficulty = Difficulty::NORMAL;
		// }*/
	}
}

void Options::save() {
	StringVector stringVec;
	
	for (auto& it : m_options) {
		if (it) stringVec.push_back(it->serialize());
	}

	optionsFile.save(stringVec);
}

void Options::setOptionsFilePath(const std::string& path) {
	optionsFile.setOptionsPath(path + "/options.txt");
}

void Options::notifyOptionUpdate(OptionId key, bool value) {
	minecraft.optionUpdated(key, value);
}

void Options::notifyOptionUpdate(OptionId key, float value) {
	minecraft.optionUpdated(key, value);
}

void Options::notifyOptionUpdate(OptionId key, int value) {
	minecraft.optionUpdated(key, value);
}
