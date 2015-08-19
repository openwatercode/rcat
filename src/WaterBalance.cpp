#include "StdAfx.h"
#include "include/SeriesData.h"
#include "include/WaterBalance.h"

TNodeYear::TNodeYear(void)
{
//	Init();
	m_nCount = 0;
	m_nET_Per = 0.0;
	m_nYear = 1900;
	m_nTotalRunoff = 0.0;
	m_nSurfaceRunoff = 0.0;
	m_nSoilVariation = 0.0;
	m_nReserv = 0.0;
	m_nRecharge = 0.0;
	m_nRain = 0.0;
	m_nLastSoil = 0.0;
	m_nLastGW = 0.0;
	m_nInterflow = 0.0;
	m_nInitSoil = 0.0;
	m_nInitGW = 0.0;
	m_nInfiltrate = 0.0;
	m_nImport = 0.0;
	m_nGWMove = 0.0;
	m_nGroundwater = 0.0;
	m_nGroundStorage = 0.0;
	m_nET = 0.0;
	m_nET_Imp = 0.0;
	m_nErrorBalance = 0.0;

}

void TNodeYear::Init(float nGW, float nSoil)
{
	m_nRain = m_nImport = m_nErrorBalance = m_nET = m_nGroundStorage = 0.0f;
	m_nGroundwater = m_nInterflow = m_nRecharge = m_nSoilVariation = m_nSurfaceRunoff = m_nTotalRunoff = 0.0f;
	m_nET_Imp = m_nET_Per = 0.0f;
	m_nInfiltrate = 0;
	m_nGWMove = 0;
	m_nReserv = 0;

	m_nInitGW = nGW;
	m_nInitSoil = nSoil;
}

void TNodeYear::Calc(float nAreaPer, float nSoilDepth, float nAqfS)
{
	m_nET = m_nET_Imp + m_nET_Per;
	m_nSoilVariation = (m_nLastSoil - m_nInitSoil) * nSoilDepth * nAreaPer;
	m_nTotalRunoff = m_nSurfaceRunoff + m_nInterflow + m_nGroundwater + m_nReserv;
	m_nGroundStorage = (m_nLastGW - m_nInitGW) * nAqfS/* * nAreaPer*/;
	m_nErrorBalance = (m_nRain + m_nImport) - (m_nET + m_nTotalRunoff) - (m_nSoilVariation + m_nGroundStorage/* + m_nRecharge*/ + m_nGWMove);
}

/*
void TNodeYear::Calc(float nAreaPer, float nSoilDepth, float nAqfS)
{
	m_nET = m_nET_Imp + m_nET_Per;
	m_nSoilVariation = (m_nLastSoil - m_nInitSoil) * nSoilDepth * nAreaPer;
	m_nTotalRunoff = m_nSurfaceRunoff + m_nInterflow + m_nGroundwater;
	m_nGroundStorage = (m_nLastGW - m_nInitGW) * nAqfS;
	m_nErrorBalance = m_nRain - (m_nET + m_nTotalRunoff) - (m_nSoilVariation + m_nGroundStorage);
}
*/

void TNodeYear::Add(float nRain, float nImport, float nETImp, float nETPer, float nSurf, float nInter, float nGW, float nRes, float nRech, float nGWL, float nSoil, float nInfiltrate, float gw_move)
{
	m_nLastGW = nGWL;
	m_nLastSoil = nSoil;

	m_nRain += nRain;
	m_nImport += nImport;
	m_nET_Imp += nETImp;
	m_nET_Per += nETPer;
//	m_nTotalRunoff += nTotal;
	m_nSurfaceRunoff += nSurf;
	m_nInterflow += nInter;
	m_nGroundwater += nGW;
	m_nReserv += nRes;
	m_nRecharge += nRech;
	m_nInfiltrate += nInfiltrate;
	m_nGWMove += gw_move;

	m_nCount++;
}
/*
void TNodeYear::Add(float nRain, float nETImp, float nETPer, float nTotal, float nSurf, float nInter, float nGW, float nRech, float nGWL, float nSoil)
{
	m_nLastGW = nGWL;
	m_nLastSoil = nSoil;

	m_nRain += nRain;
	m_nET_Imp += nETImp;
	m_nET_Per += nETPer;
	m_nTotalRunoff += nTotal;
	m_nSurfaceRunoff += nSurf;
	m_nInterflow += nInter;
	m_nGroundwater += nGW;
	m_nRecharge += nRech;

	m_nCount++;
}
*/

TNodeBalance::TNodeBalance(int nNodeID)
{
	m_nNodeID = nNodeID;
	m_nStartYear = m_nEndYear = -1;
	m_nType = 0;
	m_nArea = 0.0f;
}

TNodeBalance::~TNodeBalance()
{
}

TNodeYear* TNodeBalance::FindOrCreateYear(int nYear, float nGW, float nSoil)
{
	TNodeYear *pYear = NULL;
	int nIndex, nCount = GetCount();

	for(nIndex = 0; nIndex < nCount; nIndex++)
	{
		if(GetAt(nIndex)->m_nYear == nYear)
			return GetAt(nIndex);
	}

	pYear = new TNodeYear;
	pYear->m_nYear = nYear;
	pYear->Init(nGW, nSoil);
	Add(pYear);

	if(m_nStartYear == -1 || m_nStartYear > nYear)
		m_nStartYear = nYear;
	if(m_nEndYear == -1 || m_nEndYear < nYear)
		m_nEndYear = nYear;

	return pYear;
}

TWaterBalance::TWaterBalance(void)
{
}

TWaterBalance::~TWaterBalance(void)
{
}

TNodeBalance* TWaterBalance::FindOrCreateNode(int nNodeID, int nType)
{
	TNodeBalance *pNode = FindNode(nNodeID);

	if(pNode == NULL)
	{
		pNode = new TNodeBalance(nNodeID);
		pNode->m_nType = nType;
		Add(pNode);
	}

	return pNode;
}

int TWaterBalance::GetStartYear(void)
{
	if(GetCount() > 0)
	{
		return GetAt(0)->GetStartYear();
	}

	return -1;
}

int TWaterBalance::GetEndYear(void)
{
	if(GetCount() > 0)
	{
		return GetAt(0)->GetEndYear();
	}

	return -1;
}

TNodeBalance* TWaterBalance::FindNode(int nNodeID)
{
	//TNodeBalance *pNode = NULL;
	int nIndex, nCount = GetCount();

	for(nIndex = 0; nIndex < nCount; nIndex++)
	{
		if(GetAt(nIndex)->GetNodeID() == nNodeID)
			return GetAt(nIndex);
	}

	return NULL;
}
