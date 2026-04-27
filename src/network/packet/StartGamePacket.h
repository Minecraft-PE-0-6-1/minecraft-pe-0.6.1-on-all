#ifndef NET_MINECRAFT_NETWORK_PACKET__StartGamePacket_H__
#define NET_MINECRAFT_NETWORK_PACKET__StartGamePacket_H__

#include "../Packet.h"
#include "world/level/LevelConstants.h"
#include <cstdint>

class StartGamePacket : public Packet
{
public:
	int32_t levelSeed;
	int levelGeneratorVersion;
	int gameType;

	int entityId;
	float x, y, z;

	int chunkCacheWidth = 0;
	StartGamePacket()
	{
	}

	StartGamePacket(long seed, int levelGeneratorVersion, int gameType, int entityId, float x, float y, float z, int chunkCacheWidth)
	:	levelSeed((int32_t)seed),
		levelGeneratorVersion(levelGeneratorVersion),
		gameType(gameType),
		entityId(entityId),
		x(x),
		y(y),
		z(z),
		chunkCacheWidth(chunkCacheWidth)
	{
	}

	void write(RakNet::BitStream* bitStream)
	{
		bitStream->Write((RakNet::MessageID)(ID_USER_PACKET_ENUM + PACKET_STARTGAME));

		bitStream->Write(levelSeed);
		bitStream->Write(levelGeneratorVersion);
		bitStream->Write(gameType);
		bitStream->Write(entityId);
		bitStream->Write(x);
		bitStream->Write(y);
		bitStream->Write(z);
		bitStream->Write(CHUNK_CACHE_WIDTH);
	}

	void read(RakNet::BitStream* bitStream)
	{
		bitStream->Read(levelSeed);
		bitStream->Read(levelGeneratorVersion);
		bitStream->Read(gameType);
		bitStream->Read(entityId);
		bitStream->Read(x);
		bitStream->Read(y);
		bitStream->Read(z);
		
		if (bitStream->GetNumberOfUnreadBits() > 0) {
			bitStream->Read(chunkCacheWidth);
		}
	}

	void handle(const RakNet::RakNetGUID& source, NetEventCallback* callback)
	{
		callback->handle(source, (StartGamePacket*)this);
	}
};

#endif /*NET_MINECRAFT_NETWORK_PACKET__StartGamePacket_H__*/
