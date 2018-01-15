#pragma once

class CPacketParser
{
private:
	static CPacketParser m_cSingleton;
public:
	static CPacketParser *This()
	{
		return &m_cSingleton;
	}
public:
	CPacketParser() {}
	~CPacketParser() {}
};