#include "IConfigListener.hpp"
#include <MinecraftClient.hpp>

#include "gui/Gui.hpp"

Config::Config(MinecraftClient& mc) : minecraft(mc), options(mc.options()), pixelCalc(mc.pixelCalc()), pixelCalcUi(mc.pixelCalcUi()) {}

Config createConfig(MinecraftClient& mc) {
	Config c(mc);
	c.setScreenSize(mc.getScreenWidth(), mc.getScreenHeight(), Gui::GuiScale);
	return c;
}
