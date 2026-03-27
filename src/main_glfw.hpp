#pragma once

#include "App.hpp"
#include "NinecraftApp.hpp"
#include "client/renderer/entity/PlayerRenderer.hpp"
#include "client/renderer/gles.hpp"
#include "GLFW/glfw3.hpp"

#include <cstdio>
#include <chrono>
#include <thread>
#include "platform/input/Keyboard.hpp"
#include "platform/input/Mouse.hpp"
#include "platform/input/Multitouch.hpp"
#include "AppPlatform_glfw.hpp"
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

// void loop() {
// 	using clock = std::chrono::steady_clock;
// 	auto frameStart = clock::now();

// 	g_app->update();

// 	glfwSwapBuffers(((AppPlatform_glfw*)g_app->platform())->window);
// 	glfwPollEvents();

// 	glfwSwapInterval(((MAIN_CLASS*)g_app)->options.getBooleanValue(OPTIONS_VSYNC) ? 1 : 0);
// 	if(((MAIN_CLASS*)g_app)->options.getBooleanValue(OPTIONS_LIMIT_FRAMERATE)) {
// 		auto frameEnd = clock::now();
// 		auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(frameEnd - frameStart);
// 		auto target = std::chrono::microseconds(33333); // ~30 fps
// 		if(elapsed < target)
// 			std::this_thread::sleep_for(target - elapsed);
// 	}
// }

int main(void) {
	AppContext appContext;

	appContext.platform = new AppPlatformGlfw();

	AppPlatformGlfw* platform = (AppPlatformGlfw*)appContext.platform;

	if (!platform->init()) {
		return 1;
	}

	App* app = (App*)new NinecraftApp();
	app->init(appContext);
	app->setSize(platform->getScreenWidth(), platform->getScreenHeight());

	auto loop = [app]() {
		app->update();
	};

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(loop, 0, 1);
#else
	// Main event loop
	while(!app->wantToQuit()) {
		loop();
	}
#endif

	delete app;

	appContext.platform->finish();
	
	delete appContext.platform;

	return 0;
}

