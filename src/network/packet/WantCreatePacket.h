#ifndef NET_MINECRAFT_NETWORK_PACKET__WantCreatePacket_H__
#define NET_MINECRAFT_NETWORK_PACKET__WantCreatePacket_H__

//package net.minecraft.network.packet;

#include "../Packet.h"

class WantCreatePacket: public Packet
{
public:
    WantCreatePacket() {
    }

    WantCreatePacket(int playerId, int count, int auxValue, int itemId)
	:
		playerId(playerId),
        count(count),
		auxValue(auxValue),
		itemId(itemId)
	{
    }

	void write(RakNet::BitStream* bitStream)
	{
		bitStream->Write((RakNet::MessageID)(ID_USER_PACKET_ENUM + PACKET_WANTCREATEITEM));
		bitStream->Write(itemId);
		bitStream->Write(count);
		bitStream->Write(auxValue);

		bitStream->Write(playerId);
	}

	void read(RakNet::BitStream* bitStream)
	{
        bitStream->Read(itemId);
        bitStream->Read(count);
        bitStream->Read(auxValue);

		bitStream->Read(playerId);
	}

	void handle(const RakNet::RakNetGUID& source, NetEventCallback* callback)
	{
		callback->handle(source, (WantCreatePacket*)this);
	}
    
    int playerId;

    int itemId;
	int count;
	int auxValue;
};

#endif /*NET_MINECRAFT_NETWORK_PACKET__WantCreatePacket_H__*/
