#include "Minecraft.h"
#include "client/Options.h"
#include "client/gamemode/GameMode.h"
#include "client/gui/screens/ChatScreen.h"
#include "client/gui/screens/ConsoleScreen.h"
#include "client/gui/screens/DeathScreen.h"
#include "client/gui/screens/UsernameScreen.h"
#include "client/player/LocalPlayer.h"
#include "client/player/input/IBuildInput.h"
#include "client/player/input/IInputHolder.h"
#include "client/player/input/KeyboardInput.h"
#include "client/player/input/MouseBuildInput.h"
#include "client/player/input/MouseTurnInput.h"
#include "client/player/input/touchscreen/TouchscreenInput.h"
#include "client/player/input/touchscreen/TouchInputHolder.h"
#include "client/renderer/Chunk.h"
#include "client/renderer/EntityTileRenderer.h"
#include "client/renderer/ItemInHandRenderer.h"
#include "client/renderer/Tesselator.h"
#include "client/renderer/entity/EntityRenderDispatcher.h"
#include "client/renderer/entity/ItemRenderer.h"
#include "client/renderer/ptexture/DynamicTexture.h"
#include "network/RakNetInstance.h"
#include "network/ServerSideNetworkHandler.h"
#include "network/packet/InteractPacket.h"
#include "network/packet/RespawnPacket.h"
#include "platform/CThread.h"
#include "platform/input/Multitouch.h"
#include "util/PerfRenderer.h"
#include "util/PerfTimer.h"
#include "world/level/Level.h"
#include "world/level/chunk/ChunkSource.h"
#include <MinecraftClient.h>

static void checkGlError(const char* tag) {
#ifdef GLDEBUG
	while (1) {
        const int errCode = glGetError();
        if (errCode == GL_NO_ERROR) break;

		LOGE("################\nOpenGL-error @ %s : #%d\n", tag, errCode);
	}
#endif /*GLDEBUG*/
}

MinecraftClient::~MinecraftClient() {
    delete levelRenderer;
	delete gameRenderer;
	delete particleEngine;

	delete soundEngine;

	delete inputHolder;
	delete _perfRenderer;

    delete font;
	delete textures;

	if (getScreen != NULL) {
		delete getScreen;
		getScreen = NULL;
	}

    EntityRenderDispatcher::destroy();

#ifdef PLATFORM_DESKTOP
	ItemRenderer::teardown_static();
	if (EntityTileRenderer::instance != NULL) {
		delete EntityTileRenderer::instance;
		EntityTileRenderer::instance = NULL;
	}
	TileEntityRenderDispatcher::destroy();
#endif
}

void MinecraftClient::init() {
	Minecraft::init();

	setSize(m_platform->getScreenWidth(), m_platform->getScreenHeight());

	initGLStates();
	Tesselator::instance.init();
	I18n::loadLanguage(m_platform, "en_US");

	checkGlError("Init enter");

	_supportsNonTouchscreen = !m_platform->supportsTouchscreen();

	LOGI("IS TOUCHSCREEN? %d\n", options.getBooleanValue(OPTIONS_USE_TOUCHSCREEN));

	textures = new Textures(&options, m_platform);
	textures->addDynamicTexture(new WaterTexture());
	textures->addDynamicTexture(new WaterSideTexture());
	gui.texturesLoaded(textures);

	levelRenderer = new LevelRenderer(this);
	gameRenderer = new GameRenderer(this);
	particleEngine = new ParticleEngine(level, textures);

	// Platform specific initialization here
	font = new Font(&options, "font/default8.png", textures);

	_perfRenderer = new PerfRenderer(this, font);

	checkGlError("Init complete");

	screenChooser.setScreen(SCREEN_STARTMENU);

	if (options.getBooleanValue(OPTIONS_FIRST_LAUNCH)) {
		options.toggle(OPTIONS_FIRST_LAUNCH);
		setScreen(new UsernameScreen());
	}

	reloadOptions();
}

void MinecraftClient::leaveGame(bool renameLevel /*=false*/) {
    if (isGeneratingLevel || !_hasSignaledGeneratingLevelFinished)
        return;
    
	isGeneratingLevel = false;
	bool saveLevel = level && (!level->isClientSide || renameLevel);

	raknetInstance->disconnect();
	if (saveLevel) {
		// If server or wanting to save level as client, save all unsaved chunks!
		level->getChunkSource()->saveAll(true);
	}

	LOGI("Clearing levels\n");

	cameraTargetPlayer = NULL;
	LOGI("Erasing callback\n");
	delete netCallback;
	netCallback = NULL;

	LOGI("Erasing level\n");
	if (level != NULL) {
		delete level->getLevelStorage();
		delete level;
		level = NULL;
	}
	//delete player;
	player = NULL;
	cameraTargetPlayer = NULL;

	_running = false;
}

void MinecraftClient::update() {
    m_frames++;

    // Generate Multitouch active pointer list
	Multitouch::commit();

    if (Options::debugGl)
		LOGI(">>>>>>>>>>\n");

	TIMER_PUSH("root");

    // If we're paused (local world / invisible server), freeze gameplay and
	// networking and only keep UI responsive.
	bool freezeGame = pause;

    if (!freezeGame) {
        Minecraft::update();
    }

    if (gameMode != NULL) {
		if (gamemode.destroyProgress <= 0) {
			m_gui.progress = 0;
			levelRenderer->destroyProgress = 0;
		} else {
			float dp = oDestroyProgress + (destroyProgress - oDestroyProgress) * a;
			m_gui.progress = dp;
			levelRenderer->destroyProgress = dp;
		}
	}

    TIMER_PUSH("sound");
    soundEngine->update(player, timer.a);
    TIMER_POP_PUSH("render");
    gameRenderer->render(timer.a);
    TIMER_POP();

    Multitouch::resetThisUpdate();

    checkGlError("Update finished");

	if (options.getBooleanValue(OPTIONS_RENDER_DEBUG)) {
#ifndef PLATFORM_DESKTOP
		if (!PerfTimer::enabled) {
			PerfTimer::reset();
			PerfTimer::enabled = true;
		}
		_perfRenderer->renderFpsMeter(1);
		checkGlError("render debug");
#endif
	} else {
		PerfTimer::enabled = false;
	}

    TIMER_POP();

    swapBuffers();
	Mouse::reset2();

#ifndef WIN32
	updateStats();
#endif
}

void MinecraftClient::updateStats() {
	if (Options::debugGl)
		LOGI("--------------------------------------------\n");

	//*
	int now = getTimeMs();
	//int since = now - _lastTickMs;

	if (now >= lastTime + 1000)
	{
		if (player) {
			LOGI("%d fps   \t%3d chunk updates.   (%.2f, %.2f, %.2f)\n",
				m_frames, Chunk::updates, player->x, player->y, player->z);
			Chunk::resetUpdates();

			//static int _n = 0;
			//if (++_n % 5 == -1) { // @note: -1
			//	static char filename[256];
			//	sprintf(filename, "%s/games/com.mojang/img_%.4d.jpg", externalStoragePath.c_str(), _n/5);
			//	_context.platform->saveScreenshot(filename, width, height);
			//}

			LOGI("%s", levelRenderer->gatherStats1().c_str());
			//printf("Texture swaps (this frame): %d\n", Textures::textureChanges);
		} else {
			LOGI("%d fps\n", m_frames);
		}

		//const int* pointerIds;
		//int pointerCount = Multitouch::getActivePointerIds(&pointerIds);
		//if (pointerCount) {
		//	std::string s = "Pointers (";
		//	s += (char)(48 + pointerCount);
		//	s += ": ";
		//	for (int i = 0; i < pointerCount; ++i) {
		//		s += (char)(48 + pointerIds[i]);
		//		s += ", ";
		//	}
		//	LOGI("%s\n", s.c_str());
		//}

		lastTime = now;
		m_frames = 0;
#ifdef GLDEBUG
		while (1) {
			int error = glGetError();
			if (error == GL_NO_ERROR) break;

			LOGI("#################### GL-ERROR: %d\t#####################\n", error);
			LOGI("#################### GL-ERROR: %d\t#####################\n", error);
			LOGI("#################### GL-ERROR: %d\t#####################\n", error);
		}
#endif
	}

	Textures::textureChanges = 0;
}

void MinecraftClient::restartServer() {
	Minecraft::restartServer();

	gui.addMessage("This server has restarted!");

	// @fix maybe code below should run also on server

	for (int i = level->players.size()-1; i >= 0; --i) {
        Player* p = level->players[i];
        if (p != player)
            level->removeEntity(p);
    }
}


bool MinecraftClient::handleBack(bool isDown) {
	if (isGeneratingLevel)
	{
		return true;
	}
	if (level)
	{
		if (!isDown)
		{
			if (getScreen)
			{
				if (!getScreen->handleBackEvent(isDown))
				{
					if (player->containerMenu) player->closeContainer();
					setScreen(NULL);
				}
				return true;
			} else {
				pauseGame(true);
			}
			//leaveGame();
			return false;
		}
		return true;
	}
	else if (getScreen)
	{
		return getScreen->handleBackEvent(isDown);
	}
	return false;
}

void MinecraftClient::onGraphicsReset() {
	initGLStates();
	Tesselator::instance.init();

	m_textures.clear();
	
	font->onGraphicsReset();
	gui.onGraphicsReset();

	if (levelRenderer) levelRenderer->onGraphicsReset();
	if (gameRenderer) gameRenderer->onGraphicsReset();

	EntityRenderDispatcher::getInstance()->onGraphicsReset();
	TileEntityRenderDispatcher::getInstance()->onGraphicsReset();
}

void MinecraftClient::tick(int nTick, int maxTick) {
	if (!pause) Minecraft::tick(nTick, maxTick);

	if (!getScreen && player) {
		if (player->health <= 0) {
			setScreen(new DeathScreen());
		}
	}

	TIMER_PUSH("input");
	tickInput();

	TIMER_POP_PUSH("gui");
	gui.tick();

	//
	// Ongoing level generation in a (perhaps) different thread. When it's
	// ready, _levelGenerated() is called once and any threads are deleted.
	//
	if (isGeneratingLevel) {
		return;
	} else if (!_hasSignaledGeneratingLevelFinished) {
		if (generateLevelThread) {
			delete generateLevelThread;
			generateLevelThread = NULL;
		}
		_levelGenerated();
	}

	//
	// Normal game loop, run before or efter level generation
	//
	if (level != NULL && !pause) {
		TIMER_POP_PUSH("gameRenderer");
		gameRenderer->tick(nTick, maxTick);

		TIMER_POP_PUSH("levelRenderer");
		levelRenderer->tick();

		TIMER_POP_PUSH("animateTick");
		if (player) {
			level->animateTick(Mth::floor(player->x), Mth::floor(player->y), Mth::floor(player->z));
		}
	}

	m_textures.loadAndBindTexture("terrain.png");
	
	if (!pause && !(getScreen && !getScreen->renderGameBehind())) {
		#if !defined(RPI)
			#ifdef __APPLE__
			if (isSuperFast())
			#endif
			{
				if (nTick == maxTick) {
					TIMER_POP_PUSH("textures");
					m_textures.tick(true);
				}
			}
		#endif
	}
	TIMER_POP_PUSH("particles");

	if (!pause) {
		particleEngine->tick();
	}
	
	if (getScreen) {
		screenMutex = true;
		getScreen->tick();
		screenMutex = false;
	}

	// @note: fix to keep "isPressed" and "isReleased" as long as necessary.
	//      Most likely keyboard and mouse could/should be reset here as well.
	Multitouch::reset();
	TIMER_POP();
}


class InputRAII {
public:
	~InputRAII() {
		Mouse::reset();
		Keyboard::reset();
	}
};

void MinecraftClient::tickInput() {
	InputRAII raiiInput;

	// input handling code here...
	if (getScreen && !getScreen->passEvents) {
		screenMutex = true;
		getScreen->updateEvents();
		//screen->updateSetScreen();
		screenMutex = false;
		if (hasScheduledScreen) {
			setScreen(scheduledScreen);
			scheduledScreen = NULL;
			hasScheduledScreen = false;
		}
		return;
	}

	if (!player) {
		return;
	}

#ifdef RPI
	bool mouseDiggable = true;
	bool allowGuiClicks = !mouseGrabbed;
#else
	bool mouseDiggable = !gui.isInside(Mouse::getX(), Mouse::getY());
	bool allowGuiClicks = true;
#endif

	TIMER_PUSH("mouse");
	while (Mouse::next()) {
		//if (Mouse::getButtonState(MouseAction::ACTION_LEFT))
		//	LOGI("mouse-down-at: %d, %d\n", Mouse::getX(), Mouse::getY());
        	int passedTime = getTimeMs() - lastTickTime;
        	if (passedTime > 200) continue; // @note: As long Mouse::clear CLEARS the whole buffer, it's safe to break here
		// But since it might be rewritten anyway (and hopefully there aren't a lot of messages, we just continue.

		const MouseAction& e = Mouse::getEvent();

		if (!useTouchscreen() && !mouseGrabbed) {
			if (!getScreen && e.data == MouseAction::DATA_DOWN) {
				grabMouse();
			}
		}

		if (allowGuiClicks && e.action == MouseAction::ACTION_LEFT && e.data == MouseAction::DATA_DOWN) {
			gui.handleClick(MouseAction::ACTION_LEFT, Mouse::getX(), Mouse::getY());
		}

		if (e.action == MouseAction::ACTION_WHEEL) {
			// If chat/console is open, use the wheel to scroll through chat history.
			if (getScreen && (dynamic_cast<ChatScreen*>(getScreen) || dynamic_cast<ConsoleScreen*>(getScreen))) {
				gui.scrollChat(e.dy);
			} else {
				Inventory* v = player->inventory;

				int numSlots = gui.getNumSlots();
				if (!useTouchscreen()) numSlots--;

				int slot = (v->selected - e.dy + numSlots) % numSlots;
				v->selectSlot(slot);
			}
		}
		/*
		if (mouseDiggable && options.useMouseForDigging) {
			if (Mouse::getEventButton() == MouseAction::ACTION_LEFT && Mouse::getEventButtonState()) {
				handleMouseClick(MouseAction::ACTION_LEFT);
				lastClickTick = ticks;
			}
			if (Mouse::getEventButton() == MouseAction::ACTION_RIGHT && Mouse::getEventButtonState()) {
				handleMouseClick(MouseAction::ACTION_RIGHT);
				lastClickTick = ticks;
			}
		}
		*/
	}

	TIMER_POP_PUSH("keyboard");
	while (Keyboard::next()) {
		int key = Keyboard::getEventKey();
		bool isPressed = (Keyboard::getEventKeyState() == KeyboardAction::KEYDOWN);
		player->setKey(key, isPressed);

		if (isPressed) {
			gui.handleKeyPressed(key);

			if (key >= '0' && key <= '9') {
				int digit = key - '0';
				int slot = digit - 1;

				if (slot >= 0 && slot < gui.getNumSlots())
					player->inventory->selectSlot(slot);

				#if defined(WIN32)
					if (digit >= 1 && GetAsyncKeyState(VK_CONTROL) < 0) {
						// Set adventure settings here!
						AdventureSettingsPacket p(level->adventureSettings);
						p.toggle((AdventureSettingsPacket::Flags)(1 << slot));
						p.fillIn(level->adventureSettings);
						raknetInstance->send(p);
					}
					if (digit == 0) {
						Pos pos((int)player->x, (int)player->y-1, (int)player->z);
						SetSpawnPositionPacket p(pos);
						raknetInstance->send(p);
					}
				#endif
			}

			if (key == Keyboard::KEY_LEFT_CTRL) {
				player->setSprinting(true);
			}

			if (key == Keyboard::KEY_E) {
				screenChooser.setScreen(SCREEN_BLOCKSELECTION);
			}

			if (!getScreen && key == Keyboard::KEY_T && level) {
				setScreen(new ConsoleScreen());
			}

			if (key == Keyboard::KEY_F3) {
				options.toggle(OPTIONS_RENDER_DEBUG);
			}

			if (key == Keyboard::KEY_F5) {
				options.toggle(OPTIONS_THIRD_PERSON_VIEW);
				/*
				ImprovedNoise noise;
				for (int i = 0; i < 16; ++i)
					printf("%d\t%f\n", i, noise.grad2(i, 3, 8));
				*/
			}

			if (!getScreen && key == Keyboard::KEY_O || key == 250) {
				releaseMouse();
			}

			if (key == Keyboard::KEY_F) {
				int dst = options.getIntValue(OPTIONS_VIEW_DISTANCE);
				options.set(OPTIONS_VIEW_DISTANCE, (dst + 1) % 4);
			}
			#ifdef CHEATS
				if (key == Keyboard::KEY_U) {
					onGraphicsReset();
					player->heal(100);
				}

				if (key == Keyboard::KEY_B || key == 108) // Toggle the game mode
					setIsCreativeMode(!isCreativeMode());

				if (key == Keyboard::KEY_P) // Step forward in time
					level->setTime( level->getTime() + 1000);

				if (key == Keyboard::KEY_G) {
					setScreen(new ArmorScreen());
					/*
					std::vector<AABB>& boxs = level->getCubes(NULL, AABB(128.1f, 73, 128.1f, 128.9f, 74.9f, 128.9f));
					LOGI("boxes: %d\n", (int)boxs.size());
					*/
				}

				if (key == Keyboard::KEY_Y) {
					textures->reloadAll();
					player->hurtTo(2);
				}
				if (key == Keyboard::KEY_Z || key == 108) {
					for (int i = 0; i < 1; ++i) {
						Mob* mob = NULL;
						int forceId = 0;//MobTypes::Sheep;

						int types[] = {
							MobTypes::Sheep,
							MobTypes::Pig,
							MobTypes::Chicken,
							MobTypes::Cow,
						};

						int mobType = (forceId > 0)? forceId : types[Mth::random(sizeof(types) / sizeof(int))];
						mob = MobFactory::CreateMob(mobType, level);

						//((Animal*)mob)->setAge(-1000);
						float dx = 4 - 8 * Mth::random() + 4 * Mth::sin(Mth::DEGRAD * player->yRot);
						float dz = 4 - 8 * Mth::random() + 4 * Mth::cos(Mth::DEGRAD * player->yRot);
						if (mob && !MobSpawner::addMob(level, mob, player->x + dx, player->y, player->z + dz, Mth::random()*360, 0, true))
							delete mob;
					}
				}

				if (key == Keyboard::KEY_X) {
					const EntityList& entities = level->getAllEntities();
					for (int i = entities.size()-1; i >= 0; --i) {
						Entity* e = entities[i];
						if (!e->isPlayer())
							level->removeEntity(e);
					}
				}

				if (key == Keyboard::KEY_C /*|| key == 4*/) {
					player->inventory->clearInventoryWithDefault();
					// @todo: Add saving here for benchmarking
				}
				if (key == Keyboard::KEY_H) {
					setScreen( new PrerenderTilesScreen() );
				}

				if (key == Keyboard::KEY_O) {
					for (int i = Inventory::MAX_SELECTION_SIZE; i < player->inventory->getContainerSize(); ++i)
						if (player->inventory->getItem(i))
							player->inventory->dropSlot(i, false);
				}
				if (key == Keyboard::KEY_M) {
					Difficulty difficulty = (Difficulty)options.getIntValue(OPTIONS_DIFFICULTY);
					options.set(OPTIONS_DIFFICULTY, (difficulty == Difficulty::PEACEFUL)?
						Difficulty::NORMAL : Difficulty::PEACEFUL);
					//setIsCreativeMode( !isCreativeMode() );
				}

				if (options.getBooleanValue(OPTIONS_RENDER_DEBUG)) {
					if (key >= '0' && key <= '9') {
						_perfRenderer->debugFpsMeterKeyPress(key - '0');
					}
				}
			#endif

			if (key == Keyboard::KEY_ESCAPE)
				pauseGame(false);

			#ifndef OPENGL_ES
				if (key == Keyboard::KEY_P) {
					static bool isWireFrame = false;
					isWireFrame = !isWireFrame;
					glPolygonMode(GL_FRONT, isWireFrame? GL_LINE : GL_FILL);
					//glPolygonMode(GL_BACK, isWireFrame? GL_LINE : GL_FILL);
				}
			#endif
		}
		#ifdef WIN32
			if (key == Keyboard::KEY_M) {
				for (int i = 0; i < 5 * SharedConstants::TicksPerSecond; ++i)
					level->tick();
			}
		#endif
	}

	TIMER_POP_PUSH("handlemouse");
	
	static bool prevMouseDownLeft = false;

	if (Mouse::getButtonState(MouseAction::ACTION_LEFT) == 0) {
		gameMode->stopDestroyBlock();
	}

	if (!Mouse::isButtonDown(MouseAction::ACTION_RIGHT)) {
		gameMode->releaseUsingItem(player);
	}

	if (useTouchscreen()) {
		// Touch: gesture recognizer classifies the action type (turn/destroy/build)
		BuildActionIntention bai;

		if (inputHolder && inputHolder->getBuildInput()->tickBuild(player, &bai)) {
			handleBuildAction(&bai);
		} else {
			gameMode->stopDestroyBlock();
		}
	} else {
		// Desktop: left mouse = destroy/attack
		if (Mouse::isButtonDown(MouseAction::ACTION_LEFT)) {
			auto baiFlags = BuildActionIntention::BAI_REMOVE | BuildActionIntention::BAI_ATTACK;

			if (!prevMouseDownLeft) baiFlags |= BuildActionIntention::BAI_FIRSTREMOVE;

			BuildActionIntention bai(baiFlags);
			handleBuildAction(&bai);
		}

		prevMouseDownLeft = Mouse::isButtonDown(MouseAction::ACTION_LEFT);

		// Build and use/interact is on same button
		// USPESHNO spizheno
		static int buildHoldTicks = 0;
		if (Mouse::isButtonDown(MouseAction::ACTION_RIGHT)) {
			if (buildHoldTicks >= 5) buildHoldTicks = 0;

			if (++buildHoldTicks == 1) {
				BuildActionIntention bai(BuildActionIntention::BAI_BUILD | BuildActionIntention::BAI_INTERACT);
				handleBuildAction(&bai);
			}
		} else {
			buildHoldTicks = 0;
			gameMode->releaseUsingItem(player);
		}
	}
	// after input handling

	lastTickTime = getTimeMs();

	// we have (hopefully) handled the keyboard & mouse queue and it
	// can now be emptied. If wanted, the reset could be changed to:
	// index -= numRead; // then this code doesn't have to be placed here
	// + it prepares for tick not handling all or any events.
	// update: RAII'ing instead, see above
	//Keyboard::reset();
	//Mouse::reset();

	TIMER_POP();
}

void MinecraftClient::handleBuildAction(BuildActionIntention* action) {
	if (action->isRemove()) {
		if (missTime > 0) return;
		player->swing();
	}
	if(player->isUsingItem())
		return;
    bool mayUse = true;

	if (!hitResult.isHit()) {
		if (action->isRemove() && !gameMode->isCreativeType()) {
			missTime = 10;
		}
    } else if (hitResult.type == ENTITY) {
        if (action->isAttack()) {
			player->swing();
			//LOGI("attacking!\n");
			InteractPacket packet(InteractPacket::Attack, player->entityId, hitResult.entity->entityId);
			raknetInstance->send(packet);
            gameMode->attack(player, hitResult.entity);
		} else if (action->isInteract()) {
			if (hitResult.entity->interactPreventDefault())
				mayUse = false;
			//LOGI("interacting!\n");
			InteractPacket packet(InteractPacket::Interact, player->entityId, hitResult.entity->entityId);
			raknetInstance->send(packet);
            gameMode->interact(player, hitResult.entity);
        }
    } else if (hitResult.type == TILE) {
		int x = hitResult.x;
        int y = hitResult.y;
        int z = hitResult.z;
        int face = hitResult.f;

		int oldTileId = level->getTile(x, y, z);
        Tile* oldTile = Tile::tiles[oldTileId];

		//bool tryDestroyBlock = false;

		if (action->isRemove()) {
			if (!oldTile)
				return;

			//LOGI("tile: %s - %d, %d, %d. b: %f - %f\n", oldTile->getDescriptionId().c_str(), x, y, z, oldTile->getBrightness(level, x, y, z), oldTile->getBrightness(level, x, y+1, z));
            level->extinguishFire(x, y, z, hitResult.f);
			
			if (action->isFirstRemove()) {
				gameMode->startDestroyBlock(x, y, z, hitResult.f);
			} else {
				gameMode->continueDestroyBlock(x, y, z, hitResult.f);
			}

			particleEngine->crack(x, y, z, hitResult.f);
			player->swing();
        }
		else {
			ItemInstance* item = player->inventory->getSelected();
 			if (gameMode->useItemOn(player, level, item, x, y, z, face, hitResult.pos)) {
			    mayUse = false;
			    player->swing();
			#ifdef RPI
				} else if (item && item->id == ((Item*)Item::sword_iron)->id) {
					player->swing();
			#endif
			}
			if (item && item->count <= 0) {
				player->inventory->clearSlot(player->inventory->selected);
			}
			//} else if (item && item->count != oldCount) {
			//	  gameRenderer->itemInHandRenderer->itemPlaced();
			//}
		}
	}

	if (mayUse && action->isInteract()) {
		ItemInstance* item = player->inventory->getSelected();
		if (item && !player->isUsingItem()) {
			if (gameMode->useItem(player, level, item)) {
				gameRenderer->itemInHandRenderer->itemUsed();
			}

			if (item && item->count <= 0) {
				player->inventory->clearSlot(player->inventory->selected);
			}
		}
	}
}

bool MinecraftClient::isOnlineClient() {
	return (level && level->isClientSide);
}

void MinecraftClient::pauseGame(bool isBackPaused) {
	// Only freeze gameplay when running a local server and it is not accepting
	// incoming connections (invisible server), which includes typical single-
	// player/lobby mode. If the server is visible, the game should keep ticking.
	bool canFreeze = false;
	if (raknetInstance && raknetInstance->isServer() && netCallback) {
		ServerSideNetworkHandler* ss = (ServerSideNetworkHandler*) netCallback;
		if (!ss->allowsIncomingConnections())
			canFreeze = true;
	}
	pause = canFreeze;

	if (getScreen != NULL) return;
	screenChooser.setScreen(isBackPaused? SCREEN_PAUSEPREV : SCREEN_PAUSE);
}

void MinecraftClient::gameLostFocus() {
	if(getScreen != NULL) {
		getScreen->lostFocus();
	}
}

void MinecraftClient::setScreen( Screen* screen ) {
	Mouse::reset();
	Multitouch::reset();
	Multitouch::resetThisUpdate();

	if (screenMutex) {
		hasScheduledScreen = true;
		scheduledScreen = screen;
		return;
	}

	if (screen != NULL && screen->isErrorScreen())
		return;
	if (screen == NULL && level == NULL)
		screen = screenChooser.createScreen(SCREEN_STARTMENU);

	if (this->getScreen != NULL) {
		this->getScreen->removed();
		delete this->getScreen;
	}

	this->getScreen = screen;
	if (screen != NULL) {
		releaseMouse();
		//ScreenSizeCalculator ssc = new ScreenSizeCalculator(options, width, height);
		int screenWidth = (int)(width * Gui::InvGuiScale); //ssc.getWidth();
		int screenHeight = (int)(height * Gui::InvGuiScale); //ssc.getHeight();
		screen->init(this, screenWidth, screenHeight);

		if (screen->isInGameScreen() && level) {
			level->saveLevelData();
            level->saveGame();
        }

		//noRender = false;
	} else {
		// Closing a screen and returning to the game should unpause.
		pause = false;
		grabMouse();
	}
}

void MinecraftClient::grabMouse() {
	if (mouseGrabbed) return;
	mouseGrabbed = true;
	mouseHandler.grab();
	//setScreen(NULL);
}

void MinecraftClient::releaseMouse() {
	if (!mouseGrabbed) {
		return;
	}
	if (player) {
		player->releaseAllKeys();
	}
	mouseGrabbed = false;
	mouseHandler.release();
}

bool MinecraftClient::useTouchscreen() {
#ifdef RPI
	return false;
#endif
	return options.getBooleanValue(OPTIONS_USE_TOUCHSCREEN) && !_supportsNonTouchscreen;
}

bool MinecraftClient::supportNonTouchScreen() {
	return _supportsNonTouchscreen;
}

void MinecraftClient::setSize(int w, int h) {
    transformResolution(&w, &h);

	width  = w;
	height = h;

	int guiScale = options.getIntValue(OPTIONS_GUI_SCALE);

	// determine gui scale, optionally overriding auto
	if (guiScale != 0) {
		// manual selection: 1->small, 2->medium, 3->large, 4->larger, 5->largest
		switch (guiScale) {
		case 1: Gui::GuiScale = 2.0f; break;
		case 2: Gui::GuiScale = 3.0f; break;
		case 3: Gui::GuiScale = 4.0f; break;
		case 4: Gui::GuiScale = 5.0f; break;
		case 5: Gui::GuiScale = 6.0f; break;
		default: Gui::GuiScale = 1.0f; break; // auto
		}
	} else {
		// auto compute from resolution
		if (width >= 1000) {
        #ifdef __APPLE__
            Gui::GuiScale = (width > 2000)? 8.0f : 4.0f;
        #else
            Gui::GuiScale = 4.0f;
        #endif
    }
		else if (width >= 800) {
#ifdef __APPLE__
        Gui::GuiScale = 4.0f;
#else
		Gui::GuiScale = 3.0f;
#endif
    }
		else if (width >= 400)
			Gui::GuiScale = 2.0f;
		else
			Gui::GuiScale = 1.0f;
	}

	Gui::InvGuiScale = 1.0f / Gui::GuiScale;
	int screenWidth  = (int)(width  * Gui::InvGuiScale);
	int screenHeight = (int)(height * Gui::InvGuiScale);

	// if (platform()) {
	// 	float pixelsPerMillimeter = options.getProgressValue(&Options::Option::PIXELS_PER_MILLIMETER);
	// 	pixelCalc.setPixelsPerMillimeter(pixelsPerMillimeter);
	// 	pixelCalcUi.setPixelsPerMillimeter(pixelsPerMillimeter * Gui::InvGuiScale);
	// }

	Config config = createConfig(this);
	gui.onConfigChanged(config);
    
	if (getScreen)
		getScreen->setSize(screenWidth, screenHeight);

	if (inputHolder)
		inputHolder->onConfigChanged(config);
	//LOGI("Setting size: %d, %d: %f\n", width, height, Gui::InvGuiScale);
}

void MinecraftClient::reloadOptions() {
	options.save();
	bool wasTouchscreen = options.getBooleanValue(OPTIONS_USE_TOUCHSCREEN);
	options.set(OPTIONS_USE_TOUCHSCREEN, useTouchscreen());
	options.save();

	if ((wasTouchscreen != useTouchscreen()) || (inputHolder == 0))
		_reloadInput();

	// user->name = options.username;

	LOGI("Reloading-options\n");

    // @todo @fix Android and iOS behaves a bit differently when leaving
    //            an options screen (Android recreates OpenGL surface)
    setSize(width, height);
}

void MinecraftClient::_reloadInput() {
	delete inputHolder;

	const bool useTouchHolder = useTouchscreen();
	if (useTouchHolder) {
		inputHolder = new TouchInputHolder(this, &options);
	} else {
		#if defined(ANDROID) || defined(__APPLE__) 
			inputHolder = new CustomInputHolder(
				new XperiaPlayInput(&options),
				new ControllerTurnInput(2, ControllerTurnInput::MODE_DELTA),
				new IBuildInput());
		#else
			inputHolder = new CustomInputHolder(
				new KeyboardInput(&options),
				new MouseTurnInput(MouseTurnInput::MODE_DELTA, width/2, height/2),
				new MouseBuildInput());
		#endif
	}

	mouseHandler.setTurnInput(inputHolder->getTurnInput());
	if (level && player) {
		player->input = inputHolder->getMoveInput();
	}
}

void MinecraftClient::initGLStates() {
	//glShadeModel2(GL_SMOOTH);
	//glClearDepthf(1.0f);
	glEnable2(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthRangef(0, 1);
	glEnable2(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable2(GL_TEXTURE_2D);
#ifndef _EMSCRIPTEN_
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
#endif
	// Both updates isPowerVR flag in java and returns if the graphics chip is PowerVR SGX or not
	_powerVr = platform()->isPowerVR();
#ifdef __APPLE__
    _isSuperFast = platform()->isSuperFast();
#endif
}

void MinecraftClient::_levelGenerated() {
	Minecraft::_levelGenerated();

	level->loadPlayer(player, true);
	// if we are client side, we trust the server to have given us a correct position
	if (player && !level->isClientSide) {
		player->resetPos(false);
	}

	this->cameraTargetPlayer = player;

	if (player == NULL) {
		player = new LocalPlayer(minecraft, level, minecraft.options.getStringValue(OPTIONS_USERNAME), level->dimension->id, isCreativeType());
		gameMode->initPlayer(player);
	}

	if (player) {
		player->input = inputHolder->getMoveInput();
	}

	if (levelRenderer != NULL) levelRenderer->setLevel(level);
	if (particleEngine != NULL) particleEngine->setLevel(level);

	gameMode->adjustPlayer(player);
	gui.onLevelGenerated();

#if defined(WIN32) || defined(RPI)
	if (_commandServer) {
		delete _commandServer;
	}
	_commandServer = new CommandServer(this);
	_commandServer->init(commandPort);
#endif
}

void MinecraftClient::respawnPlayer() {
	// RESET THE FRACKING PLAYER HERE
	//bool slowCheck = false;
	//for (int i = 0; i < level->entities.size(); ++i)
	//	if (level->entities[i] == player) slowCheck = true;
	//
	//LOGI("Has entity? %d, %d\n", level->getEntity(player->entityId), slowCheck);

	resetPlayer(player);

	// tell server (or other client) that we re-spawned
	RespawnPacket packet(player);
	raknetInstance->send(packet);
}

void MinecraftClient::audioEngineOn() {
    soundEngine->enable(true);
}

void MinecraftClient::audioEngineOff() {
    soundEngine->enable(false);
}

void MinecraftClient::setIsCreativeMode(bool isCreative) {
	Minecraft::setIsCreativeMode(isCreative);

	if (player)
		gameMode->initAbilities(player->abilities);
}

void MinecraftClient::optionUpdated(OptionId option, bool value ) {
	Minecraft::optionUpdated(option, value);

	if (option == OPTIONS_USE_TOUCHSCREEN) {
		_reloadInput();
	} else if (option == OPTIONS_VSYNC) {
		m_platform->setVSync(value);
	}
}

void MinecraftClient::optionUpdated(OptionId option, float value ) {
	Minecraft::optionUpdated(option, value);
// #ifndef STANDALONE_SERVER
// 	if(option == OPTIONS_PIXELS_PER_MILLIMETER) {
// 		pixelCalcUi.setPixelsPerMillimeter(value * Gui::InvGuiScale);
// 		pixelCalc.setPixelsPerMillimeter(value);
// 	}
// #endif
}

void MinecraftClient::optionUpdated(OptionId option, int value ) {
	Minecraft::optionUpdated(option, value);

    if(option == OPTIONS_GUI_SCALE) {
        // reapply screen scaling using current window size
        setSize(width, height);
    }

	if (option == OPTIONS_DIFFICULTY && level) {
		level->difficulty = options.getIntValue(OPTIONS_DIFFICULTY);
	}
}

// @attn shittttt
void MinecraftClient::setLevel(Level* level, const std::string& message /* ="" */, Player* forceInsertPlayer /* = NULL */) {
	Minecraft::setLevel(level, message, forceInsertPlayer);
	cameraTargetPlayer = NULL;
	
	if (level != NULL) {
		if (!player && forceInsertPlayer)
		{
			player = forceInsertPlayer;
			player->resetPos(false);
			//level->addEntity(forceInsertPlayer);
		}
		else if (player != NULL) {
			player->resetPos(false);
			if (level != NULL) {
				level->addEntity(player);
			}
		}
    } else {
        player = NULL;
    }
}

void MinecraftClient::hostMultiplayer(int port) {
	Minecraft::hostMultiplayer(port);
	raknetInstance->host(options.getStringValue(OPTIONS_USERNAME), port);
}

std::string MinecraftClient::getServerName() {
	return options.getStringValue(OPTIONS_USERNAME);
}

void MinecraftClient::locateMultiplayer() {
	isLookingForMultiplayer = true;

	raknetInstance->pingForHosts(19132);
	netCallback = new ClientSideNetworkHandler(this, raknetInstance);
}

void MinecraftClient::cancelLocateMultiplayer() {
	isLookingForMultiplayer = false;

	raknetInstance->stopPingForHosts();

	delete netCallback;
	netCallback = NULL;
}

bool MinecraftClient::joinMultiplayer( const PingedCompatibleServer& server ) {
	if (isLookingForMultiplayer && netCallback) {
		isLookingForMultiplayer = false;
		return raknetInstance->connect(server.address.ToString(false), server.address.GetPort());
	}
	return false;
}

bool MinecraftClient::joinMultiplayerFromString( const std::string& server ) {	
	std::string ip = "";
	std::string port = "19132";
	
	size_t pos = server.find(":");

	if (pos != std::string::npos) {
		ip = server.substr(0, pos);
		port = server.substr(pos + 1);
	} else {
		ip = server;
	}

	printf("%s \n", port.c_str());
	
	if (isLookingForMultiplayer && netCallback) {
		isLookingForMultiplayer = false;
		printf("test");
		int portNum = atoi(port.c_str());
		return raknetInstance->connect(ip.c_str(), portNum);
	}
	return false;
}

bool MinecraftClient::isKindleFire(int kindleVersion) {
	// if (kindleVersion != 1)
	// 	return false;

	// std::string model = platform()->getPlatformStringVar(PlatformStringVars::DEVICE_BUILD_MODEL);
	// std::string modelLower(model);
	// std::transform(modelLower.begin(), modelLower.end(), modelLower.begin(), tolower);

	// return (modelLower.find("kindle") != std::string::npos) && (modelLower.find("fire") != std::string::npos);

	return false;
}

bool MinecraftClient::transformResolution(int* w, int* h)
{
	bool changed = false;

	// Kindle Fire 1: reporting wrong height in
	// certain cases (e.g. after screen lock)
	if (isKindleFire(1) && *h >= 560 && *h <= 620) {
		*h = 580;
		changed = true;
	}

	return changed;
}

void MinecraftClient::onBlockDestroyed(Player* player, int x, int y, int z, int face) {
	particleEngine->destroy(x, y, z);
	soundEngine->play(oldTile->soundType->getBreakSound(), x + 0.5f, y + 0.5f, z + 0.5f, (oldTile->soundType->getVolume() + 1) / 2, oldTile->soundType->getPitch() * 0.8f);

	if (player == this->player) {
		if (options.getBooleanValue(OPTIONS_DESTROY_VIBRATION)) minecraft.platform()->vibrate(24);

		RemoveBlockPacket packet(player, x, y, z);
		minecraft.raknetInstance->send(packet);
	}
}