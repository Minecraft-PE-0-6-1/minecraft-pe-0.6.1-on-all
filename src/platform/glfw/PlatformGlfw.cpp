#include "PlatformGlfw.hpp"
#include <IPlatform.hpp>
#include "platform/input/Keyboard.hpp"
#include "platform/input/Mouse.hpp"

#include "platform/log.hpp"
#include <GLFW/glfw3.h>
#include <png.h>

#include "platform/HttpClient.hpp"
#include "util/StringUtils.hpp"
#include <fstream>
#include <ctime>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#endif

#include "App.hpp"


bool PlatformGlfw::init() {
    glfwSetErrorCallback(PlatformGlfw::error_callback);

    if (!glfwInit()) {
		return false;
	}

    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
#ifndef __EMSCRIPTEN__
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    m_window = glfwCreateWindow(getScreenWidth(), getScreenHeight(), "Minecraft PE 0.6.1", NULL, NULL);

    if (m_window == nullptr) {
		return false;
	}

    glfwSetKeyCallback(m_window, key_callback);
	glfwSetCharCallback(m_window, character_callback);
	glfwSetCursorPosCallback(m_window, cursor_position_callback);
	glfwSetMouseButtonCallback(m_window, mouse_button_callback);
	glfwSetScrollCallback(m_window, scroll_callback);
	glfwSetWindowSizeCallback(m_window, window_size_callback);

    glfwSetWindowUserPointer(m_window, this);

	glfwMakeContextCurrent(m_window);

#ifndef __EMSCRIPTEN__
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(0);
#endif
}

void PlatformGlfw::finish() {
    glfwDestroyWindow(m_window);
	glfwTerminate();
}

int PlatformGlfw::transformKey(int glfwkey) {
	if (glfwkey >= GLFW_KEY_F1 && glfwkey <= GLFW_KEY_F12) {
		return glfwkey - 178;
	}

	switch (glfwkey) {
		case GLFW_KEY_ESCAPE: return Keyboard::KEY_ESCAPE;
		case GLFW_KEY_TAB: return Keyboard::KEY_TAB;
		case GLFW_KEY_BACKSPACE: return Keyboard::KEY_BACKSPACE;
		case GLFW_KEY_LEFT_SHIFT: return Keyboard::KEY_LSHIFT;
		case GLFW_KEY_ENTER: return Keyboard::KEY_RETURN;
		case GLFW_KEY_LEFT_CONTROL: return Keyboard::KEY_LEFT_CTRL;
		default: return glfwkey;
	}
}

void PlatformGlfw::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	static double lastX = 0.0, lastY = 0.0;
	static bool firstMouse = true;

	if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

	double deltaX = xpos - lastX;
    double deltaY = ypos - lastY;

    lastX = xpos;
    lastY = ypos;

	if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
		Mouse::feed(0, 0, xpos, ypos, deltaX, deltaY);
	} else { 
		Mouse::feed( MouseAction::ACTION_MOVE, 0, xpos, ypos);
	}

	// Multitouch::feed(0, 0, xpos, ypos, 0);
}

void PlatformGlfw::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if(action == GLFW_REPEAT) return;

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		Mouse::feed( MouseAction::ACTION_LEFT, action, xpos, ypos);
		// Multitouch::feed(1, action, xpos, ypos, 0);
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		Mouse::feed( MouseAction::ACTION_RIGHT, action, xpos, ypos);
	}
}

void PlatformGlfw::character_callback(GLFWwindow* window, unsigned int codepoint) {
	Keyboard::feedText(codepoint);
}

void PlatformGlfw::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	Mouse::feed(3, 0, xpos, ypos, 0, yoffset);
}

void PlatformGlfw::error_callback(int error, const char* desc) {
	LOGE("GLFW Error (%d): %s", error, desc);
}

void PlatformGlfw::window_size_callback(GLFWwindow* window, int width, int height) {
	PlatformGlfw* app = (PlatformGlfw*)glfwGetWindowUserPointer(window);

    app->windowSizeChanged = true;
}


float PlatformGlfw::getPixelsPerMillimeter() {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();

    int width_mm, height_mm;
    glfwGetMonitorPhysicalSize(monitor, &width_mm, &height_mm);

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    return (float)mode->width / (float)width_mm;
}

TextureData PlatformGlfw::loadTexture(const std::string& filename_, bool textureFolder) {
    // Support fetching PNG textures via HTTP/HTTPS (for skins, etc)
    if (Util::startsWith(filename_, "http://") || Util::startsWith(filename_, "https://")) {
        std::vector<unsigned char> body;
        if (HttpClient::download(filename_, body) && !body.empty()) {
            return loadTextureFromMemory(body.data(), body.size());
        }
        return TextureData();
    }

    TextureData out;

    std::string filename = textureFolder? "data/images/" + filename_
                            : filename_;
    std::ifstream source(filename.c_str(), std::ios::binary);

    if (!source) {
        LOGI("Couldn't find file: %s\n", filename.c_str());
        return out;
    }

    std::vector<unsigned char> fileData((std::istreambuf_iterator<char>(source)), std::istreambuf_iterator<char>());
    source.close();

    if (fileData.empty()) {
        LOGI("Couldn't read file: %s\n", filename.c_str());
        return out;
    }

    return loadTextureFromMemory(fileData.data(), fileData.size());
}

std::string PlatformGlfw::getDateString(int s) {
    time_t tm = s;

    char mbstr[100];
    std::strftime(mbstr, sizeof(mbstr), "%F %T", std::localtime(&tm));

    return std::string(mbstr);
}

void PlatformGlfw::hideCursor(bool hide) {
    int isHide = hide ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN;
    glfwSetInputMode(m_window, GLFW_CURSOR, isHide);
}

void PlatformGlfw::openURL(const std::string& url) {
#ifdef _WIN32
    ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
#elif __linux__
    std::string command = "xdg-open " + url;
    system(command.c_str());
#elif __EMSCRIPTEN__
    emscripten_run_script(std::string("window.open('" + url + "', '_blank')").c_str());
#endif
}

void PlatformGlfw::swapBuffers() {
    glfwSwapBuffers(m_window);
    glfwPollEvents();

    // @todo
    // if(((MAIN_CLASS*)g_app)->options.getBooleanValue(OPTIONS_LIMIT_FRAMERATE)) {
    //     auto frameEnd = clock::now();
    //     auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(frameEnd - frameStart);
    //     auto target = std::chrono::microseconds(33333); // ~30 fps
    //     if(elapsed < target)
    //         std::this_thread::sleep_for(target - elapsed);
    // }
}

void PlatformGlfw::setVSync(bool on) {
    IPlatform::setVSync(on);

    glfwSwapInterval(on);
}