#pragma once

#include "../Packet.h"

class AnimatePacket: public Packet
{
public:
    static const int Swing = 1;
	static const int WAKE_UP = 3;

    int entityId;
    char action;

    AnimatePacket() {}

    AnimatePacket(int action, int entityId)
    :   action(action),
        entityId(entityId)
    {}
    AnimatePacket(int action, Entity* e)
    :   action(action),
        entityId(e->entityId)
    {}

	void write(RakNet::BitStream* bitStream)
	{
		bitStream->Write((RakNet::MessageID)(ID_USER_PACKET_ENUM + PACKET_ANIMATE));

		bitStream->Write(action);
		bitStream->Write(entityId);
	}

	void read(RakNet::BitStream* bitStream)
	{
		bitStream->Read(action);
		bitStream->Read(entityId);
	}

	void handle(const RakNet::RakNetGUID& source, NetEventCallback* callback)
	{
		callback->handle(source, (AnimatePacket*)this);
	}
};

