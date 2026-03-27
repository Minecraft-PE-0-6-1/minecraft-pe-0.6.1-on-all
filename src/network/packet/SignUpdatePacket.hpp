#pragma once

//package net.minecraft.network.packet;

#include "network/Packet.hpp"
#include "util/RakDataIO.hpp"
#include "world/level/tile/entity/SignTileEntity.hpp"

class SignUpdatePacket: public Packet
{
public:
    SignUpdatePacket() {}

    SignUpdatePacket(int x, int y, int z, const std::string lines[])
    :   x(x),
        y(y),
        z(z)
    {
        for (int i = 0; i < 4; ++i)
            this->lines[i] = lines[i];
    }

    /*@Override*/
    void read(RakNet::BitStream* bitStream) {
        short xx, zz;
        unsigned char yy;
        bitStream->Read(xx);
        bitStream->Read(yy);
        bitStream->Read(zz);
        x = xx;
        z = zz;
        y = yy;

        RakDataInput dis(*bitStream);
        for (int i = 0; i < 4; ++i) {
            lines[i] = dis.readString();
            if (lines[i].length() > SignTileEntity::MAX_LINE_LENGTH)
                lines[i].resize(SignTileEntity::MAX_LINE_LENGTH);
        }
    }

    /*@Override*/
    void write(RakNet::BitStream* bitStream) {
        bitStream->Write((RakNet::MessageID)(ID_USER_PACKET_ENUM + PACKET_SIGNUPDATE));
        short xx = x;
        short zz = z;
        unsigned char yy = y;
        bitStream->Write(xx);
        bitStream->Write(yy);
        bitStream->Write(zz);

        RakDataOutput dos(*bitStream);
        for (int i = 0; i < 4; ++i)
            dos.writeString(lines[i]);
    }

    /*@Override*/
    void handle(const RakNet::RakNetGUID& source, NetEventCallback* callback) {
        callback->handle(source, (SignUpdatePacket*)this);
    }

public:
    int x, y, z;
    std::string lines[4];
};

