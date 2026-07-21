#include <plugins/LuaServer.hpp>
#include <plugins/PluginsManager.hpp>
#include <network/ServerSideNetworkHandler.h>
#include <client/Minecraft.h>

void LuaServer::sendServerMessage(std::string msg)  {
    auto mc = PluginsManager::get().getMinecraft();
    ServerSideNetworkHandler* ss = (ServerSideNetworkHandler*) mc->netCallback;

    ss->displayGameMessage(msg);
}