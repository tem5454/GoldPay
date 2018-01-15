#pragma once
#include "stdafx.h"
#include "JSPrecompile.h"

struct stOverlap : public OVERLAPPED
{
	static const int addrlen;
	static const int initalReciveSize;

	EState eState;
	SOCKET hSocket;
	DWORD nSize;
	char* end;
	bool ignore;
	sockaddr_in local;
	sockaddr_in peer;

	string SendString;
	WSABUF sendBuffer;
	WSABUF recvBuffer;
	DWORD recvFlag;

	char buf[g_iBufferSize];

	stOverlap() : eState(eState_Closed), ignore(false), hSocket(SOCKET_ERROR), end(&buf[0])
	{
		Internal = 0;
		InternalHigh = 0;
		Offset = 0;
		OffsetHigh = 0;
		hEvent = 0;
	}

	stOverlap(const stOverlap& o)
	{
		eState = o.eState;
		hSocket = o.hSocket;
		nSize = o.nSize;
		end = &buf[0] + (o.end - &o.buf[0]);
		ignore = o.ignore;
		peer = o.peer;
		memcpy(buf, o.buf, g_iBufferSize);
	}

	~stOverlap() {}
};

typedef list<stOverlap*> OL;
typedef OL::iterator OLI;




