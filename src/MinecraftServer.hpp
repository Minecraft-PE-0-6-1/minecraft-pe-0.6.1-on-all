#pragma once
#include <Minecraft.hpp>

class MinecraftServer : public Minecraft { 
public:
    using Minecraft::Minecraft;

    void hostMultiplayer(int port) override;
    std::string getServerName() override;
};