#include "Command.hpp"

class CommandGimmieItems : public Command {
public:
    CommandGimmieItems();

    std::string execute(Minecraft& mc, Player& player, const std::vector<std::string>& args);
    std::string help(Minecraft& mc);
};