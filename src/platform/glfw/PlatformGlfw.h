#pragma once

#include <IPlatform.h>
#include "platform/log.h"
#include "platform/HttpClient.h"
#include "platform/PngLoader.h"
#include "client/renderer/gles.h"
#include "world/level/storage/FolderMethods.h"
#include <png.h>
#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
	#include <emscripten/html5.h>
#endif

class PlatformGlfw : public IPlatform {
public:
	bool init() override;
	void finish() override;

    TextureData loadTexture(const std::string& filename_, bool textureFolder) override;

	TextureData loadTextureFromMemory(const unsigned char* data, size_t size) override { return loadPngFromMemory(data, size); }

	std::string getDateString(int s) override;

	int getScreenWidth() override { 
		#ifdef __EMSCRIPTEN__
			int w, h;
			emscripten_get_canvas_element_size("canvas", &w, &h);

			return w;
		#endif

		return 854; 
	};

	int getScreenHeight() override { 
		#ifdef __EMSCRIPTEN__
			int w, h;
			emscripten_get_canvas_element_size("canvas", &w, &h);

			return h;
		#endif

		return 480; 
	};

	float getPixelsPerMillimeter() override;

	bool supportsTouchscreen() override { return false; /* glfw supports only mouse and keyboard */ }

	void hideCursor(bool hide) override;

	void openURL(const std::string& url) override;

	void swapBuffers() override;

	void setVSync(bool on) override;

private:
	static int transformKey(int glfwkey);

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void character_callback(GLFWwindow* window, unsigned int codepoint);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void window_size_callback(GLFWwindow* window, int width, int height);
	static void error_callback(int error, const char* desc);

	GLFWwindow* m_window;
};
