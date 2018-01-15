#pragma once
#include "JPDefine.h"

class JSPC
{
private:
	int m_iAccountUniqueID;
	char m_cCardNumber[g_iCardNumber];
public:
	JSPC();
	~JSPC();

	void Set(int iAccountUniqueID, char* pcCardNumber);

	int GetAccountUniqueID();
	char* GetCardNumber();
};