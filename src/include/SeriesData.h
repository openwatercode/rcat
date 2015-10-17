#pragma once

#include "arraytempl.h"
#include <stdio.h>
#include "ModelDataExt.h"

/*
#if !defined(BOOL)
#define BOOL __int32
#endif

#if !defined(DWORD)
#define DWORD __int32
#endif

#if !defined(BYTE)
#define BYTE unsigned char
#endif

#if !defined(MAX_PATH)
#define MAX_PATH 256
#endif
*/

#define DATE_BASE_YEAR 1900
#define FILEMAGICNO 0x0527
#define FILECURVERSION 0x0011
#define FILEVERSION10 0x0010
#define FILEVERSION09 0x0009

class TSeries;
class TSerieses;

typedef struct _tagFieldItem
{
	char szNode[50];
	int nCount;
	int nFieldNo[20];
} FIELDITEM;

class TFieldList
{
public:
	TFieldList();
	~TFieldList();

public:
	int GetCount() {return m_nCount;};
	int Parsing(char* szFields);
	FIELDITEM* GetItem(int nIndex) {return &m_pFields[nIndex];};
	void Clear();

protected:
	void MakeRoom(void);

protected:
	FIELDITEM* m_pFields;
	int m_nCount;
	int m_nSize;
};

class MODELDATA_CLASS TDate : public TIMEDATA
{
public:
	TDate();
	TDate(int nYear, int nMonth, int nDay, int nHour, int nMinute);
	TDate(TIMEDATA& date);

public:
	static unsigned int GetYearDays(int nYear);
	static unsigned int GetMonthDays(int nYear, int nMonth);
	static unsigned int GetDays(int nYear, int nMonth, int nDay);
	static ULONG ToMinute(int nYear, int nMonth, int nDay, int nHour, int nMinute);
	static ULONG GetMinutes(TIMEDATA &time);
	static ULONG GetYearMinutes(int nYear);
	static ULONG GetMonthMinutes(int nYear, int nMonth);

public:
	ULONG GetMinutes();
	unsigned short GetYear() {return nYear;};
	unsigned short GetMonth() {return nMonth;};
	unsigned short GetDay() {return nDay;};
	unsigned short GetHour() {return nHour;};
	unsigned short GetMinute() {return nMinute;};
	void Increase(int nSpan);
	void SetDate(ULONG nMinutes);
	void SetDate(unsigned short nY, unsigned short nM, unsigned short nD, unsigned short nH, unsigned short nm);
	void operator =(ULONG date);
	void operator =(TDate& date);
	void operator +=(LONG span);
	void IncreaseTime(BYTE nStep);
	void AddMonth(int nMonth);
	void AddYear(int nYear);
	void AddHour(int nHours);
	void AddMinute(int nMinutes);
};

class MODELDATA_CLASS TSeriesItem : public ARRAY<DATATYPE, 10>
{
public:
	TSeriesItem(void);
	virtual ~TSeriesItem(void);

public:
	int GetCount() {return m_Header.nData == FT_DATE ? m_Date.GetCount() : m_nCount;};
	DATATYPE GetValue(int nIndex) {return GetAt(nIndex);};
	//inline void SetValue(int nIndex, DATATYPE val);
	void SetValue(int nIndex, DATATYPE val);
	char* GetHeader() {return m_Header.szHeader;};
	BOOL Save(FILE* pFile);
	BOOL Load(FILE* pFile);
	BOOL Load10(FILE* pFile);
	BOOL Load09(FILE *fp);
	int SetFirstPos(TIMEDATA& time);
	int SetFirstPos(ULONG nTime);
	DATATYPE GetNext();
	DATATYPE GetCurVal();
	unsigned short GetDataType() {return m_Header.nData;};
	unsigned short GetInterval() {return m_Header.nInterval;};
	int ConvertInterval(unsigned short nNewInter, unsigned short nMethod, TSeriesItem* pNew);
	int Copy(TSeriesItem* pCopy);
	int ConvertInterval(unsigned short nNewInter, unsigned short nMethod, BOOL bMul = TRUE);
	void AddValue(int nIndex, float nVal);
	TSeries* GetParent() {return m_pParent;};
	ULONG CopyPart(ULONG nStart, ULONG nEnd, TSeriesItem *pNew);
	void Sort(int nMethod = 0);
	void CalcAnnualStatic(int nYear);
	ULONG GetDate(int nIndex) {return m_Date.GetAt(nIndex);};
	void SetDate(int nIndex, ULONG nDate);
	ULONG GetNextDate();
	ULONG GetCurDate();
	void SetDateSize(int nSize) {m_Date.SetSize(nSize);};

protected:
	LONG m_nFirst;
	LONG m_nCurrent;
	ARRAY<ULONG, 100> m_Date;
	ULONG *m_pDate;

public:
	SERIESITEMHEADER m_Header;
	TSeries *m_pParent;
	TDate m_dtStart;
	float m_nYear;
	float m_nMonth[12];
};

class MODELDATA_CLASS TSeries : public PTRARRAY<TSeriesItem>
{
public:
	TSeries();
	virtual ~TSeries();

public:
//	BOOL Save(LPCTSTR szFile);
	BOOL Save(FILE *fp);
//	BOOL SaveText(FILE *fp, int nFieldNos[]);
	BOOL SaveText(FILE *fp, int nCount, int nFieldNos[]);
//	BOOL Load(LPCTSTR szFile);
	BOOL Load(FILE *fp);
	BOOL Load10(FILE *fp);
	BOOL Load09(FILE *fp);
	BOOL LoadHeader(char* szFile);
	DATATYPE GetValue(int nCol, int nRow);
	TSeriesItem* GetSeries(int nSeries) {return GetAt(nSeries);};
	TSeriesItem* FindSeries(unsigned short nType);
	TSeriesItem* AddItem(void);
	int SetFirstPos(ULONG nTime);
	BOOL MoveNext();
	void SetName(char* szName);
	int Copy(TSeries* pCopy);
	void ChangeInterval(unsigned short nNewInter, unsigned short nMethod);
	void ChangeIntervalByDefault(unsigned short nNewInter);
	int AddSeries(TSeriesItem* pItem);
	int RemoveSeries(unsigned short nType);
	TSerieses* GetParent() {return m_pParent;};
	void CalcAnnualStatic(int nYear);

public:
	SERIESHEADER m_Header;
	TDate m_dtStart;
	TSerieses* m_pParent;
};

class MODELDATA_CLASS TSerieses : public PTRARRAY<TSeries>
{
public:
	TSerieses();
	virtual ~TSerieses();

public:
	int Load(char* szFile);
	BOOL Save(char* szFile);
	BOOL Open(char* szFile);
	BOOL Load09(char* szFile);
	BOOL Load09(FILE *fp);
	void Close();
	void SetName(char* szName);
	void SetPathToName(char* szFile);
	void ChangeInterval(unsigned short nNewInter, unsigned short nMethod);

public:
	SERIESESHEADER m_Header;

public:
	static DWORD GetVersion(char* szFile);
	static DWORD GetVersion(FILE *fp);
	static int GetName(char* szFile, char* szName, int nMax, unsigned short* pType = NULL);

protected:
	char m_szFile[MAX_PATH];
	FILE *m_pFile;

public:
	void SaveText(char* szFile, char* szFields);
};
