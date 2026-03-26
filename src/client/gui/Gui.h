#ifndef NET_MINECRAFT_CLIENT_GUI__Gui_H__
#define NET_MINECRAFT_CLIENT_GUI__Gui_H__

//package net.minecraft.client.gui;

#include "GuiComponent.h"
#include "Font.h"
#include "../player/input/touchscreen/TouchAreaModel.h"
#include "../renderer/RenderChunk.h"
#include "../../util/Random.h"
#include "../IConfigListener.h"

class MinecraftClient;
class ItemInstance;
class Textures;
class Tesselator;
struct IntRectangle;

struct GuiMessage
{
	std::string message;
	int ticks;
};

typedef std::vector<GuiMessage> GuiMessageList;

class Gui: public GuiComponent, IConfigListener
{
public:
    Gui(MinecraftClient& minecraft);
	~Gui();

	int getSlotIdAt(int x, int y);
	void flashSlot(int slotId);
	bool isInside(int x, int y);
	RectangleArea getRectangleArea(int extendSide);
	void getSlotPos(int slot, int& posX, int& posY);
	int getNumSlots();

	void handleClick(int button, int x, int y);
	void handleKeyPressed( int key );
	void scrollChat(int delta);

	void tick();
	void render(float a, bool mouseFree, int xMouse, int yMouse);

	void renderToolBar( float a, int ySlot, const int screenWidth );

	void renderChatMessages( const int screenHeight, unsigned int max, bool isChatting, Font* font );

	// draw a string containing simple [color]...[/color] tags; color names are matched
	// case-insensitively and default to white. alpha is applied to each segment.
	// draw tagged string (ignores simple [color]…[/color] tags)
	static void drawColoredString(Font* font, const std::string& text, float x, float y, int alpha);
	static float getColoredWidth(Font* font, const std::string& text);

	void renderOnSelectItemNameText( const int screenWidth, Font* font, int ySlot );

	void renderSleepAnimation( const int screenWidth, const int screenHeight );

	void renderBubbles();
	void renderHearts();
	void renderDebugInfo();
	void renderPlayerList(Font* font, int screenWidth, int screenHeight);

	void renderProgressIndicator( const bool isTouchInterface, const int screenWidth, const int screenHeight, float a );

    void addMessage(const std::string& string);
	void clearMessages();
	void postError(int errCode);

    void onGraphicsReset();
	void inventoryUpdated();

	void setNowPlaying(const std::string& string);
	void displayClientMessage(const std::string& messageId);
	void renderSlotText(const ItemInstance* item, float x, float y, bool hasFinite, bool shadow);
	void texturesLoaded( Textures* textures );

	void onConfigChanged(const Config& config);
	void onLevelGenerated();

	void setScissorRect(const IntRectangle& rect);

	static float floorAlignToScreenPixel(float);
	static int itemCountItoa(char* buf, int count);
private:
	void renderVignette(float br, int w, int h);
	void renderSlot(int slot, int x, int y, float a);
	void tickItemDrop();
	float cubeSmoothStep(float percentage, float min, float max);
public:
	float progress = 0.f;
	std::string selectedName;
	static float InvGuiScale;
	static float GuiScale;

private:
	//ItemRenderer itemRenderer;
	GuiMessageList guiMessages;
	int chatScrollOffset = 0;
	Random random;

	MinecraftClient& minecraft;
	int tickCount = 0;
	float itemNameOverlayTime = 2;
	std::string overlayMessageString;
	int overlayMessageTime = 0;
	bool animateOverlayMessageColor = false;

	float tbr = 1.f;

	RenderChunk _inventoryRc;
	bool _inventoryNeedsUpdate = true;
	
	int _flashSlotId = -1;
	float _flashSlotStartTime = -1;

	Font* _slotFont = nullptr;
	int _numSlots = 4;

	RenderChunk rcFeedbackOuter;
	RenderChunk rcFeedbackInner;

	// For dropping
	static const float DropTicks;
	float  _currentDropTicks = -1;
	int    _currentDropSlot = -1;

	bool   _openInventorySlot;
};

#endif /*NET_MINECRAFT_CLIENT_GUI__Gui_H__*/
