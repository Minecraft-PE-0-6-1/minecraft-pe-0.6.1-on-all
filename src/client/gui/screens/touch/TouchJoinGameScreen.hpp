#pragma once

#include "client/gui/Screen.hpp"
#include "client/gui/components/Button.hpp"
#include "client/gui/components/RolledSelectionListV.hpp"
#include <MinecraftClient.hpp>
#include "platform/input/Multitouch.hpp"
#include "network/RakNetInstance.hpp"

namespace Touch {

class JoinGameScreen;

class AvailableGamesList : public RolledSelectionListV
{
	int startSelected;
	int selectedItem;
	ServerList copiedServerList;

	friend class JoinGameScreen;

public:
	AvailableGamesList(MinecraftClient& _minecraft, int _width, int _height)
	:	RolledSelectionListV(_minecraft, _width, _height, 0, _width, 24, _height, 34),
		selectedItem(-1),
		startSelected(-1)
	{
	}

protected:

	virtual int getNumberOfItems() { return (int)copiedServerList.size(); }

	virtual void selectCancel();
	virtual void selectStart(int item);
	virtual void selectItem(int item, bool doubleClick);

	virtual bool isSelectedItem(int item) { return item == selectedItem; }

	virtual void renderBackground() {}
	virtual void renderItem(int i, int x, int y, int h, Tesselator& t);
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
	TButton bBack;
	TButton bJoinByIp;
	THeader bHeader;
	AvailableGamesList* gamesList;
};

};

