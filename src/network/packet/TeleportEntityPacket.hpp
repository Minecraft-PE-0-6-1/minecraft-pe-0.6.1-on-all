#pragma once

//package net.minecraft.network.packet;

#include "network/Packet.hpp"

#include "world/entity/Entity.hpp"
#include "util/Mth.hpp"

class TeleportEntityPacket: public Packet
{
public:
    TeleportEntityPacket() {
    }

    TeleportEntityPacket(Entity* e, char cause)
    :   id(e->entityId),
        x(e->x),
        y(e->y),
        z(e->z),
        yRot(e->yRot),
        xRot(e->xRot),
		cause(cause)
    {
    }
    
    TeleportEntityPacket(int id, float x, float y, float z, float yRot, float xRot, char cause)
    :   id(id),
        x(x),
        y(y),
        z(z),
        yRot(yRot),
        xRot(xRot),
		cause(cause)
    {
    }

	void write(RakNet::BitStream* bitStream) {
		bitStream->Write((RakNet::MessageID)(ID_USER_PACKET_ENUM + PACKET_TELEPORTENTITY));
		bitStream->Write(id);
		bitStream->Write(x);
		bitStream->Write(y);
		bitStream->Write(z);
		bitStream->Write(PacketUtil::Rot_degreesToChar(yRot));
		bitStream->Write(PacketUtil::Rot_degreesToChar(xRot));
		bitStream->Write(cause);
	}

	void read(RakNet::BitStream* bitStream) {
		bitStream->Read(id);
		bitStream->Read(x);
		bitStream->Read(y);
		bitStream->Read(z);
		char rx, ry;
		bitStream->Read(ry);
		bitStream->Read(rx);
		bitStream->Read(cause);
		yRot = PacketUtil::Rot_degreesToChar(ry);
		xRot = PacketUtil::Rot_charToDegrees(rx);

	}

	void handle(const RakNet::RakNetGUID& source, NetEventCallback* callback)
	{
		callback->handle(source, (TeleportEntityPacket*)this);
	}

    int id;
    float x, y, z;
	float xRot, yRot; // sent as byte
    char cause;

	static const int WANT_RESPAWN = 1;
};

