#include "ClientSideNetworkHandler.hpp"
#include <MinecraftClient.hpp>
#include "client/Options.hpp"
#include "gamemode/GameMode.hpp"
#include "packet/PacketInclude.hpp"
#include "RakNetInstance.hpp"
#include "world/level/chunk/ChunkSource.hpp"
#include "world/level/Level.hpp"
#include "world/level/storage/LevelStorageSource.hpp"
#include "world/entity/player/Player.hpp"
#include "world/entity/player/Inventory.hpp"
#include <Minecraft.hpp>
#include "client/player/LocalPlayer.hpp"
#include "client/multiplayer/MultiPlayerLevel.hpp"
#include "client/player/input/KeyboardInput.hpp"
#include "client/sound/SoundEngine.hpp"
#include "world/entity/MobFactory.hpp"
#include "raknet/RakPeerInterface.h"
#include "world/level/Explosion.hpp"
#include "world/level/tile/entity/FurnaceTileEntity.hpp"
#include "world/inventory/BaseContainerMenu.hpp"
#include "world/entity/EntityFactory.hpp"
#include "world/entity/item/PrimedTnt.hpp"
#include "world/entity/projectile/Arrow.hpp"
#include "world/level/tile/entity/ChestTileEntity.hpp"
#include "client/player/RemotePlayer.hpp"
#include "world/level/tile/LevelEvent.hpp"
#include "world/entity/item/FallingTile.hpp"

static MultiPlayerLevel* mpcast(Level* l) { return (MultiPlayerLevel*) l; }

ClientSideNetworkHandler::ClientSideNetworkHandler(MinecraftClient& minecraft, IRakNetInstance* raknetInstance)
:	minecraft(minecraft),
	raknetInstance(raknetInstance)
{
	rakPeer = raknetInstance->getPeer();
}

ClientSideNetworkHandler::~ClientSideNetworkHandler()
{
}

void ClientSideNetworkHandler::requestNextChunk()
{
	if (requestNextChunkIndex < NumRequestChunks)
	{
        IntPair& chunk = requestNextChunkIndexList[requestNextChunkIndex];
		RequestChunkPacket packet(chunk.x, chunk.y);
        raknetInstance->send(packet);

        //LOGI("requesting chunks @ (%d, %d)\n", chunk.x, chunk.y);
        
		//raknetInstance->send(new RequestChunkPacket(requestNextChunkPosition % CHUNK_CACHE_WIDTH, requestNextChunkPosition / CHUNK_CACHE_WIDTH));
		requestNextChunkIndex++;
        requestNextChunkPosition++;
	}
}

bool ClientSideNetworkHandler::areAllChunksLoaded()
{
	return (requestNextChunkPosition >= (CHUNK_CACHE_WIDTH * CHUNK_CACHE_WIDTH));
}

bool ClientSideNetworkHandler::isChunkLoaded(int x, int z)
{
	if (x < 0 || x >= CHUNK_CACHE_WIDTH || z < 0 || z >= CHUNK_CACHE_WIDTH) {
		LOGE("Error: Tried to request chunk (%d, %d)\n", x, z);
		return true;
	}
	return chunksLoaded[x * CHUNK_CACHE_WIDTH + z];
	//return areAllChunksLoaded();
}

void ClientSideNetworkHandler::onConnect(const RakNet::RakNetGUID& hostGuid)
{
	LOGI("onConnect, server guid: %s, local guid: %s\n", hostGuid.ToString(), rakPeer->GetMyGUID().ToString());
	serverGuid = hostGuid;

	clearChunksLoaded();
	LoginPacket packet(minecraft.options().getStringValue(OPTIONS_USERNAME).c_str(), SharedConstants::NetworkProtocolVersion);
	raknetInstance->send(packet);
}

void ClientSideNetworkHandler::onUnableToConnect()
{
	LOGI("onUnableToConnect\n");
}

void ClientSideNetworkHandler::onDisconnect(const RakNet::RakNetGUID& guid)
{
	LOGI("onDisconnect\n");
	if (level)
	{
		level->isClientSide = false;
		for (int i = (int)level->players.size()-1; i >= 0; --i ) {
			Player* p = level->players[i];
			if (p != minecraft.getPlayer()) {
				p->reallyRemoveIfPlayer = true;
				level->removeEntity(p);
			}
		}
	}
#ifndef STANDALONE_SERVER
	minecraft.gui.addMessage("Disconnected from server");
#endif
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, LoginStatusPacket* packet) {
	if (packet->status == LoginStatus::Success) {
		raknetInstance->setIsLoggedIn(true);
		return;
	}

	if (packet->status == LoginStatus::Failed_ClientOld) {
		LOGI("Disconnect! Client is outdated!\n");
#ifndef STANDALONE_SERVER
		minecraft.setScreen(new DisconnectionScreen("Could not connect: Outdated client!"));
#endif
	}
	if (packet->status == LoginStatus::Failed_ServerOld) {
		LOGI("Disconnect! Server is outdated!\n");
#ifndef STANDALONE_SERVER
		minecraft.setScreen(new DisconnectionScreen("Could not connect: Outdated server!"));
#endif
	}
}


void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, StartGamePacket* packet)
{
	LOGI("StartGamePacket\n");

#ifdef RPI
	if (packet->gameType != GameType::Creative) {
		minecraft.setScreen(new DisconnectionScreen("Could not connect: Incompatible server!"));
		return;
	}
#endif

	const std::string& levelId = LevelStorageSource::TempLevelId;
	LevelStorageSource* storageSource = minecraft.getLevelSource();
	storageSource->deleteLevel(levelId);
	//level = new Level(storageSource->selectLevel(levelId, true), "temp", packet->levelSeed, SharedConstants::StorageVersion);
	MultiPlayerLevel* level = new MultiPlayerLevel(
		storageSource->selectLevel(levelId, true),
		"temp",
		LevelSettings(packet->levelSeed, LevelSettings::validateGameType(packet->gameType)),
		SharedConstants::StorageVersion);
	level->isClientSide = true;

	bool isCreative = (packet->gameType == GameType::Creative);
	LocalPlayer* player = new LocalPlayer(minecraft, level, minecraft.options().getStringValue(OPTIONS_USERNAME), level->dimension->id, isCreative);
	player->owner = rakPeer->GetMyGUID();
	player->entityId = packet->entityId;
	player->moveTo(packet->x, packet->y, packet->z, player->yRot, player->xRot);

	LOGI("new pos: %f, %f [%f - %f]\n", player->x, player->z, player->bb.y0, player->bb.y1);

	minecraft.setLevel(level, "ClientSideNetworkHandler -> setLevel", player);
	minecraft.setIsCreativeMode(isCreative);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, MessagePacket* packet)
{
	LOGI("MessagePacket\n");
#ifndef STANDALONE_SERVER
	minecraft.gui.addMessage(packet->message.C_String());
#endif
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, SetTimePacket* packet)
{
	if (!level)
		return;

	LOGI("SetTimePacket\n");
	level->setTime(packet->time);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, AddEntityPacket* packet)
{
	if (!level)
		return;

	Entity* e = EntityFactory::CreateEntity(packet->type, level);
	if (!e)
		return;

	e->entityId = packet->entityId;
	e->setPos(packet->x, packet->y, packet->z);

	// Entity Specific stuff here
	switch (packet->type)
	{
		case EntityTypes::IdFallingTile: {
			int data = -packet->data();
            FallingTile* ft = (FallingTile*) e;
            ft->tile = data & 0xff;
            ft->data = data >> 16;
		}
		case EntityTypes::IdArrow: {
			Entity* owner = level->getEntity(packet->data());
			if (owner && owner->isMob())
				((Arrow*)e)->ownerId = owner->entityId;
			break;
		}
		default:
			break;
	}

    if (packet->hasMovementData()) {
		/*
		e->xd = packet->xd;
		e->yd = packet->yd;
		e->zd = packet->zd;
		*/
		e->lerpMotion(packet->xd, packet->yd, packet->zd);
		//LOGI("Client: reading entity with %f, %f, %f\n", e->xd, e->yd, e->zd);
	}

	mpcast(level)->putEntity(packet->entityId, e);
}
void ClientSideNetworkHandler::handle( const RakNet::RakNetGUID& source, AddPaintingPacket* packet ) {
	if (!level)
		return;

	Painting* painting = new Painting(level, packet->xTile, packet->yTile, packet->zTile, packet->dir, packet->motive);
	mpcast(level)->putEntity(packet->entityId, painting);
}
void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, AddMobPacket* packet)
{
	LOGI("AddMobPacket (%p)\n", level);

	if (!level) {
		LOGW("Trying to add a mob with no level!\n");
		//we skip this since we will get this player anyway when we request level
		return;
	}
	if (!packet->type) {
		LOGE("Trying to add a mob without a type id\n");
		return;
	}

	Mob* mob = MobFactory::CreateMob(packet->type, level);
	if (!mob) {
		LOGE("Server tried to add an unknown mob type! :%d\n", packet->type);
		return;
	}

	mob->entityId = packet->entityId;
	mob->moveTo(packet->x, packet->y, packet->z, packet->yRot, packet->xRot);
	mob->getEntityData()->assignValues(&packet->unpack);
	level->addEntity(mob);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, AddPlayerPacket* packet)
{
	if (!level) {
		//we skip this since we will get this player anyway when we request level
		return;
	}
	LOGI("AddPlayerPacket\n");

	Player* player = new RemotePlayer(level, minecraft.isCreativeMode());
	minecraft.gameMode->initAbilities(player->abilities);
	player->entityId = packet->entityId;
	level->addEntity(player);

	player->moveTo(packet->x, packet->y, packet->z, packet->yRot, packet->xRot);
	player->name = packet->name.C_String();
	player->owner = packet->owner;
	player->getEntityData()->assignValues(&packet->unpack);
    int slot = Inventory::MAX_SELECTION_SIZE;
    if (packet->carriedItemId == 0) {
        player->inventory->clearSlot(slot);
    } else {
        ItemInstance newItem(packet->carriedItemId, 1, packet->carriedItemAuxValue);
        player->inventory->replaceSlot(slot, &newItem);
    }
    player->inventory->moveToSelectedSlot(slot, true);
	//player->resetPos();

	std::string message = packet->name.C_String();
	message += " joined the game";
#ifndef STANDALONE_SERVER
	minecraft.gui.addMessage(message);
#endif
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, RemovePlayerPacket* packet) {
	if (!level || source == minecraft.getPlayer()->owner) return;

	if (Player* player = findPlayer(level, packet->entityId, &packet->owner)) {
		player->reallyRemoveIfPlayer = true;
		level->removeEntity(player);
	}

	//for (int i = (int)level->players.size()-1; i >= 0; --i) {
	//	if (level->players[i]->owner == source) {
	//		level->players[i]->reallyRemoveIfPlayer = true;
	//		level->players.erase(level->players.begin() + i);
	//		break;
	//	}
	//}

	//if (!player) return;

	//std::string message = packet->name.C_String();
	//message += " joined the game";
	//minecraft.gui.addMessage(message);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, RemoveEntityPacket* packet)
{
	if (!level) return;

	Entity* entity = level->getEntity(packet->entityId);
	LOGI("RemoveEntityPacket %p %p, %d\n", entity, minecraft.getPlayer(), entity?(int)(entity->isPlayer()): -1);
	if (!entity) return;

	level->removeEntity(entity);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, AddItemEntityPacket* packet)
{
	if (!level) return;

	ItemEntity* entity = new ItemEntity(level, packet->x, packet->y, packet->z, ItemInstance(packet->itemId, packet->itemCount, packet->auxValue));
	entity->xd = packet->xa();
	entity->yd = packet->ya();
	entity->zd = packet->za();
	//LOGI("item-entity @ client: %f, %f, %f\n", entity->xd, entity->yd, entity->zd);

	mpcast(level)->putEntity(packet->id, entity);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, TakeItemEntityPacket* packet) {
	if (!level) return;

	Entity* e = level->getEntity(packet->itemId);
	if (!e)
		return;
	
	ItemInstance item;
	if (e->isItemEntity()) {
		item = ((ItemEntity*) e)->item;
#ifndef STANDALONE_SERVER
		if (Entity* to = level->getEntity(packet->playerId))
			minecraft.particleEngine->add(new TakeAnimationParticle(level, (ItemEntity*)e, to, -0.5f));
#endif
	}
	else if (e->getEntityTypeId() == EntityTypes::IdArrow)
		item = ItemInstance(Item::arrow);

	if (item.isNull())
		return;

	// try take it and if we don't have space; re-throw it
	if (minecraft.getPlayer()->entityId == packet->playerId
	&& !minecraft.getPlayer()->inventory->add(&item)) {
		DropItemPacket dropPacket(packet->playerId, item);
		minecraft.raknetInstance->send(dropPacket);
	}
	level->playSound(e, "random.pop", 0.2f, 1.0f * 2.f);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, MovePlayerPacket* packet)
{
	if (!level)
		return;

	//printf("MovePlayerPacket\n");
	Entity* entity = level->getEntity(packet->entityId);
	if (entity)
	{
		entity->lerpTo(packet->x, packet->y, packet->z, packet->yRot, packet->xRot, 3);
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, MoveEntityPacket* packet)
{
	if (!level)
		return;

	//printf("MovePlayerPacket\n");
	Entity* entity = level->getEntity(packet->entityId);
	if (entity)
	{
		float xRot = packet->hasRot? packet->xRot : entity->xRot;
		float yRot = packet->hasRot? packet->yRot : entity->yRot;
		entity->lerpTo(packet->x, packet->y, packet->z, yRot, xRot, 3);
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, UpdateBlockPacket* packet)
{
	if (!level) return;

	//LOGI("UpdateBlockPacket @ %d, %d, %d\n", packet->x, packet->y, packet->z);

	int x = packet->x, z = packet->z;

	if (isChunkLoaded(x >> 4, z >> 4))
	{
		//LOGI("chunk is loaded - UPDATE @ %d, %d, %d -- %d, %d\n", x, packet->y, z, packet->blockId, packet->blockData);

		int y = packet->y;
		int tileId = Tile::transformToValidBlockId(packet->blockId, x, y, z);
		level->setTileAndData(x, y, z, tileId, packet->blockData);
	} else {
		SBufferedBlockUpdate update;
		update.blockId = packet->blockId;
		update.blockData = packet->blockData;
		update.setData = true;
		update.x = packet->x;
		update.y = packet->y;
		update.z = packet->z;
		bufferedBlockUpdates.push_back(update);
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, ExplodePacket* packet) {
	if (!level) return;
	Explosion explosion(level, NULL, packet->x, packet->y, packet->z, packet->r);
	explosion.toBlow.insert(packet->toBlow.begin(), packet->toBlow.end());
	explosion.finalizeExplosion();
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, LevelEventPacket* packet) {
	if (!level) return;
	if(packet->eventId == LevelEvent::ALL_PLAYERS_SLEEPING) {
		minecraft.getPlayer()->setAllPlayersSleeping();
	}
	else {
		minecraft.level->levelEvent(NULL, packet->eventId, packet->x, packet->y, packet->z, packet->data);
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, TileEventPacket* packet) {
	if (!level) return;
	minecraft.level->tileEvent(packet->x, packet->y, packet->z, packet->b0, packet->b1);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, EntityEventPacket* packet) {
	if (!level) return;

	Entity* e = level->getEntity(packet->entityId);
	if (e) e->handleEntityEvent(packet->eventId);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, ChunkDataPacket* packet)
{
	if (!level) {
		LOGI("level @ handle ChunkDataPacket is 0\n");
		return;
	}
	//LOGI("ChunkDataPacket\n");

	LevelChunk* chunk = level->getChunkSource()->create(packet->x, packet->z);
	if (!chunk || chunk->isEmpty())
	{
		LOGI("Failed to find write-able chunk\n");
		return;
	}

	//unsigned char* blockIds = chunk->getBlockData();
	DataLayer& blockData = chunk->data;

	const int setSize = LEVEL_HEIGHT / 8;
	const int setShift = 4; // power of LEVEL_HEIGHT / 8

	bool recalcHeight = false;

	int x0 = 16, x1 = 0, z0 = 16, z1 = 0, y0 = LEVEL_HEIGHT, y1 = 0;
	int rx = packet->x << 4;
	int rz = packet->z << 4;

	unsigned char readBlockBuffer[setSize];
	unsigned char readDataBuffer[setSize / 2];

	for (int i = 0; i < CHUNK_COLUMNS; i++)
	{
		unsigned char updateBits = 0;
		packet->chunkData.Read(updateBits);

		if (updateBits > 0)
		{
			recalcHeight = true;

			int colX = (i % CHUNK_WIDTH);
			int colZ = (i / CHUNK_WIDTH);
			int colDataPosition = colX << 11 | colZ << 7;

			for (int set = 0; set < 8; set++)
			{
				if ((updateBits & (1 << set)) != 0) 
				{
					packet->chunkData.Read((char*)readBlockBuffer, setSize);
					packet->chunkData.Read((char*)readDataBuffer, setSize / 2);

					for (int part = 0; part < setSize; part++)
					{
						//if (readBlockBuffer[part] == ((Tile*)Tile::grass)->id)
						//	readBlockBuffer[part] = 255;
						int x = rx + colX;
						int y = (set << setShift) + part;
						int z = rz + colZ;

						int tileId = Tile::transformToValidBlockId(readBlockBuffer[part], x, y, z);
						level->setTileNoUpdate(x, y, z, tileId);
					}
					// ((part & 1) == 0) ? readDataBuffer[part >> 1] & 0xf : (readDataBuffer[part >> 1] & 0xf0) >> 4

					//packet->chunkData.Read((char*)(&blockIds[colDataPosition + (set << setShift)]), setSize);
					// block data is only 4 bits per block
					//packet->chunkData.Read((char*)(&blockData.data[(colDataPosition + (set << setShift)) >> 1]), setSize >> 1);

					memcpy(&blockData.data[(colDataPosition + (set << setShift)) >> 1], readDataBuffer, setSize >> 1);
				}

				if (((1 << set) << setShift) < y0)
				{
					y0 = ((1 << set) << setShift);
				}
				if (((1 << set) << setShift) + (LEVEL_HEIGHT / 8) - 1 > y1)
				{
					y1 = ((1 << set) << setShift) + (LEVEL_HEIGHT / 8) - 1;
				}
			}
			if ((i % CHUNK_WIDTH) < x0)
			{
				x0 = (i % CHUNK_WIDTH);
			}
			if ((i % CHUNK_WIDTH) > x1)
			{
				x1 = (i % CHUNK_WIDTH);
			}
			if ((i / CHUNK_WIDTH) < z0)
			{
				z0 = (i / CHUNK_WIDTH);
			}
			if ((i / CHUNK_WIDTH) > z1)
			{
				z1 = (i / CHUNK_WIDTH);
			}
		}
	}

 	if (recalcHeight)
 	{
// 		chunk->recalcHeightmap();
// 		//chunk->recalcBlockLights();
 		level->setTilesDirty((packet->x << 4) + x0, y0, (packet->z << 4) + z0, (packet->x << 4) + x1, y1, (packet->z << 4) + z1);
// 		int rx = packet->x << 4;
// 		int rz = packet->z << 4;
// 		level->updateLight(LightLayer::Block, x0 + rx - 1, y0, z0 + rz - 1, x1 + rx + 1, y1, z1 + rz + 1);
// 		//for (int cx = x0; cx < x1; cx++)
// 		//{
// 		//	for (int cz = z0; cz < z1; cz++)
// 		//	{
// 		//		for (int cy = y0; cy < y1; cy++)
// 		//		{
// 					//level->updateLight(LightLayer::Sky, cx + rx, cy, cz + rz, cx + rx, cy, cz + rz);
// 		//			level->updateLight(LightLayer::Block, cx + rx - 1, cy, cz + rz - 1, cx + rx + 1, cy, cz + rz + 1);
// 		//		}
// 		//	}
// 		//}
// 
 	}
	//chunk->terrainPopulated = true;
	chunk->unsaved = false;

	chunksLoaded[packet->x * CHUNK_CACHE_WIDTH + packet->z] = true;

	if (areAllChunksLoaded())
	{
		ReadyPacket packet(ReadyPacket::READY_REQUESTEDCHUNKS);
		raknetInstance->send(packet);

		for (unsigned int i = 0; i < bufferedBlockUpdates.size(); i++)
		{
			const SBufferedBlockUpdate& update = bufferedBlockUpdates[i];
			int tileId = Tile::transformToValidBlockId( update.blockId, update.x, update.y, update.z );
			if (update.setData)
				level->setTileAndData(update.x, update.y, update.z, tileId, update.blockData);
			else
				level->setTile(update.x, update.y, update.z, tileId);
		}
		bufferedBlockUpdates.clear();
	}
	else
	{
		requestNextChunk();
	}
}

class _ChunkSorter
{
public:
    _ChunkSorter(int x, int z)
    :   x(x),
        y(z)
    {
    }
    
    bool operator() (const IntPair& a, const IntPair& b) {
        const int ax = a.x - x, ay = a.y - y;
        const int bx = b.x - x, by = b.y - y;
        return (ax*ax + ay*ay) < (bx*bx + by*by);
    }
private:
    int x;
    int y;
};


void ClientSideNetworkHandler::arrangeRequestChunkOrder() {
	clearChunksLoaded();

    // Default sort is around center of the world
    int cx = CHUNK_CACHE_WIDTH / 2;
    int cz = CHUNK_CACHE_WIDTH / 2;

    // If player exists, let's sort around him
    Player* p = minecraft.getPlayer();
    if (p) {
        cx = Mth::floor(p->x / (float)CHUNK_WIDTH);
        cz = Mth::floor(p->z / (float)CHUNK_DEPTH);
    }

    _ChunkSorter sorter(cx, cz);
    std::sort(requestNextChunkIndexList, requestNextChunkIndexList + NumRequestChunks, sorter);
}

void ClientSideNetworkHandler::levelGenerated(Level* level)
{
	this->level = level;
	ReadyPacket packet(ReadyPacket::READY_CLIENTGENERATION);
	raknetInstance->send(packet);

    arrangeRequestChunkOrder();
	requestNextChunk();
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, PlayerEquipmentPacket* packet)
{
	if (!level)
		return;

	Entity* entity = level->getEntity(packet->entityId);
	if (!entity || !entity->isPlayer())
		return;

	Player* player = (Player*)entity;
	// make sure it's not our local player
	if (player->owner == rakPeer->GetMyGUID())
	{
		printf("Attempted to modify local player's inventory\n");
		return;
	}
	// override the player's inventory
	//int slot = player->inventory->getSlot(packet->itemId, packet->itemAuxValue);
	//if (slot >= 0) {
	//	player->inventory->moveToSelectedSlot(slot, true);
		//item->id = packet->itemId;
		//item->setAuxValue(packet->itemAuxValue);
		//item->count = 63;
	int slot = Inventory::MAX_SELECTION_SIZE;
	if (slot >= 0) {
		if (packet->itemId == 0) {
			player->inventory->clearSlot(slot);
		} else {
			ItemInstance newItem(packet->itemId, 63, packet->itemAuxValue);
			player->inventory->replaceSlot(slot, &newItem);
		}
		player->inventory->moveToSelectedSlot(slot, true);
	} else {
		LOGW("Warning: Remote player doesn't have his thing, Odd!\n");
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, PlayerArmorEquipmentPacket* packet) {
	if (!level)
		return;

	Entity* entity = level->getEntity(packet->entityId);
	if (!entity || !entity->isPlayer())
		return;

	Player* player = (Player*)entity;
	// make sure it's not our local player, since that should be UpdateArmorPacket
	if (player->owner == rakPeer->GetMyGUID()) {
		printf("Attempted to modify local player's armor visually\n");
		return;
	}

	packet->fillIn(player);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, InteractPacket* packet)
{
	if (!level)
		return;

	Entity* src = level->getEntity(packet->sourceId);
	Entity* entity = level->getEntity(packet->targetId);
	if (src && entity && src->isPlayer())
	{
		Player* player = (Player*) src;
		if (InteractPacket::Attack == packet->action)
			minecraft.gameMode->attack(player, entity);
		if (InteractPacket::Interact == packet->action)
			minecraft.gameMode->interact(player, entity);
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, SetEntityDataPacket* packet)
{
	if (!level)
		return;

	LOGI("SetEntityDataPacket\n");

	Entity* e = level->getEntity(packet->id);
	if (e) {
		SynchedEntityData* data = e->getEntityData();
		if (data)
			data->assignValues(&packet->getUnpackedData());
	}
}

void ClientSideNetworkHandler::handle( const RakNet::RakNetGUID& source, SetEntityMotionPacket* packet )
{
	if (!level)
		return;

	if (Entity* e = level->getEntity(packet->id)) {
		/*
		e->xd = packet->xd;
		e->yd = packet->yd;
		e->zd = packet->zd;
		*/
		e->lerpMotion(packet->xd, packet->yd, packet->zd);
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, AnimatePacket* packet)
{
	if (!level)
		return;

    // Own player - Then don't play... :
    if (minecraft.getPlayer()->entityId == packet->entityId) {
        if (packet->action == AnimatePacket::Swing) return;
    }

	Entity* entity = level->getEntity(packet->entityId);
	if (!entity || !entity->isPlayer())
        return;
    
	Player* player = (Player*) entity;

    switch (packet->action) {
	case AnimatePacket::Swing:
		player->swing();
		break;
	case AnimatePacket::WAKE_UP:
		player->stopSleepInBed(false, false, false);
		break;
	default:
		LOGW("Unknown Animate action: %d\n", packet->action);
		break;
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, UseItemPacket* packet)
{
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, SetHealthPacket* packet)
{
	if (!level || !minecraft.getPlayer())
		return;

	minecraft.getPlayer()->hurtTo(packet->health);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, SetSpawnPositionPacket* packet) {
	if (!level || !minecraft.getPlayer()) return;
	if (!level->inRange(packet->x, packet->y, packet->z)) return;

	minecraft.getPlayer()->setRespawnPosition(Pos(packet->x, packet->y, packet->z));
	level->getLevelData()->setSpawn(packet->x, packet->y, packet->z);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, HurtArmorPacket* packet) {
    if (!level || !minecraft.getPlayer()) {
        return;
    }

    minecraft.getPlayer()->hurtArmor(packet->dmg);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, RespawnPacket* packet)
{
	if (level) {
		//LOGI("RespawnPacket! %d\n", findPlayer(level, packet->entityId, NULL));
		NetEventCallback::handle(level, source, packet );
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, ContainerOpenPacket* packet)
{
	if (!level)
		return;

	if (packet->type == ContainerType::FURNACE) {
		FurnaceTileEntity* te = new FurnaceTileEntity();
		te->clientSideOnly = true;
		minecraft.getPlayer()->openFurnace(te);
		if (minecraft.getPlayer()->containerMenu)
			minecraft.getPlayer()->containerMenu->containerId = packet->containerId;
	}
	if (packet->type == ContainerType::CONTAINER) {
		ChestTileEntity* te = new ChestTileEntity();
		te->clientSideOnly = true;
		minecraft.getPlayer()->openContainer(te);
		if (minecraft.getPlayer()->containerMenu)
			minecraft.getPlayer()->containerMenu->containerId = packet->containerId;
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, ContainerClosePacket* packet)
{
	if (minecraft.getPlayer() && minecraft.getPlayer()->containerMenu)
		minecraft.getPlayer()->closeContainer();
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, ContainerSetContentPacket* packet)
{
	if (!minecraft.getPlayer())
		return;

    if (packet->containerId == 0) {
        for (unsigned int i = 0; i < packet->items.size(); ++i) {
            minecraft.getPlayer()->inventory->setItem(Inventory::MAX_SELECTION_SIZE + i, &packet->items[i]);
        }
    } else if (minecraft.getPlayer()->containerMenu && minecraft.getPlayer()->containerMenu->containerId == packet->containerId) {
        for (unsigned int i = 0; i < packet->items.size(); ++i) {
            minecraft.getPlayer()->containerMenu->setSlot(i, &packet->items[i]);
        }
    }
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, ContainerSetSlotPacket* packet)
{
	//LOGI("ContainerSetSlot\n");

	if (!minecraft.getPlayer()
	  || !minecraft.getPlayer()->containerMenu
	  || minecraft.getPlayer()->containerMenu->containerId != packet->containerId)
	  return;

	//minecraft.getPlayer()->containerMenu->setSlot(packet->slot, packet->item.isNull()? NULL : &packet->item);
	minecraft.getPlayer()->containerMenu->setSlot(packet->slot, &packet->item);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, ContainerSetDataPacket* packet)
{
	//LOGI("ContainerSetData\n");
	if (minecraft.getPlayer() && minecraft.getPlayer()->containerMenu && minecraft.getPlayer()->containerMenu->containerId == packet->containerId) {
		//LOGI("client: SetData2 %d, %d\n", packet->id, packet->value);
		minecraft.getPlayer()->containerMenu->setData(packet->id, packet->value);
	}
}

void ClientSideNetworkHandler::handle( const RakNet::RakNetGUID& source, ChatPacket* packet )
{
#ifndef STANDALONE_SERVER
	minecraft.gui.displayClientMessage(packet->message);
#endif
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, SignUpdatePacket* packet)
{
	if (!level)
		return;

	TileEntity* te = level->getTileEntity(packet->x, packet->y, packet->z);
	if (TileEntity::isType(te, TileEntityType::Sign)) {
		SignTileEntity* ste = (SignTileEntity*) te;
		if (ste->isEditable()) {
			for (int i = 0; i < SignTileEntity::NUM_LINES; i++) {
				ste->messages[i] = packet->lines[i];
			}
			//ste->setChanged();
		}
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& source, AdventureSettingsPacket* packet) {
    if (!level)
		return;
	//assert(level != NULL && "level is NULL @ handle(AdventureSettingsPacket*)");

    packet->fillIn(level->adventureSettings);
}

void ClientSideNetworkHandler::clearChunksLoaded()
{
	// Init the chunk positions
	for (int i = 0; i < NumRequestChunks; ++i) {
		requestNextChunkIndexList[i].x = i/CHUNK_WIDTH;
		requestNextChunkIndexList[i].y = i%CHUNK_WIDTH;
		chunksLoaded[i] = false;
	}
}
