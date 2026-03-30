#include "Command.hpp"

class CommandKick : public Command {
public:
    CommandKick();

    void execute(Minecraft& mc, const std::vector<std::string>& args);
    void printHelp(Minecraft& mc);
};