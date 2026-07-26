#include "Command.hpp"

class CommandBanIp : public Command {
public:
    CommandBanIp();

    std::string execute(Minecraft& mc, Player& player, const std::vector<std::string>& args);
    std::string help(Minecraft& mc);
};