#pragma once
#include "TextBox.hpp"
#include <client/Options.hpp>

class TextOption : public TextBox {
public:
    TextOption(MinecraftClient& minecraft, OptionId optId);

    virtual bool loseFocus(MinecraftClient& minecraft);
};