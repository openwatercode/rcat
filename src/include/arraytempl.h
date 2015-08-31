#if !defined(TEMPLATEARRAY_CYR_20060110_INCLUDED)
#define TEMPLATEARRAY_CYR_20060110_INCLUDED

#include <stdlib.h>

template<class T, int N = 10>
class ARRAY
{
public:
	ARRAY()
	{
		m_nCount = 0;
		m_nSize = 0;
		m_pArray = NULL;
	};
	virtual ~ARRAY() {Clear();};

public:
	int GetCount() {return m_nCount;};
	T GetAt(int nIndex) {return m_pArray[nIndex];};
	T* GetAddr(int nIndex) {return &(m_pArray[nIndex]);};
	virtual void Clear()
	{
		if(m_pArray)
		{
			free(m_pArray);
			m_pArray = NULL;
		}
		m_nCount = 0;
		m_nSize = 0;
	}
	BOOL IsExist(T P)
	{
		int nIndex;

		for(nIndex = 0; nIndex < m_nCount; nIndex++)
		{
			if(GetAt(nIndex) == P)
				return TRUE;
		}

		return FALSE;
	}
	int Find(T P)
	{
		int nIndex;

		for(nIndex = 0; nIndex < m_nCount; nIndex++)
		{
			if(GetAt(nIndex) == P)
				return  nIndex;
		}

		return -1;
	}
	virtual int Add(T P)
	{
		MakeRoomFor(m_nCount + 1);
		m_pArray[m_nCount] = P;
		m_nCount++;
		return m_nCount;
	}
	virtual void Remove(int nIndex)
	{
		if(nIndex >= 0 && nIndex < m_nCount)
		{
			memmove(&m_pArray[nIndex], &m_pArray[nIndex + 1], sizeof(T) * (m_nCount - nIndex - 1));
			m_nCount--;
		}
	}
	void Swap(int nIndex1, int nIndex2)
	{
		T t = m_pArray[nIndex1];
		m_pArray[nIndex1] = m_pArray[nIndex2];
		m_pArray[nIndex2] = t;
	}
	BOOL SetSize(int nSize)
	{
		if(nSize < m_nSize)
			return TRUE;

		m_pArray = (T*)realloc(m_pArray, sizeof(T) * nSize);

		if(m_pArray)
		{
			memset(&m_pArray[m_nSize], 0, sizeof(T) * (nSize - m_nSize));
			m_nSize = nSize;
		}
		else
		{
            // TODO (hspark#1#): 오류 반환방법 강구할 필요 있음
			//DWORD nError = GetLastError();
			m_nSize = m_nCount = 0;
		}

		return m_pArray != NULL;
	}
	void SetCount(int nCount) {m_nCount = nCount;};

protected:
	void MakeRoomFor(int nSize)
	{
		if(nSize < m_nSize)
			return;

		m_nSize = nSize + N;
		m_pArray = (T*)realloc(m_pArray, sizeof(T) * m_nSize);
	}

public:
	T* m_pArray;
	int m_nCount;
	int m_nSize;
};

template<class T, int N = 10>
class PTRARRAY
{
protected:
	typedef T* pT;

public:
	PTRARRAY()
	{
		m_nCount = 0;
		m_nSize = 0;
		m_pArray = NULL;
	};
	virtual ~PTRARRAY() {Clear();};

public:
	int GetCount() {return m_nCount;};
	T* GetAt(int nIndex) {return m_pArray[nIndex];};
	virtual void Clear()
	{
		int nIndex;

		if(m_pArray == NULL)
			return;

		for(nIndex = 0; nIndex < m_nCount; nIndex++)
			delete m_pArray[nIndex];
		free(m_pArray);

		m_pArray = NULL;
		m_nCount = 0;
		m_nSize = 0;
	}
	virtual int Add(T* P)
	{
		MakeRoomFor(m_nCount + 1);
		m_pArray[m_nCount] = P;
		m_nCount++;
		return m_nCount;
	}
	virtual void Remove(int nIndex)
	{
		if(nIndex >= 0 && nIndex < m_nCount)
		{
			T* pItem = m_pArray[nIndex];
			memmove(&m_pArray[nIndex], &m_pArray[nIndex + 1], sizeof(pT) * (m_nCount - nIndex - 1));
			m_nCount--;
			delete pItem;
		}
	}
	virtual void Remove(T* pObj)
	{
		int nIndex;

		for(nIndex = 0; nIndex < m_nCount; nIndex++)
		{
			if(GetAt(nIndex) == pObj)
			{
				Remove(nIndex);
				break;
			}
		}

	}
	void Swap(int nIndex1, int nIndex2)
	{
		pT t = m_pArray[nIndex1];
		m_pArray[nIndex1] = m_pArray[nIndex2];
		m_pArray[nIndex2] = t;
	}
	BOOL SetSize(int nSize)
	{
		if(nSize < m_nSize)
			return TRUE;

		m_pArray = (pT*)realloc(m_pArray, sizeof(pT) * nSize);
		memset(&m_pArray[m_nSize], 0, sizeof(pT) * (nSize - m_nSize));

		if(m_pArray)
			m_nSize = nSize;
		else
			m_nSize = m_nCount = 0;

		return m_pArray != NULL;
	}
	void SetCount(int nCount) {m_nCount = nCount;};

protected:
	void MakeRoomFor(int nSize)
	{
		if(nSize < m_nSize)
			return;

		m_nSize = nSize + N;
		m_pArray = (pT*)realloc(m_pArray, sizeof(pT) * m_nSize);
	}

public:
	pT* m_pArray;
	int m_nCount;
	int m_nSize;
};

#endif
