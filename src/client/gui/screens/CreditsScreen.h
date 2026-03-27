#pragma once

#include "../Screen.h"
#include "../components/Button.h"

class ImageButton;

#include <vector>
#include <string>

class CreditsScreen: public Screen {
public:
    typedef Screen super;
    CreditsScreen();
    virtual ~CreditsScreen();
    void init();
    void setupPositions();
    virtual void tick();
    void render(int xm, int ym, float a);
    void buttonClicked(Button* button);
    virtual void mouseClicked(int x, int y, int buttonNum);
private:
    Touch::THeader* bHeader;
    ImageButton* btnBack;

    std::vector<std::string> _lines;
    float _scrollY;
    float _scrollSpeed;
};

