#pragma once
#include <string>
#include <vector>

class Minecraft;
class Player;

class Command {
public:
    const std::string& getName() { return m_name; }

    bool isPlayerOp(Minecraft& mc, Player& player);

    virtual std::string execute(Minecraft& mc, Player& player, const std::vector<std::string>& args) = 0;
    virtual std::string help(Minecraft& mc) = 0;

protected:
    Command(const std::string& name) : m_name(name) {}

    const std::string m_name;
};