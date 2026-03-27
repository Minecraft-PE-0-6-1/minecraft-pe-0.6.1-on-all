#pragma once
#include "Button.hpp"
#include <client/Options.hpp>

class KeyOption : public Touch::TButton {
public:
    KeyOption(Minecraft* minecraft, OptionId optId);

    virtual void mouseClicked(Minecraft* minecraft, int x, int y, int buttonNum);
    virtual void released(int mx, int my) {}
    virtual void keyPressed(Minecraft* minecraft, int key);
protected:
    bool m_captureMode;
};