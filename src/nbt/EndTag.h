#pragma once

//package com.mojang.nbt;

/* import java.io.* */

class EndTag: public Tag {
	typedef Tag super;
public:
    EndTag()
    :	super("")
	{
    }

    void load(IDataInput* dis) /*throws IOException*/ {
    }

    void write(IDataOutput* dos) /*throws IOException*/ {
    }

    char getId() const {
        return TAG_End;
    }

    std::string toString() const {
        return "END";
    }

    //@Override
    Tag* copy() const {
        return new EndTag();
    }

    //@Override
    bool equals(const Tag& rhs) const {
        return super::equals(rhs);
    }
};

