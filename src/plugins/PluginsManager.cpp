#include <plugins/PluginsManager.hpp>
#include <world/level/Level.h>

PluginsManager::PluginsManager(Level& level) : m_level(level) { 
    m_lua.open_libraries(sol::lib::base);

    registerTypes();
}

void PluginsManager::registerTypes() {
    m_lua.set_function("subscribe", &PluginsManager::subscribe);
}

void PluginsManager::subscribe(std::string event, sol::function function) {
    m_callbacks.at(event).push_back(function);
}