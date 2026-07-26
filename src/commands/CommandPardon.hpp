#include "Command.hpp"

class CommandPardon : public Command {
public:
    CommandPardon();

    std::string execute(Minecraft& mc, Player& player, const std::vector<std::string>& args);
    std::string help(Minecraft& mc);
};