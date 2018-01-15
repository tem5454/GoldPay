#include "stdafx.h"
#include "Session.h"
#include "JPDefine.h"

Session::Session()
{
	type_ = SESSION_TYPE_CLIENT;
	this->updateHearBeat();
}

Session::~Session()
{
	this->onClose();
}

bool Session::setSocketOpt()
{
	tcp_keepalive keepAliveSet = { 0 }, returned = { 0 };
	keepAliveSet.onoff = 1;
	keepAliveSet.keepaliveinterval = 3000;
	keepAliveSet.keepalivetime = 3000;

	DWORD dwBytes;
	if (WSAIoctl(socketData_.socket_, SIO_KEEPALIVE_VALS, &keepAliveSet, sizeof(keepAliveSet), &returned, sizeof(returned), &dwBytes, NULL, NULL) != 0)
	{
		return false;
	}

	return true;
}


bool Session::onAccept(SOCKET socket, SOCKADDR_IN addrInfo)
{
	socketData_.socket_ = socket;
	
	int addrLen = 0;
	getpeername(socketData_.socket_, (struct sockaddr*)&socketData_.addrInfo_, &addrLen);
	socketData_.addrInfo_ = addrInfo;
	if (!this->setSocketOpt())
	{
		return false;
	}
	return true;
}

void Session::onClose()
{
	//SessionManager::getInstance().closeSession(this);
}

SOCKET& Session::socket()
{
	return socketData_.socket_;
}

std::string Session::clientAddress()
{
	array<char, SIZE_64> ip;
	inet_ntop(AF_INET, &(socketData_.addrInfo_.sin_addr), ip.data(), ip.size());
	return ip.data();
}

UINT64 Session::id()
{
	return id_;
}

void Session::setId(UINT64 id)
{
	id_ = id;
}

int8_t Session::type()
{
	return type_;
}

void Session::setType(int8_t type)
{
	type_ = type;
}

std::time_t Session::heartBeat()
{
	return lastHeartBeat_;
}

void Session::updateHearBeat()
{
	lastHeartBeat_ = CLOCK.systemTick();
}