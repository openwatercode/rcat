#include "StdAfx.h"
#include "include/ValidateCheck.h"
#include <stdio.h>
//#include <CATAttr.h>
//#include <Util.h>

TValidateCheck::TValidateCheck(void)
{
	m_pManager = NULL;
	m_nError = 0;
	m_nWarning = 0;
}

TValidateCheck::~TValidateCheck(void)
{
}

void TValidateCheck::Clear()
{
	m_Message.Clear();
	m_nWarning = 0;
	m_nError = 0;
}

int TValidateCheck::Check(TModelManager *pManager)
{
	int nIndex, nCount;
	if(pManager != NULL)
		m_pManager = pManager;

	Clear();

	nCount = m_pManager->GetCount();

	for(nIndex = 0; nIndex < nCount; nIndex++)
	{
		TBaseNode *pNode = m_pManager->GetAt(nIndex);

		if(pNode->GetType() == NODE_LINK)
		{
			if(CheckOrphanLink(dynamic_cast<TLink*>(pNode)) == 0)
				CheckLink(dynamic_cast<TLink*>(pNode));
		}
		else if(pNode->GetType() != NODE_CLIMATE)
			CheckOrphanNode(pNode);


		if(pNode->GetMajorType() == NODE_SOURCE)
			CheckCatchment(dynamic_cast<TCatchment*>(pNode));
		else if(pNode->GetType() == NODE_CLIMATE)
			CheckClimate(dynamic_cast<TClimate*>(pNode));
		else if(pNode->GetType() == NODE_POND)
			CheckPond(dynamic_cast<TPond*>(pNode));
		else if(pNode->GetType() == NODE_RAINTANK)
			CheckTank(dynamic_cast<TRainTank*>(pNode));
		else if(pNode->GetType() == NODE_IMPORT)
			CheckImport(dynamic_cast<TImport*>(pNode));
		else if(pNode->GetType() == NODE_INFILTRO || pNode->GetType() == NODE_BIORETENTION)
			CheckInfiltro(dynamic_cast<TInfiltro*>(pNode));
	}

	return m_nError;
}

BOOL TValidateCheck::IsConnectOutlet(TBaseNode *pNode)
{
	TBaseNode *pNext = pNode;

	while(pNext != NULL)
	{
		if(pNext->GetType() == NODE_OUTLET)
			return TRUE;
		pNext = pNext->GetOutput();
		if(pNext == NULL || m_pManager->FindItem(pNext) == 0)
			return FALSE;
	}

	return FALSE;
}

void TValidateCheck::CheckInfiltro(TInfiltro* pInfilt)
{
	char szMsg[200], szName[100];

	if(pInfilt->in_node_id == 0 || pInfilt->m_pInNode == NULL)
	{
		GetNodeName(pInfilt, szName);

		sprintf_s(szMsg, 100, "Error : '%s' have no input node\r\n", szName);
		m_Message.Add(szMsg);
		m_nError++;
		return;
	}

	if(!m_pManager->CheckUpNode(pInfilt->m_pInNode, pInfilt))
	{
		GetNodeName(pInfilt, szName);

		sprintf_s(szMsg, 100, "Error : '%s''s input node is not upstream node\r\n", szName);
		m_Message.Add(szMsg);
		m_nError++;
		return;
	}

	if(pInfilt->m_pGWMove && pInfilt->m_pGWMove->GetType() != NODE_RAINTANK)
	{
		GetNodeName(pInfilt, szName);

		sprintf_s(szMsg, 100, "Erro : '%s''s water reuse node is not RainTank", szName);
		m_Message.Add(szMsg);
		m_nError++;
	}
}

int TValidateCheck::CheckOrphanLink(TLink* pLink)
{
	int nRet = 0;

	if(m_pManager->FindItem(pLink->GetStart()) == 0 || m_pManager->FindItem(pLink->GetEnd() == 0))
	{
		TCHAR szMsg[200], szName[100];

		GetNodeName(pLink, szName);

		sprintf_s(szMsg, 100, "Info : '%s' link is not connected. delete it\r\n", szName);
		m_Message.Add(szMsg);
		m_nWarning++;
		if(m_pManager->FindItem(pLink->GetStart()) > 0 && pLink == pLink->GetStart()->GetLink())
			pLink->GetStart()->SetLink(NULL);
		m_pManager->Remove(pLink);
		nRet = 1;
	}

	return nRet;
}

void TValidateCheck::CheckOrphanNode(TBaseNode* pNode)
{
	if(!IsConnectOutlet(pNode))
	{
		TCHAR szMsg[100], szName[100];

		GetNodeName(pNode, szName);

		sprintf_s(szMsg, 100, "Error : '%s' is not connected\r\n", szName);
		m_Message.Add(szMsg);
		m_nError++;
	}
}

void TValidateCheck::GetNodeName(TBaseNode* pNode, char* szName)
{
	strcpy_s(szName, strlen(pNode->GetName()), pNode->GetName());
}

void TValidateCheck::CheckLink(TLink* pLink)
{
	if(pLink->ValidateCheck() < 0)
	{
		TCHAR szMsg[100], szName[100];

		GetNodeName(pLink, szName);
		sprintf_s(szMsg, 100, "Error : '%s' link's parameter is zero.\r\n", szName);
		m_Message.Add(szMsg);
		m_nError++;
	}
}

void TValidateCheck::CheckCatchment(TCatchment* pCatch)
{
	TCHAR szMsg[200], szName[100];
	int nIndex;

	if(pCatch->m_nClimates == 0)
	{
		GetNodeName(pCatch, szName);
		sprintf_s(szMsg, 200, "Warning : '%s' weather data is missing\r\n", szName);
		m_Message.Add(szMsg);
		m_nWarning++;
	}

	for(nIndex = 0; nIndex < pCatch->m_nClimates; nIndex++)
	{
		if(m_pManager->FindItem(pCatch->m_Climates[nIndex].pNode) == NULL)
		{
			GetNodeName(pCatch, szName);
			sprintf_s(szMsg, 200, "Error : '%s' %dnd(st)r weather data is not correct\r\n", szName, nIndex + 1);
			m_Message.Add(szMsg);
			m_nError++;
		}
	}
}

void TValidateCheck::CheckClimate(TClimate* pClimate)
{
	char szMsg[200], szName[100];

	if(!TBaseNode::IsFileExist(pClimate->m_szClimate))
	{
		GetNodeName(pClimate, szName);

		sprintf_s(szMsg, 200, "Error : '%s' data file is missing\r\n", szName);
		m_Message.Add(szMsg);
		m_nError++;
		return;
	}
}

void TValidateCheck::CheckPond(TPond* pPond)
{
	char szMsg[200], szName[100];

	if(pPond->intake_type == 1 && !TBaseNode::IsFileExist(pPond->m_szIntake))
	{
		GetNodeName(pPond, szName);

		sprintf_s(szMsg, 200, "Error : '%s' intake series data is missing\r\n", szName);
		m_Message.Add(szMsg);
		m_nError++;
	}
}

void TValidateCheck::CheckTank(TRainTank* pTank)
{
	char szMsg[200], szName[100];

	if(pTank->use_type == 1 && !TBaseNode::IsFileExist(pTank->m_szUse))
	{
		GetNodeName(pTank, szName);

		sprintf_s(szMsg, 200, "Error : '%s' use series data is missing\r\n", szName);
		m_Message.Add(szMsg);
		m_nError++;
	}
}

void TValidateCheck::CheckImport(TImport *pImport)
{
	char szMsg[200], szName[100];

	if(pImport->type == 1 && !TBaseNode::IsFileExist(pImport->m_szSeries))
	{
		GetNodeName(pImport, szName);

		sprintf_s(szMsg, 200, "Error : '%s' series data is missing\r\n", szName);
		m_Message.Add(szMsg);
		m_nError++;
	}
}
