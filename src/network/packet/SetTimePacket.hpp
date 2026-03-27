#pragma once

//package net.minecraft.network.packet;

#include "network/Packet.hpp"
#include <cstdint>

class SetTimePacket: public Packet {
public:
    SetTimePacket() {
    }

    SetTimePacket(long time)
	:	time((int32_t)time)
	{}

	void write(RakNet::BitStream* bitStream)
	{
		bitStream->Write((RakNet::MessageID)(ID_USER_PACKET_ENUM + PACKET_SETTIME));
		bitStream->Write(time);
	}

	void read(RakNet::BitStream* bitStream)
	{
		bitStream->Read(time);
	}

	void handle(const RakNet::RakNetGUID& source, NetEventCallback* callback)
	{
		callback->handle(source, (SetTimePacket*)this);
	}
	
	int32_t time;
};

