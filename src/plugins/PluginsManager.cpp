#include <plugins/PluginsManager.hpp>
#include <plugins/LuaPlayer.hpp>
#include <plugins/LuaServer.hpp>

#include <client/Minecraft.h>
#include <filesystem>

namespace fs = std::filesystem;

void PluginsManager::init(Minecraft& minecraft) {
    m_lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::math, sol::lib::table);
    registerTypes();

    m_minecraft = &minecraft;    
}

void PluginsManager::registerTypes() {
    // @note you cant just do that, because lua doesnt have access to the class in this case
    // m_lua.set_function("subscribe", &PluginsManager::subscribe);

    m_lua.set_function("subscribe", [this](std::string event, sol::protected_function func) {
        // @note: why the .at() not working?
        m_callbacks[event].push_back(func);
    });
    
    m_lua.new_usertype<LuaServer>(
        "Server",
        "sendMessage", &LuaServer::sendServerMessage
    );

    m_lua.new_usertype<LuaPlayer>(
        "Player",
        "getNickname", &LuaPlayer::getNickname,
        "getX", &LuaPlayer::getX,
        "getY", &LuaPlayer::getY,
        "getZ", &LuaPlayer::getZ
    );

    m_lua["Server"] = &m_srv;
}

void PluginsManager::loadPlugins() {
    std::string path = "plugins/";

    for (const auto & entry : fs::directory_iterator(path)) {
        m_lua.script_file(entry.path().string());
    }
}