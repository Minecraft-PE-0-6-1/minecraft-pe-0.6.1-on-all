#pragma once

//package net.minecraft.network.packet;

#include "../Packet.h"

class EntityEventPacket: public Packet {
public:
    EntityEventPacket() {}

    EntityEventPacket(int entityId, char eventId)
    :   entityId(entityId),
        eventId(eventId)
    {}

	void write(RakNet::BitStream* bitStream)
	{
		bitStream->Write((RakNet::MessageID)(ID_USER_PACKET_ENUM + PACKET_ENTITYEVENT));
		bitStream->Write(entityId);
		bitStream->Write(eventId);
	}

	void read(RakNet::BitStream* bitStream)
	{
		bitStream->Read(entityId);
		bitStream->Read(eventId);
	}

	void handle(const RakNet::RakNetGUID& source, NetEventCallback* callback)
	{
		callback->handle(source, (EntityEventPacket*)this);
	}

    int entityId;
    unsigned char eventId;

};

