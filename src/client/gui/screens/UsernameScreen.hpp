#pragma once

#include "client/gui/Screen.hpp"
#include "client/gui/components/Button.hpp"
#include "client/gui/components/TextBox.hpp"
#include <string>

class UsernameScreen : public Screen
{
    typedef Screen super;
public:
    UsernameScreen();
    virtual ~UsernameScreen();

    void init() override;
    virtual void setupPositions() override;
    void render(int xm, int ym, float a) override;
    void tick() override;

    virtual bool isPauseScreen() override { return false; }

    virtual void keyPressed(int eventKey) override;
    virtual bool handleBackEvent(bool isDown) override { return true; } // block back/escape
    virtual void removed() override;

protected:
    virtual void buttonClicked(Button* button) override;

private:
    Touch::TButton _btnDone;
    TextBox tUsername;
    std::string _input;
    int _cursorBlink;
};

