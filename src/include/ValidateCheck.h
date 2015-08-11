#pragma once

#include "CatModelExt.h"
#include "ModelManager.h"
#include "LargeString.h"

class CATMODEL_CLASS TValidateCheck
{
public:
	TValidateCheck(void);
	~TValidateCheck(void);

public:
	void SetManager(TModelManager *pManager) {m_pManager = pManager;};
	int Check(TModelManager *pManager = NULL);
	BOOL IsConnectOutlet(TBaseNode *pNode);
	void Clear();
	TLargeString* GetMessage() {return &m_Message;};

protected:
	TModelManager *m_pManager;
	TLargeString m_Message;
	int m_nWarning;
	int m_nError;

public:
	void CheckInfiltro(TInfiltro* pInfilt);
	int CheckOrphanLink(TLink* pLink);
	void CheckOrphanNode(TBaseNode* pNode);
	void GetNodeName(TBaseNode* pNode, char* szName);
	void CheckLink(TLink* pLink);
	void CheckCatchment(TCatchment* pCatch);
	void CheckClimate(TClimate* pClimate);
	void CheckPond(TPond* pPond);
	void CheckTank(TRainTank* pTank);
	void CheckImport(TImport* pImport);
};
