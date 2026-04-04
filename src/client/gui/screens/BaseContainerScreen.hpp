#pragma once

//package net.minecraft.client.gui.screens;

#include <vector>
#include "client/gui/Screen.hpp"
#include <MinecraftClient.hpp>
#include "client/player/LocalPlayer.hpp"

class BaseContainerMenu;

class BaseContainerScreen: public Screen
{
    typedef Screen super;
public:
	BaseContainerScreen(MinecraftClient& mc, BaseContainerMenu* menu) : Screen(mc), menu(menu) {}

	virtual void init() {
        super::init();
        minecraft.player()->containerMenu = menu;
    }

	virtual void tick() {
		super::tick();
		if (!minecraft.player()->isAlive() || minecraft.player()->removed)
			minecraft.player()->closeContainer();
	}

	virtual void keyPressed( int eventKey )
	{
		if (eventKey == Keyboard::KEY_ESCAPE) {
			minecraft.player()->closeContainer();
		} else {
			super::keyPressed(eventKey);
		}
	}

	virtual bool closeOnPlayerHurt() {
		return true;
	}

protected:
	BaseContainerMenu* menu;
};

