#pragma once
#include "CatModelExt.h"

class CATMODEL_CLASS TLargeString
{
public:
	TLargeString();
	~TLargeString();

public:
	void Add(char* psz);
	void Clear();
	char* GetBuffer() {return (char*)m_pData;};

protected:
	void MakeRoom(int nSize);

protected:
	char *m_pData;
	int m_nEnd;
	int m_nSize;
};

