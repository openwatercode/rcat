#include "StdAfx.h"
#include "include/BaseNode.h"
#include "include/Link.h"
#include "include/Evaporation.h"
#include <math.h>
#include <algorithm>

using namespace std;

TBaseNode::TBaseNode(void)
{
	m_pLink = NULL;
	m_nID = 0;
	m_nType = 0;
	memset(m_szDefName, 0, sizeof(m_szDefName));
	memset(m_szDesc, 0, sizeof(m_szDesc));
	memset(m_szName, 0, sizeof(m_szName));
	m_nCanAccept = NODE_ALLOWSINGLE;
	m_nCanOutput = NODE_ALLOWSINGLE;
	m_pResult = NULL;
	m_nSf = m_nGW = m_nInter = m_nInflow = 0;
	m_pDate = NULL;
}

TBaseNode::~TBaseNode(void)
{
}

void TBaseNode::SetID(int nID)
{
	m_nID = nID;
}

void TBaseNode::Clear()
{
}

void TBaseNode::Init()
{
	m_nInflow = 0;
	m_nImport = 0;
	m_nSf = m_nGW = m_nInter = 0;
	m_nImport = m_nLeakage = 0;
}

void TBaseNode::SetName(char* szName)
{
	strcpy_s(m_szName, sizeof(m_szName), szName);
}

void TBaseNode::SetDesc(char *szDesc)
{
	strcpy_s(m_szDesc, sizeof(m_szDesc), szDesc);
}

void TBaseNode::SetName(wchar_t* szName)
{
	char cszName[200];

	memset(cszName, 0, sizeof(cszName));
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, szName, wcslen(szName), cszName, 200, NULL, NULL);
	strcpy_s(m_szName, 100, cszName);
}

void TBaseNode::SetDesc(wchar_t* szDesc)
{
	char cszDesc[400];

	memset(cszDesc, 0, sizeof(cszDesc));
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, szDesc, wcslen(szDesc), cszDesc, 400, NULL, NULL);
	strcpy_s(m_szDesc, 100, cszDesc);
}

void TBaseNode::SetDefName(int nNo)
{
	sprintf_s(m_szName, 20, "%s %d", m_szDefName, nNo);
}

TBaseNode* TBaseNode::GetOutput()
{
	if(m_pLink)
		return m_pLink->GetEnd();

	return NULL;
}

void TBaseNode::operator =(TBaseNode &node)
{
	m_nType = node.m_nType;
	m_nCanAccept = node.m_nCanAccept;
	m_nCanOutput = node.m_nCanOutput;
	memcpy(m_szDefName, node.m_szDefName, sizeof(m_szDefName));
	memcpy(m_szDesc, node.m_szDesc, sizeof(m_szDesc));
	memset(m_szName, 0, sizeof(m_szName));
}

void TBaseNode::AddValue(int nIndex, float nSf, float nGW, float nInter, float nTotal)
{
	m_nInflow += nTotal;
	m_nSf += nSf;
	m_nGW += nGW;
	m_nInter += nInter;
}

void TBaseNode::AddImport(int nIndex, float nImport, float nLeakage)
{
	if(GetMajorType() == NODE_SOURCE)
		m_nLeakage += (nImport * nLeakage);

	m_nImport += (nImport - m_nLeakage);
}

float TBaseNode::CalcKU(float theta_t, float theta_s, float theta_r, int mualem)
{
	float result;

	if(theta_t > theta_s)
		result = 1;
	else if(theta_t > theta_r)
		result = pow((theta_t - theta_r) / (theta_s - theta_r), mualem);
	else
		result = 0;

	return result;

}

float TBaseNode::CalcAETSwat(float nPev, float nLai, float covsol, float nETo, float nPlantRatio, float soil_th, float soil_theta, float soil_FC, float soil_W)
{
	#define VU10 0.0000000001f
	float temp;
	float Cov;
	float ET_t, ET_s;
	float nAET;

	ET_t = nETo;

	ET_t *= nPlantRatio;
	Cov = exp(-0.00005f * covsol);
	ET_s = nETo * Cov;					// m_nET_s 로 되어 있는것 ET_s인것 같아 변경.
	temp = ET_s * (nETo / ((ET_s) + ET_t + VU10)); // 확인필요
	ET_s = min(ET_s, temp); // 확인필요
	ET_s = max(ET_s, 0.0f);

	if(nETo < (ET_s + ET_t))
	{
		ET_s = nPev * ET_s / (ET_s + ET_t);
		ET_t = nPev * ET_t / (ET_s + ET_t);
	}

	if(soil_theta < soil_FC)
	{
		ET_s = ET_s * exp(2.5f * (soil_theta - soil_FC) / (soil_FC - soil_W));
	}

	ET_s = min(ET_s, 0.8f * (soil_theta - soil_W) * soil_th);
	nAET = ET_t + ET_s;

	return nAET;
}

TClimate::TClimate(void)
{
	m_nType = NODE_CLIMATE;
}

TClimate::~TClimate(void)
{
}

void TClimate::operator =(TBaseNode &node)
{
	TClimate *pClimate = (TClimate*)(&node);

	m_bUseCalc = pClimate->m_bUseCalc;
	m_nHeight = pClimate->m_nHeight;
	m_nLat = pClimate->m_nLat;
	m_nWindHeight = pClimate->m_nWindHeight;
	m_sClimate.Clear();
	m_sClimateSrc;
	memcpy(m_szClimate, pClimate->m_szClimate, sizeof(m_szClimate));
	memcpy(m_szEva, pClimate->m_szEva, sizeof(m_szEva));

	LoadSeries();

	TBaseNode::operator =(node);
}

void TClimate::SetModelEnv(unsigned long nStart, unsigned long nEnd, unsigned short nInterval)
{
	TSeriesItem *pNew = NULL;

	// 계산용 기상 자료를 초기화
	m_sClimate.Clear();
	//memcpy_s(&(m_sClimate.m_Header), sizeof(SERIESHEADER), &(m_sClimateSrc.m_Header), sizeof(SERIESHEADER));
	memcpy(&(m_sClimate.m_Header), &(m_sClimateSrc.m_Header), sizeof(SERIESHEADER));

	// 원본 기상 자료에서 강우 자료가 있으면 모형 실행기간 만큼 복사한다
	if(m_sClimateSrc.FindSeries(FT_PRECIP))
	{
		pNew = new TSeriesItem;
		m_sClimateSrc.FindSeries(FT_PRECIP)->CopyPart(nStart, nEnd, pNew);
		m_sClimate.AddSeries(pNew);
	}

	// 원본 기상자료에서 계산된 증발산 자료가 있으면 모형실행기간 만큼 복사하고
	// 그렇지 않고 입력된 증발산 자료가 있으면 이를 복사한다.
	if(m_sClimateSrc.FindSeries(FT_EVACALC))
	{
		pNew = new TSeriesItem;
		m_sClimateSrc.FindSeries(FT_EVACALC)->CopyPart(nStart, nEnd, pNew);
		m_sClimate.AddSeries(pNew);
	}
	else if(m_sClimateSrc.FindSeries(FT_EVA))
	{
		pNew = new TSeriesItem;
		m_sClimateSrc.FindSeries(FT_EVA)->CopyPart(nStart, nEnd, pNew);
		m_sClimate.AddSeries(pNew);
	}

	// 생성된 자료를 계산용 기상자료로 설정한다.
	if(pNew)
	{
		m_sClimate.m_Header.nColumn = 2;
		m_sClimate.m_Header.nInterval = pNew->m_Header.nInterval;
		m_sClimate.m_dtStart = pNew->m_Header.date;
		m_sClimate.m_Header.nCount = pNew->GetCount();
	}

	// 계산 시간 간격으로 변환한다.
	m_sClimate.ChangeIntervalByDefault(nInterval);
}

void TClimate::SetEvaFileA(char* szFile)
{
	strcpy_s(m_szEva, MAX_PATH, szFile);
}

void TClimate::SetEvaFileW(wchar_t* szFile)
{
	char cszPath[MAX_PATH * 2];

	memset(cszPath, 0, sizeof(cszPath));
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, szFile, wcslen(szFile), cszPath, MAX_PATH * 2, NULL, NULL);
	strcpy_s(m_szEva, MAX_PATH, cszPath);
}

void TClimate::SetEvaFile(LPCTSTR szFile)
{
#ifdef _UNICODE
	char cszPath[MAX_PATH * 2];

	memset(cszPath, 0, sizeof(cszPath));
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, szFile, lstrlen(szFile), cszPath, MAX_PATH * 2, NULL, NULL);
	strcpy_s(m_szEva, MAX_PATH, cszPath);
#else
	strcpy_s(m_szEva, MAX_PATH, szFile);
#endif
}

void TClimate::SetClimateFileA(char* szFile)
{
	strcpy_s(m_szClimate, MAX_PATH, szFile);
}

void TClimate::SetClimateFileW(wchar_t* szFile)
{
	char cszPath[MAX_PATH * 2];

	memset(cszPath, 0, sizeof(cszPath));
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, szFile, wcslen(szFile), cszPath, MAX_PATH * 2, NULL, NULL);
	strcpy_s(m_szClimate, MAX_PATH, cszPath);
}

void TClimate::SetClimateFile(LPCTSTR szFile)
{
#ifdef _UNICODE
	char cszPath[MAX_PATH * 2];

	memset(cszPath, 0, sizeof(cszPath));
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, szFile, lstrlen(szFile), cszPath, MAX_PATH * 2, NULL, NULL);
	strcpy_s(m_szClimate, MAX_PATH, cszPath);
#else
	strcpy_s(m_szClimate, MAX_PATH, szFile);
#endif
}


// 기상자료 및 증발산 자료 파일을 읽어온다.
int TClimate::LoadSeries(void)
{
	char szClimate[MAX_PATH], szEva[MAX_PATH];

	memset(szClimate, 0, sizeof(szClimate));
	memset(szEva, 0, sizeof(szEva));

#ifdef _UNICODE
	MultiByteToWideChar(CP_ACP, 0, m_szClimate, (int)strlen(m_szClimate), szClimate, MAX_PATH);
	MultiByteToWideChar(CP_ACP, 0, m_szEva, (int)strlen(m_szEva), szEva, MAX_PATH);
#else
	strcpy_s(szClimate, MAX_PATH, m_szClimate);
	strcpy_s(szEva, MAX_PATH, m_szEva);
#endif

	m_sClimate.Clear();
	m_sClimateSrc.Clear();

	if(lstrlen(szClimate) > 0)
	{
		TSerieses series;
		int nRet;

		nRet = series.Load(szClimate);
		if(nRet == 9)
		{
			if(MessageBox(NULL, "Convert previous data file?", "Convert", MB_YESNO) == IDYES)
//			if(AfxMessageBox(_T("Convert previous data file?"), MB_YESNO) == IDYES)
				series.Save(NULL);
			nRet = 0;
		}

		if(nRet == 0)
			series.GetAt(0)->Copy(&m_sClimateSrc);

		m_sClimateSrc.RemoveSeries(FT_EVACALC);

		if(m_bUseCalc)
		{
			int nWantType = FT_RHUMI | FT_WIND | FT_TEMPAVG | FT_SOLAR;
			int nWantType2 = FT_RHUMI | FT_WIND | FT_SOLAR | FT_TEMPMIN | FT_TEMPMAX;

			if((m_sClimate.m_Header.nData & nWantType) != nWantType && (m_sClimate.m_Header.nData & nWantType2) != nWantType2)
			{
				m_bUseCalc = FALSE;
				return  -2;
			}

			TEvaporation *pItem = new TEvaporation;
			pItem->m_nElev = m_nHeight;
			pItem->m_nLatitude = m_nLat;
			pItem->m_nDepWind = m_nWindHeight;
			pItem->CalcEvap(&m_sClimateSrc);
			pItem->ConvertInterval(m_sClimateSrc.m_Header.nInterval, TC_SUM);
			m_sClimateSrc.AddSeries(pItem);
		}
	}

	if(!m_bUseCalc && lstrlen(szEva) > 0)
	{
		TSerieses serEva;

		if(!serEva.Load(szEva))
		{
			memset(m_szEva, 0, sizeof(m_szEva));
			return -3;
		}

		if(serEva.GetCount() > 0 && serEva.GetAt(0)->FindSeries(FT_EVA) == NULL)
		{
			memset(m_szEva, 0, sizeof(m_szEva));
			return -4;
		}

		m_sClimateSrc.RemoveSeries(FT_EVA);

		TSeriesItem *pItem = new TSeriesItem;
		serEva.GetAt(0)->FindSeries(FT_EVA)->Copy(pItem);
		if(pItem->m_Header.nInterval != m_sClimateSrc.m_Header.nInterval)
			pItem->ConvertInterval(m_sClimateSrc.m_Header.nInterval, TC_SUM);
		m_sClimateSrc.AddSeries(pItem);
	}

	return 0;
}

float TBaseNode::ConvertDay2Step(float nVal)
{
	return nVal * m_nDT / (24 * 3600);
}

int TBaseNode::ReadValue(char *szLine, float *pVals, int nCount)
{
	char *cFind;
	int nIndex;

	cFind = &szLine[0];

	for(nIndex = 0; nIndex < nCount; nIndex++)
	{
	  //sscanf_s(cFind, "%f", &pVals[nIndex]);
	  sscanf(cFind, "%f", &pVals[nIndex]);
	  cFind = strchr(cFind, ',');
		if(cFind)
			cFind++;
		else
			break;
	}

	return nIndex;
}

BOOL TBaseNode::IsWhiteChar(char c)
{
	if(c == ' ' || c == '\n' || c == '\r')
		return TRUE;

	return FALSE;
}

void TBaseNode::Trim(char *szToken)
{
	char szTemp[1024];
	char *pc = &szToken[0];
	int nPos;

	while(IsWhiteChar(*pc) && *pc != '\0')
		pc++;
	//strcpy_s(szTemp, pc);
	strcpy(szTemp, pc);

	nPos = strlen(szTemp) - 1;
	while(IsWhiteChar(szTemp[nPos]) && nPos >= 0)
	{
		szTemp[nPos] = '\0';
		nPos--;
	}

	strcpy_s(szToken, strlen(szTemp) + 1, (const char*)szTemp);
}

int TClimate::LoadText(FILE* fp)
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
		else if(strcmp(szToken, "Rainfall") == 0)
		{
			SetClimateFileA(szToken2);
		}
		else if(strcmp(szToken, "Evaporation") == 0)
		{
			SetEvaFileA(szToken2);
		}
		else if(strcmp(szToken, "Calculation") == 0)
		{
			ReadValue(szToken2, nVals, 4);
			m_bUseCalc = (int)nVals[0];
			m_nLat = nVals[1];
			m_nHeight = nVals[2];
			m_nWindHeight = nVals[3];
		}
		else
			ExtraFileContent(szLine);
	}

	return 1;
}

BOOL TBaseNode::IsFileExist(char* szFile)
{
	WIN32_FIND_DATA find;
	BOOL bFind;
	HANDLE hFind;

	ZeroMemory(&find, sizeof(find));
	hFind = FindFirstFile(szFile, &find);

	if(hFind == INVALID_HANDLE_VALUE)
		bFind = FALSE;
	else
	{
		FindClose(hFind);
		bFind = TRUE;
	}

	return bFind;
}
