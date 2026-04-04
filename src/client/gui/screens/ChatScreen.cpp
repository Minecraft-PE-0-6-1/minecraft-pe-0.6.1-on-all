#include "ChatScreen.hpp"
#include "client/gui/Screen.hpp"
#include <MinecraftClient.hpp>


void ChatScreen::init() {
	m_input.x = 2;
	m_input.width = width - 4;
	m_input.height = 12;
	m_input.y = height - m_input.height - 2;

	textBoxes.push_back(&m_input);
}

void ChatScreen::render(int xm, int ym, float a) {
	fillGradient(0, 0, width, height, 0x00000000, 0x40000000);
	m_input.focused = true;

	Screen::render(xm, ym, a);
}

bool ChatScreen::handleBackEvent(bool isDown) {
    minecraft.setScreen(NULL);
    return true;
}

void ChatScreen::keyPressed(int eventKey) {
	// Was ai code before (we're sorry about it), but now reviewed
	// and everything is ok

    if (eventKey == Keyboard::KEY_ESCAPE) {
        return minecraft.setScreen(NULL);
    }
	
	if (eventKey == Keyboard::KEY_RETURN) {
        // execute...
    }

	return ChatScreen::keyPressed(eventKey);
}