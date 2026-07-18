#include "Command.hpp"

class CommandKick : public Command {
public:
    CommandKick();

    std::string execute(Minecraft& mc, Player& player, const std::vector<std::string>& args);
    std::string help(Minecraft& mc);
};