#include "StdAfx.h"
#include "include/SeriesData.h"
#include "include/Link.h"
#include "include/BaseNode.h"
#include <math.h>

const float csnUnitRiver = 500;

TLink::TLink(void)
{
	m_nType = NODE_LINK;
	m_pfRoute = &TLink::Direct;
	m_pnPartIn = m_pnPartOut = NULL;
	Clear();
}

TLink::~TLink(void)
{
	if(m_pnPartIn)
		free(m_pnPartIn);
	if(m_pnPartOut)
		free(m_pnPartOut);
}

void TLink::Clear(void)
{
	Method = 0;
	DT = 1;
//	nDT = 0;
	X = 0.25f;
	K = 1.5f;
//	Delta_dis = 1000;
//	Routing_So = 0.005;
	Routing_N = 0.03;
//	Peak_t = 500.0f;
//	Routing_B = 10.0f;
//	mann = 0.03;
	init_route = 0.1;
	slope_riv = 0.005;
	length_riv = 1000;
	Bottom_riv = 10;
	Top_riv = 15;
	depth_riv = 5;
	DT_loop = 0;
	m_nStartID = 0;
	m_nEndID = 0;
	m_pStart = NULL;
	m_pEnd = NULL;
	m_nML = 5;
	m_nKIMAX = 25;
}

void TLink::operator =(TBaseNode &node)
{
	TLink *pLink = (TLink*)(&node);

	Method = pLink->Method;
	DT = pLink->DT;
//	nDT = pLink->nDT;
	X = pLink->X;
	K = pLink->K;
//	Delta_dis = pLink->Delta_dis;
//	Routing_So = pLink->Routing_So;
	Routing_N = pLink->Routing_N;
//	Routing_B = pLink->Routing_B;
//	mann = pLink->mann;
	init_route = pLink->init_route;
	slope_riv = pLink->slope_riv;
	length_riv = pLink->length_riv;
	Bottom_riv = pLink->Bottom_riv;
	Top_riv = pLink->Top_riv;
	depth_riv = pLink->depth_riv;
	DT_loop = pLink->DT_loop;
	m_nStartID = 0;
	m_nEndID = 0;
	m_pStart = m_pEnd = NULL;

	TBaseNode::operator =(node);
}

void TLink::Init(void)
{
	TBaseNode::Init();

	memset(m_nInput, 0, sizeof(m_nInput));
	memset(m_nOutput, 0, sizeof(m_nOutput));
	memset(m_nPrevIn, 0, sizeof(m_nPrevIn));
	memset(m_nPrevOut, 0, sizeof(m_nPrevOut));
	memset(m_nQMax, 0, sizeof(m_nQMax));

	switch(Method)
	{
	case 1:
		m_pfRoute = &TLink::Muskingum;
		m_nConst1 = (DT - 2 * K * X) / (2 * K * (1 - X) + DT);
		m_nConst2 = (DT + 2 * K * X) / (2 * K * (1 - X) + DT);
		m_nConst3 = (2 * K * (1 - X) - DT) / (2 * K * (1 - X) + DT);
		/*
		m_nConst1 = (m_nDT - 2 * K * 3600 * X) / (2 * K * 3600 * (1 - X) + m_nDT);
		m_nConst2 = (m_nDT + 2 * K * 3600 * X) / (2 * K * 3600 * (1 - X) + m_nDT);
		m_nConst3 = (2 * K * 3600 * (1 - X) - m_nDT) / (2 * K * 3600 * (1 - X) + m_nDT);
		*/
		break;
	case 2:
		m_nPartCount = ceil(length_riv / csnUnitRiver);
		if(m_nPartCount * csnUnitRiver < length_riv)
			m_nPartCount++;
		if(m_pnPartIn)
			free(m_pnPartIn);
		if(m_pnPartOut)
			free(m_pnPartOut);
		m_pnPartIn = (float*)malloc(sizeof(float) * (m_nPartCount + 1) * 4);
		m_pnPartOut = (float*)malloc(sizeof(float) * (m_nPartCount + 1)* 4);
		memset(m_pnPartIn, 0, sizeof(float) * (m_nPartCount + 1) * 4);
		memset(m_pnPartOut, 0, sizeof(float) * (m_nPartCount + 1) * 4);
		m_nUnitRiv = length_riv / m_nPartCount;

		m_pfRoute = &TLink::Cunge;
		break;
	case 3:
		m_nPartCount = ceil(length_riv / csnUnitRiver);
		if(m_nPartCount * csnUnitRiver < length_riv)
			m_nPartCount++;
		if(m_pnPartIn)
			free(m_pnPartIn);
		if(m_pnPartOut)
			free(m_pnPartOut);
		m_pnPartIn = (float*)malloc(sizeof(float) * (m_nPartCount + 1) * 4);
		m_pnPartOut = (float*)malloc(sizeof(float) * (m_nPartCount + 1)* 4);
		memset(m_pnPartIn, 0, sizeof(float) * (m_nPartCount + 1) * 4);
		memset(m_pnPartOut, 0, sizeof(float) * (m_nPartCount + 1) * 4);
		m_nUnitRiv = length_riv / m_nPartCount;

		m_pfRoute = &TLink::Kinematic;
		DT = m_nDT / DT_loop;
		m_nPrevOut[0] = m_nPrevOut[1] = m_nPrevOut[2] = m_nPrevOut[3] = init_route;
		m_nBM1 = (Top_riv - Bottom_riv) / (2 * depth_riv);
		m_nALF1 = pow((Routing_N * pow(Bottom_riv, 0.666666f) * pow(slope_riv, -0.5f)), 0.6f);
		m_nYRV1[0] = m_nYRV1[1] = m_nYRV1[2] = m_nYRV1[3] =
			pow((Routing_N * init_route / Bottom_riv * pow(slope_riv, -0.5f)), 0.6f);
		break;
	default:
		m_pfRoute = &TLink::Direct;
	}
}

void TLink::InitResult(unsigned long nTime, int nInterval, int nCount)
{
	const char* pszLinkHeader[] = {"flow_in(㎥/s)", "flow_out(㎥/s)"};

	m_pResult->Clear();
	m_pResult->m_dtStart = nTime;
	m_pResult->m_Header.nInterval = nInterval;
	m_pResult->m_Header.nData = FT_USER;
	m_pResult->m_Header.nCount = nCount;

	for(int nCol = 0; nCol < (int)(sizeof(pszLinkHeader) / sizeof(char*)); nCol++)
	{
		TSeriesItem *pItem = m_pResult->AddItem();
		strcpy_s(pItem->m_Header.szHeader, 50, pszLinkHeader[nCol]);
		pItem->m_Header.date = nTime;
		pItem->m_Header.nInterval = nInterval;
		pItem->SetSize(nCount);
	}
}

void TLink::Calculate(int nStep, float nSf, float nGw, float nInter, float nTotal)
{
	int nData;
	float nVals[4] = {nSf, nGw, nInter, nTotal};

	m_pResult->GetAt(0)->SetValue(nStep, nTotal / m_nDT);

	for(nData = 0; nData < 4; nData++)
		(this->*m_pfRoute)(nData, nVals[nData]);

	m_pEnd->AddValue(nStep, m_nOutput[0], m_nOutput[1], m_nOutput[2], m_nOutput[3]);

	m_pResult->GetAt(1)->SetValue(nStep, m_nOutput[3] / m_nDT);

	m_nImport = m_nLeakage = 0;
}

void TLink::AddImport(int nStep, float nImport, float nLeakage)
{
	m_pResult->GetAt(0)->SetValue(nStep, nImport / m_nDT);
	(this->*m_pfRoute)(0, nImport);
	m_pEnd->AddImport(nStep, m_nOutput[0], nLeakage);
	m_pResult->GetAt(1)->SetValue(nStep, m_nOutput[0] / m_nDT);
}

float TLink::Direct(int nData, float nInput)
{
	m_nOutput[nData] = m_nInput[nData] = nInput;
	return nInput;
}

float TLink::Cunge(int nData, float nInput)
{
	float nQAve, nC, nC1, nC2, nC3;
	//const float csnVar1 = 5.0f / 3.0f;
	//float dt = m_nDT;
	//float nN, nDTN, nC0, nXo, nK;
	int nIndex, nPos = 0;//, nPosNext;
	float nRivLen;

	m_nPrevOut[nData] = m_nOutput[nData];
	m_nPrevIn[nData] = m_nInput[nData];
	m_nInput[nData] = nInput;

	float nIn, nPrevIn, nPrevOut;//, nOut;

	nIn = nInput;
	nPrevIn = m_nPrevIn[nData];
	nPrevOut = m_nPrevOut[nData];
	nRivLen = length_riv / m_nPartCount;

// TODO (hspark#1#): m_nPartCount가 0보다 작아 for loop이 수행되지 않으면 nPos가 초기화 되지 않는 문제 발생
	for(nIndex = 0; nIndex < m_nPartCount; nIndex++)
	{
		nPos = nIndex * 4 + nData;

		nPrevIn = m_pnPartIn[nPos];
		nPrevOut = m_pnPartOut[nPos];

		nQAve = (nIn + nPrevIn + nPrevOut) / 3;
		nQAve /= m_nDT;

		if(nQAve > 0)
		{
			float C, D, nTemp;

			nC = 5 * ((pow(slope_riv, 0.3f) * (pow(abs(nQAve), 0.4f)) / pow(Routing_N, 0.6f))) / 3;

			C = 1;
			D = nQAve / (slope_riv * nC * nRivLen);
			nTemp = 1 + C + D;

			nC1 = (-1 + C + D) / nTemp;
			nC2 = (1 + C - D) / nTemp;
			nC3 = (1 - C + D) / nTemp;

			m_pnPartOut[nPos] = (nC1 * nIn + nC2 * nPrevIn + nC3 * nPrevOut);

			m_pnPartIn[nPos] = nIn;
			nIn = m_pnPartOut[nPos];
		}
		else
		{
			m_pnPartOut[nPos] = 0;
		}
	}

	m_nOutput[nData] = m_pnPartOut[nPos];


	// 이전 계산 공식 (2012.06.15 이전)

/*
	nQAve = (nInput + m_nPrevIn[nData] + m_nPrevOut[nData]) / 3;
	if(nQAve > 0)
	{
		float C, D, dx, nTemp;
		int N;

		nQAve /= m_nDT;
		nC = 5 * ((pow(slope_riv, 0.3f) * (pow(abs(nQAve), 0.4f)) / pow(Routing_N, 0.6f))) / 3; // 분수값 실수로 변경
		if(nC == 0)
		{
			nC = length_riv / m_nDT;
			N = 1;
		}
		else
		{
			N = int((length_riv / (nC * m_nDT)) + 0.5f);
			if(N < 1)
				N = 1;
		}

		dx = length_riv / N;
		float nIn, nOut, nPrevIn, nPrevOut;
		nIn = m_nInput[nData];
		nPrevIn = m_nPrevIn[nData];
		nPrevOut = m_nPrevOut[nData];
		for(int idx = 0; idx < N; idx++)
		{
			C = 1;
			D = nQAve / (slope_riv * nC * dx);
			nTemp = 1 + C + D;

			nC1 = (-1 + C + D) / nTemp;
			nC2 = (1 + C - D) / nTemp;
			nC3 = (1 - C + D) / nTemp;

			nOut = nC1 * nIn + nC2 * nPrevIn + nC3 * nPrevOut;
			nPrevOut = nOut;
			nIn = nOut;
			nPrevIn = nIn;
//			m_nOutput[nData] = nC1 * nInput + nC2 * m_nPrevIn[nData] + nC3 * m_nPrevOut[nData];
		}
	}
	else
	{
		m_nOutput[nData] = 0;
	}
*/


	return m_nOutput[nData];
}

float TLink::Muskingum(int nData, float nInput)
{
	m_nPrevOut[nData] = m_nOutput[nData];
	m_nPrevIn[nData] = m_nInput[nData];
	m_nInput[nData] = nInput;

	m_nOutput[nData] = m_nConst1 * nInput + m_nConst2 * m_nPrevIn[nData] + m_nConst3 * m_nPrevOut[nData];
	if(m_nOutput[nData] < 0)
		m_nOutput[nData] = m_nPrevOut[nData];

	return m_nOutput[nData];
}

float TLink::Kinematic2(int nData, float nInput)
{
	int nLoop, nIndex, nPos;
	float nTempArea;

	m_nInput[nData] = nInput;

	float nIn, nPrevIn, nPrevOut; //, nOut;

	nIn = nInput;
	nPrevIn = m_nPrevIn[nData];
	nPrevOut = m_nPrevOut[nData];

	for(nIndex = 0; nIndex < m_nPartCount; nIndex++)
	{
		nPos = nIndex * 4 + nData;

		nPrevIn = m_pnPartIn[nPos];
		nPrevOut = m_pnPartOut[nPos];

		m_nYRV1[nData] = pow((Routing_N * init_route / Bottom_riv * pow(slope_riv, -0.5f)), 0.6f);

		for(nLoop = 0; nLoop < DT_loop; nLoop++)
		{
			m_nALFA = m_nALF1 * (1 + 2 * pow((1 + m_nBM1 * m_nBM1), 0.5f)) * m_nYRV1[nData] / Bottom_riv;
			m_pnPartOut[nPos] = KinematicSub(nIn, m_pnPartOut[nPos], nPrevIn, nPrevOut);
			// m_nOutput이 0인경우 처리 추가. m_nOutput이 0인경우 area_riv(m_nAreaRiv)를 0으로 설정
			nTempArea = m_pnPartOut[nPos] > 0 ? m_nALFA * pow(m_pnPartOut[nPos], 0.6f) : 0;
			m_nYRV1[nData] = (-Bottom_riv + pow((Bottom_riv * Bottom_riv + 4 * m_nBM1 * nTempArea), 0.5f)) / (2 * m_nBM1);
		}

		m_pnPartIn[nPos] = nIn;
		nIn = m_pnPartOut[nPos];
	}

	m_nPrevIn[nData] = m_nInput[nData];
	m_nPrevOut[nData] = m_nOutput[nData];

	return m_nOutput[nData];
}

// TODO (hspark#1#): 계산 결과를 반환하지 않고 무조건 0을 반환하고 있음. 오류 여부 확인 필요
float TLink::Kinematic3(int nData, float nInput)
{
	float nQ = 0;

	if(nInput == 0)
		m_nQMax[nData] = 0;
	else if(nInput > m_nQMax[nData])
		m_nQMax[nData] = nInput;

	float nCK = Celerity(m_nQMax[nData]);

	return nQ;
}

float TLink::Celerity(float nQMax)
{
	float t1, t2, y, ck;
	const float cvt = 1.0f;

	t1 = Routing_N * nQMax;
	t2 = cvt * sqrt(slope_riv) * Bottom_riv;
	y = pow(t1/ t2, 0.6f);
	ck = cvt * sqrt(slope_riv) / Routing_N * 5.0 / 3.0 * pow(y, 2.0f/3.0f);

	return ck;
}

float TLink::Kinematic(int nData, float nInput)
{
	int nLoop; //, nIndex, nPos;
	float nTempArea;

	m_nInput[nData] = nInput;

	for(nLoop = 0; nLoop < DT_loop; nLoop++)
	{
		m_nALFA = m_nALF1 * (1 + 2 * pow((1 + m_nBM1 * m_nBM1), 0.5f)) * m_nYRV1[nData] / Bottom_riv;
		KinematicSub(nData);
		// m_nOutput이 0인경우 처리 추가. m_nOutput이 0인경우 area_riv(m_nAreaRiv)를 0으로 설정
		nTempArea = m_nOutput[nData] > 0 ? m_nALFA * pow(m_nOutput[nData], 0.6f) : 0;
		m_nYRV1[nData] = (-Bottom_riv + pow((Bottom_riv * Bottom_riv + 4 * m_nBM1 * nTempArea), 0.5f)) / (2 * m_nBM1);
	}

	m_nPrevIn[nData] = m_nInput[nData];
	m_nPrevOut[nData] = m_nOutput[nData];

	return m_nOutput[nData];
}

float TLink::KinematicSub(float nIn, float nOut, float nPrevIn, float nPrevOut)
{
	float k = 0;
	float UP, DOWN, C;
	float t;
	float nOutTemp;
	const float csnBeta = 0.6;
	const float csnEPSI = 0.001;

	t = (nOut + nPrevIn) > 0 ? pow(0.5f * (nOut + nPrevIn), csnBeta - 1) : pow(0.000000005f, csnBeta - 1);
	UP = DT / m_nUnitRiv * nPrevOut + m_nALFA * csnBeta * nOut * t;
	DOWN = DT / m_nUnitRiv + m_nALFA * csnBeta * t;
	nOutTemp = UP / DOWN;

	C = DT / m_nUnitRiv * nPrevIn + m_nALFA * csnBeta * nOut;

	for(int l = 0; l < m_nKIMAX; l++)
	{
		float nTemp, FK, F1, F = 0;

		FK = DT / m_nUnitRiv * nOutTemp + m_nALFA * (nOutTemp == 0 ? 0 : pow(nOutTemp, csnBeta)) - C;
		F1 = DT / m_nUnitRiv + m_nALFA * csnBeta * (nOutTemp == 0 ? 0 : pow(nOutTemp, csnBeta - 1));
		nTemp = nOutTemp - (FK / F1);

		k++;

		if(nTemp <= 0)
			nTemp = nOutTemp * 0.001;

		for(int im = 0; im < m_nML; im++)
		{
			float alam = pow(0.5f, im);

			nOut = alam * nTemp + (1 - alam) * nOutTemp;
			F = DT / m_nUnitRiv * nOut + m_nALFA * pow(nOut, csnBeta) - C;

			if(abs(F) <= csnEPSI)
				return nOut;

			if(abs(F) < abs(FK))
				break;
		}

		nOutTemp = nOut;

		if(abs(F) < csnEPSI)
			return nOut;
	}

	return nOut;
}

void TLink::KinematicSub(int nData)
{
	float k = 0;
	float UP, DOWN, C;
	float t;
	float nOutTemp;
	const float csnBeta = 0.6;
	const float csnEPSI = 0.001;

	t = (m_nOutput[nData] + m_nPrevIn[nData]) > 0 ? pow(0.5f * (m_nOutput[nData] + m_nPrevIn[nData]), csnBeta - 1) : pow(0.000000005f, csnBeta - 1);
	UP = DT / length_riv * m_nPrevOut[nData] + m_nALFA * csnBeta * m_nOutput[nData] * t;
	DOWN = DT / length_riv + m_nALFA * csnBeta * t;
	// 이 부분도 DOWN이 0인 경우 발생할 수 있음. 테스트시에 문제 없음.
	nOutTemp = UP / DOWN;

	// route_in_t가 0인 경우 divide zero 발생. 그 경우 값을 0.0000001로 설정하도록 수정
//	C = DT / length_riv * (m_nPrevIn[nData] > 0 ? m_nPrevIn[nData] : 0.0000001f) + m_nALFA * csnBeta * m_nOutput[nData];
	C = DT / length_riv * m_nPrevIn[nData] + m_nALFA * csnBeta * m_nOutput[nData];

	for(int l = 0; l < m_nKIMAX; l++)
	{
		float nTemp, FK, F1, F = 0;

		FK = DT / length_riv * nOutTemp + m_nALFA * pow(nOutTemp, csnBeta) - C;
		F1 = DT / length_riv + m_nALFA * csnBeta * pow(nOutTemp, csnBeta - 1);
		nTemp = nOutTemp - (FK / F1);

		k++;

		if(nTemp <= 0)
			nTemp = nOutTemp * 0.001;

		for(int im = 0; im < m_nML; im++)
		{
			float alam = pow(0.5f, im);

			m_nOutput[nData] = alam * nTemp + (1 - alam) * nOutTemp;
			// 이 부분도 오류발생 가능성 높음. 테스트시 문제는 발생하지 않음.
			F = DT / length_riv * m_nOutput[nData] + m_nALFA * pow(m_nOutput[nData], csnBeta) - C;

			if(abs(F) <= csnEPSI)
				return;

			if(abs(F) < abs(FK))
				break;
		}

		nOutTemp = m_nOutput[nData];

		if(abs(F) < csnEPSI)
			return;
	}
}

BOOL TLink::SetNode(TBaseNode *pStart, TBaseNode *pEnd)
{
	if(pStart->CanOutput() != NODE_ALLOWNONE)
		m_pStart = pStart;
	else
		return FALSE;

//	if(pEnd->CanAccept() != NODE_ALLOWNONE || pStart->GetType() == NODE_IMPORT || pEnd->GetType() == NODE_WETLAND)
	if(pEnd->CanAccept() != NODE_ALLOWNONE || pStart->GetType() == NODE_IMPORT)
		m_pEnd = pEnd;
	else
		return FALSE;

	m_nStartID = m_pStart->GetID();
	m_nEndID = m_pEnd->GetID();

	pStart->SetLink(this);

	return TRUE;
}

int TLink::ValidateCheck(void)
{
	switch(Method)
	{
	case 1:
		if(DT == 0 || K == 0 || X == 0)
			return -1;
		break;
	case 2:
		if(length_riv == 0 || slope_riv == 0 || Routing_N == 0 || Top_riv == 0)
			return -1;
		break;
	case 3:
		if(length_riv == 0 || slope_riv == 0 || Top_riv == 0 || Bottom_riv == 0 || Routing_N == 0 || depth_riv == 0 || Top_riv == 0 || Bottom_riv == 0)
			return -1;
		break;
	}

	return 0;
}

int TLink::LoadText(FILE* fp)
{
	float nVals[20];
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
		strncpy(szToken, szLine, (cFind - &szLine[0]));
		strcpy(szToken2, cFind + 1);
		Trim(szToken2);
		Trim(szToken);

		if(strcmp(szToken, "NodeID") == 0)
			SetID(atol(szToken2));
		else if(strcmp(szToken, "Name") == 0)
			SetName(szToken2);
		else if(strcmp(szToken, "Desc") == 0)
			SetDesc(szToken2);
		else if(strcmp(szToken, "Method") == 0)
			Method = atol(szToken2);
		else if(strcmp(szToken, "Muskingum") == 0)
		{
			ReadValue(szToken2, nVals, 3);

			DT = nVals[0];
			X = nVals[1];
			K = nVals[2];
		}
		else if(strcmp(szToken, "Cunge") == 0)
		{
			ReadValue(szToken2, nVals, 5);

			Delta_dis = nVals[0];
			Routing_So = nVals[1];
			Routing_N = nVals[2];
			Routing_B = nVals[3];
			Peak_t = nVals[4];
		}
		else if(strcmp(szToken, "Kinematic") == 0)
		{
			ReadValue(szToken2, nVals, 7);

			mann = nVals[0];
			slope_riv = nVals[1];
			length_riv = nVals[2];
			Bottom_riv = nVals[3];
			Top_riv = nVals[4];
			depth_riv = nVals[5];
			init_route = nVals[6];
		}
		else if(strcmp(szToken, "Connect") == 0)
		{
			ReadValue(szToken2, nVals, 2);

			m_nStartID = (int)nVals[0];
			m_nEndID = (int)nVals[1];
		}
	}
	return 0;
}
