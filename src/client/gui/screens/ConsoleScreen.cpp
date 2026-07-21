#include "ConsoleScreen.h"
#include "../Gui.h"
#include "../../Minecraft.h"
#include "../../player/LocalPlayer.h"
#include "../../../platform/input/Keyboard.h"
#include "../../../world/level/Level.h"
#include "../../../network/RakNetInstance.h"
#include "../../../network/ServerSideNetworkHandler.h"
#include "../../../network/packet/MessagePacket.h"
#include "../../../platform/log.h"
#include "util/StringUtils.h"

#include <sstream>
#include <cstdlib>
#include <cctype>

ConsoleScreen::ConsoleScreen()
:   _input(""),
    _cursorBlink(0)
{
}

void ConsoleScreen::init()
{
}

void ConsoleScreen::tick()
{
    _cursorBlink++;
}

bool ConsoleScreen::handleBackEvent(bool /*isDown*/)
{
    minecraft->setScreen(NULL);
    return true;
}

void ConsoleScreen::keyPressed(int eventKey)
{
    if (eventKey == Keyboard::KEY_ESCAPE) {
        minecraft->setScreen(NULL);
    } else if (eventKey == Keyboard::KEY_RETURN) {
        execute();
    } else if (eventKey == Keyboard::KEY_BACKSPACE) {
        if (!_input.empty())
            _input.erase(_input.size() - 1, 1);
    } else {
        super::keyPressed(eventKey);
    }
}

void ConsoleScreen::charPressed(char inputChar)
{
    if (inputChar >= 32 && inputChar < 127)
        _input += inputChar;
}

void ConsoleScreen::execute()
{
    if (!minecraft->level) return;

    if (_input.empty()) {
        return minecraft->setScreen(NULL);
    }

    if (_input[0] == '/') {
        // Command
        _input = Util::stringTrim(_input.substr(1));
        
        std::istringstream iss(minecraft->commandManager().execute(*minecraft, *minecraft->player, _input));
        for (std::string line; std::getline(iss, line); ) {
            minecraft->gui.addMessage(line);
        }
    } else {
        // @ai @rewrite
        if (minecraft->netCallback && minecraft->raknetInstance->isServer()) {
            static_cast<ServerSideNetworkHandler*>(minecraft->netCallback)->displayGameMessage(_input);
        } else if (minecraft->netCallback) {
            MessagePacket chatPkt(_input.c_str());
            minecraft->raknetInstance->send(chatPkt);
        } else {
            minecraft->gui.addMessage("<" + minecraft->player->name + "> " + _input);
        }
    }

    minecraft->setScreen(NULL);
}

// ---------------------------------------------------------------------------
// render
// ---------------------------------------------------------------------------
void ConsoleScreen::render(int /*xm*/, int /*ym*/, float /*a*/)
{
    // Dim the game world slightly
    fillGradient(0, 0, width, height, 0x00000000, 0x40000000);

    const int boxH  = 12;
    const int boxY  = height - boxH - 2;
    const int boxX0 = 2;
    const int boxX1 = width - 2;

    // Input box background
    fill(boxX0, boxY, boxX1, boxY + boxH, 0xc0000000);
    // Border
    fill(boxX0,     boxY,          boxX1, boxY + 1,   0xff808080);
    fill(boxX0,     boxY + boxH - 1, boxX1, boxY + boxH, 0xff808080);
    fill(boxX0,     boxY,          boxX0 + 1, boxY + boxH, 0xff808080);
    fill(boxX1 - 1, boxY,          boxX1,     boxY + boxH, 0xff808080);

    // Input text + blinking cursor
    std::string displayed = _input;
    if ((_cursorBlink / 10) % 2 == 0)
        displayed += '_';

    // Placeholder hint when empty
    font->drawShadow(displayed, (float)(boxX0 + 2), (float)(boxY + 2), 0xffffffff);
}
