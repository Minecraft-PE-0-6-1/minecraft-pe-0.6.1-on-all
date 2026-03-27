#pragma once
#include "raknet/TCPInterface.h"
#include "raknet/RakString.h"
class NATPuchHandler {
public:
	enum NATPuchHandlerStatus {
		NATPuchInitilized = 0,
		NATPuchFetchingServerList = 1,
		NATPuchConnecting = 2,
		NATPuchConnected = 3,
		NATPuchDissconnected = 4
	};
	NATPuchHandler();
	~NATPuchHandler();
	void initialize();
	void registerToGameList(const RakNet::RakString& serverName, int port);
	void removeFromGameList();
	void close();
private:
	RakNet::TCPInterface *tcpInterface;
};

