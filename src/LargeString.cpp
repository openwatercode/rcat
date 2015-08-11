#include "StdAfx.h"
#include "include/LargeString.h"


TLargeString::TLargeString()
{
	m_pData = (char*)malloc(sizeof(char) * 1024);
	m_nSize = 1024;
	memset(m_pData, 0, sizeof(char) * m_nSize);
	m_nEnd = 0;
}

TLargeString::~TLargeString()
{
	Clear();
}

void TLargeString::Clear()
{
	free(m_pData);
	m_pData = NULL;
	m_nSize = 0;
	m_nEnd = 0;
}

void TLargeString::MakeRoom(int nSize)
{
	if((m_nSize - m_nEnd) <= nSize)
	{
		m_nSize += ((nSize / 256 + 1) * 256);
		m_pData = (char*)realloc(m_pData, sizeof(char) * m_nSize);
	}
}

void TLargeString::Add(char* psz)
{
	int nSize = strlen(psz);

	if((m_nSize - m_nEnd) <= nSize)
		MakeRoom(nSize);
	memcpy(m_pData + m_nEnd, psz, sizeof(TCHAR) * (nSize + 1));
	m_nEnd += nSize;
}
