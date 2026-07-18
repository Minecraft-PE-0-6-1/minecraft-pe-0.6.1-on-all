#ifndef NET_MINECRAFT_NETWORK_PACKET__LoginPacket_H__
#define NET_MINECRAFT_NETWORK_PACKET__LoginPacket_H__

#include "../Packet.h"

class LoginPacket : public Packet
{
public:
	RakNet::RakString clientName;
	int clientNetworkVersion;
	int clientNetworkLowestSupportedVersion;
	bool newProto;

	LoginPacket()
	:	clientNetworkVersion(-1),
		clientNetworkLowestSupportedVersion(-1),
		newProto(false)
	{
	}

	LoginPacket(const RakNet::RakString& clientName, int clientVersion, bool newProto)
	:	clientName(clientName),
		clientNetworkVersion(clientVersion),
		clientNetworkLowestSupportedVersion(clientVersion),
		newProto(newProto)
	{
	}

	void write(RakNet::BitStream* bitStream)
	{
		bitStream->Write((RakNet::MessageID)(ID_USER_PACKET_ENUM + PACKET_LOGIN));
		bitStream->Write(clientName);
		bitStream->Write(clientNetworkVersion);
		bitStream->Write(clientNetworkLowestSupportedVersion);
		bitStream->Write(newProto);
	}

	void read(RakNet::BitStream* bitStream)
	{
		bitStream->Read(clientName);
		// First versions didn't send the client version
		//LOGI("unread: %d\n", bitStream->GetNumberOfUnreadBits());
		if (bitStream->GetNumberOfUnreadBits() > 0) {
			bitStream->Read(clientNetworkVersion);
			bitStream->Read(clientNetworkLowestSupportedVersion);
			
			// Checking for new proto
			if (bitStream->GetNumberOfUnreadBits() > 0) {
				bitStream->Read(newProto);
			}
		}
	}

	void handle(const RakNet::RakNetGUID& source, NetEventCallback* callback)
	{
		callback->handle(source, (LoginPacket*)this);
	}
};

#endif /*NET_MINECRAFT_NETWORK_PACKET__LoginPacket_H__*/
