#include "Command.hpp"

class CommandOp : public Command {
public:
    CommandOp();

    std::string execute(Minecraft& mc, Player& player, const std::vector<std::string>& args);
    std::string help(Minecraft& mc);
};