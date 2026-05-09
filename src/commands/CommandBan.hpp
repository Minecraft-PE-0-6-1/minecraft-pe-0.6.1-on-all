#include "Command.hpp"

class CommandBan : public Command {
public:
    CommandBan();

    std::string execute(Minecraft& mc, Player& player, const std::vector<std::string>& args);
    std::string help(Minecraft& mc);
};