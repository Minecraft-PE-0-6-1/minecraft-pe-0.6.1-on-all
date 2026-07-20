#ifndef NET_MINECRAFT_CLIENT__Options_H__
#define NET_MINECRAFT_CLIENT__Options_H__

#define SOUND_MIN_VALUE 0.0f
#define SOUND_MAX_VALUE 1.0f
#define MUSIC_MIN_VALUE 0.0f
#define MUSIC_MAX_VALUE 1.0f
#define SENSITIVITY_MIN_VALUE 0.0f
#define SENSITIVITY_MAX_VALUE 1.0f
#define PIXELS_PER_MILLIMETER_MIN_VALUE 3.0f
#define PIXELS_PER_MILLIMETER_MAX_VALUE 4.0f

//package net.minecraft.client;

//#include "locale/Language.h"

#include <string>
#include <cstdio>
#include "../platform/input/Keyboard.h"
#include "../util/StringUtils.h"
#include "OptionsFile.h"
#include "Option.h"
#include <array>

enum OptionId {
    // General
    OPTIONS_DIFFICULTY,
    OPTIONS_HIDEGUI,
    OPTIONS_THIRD_PERSON_VIEW,
    OPTIONS_GUI_SCALE,
    OPTIONS_DESTROY_VIBRATION,
    OPTIONS_MUSIC_VOLUME,
    OPTIONS_SOUND_VOLUME,
    OPTIONS_SKIN,
    OPTIONS_USERNAME,
    OPTIONS_SERVER_VISIBLE,
    OPTIONS_BAR_ON_TOP,
    OPTIONS_ALLOW_SPRINT,
    OPTIONS_AUTOJUMP,
	OPTIONS_WINDOW_SCALE,

    // Graphics
    OPTIONS_RENDER_DEBUG,
    OPTIONS_SMOOTH_CAMERA,
    OPTIONS_FIXED_CAMERA,
    OPTIONS_VIEW_DISTANCE,
    OPTIONS_VIEW_BOBBING,
    OPTIONS_AMBIENT_OCCLUSION,
    OPTIONS_ANAGLYPH_3D,
    OPTIONS_LIMIT_FRAMERATE,
    OPTIONS_VSYNC,
    OPTIONS_FANCY_GRAPHICS,
	OPTIONS_NORMAL_LIGHTING,


    // Cheats / debug
    OPTIONS_FLY_SPEED,
    OPTIONS_CAMERA_SPEED,
    OPTIONS_IS_FLYING,

    // Control
	OPTIONS_BLOCK_OUTLINE,
    OPTIONS_USE_MOUSE_FOR_DIGGING,
    OPTIONS_IS_LEFT_HANDED,
    OPTIONS_IS_JOY_TOUCH_AREA,
    OPTIONS_SENSITIVITY,
    OPTIONS_INVERT_Y_MOUSE,
    OPTIONS_USE_TOUCHSCREEN,

    OPTIONS_KEY_FORWARD,
    OPTIONS_KEY_LEFT,
    OPTIONS_KEY_BACK,
    OPTIONS_KEY_RIGHT,
    OPTIONS_KEY_JUMP,
    OPTIONS_KEY_INVENTORY,
    OPTIONS_KEY_SNEAK,
    OPTIONS_KEY_DROP,
    OPTIONS_KEY_CHAT,
    OPTIONS_KEY_FOG,
    OPTIONS_KEY_USE,

    OPTIONS_KEY_MENU_NEXT,
    OPTIONS_KEY_MENU_PREV,
    OPTIONS_KEY_MENU_OK,
    OPTIONS_KEY_MENU_CANCEL,

    OPTIONS_FIRST_LAUNCH,
    OPTIONS_LAST_IP,

    OPTIONS_RPI_CURSOR,
#ifndef __APPLE__
	OPTIONS_FOLIAGE_TINT,
#endif
    OPTIONS_FOG_TYPE,
	OPTIONS_JAVA_HUD,
	OPTIONS_RESTORED_ANIMS,
#ifndef __APPLE__
	OPTIONS_TINTED_SIDE,
#endif
	OPTIONS_BETA_SKY,
	OPTIONS_BEAUTIFUL_SKY,
	OPTIONS_VIGNETTE,
	OPTIONS_DEBUG_STYLE,
	OPTIONS_COMPLETE_LIGHTING,
	OPTIONS_MENU_STYLE,
	// Should be last!
	OPTIONS_COUNT
};

class Minecraft;
typedef std::vector<std::string> StringVector;

class Options
{
public:
    // deepfriedwaffles: for iOS, was getting compile errors saying: No member named 'sound' in 'Options' and No member named 'music' in 'Options' so I floated them here. 1.0f means full volume out of the box, but if everything is too loud, you might want to try adjusting this
    float sound = 1.0f;
    float music = 1.0f;
    
    static bool debugGl;

    Options(Minecraft* minecraft, const std::string& workingDirectory = "") 
	: minecraft(minecraft) {
        // elements werent initialized so i was getting a garbage pointer and a crash
        m_options.fill(nullptr);
        initTable();
	    // load() is deferred to init() where path is configured correctly
    }

    void initTable();

    int getIntValue(OptionId key) {
        auto option = opt<OptionInt>(key);
        return (option)? option->get() : 0;
    }

    std::string getStringValue(OptionId key) {
        auto option = opt<OptionString>(key);
        return (option)? option->get() : "";
    }

    float getProgressValue(OptionId key) {
        auto option = opt<OptionFloat>(key);
        return (option)? option->get() : 0.f;
    }

    bool getBooleanValue(OptionId key) {
        auto option = opt<OptionBool>(key);
        return (option)? option->get() : false;
    }

    float getProgrssMin(OptionId key) {
        auto option = opt<OptionFloat>(key);
        return (option)? option->getMin() : 0.f;
    }

    float getProgrssMax(OptionId key) {
        auto option = opt<OptionFloat>(key);
        return (option)? option->getMax() : 0.f;
    }

    Option* getOpt(OptionId id) { return m_options[id]; }

    void load();
    void save();
    void set(OptionId key, int value);
    void set(OptionId key, float value);
    void set(OptionId key, const std::string& value);
	void setOptionsFilePath(const std::string& path);
	void toggle(OptionId key);

	void notifyOptionUpdate(OptionId key, bool value);
	void notifyOptionUpdate(OptionId key, float value);
	void notifyOptionUpdate(OptionId key, int value);
    void notifyOptionUpdate(OptionId key, const std::string& value) {}

private:
    template<typename T>
    T* opt(OptionId key) { 
        if (m_options[key] == nullptr) return nullptr;
        return dynamic_cast<T*>(m_options[key]); 
    }

	std::array<Option*, OPTIONS_COUNT> m_options;
	OptionsFile optionsFile;

	Minecraft* minecraft;
};

#endif /*NET_MINECRAFT_CLIENT__Options_H__*/
