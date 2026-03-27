#pragma once

#include "../../../../platform/input/Mouse.h"

class ITouchScreenModel
{
public:
	virtual ~ITouchScreenModel() {}
	virtual int getPointerId(const MouseAction& m) { return m.pointerId; }
	virtual int getPointerId(int x, int y, int pid) { return pid; }
};

