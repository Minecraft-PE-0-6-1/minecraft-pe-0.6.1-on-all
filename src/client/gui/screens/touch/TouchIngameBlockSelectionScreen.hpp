#pragma once

#include "client/gui/Screen.hpp"
#include "client/gui/components/InventoryPane.hpp"
#include "client/gui/components/Button.hpp"
#include "client/gui/components/ScrollingPane.hpp"
#include "client/gui/components/ItemPane.hpp"
#include "client/gui/TweenData.hpp"
#include "client/player/input/touchscreen/TouchAreaModel.hpp"
#include "AppPlatform.hpp"

namespace Touch {

class IngameBlockSelectionScreen :	public Screen,
									public IInventoryPaneCallback
{
	typedef Screen super;

public:
	IngameBlockSelectionScreen();
	virtual ~IngameBlockSelectionScreen();

	virtual void init() override;
	virtual void setupPositions() override;
	virtual void removed() override;

	void tick() override;
	void render(int xm, int ym, float a) override;

	bool hasClippingArea(IntRectangle& out) override;

	// IInventoryPaneCallback
	bool addItem(const InventoryPane* pane, int itemId) override;
	bool isAllowed(int slot) override;
	std::vector<const ItemInstance*> getItems(const InventoryPane* forPane) override;

	void buttonClicked(Button* button) override;
protected:
	virtual void mouseClicked(int x, int y, int buttonNum) override;
	virtual void mouseReleased(int x, int y, int buttonNum) override;

	// also support wheel scrolling
	virtual void mouseWheel(int dx, int dy, int xm, int ym) override;
private:
	void renderDemoOverlay();

	//int getLinearSlotId(int x, int y);
	int getSlotPosX(int slotX);
	int getSlotPosY(int slotY);
	int getSlotHeight();

private:
	int selectedItem;
	bool  _pendingClose;
	InventoryPane* _blockList;

	THeader bHeader;
	ImageButton bDone;
	TButton bCraft;
	TButton bArmor;
	TButton bMenu;

	IntRectangle clippingArea;

	int InventoryRows;
	int InventorySize;
	int InventoryColumns;
};

}

