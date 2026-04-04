#pragma once

#include "client/gui/Screen.hpp"
#include "client/gui/components/Button.hpp"
#include "client/gui/components/ScrolledSelectionList.hpp"
#include <MinecraftClient.hpp>
#include "network/RakNetInstance.hpp"


class JoinGameScreen;

class AvailableGamesList : public ScrolledSelectionList
{
	int selectedItem;
	ServerList copiedServerList;

	friend class JoinGameScreen;

public:

	AvailableGamesList(MinecraftClient& _minecraft, int _width, int _height)
	:	ScrolledSelectionList(_minecraft, _width, _height, 24, _height - 30, 28)
	{
	}

protected:

	virtual int getNumberOfItems() { return (int)copiedServerList.size(); }

	virtual void selectItem(int item, bool doubleClick) { selectedItem = item; }
	virtual bool isSelectedItem(int item) { return item == selectedItem; }

	virtual void renderBackground() {}
	virtual void renderItem(int i, int x, int y, int h, Tesselator& t)
	{
		const PingedCompatibleServer& s = copiedServerList[i];
		unsigned int color = s.isSpecial? 0xff00b0 : 0xffffa0;
		drawString(minecraft.font(), s.name.C_String(), x, y + 2, color);
		drawString(minecraft.font(), s.address.ToString(false), x, y + 16, 0xffffa0);
	}
};

class JoinGameScreen: public Screen
{
public:
	JoinGameScreen();
	virtual ~JoinGameScreen();

	void init();
	void setupPositions();

	virtual bool handleBackEvent(bool isDown);

	virtual bool isIndexValid(int index);

	virtual void tick();

	void render(int xm, int ym, float a);

	void buttonClicked(Button* button);

	bool isInGameScreen();
private:
	Button bJoin;
	Button bBack;
	AvailableGamesList* gamesList;
};

