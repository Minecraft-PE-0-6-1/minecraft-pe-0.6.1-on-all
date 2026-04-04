#include "KeyOption.hpp"
#include <MinecraftClient.hpp>

KeyOption::KeyOption(MinecraftClient& minecraft, OptionId optId) 
    : Touch::TButton((int)optId, Keyboard::getKeyName(minecraft.options().getIntValue(optId))) {}


void KeyOption::mouseClicked(MinecraftClient& minecraft, int x, int y, int buttonNum) {
    selected = isInside(x, y);
    msg = (selected)? "..." : Keyboard::getKeyName(minecraft.options().getIntValue((OptionId)id));
}

void KeyOption::keyPressed(MinecraftClient& minecraft, int key) {
    if (!selected) return;

    if (key != Keyboard::KEY_ESCAPE) {
        minecraft.options().set((OptionId)id, key);
    }

    selected = false;
    msg = Keyboard::getKeyName(minecraft.options().getIntValue((OptionId)id));
}