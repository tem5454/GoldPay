#pragma once
#include <map>

class JSPC;

class JSPCManager
{
private:
	static JSPCManager m_cSingleton;

public:
	static JSPCManager* This()
	{
		return &m_cSingleton;
	}
public:
	JSPCManager();
	~JSPCManager();

private:
	std::map<int, JSPC*> m_mapJSPCMap;
};