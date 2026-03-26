#include "MinecraftServer.h"
#include <Minecraft.h>
#include <network/RakNetInstance.h>


void MinecraftServer::hostMultiplayer(int port) {
    Minecraft::hostMultiplayer(port);

    raknetInstance->host("Server", port, 16);
}

std::string MinecraftServer::getServerName() {
	// @todo read server name from config
    return "Dedicated server 0.6.1";
}