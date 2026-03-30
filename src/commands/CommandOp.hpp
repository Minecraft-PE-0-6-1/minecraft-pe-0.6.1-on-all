#include "Command.hpp"

class CommandOp : public Command {
public:
    CommandOp();

    void execute(Minecraft& mc, Player& player, const std::vector<std::string>& args);
    void printHelp(Minecraft& mc);
};