#include "Gui.h"
#include "Font.h"
#include "client/Options.h"
#include "platform/input/Keyboard.h"
#include "screens/IngameBlockSelectionScreen.h"
#include "screens/ChatScreen.h"
#include "screens/ConsoleScreen.h"
#include "../Minecraft.h"
#include "../player/LocalPlayer.h"
#include "../renderer/Chunk.h"
#include "../renderer/Tesselator.h"
#include "../renderer/TileRenderer.h"
#include "../renderer/LevelRenderer.h"
#include "../renderer/GameRenderer.h"
#include "../renderer/entity/ItemRenderer.h"
#include "../player/input/IInputHolder.h"
#include "../gamemode/GameMode.h"
#include "../gamemode/CreativeMode.h"
#include "../renderer/Textures.h"
#include "../../AppConstants.h"
#include "../../world/entity/player/Inventory.h"
#include "../../world/level/material/Material.h"
#include "../../world/item/Item.h"
#include "../../world/item/ItemInstance.h"
#include "../../platform/input/Mouse.h"
#include "../../world/level/Level.h"
#include "../../world/PosTranslator.h"
#include "../../platform/time.h"
#include <cmath>
#include <algorithm>
#include <sstream>

float Gui::InvGuiScale = 1.0f / 3.0f;
float Gui::GuiScale = 1.0f / Gui::InvGuiScale;
const float Gui::DropTicks = 40.0f;

//#include <android/log.h>

Gui::Gui(Minecraft* minecraft)
	:	minecraft(minecraft),
	tickCount(0),
	progress(0),
	overlayMessageTime(0),
	animateOverlayMessageColor(false),
	chatScrollOffset(0),
	tbr(1),
	_inventoryNeedsUpdate(true),
	_flashSlotId(-1),
	_flashSlotStartTime(-1),
	_slotFont(NULL),
	_numSlots(4),
	_currentDropTicks(-1),
	_currentDropSlot(-1),
	MAX_MESSAGE_WIDTH(240),
	itemNameOverlayTime(2),
	_openInventorySlot(minecraft->useTouchscreen())
{
	glGenBuffers2(1, &_inventoryRc.vboId);
	glGenBuffers2(1, &rcFeedbackInner.vboId);
	glGenBuffers2(1, &rcFeedbackOuter.vboId);
	//Gui::InvGuiScale = 1.0f / (int) (3 * Minecraft::width / 854);
}

Gui::~Gui()
{
	if (_slotFont)
		delete _slotFont;

	glDeleteBuffers(1, &_inventoryRc.vboId);
}

void Gui::render(float a, bool mouseFree, int xMouse, int yMouse) {

	if (!minecraft->level || !minecraft->player)
		return;

	//minecraft->gameRenderer->setupGuiScreen();
	Font* font = minecraft->font;

	const bool isTouchInterface = minecraft->useTouchscreen();

	const int screenWidth = (int)(minecraft->width * InvGuiScale);
	const int screenHeight = (int)(minecraft->height * InvGuiScale);
	blitOffset = -90;
	renderProgressIndicator(isTouchInterface, screenWidth, screenHeight, a);

	glColor4f2(1, 1, 1, 1);

	// H: 4
	// T: 7
	// L: 6
	// F: 3
	int ySlot = screenHeight - 16 - 3;

	if (!minecraft->options.getBooleanValue(OPTIONS_HIDEGUI)) {
		if (minecraft->gameMode->canHurtPlayer()) {
			minecraft->textures->loadAndBindTexture("gui/icons.png");
			Tesselator& t = Tesselator::instance;
			t.beginOverride();
			t.colorABGR(0xffffffff);
			renderHearts();
			renderBubbles();
			t.endOverrideAndDraw();
		}
	}

	// viginette has been fixed, was due to gl_blend not being enabled, my bad
	if (minecraft->options.getBooleanValue(OPTIONS_FANCY_GRAPHICS) && minecraft->options.getBooleanValue(OPTIONS_VIGNETTE)){
			renderVignette(this->minecraft->player->getBrightness(a), screenWidth, screenHeight);
		}
	// shredder end

	if(minecraft->player->getSleepTimer() > 0) {
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_ALPHA_TEST);

		renderSleepAnimation(screenWidth, screenHeight);

		glEnable(GL_ALPHA_TEST);
		glEnable(GL_DEPTH_TEST);
	}
	if (!minecraft->options.getBooleanValue(OPTIONS_HIDEGUI)) {
		renderToolBar(a, ySlot, screenWidth);

	//	font->drawShadow("Minecraft - Pocket Edition ", 2, 2, 0xffffffff);
//	font->drawShadow("This is a demo, not the finished product", 2, 10 + 2, 0xffffffff);

		glEnable(GL_BLEND);
		bool isChatting = (minecraft->screen && (dynamic_cast<ChatScreen*>(minecraft->screen) || dynamic_cast<ConsoleScreen*>(minecraft->screen)));
		unsigned int max = 10;
		if (isChatting) {
			int lineHeight = 9;
			max = (screenHeight - 48) / lineHeight;
			if (max < 1) max = 1;
			int maxScroll = (int)guiMessages.size() - (int)max;
			if (maxScroll < 0) maxScroll = 0;
			if (chatScrollOffset > maxScroll) chatScrollOffset = maxScroll;
		} else {
			chatScrollOffset = 0;
		}
		renderChatMessages(screenHeight, max, isChatting, font);
#if !defined(RPI)
		renderOnSelectItemNameText(screenWidth, font, ySlot);
#endif
#if defined(RPI)
		renderDebugInfo();
#endif

		if (Keyboard::isKeyDown(Keyboard::KEY_TAB)) {
			renderPlayerList(font, screenWidth, screenHeight);
		}

		if (minecraft->options.getBooleanValue(OPTIONS_RENDER_DEBUG))
			renderDebugInfo();
	}

	glDisable(GL_BLEND);
	glEnable2(GL_ALPHA_TEST);
}

int Gui::getSlotIdAt(int x, int y) {
	int screenWidth = (int)(minecraft->width * InvGuiScale);
	int screenHeight = (int)(minecraft->height * InvGuiScale);
	x = (int)(x * InvGuiScale);
	y = (int)(y * InvGuiScale);

	if (y < (screenHeight - 16 - 3) || y > screenHeight)
		return -1;

	int xBase = 2 + screenWidth / 2 - getNumSlots() * 10;
	int xRel  = (x - xBase);
	if (xRel < 0)
		return -1;

	int slot = xRel / 20;
	return (slot >= 0 && slot < getNumSlots())? slot : -1;
}

bool Gui::isInside(int x, int y) {
	return getSlotIdAt(x, y) != -1;
}

int Gui::getNumSlots() {
	return _numSlots;
}

void Gui::flashSlot(int slotId) {
	_flashSlotId = slotId;
	_flashSlotStartTime = getTimeS();
}

void Gui::getSlotPos(int slot, int& posX, int& posY) {
	int screenWidth = (int)(minecraft->width * InvGuiScale);
	int screenHeight = (int)(minecraft->height * InvGuiScale);
	posX = screenWidth / 2 - getNumSlots() * 10 + slot * 20, 
		posY = screenHeight - 22;
}

RectangleArea Gui::getRectangleArea(int extendSide) {
	const int Spacing = 3;
	const float pCenterX   = 2.0f + (float)(minecraft->width / 2);
	const float pHalfWidth = (1.0f + (getNumSlots() * 10 + Spacing)) * Gui::GuiScale;
	const float pHeight    = (22 + Spacing) * Gui::GuiScale;

	if (extendSide < 0)
		return RectangleArea(0, (float)minecraft->height-pHeight, pCenterX+pHalfWidth+2, (float)minecraft->height);
	if (extendSide > 0)
		return RectangleArea(pCenterX-pHalfWidth, (float)minecraft->height-pHeight, (float)minecraft->width, (float)minecraft->height);

	return RectangleArea(pCenterX-pHalfWidth, (float)minecraft->height-pHeight, pCenterX+pHalfWidth+2, (float)minecraft->height);
}

void Gui::handleClick(int button, int x, int y) {
	if (button != MouseAction::ACTION_LEFT)	return;

	int slot = getSlotIdAt(x, y);
	if (slot != -1) {
		if (_openInventorySlot && slot == (getNumSlots()-1)) {
			minecraft->screenChooser.setScreen(SCREEN_BLOCKSELECTION);
		} else {
			minecraft->player->inventory->selectSlot(slot);
			itemNameOverlayTime = 0;
		}
	}
}

	void Gui::handleKeyPressed(int key)
	{
		bool isChatting = (minecraft->screen && (dynamic_cast<ChatScreen*>(minecraft->screen) || dynamic_cast<ConsoleScreen*>(minecraft->screen)));
		if (isChatting) {
			// Allow scrolling the chat history with the mouse/keyboard when chat is open
			if (key == 38) { // VK_UP
				scrollChat(1);
				return;
			} else if (key == 40) { // VK_DOWN
				scrollChat(-1);
				return;
			} else if (key == 33) { // VK_PRIOR (Page Up)
				// Scroll by a page
				int screenHeight = (int)(minecraft->height * InvGuiScale);
				int maxVisible = (screenHeight - 48) / 9;
				scrollChat(maxVisible);
				return;
			} else if (key == 34) { // VK_NEXT (Page Down)
				int screenHeight = (int)(minecraft->height * InvGuiScale);
				int maxVisible = (screenHeight - 48) / 9;
				scrollChat(-maxVisible);
				return;
			}
		}

		if (key == Keyboard::KEY_F1) {
			minecraft->options.toggle(OPTIONS_HIDEGUI);
		}

		if (key == 99)
		{
			if (minecraft->player->inventory->selected > 0)
			{
				minecraft->player->inventory->selected--;
			}
		}
		else if (key == 4)
		{
			if (minecraft->player->inventory->selected < (getNumSlots() - 2))
			{
				minecraft->player->inventory->selected++;
			}
		}
		else if (key == 100)
		{
			minecraft->screenChooser.setScreen(SCREEN_BLOCKSELECTION);
		}
		else if (key == minecraft->options.getIntValue(OPTIONS_KEY_DROP)) 
		{
			minecraft->player->inventory->dropSlot(minecraft->player->inventory->selected, false);
		}
	}

void Gui::scrollChat(int delta) {
	if (delta == 0)
		return;

	int screenHeight = (int)(minecraft->height * InvGuiScale);
	int maxVisible = (screenHeight - 48) / 9;
	if (maxVisible <= 0)
		return;

	int maxScroll = (int)guiMessages.size() - maxVisible;
	if (maxScroll < 0) maxScroll = 0;
	int desired = chatScrollOffset + delta;
	if (desired < 0) desired = 0;
	if (desired > maxScroll) desired = maxScroll;
	chatScrollOffset = desired;
}

void Gui::tick() {
	if (overlayMessageTime > 0) overlayMessageTime--;
	tickCount++;
	if(itemNameOverlayTime < 2)
		itemNameOverlayTime += 1.0f / SharedConstants::TicksPerSecond;
	for (unsigned int i = 0; i < guiMessages.size(); i++) {
		guiMessages.at(i).ticks++;
	}

	if (!minecraft->isCreativeMode())
		tickItemDrop();
}

void Gui::addMessage(const std::string& _string) {
	if (!minecraft->font)
		return;

	std::string string = _string;
	while (minecraft->font->width(string) > MAX_MESSAGE_WIDTH) {
		unsigned int i = 1;
		while (i < string.length() && minecraft->font->width(string.substr(0, i + 1)) <= MAX_MESSAGE_WIDTH) {
			i++;
		}
		addMessage(string.substr(0, i));
		string = string.substr(i);
	}
	GuiMessage message;
	message.message = string;
	message.ticks = 0;
	guiMessages.insert(guiMessages.begin(), message);

	// Keep a larger history so users can scroll through the full chat
	const unsigned int MaxHistoryLines = 200;
	while (guiMessages.size() > MaxHistoryLines) {
		guiMessages.pop_back();
	}

	// If the user has scrolled up, keep their window fixed (new messages shift older ones down)
	if (chatScrollOffset > 0) {
		chatScrollOffset++;
	}
}

void Gui::clearMessages() {
	guiMessages.clear();
	chatScrollOffset = 0;
}

void Gui::setNowPlaying(const std::string& string) {
	overlayMessageString = "Now playing: " + string;
	overlayMessageTime = 20 * 3;
	animateOverlayMessageColor = true;
}

void Gui::displayClientMessage(const std::string& messageId) {
	//Language language = Language.getInstance();
	//std::string languageString = language.getElement(messageId);
	addMessage(messageId);
}


// @todo - shredder: Function seems to be completely fine and ported over from java beta, but renders opaque??? need to investigate
void Gui::renderVignette(float br, int w, int h) {
	br = 1 - br;
	if (br < 0) br = 0;
	if (br > 1) br = 1;
	tbr += (br - tbr) * 0.01f;

	glDisable(GL_DEPTH_TEST);
	glDepthMask(false);
	glEnable(GL_BLEND);
	glBlendFunc2(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
	glColor4f2(tbr, tbr, tbr, 1);

	minecraft->textures->loadAndBindTexture("misc/vignette.png");
	glTexParameteri2(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri2(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	Tesselator& t = Tesselator::instance;
	t.begin();
	t.vertexUV(0, (float)h, -90, 0, 1);
	t.vertexUV((float)w, (float)h, -90, 1, 1);
	t.vertexUV((float)w, 0, -90, 1, 0);
	t.vertexUV(0, 0, -90, 0, 0);
	t.draw();
	glDepthMask(true);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glColor4f2(1, 1, 1, 1);
	glBlendFunc2(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Gui::renderSlot(int slot, int x, int y, float a) {
	ItemInstance* item = minecraft->player->inventory->getItem(slot);
	if (!item) {
		//LOGW("Warning: item @ Gui::renderSlot is NULL\n");
		return;
	}

	const bool fancy = true;
	ItemRenderer::renderGuiItem(minecraft->font, minecraft->textures, item, (float)x, (float)y, fancy);
}

void Gui::renderSlotText( const ItemInstance* item, float x, float y, bool hasFinite, bool shadow )
{
	//if (!item || item->getItem()->getMaxStackSize() <= 1) {
	if (item->count <= 1) {
		return;
	}

	int c = item->count;

	char buffer[4] = {0,0,0,0};
	if (hasFinite)
		itemCountItoa(buffer, c);
	else
		buffer[0] = (char)157;

	//LOGI("slot: %d - %s\n", slot, buffer);
	if (shadow)
		minecraft->font->drawShadow(buffer, x, y, item->count>0?0xffcccccc:0x60cccccc);
	else
		minecraft->font->draw(buffer, x, y, item->count>0?0xffcccccc:0x60cccccc);
}

void Gui::inventoryUpdated() {
	_inventoryNeedsUpdate = true;
}

void Gui::onGraphicsReset() {
	inventoryUpdated();
}

void Gui::texturesLoaded( Textures* textures ) {
	//_slotFont = new Font(&minecraft->options, "gui/gui_blocks.png", textures, 0, 504, 10, 1, '0');
}

void Gui::onConfigChanged( const Config& c ) {
	Tesselator& t = Tesselator::instance;
	t.begin();

	//
	// Create outer feedback circle
	//
#ifdef ANDROID
	const float mm = 50; //20
#else
	const float mm = 50; //20
#endif
	const float maxRadius = minecraft->pixelCalcUi.millimetersToPixels(mm);
	const float radius = Mth::Min(80.0f/2, maxRadius);
	//LOGI("radius, maxradius: %f, %f\n", radius, maxRadius);
	const float radiusInner = radius * 0.95f;

	const int steps = 24;
	const float fstep = Mth::TWO_PI / steps;
	for (int i = 0; i < steps; ++i) {
		float a = i * fstep;;
		float b = a + fstep;

		float aCos = Mth::cos(a);
		float bCos = Mth::cos(b);
		float aSin = Mth::sin(a);
		float bSin = Mth::sin(b);
		float x00 = radius * aCos;
		float x01 = radiusInner * aCos;
		float x10 = radius * bCos;
		float x11 = radiusInner * bCos;
		float y00 = radius * aSin;
		float y01 = radiusInner * aSin;
		float y10 = radius * bSin;
		float y11 = radiusInner * bSin;

		t.vertexUV(x01, y01, 0, 0, 1);
		t.vertexUV(x11, y11, 0, 1, 1);
		t.vertexUV(x10, y10, 0, 1, 0);
		t.vertexUV(x00, y00, 0, 0, 0);
	}
	rcFeedbackOuter = t.end(true, rcFeedbackOuter.vboId);

	//
	// Create the inner feedback ring
	//
	t.begin(GL_TRIANGLE_FAN);
	t.vertex(0, 0, 0);
	for (int i = 0; i < steps + 1; ++i) {
		float a = -i * fstep;
		float xx = radiusInner * Mth::cos(a);
		float yy = radiusInner * Mth::sin(a);
		t.vertex(xx, yy, 0);
		//LOGI("x,y: %f, %f\n", xx, yy);
	}
	rcFeedbackInner = t.end(true, rcFeedbackInner.vboId);

	if (c.minecraft->useTouchscreen()) {
		// I'll bump this up to 6.
		int num = 6; // without "..." dots
		if (!c.minecraft->options.getBooleanValue(OPTIONS_IS_JOY_TOUCH_AREA) && c.width > 480) {
			while (num < Inventory::MAX_SELECTION_SIZE - 1) {
				int x0, x1, y;
				getSlotPos(0, x0, y);
				getSlotPos(num, x1, y);
				int width = x1 - x0;
				float leftoverPixels = c.width - c.guiScale*width;
				if (c.pixelCalc.pixelsToMillimeters(leftoverPixels) < 80)
					break;
				num++;
			}
		}
		_numSlots = num;
#if defined(__APPLE__)
		_numSlots = Mth::Min(7, _numSlots);
#endif
	} else {
		_numSlots = Inventory::MAX_SELECTION_SIZE; // Xperia Play
	}
	MAX_MESSAGE_WIDTH = c.guiWidth;
}

float Gui::floorAlignToScreenPixel(float v) {
	return (int)(v * Gui::GuiScale) * Gui::InvGuiScale;
}

int Gui::itemCountItoa( char* buffer, int count )
{
	if (count < 0)
		return 0;

	if (count < 10) { // 1 digit
		buffer[0] = '0' + count;
		buffer[1] = 0;
		return 1;
	} else if (count < 100) { // 2 digits
		int digit = count/10;
		buffer[0] = '0' + digit;
		buffer[1] = '0' + count - digit*10;
		buffer[2] = 0;
	} else { // 3 digits -> "99+"
		buffer[0] = buffer[1] = '9';
		buffer[2] = '+';
		buffer[3] = 0;
		return 3;
	}
	return 2;
}

void Gui::tickItemDrop()
{
	// Handle item drop
	static bool isCurrentlyActive = false;
	isCurrentlyActive = false;

	int slots = getNumSlots() - _openInventorySlot;

	if (Mouse::isButtonDown(MouseAction::ACTION_LEFT)) {
		int slot = getSlotIdAt(Mouse::getX(), Mouse::getY());
		if (slot >= 0 && slot < slots) {
			if (slot != _currentDropSlot) {
				_currentDropTicks = 0;
				_currentDropSlot = slot;
			}
			isCurrentlyActive = true;
			if ((_currentDropTicks += 1.0f) >= DropTicks) {
				minecraft->player->inventory->dropSlot(slot, false);
				minecraft->level->playSound(minecraft->player, "random.pop", 0.3f, 1);
				isCurrentlyActive = false;
			}
		}
	}
	if (!isCurrentlyActive) {
		_currentDropSlot = -1;
		_currentDropTicks = -1;
	}
}

void Gui::postError( int errCode )
{
	static std::set<int> posted;
	if (posted.find(errCode) != posted.end())
		return;

	posted.insert(errCode);

	std::stringstream s;
	s << "Something went wrong! (errcode " << errCode << ")\n";
	addMessage(s.str());
}

void Gui::setScissorRect( const IntRectangle& bbox )
{
	GLuint x = (GLuint)(GuiScale * bbox.x);
	GLuint y = minecraft->height - (GLuint)(GuiScale * (bbox.y + bbox.h));
	GLuint w = (GLuint)(GuiScale * bbox.w);
	GLuint h = (GLuint)(GuiScale * bbox.h);
	glScissor(x, y, w, h);
}

float Gui::cubeSmoothStep(float percentage, float min, float max) {
	//percentage = percentage * percentage;
	//return (min * percentage) + (max * (1 - percentage));
	return (percentage) * (percentage) * (3 - 2 * (percentage));
}

void Gui::renderProgressIndicator( const bool isTouchInterface, const int screenWidth, const int screenHeight, float a ) {
	ItemInstance* currentItem = minecraft->player->inventory->getSelected();
	bool bowEquipped = currentItem != NULL ? currentItem->getItem() == Item::bow : false;
	bool itemInUse = currentItem != NULL ? currentItem->getItem() == minecraft->player->getUseItem()->getItem() : false;
	if ((!isTouchInterface || minecraft->options.getBooleanValue(OPTIONS_IS_JOY_TOUCH_AREA) 
		|| (bowEquipped && itemInUse)) && !minecraft->options.getBooleanValue(OPTIONS_HIDEGUI))
	{
			minecraft->textures->loadAndBindTexture("gui/icons.png");
			glEnable(GL_BLEND);
			glBlendFunc2(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
			blit(screenWidth/2 - 8, screenHeight/2 - 8, 0, 0, 16, 16);
			glDisable(GL_BLEND);
	} else if(!bowEquipped) {
		const float tprogress = minecraft->gameMode->destroyProgress;
		const float alpha = Mth::clamp(minecraft->inputHolder->alpha, 0.0f, 1.0f);
		//LOGI("alpha: %f\n", alpha);

		if (tprogress <= 0 && minecraft->inputHolder->alpha >= 0) {
			glDisable2(GL_TEXTURE_2D);
			glEnable2(GL_BLEND);
			glBlendFunc2(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			if (minecraft->hitResult.isHit())
				glColor4f2(1, 1, 1, 0.8f * alpha);
			else
				glColor4f2(1, 1, 1, Mth::Min(0.4f, alpha*0.4f));

			//LOGI("alpha2: %f\n", alpha);
			const float x = InvGuiScale * minecraft->inputHolder->mousex;
			const float y = InvGuiScale * minecraft->inputHolder->mousey;
			glTranslatef2(x, y, 0);
			drawArrayVT(rcFeedbackOuter.vboId, rcFeedbackOuter.vertexCount, 36);
			glTranslatef2(-x, -y, 0);

			glEnable2(GL_TEXTURE_2D);
			glDisable(GL_BLEND);
		} else if (tprogress > 0) {
			const float oProgress = minecraft->gameMode->oDestroyProgress;
			const float progress = 0.5f * (oProgress + (tprogress - oProgress) * a);

			//static Stopwatch w;
			//w.start();

			glDisable2(GL_TEXTURE_2D);
			glColor4f2(1, 1, 1, 0.8f * alpha);
			glEnable(GL_BLEND);
			glBlendFunc2(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			const float x = InvGuiScale * minecraft->inputHolder->mousex;
			const float y = InvGuiScale * minecraft->inputHolder->mousey;
			glPushMatrix2();
			glTranslatef2(x, y, 0);
			drawArrayVT(rcFeedbackOuter.vboId, rcFeedbackOuter.vertexCount, 36);
			glScalef2(0.5f + progress, 0.5f + progress, 1);
			//glDisable2(GL_CULL_FACE);
			glColor4f2(1, 1, 1, 1);
			glBlendFunc2(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
			drawArrayVT(rcFeedbackInner.vboId, rcFeedbackInner.vertexCount, 36, GL_TRIANGLE_FAN);
			glPopMatrix2();

			glDisable(GL_BLEND);
			glEnable2(GL_TEXTURE_2D);

			//w.stop();
			//w.printEvery(100, "feedback-r ");
		}
	}
}

void Gui::renderHearts() {
	bool blink = (minecraft->player->invulnerableTime / 3) % 2 == 1;
	if (minecraft->player->invulnerableTime < 10) blink = false;
	int h = minecraft->player->health;
	int oh = minecraft->player->lastHealth;
	random.setSeed(tickCount * 312871);

	int screenWidth = (int)(minecraft->width * InvGuiScale);
	int screenHeight = (int)(minecraft->height * InvGuiScale);

	int xx = (minecraft->options.getBooleanValue(OPTIONS_BAR_ON_TOP)) ? screenWidth / 2 - getNumSlots() * 10 - 1 : 2;

	int armor = minecraft->player->getArmorValue();
	for (int i = 0; i < Player::MAX_HEALTH / 2; i++) {
		int yo = (minecraft->options.getBooleanValue(OPTIONS_BAR_ON_TOP)) ? screenHeight - 32 : 2;
		int ip2 = i + i + 1;

		if (armor > 0) {
			int xo = xx + 80 + i * 8 + 4;
			if (ip2 < armor) blit(xo, yo, 16 + 2 * 9, 9 * 1, 9, 9);
			else if (ip2 == armor) blit(xo, yo, 16 + 4 * 9, 9 * 1, 9, 9);
			else if (ip2 > armor) blit(xo, yo, 16 + 0 * 9, 9 * 1, 9, 9);
		}

		int bg = 0;
		if (blink) bg = 1;
		int xo = xx + i * 8;
		if (h <= 4) {
			yo = yo + random.nextInt(2) - 1;
		}
		blit(xo, yo, 16 + bg * 9, 9 * 0, 9, 9);
		if (blink) {
			if (ip2 < oh) blit(xo, yo, 16 + 6 * 9, 9 * 0, 9, 9);
			else if (ip2 == oh) blit(xo, yo, 16 + 7 * 9, 9 * 0, 9, 9);
		}
		if (ip2 < h) blit(xo, yo, 16 + 4 * 9, 9 * 0, 9, 9);
		else if (ip2 == h) blit(xo, yo, 16 + 5 * 9, 9 * 0, 9, 9);
	}
}

void Gui::renderBubbles() {
	if (minecraft->player->isUnderLiquid(Material::water)) {
		int screenWidth = (int)(minecraft->width * InvGuiScale);
		int screenHeight = (int)(minecraft->height * InvGuiScale);

		int xx = (minecraft->options.getBooleanValue(OPTIONS_BAR_ON_TOP)) ? screenWidth / 2 - getNumSlots() * 10 - 1 : 2;
		int yo = (minecraft->options.getBooleanValue(OPTIONS_BAR_ON_TOP)) ? screenHeight - 42 : 12;
		int count = (int) std::ceil((minecraft->player->airSupply - 2) * 10.0f / Player::TOTAL_AIR_SUPPLY);
		int extra = (int) std::ceil((minecraft->player->airSupply) * 10.0f / Player::TOTAL_AIR_SUPPLY) - count;
		for (int i = 0; i < count + extra; i++) {
			int xo =  i * 8 + xx;
			if (i < count) blit(xo, yo, 16, 9 * 2, 9, 9);
			else blit(xo, yo, 16 + 9, 9 * 2, 9, 9);
		}
	}
}

static OffsetPosTranslator posTranslator;
void Gui::onLevelGenerated() {
	if (Level* level = minecraft->level) {
		Pos p = level->getSharedSpawnPos();
		posTranslator = OffsetPosTranslator((float)-p.x, (float)-p.y, (float)-p.z);
	}
}

void Gui::renderDebugInfo() {
	// FPS counter (updates once per second)
	static int fps = 0;
	static int fpsLastTime = 0;
	static int   fpsFrames = 0;
	static int   displayChunkUpdates = 0;
	float now = getTimeS();
	fpsFrames++;
	if (now - fpsLastTime >= 1) {
		fps = fpsFrames / (now - fpsLastTime);

		displayChunkUpdates = Chunk::updates;
    
    // 3. RESET the actual game counter to 0 for the next second
		Chunk::updates = 0;

		fpsFrames = 0;
		fpsLastTime = now;
	}

	LocalPlayer* p   = minecraft->player;
	Level*       lvl = minecraft->level;

	// Position
	float px = p->x, py = p->y - p->heightOffset, pz = p->z;
	int bx = (int)floorf(px), by = (int)floorf(py), bz = (int)floorf(pz);
	int cx = bx >> 4, cz = bz >> 4;

	// Facing direction
	float yMod = fmodf(p->yRot, 360.0f);
	if (yMod < 0) yMod += 360.0f;
	const char* facing;
	const char* axis;
	if      (yMod < 45  || yMod >= 315) { facing = "South"; axis = "+Z"; }
	else if (yMod < 135)                 { facing = "West";  axis = "-X"; }
	else if (yMod < 225)                 { facing = "North"; axis = "-Z"; }
	else                                 { facing = "East";  axis = "+X"; }

	// Biome
	const char* biomeName = "unknown";
	if (lvl) {
		Biome* biome = lvl->getBiome(bx, bz);
		if (biome) biomeName = biome->name.c_str();
	}

	// Time
	long worldTime = lvl ? lvl->getTime() : 0;
	long dayTime   = worldTime % Level::TICKS_PER_DAY;
	long day       = worldTime / Level::TICKS_PER_DAY;
	long seed      = lvl ? lvl->getSeed() : 0;

	// Block looking at
	std::string CurrentTile = "Air";
	if (minecraft->hitResult.type == TILE) {
		int LookingX = minecraft->hitResult.x;
		int LookingY = minecraft->hitResult.y;
		int LookingZ = minecraft->hitResult.z;

		int tileID = minecraft->level->getTile(LookingX, LookingY, LookingZ);
		if (tileID > 0 && tileID < 256){
			Tile* LookingTile = Tile::tiles[tileID];
			if (LookingTile != NULL) {
				CurrentTile = LookingTile->getDescriptionId();
			} else {
				CurrentTile = "Unknown Tile";
			}
		} else {
			CurrentTile = "Air";
		}
	}
	// Build lines (NULL entry = blank gap)
	Font* font = minecraft->font;

	// @todo - add our own debug screen as an option alongside the restored java one, why does renderdebug have to be so jank - shredder

	// if java beta's restored debug menu is enabled
	if (minecraft->options.getIntValue(OPTIONS_DEBUG_STYLE) == 0){

	char buf[128];

	sprintf(buf, "Minecraft - Pocket Edition (%d fps, %d chunk updates)", (int)fps, displayChunkUpdates);
	font->drawShadow(buf, 2, 2, 0xffffff);

	font->drawShadow(minecraft->gatherStats1(), 2, 12, 0xFFFFFF);
	font->drawShadow(minecraft->gatherStats2(), 2, 22, 0xFFFFFF);
    font->drawShadow(minecraft->gatherStats3(), 2, 32, 0xFFFFFF);
    font->drawShadow(minecraft->gatherStats4(), 2, 42, 0xFFFFFF);

	sprintf(buf, "x: %.8f", minecraft->player->x);
	drawString(font, buf, 2, 64, 0xE0E0E0);

	sprintf(buf, "y: %.8f", minecraft->player->y);
	drawString(font, buf, 2, 72, 0xE0E0E0);

	sprintf(buf, "z: %.8f", minecraft->player->z);
	drawString(font, buf, 2, 80, 0xE0E0E0);

	sprintf(buf, "f: %d",Mth::floor(minecraft->player->yRot * 4.0f / 360.0f + 0.5) & 0x3);
	drawString(font, buf, 2, 88, 0xE0E0E0);

	sprintf(buf, "Seed: %.ld", lvl->getSeed());
	drawString(font, buf, 2, 104, 0xE0E0E0);

	sprintf(buf, "Dimension: %d (%s)", lvl->dimension->id, lvl->dimension->getDimension().c_str());
	drawString(font, buf, 2, 114, 0xE0E0E0);

	sprintf(buf, "Biome: %s", biomeName);
	drawString(font, buf, 2, 124, 0xE0E0E0);

	sprintf(buf, "Looking at: %s", CurrentTile.c_str());
		drawString(font, buf, 2, 134, 0xE0E0E0);
	}
	else if (minecraft->options.getIntValue(OPTIONS_DEBUG_STYLE) == 1){
	

	static char ln[8][96];
	sprintf(ln[0], "Minecraft PE 0.6.1 alpha (mcpe64)");
	sprintf(ln[1], "%.1f fps", fps);
	ln[2][0] = '\0'; // blank separator
	sprintf(ln[3], "XYZ: %.3f / %.3f / %.3f", px, py, pz);
	sprintf(ln[4], "Block: %d %d %d   Chunk: %d %d", bx, by, bz, cx, cz);
	sprintf(ln[5], "Facing: %s (%s)  (%.1f / %.1f)", facing, axis, p->yRot, p->xRot);
	sprintf(ln[6], "Biome: %s", biomeName);
	sprintf(ln[7], "Day %ld  Time: %ld  Seed: %ld", day, dayTime, seed);

	const int N   = 8;
	const float LH  = (float)Font::DefaultLineHeight; // 10 font-pixels
	const float MGN = 2.0f;  // left/top margin in font-pixels
	const float PAD = 2.0f;  // horizontal padding for background
//	Font* font = minecraft->font;
	
	// 1) Draw semi-transparent background boxes behind each line
	for (int i = 0; i < N; i++) {
		if (ln[i][0] == '\0') continue;
		float w  = (float)font->width(ln[i]);
		float x0 = MGN - PAD;
		float y0 = MGN + i * LH - 1.0f;
		float x1 = MGN + w + PAD;
		float y1 = MGN + (i + 1) * LH - 1.0f;
		fill(x0, y0, x1, y1, 0x90000000);
	}

	// 2) Draw text (no extra scale — font coords are in GUI units, same as fill)
	Tesselator& t = Tesselator::instance;
	t.beginOverride();
	for (int i = 0; i < N; i++) {
		if (ln[i][0] == '\0') continue;
		float y = MGN + i * LH;
		int col = (i == 0) ? 0xffFFFF55 : 0xffffffff; // title yellow, rest white
		font->draw(ln[i], MGN, y, col);
	}
	t.endOverrideAndDraw();
	}
}

void Gui::renderPlayerList(Font* font, int screenWidth, int screenHeight) {
	// only show when in game, no other screen
	// if (!minecraft->level) return;

	// only show the overlay while connected to a multiplayer server
	Level* level = minecraft->level;
	if (!level) return;
	if (!level->isClientSide) return;

	std::vector<std::string> playerNames;
	playerNames.reserve(level->players.size());

	for (Player* player : level->players) {
		if (!player) continue;
		playerNames.push_back(player->name);
	}

	// is this check needed? if there are no players, the box won't render at all since height will be 0, 
	// but maybe we want to skip rendering entirely in that case
	// if (playerNames.empty())
	// 	return;

	std::sort(playerNames.begin(), playerNames.end());

	float maxNameWidth = 0.0f;
	// find the longest name so we can size the box accordingly
	for (const std::string& name : playerNames) {
		float nameWidth = font->width(name);
		if (nameWidth > maxNameWidth)
			maxNameWidth = nameWidth;
	}

	// player count title
	std::ostringstream titleStream;
	titleStream << "Players (" << playerNames.size() << ")";
	std::string titleText = titleStream.str();
	float titleWidth = font->width(titleText);

	if (titleWidth > maxNameWidth)
		maxNameWidth = titleWidth;

	const float padding = 4.0f;
	const float lineHeight = (float)Font::DefaultLineHeight;

	const float boxWidth = maxNameWidth + padding * 2;
	const float boxHeight = (playerNames.size() + 1) * lineHeight + padding * 2;

	const float boxLeft = (screenWidth - boxWidth) / 2.0f;
	const float boxTop = 10.0f;
	const float boxRight = boxLeft + boxWidth;
	const float boxBottom = boxTop + boxHeight;

	fill(boxLeft, boxTop, boxRight, boxBottom, 0x90000000);

	float titleX = (screenWidth - titleWidth) / 2.0f;
	float titleY = boxTop + padding;

	// scale the text down slightly
	// i think the gl scaling is the best for this
	// oh my god this looks really bad OH GOD
	//const float textScale = 0.8f;
	//const float invTextScale = 1.0f / textScale;
	//glPushMatrix2();
	//glScalef2(textScale, textScale, 1);

	// draw title
	//font->draw(titleText, titleX * invTextScale, titleY * invTextScale, 0xFFFFFFFF);
	font->draw(titleText, titleX, titleY, 0xFFFFFFFF);

	// draw player names
	// we should add ping icons here eventually, but for now just show names
	float currentY = boxTop + padding + lineHeight;
	for (const std::string& name : playerNames) {
		font->draw(name, (boxLeft + padding), currentY, 0xFFDDDDDD);
		currentY += lineHeight;
	}
	//glPopMatrix2();
}

void Gui::renderSleepAnimation( const int screenWidth, const int screenHeight ) {
	int timer = minecraft->player->getSleepTimer();
	float amount = (float) timer / (float) Player::SLEEP_DURATION;
	if (amount > 1) {
		// waking up
		amount = 1.0f - ((float) (timer - Player::SLEEP_DURATION) / (float) Player::WAKE_UP_DURATION);
	}

	int color = (int) (220.0f * amount) << 24 | (0x101020);
	fill(0, 0, screenWidth, screenHeight, color);
}

void Gui::renderOnSelectItemNameText( const int screenWidth, Font* font, int ySlot ) {
	if(itemNameOverlayTime < 1.0f) {
		ItemInstance* item = minecraft->player->inventory->getSelected();
		if(item != NULL) {
			float x = float(screenWidth / 2 - font->width(item->getName()) / 2);
			float y = float(ySlot - 22);
			int alpha = 255;
			if(itemNameOverlayTime > 0.75) {
				float time = 0.25f - (itemNameOverlayTime - 0.75f);
				float percentage = cubeSmoothStep(time *  4, 0.0f, 1.0f);
				alpha = int(percentage * 255);
			}
			if(alpha != 0)
				font->drawShadow(item->getName(), x, y, 0x00ffffff + (alpha << 24));
		}
	}
}



// helper structure used by drawColoredString
struct ColorSegment {
	std::string text;
	uint32_t color;
};

// parse [tag] and [/tag] markers; tags may contain a color name (gold, green, etc.)
static void parseColorTags(const std::string& in, std::vector<ColorSegment>& out) {
	uint32_t curColor = 0xffffff;
	size_t pos = 0;
	while (pos < in.size()) {
		size_t open = in.find('[', pos);
		if (open == std::string::npos) {
			out.push_back({in.substr(pos), curColor});
			break;
		}
		if (open > pos) {
			out.push_back({in.substr(pos, open - pos), curColor});
		}
		size_t close = in.find(']', open);
		if (close == std::string::npos) {
			out.push_back({in.substr(open), curColor});
			break;
		}
		std::string tag = in.substr(open + 1, close - open - 1);
		if (!tag.empty() && tag[0] == '/') {
			curColor = 0xffffff;
		} else {
			std::string lower;
			lower.resize(tag.size());
			std::transform(tag.begin(), tag.end(), lower.begin(), ::tolower);
			if (lower.find("gold") != std::string::npos) curColor = 0xffd700;
			else if (lower.find("green") != std::string::npos) curColor = 0x00ff00;
			else if (lower.find("yellow") != std::string::npos) curColor = 0xffff00;
			else if (lower.find("red") != std::string::npos) curColor = 0xff0000;
			else if (lower.find("blue") != std::string::npos) curColor = 0x0000ff;
		}
		pos = close + 1;
	}
}

void Gui::drawColoredString(Font* font, const std::string& text, float x, float y, int alpha) {
	std::vector<ColorSegment> segs;
	parseColorTags(text, segs);
	float cx = x;
	for (auto &s : segs) {
		int color = s.color + (alpha << 24);
		font->drawShadow(s.text, cx, y, color);
		cx += font->width(s.text);
	}
}

float Gui::getColoredWidth(Font* font, const std::string& text) {
	std::vector<ColorSegment> segs;
	parseColorTags(text, segs);
	float w = 0;
	for (auto &s : segs) {
		w += font->width(s.text);
	}
	return w;
}

void Gui::renderChatMessages( const int screenHeight, unsigned int max, bool isChatting, Font* font ) {
	//        if (minecraft.screen instanceof ChatScreen) {
	//            max = 20;
	//            isChatting = true;
	//        }
	//
	//        glEnable(GL_BLEND);
	//        glBlendFunc2(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//        glDisable(GL_ALPHA_TEST);
	//
	//        glPushMatrix2();
	//        glTranslatef2(0, screenHeight - 48, 0);
	//        // glScalef2(1.0f / ssc.scale, 1.0f / ssc.scale, 1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int baseY = screenHeight - 48;
	int start = chatScrollOffset;
	if (start < 0) start = 0;
	for (unsigned int i = 0; i < max; i++) {
		unsigned int msgIdx = (unsigned int)start + i;
		if (msgIdx >= guiMessages.size())
			break;

		GuiMessage& message = guiMessages.at(msgIdx);
		if (message.ticks < 20 * 10 || isChatting) {
			float t = message.ticks / (20 * 10.0f);
			t = 1 - t;
			t = t * 10;
			if (t < 0) t = 0;
			if (t > 1) t = 1;
			t = t * t;
			int alpha = (int) (255 * t);
			if (isChatting) alpha = 255;

			if (alpha > 0) {
				const float x = 2;
				const float y = (float)(baseY - i * 9);
				std::string msg = message.message;
				this->fill(x, y - 1, x + MAX_MESSAGE_WIDTH, y + 8, (alpha / 2) << 24);
				glEnable(GL_BLEND);

				// special-case join/leave announcements
				int baseColor = 0xffffff;
				if (msg.find(" joined the game") != std::string::npos ||
					msg.find(" left the game") != std::string::npos) {
						baseColor = 0xffff00; // yellow
				}
				// replace previous logic; allow full colour tags now
				Gui::drawColoredString(font, msg, x, y, alpha);
			}
		}
	}
}

void Gui::renderToolBar( float a, int ySlot, const int screenWidth ) {
	glColor4f2(1, 1, 1, 1);
	    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	minecraft->textures->loadAndBindTexture("gui/gui.png");

	Inventory* inventory = minecraft->player->inventory;

	int xBase, yBase;
	getSlotPos(0, xBase, yBase);
	const float baseItemX = (float)xBase + 3;
	const int slotsWidth = 20 * getNumSlots();
	// Left + right side of the selection bar
	blit(xBase, yBase, 0, 0, slotsWidth, 22);
	blit(xBase + slotsWidth, yBase, 180, 0, 2, 22);

	if (_currentDropSlot >= 0 && inventory->getItem(_currentDropSlot)) {
		int x = xBase + 3 +  _currentDropSlot * 20;
		int color = 0x8000ff00;
		int yy = (int)(17.0f * (_currentDropTicks + a) / DropTicks);

		if (_currentDropTicks >= 3) {
			glColor4f2(0, 1, 0, 0.5f);
		}
		fill(x, ySlot+16-yy, x+16, ySlot+16, color);
	}
	blit(xBase-1 + 20*inventory->selected, yBase - 1, 0, 22, 24, 22);
	glColor4f2(1, 1, 1, 1);

	// Flash a slot background
	if (_flashSlotId >= 0) {
		const float since = getTimeS() - _flashSlotStartTime;
		if (since > 0.2f) _flashSlotId = -1;
		else {
			int x = screenWidth / 2 - getNumSlots() * 10 + _flashSlotId * 20 + 2;
			int color = 0xffffff + (((int)(/*0x80 * since +*/ 0x51 - 0x50 * Mth::cos(10 * 6.28f * since))) << 24);
			//LOGI("Color: %.8x\n", color);
			fill(x, ySlot, x+16, ySlot+16, color);
		}
	}
	glColor4f2(1, 1, 1, 1);

	//static Stopwatch w;
	//w.start();

	Tesselator& t = Tesselator::instance;
	t.beginOverride();

	float x = baseItemX;

	int slots = getNumSlots() - _openInventorySlot;

	for (int i = 0; i < slots; i++) {
		renderSlot(i, (int)x, ySlot, a);
		x += 20;
	}
	_inventoryNeedsUpdate = false;


	if (_openInventorySlot) {
		blit(screenWidth / 2 + 10 * getNumSlots() - 20 + 4, ySlot + 6, 242, 252, 14, 4, 14, 4);
	}

	minecraft->textures->loadAndBindTexture("gui/gui_blocks.png");
	t.endOverrideAndDraw();

	// Render damaged items (@todo: investigate if it's faster by drawing in same batch)
	glDisable2(GL_DEPTH_TEST);
	glDisable2(GL_TEXTURE_2D);
	t.beginOverride();
	x = baseItemX;
	for (int i = 0; i < slots; i++) {
		ItemRenderer::renderGuiItemDecorations(minecraft->player->inventory->getItem(i), x, (float)ySlot);
		x += 20;
	}
	t.endOverrideAndDraw();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	//w.stop();
	//w.printEvery(100, "gui-slots");

	// Draw count
	//Tesselator& t = Tesselator::instance;

	const float k = 0.5f * GuiScale;
	if (minecraft->options.getBooleanValue(OPTIONS_JAVA_HUD)) // if true enables the java beta item count size and color and calls the java items decorations
	{
		t.beginOverride();
		if (minecraft->gameMode->isSurvivalType()) {
			x = baseItemX;
			for (int i = 0; i < slots; i++) {
				ItemInstance* item = minecraft->player->inventory->getItem(i);
				if (item && item->count >= 0)
					ItemRenderer::renderGuiItemDecorations(minecraft->font, minecraft->textures, minecraft->player->inventory->getItem(i), x, (float)ySlot);
				x += 20;
			}
		}
		minecraft->textures->loadAndBindTexture("font/default8.png");
		t.endOverrideAndDraw();
	}
	else { // otherwise uses the normal pocket edition one
		glPushMatrix2();
		glScalef2(InvGuiScale + InvGuiScale, InvGuiScale + InvGuiScale, 1);
		t.beginOverride();
		if (minecraft->gameMode->isSurvivalType()) {
			x = baseItemX;
			for (int i = 0; i < slots; i++) {
				ItemInstance* item = minecraft->player->inventory->getItem(i);
				if (item && item->count >= 0)
					renderSlotText(item, k*x, k*ySlot, true, true);
				x += 20;
			}
		}

		minecraft->textures->loadAndBindTexture("font/default8.png");
		t.endOverrideAndDraw();

		glPopMatrix2();
	}

}
