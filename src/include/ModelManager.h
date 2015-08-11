#pragma once

#include "CatModelExt.h"
#include "arraytempl.h"
#include "Catchment.h"
#include "Link.h"
#include "Junc.h"
#include "WaterBalance.h"
#include "Treatment.h"

typedef PTRARRAY<TBaseNode, 10> CATNODEARRAY;

class CATMODEL_CLASS TModelManager : public CATNODEARRAY
{
	typedef void (TModelManager::*PROCMSG)(int, int);
public:
	TModelManager(void);
	~TModelManager(void);

public:
	TBaseNode* FindItem(int nID);
	int FindItem(TBaseNode* pNode);
	int GetNodeCount(int nType, int bMajorOnly = FALSE);
	TJunc* FindOutlet(TBaseNode* pNode);
	int Calculate(BOOL bThread = TRUE);
	TSerieses* GetResult() {return &m_Result;};
	TWaterBalance* GetBalance() {return &m_Balance;};
	TWaterBalance* GetBalanceIF() {return &m_BalanceIF;};
	void SetNotifyHwnd(HWND hwndNotify = NULL);
	void PostSafeMessage(int nState, int nStep);
	int LoadText(char* szFile);
	void CheckLoad(void);
	int ChangeFilePathA(char* szPath);
	TClimate* FindClimate(char* szName);
	void Remove(TBaseNode* pNode);

protected:
	void SetRouteOrder(void);
	void CheckBalanceOutlet(void);
	int SetModelData(void);
	void SetClimateData(TCatchment *pCatch);
	void SetClimateData(TPond *pCatch);
	void SetClimateData(TWetLand *pLand);
	void SetClimateData(TBioRetention *pRetention);
	void PostProcessMessage(int nState, int nStep);
	void VoidMessage(int nState, int nStep) {};
	static DWORD WINAPI CalculateNode(LPVOID pData);
	BOOL CheckCatchLoad(TCatchment *pCatch);
	BOOL CheckInfiltroLoad(TInfiltro *pInfiltro);
	BOOL CheckClimateLoad(NODECOMPOSITE *pNode);
	BOOL CheckRecycleLoad(TRecycle *pRecycle);
	BOOL CheckLinkLoad(TLink* pLink);
	void CheckClimateLoad(TClimate* pClimate);
	void RemoveClimate(TClimate* pClimate);
	void RemoveNode(TBaseNode* pNode);
	void RemoveUrban(TUrban* pUrban);
	void RemoveJunc(TJunc* pJunc);
	void RemoveLink(TLink* pLink);
	void SetClimateString(TUrban* pUrban);
	void SetCurrentPath(char *szPath);

public:
	int m_nDT;
	int m_nLoop;
	TDate m_dtStart;
	TDate m_dtEnd;
	TWaterBalance m_Balance;
	TWaterBalance m_BalanceIF;
	TSerieses m_Result;
	TDate m_Date;
	PROCMSG m_pfPostMsg;
	BOOL m_bStop;

protected:
	TImport** m_ppImport;
	TBaseNode** m_ppSource;
	TBaseNode** m_ppConnect;
	int m_nSource;
	int m_nConnect;
	int m_nImport;
	unsigned short m_nInterval;
	int m_nTimes;
	HWND m_hwndNotify;
	char m_szPath[MAX_PATH];

public:
	BOOL CheckUpNode(TBaseNode* pStart, TBaseNode *pCurrent);
};
