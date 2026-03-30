#include "Command.hpp"

class CommandKick : public Command {
public:
    CommandKick();

    void execute(Minecraft& mc, Player& player, const std::vector<std::string>& args);
    void printHelp(Minecraft& mc);
};