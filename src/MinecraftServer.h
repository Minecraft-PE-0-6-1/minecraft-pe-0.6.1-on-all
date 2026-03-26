#pragma once
#include <Minecraft.h>

class MinecraftServer : public Minecraft { 
public:
    using Minecraft::Minecraft;

    void hostMultiplayer(int port) override;
    std::string getServerName() override;
};