#pragma once

#include "network/Packet.hpp"
#include "world/entity/player/Player.hpp"

class RemoveBlockPacket : public Packet
{
public:

	// the id of the player who is placing the block, used to animate the player
	int entityId;
	int x, z;
	unsigned char y;

	RemoveBlockPacket()
	{
	}

	RemoveBlockPacket(Player* p, int x, int y, int z)
	:	entityId(p->entityId),
		x(x),
		y((unsigned char)(y & 0xff)),
		z(z)
	{
	}

	void write(RakNet::BitStream* bitStream)
	{
		bitStream->Write((RakNet::MessageID)(ID_USER_PACKET_ENUM + PACKET_REMOVEBLOCK));

		bitStream->Write(entityId);
		bitStream->Write(x);
		bitStream->Write(z);
		bitStream->Write(y);
	}

	void read(RakNet::BitStream* bitStream)
	{
		bitStream->Read(entityId);
		bitStream->Read(x);
		bitStream->Read(z);
		bitStream->Read(y);
	}

	void handle(const RakNet::RakNetGUID& source, NetEventCallback* callback)
	{
		callback->handle(source, (RemoveBlockPacket*)this);
	}
};

