#include "LocalPlayer.hpp"
#include <Minecraft.hpp>
#include "world/entity/EntityEvent.hpp"
#include "world/entity/player/Player.hpp"
#include "world/inventory/BaseContainerMenu.hpp"
#include "world/item/BowItem.hpp"
#include "world/level/Level.hpp"
#include "world/level/tile/Tile.hpp"
#include "world/level/tile/entity/TileEntity.hpp"
#include "world/level/material/Material.hpp"
#include "network/packet/ContainerClosePacket.hpp"
#include "network/packet/MovePlayerPacket.hpp"
#include "network/packet/PlayerEquipmentPacket.hpp"
#include "network/RakNetInstance.hpp"
#include "network/packet/DropItemPacket.hpp"
#include "network/packet/SetHealthPacket.hpp"
#include "network/packet/SendInventoryPacket.hpp"
#include "network/packet/EntityEventPacket.hpp"
#include "network/packet/PlayerActionPacket.hpp"
#include <vector>
#include <cctype>
#include "platform/log.hpp"
#include "platform/HttpClient.hpp"
#include "platform/CThread.hpp"
#include "util/StringUtils.hpp"
#include "client/Options.hpp"

#if defined(_WIN32)
#include <direct.h>
#include <sys/stat.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#endif


#include "client/gui/Screen.hpp"
#include "client/gui/screens/FurnaceScreen.hpp"
#include "client/gui/screens/ChestScreen.hpp"
#include "client/gui/screens/crafting/WorkbenchScreen.hpp"
#include "client/gui/screens/crafting/StonecutterScreen.hpp"
#include "client/gui/screens/InBedScreen.hpp"
#include "client/gui/screens/TextEditScreen.hpp"
#include "client/particle/TakeAnimationParticle.hpp"
#include "network/packet/AnimatePacket.hpp"
#include "world/item/ArmorItem.hpp"
#include "network/packet/PlayerArmorEquipmentPacket.hpp"
#include <MinecraftClient.hpp>

namespace {


static bool isBase64(unsigned char c) {
    return (std::isalnum(c) || (c == '+') || (c == '/'));
}

static std::string base64Decode(const std::string& encoded) {
    static const std::string base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    int in_len = (int)encoded.size();
    int i = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];

    while (in_len-- && (encoded[in_] != '=') && isBase64(encoded[in_])) {
        char_array_4[i++] = encoded[in_]; in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = (unsigned char)base64Chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; i < 3; i++)
                out += char_array_3[i];
            i = 0;
        }
    }

    if (i) {
        for (int j = i; j < 4; j++)
            char_array_4[j] = 0;
        for (int j = 0; j < 4; j++)
            char_array_4[j] = (unsigned char)base64Chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (int j = 0; (j < i - 1); j++)
            out += char_array_3[j];
    }
    return out;
}

static std::string extractJsonString(const std::string& json, const std::string& key) {
    std::string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return "";
    pos = json.find(':', pos + search.size());
    if (pos == std::string::npos) return "";
    pos++;
    while (pos < json.size() && std::isspace((unsigned char)json[pos])) pos++;
    if (pos >= json.size() || json[pos] != '"') return "";
    pos++;
    size_t end = json.find('"', pos);
    if (end == std::string::npos) return "";
    return json.substr(pos, end - pos);
}

static std::string getTextureUrlForUsername(const std::string& username, const std::string& textureKey) {
    if (username.empty()) {
        LOGI("[%s] username empty\n", textureKey.c_str());
        return "";
    }

    LOGI("[%s] resolving UUID for user '%s'...\n", textureKey.c_str(), username.c_str());
    std::vector<unsigned char> body;
    std::string apiUrl = "http://api.mojang.com/users/profiles/minecraft/" + username;
    if (!HttpClient::download(apiUrl, body)) {
        LOGW("[%s] failed to download UUID for %s\n", textureKey.c_str(), username.c_str());
        return "";
    }

    std::string response(body.begin(), body.end());
    std::string uuid = extractJsonString(response, "id");
    if (uuid.empty()) {
        LOGW("[%s] no UUID found in Mojang response for %s\n", textureKey.c_str(), username.c_str());
        return "";
    }

    LOGI("[%s] UUID=%s for user %s\n", textureKey.c_str(), uuid.c_str(), username.c_str());

    std::string profileUrl = "http://sessionserver.mojang.com/session/minecraft/profile/" + uuid;
    if (!HttpClient::download(profileUrl, body)) {
        LOGW("[%s] failed to download profile for UUID %s\n", textureKey.c_str(), uuid.c_str());
        return "";
    }

    response.assign(body.begin(), body.end());
    std::string encoded = extractJsonString(response, "value");
    if (encoded.empty()) {
        LOGW("[%s] no value field in profile response for UUID %s\n", textureKey.c_str(), uuid.c_str());
        return "";
    }

    std::string decoded = base64Decode(encoded);

    std::string searchKey = "\"" + textureKey + "\"";
    size_t texturePos = decoded.find(searchKey);
    if (texturePos == std::string::npos) {
        LOGW("[%s] no %s entry in decoded profile for UUID %s\n", textureKey.c_str(), textureKey.c_str(), uuid.c_str());
        return "";
    }
    size_t urlPos = decoded.find("\"url\"", texturePos);
    if (urlPos == std::string::npos) {
        LOGW("[%s] no url field under %s for UUID %s\n", textureKey.c_str(), textureKey.c_str(), uuid.c_str());
        return "";
    }

    // extract the URL value from the substring starting at urlPos
    std::string urlFragment = decoded.substr(urlPos);
    std::string textureUrl = extractJsonString(urlFragment, "url");
    if (textureUrl.empty()) {
        LOGW("[%s] failed to parse %s URL for UUID %s\n", textureKey.c_str(), textureKey.c_str(), uuid.c_str());
        return "";
    }

    LOGI("[%s] %s URL for %s: %s\n", textureKey.c_str(), textureKey.c_str(), username.c_str(), textureUrl.c_str());
    return textureUrl;
}

static std::string getSkinUrlForUsername(const std::string& username) {
    return getTextureUrlForUsername(username, "SKIN");
}

static std::string getCapeUrlForUsername(const std::string& username) {
    return getTextureUrlForUsername(username, "CAPE");
}

#endif

static bool ensureDirectoryExists(const std::string& path) {
#if defined(_WIN32)
    return _mkdir(path.c_str()) == 0 || errno == EEXIST;
#else
    struct stat st;
    if (stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
        return true;

    std::string subPath;
    size_t i = 0;
    while (i < path.length()) {
        i = path.find_first_of("/\\", i);
        if (i == std::string::npos) {
            subPath = path;
        } else {
            subPath = path.substr(0, i);
        }

        if (!subPath.empty()) {
            if (stat(subPath.c_str(), &st) != 0) {
                if (mkdir(subPath.c_str(), 0755) != 0 && errno != EEXIST)
                    return false;
            } else if (!S_ISDIR(st.st_mode)) {
                return false;
            }
        }

        if (i == std::string::npos)
            break;
        i += 1;
    }

    if (stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
        return true;
    return mkdir(path.c_str(), 0755) == 0 || errno == EEXIST;
#endif
}

static bool fileExists(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0)
        return false;

#if defined(_WIN32)
    return (st.st_mode & _S_IFREG) != 0;
#else
    return S_ISREG(st.st_mode);
#endif
}



static void* fetchSkinForPlayer(void* param) {
    LocalPlayer* player = (LocalPlayer*)param;
    if (!player) return NULL;

    LOGI("[Skin] starting skin download for %s\n", player->name.c_str());

    const std::string cacheDir = "data/images/skins";
    if (!ensureDirectoryExists(cacheDir)) {
        LOGW("[Skin] failed to create cache directory %s\n", cacheDir.c_str());
    }

    std::string cacheFile = cacheDir + "/" + player->name + ".png";
    if (fileExists(cacheFile)) {
        LOGI("[Skin] using cached skin for %s\n", player->name.c_str());
        player->setTextureName("skins/" + player->name + ".png");
        return NULL;
    }

    std::string skinUrl = getSkinUrlForUsername(player->name);
    if (skinUrl.empty()) {
        LOGW("[Skin] skin URL lookup failed for %s\n", player->name.c_str());
        player->setTextureName("mob/char.png");
        return NULL;
    }

    LOGI("[Skin] downloading skin from %s\n", skinUrl.c_str());
    std::vector<unsigned char> skinData;
    if (!HttpClient::download(skinUrl, skinData) || skinData.empty()) {
        LOGW("[Skin] download failed for %s\n", skinUrl.c_str());
        player->setTextureName("mob/char.png");
        return NULL;
    }

    // Save to cache
    FILE* fp = fopen(cacheFile.c_str(), "wb");
    if (fp) {
        fwrite(skinData.data(), 1, skinData.size(), fp);
        fclose(fp);
        LOGI("[Skin] cached skin to %s\n", cacheFile.c_str());

		player->setTextureName("skins/" + player->name + ".png");
	} else {
        LOGW("[Skin] failed to write skin cache %s\n", cacheFile.c_str());
        player->setTextureName("mob/char.png");
    }

    return NULL;
}

static void* fetchCapeForPlayer(void* param) {
    LocalPlayer* player = (LocalPlayer*)param;
    if (!player) return NULL;

    LOGI("[Cape] starting cape download for %s\n", player->name.c_str());

    const std::string cacheDir = "data/images/capes";
    if (!ensureDirectoryExists(cacheDir)) {
        LOGW("[Cape] failed to create cache directory %s\n", cacheDir.c_str());
    }

    std::string cacheFile = cacheDir + "/" + player->name + ".png";
    if (fileExists(cacheFile)) {
        LOGI("[Cape] using cached cape for %s\n", player->name.c_str());
        player->setCapeTextureName("capes/" + player->name + ".png");
        return NULL;
    }

    std::string capeUrl = getCapeUrlForUsername(player->name);
    if (capeUrl.empty()) {
        LOGW("[Cape] cape URL lookup failed for %s\n", player->name.c_str());
        return NULL;
    }

    LOGI("[Cape] downloading cape from %s\n", capeUrl.c_str());
    std::vector<unsigned char> capeData;
    if (!HttpClient::download(capeUrl, capeData) || capeData.empty()) {
        LOGW("[Cape] download failed for %s\n", capeUrl.c_str());
        return NULL;
    }

    // Save to cache
    FILE* fp = fopen(cacheFile.c_str(), "wb");
    if (fp) {
        fwrite(capeData.data(), 1, capeData.size(), fp);
        fclose(fp);
        LOGI("[Cape] cached cape to %s\n", cacheFile.c_str());

		player->setCapeTextureName("capes/" + player->name + ".png");
    } else {
        LOGW("[Cape] failed to write cape cache %s\n", cacheFile.c_str());
    }

    return NULL;
}

#endif

//@note: doesn't work completely, since it doesn't care about stairs rotation
static bool isJumpable(int tileId) {
	return tileId != Tile::fence->id
		&& tileId != Tile::fenceGate->id
		&& tileId != Tile::stoneSlabHalf->id
		&& tileId != Tile::trapdoor->id
        && tileId != Tile::sign->id
        && tileId != Tile::wallSign->id
		&& (Tile::tiles[tileId] != NULL && Tile::tiles[tileId]->getRenderShape() != Tile::SHAPE_STAIRS);
}

} // anonymous namespace

LocalPlayer::LocalPlayer(MinecraftClient& minecraft, Level* level, const std::string& username, int dimension, bool isCreative)
:	Player(level, isCreative), minecraft(minecraft) {
	this->dimension = dimension;
	_init();

	if (minecraft.options().getStringValue(OPTIONS_USERNAME).size() != 0) {
		textureName = "mob/char.png";

		this->name = minecraft.options().getStringValue(OPTIONS_USERNAME);
		printf("test \n");
		// Fetch user skin and cape from Mojang servers in the background (avoids blocking the main thread)
		// TODO: Fix this memory leak
		new CThread(fetchSkinForPlayer, this);
		new CThread(fetchCapeForPlayer, this);
	}
}

LocalPlayer::~LocalPlayer() {
	//delete input;
	//input = NULL;
}

/*private*/
void LocalPlayer::calculateFlight(float xa, float ya, float za) {
	float flySpeed = minecraft.options().getProgressValue(OPTIONS_FLY_SPEED);
	float sensivity = minecraft.options().getProgressValue(OPTIONS_SENSITIVITY);

    xa = xa * flySpeed;
    ya = 0;
    za = za * flySpeed;

    if (sprinting) {
        float sprintBoost = getWalkingSpeedModifier(); // 1.3x
        xa *= sprintBoost;
        za *= sprintBoost;
    }

#ifdef ANDROID
    if (Keyboard::isKeyDown(103)) ya = .2f * flySpeed;
    if (Keyboard::isKeyDown(102)) ya = -.2f * flySpeed;
#else
    if (Keyboard::isKeyDown(Keyboard::KEY_E)) ya = .2f * flySpeed;
    if (Keyboard::isKeyDown(Keyboard::KEY_Q)) ya = -.2f * flySpeed;
#endif

    flyX = 10 * smoothFlyX.getNewDeltaValue(xa, .35f * sensivity);
    flyY = 10 * smoothFlyY.getNewDeltaValue(ya, .35f * sensivity);
    flyZ = 10 * smoothFlyZ.getNewDeltaValue(za, .35f * sensivity);
}

bool LocalPlayer::isSolidTile(int x, int y, int z) {
	int tileId = level->getTile(x, y, z);
	return tileId > 0 && Tile::tiles[tileId]->material->isSolid();
}

void LocalPlayer::tick() {

	super::tick();
	if(!useItem.isNull()) {
		ItemInstance* item = inventory->getSelected();
		if(item != NULL && *item == useItem) {
			if (useItemDuration <= 25 && useItemDuration % 4 == 0) {
				spawnEatParticles(item, 5);
			}
			if(--useItemDuration == 0) {
				if(!level->isClientSide) {
					completeUsingItem();
				} else {
					EntityEventPacket p(entityId, EntityEvent::USE_ITEM_COMPLETE);
					level->raknetInstance->send(p);
				}
			}
		}
		else {
			stopUsingItem();
		}
	}
	if (minecraft.isOnline())
	{
		if (std::abs(x - sentX) > .1f || std::abs(y - sentY) > .01f || std::abs(z - sentZ) > .1f || std::abs(sentRotX - xRot) > 1 || std::abs(sentRotY - yRot) > 1)
		{
			MovePlayerPacket packet(entityId, x, y - heightOffset, z, xRot, yRot);
			minecraft.raknetInstance->send(packet);
			sentX = x;
			sentY = y;
			sentZ = z;
			sentRotX = xRot;
			sentRotY = yRot;
		}

		ItemInstance* item = inventory->getSelected();
		int newItemId   = (item && item->count > 0) ? item->id : 0;
		int newItemData = (item && item->count > 0) ? item->getAuxValue() : 0;

		if (sentInventoryItemId != newItemId
		||  sentInventoryItemData != newItemData)
		{
			sentInventoryItemId   = newItemId;
			sentInventoryItemData = newItemData;
			PlayerEquipmentPacket packet(entityId, newItemId, newItemData);
			minecraft.raknetInstance->send(packet);
		}
	}
/*
    for (int i = 0; i < 4; ++i) {
        ItemInstance* a = getArmor(i);
        if (!a) continue;

        ArmorItem* item = (ArmorItem*) a->getItem();

        printf("armor %d: %d\n", i, a->getAuxValue());
    }
*/

	updateArmorTypeHash();
	if (!minecraft.getScreen() && containerMenu) {
		static bool hasPostedError = false;
		if (!hasPostedError) {
			// @todo
			// minecraft.gui().postError( ErrorCodes::ContainerRefStillExistsAfterDestruction );
			hasPostedError = true;
		}
	}
	//LOGI("biome: %s\n", level->getBiomeSource()->getBiome((int)x >> 4, (int)z >> 4)->name.c_str());
}

/*public*/
void LocalPlayer::aiStep() {
	jumpTriggerTime--;
	ascendTriggerTime--;
	descendTriggerTime--;

	bool wasJumping = input->jumping;
    bool screenCovering = minecraft.getScreen() && !minecraft.getScreen()->passEvents;
	if (!screenCovering)
		input->tick(this);

	// Sprint: detect W double-tap
	{
		bool forwardHeld = (input->ya > 0);
		if (forwardHeld && !prevForwardHeld && minecraft.options().getBooleanValue(OPTIONS_ALLOW_SPRINT)) {
			// leading edge of W press
			if (sprintDoubleTapTimer > 0)
				sprinting = true;
			else
				sprintDoubleTapTimer = 7;
		}
		if (!forwardHeld) {
			sprinting = false;
		}
		if (sprintDoubleTapTimer > 0) sprintDoubleTapTimer--;
		prevForwardHeld = forwardHeld;
	}
	if (input->sneaking)
		sprinting = false;

    if (input->sneaking) {
        if (ySlideOffset < 0.2f) ySlideOffset = 0.2f;
    }

	if (abilities.mayfly) {
		// Check for flight toggle
		if (!wasJumping && input->jumping) {
			if (jumpTriggerTime <= 0) jumpTriggerTime = 7;
			else {
				abilities.flying = !abilities.flying;
				jumpTriggerTime = 0;
			}
		}
		if (abilities.flying) {
			if (input->wantUp) {
				yd += 0.15f;
				//xd = zd = 0;
			}
			if (input->wantDown) {
				yd -= 0.15f;
			}
		}
	}

	if(isUsingItem()) {
		const float k = 0.35f;
		input->xa *= k;
		input->ya *= k;
	}

	Mob::aiStep();
    super::aiStep();

	//if (onGround && abilities.flying)
	//	abilities.flying = false;

	if (interpolateOnly())
		updateAi();
}

/*public*/
void LocalPlayer::closeContainer() {
	if (level->isClientSide) {
		ContainerClosePacket packet(containerMenu->containerId);
		minecraft.raknetInstance->send(packet);
	}
	super::closeContainer();
    minecraft.setScreen(NULL);
}

//@Override
void LocalPlayer::move(float xa, float ya, float za) {
    //@note: why is this == minecraft.player needed?
    if (this == minecraft.getPlayer()  && minecraft.options().getBooleanValue(OPTIONS_IS_FLYING)) {
        noPhysics = true;
        float tmp = walkDist; // update
        calculateFlight((float) xa, (float) ya, (float) za);
        fallDistance = 0;
        yd = 0;
        super::move(flyX, flyY, flyZ);
        onGround = true;
        walkDist = tmp;
    } else {
		if (autoJumpTime > 0) {
			autoJumpTime--;
			input->jumping = true;
		}
		float prevX = x, prevZ = z;

        super::move(xa, ya, za);

		float newX = x, newZ = z;

		if (autoJumpTime <= 0 && minecraft.options().getBooleanValue(OPTIONS_AUTOJUMP))
		{
			// auto-jump when crossing the middle of a tile, and the tile in the front is blocked
			bool jump = false;
			if (Mth::floor(prevX * 2.0f) != Mth::floor(newX * 2.0f) || Mth::floor(prevZ * 2.0f) != Mth::floor(newZ * 2.0f))
			{
				float dist = Mth::sqrt(xa * xa + za * za);
				const int xx = Mth::floor(x + xa / dist);
				const int zz = Mth::floor(z + za / dist);
				const int tileId = level->getTile(xx, (int)(y-1), zz);
				jump = (isSolidTile(xx, (int)(y-1), zz) // Solid block to jump up on
					&& !isSolidTile(xx, (int)y, zz) && !isSolidTile(xx, (int)(y+1), zz)) // Enough space
					&& isJumpable(tileId);
			}
			if (jump)
			{
				autoJumpTime = 1;
			}
		}
    }
}

void LocalPlayer::updateAi() {
    super::updateAi();
    this->xxa = input->xa;
    this->yya = input->ya;
    this->jumping = input->jumping || autoJumpTime > 0;
}

void LocalPlayer::take( Entity* e, int orgCount ) {
	if (e->isItemEntity())
		minecraft.getParticleEngine()->add(new TakeAnimationParticle(minecraft.level, (ItemEntity*)e, this, -0.5f));
}

void LocalPlayer::setKey( int eventKey, bool eventKeyState )
{
	input->setKey(eventKey, eventKeyState);
}

void LocalPlayer::releaseAllKeys()
{
	if (input) input->releaseAllKeys();
}

float LocalPlayer::getWalkingSpeedModifier() {
	return sprinting ? 1.3f : 1.0f;
}

float LocalPlayer::getFieldOfViewModifier() {
	float targetFov = 1.0f;
	if(abilities.flying) targetFov *= 1.1f;
	targetFov *= ((walkingSpeed * getWalkingSpeedModifier()) / DEFAULT_WALK_SPEED +1) / 2;

	if(isUsingItem() && getUseItem()->id == Item::bow->id) {
		float ticksHeld = (float)getTicksUsingItem();
		float scale = ticksHeld / BowItem::MAX_DRAW_DURATION;
		if(scale > 1) {
			scale = 1;
		}
		else {
			scale *= scale;
		}
		targetFov *= 1.0f - scale * 0.15f;
	}
	return targetFov;
}
void LocalPlayer::addAdditonalSaveData( CompoundTag* entityTag )
{
	super::addAdditonalSaveData(entityTag);
	entityTag->putInt("Score", score);
}

void LocalPlayer::readAdditionalSaveData( CompoundTag* entityTag )
{
	super::readAdditionalSaveData(entityTag);
	score = entityTag->getInt("Score");
}

bool LocalPlayer::isSneaking()
{
	return input->sneaking;
}

void LocalPlayer::hurtTo( int newHealth )
{
	int dmg = health - newHealth;
	if (dmg <= 0) {
		this->health = newHealth;
	} else {
		lastHurt = dmg;
		lastHealth = health;
		invulnerableTime = invulnerableDuration;

		minecraft.getPlayer()->bypassArmor = true;
		actuallyHurt(dmg);
		minecraft.getPlayer()->bypassArmor = false;

		hurtTime = hurtDuration = 10;
	}
}

void LocalPlayer::actuallyHurt( int dmg ) {
    if (minecraft.getScreen() && minecraft.getScreen()->closeOnPlayerHurt()) {
		if (containerMenu) closeContainer();
        else minecraft.setScreen(NULL);
	}

    super::actuallyHurt(dmg);
}

void LocalPlayer::respawn() {
	minecraft.respawnPlayer();
}

void LocalPlayer::die(Entity* source)
{
	// If we're an online client, send the inventory to be dropped
	// If we're the server, drop the inventory immediately
	if (level->isClientSide) {
		SendInventoryPacket packet(this, true);
		minecraft.raknetInstance->send(packet);
	}
	inventory->dropAll(level->isClientSide);
	for (int i = 0; i < NUM_ARMOR; ++i) {
		ItemInstance* item = getArmor(i);
		if (!ItemInstance::isArmorItem(item)) return;

		drop(new ItemInstance(*item), true);
		setArmor(i, NULL);
	}

	super::die(source);
}

void LocalPlayer::swing() {
    super::swing();

    if (swingTime == -1) {
        AnimatePacket packet(AnimatePacket::Swing, this);
        packet.reliability = UNRELIABLE;
        packet.priority = MEDIUM_PRIORITY;
        minecraft.raknetInstance->send(packet);
    }
}

void LocalPlayer::reset() {
	super::reset();
	this->_init();
}

void LocalPlayer::_init() {
	autoJumpTime		= 0;
	jumpTriggerTime		= 0;
	ascendTriggerTime	= 0;
	descendTriggerTime	= 0;
	ascending	= false;
	descending	= false;

	ItemInstance* item = inventory->getSelected();
	sentInventoryItemId = item? item->id : 0;
	sentInventoryItemData = item? item->getAuxValue() : 0;
}

void LocalPlayer::startCrafting(int x, int y, int z, int tableSize) {
	if (!minecraft.isCreativeMode())
		minecraft.setScreen( new WorkbenchScreen(tableSize) );
}

void LocalPlayer::startStonecutting(int x, int y, int z) {
	if (!minecraft.isCreativeMode())
		minecraft.setScreen( new StonecutterScreen() );
}

void LocalPlayer::openFurnace( FurnaceTileEntity* e ) {
	if (!minecraft.isCreativeMode())
		minecraft.setScreen( new FurnaceScreen(this, e) );
}

void LocalPlayer::openContainer( ChestTileEntity* container ) {
	if (!minecraft.isCreativeMode())
		minecraft.setScreen( new ChestScreen(this, container) );
}

void LocalPlayer::drop( ItemInstance* item, bool randomly )
{
	if (!item)
		return;

	if (level->isClientSide) {
		DropItemPacket packet(entityId, *item);
		minecraft.raknetInstance->send(packet);
		// delete the ItemEntity here, since we don't add it to level
		delete item;
	} else {
		super::drop(item, randomly);
	}
}

void LocalPlayer::causeFallDamage( float distance )
{
	int dmg = (int) ceil((distance - 3));
	if (dmg > 0) {
		if (level->isClientSide) {
			SetHealthPacket packet(SetHealthPacket::HEALTH_MODIFY_OFFSET + dmg);
			minecraft.raknetInstance->send(packet);
		}
	}
	super::causeFallDamage(distance);

}

void LocalPlayer::displayClientMessage( const std::string& messageId ) {
	minecraft.gui().displayClientMessage(messageId);
}

int LocalPlayer::startSleepInBed( int x, int y, int z ) {
	int startSleepInBedReturnValue = super::startSleepInBed(x, y, z);

	if(startSleepInBedReturnValue == BedSleepingResult::OK)
		minecraft.setScreen(new InBedScreen());

	return startSleepInBedReturnValue;
}

void LocalPlayer::stopSleepInBed( bool forcefulWakeUp, bool updateLevelList, bool saveRespawnPoint ) {
	if(level->isClientSide) {
		PlayerActionPacket packet(PlayerActionPacket::STOP_SLEEPING, 0, 0, 0, 0, entityId);
		minecraft.raknetInstance->send(packet);
	}

	minecraft.setScreen(NULL);

	super::stopSleepInBed(forcefulWakeUp, updateLevelList, saveRespawnPoint);
}

void LocalPlayer::openTextEdit( TileEntity* tileEntity ) {
#if !defined(RPI)
	if(tileEntity->type == TileEntityType::Sign)
		minecraft.setScreen(new TextEditScreen((SignTileEntity*) tileEntity));
#endif
}

void LocalPlayer::updateArmorTypeHash() {
    int hash = getArmorTypeHash();
    if (hash != armorTypeHash) {
        PlayerArmorEquipmentPacket p(this);
        minecraft.raknetInstance->send(p);
        armorTypeHash = hash;
    }
}
