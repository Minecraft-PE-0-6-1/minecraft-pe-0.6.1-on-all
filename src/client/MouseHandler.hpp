#pragma once

//package net.minecraft.client;

class ITurnInput;

class MouseHandler
{
public:
	MouseHandler(ITurnInput* turnInput);
	MouseHandler();
	~MouseHandler();

	void setTurnInput(ITurnInput* turnInput);

    void grab();
    void release();

    void poll();

    float xd, yd;
private:
	int toSkip;
	ITurnInput* _turnInput;
};

