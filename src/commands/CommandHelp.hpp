#include "Command.hpp"

class CommandHelp : public Command {
public:
    CommandHelp();

    void execute(Minecraft& mc, const std::vector<std::string>& args);
    void printHelp(Minecraft& mc);
};