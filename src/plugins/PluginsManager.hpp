#pragma once
#include <unordered_map>
#include <sol/sol.hpp>
#include <vector>
#include <iostream>
#include <plugins/LuaServer.hpp>
#include <raknet/RakNetTypes.h>

class Minecraft;

class PluginsManager {
public:
    static inline PluginsManager& get() {
        static PluginsManager inst;
        return inst;
    }

    void init(Minecraft& minecraft);

    void registerTypes();

    void loadPlugins();

    sol::state& getLua() { return m_lua; }

    // @ai @note maybe we should rewrite it...
    // problem that i dont know how to pass args in sol::function dynamically
    // so i used ai
    // sry :(
    template<typename... Args>
    void emit(std::string event, Args&&... args) {
        auto it = m_callbacks.find(event);

        if (it == m_callbacks.end()) return;

        for (auto& callback : it->second) {
            sol::protected_function_result result = callback(std::forward<Args>(args)...);

            if (!result.valid()) {
                sol::error err = result;
                std::cout << err.what() << std::endl;
            }
        }
    }  

    int emitCommands(std::string command, const RakNet::RakNetGUID& source);
    
    Minecraft* getMinecraft() { return m_minecraft; }
private:
    std::unordered_map<std::string, std::vector<sol::function>> m_callbacks;
    std::unordered_map<std::string, std::vector<sol::function>> m_luaCommands;

    LuaServer  m_srv;
    sol::state m_lua;
    Minecraft* m_minecraft;
};