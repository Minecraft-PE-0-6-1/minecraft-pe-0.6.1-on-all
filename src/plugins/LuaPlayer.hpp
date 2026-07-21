#pragma once
#include <cstdint>
#include <raknet/RakNetTypes.h>
#include <plugins/PluginsManager.hpp>
#include <network/ServerSideNetworkHandler.h>
#include <world/entity/player/Player.h>
#include <client/Minecraft.h>

class LuaPlayer {
public:
    LuaPlayer(const RakNet::RakNetGUID source) : m_source(source) {}

    std::string getNickname() {
        auto mc = PluginsManager::get().getMinecraft();
        ServerSideNetworkHandler* ss = (ServerSideNetworkHandler*) mc->netCallback;

        auto plr = ss->getPlayer(m_source);
        if (plr != nullptr) return plr->name;

        return "";
    }

    float getX() {
        auto mc = PluginsManager::get().getMinecraft();
        ServerSideNetworkHandler* ss = (ServerSideNetworkHandler*) mc->netCallback;

        auto plr = ss->getPlayer(m_source);
        if (plr != nullptr) return plr->x;

        return 0;
    }

    float getY() {
        auto mc = PluginsManager::get().getMinecraft();
        ServerSideNetworkHandler* ss = (ServerSideNetworkHandler*) mc->netCallback;

        auto plr = ss->getPlayer(m_source);
        if (plr != nullptr) return plr->y;

        return 0;
    }

    float getZ() {
        auto mc = PluginsManager::get().getMinecraft();
        ServerSideNetworkHandler* ss = (ServerSideNetworkHandler*) mc->netCallback;

        auto plr = ss->getPlayer(m_source);
        if (plr != nullptr) return plr->z;

        return 0;
    }
private:
    const RakNet::RakNetGUID m_source;
};