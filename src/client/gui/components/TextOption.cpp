#include "TextOption.hpp"
#include <client/Minecraft.hpp>

TextOption::TextOption(MinecraftClient& minecraft, OptionId optId) 
    : TextBox((int)optId, minecraft.options().getOpt(optId)->getStringId()) 
{
    text = minecraft.options().getStringValue(optId);
}

bool TextOption::loseFocus(MinecraftClient& minecraft) {
    if (TextBox::loseFocus(minecraft)) {
        minecraft.options().set((OptionId)id, text);
        return true;
    }

    return false;
}