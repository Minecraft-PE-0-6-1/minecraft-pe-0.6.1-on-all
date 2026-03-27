#include "App.hpp"
#include "IPlatform.hpp"

#include "platform/server/PlatformServer.hpp"
#include "platform/glfw/PlatformGlfw.hpp"

std::unique_ptr<IPlatform> App::CreatePlatform() {
#if defined(STANDALONE_SERVER)
    return std::make_unique<PlatformServer>();
#elif defined(PLATFORM_DESKTOP)
    return std::make_unique<PlatformGlfw>();
#else 
    static_assert(false, "Unsupported platform!");
#endif
}

void App::run() {
    init();

    m_platform->runMainLoop(*this);
}

void App::swapBuffers() {
    m_platform->swapBuffers();
}

