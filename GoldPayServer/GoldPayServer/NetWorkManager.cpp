#include "stdafx.h"
#include "NetWorkManager.h"

CNetWorkManager CNetWorkManager::m_cSingleton;

bool CNetWorkManager::Init()
{
	if (0 != WSAStartup(MAKEWORD(2, 2), &m_wsaData))
	{
		return false;
	}
	return true;
}
bool CNetWorkManager::CreateSocket()
{
	bool bResult = true;
	
	m_hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == m_hSocket)
	{
		printf("CreateSocket() WSASocket Error\n");
		bResult = false;
		WSACleanup();
	}
	else
	{
		printf("CreateSocket OK.\n");
	}


	return bResult;
}
bool CNetWorkManager::Connect(std::string strIP, int iPort)
{
	bool bResult = true;

	memset(&m_addr, 0, sizeof(m_addr));
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(iPort);
	inet_pton(AF_INET, strIP.c_str(), &(m_addr.sin_addr));

	int ret = ::connect(m_hSocket, (sockaddr*)&m_addr, sizeof(m_addr));

	if( ret == SOCKET_ERROR)
	{
		printf("Connect() Error...\n");
		closesocket(m_hSocket);
		WSACleanup();
		bResult = false;
	}
	else
	{
		printf("Connect() Ok...\n");
	}
	
	return bResult;
}

bool CNetWorkManager::Send(SOCKET hSocket, stOverlap &stoverlap)
{
	bool bResult = true;
	
	BOOL result;
	DWORD err;

	stoverlap.hSocket = hSocket;
	stoverlap.eState = eState_Writing;
	stoverlap.nSize = stoverlap.SendString.size();
	stoverlap.sendBuffer.len = stoverlap.nSize;
	stoverlap.sendBuffer.buf = const_cast<char*>(stoverlap.SendString.c_str());

	result = WSASend(m_hSocket, &stoverlap.sendBuffer, 1, &stoverlap.nSize, 0, &stoverlap, 0);

	result = (result != SOCKET_ERROR);
	err = WSAGetLastError();

	if (!result)
	{
		if (err != ERROR_IO_PENDING)
		{
			//CloseSocket(stoverlap, true);
		}
		else
		{

		}
	}
	else
	{

	}

	return bResult;
}