#include "StdAfx.h"
#include "include/Junc.h"
#include "include/Link.h"

TJunc::TJunc(void)
{
	m_nType = NODE_JUNC;
	Clear();
}

TJunc::~TJunc(void)
{
}

void TJunc::Clear(void)
{
	nStep = 0;
	pOutput = NULL;
	memset(&m_Drain, 0, sizeof(NODECOMPOSITE));
	m_Drain.nRain = 0.1;
}

void TJunc::Init(void)
{
	TBaseNode::Init();

	if(m_Drain.pNode)
		step_drain = m_Drain.nRain * m_nDT;
//		step_drain = ConvertDay2Step(m_Drain.nAmount);
	flow_drain = 0;
}

void TJunc::InitResult(ULONG nTime, int nInterval, int nCount)
{
	const char* pszJunkHeader[] = {"flow_total(㎥/s)", "flow_sf(㎥/s)", "flow_inter(㎥/s)", "flow_gw(㎥/s)", "flow_drain(㎥/s)"};

	m_pResult->Clear();

	m_pResult->m_dtStart = nTime;
	m_pResult->m_Header.nInterval = nInterval;
	m_pResult->m_Header.nData = FT_USER;
	m_pResult->m_Header.nCount = nCount;

	for(int nCol = 0; nCol < (int)(sizeof(pszJunkHeader) / sizeof(char*)); nCol++)
	{
		TSeriesItem *pItem = m_pResult->AddItem();
		strcpy_s(pItem->m_Header.szHeader, 50, pszJunkHeader[nCol]);
		pItem->m_Header.date = nTime;
		pItem->m_Header.nInterval = nInterval;
		pItem->SetSize(nCount);
	}
}

void TJunc::AddResult(int nStep)
{
	m_pResult->GetAt(1)->AddValue(nStep, (m_nSf + m_nImport) / m_nDT);
	m_pResult->GetAt(2)->AddValue(nStep, m_nGW / m_nDT);
	m_pResult->GetAt(3)->AddValue(nStep, m_nInter / m_nDT);
	m_pResult->GetAt(0)->AddValue(nStep, (m_nInflow + m_nImport) / m_nDT);
	m_pResult->GetAt(4)->AddValue(nStep, flow_drain / m_nDT);
}

void TJunc::Calculate(int nStep)
{
	flow_drain = 0;
	if(m_Drain.pNode)
	{
		float step, drain = step_drain;

		step = m_nGW > drain ? drain : m_nGW;
		drain -= step;
		flow_drain += step;
		m_nGW -= step;

		step = m_nInter > drain ? drain : m_nInter;
		drain -= step;
		flow_drain += step;
		m_nInter -= step;

		step = m_nSf > drain ? drain : m_nSf;
		drain -= step;
		flow_drain += step;
		m_nSf -= step;

		m_nInflow -= flow_drain;

		step = m_nImport > drain ? drain : m_nImport;
		flow_drain += step;
		m_nImport -= step;

		m_Drain.pNode->AddValue(nStep, flow_drain, 0, 0, flow_drain);
	}


	if(m_pLink)
		m_pLink->Calculate(nStep, (m_nSf + m_nImport), m_nGW, m_nInter, (m_nInflow + m_nImport));

	AddResult(nStep);
	m_nImport = m_nInflow = m_nSf = m_nGW = m_nInter = m_nLeakage = 0;
}

void TJunc::operator =(TBaseNode& node)
{
	TJunc *pJunc = (TJunc*)(&node);

	nStep = pJunc->nStep;
	pOutput = NULL;

	TBaseNode::operator =(node);
}

int TJunc::LoadText(FILE *fp)
{
	char szToken[100];
	char szToken2[1024];
	char *cFind;
	char szLine[1024];

	while(!feof(fp))
	{
		memset(szLine, 0, sizeof(szLine));
		fgets(szLine, 1024, fp);

		Trim(szLine);
		if(strcmp(szLine, "EndNode") == 0)
			break;

		memset(szToken, 0, sizeof(szToken));
		memset(szToken2, 0, sizeof(szToken2));

		cFind = strchr(szLine, '=');
		strncpy_s(szToken, 100, szLine, (cFind - &szLine[0]));
		strcpy(szToken2, cFind + 1);
		Trim(szToken);
		Trim(szToken2);

		if(strcmp(szToken, "NodeID") == 0)
			SetID(atol(szToken2));
		else if(strcmp(szToken, "Name") == 0)
			SetName(szToken2);
		else if(strcmp(szToken, "Desc") == 0)
			SetDesc(szToken2);
		else
			ExtraFileContent(szLine);
	}

	return 0;
}
