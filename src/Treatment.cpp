#include "StdAfx.h"
#include "include/Treatment.h"
#include <math.h>
#include <stdio.h>
#include "include/Link.h"
#include "algorithm"

using namespace std;

// 중력가속도 m/s2

#define G 9.80665

TImport::TImport(void)
{
	m_nType = NODE_IMPORT;
	Clear();
}

TImport::~TImport(void)
{
}

void TImport::Clear()
{
	m_pfCalc = &TImport::CalcConst;
	type = 0;
	m_nConst = 10000;
	m_pData = NULL;
	m_nOut = 0;
	m_nLeakage = 0;
	memset(m_szSeries, 0, sizeof(m_szSeries));
	m_nTable = 0;
	m_nData = 0;
}

void TImport::Init()
{
	TBaseNode::Init();

	if(GetOutput())
	{
		if(type == 0)
		{
			m_nConst_dt = ConvertDay2Step(m_nConst);
			m_pfCalc = &TImport::CalcConst;
			m_nOut = m_nConst_dt;
		}
		else
		{
			if(m_Series.GetCount() > m_nTable)
			{
				if(m_Series.GetAt(m_nTable)->GetCount() > m_nData)
				{
					m_pfCalc = &TImport::CalcSeries;
					m_pData = m_Series.GetAt(m_nTable)->GetAt(m_nData);
					m_pData->SetFirstPos(0);
				}
				else
				{
					m_pData = NULL;
					m_pfCalc = &TImport::CalcNone;
				}
			}
			else
			{
				m_pData = NULL;
				m_pfCalc = &TImport::CalcNone;
			}
		}
	}
	else
		m_pfCalc = &TImport::CalcNone;
}

void TImport::InitResult(unsigned long nTime, int nInterval, int nCount)
{
	m_pResult->Clear();
	m_pResult->m_dtStart = nTime;
	m_pResult->m_Header.nInterval = nInterval;
	m_pResult->m_Header.nData = FT_USER;
	m_pResult->m_Header.nCount = nCount;

	TSeriesItem *pItem = m_pResult->AddItem();
	strcpy_s(pItem->m_Header.szHeader, 50, "Water Supply(㎥)");
	pItem->m_Header.date = nTime;
	pItem->m_Header.nInterval = nInterval;
	pItem->SetSize(nCount);
}

void TImport::SetSeriesFileA(char* szFile)
{
	strcpy_s(m_szSeries, MAX_PATH, szFile);
}

void TImport::SetSeriesFileW(wchar_t* szFile)
{
	char cszPath[MAX_PATH * 2];

	memset(cszPath, 0, sizeof(cszPath));
	#ifndef WINRLIB
	wcstombs(cszPath, szFile, MAX_PATH * 2);
	#else
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, szFile, wcslen(szFile), cszPath, MAX_PATH * 2, NULL, NULL);
	#endif // WINRLIB
	strcpy_s(m_szSeries, MAX_PATH, cszPath);
}

void TImport::LoadSeries(void)
{
	TCHAR szSeries[MAX_PATH];

	memset(szSeries, 0, sizeof(szSeries));

#ifdef _UNICODE
	MultiByteToWideChar(CP_ACP, 0, m_szSeries, (int)strlen(m_szSeries), szSeries, MAX_PATH);
#else
	strcpy_s(szSeries, MAX_PATH, m_szSeries);
#endif

	m_Series.Load(szSeries);

}

void TImport::Calculate(int nStep)
{
	(this->*m_pfCalc)(nStep);
	if(m_pLink)
		m_pLink->AddImport(nStep, m_nOut, m_nLeakage);

	m_pResult->GetAt(0)->AddValue(nStep, m_nOut);
	m_nInflow = m_nSf = m_nGW = m_nInter = 0;
}

void TImport::CalcConst(int nStep)
{
}

void TImport::CalcSeries(int nStep)
{
	m_nOut = m_pData->GetNext();
}

TInfiltro::TInfiltro(void)
{
	m_nType = NODE_INFILTRO;
	Clear();
}

TInfiltro::~TInfiltro(void)
{
}

void TInfiltro::Clear(void)
{
	in_node_id = 0;
	area = 1000;
	potential = 200;

	surf_inf = 0;
	surf_out = 0;
	aqf_cap = 0;
	aqf_top = 35 * 1000;
	aqf_bot = -35 * 1000;
	aqf_S_coef = 0.5;
	gwE = 0;
	infiltro_inf = 0;
	m_pInNode = NULL;

	gw_node_id = 0;
	m_pGWMove = NULL;
	slope_aqf = 0;
	node_len = 0;
	conn_len = 0;
	Kgw = 0;
}

void TInfiltro::Init(void)
{
	TBaseNode::Init();
	potential_L = ConvertDay2Step(potential * 1000 / area);
}

void TInfiltro::InitResult(unsigned long nTime, int nInterval, int nCount)
{
	const char* pszItemHeader[] = {"Inflow(㎥)", "Import(㎥)", "Infiltrate(㎥)", "Outflow(㎥)"};

	m_pResult->Clear();
	m_pResult->m_dtStart = nTime;
	m_pResult->m_Header.nInterval = nInterval;
	m_pResult->m_Header.nData = FT_USER;
	m_pResult->m_Header.nCount = nCount;

	for(int nCol = 0; nCol < (int)(sizeof(pszItemHeader) / sizeof(char*)); nCol++)
	{
		TSeriesItem *pItem = m_pResult->AddItem();
		strcpy_s(pItem->m_Header.szHeader, 50, pszItemHeader[nCol]);
		pItem->m_Header.date = nTime;
		pItem->m_Header.nInterval = nInterval;
		pItem->SetSize(nCount);
	}
}

void TInfiltro::Calculate(int nStep)
{
	TUrban *pUrban = dynamic_cast<TUrban*>(m_pInNode);

	// 지표면 유입량
	surf_inf = (m_nInflow + m_nImport) * 1000 / area; // 단위를 mm로 변경.
	surf_out = 0;

	// 설계침투량 단위 환산. 가능량 산정
	aqf_cap = (pUrban->aqf_Top - pUrban->gwE_t) * pUrban->aqf_S;

	// 저류가능량 확인
	if(aqf_cap < 0)
	{
		surf_out = surf_inf;
		infiltro_inf = 0;
	}
	else if(surf_inf > potential_L)
	{
		surf_out = surf_inf - potential_L;
		infiltro_inf = potential_L;
	}
	else
	{
		surf_out = 0;
		infiltro_inf = surf_inf;
	}

	// 침투량 - 가능량 비교
	if(infiltro_inf > aqf_cap)
	{
		surf_out += (infiltro_inf - aqf_cap);
		infiltro_inf = aqf_cap;
		aqf_cap = 0;
	}
	else
		aqf_cap -= infiltro_inf;

	if(m_pGWMove)
	{
		m_pGWMove->AddImport(nStep, infiltro_inf * area / 1000, 0);
	}
	else
	{
		pUrban->AddRecharge(infiltro_inf * area / 1000);
	}

	// 지표유출량 m3로 환산
	float surf_out_t = surf_out * area / 1000;

	if(m_pLink)
		m_pLink->Calculate(nStep, surf_out_t, 0, 0, surf_out_t);

	AddResult(nStep);
	AddBalance();
	m_nImport = m_nInflow = m_nSf = m_nGW = m_nInter = m_nLeakage = 0;
}

void TInfiltro::AddBalance(void)
{
	TNodeYear *pYear = m_pBalance->FindOrCreateNode(m_nID, m_nType)->FindOrCreateYear(m_pDate->nYear, 0, 0);
	pYear->Add(0, m_nInflow + m_nImport, 0, 0, surf_out * area / 1000, 0, 0, 0, 0, 0, 0, 0, infiltro_inf * area / 1000);
	pYear->Calc(1, 1, 1);
}

void TInfiltro::AddResult(int nStep)
{
	//m_pResult->GetAt(0)->AddValue(nStep, (m_nInflow + m_nImport));
	m_pResult->GetAt(0)->AddValue(nStep, m_nInflow);
	m_pResult->GetAt(1)->AddValue(nStep, m_nImport);
	m_pResult->GetAt(2)->AddValue(nStep, infiltro_inf * area / 1000);
	m_pResult->GetAt(3)->AddValue(nStep, surf_out * area / 1000);
}

TBioRetention::TBioRetention(void)
{
	m_nType = NODE_BIORETENTION;
	Clear();
}

TBioRetention::~TBioRetention(void)
{
}

void TBioRetention::Clear(void)
{
	TInfiltro::Clear();

	memset(&m_Climates, 0, sizeof(m_Climates));
	m_Climates[0].nRain = 100;
	m_Climates[1].nEva = 100;
	m_nClimate = 0;
	m_pPev = NULL;
	m_pRain = NULL;

	LAI[0] = LAI[1] = LAI[2] = LAI[3] = LAI[4] = LAI[5] = LAI[6] = LAI[7] = LAI[8] = LAI[9] = LAI[10] = LAI[11] = 0.7;
}

void TBioRetention::Init(void)
{
	TInfiltro::Init();
	surf_ev = 0;
	m_pRain = m_sClimate.FindSeries(FT_PRECIP);
	m_pPev = m_sClimate.FindSeries(FT_EVA);
}

void TBioRetention::InitResult(unsigned long nTime, int nInterval, int nCount)
{
	const char* pszItemHeader[] = {"Inflow(㎥)", "Rainfall(mm)", "Total Inflow(㎥)", "Evapotranspiration(mm)", "Infiltrate(㎥)", "Outflow(㎥)"};

	m_pResult->Clear();
	m_pResult->m_dtStart = nTime;
	m_pResult->m_Header.nInterval = nInterval;
	m_pResult->m_Header.nData = FT_USER;
	m_pResult->m_Header.nCount = nCount;

	for(int nCol = 0; nCol < (int)(sizeof(pszItemHeader) / sizeof(char*)); nCol++)
	{
		TSeriesItem *pItem = m_pResult->AddItem();
		strcpy_s(pItem->m_Header.szHeader, 50, pszItemHeader[nCol]);
		pItem->m_Header.date = nTime;
		pItem->m_Header.nInterval = nInterval;
		pItem->SetSize(nCount);
	}
}

void TBioRetention::AddBalance()
{
	TNodeYear *pYear = m_pBalance->FindOrCreateNode(m_nID, m_nType)->FindOrCreateYear(m_pDate->nYear, 0, 0);
	pYear->Add(m_nRain * area / 1000, m_nInflow + m_nImport, surf_ev * area / 1000, 0, surf_out * area / 1000, 0, 0, 0, 0, 0, 0, 0, infiltro_inf * area / 1000);
	pYear->Calc(1, 1, 1);
}

void TBioRetention::AddResult(int nStep)
{
	m_pResult->GetAt(0)->AddValue(nStep, (m_nInflow + m_nImport));
	m_pResult->GetAt(1)->AddValue(nStep, m_nRain);
	m_pResult->GetAt(2)->AddValue(nStep, surf_inf * area / 1000);
	m_pResult->GetAt(3)->AddValue(nStep, surf_ev);
	m_pResult->GetAt(4)->AddValue(nStep, infiltro_inf * area / 1000);
	m_pResult->GetAt(5)->AddValue(nStep, surf_out * area / 1000);
}

void TBioRetention::Calculate(int nStep)
{
	float aev, inf;
	TUrban *pUrban = dynamic_cast<TUrban*>(m_pInNode);

	m_nRain = m_pRain ? m_pRain->GetNext() : 0.0f;
	m_nPev = m_pPev ? m_pPev->GetNext() : 0.0f;

	// 지표면 유입량
	surf_inf = (m_nInflow + m_nImport) * 1000 / area + m_nRain; // 단위를 mm로 변경.
	surf_out = 0;

	aev = m_nPev * GetLAI();
	if(surf_inf > aev)
	{
		surf_ev = aev;
		inf = surf_inf - surf_ev;
	}
	else
	{
		surf_ev = surf_inf;
		inf = 0;
	}

	// 설계침투량 단위 환산. 가능량 산정
	aqf_cap = (pUrban->aqf_Top - pUrban->gwE_t) * pUrban->aqf_S;

	// 저류가능량 확인
	if(aqf_cap < 0)
	{
		surf_out = inf;
		infiltro_inf = 0;
	}
	else if(inf > potential_L)
	{
		surf_out = inf - potential_L;
		infiltro_inf = potential_L;
	}
	else
	{
		surf_out = 0;
		infiltro_inf = inf;
	}

	// 침투량 - 가능량 비교
	if(infiltro_inf > aqf_cap)
	{
		surf_out += (infiltro_inf - aqf_cap);
		infiltro_inf = aqf_cap;
		aqf_cap = 0;
	}
	else
		aqf_cap -= infiltro_inf;

	if(m_pGWMove)
	{
		m_pGWMove->AddImport(nStep, infiltro_inf * area / 1000, 0);
	}
	else
	{
		pUrban->AddRecharge(infiltro_inf * area / 1000);
	}

	// 지표유출량 m3로 환산
	float surf_out_t = surf_out * area / 1000;

	if(m_pLink)
		m_pLink->Calculate(nStep, surf_out_t, 0, 0, surf_out_t);

	AddResult(nStep);
	AddBalance();

	m_nImport = m_nInflow = m_nSf = m_nGW = m_nInter = m_nLeakage = 0;
}

float TBioRetention::GetLAI(void)
{
	return LAI[m_pDate->GetMonth() - 1];
}

TWetLand::TWetLand()
{
	m_nType = NODE_WETLAND;
	recharge_id = -1;
	m_pRechargeNode = NULL;
	Clear();
}

TWetLand::~TWetLand()
{
}

void TWetLand::Clear()
{
	TBaseNode::Clear();

	vol_init = 10000;
	vol_max = 100000;
	bypass = 1;
	aqf_ks = 0.0005;
	pipe_ht = 1000;
	pipe_area = 0.5;
	pipe_coef = 0.5;
	rate_count = 0;
	memset(wl_rate, 0, sizeof(wl_rate));
	memset(m_Climates, 0, sizeof(m_Climates));
	m_Climates[0].nRain = 100;
}

void TWetLand::Init()
{
	TBaseNode::Init();
	area = wl = 0;
	vol = vol_init;
	m_pRain = m_sClimate.FindSeries(FT_PRECIP);
	m_pPev = m_sClimate.FindSeries(FT_EVA);
	bypass_step = ConvertDay2Step(bypass * 86400);

	const int AREA = 2;
	fullArea = 0;
	for(int nIndex = 0; nIndex < rate_count; nIndex++)
	{
		if(wl_rate[nIndex][AREA] > fullArea)
			fullArea = wl_rate[nIndex][AREA];
	}
}

void TWetLand::InitResult(unsigned long nTime, int nInterval, int nCount)
{
	const char* pszItemHeader[] = {"Inflow(㎥)", "Rainfall(mm)", "Evaporation Water surface(mm)", "Bypass Volume(㎥)", "Spillway Outflow(㎥)",
								"Pipe Outflow(㎥)", "Aquifer Loss(㎥)", "Volume(㎥)", "Water Level(m)"};

	m_pResult->Clear();
	m_pResult->m_dtStart = nTime;
	m_pResult->m_Header.nInterval = nInterval;
	m_pResult->m_Header.nData = FT_USER;
	m_pResult->m_Header.nCount = nCount;

	for(int nCol = 0; nCol < (int)(sizeof(pszItemHeader) / sizeof(char*)); nCol++)
	{
		TSeriesItem *pItem = m_pResult->AddItem();
		strcpy_s(pItem->m_Header.szHeader, 50, pszItemHeader[nCol]);
		pItem->m_Header.date = nTime;
		pItem->m_Header.nInterval = nInterval;
		pItem->SetSize(nCount);
	}
}

void TWetLand::AddBalance(void)
{
	TNodeYear *pYear = m_pBalance->FindOrCreateNode(m_nID, m_nType)->FindOrCreateYear(m_pDate->nYear, vol, 0);
	pYear->Add(m_nRain * fullArea / 1000, surf_inf, surf_evV, 0, bypass_out, spill_out, pipe_out, 0, 0, vol, 0, 0, aqf_loss);
	pYear->Calc(1, 1, 1);
}

void TWetLand::AddResult(int nStep)
{
	m_pResult->GetAt(0)->AddValue(nStep, surf_inf);
	m_pResult->GetAt(1)->AddValue(nStep, m_nRain);
	m_pResult->GetAt(2)->AddValue(nStep, surf_ev);
	m_pResult->GetAt(3)->AddValue(nStep, bypass_out);
	m_pResult->GetAt(4)->AddValue(nStep, spill_out);
	m_pResult->GetAt(5)->AddValue(nStep, pipe_out);
	m_pResult->GetAt(6)->AddValue(nStep, aqf_loss);
	m_pResult->GetAt(7)->AddValue(nStep, vol);
	m_pResult->GetAt(8)->AddValue(nStep, wl / 1000);
}

void TWetLand::Calculate(int nStep)
{
	m_nRain = m_pRain ? m_pRain->GetNext() : 0.0f;
	m_nPev = m_pPev ? m_pPev->GetNext() : 0.0f;

	surf_inf = m_nInflow + m_nImport;// + (m_nRain * fullArea / 1000);
	m_pBalance->FindOrCreateNode(m_nID, m_nType)->FindOrCreateYear(m_pDate->nYear, vol, 0);

	if(surf_inf > bypass_step)
	{
		bypass_out = surf_inf - bypass_step;
		vol += bypass_step;
	}
	else
	{
		bypass_out = 0;
		vol += surf_inf;
	}

	if(vol > vol_max)
	{
		spill_out = vol - vol_max;
		vol = vol_max;
	}
	else
		spill_out = 0;

	CalcRate(vol, wl, area);

	// 수면 증발량
	surf_ev = m_nPev;
	surf_evV = surf_ev * area / 1000;

	// 방류구 방류량
	pipe_out = wl > pipe_ht ? pipe_coef * pipe_area * sqrt((wl - pipe_ht) / 1000) : 0;

	aqf_loss = aqf_ks * area *1.3f;
	sur_rain = m_nRain * fullArea / 1000;
	if(m_pRechargeNode)
		m_pRechargeNode->AddRecharge(aqf_loss);

	out_flow = bypass_out + spill_out + pipe_out;
	vol += (sur_rain - surf_evV - pipe_out - aqf_loss);

	if(m_pLink)
		m_pLink->Calculate(nStep, out_flow, 0, 0, out_flow);

	AddResult(nStep);
	AddBalance();

	m_nInflow = m_nSf = m_nGW = m_nInter = m_nLeakage = 0;
}

void TWetLand::CalcRate(float in_vol, float &out_wl, float &out_area)
{
	const int EL = 0;
	const int VOL = 1;
	const int AREA = 2;
	int nIndex;

	if(in_vol < wl_rate[0][VOL])
	{
		if(wl_rate[0][VOL] > 0)
		{
			out_wl = wl_rate[0][EL] * in_vol / wl_rate[0][VOL];
			out_area = wl_rate[0][AREA] * in_vol / wl_rate[0][VOL];
		}
		else
			out_wl = out_area = 0;
	}
	else if(in_vol > wl_rate[rate_count - 1][VOL])
	{
		out_wl = wl_rate[rate_count - 1][EL] + (in_vol - wl_rate[rate_count - 1][VOL]) / wl_rate[rate_count - 1][AREA];
		out_area = wl_rate[rate_count - 1][AREA];
	}
	else
	{
		for(nIndex = 0; nIndex < rate_count - 1; nIndex++)
		{
			if(wl_rate[nIndex][VOL] <= in_vol && in_vol <= wl_rate[nIndex + 1][VOL])
			{
				out_wl = wl_rate[nIndex][EL] + (wl_rate[nIndex + 1][EL] - wl_rate[nIndex][EL]) * (in_vol - wl_rate[nIndex][VOL]) / (wl_rate[nIndex + 1][VOL] - wl_rate[nIndex][VOL]);
				out_area = wl_rate[nIndex][AREA] + (wl_rate[nIndex + 1][AREA] - wl_rate[nIndex][AREA]) * (in_vol - wl_rate[nIndex][VOL]) / (wl_rate[nIndex + 1][VOL] - wl_rate[nIndex][VOL]);
				break;
			}
		}
	}

	out_wl *= 1000;
}

TPond::TPond(void)
{
	m_nType = NODE_POND;
	m_pTable = NULL;
	supply_id = -1;
	m_pSupplyNode = NULL;
	recharge_id = -1;
	m_pRechargeNode = NULL;
	Clear();
}

TPond::~TPond(void)
{
	if(m_pTable)
		free(m_pTable);
	m_pTable = NULL;
}

void TPond::Clear(void)
{
	m_pfCalculate = NULL;
	type = 0;
	memset(wl_rate, 0, sizeof(wl_rate));
	rate_count = 0;

	vol_init = 1000000;
	vol_eff = 1000000;
	aqf_ks = 0.0005;
	pipe_ht = 5 * 1000;
	pipe_area = 1;
	pipe_coef = 0.5;
	spill_ht = 15 * 1000;
	spill_length = 30 * 1000;
	spill_coef = 0.8;

	intake_vol = 10000;

	offline_max = 0.5;
	offline_ratio = 0.5;
	offline_dnQ = 0;

	surf_inf = 0;
	surf_ev = 0;
	area = 0;
	spill_out = 0;
	pipe_out = 0;
	aqf_loss = 0;
	offline_out = 1;
	method = 1;
	prev_inf = prev_out = 0;

	m_nClimate = 0;
	memset(&m_Climates, 0, sizeof(m_Climates));
	m_Climates[0].nRain = 100;
	m_Climates[1].nEva = 100;
	intake_type = 0;
	m_nTable = 0;
	m_nData = 0;
	memset(m_szIntake, 0, sizeof(m_szIntake));
}

void TPond::Init(void)
{
	TBaseNode::Init();

	if(type == 0)
		m_pfCalculate = &TPond::CalcOnline;
	else
		m_pfCalculate = &TPond::CalcOffline;
	vol = vol_init;
	m_pRain = m_sClimate.FindSeries(FT_PRECIP);
	m_pPev = m_sClimate.FindSeries(FT_EVA);
	offline_dnQ = 0;
	// spill height에서의 저수량 계산
	max_vol = CalcVolByHeight(spill_ht / 1000);
	offline_max_step = ConvertDay2Step(offline_max * 86400);
	offline_out_step = ConvertDay2Step(offline_out * 86400);
	if(intake_type == 0)
	{
		intake_vol_t = ConvertDay2Step(intake_vol);
		m_pfIntake = &TPond::GetIntakeConst;
	}
	else
	{
		LoadSeries();
		if(m_sIntake.GetCount() > m_nTable && m_sIntake.GetAt(m_nTable)->GetCount() > m_nData)
		{
			m_pIntake = m_sIntake.GetAt(m_nTable)->GetAt(m_nData);
			m_pIntake->SetFirstPos(0);
			m_pfIntake = &TPond::GetIntakeSeries;
		}
		else
		{
			intake_vol_t = 0;
			m_pfIntake = &TPond::GetIntakeConst;
		}
	}

	const int AREA = 2;
	fullArea = 0;
	for(int nIndex = 0; nIndex < rate_count; nIndex++)
	{
		if(wl_rate[nIndex][AREA] > fullArea)
			fullArea = wl_rate[nIndex][AREA];
	}
}

void TPond::InitResult(unsigned long nTime, int nInterval, int nCount)
{
	const char* pszItemHeader[] = {"Inflow(㎥)", "Rainfall(mm)", "Evaporation Water surface(mm)", "Intake(㎥)", "Downstream Outflow(㎥)", "Spillway Outflow(㎥)",
								"Pipe Outflow(㎥)", "Aquifer Loss(㎥)", "Volume(㎥)", "Water Level(m)"};

	m_pResult->Clear();
	m_pResult->m_dtStart = nTime;
	m_pResult->m_Header.nInterval = nInterval;
	m_pResult->m_Header.nData = FT_USER;
	m_pResult->m_Header.nCount = nCount;

	for(int nCol = 0; nCol < (int)(sizeof(pszItemHeader) / sizeof(char*)); nCol++)
	{
		TSeriesItem *pItem = m_pResult->AddItem();
		strcpy_s(pItem->m_Header.szHeader, 50, pszItemHeader[nCol]);
		pItem->m_Header.date = nTime;
		pItem->m_Header.nInterval = nInterval;
		pItem->SetSize(nCount);
	}
}

void TPond::Calculate(int nStep)
{
	m_nRain = m_pRain ? m_pRain->GetNext() : 0.0f;
	m_nPev = m_pPev ? m_pPev->GetNext() : 0.0f;
	m_pBalance->FindOrCreateNode(m_nID, m_nType)->FindOrCreateYear(m_pDate->nYear, vol, 0);

	(this->*m_pfCalculate)();
	CalcRate(vol, wl, area);

	if(m_pLink)
		m_pLink->Calculate(nStep, pipe_out + spill_out, 0, 0, pipe_out + spill_out);
	if(m_pSupplyNode)
		m_pSupplyNode->AddValue(nStep, intake, 0, 0, intake);

	AddResult(nStep);
	AddBalance();
	m_nImport = m_nInflow = m_nSf = m_nGW = m_nInter = m_nLeakage = 0;
}

void TPond::CalcInitial(void)
{
	// 저류량, 수면적, 수위 계산
	vol += surf_inf;
	CalcRate(vol, wl, area);

	// 수면 증발량
	surf_ev = m_nPev;
	surf_evV = surf_ev * area / 1000;

	// 물넘이 방류량
	spill_out = wl > spill_ht ? spill_coef * spill_length * pow((wl - spill_ht) / 1000, 1.5f) * m_nDT / 1000 : 0;
	// spill_out이 spill에서의 저수량을 넘어갈 경우 spill_out값 변경 (현재볼륨 - spill높이의 저수량)
	if((spill_out > vol - max_vol) && (vol > max_vol))
		spill_out = vol - max_vol;
}

void TPond::CalcOnline(void)
{
	surf_inf = m_nInflow + m_nImport;// + (m_nRain * fullArea / 1000);

	CalcInitial();

	// 방류구 방류량
	pipe_out = wl > pipe_ht ? pipe_coef * pipe_area * sqrt(2 * G * (wl - pipe_ht) / 1000) : 0;

	CalcLoss();

	vol = CalcStorage();
}

void TPond::CalcOffline(void)
{
	if(m_nInflow > offline_max_step)
	{
		surf_inf = (m_nInflow - offline_max_step) * offline_ratio;
		offline_dnQ = m_nInflow - surf_inf;
	}
	else
	{
		surf_inf = 0;
		offline_dnQ = m_nInflow;
	}
	surf_inf += (m_nImport + m_nRain * fullArea / 1000);

	CalcInitial();

	pipe_out = ((offline_dnQ < offline_out_step) && (wl > pipe_ht)) ? pipe_coef * pipe_area * sqrt(2 * G * (wl - pipe_ht) * m_nDT / 1000) : 0;

	CalcLoss();

	vol = CalcStorage();
}

void TPond::CalcLoss(void)
{
	float intake_t = (this->*m_pfIntake)();
	intake = vol > vol_eff * 0.2f ?  intake_t : 0;
	aqf_loss = aqf_ks * area *1.3f;
	sur_rain = m_nRain * fullArea / 1000;
	if(m_pRechargeNode)
	{
		m_pRechargeNode->AddRecharge(aqf_loss);
	}
}

float TPond::CalcStorage(void)
{
	float vol_t;

	vol_t = vol + sur_rain - surf_evV - spill_out - pipe_out - intake - aqf_loss;
	return vol_t;
}

void TPond::CalcRate(float in_vol, float &out_wl, float &out_area)
{
	const int EL = 0;
	const int VOL = 1;
	const int AREA = 2;
	int nIndex;

	if(in_vol < wl_rate[0][VOL])
	{
		if(wl_rate[0][VOL] > 0)
		{
			out_wl = wl_rate[0][EL] * in_vol / wl_rate[0][VOL];
			out_area = wl_rate[0][AREA] * in_vol / wl_rate[0][VOL];
		}
		else
			out_wl = out_area = 0;
	}
	else if(in_vol > wl_rate[rate_count - 1][VOL])
	{
		out_wl = wl_rate[rate_count - 1][EL] + (in_vol - wl_rate[rate_count - 1][VOL]) / wl_rate[rate_count - 1][AREA];
		out_area = wl_rate[rate_count - 1][AREA];
	}
	else
	{
		for(nIndex = 0; nIndex < rate_count - 1; nIndex++)
		{
			if(wl_rate[nIndex][VOL] <= in_vol && in_vol <= wl_rate[nIndex + 1][VOL])
			{
				out_wl = wl_rate[nIndex][EL] + (wl_rate[nIndex + 1][EL] - wl_rate[nIndex][EL]) * (in_vol - wl_rate[nIndex][VOL]) / (wl_rate[nIndex + 1][VOL] - wl_rate[nIndex][VOL]);
				out_area = wl_rate[nIndex][AREA] + (wl_rate[nIndex + 1][AREA] - wl_rate[nIndex][AREA]) * (in_vol - wl_rate[nIndex][VOL]) / (wl_rate[nIndex + 1][VOL] - wl_rate[nIndex][VOL]);
				break;
			}
		}
	}

	out_wl *= 1000;
}

float TPond::CalcVolByHeight(float nHeight)
{
	float nVol = 0;
	int nIndex;
	const int EL = 0;
	const int VOL = 1;

	if(rate_count == 0)
		return 0;

	if(nHeight < wl_rate[0][EL])
		nVol = wl_rate[0][VOL] * nHeight / wl_rate[0][EL];
	else if(nHeight > wl_rate[rate_count - 1][EL])
		nVol = wl_rate[rate_count - 1][VOL];
	else
	{
		for(nIndex = 0; nIndex < rate_count - 1; nIndex++)
		{
			if(wl_rate[nIndex][EL] <= nHeight && nHeight <= wl_rate[nIndex + 1][EL])
				nVol = wl_rate[nIndex][VOL] + (wl_rate[nIndex + 1][VOL] - wl_rate[nIndex][VOL]) * (nHeight - wl_rate[nIndex][EL]) / (wl_rate[nIndex + 1][EL] - wl_rate[nIndex][EL]);
		}
	}

	return nVol;
}

void TPond::AddBalance(void)
{
	TNodeYear *pYear = m_pBalance->FindOrCreateNode(m_nID, m_nType)->FindOrCreateYear(m_pDate->nYear, vol, 0);
	pYear->Add(m_nRain * fullArea / 1000, m_nInflow + m_nImport, surf_evV, 0, offline_dnQ, spill_out, pipe_out, intake, 0, vol, 0, 0, aqf_loss);
	pYear->Calc(1, 1, 1);
}

void TPond::AddResult(int nStep)
{
	m_pResult->GetAt(0)->AddValue(nStep, surf_inf);
	m_pResult->GetAt(1)->AddValue(nStep, m_nRain);
	m_pResult->GetAt(2)->AddValue(nStep, surf_ev);
	m_pResult->GetAt(3)->AddValue(nStep, intake);
	m_pResult->GetAt(4)->AddValue(nStep, offline_dnQ);
	m_pResult->GetAt(5)->AddValue(nStep, spill_out);
	m_pResult->GetAt(6)->AddValue(nStep, pipe_out);
	m_pResult->GetAt(7)->AddValue(nStep, aqf_loss);
	m_pResult->GetAt(8)->AddValue(nStep, vol);
	m_pResult->GetAt(9)->AddValue(nStep, wl / 1000);
}

float TPond::GetIntakeConst(void)
{
	return intake_vol_t;
}

float TPond::GetIntakeSeries(void)
{
	return m_pIntake->GetNext();
}

void TPond::CalcOutflow(float &spil, float &pipe, float &total)
{
	int nIndex;

	for(nIndex = 0; nIndex < m_nTables; nIndex++)
	{
		if(m_pTable[nIndex].storage > vol_init)
		{
			if(nIndex > 0)
			{
				float t = (vol_init - m_pTable[nIndex - 1].storage) / (m_pTable[nIndex].storage - m_pTable[nIndex - 1].storage);
				spil = m_pTable[nIndex - 1].spil + (m_pTable[nIndex].spil - m_pTable[nIndex - 1].spil) * t;
				pipe = m_pTable[nIndex - 1].pipe + (m_pTable[nIndex].pipe - m_pTable[nIndex - 1].pipe) * t;
				total = m_pTable[nIndex - 1].total + (m_pTable[nIndex].total - m_pTable[nIndex - 1].total) * t;
				break;
			}
			else
			{
				spil = m_pTable[nIndex].spil;
				pipe = m_pTable[nIndex].pipe;
				total = m_pTable[nIndex].total;
			}
		}
	}
}

void TPond::CalcSI(float spil, float pipe, float total, float &si_spil, float &si_pipe, float &si_total)
{
	int nIndex;

	for(nIndex = 0; nIndex < m_nTables; nIndex++)
	{
		if(m_pTable[nIndex].spil > spil)
		{
			if(nIndex > 0)
			{
				si_spil = m_pTable[nIndex - 1].si_spil + (m_pTable[nIndex].si_spil - m_pTable[nIndex - 1].si_spil) * (spil - m_pTable[nIndex - 1].spil) / (m_pTable[nIndex].spil - m_pTable[nIndex - 1].spil);
				break;
			}
			else
				si_spil = m_pTable[nIndex].si_spil;
		}
	}

	for(nIndex = 0; nIndex < m_nTables; nIndex++)
	{
		if(m_pTable[nIndex].pipe > pipe)
		{
			if(nIndex > 0)
			{
				si_pipe = m_pTable[nIndex - 1].si_pipe + (m_pTable[nIndex].si_pipe - m_pTable[nIndex - 1].si_pipe) * (pipe - m_pTable[nIndex - 1].pipe) / (m_pTable[nIndex].pipe - m_pTable[nIndex - 1].pipe);
				break;
			}
			else
				si_pipe = m_pTable[nIndex].si_pipe;
		}
	}

	for(nIndex = 0; nIndex < m_nTables; nIndex++)
	{
		if(m_pTable[nIndex].total > total)
		{
			if(nIndex > 0)
			{
				si_total = m_pTable[nIndex - 1].si_total + (m_pTable[nIndex].si_total - m_pTable[nIndex - 1].si_total) * (total - m_pTable[nIndex - 1].total) / (m_pTable[nIndex].total - m_pTable[nIndex - 1].total);
				break;
			}
			else
				si_total = m_pTable[nIndex].si_total;
		}
	}
}

void TPond::GetOutflowFromSI(float si_spil, float si_pipe, float si_total, float &spil, float &pipe, float &total)
{
	int nIndex;

	for(nIndex = 0; nIndex < m_nTables; nIndex++)
	{
		if(m_pTable[nIndex].si_spil > si_spil)
		{
			spil = m_pTable[nIndex - 1].spil + (m_pTable[nIndex].spil - m_pTable[nIndex - 1].spil) * (si_spil - m_pTable[nIndex - 1].si_spil) / (m_pTable[nIndex].si_spil - m_pTable[nIndex - 1].si_spil);
			break;
		}
		else
			spil = m_pTable[nIndex].spil;
	}

	for(nIndex = 0; nIndex < m_nTables; nIndex++)
	{
		if(m_pTable[nIndex].si_pipe > si_pipe)
		{
			pipe = m_pTable[nIndex - 1].pipe + (m_pTable[nIndex].pipe - m_pTable[nIndex - 1].pipe) * (si_pipe - m_pTable[nIndex - 1].si_pipe) / (m_pTable[nIndex].si_pipe - m_pTable[nIndex - 1].si_pipe);
			break;
		}
		else
			pipe = m_pTable[nIndex].pipe;
	}

	for(nIndex = 0; nIndex < m_nTables; nIndex++)
	{
		if(m_pTable[nIndex].si_total > si_total)
		{
			total = m_pTable[nIndex - 1].total + (m_pTable[nIndex].total - m_pTable[nIndex - 1].total) * (si_total - m_pTable[nIndex - 1].si_total) / (m_pTable[nIndex].si_total - m_pTable[nIndex - 1].si_total);
			break;
		}
		else
			total = m_pTable[nIndex].total;
	}
}

void TPond::CalcPuls(void)
{
	float si_spil, si_pipe, si_total;

	si_spil = (prev_inf + surf_inf) / 2 + prev_si_spil - prev_spil;
	si_pipe = (prev_inf + surf_inf) / 2 + prev_si_pipe - prev_pipe;
	si_total = (prev_inf + surf_inf) / 2 + prev_si_total - prev_out;

	GetOutflowFromSI(si_spil, si_spil, si_total, puls_spil, puls_pipe, puls_total);

	prev_inf = surf_inf;
	prev_si_spil = si_spil;
	prev_si_pipe = si_pipe;
	prev_si_total = si_total;
}

void TPond::CalcPulsTable(void)
{
	int nIndex;

	m_nTables = spill_ht / 1000;

	if(rate_count > 0 && m_nTables < wl_rate[rate_count - 1][0])
		m_nTables = wl_rate[rate_count - 1][0];
	m_nTables += 2;

	m_pTable = (WLOUT*)realloc(m_pTable, sizeof(WLOUT) * (m_nTables));

	for(nIndex = 0; nIndex < m_nTables; nIndex++)
	{
		float wl = nIndex * 1000;

		m_pTable[nIndex].wl = wl;
		m_pTable[nIndex].storage = CalcVolByHeight(wl / 1000);
		m_pTable[nIndex].spil = wl > spill_ht ? spill_coef * spill_length * pow((wl - spill_ht) / 1000, 1.5f) * m_nDT / 1000 : 0;
		m_pTable[nIndex].pipe = wl > pipe_ht ? pipe_coef * pipe_area * sqrt((wl - pipe_ht) / 1000) : 0;
		m_pTable[nIndex].total = m_pTable[nIndex].spil + m_pTable[nIndex].pipe;
		m_pTable[nIndex].si_spil = m_pTable[nIndex].storage + m_pTable[nIndex].spil / 2;
		m_pTable[nIndex].si_pipe = m_pTable[nIndex].storage + m_pTable[nIndex].pipe / 2;
		m_pTable[nIndex].si_total = m_pTable[nIndex].storage + m_pTable[nIndex].total / 2;
	}
}

TRainTank::TRainTank(void)
{
	m_nType = NODE_RAINTANK;
	Clear();
}

TRainTank::~TRainTank(void)
{
}

void TRainTank::Clear(void)
{
	vol_init = 500;
	vol_min = 100;
	use = 100;
	cap = 2000;
	use_dt = 0;
	inf = 0;
	use_type = 0;
	m_nTable = 0;
	m_nData = 0;
	supply_id = -1;
	m_pSupplyNode = NULL;
	memset(m_szUse, 0, sizeof(m_szUse));
}

void TRainTank::Init(void)
{
	TBaseNode::Init();

	vol = vol_init;
	use_dt = 0;
	inf = 0;
	if(use_type == 0)
	{
		use_t = ConvertDay2Step(use);
		m_pfUse = &TRainTank::GetUseConst;
	}
	else
	{
		LoadSeries();
		if(m_sUse.GetCount() > m_nTable && m_sUse.GetAt(m_nTable)->GetCount() > m_nData)
		{
			m_pUse = m_sUse.GetAt(m_nTable)->GetAt(m_nData);
			m_pUse->SetFirstPos(0);
			m_pfUse = &TRainTank::GetUseSeries;
		}
		else
		{
			use_t = 0;
			m_pfUse = &TRainTank::GetUseConst;
		}
	}
}

void TRainTank::InitResult(unsigned long nTime, int nInterval, int nCount)
{
	const char* pszItemHeader[] = {"Inflow(㎥)", "Rain Tank Inflow(㎥)", "Water Use(㎥)", "Spillway Outflow(㎥)", "Downstream Outflow(㎥)", "Volume(㎥)"};

	m_pResult->Clear();
	m_pResult->m_dtStart = nTime;
	m_pResult->m_Header.nInterval = nInterval;
	m_pResult->m_Header.nData = FT_USER;
	m_pResult->m_Header.nCount = nCount;

	for(int nCol = 0; nCol < (int)(sizeof(pszItemHeader) / sizeof(char*)); nCol++)
	{
		TSeriesItem *pItem = m_pResult->AddItem();
		strcpy_s(pItem->m_Header.szHeader, 50, pszItemHeader[nCol]);
		pItem->m_Header.date = nTime;
		pItem->m_Header.nInterval = nInterval;
		pItem->SetSize(nCount);
	}
}

void TRainTank::Calculate(int nStep)
{
	float inflow = m_nInflow + m_nImport;
	float use_n = (this->*m_pfUse)();

	m_pBalance->FindOrCreateNode(m_nID, m_nType)->FindOrCreateYear(m_pDate->nYear, vol, 0);

	if(inflow > vol_min)
	{
		inf = inflow;
		out = 0;
	}
	else
	{
		inf = 0;
		out = inflow;
	}

	vol += inf;
	if(vol > use_n)
	{
		vol -= use_n;
		use_dt = use_n;
		if(vol > cap)
		{
			spill = vol - cap;
			vol = cap;
		}
		else
			spill = 0;
	}
	else
	{
		use_dt = vol;
		vol = 0;
		spill = 0;
	}

	out += spill;

	AddResult(nStep);
	AddBalance();

	if(m_pLink)
		m_pLink->Calculate(nStep, out, 0, 0, out);
	if(m_pSupplyNode)
		m_pSupplyNode->AddValue(nStep, use_dt, 0, 0, use_dt);

	m_nImport = m_nInflow = m_nSf = m_nGW = m_nInter = m_nLeakage = 0;
}

void TRainTank::AddBalance(void)
{
	TNodeYear *pYear = m_pBalance->FindOrCreateNode(m_nID, m_nType)->FindOrCreateYear(m_pDate->nYear, vol, 0);
	pYear->Add(0, (m_nInflow + m_nImport), 0, 0, (out - spill), spill, use_dt, 0, 0, vol, 0, 0, 0);
	pYear->Calc(1, 1, 1);
}

void TRainTank::AddResult(int nStep)
{
	m_pResult->GetAt(0)->AddValue(nStep, m_nInflow + m_nImport);
	m_pResult->GetAt(1)->AddValue(nStep, inf);
	m_pResult->GetAt(2)->AddValue(nStep, use_dt);
	m_pResult->GetAt(3)->AddValue(nStep, spill);
	m_pResult->GetAt(4)->AddValue(nStep, out);
	m_pResult->GetAt(5)->AddValue(nStep, vol);
}

float TRainTank::GetUseConst(void)
{
	return use_t;
}

float TRainTank::GetUseSeries(void)
{
	return m_pUse->GetNext();
}

TRecycle::TRecycle(void)
{
	m_nType = NODE_RECYCLE;
	Clear();
}

TRecycle::~TRecycle(void)
{
}

void TRecycle::Clear(void)
{
	m_nRecursive = 0;
	memset(&m_Recursive, 0, sizeof(m_Recursive));
	rec_cond = 0.5;
	rec_intake = 10000;
}

void TRecycle::Init(void)
{
	TBaseNode::Init();

	surf_inf = 0;
	surf_out = 0;
	rec_intake_dt = ConvertDay2Step(rec_intake);
}

void TRecycle::InitResult(unsigned long nTime, int nInterval, int nCount)
{
	const char* pszItemHeader[] = {"Inflow(㎥)", "Intake(㎥)", "Downstream Outflow(㎥)"};
	int nCol;

	m_pResult->Clear();
	m_pResult->m_dtStart = nTime;
	m_pResult->m_Header.nInterval = nInterval;
	m_pResult->m_Header.nData = FT_USER;
	m_pResult->m_Header.nCount = nCount;

	for(nCol = 0; nCol < (int)(sizeof(pszItemHeader) / sizeof(char*)); nCol++)
	{
		TSeriesItem *pItem = m_pResult->AddItem();
		strcpy_s(pItem->m_Header.szHeader, 50, pszItemHeader[nCol]);
		pItem->m_Header.date = nTime;
		pItem->m_Header.nInterval = nInterval;
		pItem->SetSize(nCount);
	}

	for(nCol = 0; nCol < m_nRecursive; nCol++)
	{
		TSeriesItem *pItem = m_pResult->AddItem();

		sprintf_s(pItem->m_Header.szHeader, 50, "%s(㎥)", m_Recursive[nCol].pNode->GetName());
		pItem->m_Header.date = nTime;
		pItem->m_Header.nInterval = nInterval;
		pItem->SetSize(nCount);
	}
}

void TRecycle::Calculate(int nStep)
{
	int nIndex;
	float inflow = m_nInflow + m_nImport;

	/*
	surf_inf = rec_intake_dt;
	if(surf_inf > inflow * rec_cond)
		surf_inf = inflow * rec_cond;
	*/
	if(rec_intake_dt > inflow * rec_cond)
	{
		if(rec_intake_dt > inflow)
			surf_inf = inflow;
		else
			surf_inf = rec_intake_dt;
	}
	else
		surf_inf = inflow * rec_cond;

	surf_out = inflow - surf_inf;

	for(nIndex = 0; nIndex < m_nRecursive; nIndex++)
	{
		if(m_Recursive[nIndex].pNode)
		{
			surf_node[nIndex] = surf_inf * m_Recursive[nIndex].nRain;
			//m_Recursive[nIndex].pNode->AddValue(nStep, surf_node[nIndex], 0, 0, surf_node[nIndex]);
			m_Recursive[nIndex].pNode->AddImport(nStep, surf_node[nIndex], 0);
		}
	}

	AddResult(nStep);
	AddBalance();

	if(m_pLink)
		m_pLink->Calculate(nStep, surf_out, 0, 0, surf_out);

	m_nImport = m_nInflow = m_nSf = m_nGW = m_nInter = m_nLeakage = 0;
}

void TRecycle::AddBalance(void)
{
	TNodeYear *pYear = m_pBalance->FindOrCreateNode(m_nID, m_nType)->FindOrCreateYear(m_pDate->nYear, 0, 0);
	pYear->Add(0, m_nInflow, 0, 0, surf_inf, surf_out, 0, 0, 0, 0, 0, 0, 0);
	pYear->Calc(1, 1, 1);
}

void TRecycle::AddResult(int nStep)
{
	int nIndex;

	m_pResult->GetAt(0)->AddValue(nStep, m_nInflow + m_nImport);
	m_pResult->GetAt(1)->AddValue(nStep, surf_inf);
	m_pResult->GetAt(2)->AddValue(nStep, surf_out);
	for(nIndex = 0; nIndex < m_nRecursive; nIndex++)
		m_pResult->GetAt(nIndex + 3)->AddValue(nStep, surf_node[nIndex]);
}

void TPond::SetSeriesFileA(char* szFile)
{
	strcpy_s(m_szIntake, MAX_PATH, szFile);
}

void TPond::SetSeriesFileW(wchar_t* szFile)
{
	char cszPath[MAX_PATH * 2];

	memset(cszPath, 0, sizeof(cszPath));
	#ifndef WINRLIB
	wcstombs(cszPath, szFile, MAX_PATH * 2);
	#else
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, szFile, wcslen(szFile), cszPath, MAX_PATH * 2, NULL, NULL);
	#endif // WINRLIB
	strcpy_s(m_szIntake, MAX_PATH, cszPath);
}

void TPond::LoadSeries(void)
{
	TCHAR szSeries[MAX_PATH];

	memset(szSeries, 0, sizeof(szSeries));

#ifdef _UNICODE
	MultiByteToWideChar(CP_ACP, 0, m_szIntake, (int)strlen(m_szIntake), szSeries, MAX_PATH);
#else
	strcpy_s(szSeries, MAX_PATH, m_szIntake);
#endif

	m_sIntake.Load(szSeries);

}

void TRainTank::SetSeriesFileA(char* szFile)
{
	strcpy_s(m_szUse, MAX_PATH, szFile);
}

void TRainTank::SetSeriesFileW(wchar_t* szFile)
{
	char cszPath[MAX_PATH * 2];

	memset(cszPath, 0, sizeof(cszPath));
	#ifndef WINRLIB
	wcstombs(cszPath, szFile, MAX_PATH * 2);
	#else
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, szFile, wcslen(szFile), cszPath, MAX_PATH * 2, NULL, NULL);
	#endif // WINRLIB
	strcpy_s(m_szUse, MAX_PATH, cszPath);
}

void TRainTank::LoadSeries(void)
{
	TCHAR szSeries[MAX_PATH];

	memset(szSeries, 0, sizeof(szSeries));

#ifdef _UNICODE
	MultiByteToWideChar(CP_ACP, 0, m_szUse, (int)strlen(m_szUse), szSeries, MAX_PATH);
#else
	strcpy_s(szSeries, MAX_PATH, m_szUse);
#endif

	m_sUse.Load(szSeries);
}

int TImport::LoadText(FILE *fp)
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
		//strncpy_s(szToken, 100, szLine, (cFind - &szLine[0]));
		//strcpy_s(szToken2, cFind + 1);
		strncpy(szToken, szLine, min(100, (int)(cFind - &szLine[0])));
		strcpy(szToken2, cFind + 1);
		Trim(szToken2);
		Trim(szToken);

		if(strcmp(szToken, "NodeID") == 0)
		{
			ReadValue(szToken2, nVals, 1);
			SetID(int(nVals[0]));
		}
		else if(strcmp(szToken, "Name") == 0)
		{
			SetName(szToken2);
		}
		else if(strcmp(szToken, "Desc") == 0)
		{
			SetDesc(szToken2);
		}
		else if(strcmp(szToken, "Type") == 0)
		{
			ReadValue(szToken2, nVals, 1);
			type = (int)nVals[0];
		}
		else if(strcmp(szToken, "Constant") == 0)
		{
			ReadValue(szToken2, nVals, 1);
			m_nConst = nVals[0];
		}
		else if(strcmp(szToken, "Series") == 0)
		{
			SetSeriesFileA(szToken2);
		}
		else if(strcmp(szToken, "Table") == 0)
		{
			ReadValue(szToken2, nVals, 2);
			m_nTable = nVals[0];
			m_nData = nVals[1];
		}
		else if(strcmp(szToken, "Leakage") == 0)
		{
			ReadValue(szToken2, nVals, 1);
			m_nLeakage = nVals[0];
		}
		else
			ExtraFileContent(szLine);
	}

	return 1;
}

int TInfiltro::LoadText(FILE *fp)
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
		//strncpy_s(szToken, 100, szLine, (cFind - &szLine[0]));
		//strcpy_s(szToken2, cFind + 1);
		strncpy(szToken, szLine, min(100, (int)(cFind - &szLine[0])));
		strcpy(szToken2, cFind + 1);
		Trim(szToken2);
		Trim(szToken);

		if(strcmp(szToken, "NodeID") == 0)
		{
			ReadValue(szToken2, nVals, 1);
			SetID(int(nVals[0]));
		}
		else if(strcmp(szToken, "Name") == 0)
		{
			SetName(szToken2);
		}
		else if(strcmp(szToken, "Desc") == 0)
		{
			SetDesc(szToken2);
		}
		else if(strcmp(szToken, "Aquifer") == 0)
		{
			ReadValue(szToken2, nVals, 6);

			area = nVals[0];
			aqf_top = nVals[1];
			aqf_bot = nVals[2];
			aqf_S_coef = nVals[3];
			potential = nVals[4];
			in_node_id = nVals[5];
		}
		else if(strcmp(szToken, "GWMove") == 0)
		{
			ReadValue(szToken2, nVals, 6);

			gw_node_id = nVals[0];
			slope_method = nVals[1];
			slope_aqf = nVals[2];
			node_len = nVals[3];
			conn_len = nVals[4];
			Kgw = nVals[5];
		}
		else
			ExtraFileContent(szLine);
	}

	return 1;
}

int TBioRetention::LoadText(FILE *fp)
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
		//strncpy_s(szToken, 100, szLine, (cFind - &szLine[0]));
		//strcpy_s(szToken2, cFind + 1);
		strncpy(szToken, szLine, min(100, (int)(cFind - &szLine[0])));
		strcpy(szToken2, cFind + 1);
		Trim(szToken2);
		Trim(szToken);

		if(strcmp(szToken, "NodeID") == 0)
		{
			ReadValue(szToken2, nVals, 1);
			SetID(int(nVals[0]));
		}
		else if(strcmp(szToken, "Name") == 0)
		{
			SetName(szToken2);
		}
		else if(strcmp(szToken, "Desc") == 0)
		{
			SetDesc(szToken2);
		}
		else if(strcmp(szToken, "Aquifer") == 0)
		{
			ReadValue(szToken2, nVals, 6);

			area = nVals[0];
			aqf_top = nVals[1];
			aqf_bot = nVals[2];
			aqf_S_coef = nVals[3];
			potential = nVals[4];
			in_node_id = nVals[5];
		}
		else if(strcmp(szToken, "GWMove") == 0)
		{
			ReadValue(szToken2, nVals, 6);

			gw_node_id = nVals[0];
			slope_method = nVals[1];
			slope_aqf = nVals[2];
			node_len = nVals[3];
			conn_len = nVals[4];
			Kgw = nVals[5];
		}
		else if(strcmp(szToken, "Evaporation") == 0)
		{
			ReadValue(szToken2, nVals, 12);

			LAI[0] = nVals[0];
			LAI[1] = nVals[1];
			LAI[2] = nVals[2];
			LAI[3] = nVals[3];
			LAI[4] = nVals[4];
			LAI[5] = nVals[5];
			LAI[6] = nVals[6];
			LAI[7] = nVals[7];
			LAI[8] = nVals[8];
			LAI[9] = nVals[9];
			LAI[10] = nVals[10];
			LAI[11] = nVals[11];
		}
		else if(strcmp(szToken, "Rainfall") == 0)
		{
			ReadValue(szToken2, nVals, 1);
			m_Climates[0].nID = nVals[0];
			m_Climates[0].nRain = 100;
		}
		else if(strcmp(szToken, "EVA") == 0)
		{
			ReadValue(szToken2, nVals, 1);
			m_Climates[1].nID = nVals[0];
			m_Climates[1].nEva = 100;
		}
		else
			ExtraFileContent(szLine);
	}

	return 1;
}

int TWetLand::LoadText(FILE *fp)
{
	float nVals[30];
	char szToken[100];
	char szToken2[1024];
	char *cFind;
	char szLine[1024];
	int nIndex;

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
		//strncpy_s(szToken, 100, szLine, (cFind - &szLine[0]));
		//strcpy_s(szToken2, cFind + 1);
		strncpy(szToken, szLine, min(100,(int)(cFind - &szLine[0])));
		strcpy(szToken2, cFind + 1);
		Trim(szToken2);
		Trim(szToken);

		if(strcmp(szToken, "NodeID") == 0)
		{
			ReadValue(szToken2, nVals, 1);
			SetID(int(nVals[0]));
		}
		else if(strcmp(szToken, "Name") == 0)
		{
			SetName(szToken2);
		}
		else if(strcmp(szToken, "Desc") == 0)
		{
			SetDesc(szToken2);
		}
		else if(strcmp(szToken, "Base") == 0)
		{
			ReadValue(szToken2, nVals, 4);

			vol_init = nVals[0];
			vol_max = nVals[1];
			bypass = nVals[2];
			aqf_ks = nVals[3];
		}
		else if(strcmp(szToken, "Pipe") == 0)
		{
			ReadValue(szToken2, nVals, 3);

			pipe_ht = nVals[0];
			pipe_area = nVals[1];
			pipe_coef = nVals[2];
		}
		else if(strcmp(szToken, "RateCount") == 0)
		{
			ReadValue(szToken2, nVals, 1);

			rate_count = nVals[0];
		}
		else if(strcmp(szToken, "WL") == 0)
		{
			ReadValue(szToken2, nVals, rate_count);

			for(nIndex = 0; nIndex < rate_count; nIndex++)
				wl_rate[nIndex][0] = nVals[nIndex];
		}
		else if(strcmp(szToken, "VOL") == 0)
		{
			ReadValue(szToken2, nVals, rate_count);

			for(nIndex = 0; nIndex < rate_count; nIndex++)
				wl_rate[nIndex][1] = nVals[nIndex];
		}
		else if(strcmp(szToken, "AREA") == 0)
		{
			ReadValue(szToken2, nVals, rate_count);

			for(nIndex = 0; nIndex < rate_count; nIndex++)
				wl_rate[nIndex][2] = nVals[nIndex];
		}
		else if(strcmp(szToken, "Rainfall") == 0)
		{
			ReadValue(szToken2, nVals, 1);
			m_Climates[0].nID = nVals[0];
			m_Climates[0].nRain = 100;
		}
		else if(strcmp(szToken, "EVA") == 0)
		{
			ReadValue(szToken2, nVals, 1);
			m_Climates[1].nID = nVals[0];
			m_Climates[1].nEva = 100;
		}
		else if(strcmp(szToken, "Recharge") == 0)
		{
			ReadValue(szToken2, nVals, 1);

			recharge_id = nVals[0];
		}
		else
			ExtraFileContent(szLine);
	}

	return 1;
}

int TPond::LoadText(FILE *fp)
{
	int nIndex;
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
		//strncpy_s(szToken, 100, szLine, (cFind - &szLine[0]));
		//strcpy_s(szToken2, cFind + 1);
		strncpy(szToken, szLine, min(100, (int)(cFind - &szLine[0])));
		strcpy(szToken2, cFind + 1);
		Trim(szToken2);
		Trim(szToken);

		if(strcmp(szToken, "NodeID") == 0)
		{
			ReadValue(szToken2, nVals, 1);
			SetID(int(nVals[0]));
		}
		else if(strcmp(szToken, "Name") == 0)
		{
			SetName(szToken2);
		}
		else if(strcmp(szToken, "Desc") == 0)
		{
			SetDesc(szToken2);
		}
		else if(strcmp(szToken, "Base") == 0)
		{
			ReadValue(szToken2, nVals, 3);

			vol_init = nVals[0];
			vol_eff = nVals[1];
			aqf_ks = nVals[2];
		}
		else if(strcmp(szToken, "Pipe") == 0)
		{
			ReadValue(szToken2, nVals, 3);

			pipe_ht = nVals[0];
			pipe_area = nVals[1];
			pipe_coef = nVals[2];
		}
		else if(strcmp(szToken, "Spill") == 0)
		{
			ReadValue(szToken2, nVals, 3);

			spill_ht = nVals[0];
			spill_length = nVals[1];
			spill_coef = nVals[2];
		}
		else if(strcmp(szToken, "Offline") == 0)
		{
			ReadValue(szToken2, nVals, 3);

			offline_max = nVals[0];
			offline_ratio = nVals[1];
			offline_out = nVals[2];
		}
		else if(strcmp(szToken, "Intake") == 0)
		{
			ReadValue(szToken2, nVals, 4);

			intake_type = nVals[0];
			intake_vol = nVals[1];
			m_nTable = nVals[2];
			m_nData = nVals[3];
		}
		else if(strcmp(szToken, "Series") == 0)
			SetSeriesFileA(szToken2);
		else if(strcmp(szToken, "RateCount") == 0)
		{
			ReadValue(szToken2, nVals, 1);

			rate_count = nVals[0];
		}
		else if(strcmp(szToken, "WL") == 0)
		{
			ReadValue(szToken2, nVals, rate_count);

			for(nIndex = 0; nIndex < rate_count; nIndex++)
				wl_rate[nIndex][0] = nVals[nIndex];
		}
		else if(strcmp(szToken, "VOL") == 0)
		{
			ReadValue(szToken2, nVals, rate_count);

			for(nIndex = 0; nIndex < rate_count; nIndex++)
				wl_rate[nIndex][1] = nVals[nIndex];
		}
		else if(strcmp(szToken, "AREA") == 0)
		{
			ReadValue(szToken2, nVals, rate_count);

			for(nIndex = 0; nIndex < rate_count; nIndex++)
				wl_rate[nIndex][2] = nVals[nIndex];
		}
		else if(strcmp(szToken, "Rainfall") == 0)
		{
			ReadValue(szToken2, nVals, 1);
			m_Climates[0].nID = nVals[0];
			m_Climates[0].nRain = 100;
		}
		else if(strcmp(szToken, "EVA") == 0)
		{
			ReadValue(szToken2, nVals, 1);
			m_Climates[1].nID = nVals[0];
			m_Climates[1].nEva = 100;
		}
		else if(strcmp(szToken, "Supply") == 0)
		{
			ReadValue(szToken2, nVals, 1);

			supply_id = nVals[0];
		}
		else if(strcmp(szToken, "Recharge") == 0)
		{
			ReadValue(szToken2, nVals, 1);

			recharge_id = nVals[0];
		}
		else
			ExtraFileContent(szLine);
	}

	return 1;
}

int TRainTank::LoadText(FILE *fp)
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
		//strncpy_s(szToken, 100, szLine, (cFind - &szLine[0]));
		//strcpy_s(szToken2, cFind + 1);
		strncpy(szToken, szLine, min(100, (int)(cFind - &szLine[0])));
		strcpy(szToken2, cFind + 1);
		Trim(szToken2);
		Trim(szToken);

		if(strcmp(szToken, "NodeID") == 0)
		{
			ReadValue(szToken2, nVals, 1);
			SetID(int(nVals[0]));
		}
		else if(strcmp(szToken, "Name") == 0)
		{
			SetName(szToken2);
		}
		else if(strcmp(szToken, "Desc") == 0)
		{
			SetDesc(szToken2);
		}
		else if(strcmp(szToken, "Volume") == 0)
		{
			ReadValue(szToken2, nVals, 3);

			vol_init = nVals[0];
			vol_min = nVals[1];
			cap = nVals[2];
		}
		else if(strcmp(szToken, "Use") == 0)
		{
			ReadValue(szToken2, nVals, 4);

			use_type = nVals[0];
			use = nVals[1];
			m_nTable = nVals[2];
			m_nData = nVals[3];
		}
		else if(strcmp(szToken, "Series") == 0)
		{
			SetSeriesFileA(szToken2);
		}
		else if(strcmp(szToken, "Supply") == 0)
		{
			ReadValue(szToken2, nVals, 1);

			supply_id = nVals[0];
		}
		else
			ExtraFileContent(szLine);
	}

	return 1;
}

int TRecycle::LoadText(FILE *fp)
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
		//strncpy_s(szToken, 100, szLine, (cFind - &szLine[0]));
		//strcpy_s(szToken2, cFind + 1);
		strncpy(szToken, szLine, min(100, (int)(cFind - &szLine[0])));
		strcpy(szToken2, cFind + 1);
		Trim(szToken2);
		Trim(szToken);

		if(strcmp(szToken, "NodeID") == 0)
		{
			ReadValue(szToken2, nVals, 1);
			SetID(int(nVals[0]));
		}
		else if(strcmp(szToken, "Name") == 0)
		{
			SetName(szToken2);
		}
		else if(strcmp(szToken, "Desc") == 0)
		{
			SetDesc(szToken2);
		}
		else if(strcmp(szToken, "Intake") == 0)
		{
			ReadValue(szToken2, nVals, 2);

			rec_cond = nVals[0];
			rec_intake = nVals[1];
		}
		else if(strcmp(szToken, "Nodes") == 0)
		{
			char *cFind = strchr(szToken2, ',');
			int nPos = 0;

			m_nRecursive = 0;
			//sscanf_s(szToken2, "%d", &m_nRecursive);
			sscanf(szToken2, "%d", &m_nRecursive);
			while(cFind)
			{
				char *cFind2 = strchr(cFind + 1, ':');
				m_Recursive[nPos].nID = atol(cFind + 1);
				m_Recursive[nPos].nRain = (float)atof(cFind2 + 1);
				nPos++;

				cFind = strchr(cFind + 1, ',');
			}
			m_nRecursive = nPos;
		}
		else
			ExtraFileContent(szLine);
	}

	return 1;
}
