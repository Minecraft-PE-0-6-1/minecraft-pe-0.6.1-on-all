#include <plugins/PluginsManager.hpp>
#include <plugins/LuaPlayer.hpp>
#include <plugins/LuaServer.hpp>

#include <client/Minecraft.h>
#include <filesystem>
#include "MessageEvent.hpp"

namespace fs = std::filesystem;

int PluginsManager::emitCommands(std::string command, const RakNet::RakNetGUID& source) {
    std::vector<std::string> args;
    std::string cmd;

    int i = 0;
    std::string word;
    std::stringstream ss(command);

    while (ss >> word) {
        if (i == 0) {
            cmd = word;
        } else {
            args.push_back(word);
        }
        
        i++;
    }

    auto it = m_luaCommands.find(cmd);

    if (it == m_luaCommands.end()) return 1;

    for (auto& callback : it->second) {
        sol::protected_function_result result = callback(LuaPlayer(source), sol::as_table(args));

        if (!result.valid()) {
            sol::error err = result;
            std::cout << err.what() << std::endl;
        }
    }

    return 0;
}

void PluginsManager::init(Minecraft& minecraft) {
    m_lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::io);
    registerTypes();

    m_minecraft = &minecraft;    
}

void PluginsManager::registerTypes() {
    // @note you cant just do that, because lua doesnt have access to the class in this case
    // m_lua.set_function("subscribe", &PluginsManager::subscribe);

    m_lua.set_function("subscribe", [this](std::string event, sol::protected_function func) {
        m_callbacks[event].push_back(func);
    });

    m_lua.set_function("addCommand", [this](std::string commandName, sol::function func) {
        m_luaCommands[commandName].push_back(func);
    });

    m_lua.new_usertype<LuaServer>(
        "Server",
        "sendMessage", &LuaServer::sendServerMessage
    );

    m_lua.new_usertype<LuaPlayer>(
        "Player",
        "getNickname", &LuaPlayer::getNickname,
        "getPos", &LuaPlayer::getPos,
        "setPos", &LuaPlayer::setPos,
        "sendMessage", &LuaPlayer::sendMessage,
        "getInventory", &LuaPlayer::getInventory,
        "addItem", &LuaPlayer::addItem,
        "getItem", &LuaPlayer::getItem,
        "clearInventory", &LuaPlayer::clearInventory,
        "getHp", &LuaPlayer::getHp,
        "setHp", &LuaPlayer::setHp
    );

    m_lua.new_usertype<MessageEvent>(
        "MessageEvent",
        "setDecline", &MessageEvent::setDecline
    );

    m_lua["Server"] = &m_srv;
}

void PluginsManager::loadPlugins() {
    std::string path = "plugins/";

    if (fs::exists(path)) {
        for (const auto& entry : fs::directory_iterator(path)) {
            if (!fs::is_directory(entry.path().string())) {
                m_lua.script_file(entry.path().string());        
            }
        }
    } else {
        std::filesystem::create_directory(path);
    }
}