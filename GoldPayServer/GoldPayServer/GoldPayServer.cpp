// GoldPayServer.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "JSPrecompile.h"
#include "NetWorkManager.h"

const int stOverlap::addrlen = sizeof(sockaddr_in) + 16;
const int stOverlap::initalReciveSize = 0;

SOCKET g_listener = INVALID_SOCKET;

HANDLE g_hIOCP;
int g_iconcurrency = 0;

OL g_OverlappedList;

void DoIo(stOverlap &stoverlap);
void CloseSocket(stOverlap &stoverlap, bool bForce = false, bool blistenAgain = true);
void InitOverlap(stOverlap &stOverlap );
void PostRead(stOverlap &stOverlap );
bool DoCommands(stOverlap &stOverlap );
void PostSend(stOverlap &stOverlap );

unsigned int __stdcall WorkerThread(void* arg)
{
	DWORD n;
	ULONG key;
	BOOL result;
	stOverlap* pOverlap;

	while (1)
	{
		result = GetQueuedCompletionStatus(g_hIOCP, &n, &key, (OVERLAPPED**)&pOverlap, INFINITE);

		if (FALSE == result || nullptr == pOverlap)
		{
			if (nullptr != pOverlap)
			{
				CloseSocket(*pOverlap, true, g_listener == INVALID_SOCKET ? false : true);
			}
		}
		else
		{
			pOverlap->nSize = n;
			DoIo(*pOverlap);
		}
	}
}

void PostRead(stOverlap &stoverlap)
{
	BOOL result; 
	if (stoverlap.end == &stoverlap.buf[g_iBufferSize])
	{
		stoverlap.end = &stoverlap.buf[0];
		stoverlap.ignore = true;
	}

	stoverlap.eState = eState_Reading;
	stoverlap.nSize = &stoverlap.buf[g_iBufferSize] - stoverlap.end;
	stoverlap.recvBuffer.len = stoverlap.nSize;
	stoverlap.recvBuffer.buf = stoverlap.end;
	stoverlap.recvFlag = 0;

	result = WSARecv(stoverlap.hSocket, &stoverlap.recvBuffer, 1, &stoverlap.nSize, &stoverlap.recvFlag, &stoverlap, 0);
	if (result)
	{
		return;
	}
	else
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			CloseSocket(stoverlap, true);
		}
		else
		{			
		}
	}
	return;
}

void PostSend(stOverlap &stoverlap)
{
	BOOL result;
	DWORD err;

	stoverlap.eState = eState_Writing;
	stoverlap.nSize = stoverlap.SendString.size();
	stoverlap.sendBuffer.len = stoverlap.nSize;
	stoverlap.sendBuffer.buf = const_cast<char*>(stoverlap.SendString.c_str());

	result = WSASend(stoverlap.hSocket, &stoverlap.sendBuffer, 1, &stoverlap.nSize, 0, &stoverlap, 0);

	result = (result != SOCKET_ERROR);
	err = WSAGetLastError();

	if (!result)
	{
		if (err != ERROR_IO_PENDING)
		{
			printf("err != ERROR_IO_PENDING\n");
			CloseSocket(stoverlap, true);
		}
		else
		{

		}
	}
	else
	{

	}
}

bool DoCommands(stOverlap &stoverlap)
{
	char buff[1024];
	
	memcpy(buff, stoverlap.buf, stoverlap.nSize);
	buff[stoverlap.nSize] = '\0';
	printf(stoverlap.buf);
	printf("\n");
	memset(stoverlap.buf, '\0', sizeof(stoverlap.buf));
	stoverlap.nSize = 0;
	stoverlap.end = stoverlap.buf;

	char dataSize[4];
	int iDataSize;
	memcpy(dataSize, buff, sizeof(char) * 4);

	iDataSize = atoi(dataSize);


	if (stoverlap.eState != eState_Accept)
	{
		stoverlap.SendString = buff;
		//PostSend(stoverlap);

		CNetWorkManager::This()->Send(g_listener, stoverlap);
	}

	return true;
}

void DoIo(stOverlap &stoverlap)
{
	int ilocallen = 0;
	int iremotelen = 0;
	string tt;
	sockaddr_in* plocal = 0;
	sockaddr_in* premote = 0;

reevaluate:
	switch (stoverlap.eState)
	{
	case eState_Accept:
		GetAcceptExSockaddrs(&stoverlap.buf[0], stOverlap::initalReciveSize,
			stoverlap.addrlen, stoverlap.addrlen, (sockaddr**)&plocal, &ilocallen,
			(sockaddr**)&premote, &iremotelen);

		memcpy(&stoverlap.local, plocal, sizeof(sockaddr_in));
		memcpy(&stoverlap.peer, premote, sizeof(sockaddr_in));

		setsockopt(stoverlap.hSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
			(char*)&g_listener, sizeof(g_listener));

		CreateIoCompletionPort((HANDLE)stoverlap.hSocket, g_hIOCP, 0, g_iconcurrency);
		tt = inet_ntoa(stoverlap.peer.sin_addr);

		if (stoverlap.nSize != 0)
		{
			stoverlap.eState = eState_Reading;

			goto reevaluate;
		}

		PostRead(stoverlap);
		printf("Connect Client...\n");
		break;
	case eState_Reading:
		printf("eState_Reading\n");
		if (stoverlap.hSocket == SOCKET_ERROR)
		{
			CloseSocket(stoverlap, true);
		}
		else if (stoverlap.nSize == 0)
		{
			CloseSocket(stoverlap, false);
		}
		else
		{
			stoverlap.end += stoverlap.nSize;
			if (!DoCommands(stoverlap))
			{
				PostRead(stoverlap);
			}
			else
			{

			}
		}
		break;
	case eState_Writing:
		printf("eState_Writing\n");
		if (stoverlap.hSocket == SOCKET_ERROR)
		{
			printf("stoverlap.hSocket == SOCKET_ERROR\n");
			CloseSocket(stoverlap, true);
		}
		else if (stoverlap.nSize == 0)
		{
			printf("stoverlap.nSize == 0\n");
			CloseSocket(stoverlap, false);
		}
		else
		{
			printf("PostSend\n");
			PostRead(stoverlap);
		}
		break;
	}
}

void CloseSocket(stOverlap& stOverlap, bool bForce, bool blistenAgain)
{
	struct linger li = { 0,0 };

	if (bForce)
	{
		li.l_onoff = 1;
	}

	setsockopt(stOverlap.hSocket, SOL_SOCKET, SO_LINGER, (char *)&li, sizeof(li));
	closesocket(stOverlap.hSocket);

	if (blistenAgain)
	{
		InitOverlap(stOverlap);
	}
}

void InitOverlap(stOverlap &stOverlap)
{
	int iZero = 0;
	stOverlap.hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (stOverlap.hSocket != INVALID_SOCKET)
	{
		stOverlap.eState = eState_Accept;
		stOverlap.ignore = false;
		stOverlap.end = &stOverlap.buf[0];

		setsockopt(stOverlap.hSocket, SOL_SOCKET, SO_SNDBUF, (char*)&iZero, sizeof(iZero));
		setsockopt(stOverlap.hSocket, SOL_SOCKET, SO_RCVBUF, (char*)&iZero, sizeof(iZero));

		memset(stOverlap.buf, '\0', sizeof(stOverlap.buf));

		AcceptEx(g_listener, stOverlap.hSocket, &stOverlap.buf[0], stOverlap::initalReciveSize, stOverlap.addrlen, stOverlap.addrlen, &stOverlap.nSize, &stOverlap);

	}
	else
	{

	}
}


int main()
{
	CNetWorkManager::This()->Init();
	CNetWorkManager::This()->CreateSocket();
	CNetWorkManager::This()->Connect("127.0.0.1", 15000);


	SYSTEM_INFO si;
	GetSystemInfo(&si);
	int iThreadCount = si.dwNumberOfProcessors;
	int iPort = 13000;
	int iMaxSocket = 30;
	int iZero = 0;
	unsigned int iDummy;

	OL ovl;
	OLI ovi;
	HANDLE hHandle;
	
	g_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, g_iconcurrency);

	WSADATA wd = { 0};
	WSAStartup(MAKEWORD(2, 2), &wd);

	/*SOCKADDR_IN addr;
	g_listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons( (short)iPort);
	bind(g_listener, (SOCKADDR*)&addr, sizeof(addr));
	listen(g_listener, 200);*/

	g_listener = WSASocket(AF_INET, SOCK_STREAM, NULL, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (g_listener == INVALID_SOCKET)
	{
		printf("listenSocket fail\n");
		return false;
	}

	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons((u_short)iPort);
	inet_pton(AF_INET, "127.0.0.1", &(serverAddr.sin_addr));

	int reUseAddr = 1;
	setsockopt(g_listener, SOL_SOCKET, SO_REUSEADDR, (char*)&reUseAddr, (int)sizeof(reUseAddr));

	int retval = ::bind(g_listener, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR)
	{
		printf("bind fail\n");
		return false;
	}

	const int BACK_SOCKET = 5;
	retval = ::listen(g_listener, BACK_SOCKET);
	if (retval == SOCKET_ERROR)
	{
		printf("listen fail\n");
		return false;
	}

	g_OverlappedList.clear();

	for (int i = 0; i < iMaxSocket; ++i)
	{
		stOverlap* pOverlap = new stOverlap;
		pOverlap->eState = eState_Accept;
		InitOverlap(*pOverlap);
		g_OverlappedList.push_back(pOverlap);
	}

	for (int i = 0; i < iThreadCount; ++i)
	{
		hHandle = (HANDLE)_beginthreadex(0, 0, WorkerThread, g_hIOCP, 0, &iDummy);
	}

	CreateIoCompletionPort((HANDLE)g_listener, g_hIOCP, 0, g_iconcurrency);

	printf("Server Start\n");
	while (true)
	{
		int a = 0;
	};

	for (ovi = g_OverlappedList.begin(); ovi != g_OverlappedList.end(); ++ovi)
	{
		closesocket((*ovi)->hSocket);
		delete(*ovi);
	}

	CloseHandle(g_hIOCP);
	WSACleanup();

    return 0;
}

