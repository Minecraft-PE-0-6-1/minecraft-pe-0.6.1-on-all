#pragma once
#include "TextBox.hpp"
#include <client/Options.hpp>

class TextOption : public TextBox {
public:
    TextOption(Minecraft* minecraft, OptionId optId);

    virtual bool loseFocus(Minecraft* minecraft);
};