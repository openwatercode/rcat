#include "StdAfx.h"
#include "include/ModelManager.h"
#include "include/CATMsg.h"
#include <wchar.h>
#include "include/ValidateCheck.h"

TModelManager::TModelManager(void)
{
	m_ppSource = m_ppConnect = NULL;
	m_nImport = m_nSource = m_nConnect = 0;
	m_ppImport = NULL;
	m_hwndNotify = NULL;
	m_pfPostMsg = &TModelManager::VoidMessage;
	m_nTimes = 0;
	m_nLoop = 0;
	m_nInterval = 0;
	m_nDT = 0;
	m_bStop = 0;
}

TModelManager::~TModelManager(void)
{
	if(m_ppSource)
		delete [] m_ppSource;
	if(m_ppConnect)
		delete [] m_ppConnect;
	if(m_ppImport)
		delete [] m_ppImport;
}

TBaseNode* TModelManager::FindItem(int nID)
{
	int nIndex, nCount = GetCount();

	for(nIndex = 0; nIndex < nCount; nIndex++)
	{
		if(GetAt(nIndex)->m_nID == nID)
			return GetAt(nIndex);
	}

	return NULL;
}

int TModelManager::FindItem(TBaseNode *pNode)
{
	int nIndex, nCount = GetCount();

	for(nIndex = 0; nIndex < nCount; nIndex++)
	{
		if(GetAt(nIndex) == pNode)
			return GetAt(nIndex)->GetID();
	}

	return -1;
}

int TModelManager::GetNodeCount(int nType, BOOL bMajorOnly)
{
	int nIndex, nCount = GetCount(), nResult = 0;

	if(bMajorOnly)
	{
		nType = nType & 0xFF00;

		for(nIndex = 0; nIndex < nCount; nIndex++)
		{
			if(GetAt(nIndex)->GetMajorType() == nType)
				nResult++;
		}
	}
	else
	{
		for(nIndex = 0; nIndex < nCount; nIndex++)
		{
			if(GetAt(nIndex)->GetType() == nType)
				nResult++;
		}
	}

	return nResult;
}

void TModelManager::SetRouteOrder(void)
{
	int i, j, nIndex;
	int *pDepth;
	TBaseNode* pNext = NULL;
	//int nMax = 0, nOrder = 0, nCount = GetCount();
	int nPos;

	if(m_nConnect <= 0)
		return;

	pDepth = new int[m_nConnect];
	memset(pDepth, 0, sizeof(int) * m_nConnect);

	nPos = 0;
	for(nIndex = 0; nIndex < m_nConnect; nIndex++)
	{
		pNext = m_ppConnect[nIndex]->GetOutput();
		while(pNext)
		{
			pDepth[nPos]++;
			pNext = pNext->GetOutput();
		}
		nPos++;
	}

	for(i = m_nConnect - 1; i > 0; i--)
	{
		for(j = 0; j < i; j++)
		{
			if(pDepth[j] < pDepth[j + 1])
			{
				int nTemp = pDepth[j];
				pDepth[j] = pDepth[j + 1];
				pDepth[j + 1] = nTemp;

				TBaseNode* pTemp = m_ppConnect[j];
				m_ppConnect[j] = m_ppConnect[j + 1];
				m_ppConnect[j + 1] = pTemp;
			}
		}
	}

	delete [] pDepth;

#ifdef _DEBUG
	FILE *fp;

	fp = fopen("calculate order.txt", "wt");
	if(fp)
	{
		for(i = 0; i < m_nConnect; i++)
		{
			fprintf(fp, "  %d : %s\n", i + 1, m_ppConnect[i]->GetName());
		}
		fclose(fp);
	}
#endif
}

int TModelManager::Calculate(BOOL bThread)
{
	m_nInterval = m_nDT / 60;
	unsigned long nStartTime, nEndTime;

	m_bStop = FALSE;
	if(m_nInterval < 60)
	{
		nStartTime = TDate::ToMinute(m_dtStart.nYear, m_dtStart.nMonth, m_dtStart.nDay, m_dtStart.nHour, m_dtStart.nMinute);
		nEndTime = TDate::ToMinute(m_dtEnd.nYear, m_dtEnd.nMonth, m_dtEnd.nDay, m_dtEnd.nHour, m_dtEnd.nMinute);
	}
	else if(m_nInterval < 1440)
	{
		nStartTime = TDate::ToMinute(m_dtStart.nYear, m_dtStart.nMonth, m_dtStart.nDay, 1, 0);
		nEndTime = TDate::ToMinute(m_dtEnd.nYear, m_dtEnd.nMonth, m_dtEnd.nDay, 24, 0) + m_nInterval;
	}
//	else if(m_nInterval)
	else
	{
		nStartTime = TDate::ToMinute(m_dtStart.nYear, m_dtStart.nMonth, m_dtStart.nDay, 1, 0);
		nEndTime = TDate::ToMinute(m_dtEnd.nYear, m_dtEnd.nMonth, m_dtEnd.nDay, 1, 0) + 1440;
	}

	m_nTimes = (nEndTime - nStartTime) / m_nInterval;

	(this->*m_pfPostMsg)(0, m_nTimes);

	m_Balance.Clear();
	m_BalanceIF.Clear();
	m_Result.Clear();

	SetModelData();

#ifdef WINRLIB
	if(bThread)
	{
		DWORD dwThreadID = 0;
		HANDLE hThread = NULL;

		hThread = CreateThread(NULL, 0, CalculateNode, (LPVOID)this, 0, &dwThreadID);
	}
	else
#endif // WINRLIB
		CalculateNode(this);

	return 0;
}

DWORD TModelManager::CalculateNode(LPVOID pData)
{
	int nIndex;
	TModelManager *pMan = (TModelManager*)pData;
	unsigned long nTime = TDate::GetMinutes(pMan->m_dtStart);

	pMan->m_Date = nTime;

	// 노드의 계산변수를 초기화 하고, 출력 테이블을 초기화한다.
	for(nIndex = 0; nIndex < pMan->GetCount(); nIndex++)
	{
		TBaseNode* pNode = pMan->GetAt(nIndex);

		pNode->Init();
		pNode->InitResult(nTime, pMan->m_nInterval, pMan->m_nTimes);

		if(pNode->GetMajorType() == NODE_SOURCE)
			((TCatchment*)pNode)->m_sClimate.SetFirstPos(0);
		else if(pNode->GetType() == NODE_WETLAND)
			((TWetLand*)pNode)->m_sClimate.SetFirstPos(0);
		else if(pNode->GetType() == NODE_POND)
			((TPond*)pNode)->m_sClimate.SetFirstPos(0);
		else if(pNode->GetType() == NODE_BIORETENTION)
			((TBioRetention*)pNode)->m_sClimate.SetFirstPos(0);
	}

	// 계산횟수만큼 반복하며
	for(nIndex = 0; nIndex < pMan->m_nTimes && !pMan->m_bStop; nIndex++)
	{
		int nCat, nJunc;

//		if(pMan->m_hwndNotify)
//			PostMessage(pMan->m_hwndNotify, UM_CALCULATE, WPARAM(1), LPARAM(nIndex));
		pMan->PostSafeMessage(1, nIndex);

		// 지하수 이동을 계산한다.
		for(nCat = 0; nCat < pMan->m_nSource; nCat++)
			pMan->m_ppSource[nCat]->CalcGWMove();

/*
		for(nCat = 0; nCat < pMan->m_nConnect; nCat++)
			pMan->m_ppConnect[nCat]->CalcGWMove();
*/


		for(nCat = 0; nCat < pMan->m_nImport; nCat++)
			pMan->m_ppImport[nCat]->Calculate(nIndex);

		// 매시간마다 노드를 계산한다.
		for(nCat = 0; nCat < pMan->m_nSource; nCat++)
		{
			pMan->m_ppSource[nCat]->Calculate(nIndex);
		}

		// Junk간의 유출 계산
		for(nJunc = 0; nJunc < pMan->m_nConnect; nJunc++)
			pMan->m_ppConnect[nJunc]->Calculate(nIndex);

		pMan->m_Date.Increase(pMan->m_nDT / 60);
	}

	pMan->CheckBalanceOutlet();

//	if(pMan->m_hwndNotify)
//		PostMessage(pMan->m_hwndNotify, UM_CALCULATE, 2, LPARAM(pMan->m_nTimes));

#ifdef WINRLIB
	if(pMan->m_hwndNotify)
	{
		if(nIndex == pMan->m_nTimes)
			SendMessage(pMan->m_hwndNotify, UM_CALCULATE, 2, LPARAM(pMan->m_nTimes));
//			pMan->PostSafeMessage(2, pMan->m_nTimes);
		else
			SendMessage(pMan->m_hwndNotify, UM_CALCULATE, 3, LPARAM(nIndex));
//			pMan->PostSafeMessage(3, nIndex);
	}
#endif // WINRLIB

	return 0;
}

int TModelManager::SetModelData(void)
{
	int nIndex, nCount = GetCount();
	int nUrban = 0, nJunc = 0, nImport = 0;
	int nMaxRec = 0;
	unsigned long nStartTime = TDate::GetMinutes(m_dtStart);
	unsigned long nEndTime = nStartTime + m_nInterval * m_nTimes;

	m_Result.Clear();

	m_nImport = m_nSource = m_nConnect = 0;
	for(nIndex = 0; nIndex < nCount; nIndex++)
	{
		TSeries *pResult = NULL;
		TBaseNode *pNode = GetAt(nIndex);

		pNode->m_nDT = m_nDT;
		pNode->m_nLoop = m_nLoop;

		pNode->SetDate(&m_Date);

		switch(pNode->GetType())
		{
		case NODE_CLIMATE:
			((TClimate*)(pNode))->SetModelEnv(nStartTime, nEndTime, m_nInterval);
			break;
		case NODE_URBAN:
		case NODE_FOREST:
		case NODE_PADDY:
			m_nSource++;
			pResult = new TSeries;
			pNode->m_pResult = pResult;
			break;
		case NODE_IMPORT:
			m_nImport++;
			pResult = new TSeries;
			pNode->m_pResult = pResult;
			break;
		case NODE_INFILTRO:
		case NODE_BIORETENTION:
		case NODE_WETLAND:
		case NODE_POND:
		case NODE_RAINTANK:
		case NODE_RECYCLE:
		case NODE_JUNC:
		case NODE_OUTLET:
			m_nConnect++;
			pResult = new TSeries;
			pNode->m_pResult = pResult;
			break;
		case NODE_LINK:
			pResult = new TSeries;
			pNode->m_pResult = pResult;
			strcpy_s(GetAt(nIndex)->m_pResult->m_Header.szDescript, 100, GetAt(nIndex)->GetName());
			break;
		}

		if(pResult)
		{
			pResult->m_Header.nTime = nStartTime;
			pResult->m_dtStart = nStartTime;
			pResult->m_Header.nInterval = m_nInterval;
			m_Result.Add(pResult);
		}
	}

	m_Result.m_Header.nTime = nStartTime;
	m_Result.m_Header.nInterval = m_nInterval;

/*
	if(m_nSource == 0)
		return -1;

	if(m_nConnect == 0)
		return -2;
*/
	if(m_nSource == 0 && m_nConnect == 0)
		return -1;

	if(m_ppSource)
		delete [] m_ppSource;
	if(m_ppConnect)
		delete [] m_ppConnect;
	if(m_ppImport)
	{
		delete [] m_ppImport;
//		m_ppImport = NULL;
	}

// TODO (hspark#1#): 경고 에러 등 메시지 전달 방식을 바꿀 필요 있음
#ifdef WINRDLL
	if(m_nSource == 0 && m_nConnect == 0)
		::MessageBox(NULL, "Node error!", "Error", 0);
#endif // WINRDLL

	m_ppSource = new TBaseNode*[m_nSource];
	m_ppConnect = new TBaseNode*[m_nConnect];
//	if(m_nImport > 0)
		m_ppImport = new TImport*[m_nImport];

	for(nIndex = 0; nIndex < nCount; nIndex++)
	{
		TBaseNode* pItem = GetAt(nIndex);

		pItem->m_pBalance = &m_BalanceIF;
		switch(pItem->GetType())
		{
		case NODE_URBAN:
		case NODE_FOREST:
		case NODE_PADDY:
			SetClimateData(dynamic_cast<TCatchment*>(pItem));
			pItem->m_pBalance = &m_Balance;
			m_ppSource[nUrban] = pItem;
//			((TUrban*)pItem)->SetStartTime(m_dtStart.nYear, m_dtStart.nMonth, m_dtStart.nDay, m_dtStart.nHour, m_dtStart.nMinute);
			strcpy_s(pItem->m_pResult->m_Header.szDescript, 100, pItem->GetName());
			nUrban++;
			break;
		case NODE_IMPORT:
			m_ppImport[nImport] = dynamic_cast<TImport*>(pItem);
			m_ppImport[nImport]->LoadSeries();
			strcpy_s(pItem->m_pResult->m_Header.szDescript, 100, pItem->GetName());
			nImport++;
			break;
		case NODE_POND:
		case NODE_INFILTRO:
		case NODE_BIORETENTION:
		case NODE_WETLAND:
		case NODE_RAINTANK:
		case NODE_RECYCLE:
		case NODE_JUNC:
		case NODE_OUTLET:
			if(pItem->GetType() == NODE_POND)
				SetClimateData((TPond*)pItem);
			else if(pItem->GetType() == NODE_BIORETENTION)
				SetClimateData((TBioRetention*)pItem);
			else if(pItem->GetType() == NODE_WETLAND)
			{
				SetClimateData((TWetLand*)pItem);
//				((TUrban*)pItem)->SetStartTime(m_dtStart.nYear, m_dtStart.nMonth, m_dtStart.nDay, m_dtStart.nHour, m_dtStart.nMinute);
			}
			m_ppConnect[nJunc] = pItem;
			strcpy_s(pItem->m_pResult->m_Header.szDescript, 100, pItem->GetName());
			nJunc++;
			break;
		case NODE_CLIMATE:
			if((((TClimate*)(pItem))->m_sClimate.GetCount() > 0) && ((TClimate*)(pItem))->m_sClimate.GetSeries(0)->GetCount() > nMaxRec)
				nMaxRec = ((TClimate*)(pItem))->m_sClimate.GetSeries(0)->GetCount();
			break;
		}

		if(pItem->GetOutput())
		{
			pItem->GetLink()->DT_loop = m_nLoop;
		}
	}

	if(m_nTimes > nMaxRec && nMaxRec > 0)
		m_nTimes = nMaxRec;

	SetRouteOrder();

	return 0;
}

void TModelManager::SetClimateData(TCatchment *pCatch)
{
	TSeries* pClimate = &(pCatch->m_sClimate);
	TSeriesItem *pPrecip, *pEva;
	int nClimate, nIndex;
	unsigned long nTime = TDate::ToMinute(m_dtStart.nYear, m_dtStart.nMonth, m_dtStart.nDay, m_dtStart.nHour, m_dtStart.nMinute);

	pClimate->Clear();

	pPrecip = new TSeriesItem;
	pEva = new TSeriesItem;

	pPrecip->m_Header.nData = FT_PRECIP;
	pEva->m_Header.nData = FT_EVA;
	pPrecip->m_Header.nInterval = m_nInterval;
	pEva->m_Header.nInterval = m_nInterval;
	pPrecip->m_Header.date = nTime;
	pEva->m_Header.date = nTime;
	pPrecip->SetSize(m_nTimes);
	pEva->SetSize(m_nTimes);

	pClimate->m_Header.nInterval = m_nInterval;
	pClimate->m_dtStart = nTime;

	if(pClimate->AddSeries(pPrecip) == 0)
		pClimate->m_Header.nData |= FT_PRECIP;
	if(pClimate->AddSeries(pEva) == 0)
		pClimate->m_Header.nData |= FT_EVA;

	for(nClimate = 0; nClimate < 5; nClimate++)
	{
		if(pCatch->m_Climates[nClimate].pNode)
		{
			TClimate *pSrc = (TClimate*)(pCatch->m_Climates[nClimate].pNode);
			TSeriesItem *pPre = pSrc->m_sClimate.FindSeries(FT_PRECIP);
			TSeriesItem *pEv = pSrc->m_sClimate.FindSeries(FT_EVACALC);
			float nRate = pCatch->m_Climates[nClimate].nRain;

			if(pPrecip && pPre)
			{
				for(nIndex = 0; nIndex < m_nTimes; nIndex++)
					pPrecip->AddValue(nIndex, pPre->GetValue(nIndex) * nRate / 100.0f);
			}

			nRate = pCatch->m_Climates[nClimate].nEva;

			if(pEv == NULL)
				pEv = pSrc->m_sClimate.FindSeries(FT_EVA);

			if(pEv)
			{
				for(nIndex = 0; nIndex < m_nTimes; nIndex++)
					pEva->AddValue(nIndex, pEv->GetValue(nIndex) * nRate / 100.0f);
			}
		}
	}

	pClimate->ChangeIntervalByDefault(m_nInterval);
}

void TModelManager::SetClimateData(TPond *pPond)
{
	TSeries* pClimate = &(pPond->m_sClimate);
	TSeriesItem *pPrecip, *pEva;
	int nClimate, nIndex;
	unsigned long nTime = TDate::ToMinute(m_dtStart.nYear, m_dtStart.nMonth, m_dtStart.nDay, m_dtStart.nHour, m_dtStart.nMinute);

	pClimate->Clear();

	pPrecip = new TSeriesItem;
	pEva = new TSeriesItem;

	pPrecip->m_Header.nData = FT_PRECIP;
	pEva->m_Header.nData = FT_EVA;
	pPrecip->m_Header.nInterval = m_nInterval;
	pEva->m_Header.nInterval = m_nInterval;
	pPrecip->m_Header.date = nTime;
	pEva->m_Header.date = nTime;
	pPrecip->SetSize(m_nTimes);
	pEva->SetSize(m_nTimes);

	pClimate->m_Header.nInterval = m_nInterval;
	pClimate->m_dtStart = nTime;

	if(pClimate->AddSeries(pPrecip) == 0)
		pClimate->m_Header.nData |= FT_PRECIP;
	if(pClimate->AddSeries(pEva) == 0)
		pClimate->m_Header.nData |= FT_EVA;

	for(nClimate = 0; nClimate < 5; nClimate++)
	{
		if(pPond->m_Climates[nClimate].pNode)
		{
			TClimate *pSrc = (TClimate*)(pPond->m_Climates[nClimate].pNode);

			TSeriesItem *pPre = pSrc->m_sClimate.FindSeries(FT_PRECIP);
			TSeriesItem *pEv = pSrc->m_sClimate.FindSeries(FT_EVACALC);
			float nRate = pPond->m_Climates[nClimate].nRain;
			float nEva = pPond->m_Climates[nClimate].nEva;

			if(pEv == NULL)
				pEv = pSrc->m_sClimate.FindSeries(FT_EVA);

			if(pPre)
			{
				for(nIndex = 0; nIndex < m_nTimes; nIndex++)
				{
					pPrecip->AddValue(nIndex, pPre->GetValue(nIndex) * nRate / 100.0f);
				}
			}

			if(pEv)
			{
				for(nIndex = 0; nIndex < m_nTimes; nIndex++)
				{
					pEva->AddValue(nIndex, pEv->GetValue(nIndex) * nEva / 100.0f);
				}
			}
		}
	}

	pClimate->ChangeIntervalByDefault(m_nInterval);
}

void TModelManager::SetClimateData(TWetLand *pLand)
{
	TSeries* pClimate = &(pLand->m_sClimate);
	TSeriesItem *pPrecip, *pEva;
	int nClimate, nIndex;
	unsigned long nTime = TDate::ToMinute(m_dtStart.nYear, m_dtStart.nMonth, m_dtStart.nDay, m_dtStart.nHour, m_dtStart.nMinute);

	pClimate->Clear();

	pPrecip = new TSeriesItem;
	pEva = new TSeriesItem;

	pPrecip->m_Header.nData = FT_PRECIP;
	pEva->m_Header.nData = FT_EVA;
	pPrecip->m_Header.nInterval = m_nInterval;
	pEva->m_Header.nInterval = m_nInterval;
	pPrecip->m_Header.date = nTime;
	pEva->m_Header.date = nTime;
	pPrecip->SetSize(m_nTimes);
	pEva->SetSize(m_nTimes);

	pClimate->m_Header.nInterval = m_nInterval;
	pClimate->m_dtStart = nTime;

	if(pClimate->AddSeries(pPrecip) == 0)
		pClimate->m_Header.nData |= FT_PRECIP;
	if(pClimate->AddSeries(pEva) == 0)
		pClimate->m_Header.nData |= FT_EVA;

	for(nClimate = 0; nClimate < 5; nClimate++)
	{
		if(pLand->m_Climates[nClimate].pNode)
		{
			TClimate *pSrc = (TClimate*)(pLand->m_Climates[nClimate].pNode);

			TSeriesItem *pPre = pSrc->m_sClimate.FindSeries(FT_PRECIP);
			TSeriesItem *pEv = pSrc->m_sClimate.FindSeries(FT_EVACALC);
			float nRate = pLand->m_Climates[nClimate].nRain;
			float nEva = pLand->m_Climates[nClimate].nEva;

			if(pEv == NULL)
				pEv = pSrc->m_sClimate.FindSeries(FT_EVA);

			if(pPre)
			{
				for(nIndex = 0; nIndex < m_nTimes; nIndex++)
				{
					pPrecip->AddValue(nIndex, pPre->GetValue(nIndex) * nRate / 100.0f);
				}
			}

			if(pEv)
			{
				for(nIndex = 0; nIndex < m_nTimes; nIndex++)
				{
					pEva->AddValue(nIndex, pEv->GetValue(nIndex) * nEva / 100.0f);
				}
			}
		}
	}

	pClimate->ChangeIntervalByDefault(m_nInterval);
}

void TModelManager::SetClimateData(TBioRetention *pRetention)
{
	TSeries* pClimate = &(pRetention->m_sClimate);
	TSeriesItem *pPrecip, *pEva;
	int nClimate, nIndex;
	unsigned long nTime = TDate::ToMinute(m_dtStart.nYear, m_dtStart.nMonth, m_dtStart.nDay, m_dtStart.nHour, m_dtStart.nMinute);

	pClimate->Clear();

	pPrecip = new TSeriesItem;
	pEva = new TSeriesItem;

	pPrecip->m_Header.nData = FT_PRECIP;
	pEva->m_Header.nData = FT_EVA;
	pPrecip->m_Header.nInterval = m_nInterval;
	pEva->m_Header.nInterval = m_nInterval;
	pPrecip->m_Header.date = nTime;
	pEva->m_Header.date = nTime;
	pPrecip->SetSize(m_nTimes);
	pEva->SetSize(m_nTimes);

	pClimate->m_Header.nInterval = m_nInterval;
	pClimate->m_dtStart = nTime;

	if(pClimate->AddSeries(pPrecip) == 0)
		pClimate->m_Header.nData |= FT_PRECIP;
	if(pClimate->AddSeries(pEva) == 0)
		pClimate->m_Header.nData |= FT_EVA;

	for(nClimate = 0; nClimate < 5; nClimate++)
	{
		if(pRetention->m_Climates[nClimate].pNode)
		{
			TClimate *pSrc = (TClimate*)(pRetention->m_Climates[nClimate].pNode);

			TSeriesItem *pPre = pSrc->m_sClimate.FindSeries(FT_PRECIP);
			TSeriesItem *pEv = pSrc->m_sClimate.FindSeries(FT_EVACALC);
			float nRate = pRetention->m_Climates[nClimate].nRain;
			float nEva = pRetention->m_Climates[nClimate].nEva;

			if(pEv == NULL)
				pEv = pSrc->m_sClimate.FindSeries(FT_EVA);

			if(pPre)
			{
				for(nIndex = 0; nIndex < m_nTimes; nIndex++)
				{
					pPrecip->AddValue(nIndex, pPre->GetValue(nIndex) * nRate / 100.0f);
				}
			}

			if(pEv)
			{
				for(nIndex = 0; nIndex < m_nTimes; nIndex++)
				{
					pEva->AddValue(nIndex, pEv->GetValue(nIndex) * nEva / 100.0f);
				}
			}
		}
	}

	pClimate->ChangeIntervalByDefault(m_nInterval);
}

void TModelManager::CheckBalanceOutlet(void)
{
	int nIndex, nCount = m_Balance.GetCount();
	TBaseNode *pObj;

	for(nIndex = 0; nIndex < nCount; nIndex++)
	{
		TNodeBalance *pNode = m_Balance.GetAt(nIndex);
		pObj = FindItem(pNode->GetNodeID());

		if(pObj)
		{
			TJunc *pOutlet = FindOutlet(pObj);

			strcpy_s(pNode->m_szName, 100, pObj->GetName());
			strcpy_s(pNode->m_szOutlet, 100, pOutlet->GetName());
			pNode->m_nArea = pObj->GetArea();
//			pNode->m_nArea = ((TUrban*)pObj)->Area;
		}
	}
}

TJunc* TModelManager::FindOutlet(TBaseNode *pNode)
{
	//int nCount = GetCount();
	TBaseNode* pCur = pNode;

	while(pCur->GetOutput())
		pCur = pCur->GetOutput();

	return (pCur->GetType() == NODE_OUTLET) ? (TJunc*)pCur : NULL;
}

void TModelManager::PostProcessMessage(int nState, int nStep)
{
    #ifdef WINRLIB
	if(m_hwndNotify)
		//SendMessage(m_hwndNotify, UM_CALCULATE, WPARAM(nState), LPARAM(nStep));
		PostMessage(m_hwndNotify, UM_CALCULATE, WPARAM(nState), LPARAM(nStep));
    #endif // WINRLIB
}

void TModelManager::SetNotifyHwnd(HWND hwndNotify)
{
	m_hwndNotify = hwndNotify;
	m_pfPostMsg = m_hwndNotify ? &TModelManager::PostProcessMessage : &TModelManager::VoidMessage;
}

void TModelManager::PostSafeMessage(int nState, int nStep)
{
	(this->*m_pfPostMsg)(nState, nStep);
}

BOOL TModelManager::CheckUpNode(TBaseNode* pStart, TBaseNode *pCurrent)
{
	TBaseNode *pOutput = pStart;

	while(pOutput != NULL)
	{
		if(pOutput == pCurrent)
			return TRUE;

		pOutput = pOutput->GetOutput();
	}

	return FALSE;
}

/** @brief 텍스트 형태의 데이터를 로딩하는 함수
 *
 * @param szFile 데이터 파일 경로
 * @return 최근 발생한 에러 번호(정상종료는 0)
 *
 */
int TModelManager::LoadText(char* szFile)
{
	char szLine[1024];
	char szToken[100];
	char szToken2[100];
	char* cFind;
	FILE *fp;
	float nVals[10];

	/*
	if(_tfopen_s(&fp, szFile, _T("rt")) != 0)
		return GetLastError();
	*/

	fopen_s(&fp, szFile, "rt");
	if(fp == NULL)
	#ifndef WINRLIB
        return -1;
	#else
		return GetLastError();
    #endif // WINRLIB

	//strcpy_s(m_szPath, szFile);
	strcpy(m_szPath, szFile);
	cFind = strrchr(m_szPath, '\\');
	if(cFind)
		*cFind = '\0';

	while(!feof(fp))
	{
		memset(szLine, 0, sizeof(szLine));
		fgets(szLine, 1024, fp);

		memset(szToken, 0, sizeof(szToken));
		memset(szToken2, 0, sizeof(szToken2));

		cFind = strchr(szLine, '=');
		if(cFind)
		{
			strncpy_s(szToken, 100, szLine, (int)(cFind - szLine));
			TBaseNode::Trim(szToken);

			if(strcmp(szToken, "Node") == 0)
			{
				TBaseNode *pNode = NULL;

				strcpy_s(szToken2, 100, cFind + 1);
				TBaseNode::Trim(szToken2);

				if(strcmp(szToken2, "Urban") == 0)
					pNode = new TUrban;
				else if(strcmp(szToken2, "Forest") == 0)
				{
					pNode = new TUrban;
					pNode->m_nType = NODE_FOREST;
				}
				else if(strcmp(szToken2, "Paddy") == 0)
					pNode = new TPaddy;
				else if(strcmp(szToken2, "Link") == 0)
					pNode = new TLink;
				else if(strcmp(szToken2, "Junction") == 0)
					pNode = new TJunc;
				else if(strcmp(szToken2, "Outlet") == 0)
				{
					pNode = new TJunc;
					pNode->m_nType = NODE_OUTLET;
				}
				else if(strcmp(szToken2, "Climate") == 0)
					pNode = new TClimate;
				else if(strcmp(szToken2, "Import") == 0)
					pNode = new TImport;
				else if(strcmp(szToken2, "Infiltro") == 0)
					pNode = new TInfiltro;
				else if(strcmp(szToken2, "BioRetention") == 0)
					pNode = new TBioRetention;
				else if(strcmp(szToken2, "WetLand") == 0)
					pNode = new TWetLand;
				else if(strcmp(szToken2, "Pond") == 0)
					pNode = new TPond;
				else if(strcmp(szToken2, "Recycle") == 0)
					pNode = new TRecycle;
				else if(strcmp(szToken2, "RainTank") == 0)
					pNode = new TRainTank;

				if(pNode)
				{
					pNode->LoadText(fp);
					Add(pNode);

					switch(pNode->GetType())
					{
					case NODE_IMPORT:
						ChangeFilePathA(&(((TImport*)pNode)->m_szSeries[0]));
						break;
					case NODE_CLIMATE:
						CheckClimateLoad((TClimate*)pNode);
						break;
					}
				}
			}
			else if(strcmp(szToken, "Title") == 0)
			{
			}
			else if(strcmp(szToken, "StartTime") == 0)
			{
				TBaseNode::ReadValue(cFind + 1, nVals, 5);

				m_dtStart = TDate::ToMinute((int)nVals[0], (int)nVals[1], (int)nVals[2], (int)nVals[3], (int)nVals[4]);
			}
			else if(strcmp(szToken, "EndTime") == 0)
			{
				TBaseNode::ReadValue(cFind + 1, nVals, 5);

				m_dtEnd = TDate::ToMinute((int)nVals[0], (int)nVals[1], (int)nVals[2], (int)nVals[3], (int)nVals[4]);
			}
			else if(strcmp(szToken, "NodeCount") == 0)
			{
			}
			else if(strcmp(szToken, "Parameter") == 0)
			{
				TBaseNode::ReadValue(cFind + 1, nVals, 2);
				m_nDT = (int)nVals[0];
				m_nLoop = (int)nVals[1];
			}
		}
	};
	fclose(fp);

	CheckLoad();

	return 0;
}

void TModelManager::CheckLoad(void)
{
	int nItem, nCount = GetCount();

	for(nItem = 0; nItem < nCount; nItem++)
	{
		TBaseNode* pNode = GetAt(nItem);

		switch(pNode->GetType())
		{
/*
		case NODE_IMPORT:
			ChangeFilePathA(&(((TImport*)pNode)->m_szSeries[0]));
			break;
*/
		case NODE_URBAN:
		case NODE_FOREST:
		case NODE_PADDY:
			CheckCatchLoad((TCatchment*)pNode);
			break;
/*
		case NODE_CLIMATE:
			CheckClimateLoad((TClimate*)pNode);
			break;
*/
		case NODE_LINK:
			if(!CheckLinkLoad((TLink*)pNode))
			{
				Remove(pNode);
				nCount--;
			}
			break;
		case NODE_INFILTRO:
			CheckInfiltroLoad((TInfiltro*)pNode);
			break;
		case NODE_BIORETENTION:
			CheckInfiltroLoad((TInfiltro*)pNode);
			CheckClimateLoad(&((TBioRetention*)(pNode))->m_Climates[0]);
			break;
		case NODE_POND:
			CheckClimateLoad(&((TPond*)(pNode))->m_Climates[0]);
			break;
		case NODE_WETLAND:
			CheckClimateLoad(&((TWetLand*)(pNode))->m_Climates[0]);
			break;
		case NODE_RECYCLE:
			CheckRecycleLoad((TRecycle*)(pNode));
			break;
		case NODE_JUNC:
			((TJunc*)(pNode))->m_Drain.pNode = FindItem(((TJunc*)(pNode))->m_Drain.nID);
			break;
		}
	}
}



// From TObjectManager to TModelManager
// Modify on 2015/08/02 by Y.R.Cha
// Must check validation!!!!

int TModelManager::ChangeFilePathA(char* szPath)
{
	int nRet;
	char szPath2[MAX_PATH];
	//int nLen = strlen(szPath);

	if(TBaseNode::IsFileExist(szPath))
		nRet = 0;
	else
	{
		char szFile[100], *cFind;

		cFind = strrchr(szPath, '\\');
		if(cFind == NULL)
			cFind = strrchr(szPath, '/');

		nRet = 2;
		if(cFind)
		{
			strcpy_s(szFile, 100, cFind + 1);
			memset(szPath2, 0, sizeof(szPath2));
			sprintf_s(szPath2, MAX_PATH, "%s\\%s", m_szPath, szFile);
			if(TBaseNode::IsFileExist(szPath2))
			{
				strcpy_s(szPath, MAX_PATH, szPath2);
				nRet = 1;
			}
		}
	}

	return nRet;
}

TClimate* TModelManager::FindClimate(char* szName)
{
	int nIndex, nCount = GetCount();

	for(nIndex = 0; nIndex < nCount; nIndex++)
	{
		TBaseNode *pNode = GetAt(nIndex);
		if(pNode->GetType() == NODE_CLIMATE &&
			strcmp(pNode->GetName(), szName) == 0)
		{
			return (TClimate*)pNode;
		}
	}

	return NULL;
}

BOOL TModelManager::CheckCatchLoad(TCatchment *pCatch)
{
	int nIndex;

	for(nIndex = 0; nIndex < 5; nIndex++)
	{
		if(pCatch->m_Climates[nIndex].nID > 0)
		{
			pCatch->m_Climates[nIndex].pNode = FindItem(pCatch->m_Climates[nIndex].nID);
//			return TRUE;
		}
	}

	if(pCatch->GetType() == NODE_URBAN || pCatch->GetType() == NODE_FOREST || pCatch->GetType() == NODE_PADDY)
	{
		TUrban *pUrban = (TUrban*)pCatch;

		if(pUrban->gw_move_node > 0)
			pUrban->m_pGWMove = (TUrban*)FindItem(pUrban->gw_move_node);
	}

	return FALSE;
}

BOOL TModelManager::CheckInfiltroLoad(TInfiltro *pInfiltro)
{
	if(pInfiltro->in_node_id > 0)
		pInfiltro->m_pInNode = (TCatchment*)FindItem(pInfiltro->in_node_id);

	if(pInfiltro->gw_node_id > 0)
		pInfiltro->m_pGWMove = (TUrban*)FindItem(pInfiltro->gw_node_id);

	return TRUE;
}

BOOL TModelManager::CheckClimateLoad(NODECOMPOSITE *pNode)
{
	int nIndex;

	for(nIndex = 0; nIndex < 5; nIndex++)
	{
		if(pNode[nIndex].nID > 0)
		{
			pNode[nIndex].pNode = FindItem(pNode[nIndex].nID);
		}
	}

	return TRUE;
}

BOOL TModelManager::CheckRecycleLoad(TRecycle *pRecycle)
{
	int nIndex;

	for(nIndex = 0; nIndex < 5; nIndex++)
	{
		if(pRecycle->m_Recursive[nIndex].nID > 0)
		{
			pRecycle->m_Recursive[nIndex].pNode = FindItem(pRecycle->m_Recursive[nIndex].nID);
		}
	}

	return TRUE;
}

BOOL TModelManager::CheckLinkLoad(TLink* pLink)
{
	pLink->SetNode(FindItem(pLink->m_nStartID), FindItem(pLink->m_nEndID));
	return TRUE;
}

void TModelManager::CheckClimateLoad(TClimate* pClimate)
{
//	char szClimate[MAX_PATH];

//	strcpy(szClimate, pClimate->m_szClimate);
	if(ChangeFilePathA(pClimate->m_szClimate) != 2)
		pClimate->LoadSeries();
/*
	TCHAR szClimate[MAX_PATH];

	memset(szClimate, 0, sizeof(szClimate));
	MultiByteToWideChar(CP_ACP, 0, pClimate->m_szClimate, strlen(pClimate->m_szClimate), szClimate, MAX_PATH);
	if(TUtil::IsFileExist(szClimate))
		pClimate->LoadSeries();
	else
	{
		TCHAR szFile[100], *cFind;

		cFind = _tcsrchr(szClimate, _T('\\'));
		if(cFind == NULL)
			cFind = _tcsrchr(szClimate, _T('/'));

		if(cFind)
		{
			lstrcpy(szFile, cFind + 1);
			_stprintf_s(szClimate, _T("%s\\%s"), m_szPath, szFile);
			if(TUtil::IsFileExist(szClimate))
			{
				pClimate->SetClimateFile(szClimate);
				pClimate->LoadSeries();
			}
		}
	}
*/
}

void TModelManager::Remove(TBaseNode* pNode)
{
	switch(pNode->GetMajorType())
	{
	case NODE_SOURCE:
	case NODE_TREATMENT:
		RemoveNode(pNode);
		break;
	case NODE_JUNCTION:
		RemoveJunc((TJunc*)pNode);
		break;
	case NODE_CLIMATE:
		RemoveClimate((TClimate*)pNode);
		break;
	case NODE_LINKAGE:
		RemoveLink((TLink*)pNode);
		break;
	}

	CATNODEARRAY::Remove(pNode);
}

void TModelManager::RemoveClimate(TClimate* pClimate)
{
	int nIndex, nCount = GetCount();

	for(nIndex = 0; nIndex < nCount; nIndex++)
	{
		if((GetAt(nIndex)->GetType() & 0xff00) == NODE_SOURCE)
		{
			TUrban *pNode = (TUrban*)GetAt(nIndex);
			pNode->RemoveClimate(pClimate);
		}
	}
}

void TModelManager::RemoveNode(TBaseNode* pNode)
{
	int nIndex;

	for(nIndex = 0; nIndex < GetCount(); nIndex++)
	{
		if(GetAt(nIndex)->GetMajorType() == NODE_LINKAGE)
		{
			TLink* pLink = (TLink*)(GetAt(nIndex));
			if(pLink->GetStart() == pNode || pLink->GetEnd() == pNode)
			{
				Remove(pLink);
				nIndex--;
			}
		}
	}
}

void TModelManager::RemoveUrban(TUrban* pUrban)
{
	int nIndex;

	for(nIndex = 0; nIndex < GetCount(); nIndex++)
	{
		if(GetAt(nIndex)->GetMajorType() == NODE_LINKAGE)
		{
			TLink* pLink = (TLink*)GetAt(nIndex);
			if(pLink->GetStart() == pUrban)
			{
				Remove(pLink);
				nIndex--;
			}
		}
	}
}

void TModelManager::RemoveJunc(TJunc* pJunc)
{
	int nIndex;

	for(nIndex = 0; nIndex < GetCount(); nIndex++)
	{
		if(GetAt(nIndex)->GetMajorType() == NODE_LINKAGE)
		{
			TLink* pLink = (TLink*)GetAt(nIndex);
			if(pLink->GetStart() == pJunc || pLink->GetEnd() == pJunc)
			{
				Remove(pLink);
				nIndex--;
			}
		}
	}
}

void TModelManager::RemoveLink(TLink* pLink)
{
	if(pLink->GetStart())
		pLink->GetStart()->SetLink(NULL);
}

void TModelManager::SetClimateString(TUrban* pUrban)
{
	int nIndex;
	TCHAR szType[100], szRain[20];

	memset(szType, 0, sizeof(szType));
	for(nIndex = 0; nIndex < 5; nIndex++)
	{
		if(pUrban->m_Climates[nIndex].pNode)
		{
			char szNode[100];

		  //strcpy_s(szNode, 100, pUrban->m_Climates[nIndex].pNode->GetName());
		  //sprintf_s(szRain, 20, "%sX%.2f;", szNode, pUrban->m_Climates[nIndex].nRain);
		  //strcat_s(szType, 100, szRain);
		  strcpy(szNode, pUrban->m_Climates[nIndex].pNode->GetName());
		  sprintf(szRain, "%sX%.2f;", szNode, pUrban->m_Climates[nIndex].nRain);
		  strcat(szType, szRain);
		}
	}
}

void TModelManager::SetCurrentPath(char* szPath)
{
	strcpy_s(m_szPath, MAX_PATH, szPath);
}
