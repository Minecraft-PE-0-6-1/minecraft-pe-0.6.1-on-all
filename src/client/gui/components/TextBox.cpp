#include "TextBox.hpp"
#include "client/gui/Gui.hpp"
#include "client/Minecraft.hpp"
#include "AppPlatform.hpp"
#include "platform/input/Mouse.hpp"

// delegate constructors
TextBox::TextBox(int id, const std::string& msg)
 : TextBox(id, 0, 0, msg)
{
}

TextBox::TextBox(int id, int x, int y, const std::string& msg)
 : TextBox(id, x, y, 24, Font::DefaultLineHeight + 4, msg)
{
}

TextBox::TextBox(int id, int x, int y, int w, int h, const std::string& msg)
 : GuiElement(true, true, x, y, w, h),
   id(id), hint(msg), focused(false), blink(false), blinkTicks(0)
{
}

void TextBox::setFocus(Minecraft* minecraft) {
    if (!focused) {
        minecraft->platform()->showKeyboard();
        focused = true;
        blinkTicks = 0;
        blink = false;
    }
}

bool TextBox::loseFocus(Minecraft* minecraft) {
    if (focused) {
        minecraft->platform()->hideKeyboard();
        focused = false;
        return true;
    }
    return false;
}

void TextBox::mouseClicked(Minecraft* minecraft, int x, int y, int buttonNum) {
    if (buttonNum == MouseAction::ACTION_LEFT) {
        if (pointInside(x, y)) {
            setFocus(minecraft);
        } else {
            loseFocus(minecraft);
        }
    }
}

void TextBox::charPressed(Minecraft* minecraft, char c)  {
    if (focused && c >= 32 && c < 127 && (int)text.size() < 256) {
        text.push_back(c);
    }
}

void TextBox::keyPressed(Minecraft* minecraft, int key) {
    if (focused && key == Keyboard::KEY_BACKSPACE && !text.empty()) {
        text.pop_back();
    }
}

void TextBox::tick(Minecraft* minecraft) {
    blinkTicks++;
    if (blinkTicks >= 5) {
        blink = !blink;
        blinkTicks = 0;
    }
}

void TextBox::render(Minecraft* minecraft, int xm, int ym) {
    // textbox like in beta 1.7.3
    // change appearance when focused so the user can tell it's active
    // active background darker gray with a subtle border
    uint32_t bgColor = focused ? 0xffa0a0a0 : 0xffa0a0a0;
    uint32_t borderColor = focused ? 0xff000000 : 0xff000000;
    fill(x, y, x + width, y + height, bgColor);
    fill(x + 1, y + 1, x + width - 1, y + height - 1, borderColor);

    glEnable2(GL_SCISSOR_TEST);
    glScissor(
        Gui::GuiScale * (x + 2), 
        minecraft->height - Gui::GuiScale * (y + height - 2), 
        Gui::GuiScale * (width - 2), 
        Gui::GuiScale * (height - 2)
    );

	int _y = y + (height - Font::DefaultLineHeight) / 2;

    if (text.empty() && !focused) {
        drawString(minecraft->font, hint, x + 2, _y, 0xff5e5e5e);
    }

    if (focused && blink) text.push_back('_');

    drawString(minecraft->font, text, x + 2, _y, 0xffffffff);

    if (focused && blink) text.pop_back();

    glDisable2(GL_SCISSOR_TEST);
}
