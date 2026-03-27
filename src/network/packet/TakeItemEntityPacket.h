#pragma once

//package net.minecraft.network.packet;

#include "../Packet.h"

class TakeItemEntityPacket: public Packet
{
public:
    TakeItemEntityPacket() {
    }

    TakeItemEntityPacket(int itemId, int playerId)
	:	itemId(itemId),
		playerId(playerId)
	{
    }

	void write(RakNet::BitStream* bitStream)
	{
		bitStream->Write((RakNet::MessageID)(ID_USER_PACKET_ENUM + PACKET_TAKEITEMENTITY));
		bitStream->Write(itemId);
		bitStream->Write(playerId);
	}

	void read(RakNet::BitStream* bitStream)
	{
		bitStream->Read(itemId);
		bitStream->Read(playerId);
	}

	void handle(const RakNet::RakNetGUID& source, NetEventCallback* callback)
	{
		callback->handle(source, (TakeItemEntityPacket*)this);
	}

    int itemId;
    int playerId;
};

