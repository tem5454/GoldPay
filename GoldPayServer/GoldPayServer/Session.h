#pragma once
#include "stdafx.h"

struct SOCKET_DATA
{
	SOCKET			socket_;
	SOCKADDR_IN		addrInfo_;
};

enum {
	SESSION_TYPE_TERMINAL,
	SESSION_TYPE_CLIENT,
};

class Session
{
protected:
	SOCKET_DATA  socketData_;
	UINT64		 id_;
	int8_t		 type_;
	bool		 setSocketOpt();
	std::time_t	 lastHeartBeat_;
public:
	Session();
	virtual ~Session();

	virtual bool onAccept(SOCKET socket, SOCKADDR_IN addrInfo);
	virtual void onSend(size_t transferSize) = 0;
	//virtual void sendPacket(Packet* packet) = 0;

	//virtual Package* onRecv(size_t transferSize) = 0;
	virtual void	 recvStandBy() {};

	virtual void onClose();

	SOCKET& socket();
	std::string	   clientAddress();

	UINT64 id();
	void setId(UINT64 id);

	int8_t type();
	void setType(int8_t type);

	std::time_t heartBeat();
	void updateHearBeat();

};