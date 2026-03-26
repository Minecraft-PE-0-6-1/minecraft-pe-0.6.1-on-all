#pragma once
#include <vector>
#include <string>
#include "client/renderer/TextureData.h"
#include <cstdint>

typedef std::vector<std::string> StringVector;
typedef std::vector<uint8_t> ByteVector; 

class App;

class IPlatform {
public:
	IPlatform() : keyboardVisible(false), windowSizeChanged(false), m_targetFrametime(0.f) {}
    virtual ~IPlatform() {}

	virtual bool init() { return true; }
	virtual void swapBuffers() {}

	virtual void runMainLoop(App& app);

	virtual ByteVector readAssetFile(const std::string& path);


	// Mojang functions here
	virtual TextureData loadTexture(const std::string& filename_, bool textureFolder) { return TextureData(); }
	virtual TextureData loadTextureFromMemory(const unsigned char* data, size_t size) { return TextureData(); }

    virtual void playSound(const std::string& fn, float volume, float pitch) {}
	
	virtual void hideCursor(bool hide) {}

	virtual std::string getDateString(int s) = 0;

	virtual void uploadPlatformDependentData(int id, void* data) {}
	// virtual BinaryBlob readAssetFile(const std::string& filename) { return BinaryBlob(); }
	virtual void _tick() {}

	virtual int getScreenWidth() { return 854; }
	virtual int getScreenHeight() { return 480; }
    virtual float getPixelsPerMillimeter() { return 10; }

	virtual bool isNetworkEnabled(bool onlyWifiAllowed) { return true; }

	virtual bool isPowerVR() {
		return false;
	}
	virtual int getKeyFromKeyCode(int keyCode, int metaState, int deviceId) {return 0;}
#ifdef __APPLE__
    virtual bool isSuperFast() = 0;
#endif

	virtual void openURL(const std::string& url) {}

	virtual void finish() {}
	
	virtual bool supportsTouchscreen() { return false; }
	
	virtual void vibrate(int milliSeconds) {}

	virtual std::string getPlatformStringVar(int stringId) = 0;

	virtual void showKeyboard() { keyboardVisible = true; }

	virtual void hideKeyboard() { keyboardVisible = false; }

	virtual bool isKeyboardVisible() { return keyboardVisible; }

	virtual void setTargetFPS(int fps) { m_targetFrametime = 1.0 / fps; }

	bool isWindowSizeChanged() { return windowSizeChanged; }

	virtual void setVSync(bool on) { vsync = on; }

protected:
	bool keyboardVisible;
	bool windowSizeChanged;
	bool vsync;

	double m_targetFrametime;
};
