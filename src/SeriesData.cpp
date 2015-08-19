#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "StdAfx.h"
#include "include/SeriesData.h"

TFieldList::TFieldList()
{
	m_pFields = NULL;
	m_nCount = m_nSize = 0;
}

TFieldList::~TFieldList()
{
	Clear();
}

void TFieldList::Clear()
{
	if(m_pFields)
		free(m_pFields);
	m_pFields = NULL;
	m_nCount = 0;
	m_nSize = 0;
}

void TFieldList::MakeRoom()
{
	if((m_nCount + 1) >= m_nSize)
	{
		int nIndex, nSize = m_nSize + 10;

		m_pFields = (FIELDITEM*)realloc(m_pFields, sizeof(FIELDITEM) * nSize);
		for(nIndex = m_nCount; nIndex < nSize; nIndex++)
			memset(&m_pFields[nIndex], 0, sizeof(FIELDITEM));

		m_nSize = nSize;
	}
}

int TFieldList::Parsing(char* szFields)
{
	char* sz;
	char* szName;
	BOOL bName = FALSE;
	char szVal[100], *s;
	FIELDITEM* pItem;
/*
#ifdef _UNICODE
	char mbszFields[200];

	memset(mbszFields, 0, sizeof(mbszFields));
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, szFields, lstrlen(szFields), mbszFields, 200, NULL, NULL);
	sz = mbszFields;
#else
*/

	sz = szFields;
//#endif

	Clear();

	while(*sz)
	{
		if(*sz == '[')
		{
			MakeRoom();
			m_nCount++;
			pItem = GetItem(m_nCount - 1);
			szName = pItem->szNode;
			bName = TRUE;
		}
		else if(*sz == ']')
		{
			if(strlen(szVal) > 0)
			{
				pItem->nFieldNo[pItem->nCount] = atoi(szVal) - 1;
				pItem->nCount++;
			}
		}
		else if(*sz == ':')
		{
			bName = FALSE;
			s = szVal;
			memset(szVal, 0, sizeof(szVal));
		}
		else if(*sz == ',')
		{
			if(strlen(szVal) > 0)
			{
				pItem->nFieldNo[pItem->nCount] = atoi(szVal) - 1;
				pItem->nCount++;
			}
			memset(szVal, 0, sizeof(szVal));
			s = szVal;
		}
		else if(*sz == '*')
		{
			if(bName)
			{
			  //strcpy_s(pItem->szNode, "*");
			  strcpy(pItem->szNode, "*");
			}
			else
			{
				pItem->nFieldNo[0] = -1;
				while(*sz && *sz != ']')
					sz++;
				pItem->nCount++;
			}
		}
		else
		{
			if(bName)
			{
				*szName = *sz;
				szName++;
			}
			else
			{
				*s = *sz;
				s++;
			}
		}

		sz++;
	}

	return m_nCount;
}

TDate::TDate()
{
	nYear = nMonth = nDay = 0;
	nHour = nMinute = 0;
}

TDate::TDate(int nYear, int nMonth, int nDay, int nHour, int nMinute)
{
	this->nYear = nYear;
	this->nMonth = nMonth;
	this->nDay = nDay;
	this->nHour = nHour;
	this->nMinute = nMinute;
}

TDate::TDate(TIMEDATA& date)
{
	nYear = date.nYear;
	nMonth = date.nMonth;
	nDay = date.nDay;
	nHour = date.nHour;
	nMinute = date.nMinute;
}

void TDate::Increase(int nSpan)
{
	int nTempDay, nDays = GetDays(nYear, nMonth, nDay);
//	if(nSpan < 50000)
	{
		nMinute += nSpan % 60; nSpan /= 60;
		nHour += nSpan % 24; nSpan /= 24;
		nDays += nSpan;
	}
//	else
//	{
//	}
/*
	switch(nSpan)
	{
	case 0:
		nMin++;
		break;
	case 1:
		nHour++;
		break;
	case 2:
		nDay++;
		break;
	case 3:
		nMonth++;
		break;
	case 4:
		nYear++;
		break;
	}
*/

	if(nMinute >= 60)
	{
		nHour += nMinute / 60;
		nMinute %= 60;
	}

	if(nHour >= 24)
	{
		nDays += nHour / 24;
		nHour %= 24;
	}

	while(nDays > (nTempDay = GetYearDays(nYear)))
	{
		nYear++;
		nDays -= nTempDay;
	}

//	int nDays = GetMonthDays(nYear, nMonth);
	nMonth = 1;
	while(nDays > (nTempDay = GetMonthDays(nYear, nMonth)))
	{
		nMonth++;
		nDays -= nTempDay;
	}

	nDay = nDays;
//	if(nMonth > 12)
//	{
//		nYear++;
//		nMonth = 1;
//	}
}

unsigned long TDate::ToMinute(int nYear2, int nMonth2, int nDay2, int nHour2, int nMinute2)
{
	unsigned long nMin = 0;
	const int csnDayMinute = 24 * 60;
	int nIndex;

	for(nIndex = DATE_BASE_YEAR; nIndex < nYear2; nIndex++)
		nMin += GetYearDays(nIndex) * csnDayMinute;

	nMin += ((GetDays(nYear2, nMonth2, nDay2) - 1) * csnDayMinute);
	nMin += (nHour2 * 60);
	nMin += nMinute2;

	return nMin;
}

unsigned long TDate::GetMinutes(TIMEDATA& time)
{
	return ToMinute(time.nYear, time.nMonth, time.nDay, time.nHour, time.nMinute);
}

unsigned long TDate::GetMinutes()
{
	return ToMinute(nYear, nMonth, nDay, nHour, nMinute);
}

void TDate::SetDate(unsigned short nY, unsigned short nM, unsigned short nD, unsigned short nH, unsigned short nm)
{
	nYear = nY;
	nMonth = nM;
	nDay = nD;
	nHour = nH;
	nMinute = nm;
}

void TDate::SetDate(unsigned long nMinutes)
{
//	int nTempYear;
	unsigned int nYearDays, nMonthDays;

	nMinute = BYTE(nMinutes % 60); nMinutes /= 60;
	nHour = BYTE(nMinutes % 24); nMinutes /= 24;

	nYear = DATE_BASE_YEAR;
	while(nMinutes >= (nYearDays = GetYearDays(nYear)))
	{
		nMinutes -= nYearDays;
		nYear++;
	}

//	nYear = BYTE(nTempYear - DATE_BASE_YEAR);
	nMonth = 1;
	while(nMinutes >= (nMonthDays = GetMonthDays(nYear, nMonth)))
	{
		nMinutes -= nMonthDays;
		nMonth++;
	}

	nDay = BYTE(nMinutes + 1);
}

void TDate::operator =(unsigned long date)
{
	SetDate(date);
}

void TDate::operator =(TDate& date)
{
	nYear = date.nYear;
	nMonth = date.nMonth;
	nDay = date.nDay;
	nHour = date.nHour;
	nMinute = date.nMinute;
}

void TDate::operator +=(long span)
{
	unsigned long nTime = ToMinute(nYear, nMonth, nDay, nHour, nMinute);

	nTime += span;
	SetDate(nTime);
}

unsigned int TDate::GetYearDays(int nYear)
{
	int nDays = 365;

	if((nYear % 4) == 0)
	{
		if((nYear % 100) == 0)
		{
			if((nYear % 400) == 0)
				nDays = 366;
			else
				nDays = 365;
		}
		else
			nDays = 366;
	}

	return nDays;
}

unsigned int TDate::GetMonthDays(int nYear, int nMonth)
{
	int nMonthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	if(nMonth == 2)
	{
		if(GetYearDays(nYear) == 366)
			return 29;
		else
			return 28;
	}

	return nMonthDays[nMonth - 1];
}

unsigned int TDate::GetDays(int nYear, int nMonth, int nDay)
{
	int nMonthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int nDays = 0;
	int nIndex;

	if(GetYearDays(nYear) == 366)
		nMonthDays[1] = 29;

	for(nIndex = 1; nIndex < nMonth; nIndex++)
		nDays += nMonthDays[nIndex - 1];
	nDays += nDay;

	return nDays;
}

unsigned long TDate::GetYearMinutes(int nYear)
{
	return GetYearDays(nYear) * 1440;
}

unsigned long TDate::GetMonthMinutes(int nYear, int nMonth)
{
	int nMonthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	if(nMonth == 2 && GetYearDays(nYear) == 366)
		return 29 * 1440;

	return nMonthDays[nMonth - 1] * 1440;
}

TSeriesItem::TSeriesItem(void)
{
	m_pParent = NULL;
	memset(&m_Header, 0, sizeof(m_Header));
	m_pDate = NULL;
	m_nYear = 1900.0f;
	m_nFirst = 0L;
	m_nCurrent = 0L;
}

TSeriesItem::~TSeriesItem(void)
{
}

BOOL TSeriesItem::Save(FILE* pFile)
{
	m_Header.nCount = GetCount();
//	fwrite(&m_Header, sizeof(SERIESDUMP), 1, pFile);
	fwrite(&m_Header, sizeof(SERIESITEMHEADER), 1, pFile);
	if(m_Header.nData == FT_DATE)
		fwrite(m_Date.m_pArray, sizeof(unsigned long) * GetCount(), 1, pFile);
	else
		fwrite(m_pArray, sizeof(DATATYPE) * GetCount(), 1, pFile);

	return TRUE;
}

BOOL TSeriesItem::Load(FILE* pFile)
{
	fread(&m_Header, sizeof(SERIESITEMHEADER), 1, pFile);
	m_dtStart = m_Header.date;
	if(SetSize(m_Header.nCount))
	{
		if(m_Header.nData == FT_DATE)
		{
			m_Date.SetSize(m_Header.nCount);
			fread(m_Date.m_pArray, sizeof(unsigned long) * m_Header.nCount, 1, pFile);
			m_Date.m_nCount = m_Header.nCount;
		}
		else
			fread(m_pArray, sizeof(DATATYPE) * m_Header.nCount, 1, pFile);
		m_nCount = m_Header.nCount;
	}
	else
		m_nCount = 0;

	return TRUE;
}

BOOL TSeriesItem::Load10(FILE* pFile)
{
	fread(&m_Header, sizeof(SERIESITEMHEADER), 1, pFile);
	m_dtStart = m_Header.date;
	if(SetSize(m_Header.nCount))
	{
		fread(m_pArray, sizeof(DATATYPE) * m_Header.nCount, 1, pFile);
		m_Date.SetSize(m_Header.nCount);
		for(int i = 0; i < (int)m_Header.nCount; i++)
			m_Date.m_pArray[i] = m_pArray[i];
		m_Date.m_nCount = m_Header.nCount;
		m_nCount = m_Header.nCount;
	}
	else
		m_nCount = 0;

	return TRUE;
}

BOOL TSeriesItem::Load09(FILE *fp)
{
	SERIESDUMP dump;
	BOOL bRet = TRUE;

//	fread(&m_Header, sizeof(SERIESDUMP), 1, fp);
	fread(&dump, sizeof(SERIESDUMP), 1, fp);
	memcpy(&m_Header, &dump, sizeof(m_Header));
	m_dtStart = m_Header.date;
	if(SetSize(m_Header.nCount))
	{
		fread(m_pArray, sizeof(DATATYPE) * m_Header.nCount, 1, fp);
		m_nCount = m_Header.nCount;
	}
	else
	{
		m_nCount = 0;
		bRet = FALSE;
	}

	return bRet;
}

void TSeriesItem::SetValue(int nIndex, DATATYPE val)
{
	m_pArray[nIndex] = val;
	if(nIndex >= m_nCount)
		m_nCount = nIndex + 1;
}

void TSeriesItem::SetDate(int nIndex, unsigned long nDate)
{
	m_Date.m_pArray[nIndex] = nDate;
	if(nIndex >= m_Date.m_nCount)
		m_Date.m_nCount = nIndex + 1;
}

int TSeriesItem::SetFirstPos(unsigned long nTime)
{
	unsigned long nFirst = m_dtStart.GetMinutes();

	if(nTime == 0)
		m_nCurrent = -1;
	else if(m_Header.nInterval < TI_MONTH)
	{
		m_nCurrent = (nTime - nFirst) / m_Header.nInterval - 1;
	}
	else if(m_Header.nInterval == TI_MONTH)
	{
	}
	else if(m_Header.nInterval == TI_YEAR)
	{
	}

	return m_nCurrent;
}

int TSeriesItem::SetFirstPos(TIMEDATA& time)
{
	if(time.nYear == 0 && time.nMonth == 0 && time.nDay == 0 && time.nHour == 0 && time.nMinute == 0)
		m_nCurrent = 0;
	else
	{
		unsigned long nTime = TDate::ToMinute(time.nYear, time.nMonth, time.nDay, time.nHour, time.nMinute);
		SetFirstPos(nTime);
	}

	return m_nCurrent;
}

DATATYPE TSeriesItem::GetNext()
{
	return (m_nCurrent < m_nCount - 1 ? GetValue(++m_nCurrent) : 0);
}

DATATYPE TSeriesItem::GetCurVal()
{
	return GetValue(m_nCurrent);
}

unsigned long TSeriesItem::GetNextDate()
{
	return (m_nCurrent < m_Date.GetCount() - 1 ? GetDate(++m_nCurrent) : 0);
}

unsigned long TSeriesItem::GetCurDate()
{
	return GetDate(m_nCurrent);
}

unsigned long TSeriesItem::CopyPart(unsigned long nStart, unsigned long nEnd, TSeriesItem *pNew)
{
	unsigned long nNewEnd, nCurrent = m_Header.date;
	unsigned long nIndex, nCount = GetCount();

	pNew->Clear();

	//memcpy_s(&pNew->m_Header, sizeof(SERIESITEMHEADER), &m_Header, sizeof(SERIESITEMHEADER));
	memcpy(&pNew->m_Header, &m_Header, sizeof(SERIESITEMHEADER));
	pNew->m_Header.date = nStart;

	nNewEnd = m_Header.date + nCount * m_Header.nInterval;
	if(nNewEnd > nEnd)
		nNewEnd = nEnd;

	nCount = (nNewEnd - nStart) / m_Header.nInterval + 1;
	pNew->SetSize(nCount);

	for(nIndex = 0; nIndex < (unsigned long)GetCount(); nIndex++)
	{
		if(nCurrent >= nStart && nCurrent <= nNewEnd)
			pNew->Add(GetValue(nIndex));
		nCurrent += m_Header.nInterval;
	}

	pNew->m_Header.nCount = pNew->GetCount();
	pNew->m_Header.date = nStart;
	pNew->m_dtStart = nStart;

	return pNew->GetCount();
}

void TSeriesItem::Sort(int nMethod)
{
	int i, j;

	if(nMethod == 0)
	{
		for(i = m_nCount - 1; i > 0; i--)
		{
			bool bSwap = false;
			for(j = 0; j < i; j++)
			{
				if(m_pArray[j] < m_pArray[j + 1])
				{
					Swap(j, j+1);
					bSwap = true;
				}
			}

			if(!bSwap)
				break;
		}
	}
	else
	{
		for(i = m_nCount - 1; i > 0; i--)
		{
			bool bSwap = false;
			for(j = 0; j < i; j++)
			{
				if(m_pArray[j] > m_pArray[j + 1])
				{
					Swap(j, j+1);
					bSwap = true;
				}
			}

			if(!bSwap)
				break;
		}
	}
}

void TSeriesItem::CalcAnnualStatic(int nYear)
{
	TDate date;
	int nIndex;

	date.SetDate(m_Header.date);
	m_nYear = 0;
	memset(m_nMonth, 0, sizeof(m_nMonth));

	for(nIndex = 0; nIndex < m_nCount; nIndex++)
	{
		if(date.GetYear() == nYear)
		{
			break;
		}

		date.Increase(m_Header.nInterval);
	}

	for(; nIndex < m_nCount; nIndex++)
	{
		if(date.GetYear() != nYear)
			break;

		m_nYear += GetValue(nIndex);
		m_nMonth[date.GetMonth() - 1] += GetValue(nIndex);

		date.Increase(m_Header.nInterval);
	}
}

TSeries::TSeries()
{
	m_pParent = NULL;
	memset(&m_Header, 0, sizeof(m_Header));
}

TSeries::~TSeries()
{
}

/*
BOOL TSeries::Load(LPCTSTR szFile)
{
	FILE *fp;

	Clear();

#ifdef _UNICODE
	if(_wfopen_s(&fp, szFile, L"rb") != 0)
		return FALSE;
#else
	if(fopen_s(&fp, szFile, "wb") != 0)
		return FALSE;
#endif

	Load(fp);

	fclose(fp);

//	return (m_Header.nMagic == FILEMAGICNO);
	return TRUE;
}
*/

BOOL TSeries::Load09(FILE *fp)
{
	SERIESHEADEROLD header;
	BOOL bRet = FALSE;

	fread(&header, sizeof(header), 1, fp);

	if(header.nMagic == FILEMAGICNO)
	{
		m_Header.nData = header.nFile;
		m_Header.nInterval = header.nInterval;
		m_Header.nTime = header.date;
		m_Header.nCount = header.nCount;
		memcpy(m_Header.szDescript, header.szDescript, sizeof(header.szDescript));
		m_Header.nColumn = header.nColumn;
		memset(m_Header.Reserved, 0, sizeof(m_Header.Reserved));

		m_dtStart = header.date;

		for(int nSeries = 0; nSeries < m_Header.nColumn; nSeries++)
		{
			TSeriesItem *pSeries = new TSeriesItem;
			pSeries->Load09(fp);
			pSeries->m_pParent = this;
			Add(pSeries);
		}

		bRet = TRUE;
	}
	else
		memset(&m_Header, 0, sizeof(m_Header));

	return bRet;
}

BOOL TSeries::Load(FILE *fp)
{
	fread(&m_Header, sizeof(m_Header), 1, fp);

	for(int nSeries = 0; nSeries < m_Header.nColumn; nSeries++)
	{
		TSeriesItem *pSeries = new TSeriesItem;
		pSeries->Load(fp);
		pSeries->m_pParent = this;
		Add(pSeries);
	}

	return TRUE;
}

BOOL TSeries::Load10(FILE *fp)
{
	fread(&m_Header, sizeof(m_Header), 1, fp);

	for(int nSeries = 0; nSeries < m_Header.nColumn; nSeries++)
	{
		TSeriesItem *pSeries = new TSeriesItem;
		pSeries->Load10(fp);
		pSeries->m_pParent = this;
		Add(pSeries);
	}

	return TRUE;
}

BOOL TSeries::LoadHeader(char* szFile)
{
	FILE *fp;

	Clear();

/*
#ifdef _UNICODE
	if(_wfopen_s(&fp, szFile, L"rb") != 0)
		return FALSE;
#else
*/
	if(fopen_s(&fp, szFile, "wb") != 0)
		return FALSE;
//#endif

	fread(&m_Header, sizeof(m_Header), 1, fp);

	fclose(fp);

	return TRUE;
}

/*
BOOL TSeries::Save(LPCTSTR szFile)
{
	FILE *fp;

#ifdef _UNICODE
	if(_wfopen_s(&fp, szFile, L"wb") != 0)
		return -1;
#else
	if(fopen_s(&fp, szFile, "rb") != 0)
		return -1;
#endif

	Save(fp);

	fclose(fp);

	return TRUE;
}
*/

BOOL TSeries::Save(FILE *fp)
{
	m_Header.nColumn = GetCount();
	if(m_Header.nColumn > 0)
		m_Header.nCount = GetAt(0)->GetCount();
	fwrite(&m_Header, sizeof(m_Header), 1, fp);
	for(int nSeries = 0; nSeries < m_Header.nColumn; nSeries++)
		GetAt(nSeries)->Save(fp);

	return TRUE;
}

BOOL TSeries::SaveText(FILE *fp, int nCount, int nFieldNos[])
{
	int nIndex, nCol;
	int nFields[30];
	TDate date;

	if(nFieldNos[0] == -1)
	{
		nCount = GetCount();
		for(nIndex = 0; nIndex < nCount; nIndex++)
			nFields[nIndex] = nIndex;
	}
	else
		memcpy(nFields, nFieldNos, sizeof(int) * nCount);

	fprintf_s(fp, "%s\n", m_Header.szDescript);
	fprintf_s(fp, "TIME            \t");
	for(nCol = 0; nCol < nCount - 1; nCol++)
	{
		fprintf_s(fp, "%s\t", GetSeries(nFields[nCol])->m_Header.szHeader);
	}
	fprintf_s(fp, "%s\n", GetSeries(nFields[nCount - 1])->m_Header.szHeader);

	//int nRows = GetSeries(0)->GetCount();
	date = m_Header.nTime;

	for(nIndex = 0; nIndex < GetSeries(0)->GetCount(); nIndex++)
	{
		fprintf_s(fp, "%04d/%02d/%02d %02d:%02d\t", date.GetYear(), date.GetMonth(), date.GetDay(), date.GetHour(), date.GetMinute());

		for(nCol = 0; nCol < nCount - 1; nCol++)
		{
			fprintf_s(fp, "%.3f\t", GetSeries(nFields[nCol])->GetValue(nIndex));
		}
		fprintf_s(fp, "%.3f\n", GetSeries(nFields[nCol])->GetValue(nIndex));

		date += m_Header.nInterval;
	}
	fprintf(fp, "\n");

	return TRUE;
}

DATATYPE TSeries::GetValue(int nCol, int nRow)
{
	return GetAt(nCol)->GetValue(nRow);
}

int TSeriesItem::ConvertInterval(unsigned short nNewInter, unsigned short nMethod, TSeriesItem* pNew)
{
	int nIndex, nCount;
	float nVal;

	memcpy(&(pNew->m_Header), &m_Header, sizeof(m_Header));
	pNew->Clear();

	nCount = GetCount();
	if(m_Header.nInterval > nNewInter)
	{
		if(m_Header.nInterval == TI_MONTH)
		{
			TDate date;

			date.SetDate(m_Header.date);

			for(nIndex = 0; nIndex < nCount; nIndex++)
			{
				int nDays = date.GetMonthDays(date.GetYear(), date.GetMonth());
				int nMinutes = nDays * 1440;
				int nStep = nMinutes / nNewInter;

				nVal = GetAt(nIndex) / nStep;
				for(int nStep2 = 0; nStep2 < nStep; nStep2++)
					pNew->Add(nVal);

				date.AddMonth(1);
			}
		}
		else
		{
			int nStep = m_Header.nInterval / nNewInter;

			for(nIndex = 0; nIndex < nCount; nIndex++)
			{
				nVal = GetAt(nIndex) / nStep;

				for(int nStep2 = 0; nStep2 < nStep; nStep2++)
					pNew->Add(nVal);
			}
		}
	}
	else if(nNewInter == m_Header.nInterval)
	{
		pNew->SetSize(m_nSize);
		memcpy(pNew->m_pArray, m_pArray, sizeof(float) * m_nSize);
		pNew->m_nCount = m_nCount;
	}
	else
	{
		float nMin = 999999.0f, nMax = -999999.0f, nSum = 0.0f;
		int nStep = nNewInter / m_Header.nInterval;
		int nPos = 0;

		for(nIndex = 0; nIndex < nCount; nIndex++)
		{
			nVal = GetAt(nIndex);
			if(nMin > nVal) nMin = nVal;
			if(nMax < nVal) nMax = nVal;
			nSum += nVal;

			nPos++;
			if(nPos == nStep)
			{
				switch(nMethod)
				{
				case TC_SUM:
					pNew->Add(nSum);
					break;
				case TC_AVG:
					pNew->Add(nSum / nStep);
					break;
				case TC_MIN:
					pNew->Add(nMin);
					break;
				case TC_MAX:
					pNew->Add(nMax);
				}

				nPos = 0;
				nMin = 999999.0f;
				nMax = -nMin;
				nSum = 0.0f;
			}
		}
	}

	pNew->m_Header.nInterval = nNewInter;
	pNew->m_dtStart = pNew->m_Header.date;

	return pNew->GetCount();
}

TSeriesItem* TSeries::FindSeries(unsigned short nType)
{
	TSeriesItem *pItem;
	int nSeries, nSerieses = GetCount();

	for(nSeries = 0; nSeries < nSerieses; nSeries++)
	{
		pItem = GetAt(nSeries);
		if(pItem->GetDataType() == nType)
			return pItem;
	}

	return NULL;
}

int TSeries::RemoveSeries(unsigned short nType)
{
	TSeriesItem *pItem = FindSeries(nType);

	if(pItem)
	{
		Remove(pItem);
		return 1;
	}

	return 0;
}

void TDate::IncreaseTime(BYTE nStep)
{
	switch(nStep)
	{
	case 0:
		nMinute++;
		break;
	case 1:
		nHour++;
		break;
	case 2:
		nDay++;
		break;
	case 3:
		nMonth++;
		break;
	case 4:
		nYear++;
		break;
	}

	if(nMinute >= 60)
	{
		nHour += (nMinute / 60);
		nMinute %= 60;
	}

	if(nHour >= 24)
	{
		nDay += (nHour / 24);
		nHour %= 24;
	}

	int nDays = GetMonthDays(nYear, nMonth);
	if(nDay > nDays)
	{
		nMonth++;
		nDay = 1;
	}

	if(nMonth > 12)
	{
		nYear++;
		nMonth = 1;
	}
}

void TDate::AddMonth(int nMonths)
{
	nMonth += nMonths;
	if(nMonth > 12)
	{
		nYear += nMonth / 12;
		nMonth = nMonth % 12;
	}

	while(nMonth < 0)
	{
		nYear--;
		nMonth += 12;
	}
}

void TDate::AddHour(int nHours)
{
	nHour += nHours;

	if(nHour >= 24 || nHour < 0)
	{
		nDay += (nHour / 24);
		nHour %= 24;
	}

	int nDays = GetMonthDays(nYear, nMonth);
	if(nDay > nDays)
	{
		nMonth++;
		nDay = 1;
	}

	if(nMonth > 12)
	{
		nYear++;
		nMonth = 1;
	}
}

void TDate::AddMinute(int nMinutes)
{
	nMinute += nMinutes;

	if(nMinute >= 60 || nMinute < 0)
	{
		nHour += (nMinute / 60);
		nMinute %= 60;
	}

	if(nHour >= 24 || nHour < 0)
	{
		nDay += (nHour / 24);
		nHour %= 24;
	}

	int nDays = GetMonthDays(nYear, nMonth);
	if(nDay > nDays)
	{
		nMonth++;
		nDay = 1;
	}

	if(nMonth > 12)
	{
		nYear++;
		nMonth = 1;
	}
}

void TDate::AddYear(int nYears)
{
	nYear += nYears;
}

TSeriesItem* TSeries::AddItem(void)
{
	TSeriesItem *pItem = new TSeriesItem;
	pItem->m_Header.date = m_dtStart.GetMinutes();
	pItem->m_Header.nInterval = m_Header.nInterval;
	pItem->m_pParent = this;
	Add(pItem);
	return pItem;
}

int TSeries::SetFirstPos(unsigned long nTime)
{
	int nIndex, nRet = 0;

	for(nIndex = 0; nIndex < GetCount(); nIndex++)
		nRet = GetAt(nIndex)->SetFirstPos(nTime);

	return nRet;
}

BOOL TSeries::MoveNext()
{
	int nIndex;

	for(nIndex = 0; nIndex < GetCount(); nIndex++)
		GetAt(nIndex)->GetNext();

	return TRUE;
}

int TSerieses::GetName(char* szFile, char* szName, int nMax, unsigned short *pType)
{
	FILE *fp;

/*
#ifdef _UNICODE
	if(_wfopen_s(&fp, szFile, L"rb") != 0)
		return -1;
#else
*/
	if(fopen_s(&fp, szFile, "wb") != 0)
		return -1;
//#endif

	if(GetVersion(szFile) == FILEVERSION09)
	{
		SERIESHEADEROLD header;
		fread(&header, sizeof(header), 1, fp);

/*
#ifdef _UNICODE
		memset(szName, 0, sizeof(TCHAR) * nMax);
		MultiByteToWideChar(CP_ACP, 0, header.szDescript, (int)strlen(header.szDescript), szName, nMax);
#else
*/
		strcpy_s(szName, strlen(header.szDescript), header.szDescript);
//#endif

		if(pType)
			*pType = header.nFile;
	}
	else
	{
		SERIESESHEADER header;
		fread(&header, sizeof(header), 1, fp);

#ifdef _UNICODE
		memset(szName, 0, sizeof(TCHAR) * nMax);
		MultiByteToWideChar(CP_ACP, 0, header.szDescript, (int)strlen(header.szDescript), szName, nMax);
#else
		strcpy_s(szName, strlen(header.szDescript), header.szDescript);
#endif

		if(pType)
		{
			SERIESHEADER header2;

			fread(&header2, sizeof(header2), 1, fp);
			*pType = header2.nData;
		}
	}


	fclose(fp);



	return 0;
}

void TSeries::SetName(char* szName)
{
/*
#ifdef _UNICODE
	char sName[100];

	memset(sName, 0, sizeof(sName));
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, szName, lstrlen(szName), sName, 100, NULL, NULL);
	strcpy_s(m_Header.szDescript, 50, sName);
#else
*/
	strcpy_s(m_Header.szDescript, 50, szName);
//#endif
}

int TSeriesItem::Copy(TSeriesItem* pCopy)
{
	pCopy->m_dtStart = m_dtStart;
	memcpy(&(pCopy->m_Header), &m_Header, sizeof(SERIESITEMHEADER));
	pCopy->m_nCurrent = m_nCurrent;
	pCopy->m_nFirst = m_nFirst;
//	lstrcpy(pCopy->m_szHeader, m_szHeader);
	pCopy->SetSize(m_nSize);
	memcpy(pCopy->m_pArray, m_pArray, sizeof(DATATYPE) * m_nSize);
	pCopy->m_nCount = m_nCount;

	return 0;
}

int TSeries::Copy(TSeries* pCopy)
{
	int nIndex, nCount = GetCount();

	pCopy->m_dtStart = m_dtStart;
	memcpy(&(pCopy->m_Header), &m_Header, sizeof(SERIESHEADER));

	for(nIndex = 0; nIndex < nCount; nIndex++)
	{
		TSeriesItem *pNew = new TSeriesItem;
		GetAt(nIndex)->Copy(pNew);
		pCopy->Add(pNew);
	}

	return 0;
}

void TSeries::CalcAnnualStatic(int nYear)
{
	int nIndex;

	for(nIndex = 0; nIndex < m_nCount; nIndex++)
		GetSeries(nIndex)->CalcAnnualStatic(nYear);
}

int TSeriesItem::ConvertInterval(unsigned short nNewInter, unsigned short nMethod, BOOL bMul)
{
	int nIndex, nCount, nNewCount;
	float nVal;
	DATATYPE *pNewData = NULL;

	nCount = GetCount();

	if(nNewInter == m_Header.nInterval)
		return nCount;

	if(m_Header.nInterval > nNewInter)
	{
		if(m_Header.nInterval == TI_MONTH)
		{
			TDate date;
			unsigned long nS, nE;

			date.SetDate(m_Header.date);
			nS = date.GetMinutes();
			date.AddMonth(nCount);
			nE = date.GetMinutes();

			nNewCount = (nE - nS + 1) / nNewInter;
		}
		else
			nNewCount = nCount * m_Header.nInterval / nNewInter;

		pNewData = new DATATYPE[nNewCount];
		DATATYPE *pData = pNewData;

		int nStep = m_Header.nInterval / nNewInter;

		if(GetDataType() == FT_DATE)
		{
		}
		else
		{
			if(bMul)
			{
				if(m_Header.nInterval == TI_MONTH)
				{
					TDate date;

					date.SetDate(m_Header.date);

					for(nIndex = 0; nIndex < nCount; nIndex++)
					{
						int nDays = date.GetMonthDays(date.GetYear(), date.GetMonth());
						int nMinutes = nDays * 1440;
						nStep = nMinutes / nNewInter;

						nVal = GetAt(nIndex) / nStep;

						for(int nStep2 = 0; nStep2 < nStep; nStep2++)
							*(pData++) = nVal;

						date.AddMonth(1);
					}
				}
				else
				{
					for(nIndex = 0; nIndex < nCount; nIndex++)
					{
						nVal = GetAt(nIndex) / nStep;

						for(int nStep2 = 0; nStep2 < nStep; nStep2++)
							*(pData++) = nVal;
					}
				}
			}
			else
			{
				for(nIndex = 0; nIndex < nCount; nIndex++)
				{
					for(int nStep2 = 0; nStep2 < nStep; nStep2++)
						*(pData++) = GetAt(nIndex);
				}
			}
		}
	}
	else
	{
		float nMin = 9E10f, nMax = -999999.0f, nSum = 0.0f;
		int nStep = nNewInter / m_Header.nInterval;
		int nPos = 0;

//		nNewCount= nCount * nNewInter / m_Header.nInterval;
		nNewCount = nCount * m_Header.nInterval / nNewInter;
		pNewData = new DATATYPE[nNewCount];
		DATATYPE *pData = pNewData;

		for(nIndex = 0; nIndex < nCount; nIndex++)
		{
			nVal = GetAt(nIndex);
			if(nMin > nVal) nMin = nVal;
			if(nMax < nVal) nMax = nVal;
			nSum += nVal;

			nPos++;
			if(nPos == nStep)
			{
				switch(nMethod)
				{
				case TC_SUM:
					*(pData++) = nSum;
					break;
				case TC_AVG:
					*(pData++) = nSum / nStep;
					break;
				case TC_MIN:
					*(pData++) = nMin;
					break;
				case TC_MAX:
					*(pData++) = nMax;
				}

				nPos = 0;
				nMin = 9E10f;
				nMax = -nMin;
				nSum = 0.0f;
			}
		}
	}

	if(pNewData)
	{
		m_Header.nInterval = nNewInter;
		m_nCount = m_nSize = nNewCount;
		delete [] m_pArray;
		m_pArray = pNewData;

		return GetCount();
	}

	return 0;
}

void TSeries::ChangeInterval(unsigned short nNewInter, unsigned short nMethod)
{
	int nIndex, nCount = GetCount();

	for(nIndex = 0; nIndex < nCount; nIndex++)
	{
		TSeriesItem *pItem = GetAt(nIndex);

		if(pItem->GetDataType() == FT_DATE)
			pItem->ConvertInterval(nNewInter, TC_MIN);
		else
			pItem->ConvertInterval(nNewInter, nMethod);
	}

	m_Header.nCount = m_Header.nCount * m_Header.nInterval / nNewInter;
	m_Header.nInterval = nNewInter;
}

void TSeries::ChangeIntervalByDefault(unsigned short nNewInter)
{
	int nIndex, nCount = GetCount();

	if(m_Header.nInterval == nNewInter)
		return;

	for(nIndex = 0; nIndex < nCount; nIndex++)
	{
		TSeriesItem *pItem = GetAt(nIndex);

		switch(pItem->m_Header.nData)
		{
		case FT_PRECIP:
		case FT_EVA:
		case FT_SOLAR:
		case FT_EVACALC:
			pItem->ConvertInterval(nNewInter, TC_SUM);
			break;
		case FT_RHUMI:
		case FT_TEMPAVG:
			pItem->ConvertInterval(nNewInter, TC_AVG, FALSE);
			break;
		case FT_WIND:
			pItem->ConvertInterval(nNewInter, TC_SUM, FALSE);
			break;
		case FT_TEMPMIN:
			pItem->ConvertInterval(nNewInter, TC_MIN, FALSE);
			break;
		case FT_TEMPMAX:
			pItem->ConvertInterval(nNewInter, TC_MAX, FALSE);
			break;
		case FT_DATE:
			pItem->ConvertInterval(nNewInter, TC_MIN, FALSE);
			break;
		default:
			pItem->ConvertInterval(nNewInter, TC_SUM);
			break;
		}
	}

	m_Header.nCount = m_Header.nCount * m_Header.nInterval / nNewInter;
	m_Header.nInterval = nNewInter;
}

int TSeries::AddSeries(TSeriesItem* pItem)
{
	if(pItem == NULL)
		return -1;

	if(pItem->m_Header.nInterval != m_Header.nInterval)
		return -2;

	if(GetCount() > 0 && GetAt(0)->GetCount() != pItem->GetCount())
	{
		for(int nIndex = pItem->GetCount(); nIndex < GetAt(0)->GetCount(); nIndex++)
			pItem->Add(0);
//		return -3;
	}

	Add(pItem);
	m_Header.nData |= pItem->m_Header.nData;

	return 0;
}

void TSeriesItem::AddValue(int nIndex, float nVal)
{
	SetValue(nIndex, GetValue(nIndex) + nVal);
}

TSerieses::TSerieses()
{
	memset(m_szFile, 0, sizeof(m_szFile));
	memset(&m_Header, 0, sizeof(m_Header));
	m_pFile = NULL;
}

TSerieses::~TSerieses()
{
	if(m_pFile)
		Close();
}

int TSerieses::Load(char* szFile)
{
	FILE *fp;
	int nIndex;
	int nRet = 0;
	DWORD dwVersion;

#ifdef _UNICODE
	if(_wfopen_s(&fp, szFile, L"rb") != 0)
		return 1;
#else
	if(fopen_s(&fp, szFile, "rb") != 0)
		return 1;
#endif

	lstrcpy(m_szFile, szFile);

	Clear();

	dwVersion = GetVersion(fp);

	if(dwVersion == FILEVERSION09)
	{
		if(Load09(fp))
		{
			nRet = 9;
			SetPathToName(szFile);
		}
		else
			nRet = 1;
	}
	else if(dwVersion == FILEVERSION10)
	{
		fread(&m_Header, sizeof(m_Header), 1, fp);

		for(nIndex = 0; nIndex < m_Header.nSeries; nIndex++)
		{
			TSeries *pSeries = new TSeries;
			pSeries->Load10(fp);
			pSeries->m_dtStart = m_Header.nTime;
			pSeries->m_pParent = this;
			Add(pSeries);
		}
	}
	else
	{
		fread(&m_Header, sizeof(m_Header), 1, fp);

		for(nIndex = 0; nIndex < m_Header.nSeries; nIndex++)
		{
			TSeries *pSeries = new TSeries;
			pSeries->Load(fp);
			pSeries->m_dtStart = m_Header.nTime;
			pSeries->m_pParent = this;
			Add(pSeries);
		}
	}

	fclose(fp);

	return nRet;
}

BOOL TSerieses::Save(char* szFile)
{
	FILE *fp;
	errno_t e;
	int nIndex;

#ifdef _UNICODE
	if(szFile)
		e = _wfopen_s(&fp, szFile, L"wb");
	else
		e = _wfopen_s(&fp, m_szFile, L"wb");
#else
	if(szFile)
		e = fopen_s(&fp, szFile, "wb");
	else
		e = fopen_s(&fp, m_szFile, "wb");
#endif

	if(e != 0)
		return FALSE;

	m_Header.dwVersion = FILECURVERSION;
	m_Header.nSeries = GetCount();
	m_Header.nMagic = FILEMAGICNO;

	fwrite(&m_Header, sizeof(m_Header), 1, fp);

	for(nIndex = 0; nIndex < m_Header.nSeries; nIndex++)
	{
		TSeries *pSeries = GetAt(nIndex);

		pSeries->Save(fp);
	}

	fclose(fp);

	return TRUE;
}

BOOL TSerieses::Open(char* szFile)
{
	FILE *fp;
	//int nRet = 0;

#ifdef _UNICODE
	if(_wfopen_s(&fp, szFile, L"rb") != 0)
		return 1;
#else
	if(fopen_s(&fp, szFile, "rb") != 0)
		return 1;
#endif

	lstrcpy(m_szFile, szFile);

	Clear();

	if(GetVersion(fp) != FILECURVERSION)
	{
		fclose(m_pFile);
		m_pFile = NULL;
		return FALSE;
	}
	else
	{
		fread(&m_Header, sizeof(m_Header), 1, fp);
	}

	return TRUE;
}

void TSerieses::Close()
{
	if(m_pFile)
		fclose(m_pFile);
	m_pFile = NULL;
}

DWORD TSerieses::GetVersion(char* szFile)
{
	FILE *fp;
	DWORD dwVersion = 0;
	errno_t e;

#ifdef _UNICODE
	e = _wfopen_s(&fp, szFile, L"rb");
#else
	e = fopen_s(&fp, szFile, "rb");
#endif

	if(e != 0)
		return 0;

	dwVersion = GetVersion(fp);

	fclose(fp);

	return dwVersion;
}

DWORD TSerieses::GetVersion(FILE *fp)
{
	DWORD dwVersion = 0;
	SERIESHEADEROLD old;
	SERIESESHEADER header;

	fseek(fp, 0, SEEK_SET);
	fread(&header, sizeof(header), 1, fp);
	if(header.nMagic != FILEMAGICNO)
		return 0xffff;

	if(header.dwVersion == FILECURVERSION)
		dwVersion = FILECURVERSION;
	else
	{
		fseek(fp, 0, SEEK_SET);
		fread(&old, sizeof(old), 1, fp);
		if(old.dwVersion == FILEVERSION09)
			dwVersion = old.dwVersion;
		else
			dwVersion = header.dwVersion;
	}

	fseek(fp, 0, SEEK_SET);

	return dwVersion;
}

BOOL TSerieses::Load09(char* szFile)
{
	FILE *fp;
	errno_t e;

#ifdef _UNICODE
	e = _wfopen_s(&fp, szFile, L"rb");
#else
	e = fopen_s(&fp, szFile, "rb");
#endif

	if(e != 0)
		return FALSE;

	Load09(fp);
	SetPathToName(szFile);

	fclose(fp);

	return TRUE;
}

BOOL TSerieses::Load09(FILE *fp)
{
	TSeries *pSeries = new TSeries;
	BOOL bRet = FALSE;

	if(pSeries->Load09(fp))
	{
		m_Header.dwVersion = FILECURVERSION;
		m_Header.nMagic = FILEMAGICNO;
		m_Header.nTime = pSeries->m_dtStart.GetMinutes();
		m_Header.nSeries = 1;
		m_Header.nInterval = pSeries->m_Header.nInterval;
		m_Header.nPartNo = 1;
		m_Header.nPartTotal = 1;
		memset(m_Header.szDescript, 0, sizeof(m_Header.szDescript));
		pSeries->m_pParent = this;
		Add(pSeries);
		bRet = TRUE;
	}
	else
		delete pSeries;

	return bRet;
}

void TSerieses::SetName(char* szName)
{
#ifdef _UNICODE
	char sName[100];

	memset(sName, 0, sizeof(sName));
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, szName, lstrlen(szName), sName, 100, NULL, NULL);
	strcpy_s(m_Header.szName, 50, sName);
#else
	strcpy_s(m_Header.szName, 50, szName);
#endif
}

void TSerieses::SetPathToName(char* szFile)
{
#ifdef _UNICODE
	char szName[100];
	wchar_t *cFind;

	memset(szName, 0, sizeof(szName));
	cFind = (wchar_t*)wcsrchr(szFile, L'\\');
	if(cFind)
	{
		cFind++;
		WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, cFind, lstrlen(cFind), szName, 100, NULL, NULL);
		strcpy_s(m_Header.szName, 50, szName);
	}
#else
	char *cFind;
	cFind = strrchr(szFile, '\\');
	if(cFind)
		cFind++;
	strcpy_s(m_Header.szName, strlen(m_Header.szName), cFind);
#endif
}

void TSerieses::ChangeInterval(unsigned short nNewInter, unsigned short nMethod)
{
	int nIndex, nCount = GetCount();

	for(nIndex = 0; nIndex < nCount; nIndex++)
	{
		GetAt(nIndex)->ChangeInterval(nNewInter, nMethod);
	}

	m_Header.nInterval = nNewInter;
}

void TSerieses::SaveText(char* szFile, char* szFields)
{
	TFieldList list;
	FILE *fp;

	list.Parsing(szFields);

#ifdef _UNICODE
	_wfopen_s(&fp, szFile, L"wt");
#else
	fopen_s(&fp, szFile, "wt");
#endif

	for(int nIndex = 0; nIndex < list.GetCount(); nIndex++)
	{
		FIELDITEM *pItem = list.GetItem(nIndex);

		for(int nSeries = 0; nSeries < GetCount(); nSeries++)
		{
			TSeries *pSeries = GetAt(nSeries);

			if(strcmp(pItem->szNode, "*") == 0 || strcmp(pSeries->m_Header.szDescript, pItem->szNode) == 0)
			{
				pSeries->SaveText(fp, pItem->nCount, pItem->nFieldNo);
			}
		}
	}

	fclose(fp);
}
