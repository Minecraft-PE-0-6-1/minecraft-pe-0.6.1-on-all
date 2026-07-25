#pragma once
#include <cstdint>
#include <raknet/RakNetTypes.h>
#include <plugins/PluginsManager.hpp>
#include <network/ServerSideNetworkHandler.h>
#include <world/entity/player/Player.h>
#include <client/Minecraft.h>
#include <network/packet/ChatPacket.h>
#include <network/packet/MovePlayerPacket.h>
#include <world/entity/player/Inventory.h>

class LuaPlayer {
public:
    LuaPlayer(const RakNet::RakNetGUID source) : m_source(source) {}

    Player* getPlayer() {
        auto mc = PluginsManager::get().getMinecraft();
        ServerSideNetworkHandler* ss = (ServerSideNetworkHandler*) mc->netCallback;
        auto plr = ss->getPlayer(m_source);

        return plr;
    }

    std::string getNickname() {
        auto plr = getPlayer();
        if (plr != nullptr) return plr->name;

        return "";
    }

    std::tuple<float, float, float, float, float> getPos() {
        auto plr = getPlayer();
        if (plr != nullptr) return {plr->x, plr->y, plr->z, plr->yRot, plr->xRot};

        return {0, 0, 0, 0, 0};
    }

    void setPos(float x, float y, float z, float yRot, float xRot) {
        auto plr = getPlayer();
        if (plr == nullptr) return;
        
        auto mc = PluginsManager::get().getMinecraft();
        ServerSideNetworkHandler* ss = (ServerSideNetworkHandler*) mc->netCallback;


        // plr->x = x;
        // plr->y = y;
        // plr->z = z;
        // plr->yRot = yRot;
        // plr->xRot = xRot;
        plr->xd = plr->yd = plr->zd = 0;	
		plr->lerpTo(x, y, z, yRot, xRot, 3);
		
        MovePlayerPacket move(plr->entityId, x, y, z, yRot, xRot);

        ss->sendPrivate(move, m_source);
    }

    void sendMessage(std::string msg) {
        auto mc = PluginsManager::get().getMinecraft();
        ServerSideNetworkHandler* ss = (ServerSideNetworkHandler*) mc->netCallback;
        ChatPacket packet(msg);

        ss->sendPrivate(packet, m_source);
    }

    void addItem(int id, int count, int auxValue) {
        auto plr = getPlayer();
        if (plr == nullptr) return;

        plr->inventory->add(new ItemInstance(id, count, auxValue));
    }

    sol::table getInventory() {
        auto plr = getPlayer();
        if (plr == nullptr) return {};

        sol::table inv = PluginsManager::get().getLua().create_table();

		for (int i = Inventory::MAX_SELECTION_SIZE; i < plr->inventory->getContainerSize(); i++) {
			auto itm = plr->inventory->getItem(i);

			if (itm != NULL) {
                inv[i - 8] = {itm->id, itm->count, itm->getAuxValue()};
			} else {
                inv[i - 8] = sol::lua_nil;
            }
		}

        return inv;
    }
private:
    const RakNet::RakNetGUID m_source;
};