#pragma once
#include <unordered_map>
#include <sol/sol.hpp>
#include <vector>

class Level;

class PluginsManager {
public:
    PluginsManager(Level& level);

    void registerTypes();

    void subscribe(std::string event, sol::function function);
private:
    std::unordered_map<std::string, std::vector<sol::function>> m_callbacks;

    sol::state m_lua;
    Level& m_level;
};