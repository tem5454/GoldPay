#include "stdafx.h"
#include "JSPC.h"

JSPC::JSPC()
{

}

JSPC::~JSPC()
{

}

void JSPC::Set(int iAccountUniqueID,char* pcCardNumber)
{
	m_iAccountUniqueID = iAccountUniqueID;

	if (pcCardNumber)
	{
		memcpy(m_cCardNumber, pcCardNumber, sizeof(char) * g_iCardNumber);
	}
}

int JSPC::GetAccountUniqueID()
{
	return m_iAccountUniqueID;
}

char* JSPC::GetCardNumber()
{
	return m_cCardNumber;
}