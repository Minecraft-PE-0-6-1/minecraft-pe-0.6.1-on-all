#pragma once
#include "Button.hpp"
#include <client/Options.hpp>

class KeyOption : public Touch::TButton {
public:
    KeyOption(MinecraftClient& minecraft, OptionId optId);

    virtual void mouseClicked(MinecraftClient& minecraft, int x, int y, int buttonNum);
    virtual void released(int mx, int my) {}
    virtual void keyPressed(MinecraftClient& minecraft, int key);
protected:
    bool m_captureMode;
};