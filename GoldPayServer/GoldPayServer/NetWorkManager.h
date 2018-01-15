#pragma once
#include "stdafx.h"
#include "JSStruct.h"
#include "JPDefine.h"
#include "JPEnum.h"

class CNetWorkManager
{
private:
	static CNetWorkManager m_cSingleton;

public:
	static CNetWorkManager* This()
	{
		return &m_cSingleton;
	}

public:
	CNetWorkManager() {}
	~CNetWorkManager() {}

private:
	SOCKET		m_hSocket;
	WSADATA		m_wsaData;
	SOCKADDR_IN m_addr;

public:
	bool Init();
	bool CreateSocket();
	bool Connect(std::string strIP, int iPort);
	bool Send(SOCKET hSocket, stOverlap &stoverlap);
};