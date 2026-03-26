#pragma once
#include <IPlatform.h>

class PlatformServer : public IPlatform {
public:
	void runMainLoop(App& app) override;

	std::string getDateString(int s) override;

	int getScreenWidth() override { return 0; }
	int getScreenHeight() override { return 0; }
	float getPixelsPerMillimeter() override { return 0.f; }
	bool supportsTouchscreen() override { return false; }

	std::string getPlatformStringVar(int stringId) override { return "Minecraft PE 0.6.1 Standalone Server"; }
};
