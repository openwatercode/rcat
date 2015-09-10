#include "StdAfx.h"
#include "include/SeriesData.h"
#include "include/Catchment.h"
#include "include/Link.h"
#include <math.h>
#include <algorithm>

using namespace std;

#define VU10 0.0000000001f

TCatchment::TCatchment(void)
{
	m_nCanAccept = NODE_ALLOWNONE;
	m_nCanOutput = NODE_ALLOWSINGLE;
	m_nClimates = 0;
	memset(m_Climates, 0, sizeof(m_Climates));
	m_nPev = m_nRain = 0;
	Clear();
}

TCatchment::~TCatchment(void)
{
}

void TCatchment::operator =(TBaseNode &node)
{
	TCatchment *pCatch = (TCatchment*)(&node);

	memcpy(m_Climates, pCatch->m_Climates, sizeof(m_Climates));
	m_nClimates = pCatch->m_nClimates;
	m_nPev = m_nRain = 0;
	m_sClimate.Clear();

	TBaseNode::operator =(node);
}

TUrban::TUrban(void)
{
	m_nType = NODE_URBAN;
	m_nCanAccept = NODE_ALLOWNONE;
	m_nCanOutput = NODE_ALLOWSINGLE;
	Clear();
}

TUrban::~TUrban(void)
{
}

void TUrban::Clear(void)
{
	const float csnDefLAI[] = {0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f};

	m_pfnAddResult = &TUrban::VoidFunc;
	m_pfnAddBalance = &TUrban::VoidFunc;
	m_pfnGetLAI = &TUrban::GetLAI;

	// Node
	Area = 1000000;				// m2
	slope = 0.05f;
	Aratio_imp = 0.5f;
	Aratio_per = 0.5f;
	Aratio_per_plant = 0.5f;
	depC_per = 5.0f;				// mm
	depC_imp =2.0f;

	// Soil
	theta_per = 0.25f;
	soil_th_per = 1000.0f;		// mm
	theta_s_per = 0.453f;
	theta_r_per = 0.041f;
	theta_FC_per = 0.207f;
	theta_W_per = 0.095f;
	ks_per = 0.00061; // mm/s
	ksi_per = 0.002; // mm/s
	n_mualem = 7;
	infilt_method = 0;
	PSI = 110.1;
	ht_k = 4.14 / 3600;
	ht_fc = 9.5 / 3600;
	ht_fo = 80.8 / 3600;

	// Aquifer
	gwE = 35.0f * 1000;			// mm
	rivE = 34.0f * 1000;			// mm
	riv_th = 1.0f * 1000;			// mm
	ku_riv = 0.004f;
	Area_riv = 30000.0f;			// m2
	aqf_S = 0.1f;
	aqf_Top = 36.0f * 1000;		// mm
	aqf_Bot = 26.0f * 1000;		// mm
	solcov = 10000.0f;
	gw_intake_rate = 0;	// m3/day
	leakage_rate = 0.15;	// 0-1

	// GW Move
	gw_move_node = 0;
	slope_method = 0;
	slope_aqf = 0.05;
	node_len = 1000;
	conn_len = 3000;

	// Weather
//	Kgw = 0.0005; // mm/s
	Kgw = 0.0000005; // m/s
	ET_method = 0;
	memcpy(LAI, csnDefLAI, sizeof(csnDefLAI));

	m_pGWMove = NULL;
	m_pBalance = NULL;

	nRes_ETo = 0;
	nAET_per = 0;
	nAET_imp = 0;
	nAET_tot = 0;

	m_pRain = NULL;
	m_pPev = NULL;
	m_nTempETo = 0;
	m_nCount = 0;

	dep_imp = 0;
	dep_per = 0;
	flow_sf = 0;
	flow_sf_imp = 0;
	flow_sf_per = 0;
	flow_inter = 0;
	flow_gw = 0;
	recharge = 0;
	flow_tot = 0;
	infiltrate = 0;
}

void TUrban::operator =(TBaseNode& node)
{
	TUrban* pUrban = (TUrban*)(&node);

	Area = pUrban->Area;
	slope = pUrban->slope;
	Aratio_imp = pUrban->Aratio_imp;
	Aratio_per = pUrban->Aratio_per;
	Aratio_per_plant = pUrban->Aratio_per_plant;
	depC_imp = pUrban->depC_imp;
	depC_per = pUrban->depC_per;
	theta_per = pUrban->theta_per;
	soil_th_per = pUrban->soil_th_per;
	theta_s_per = pUrban->theta_s_per;
	theta_r_per = pUrban->theta_r_per;
	theta_W_per = pUrban->theta_W_per;
	theta_FC_per = pUrban->theta_FC_per;
	ks_per = pUrban->ks_per;
	ksi_per = pUrban->ksi_per;
	n_mualem = pUrban->n_mualem;
	gwE = pUrban->gwE;
	rivE = pUrban->rivE;
	riv_th = pUrban->riv_th;
	ku_riv = pUrban->ku_riv;
	Area_riv = pUrban->Area_riv;
	aqf_S = pUrban->aqf_S;
	aqf_Top = pUrban->aqf_Top;
	aqf_Bot = pUrban->aqf_Bot;
	gw_move_node = pUrban->gw_move_node;
	slope_aqf = pUrban->slope_aqf;
	node_len = pUrban->node_len;
	conn_len = pUrban->conn_len;
	Kgw = pUrban->Kgw;
	solcov = pUrban->solcov;
	ET_method = pUrban->ET_method;

	TCatchment::operator =(node);
}

void TUrban::Init(void)
{
	TCatchment::Init();

	m_pfnAddResult = &TUrban::AddResult;
	m_pfnAddBalance = &TUrban::AddBalance;

	if(Aratio_per > 0)
		m_pfPervious = &TUrban::CalcPervious;
	else
		m_pfPervious = &TUrban::VoidFunc;

	if(Aratio_imp > 0)
		m_pfImpervious = &TUrban::CalcImpervious;
	else
		m_pfImpervious = &TUrban::VoidFunc;

	m_pRain = m_sClimate.FindSeries(FT_PRECIP);
	if(m_pRain)
		m_pfnGetNextRain = &TUrban::GetNextRain;
	else
		m_pfnGetNextRain = &TUrban::VoidFloat;

	m_pPev = m_sClimate.FindSeries(FT_EVA);
	if(m_pPev)
		m_pfnGetNextEV = &TUrban::GetNextEV;
	else
		m_pfnGetNextEV = &TUrban::VoidFloat;

	m_pfGW = &TUrban::CalcGW;

	m_nCount = 0;
	theta = theta_per;
	gwE_t = gwE;
	gw_intake = 0;
	surf_add = 0;
	gw_leakage = 0;
	intake_rate_dt = ConvertDay2Step(gw_intake_rate);
	gw_move_t = 0;
	flow_tot = flow_sf = flow_sf_per = flow_sf_imp = flow_inter = /*flow_inter_per = */ flow_gw = 0;
	infiltrate = 0;
	nAET_imp = nAET_per = nAET_tot = 0;
	nRes_ETo = 0;
	recharge = 0;
	gw_move_in = 0;
	gw_move_out = 0;
	dep_per = dep_imp = 0;
	area_per = Area * Aratio_per;
	area_imp = Area * Aratio_imp;

	inf_rate = 0;
	cum_rain = 0;
	cum_inf = 0;
	cum_excess_rain = 0;
	m_nBefRain = 0;
	bef_inf = 0;
	bef_rate = 0;
	Ke = ks_per / 2;
}

void TUrban::InitResult(unsigned long nTime, int nInterval, int nCount)
{
	const char* pszItemHeader[] = {"Inflow(㎥)", "Rainfall(mm)", "Actual Evapotranspiration(mm)", "Potential Evapotranspiration(mm)", "Flow_Surface(mm)",
				"Flow_Inter(mm)", "Flow_Groundwater(mm)", "Flow_Total(mm)", "Infiltrate(mm)", "Recharge(mm)", "Groundwater Movement(mm)",
				"Soil Moisture Content", "Groundwater Elevation(EL.m)", "dep_imp(mm)", "dep_per(mm)"};

	m_pResult->Clear();

	m_pResult->m_dtStart = nTime;
	m_pResult->m_Header.nInterval = nInterval;
	m_pResult->m_Header.nData = FT_USER;
	m_pResult->m_Header.nCount = nCount;

	// 출력 요소별 설정
	for(int nCol = 0; nCol < (int)(sizeof(pszItemHeader) / sizeof(char*)); nCol++)
	{
		TSeriesItem *pItem = m_pResult->AddItem();
		strcpy_s(pItem->m_Header.szHeader, 50, pszItemHeader[nCol]);
		pItem->m_Header.date = nTime;
		pItem->m_Header.nInterval = nInterval;
		pItem->SetSize(nCount);
	}

	if(Aratio_per == 0)
	{
		m_nImport += m_nLeakage;
		m_nLeakage = 0;
	}
}

void TUrban::AddRecharge(float nRecharge)
{
	float t = (nRecharge * 1000) / Area;
	gwE_t += (t / aqf_S);

	if(t > 0)
		gw_move_in += t;
	else
		gw_move_out += (-t);
}

void TUrban::Calculate(int nStep)
{
	m_nRain = (this->*m_pfnGetNextRain)();
	m_nPev = (this->*m_pfnGetNextEV)();
	m_nTempETo = m_nPev;
	float nLai = (this->*m_pfnGetLAI)();

	if(ET_method == 1)
		m_nPev *= (nLai > 3 ? 1 : nLai / 3);
	else
		m_nPev *= nLai;

	m_pBalance->FindOrCreateNode(m_nID, m_nType)->FindOrCreateYear(m_pDate->nYear, gwE_t, theta);

	m_dtInflow = (m_nInflow + m_nImport) * 1000 / Area;

	(this->*m_pfImpervious)();
	(this->*m_pfPervious)();
	(this->*m_pfGW)();
	CalcAreaVal();

	(this->*m_pfnAddResult)();
	(this->*m_pfnAddBalance)();

	m_nCount++;

	if(m_pLink)
		m_pLink->Calculate(nStep, flow_sf * Area / 1000, flow_inter * Area / 1000, flow_gw * Area / 1000, flow_tot * Area / 1000);
	m_nImport = m_nInflow = m_nSf = m_nGW = m_nInter = m_nLeakage = 0;
	gw_move_in = gw_move_out = 0;
	m_nBefRain = m_nRain;
}

void TUrban::CalcImpervious()
{
	if(dep_imp > m_nPev)
	{
		nAET_imp = m_nPev;
		dep_imp = dep_imp - m_nPev;
	}
	else if(dep_imp > 0)
	{
		nAET_imp = dep_imp;
		dep_imp = 0;
	}
	else
	{
		nAET_imp = 0;
		dep_imp = 0;
	}

	// 코드 확인요!!
	nAET_imp = nAET_imp * Aratio_imp;

	// Inflow(Import양이 m3이므로 mm로 변환 - 10.05.26 변경
	dep_imp += (m_nRain + m_dtInflow);

	if(dep_imp > depC_imp)
	{
		flow_sf_imp = dep_imp - depC_imp;
		dep_imp = depC_imp;
	}
	else
	{
		flow_sf_imp = 0;
	}

	flow_sf_imp = flow_sf_imp * Aratio_imp;
}

void TUrban::CalcGA(float inf_capa)
{
	if(inf_capa > 0)
	{
		float delta_theta, rain_inf, cum_inf_bef;

		cum_rain += m_nRain;
		rain_inf = inf_capa * 3600 / m_nDT;

		delta_theta = theta_s_per * (theta_s_per - theta) / (theta_s_per - theta_r_per);

		inf_rate = Ke * 3600 * ((PSI * delta_theta) / (cum_inf + inf_capa) + 1);
		cum_inf_bef = cum_inf;

		if(inf_rate < rain_inf)
			cum_inf = fabs(NewtonRaphson(cum_inf_bef, delta_theta));
		else
			cum_inf += inf_capa;
		infiltrate = cum_inf - cum_inf_bef;

		excess_rain = inf_capa - infiltrate;
		cum_excess_rain += excess_rain;
	}
	else
	{
		cum_inf = 0;
		cum_rain = 0;
		inf_rate = 0;
		infiltrate = 0;
		excess_rain = 0;
		cum_excess_rain = 0;
	}
}

void TUrban::CalcHorton(float inf_capa)
{
	float fp, ft;

	if(inf_capa == 0)
	{
		excess_rain = 0;
		cum_inf = 0;
		bef_inf = 0;
		infiltrate = 0;
		rain_time = 0;
	}
	else
	{
		float rain_int;

		rain_time += m_nDT;
		rain_int = m_nRain / m_nDT;

		fp = ht_fc + (ht_fo - ht_fc) * exp(-1 * ht_k * rain_time);
		ft = ht_fc * rain_time + (ht_fo - ht_fc) * (1 - exp(-1 * ht_k * rain_time)) / ht_k;

		if(fp > rain_int)
		{
			infiltrate = inf_capa;
			excess_rain = 0;
		}
		else
		{
			infiltrate = ft - bef_inf;
			if(infiltrate > inf_capa)
				infiltrate = inf_capa;
			excess_rain = inf_capa - infiltrate;
		}
		bef_inf = ft;
		cum_inf += infiltrate;
		inf_rate = fp * m_nDT;
	}
}

void TUrban::CalcPervious()
{
	float aqfCapa_per, ku_per;
	float flow_inter_t, rch_per_t;

	CalcPumping();

	// 지표면 증발
	if(dep_per > m_nPev)
	{
		nAET_per = m_nPev;
		dep_per -= m_nPev;
		nRes_ETo = 0;
	}
	else if(dep_per > 0)
	{
		nAET_per = dep_per;
		nRes_ETo = m_nPev - dep_per;
		dep_per = 0;
	}
	else
	{
		nAET_per = dep_per = 0;
		nRes_ETo = m_nPev;
	}

	// 토양층 증발
	if(theta > theta_W_per)
	{
		float t_temp;

		if(ET_method == 2)
		{
			t_temp = CalcAETSwat(m_nPev, GetLAI(), solcov, nRes_ETo, Aratio_per_plant, soil_th_per, theta, theta_FC_per, theta_W_per);

			if(t_temp > nRes_ETo)
			{
				nAET_per += nRes_ETo;
				theta -= (nRes_ETo / soil_th_per);
			}
			else
			{
				nAET_per += t_temp;
				theta -= (t_temp / soil_th_per);
			}
		}
		else
		{
			t_temp = (theta - theta_r_per) * soil_th_per;

			if(t_temp > nRes_ETo)
			{
				nAET_per += nRes_ETo;
				theta -= (nRes_ETo / soil_th_per);
			}
			else if(t_temp > 0)
			{
				nAET_per += t_temp;
				theta = theta_r_per;
			}
			else
			{
				theta = theta_r_per;
			}
		}
	} //end for if(theta_per > theta_W_per)

	nAET_per = nAET_per * Aratio_per;

	// 침투
	flow_sf_per = 0;
	float inf_capa = m_nRain + dep_per + (surf_add * 1000 / area_per) + m_dtInflow;
	if(inf_capa < 0)
		inf_capa = 0;
	dep_per = 0;
	switch(infilt_method)
	{
	case 1:
		CalcGA(inf_capa);
		break;
	case 2:
		CalcHorton(inf_capa);
		break;
	default:
		infiltrate = inf_capa;
		excess_rain = 0;
		break;
	}

	theta += ((infiltrate + (m_nLeakage * 1000 / area_per)) / soil_th_per);
//	theta += ((infiltrate + m_nLeakage) / soil_th_per);


	gwE_t -= (gw_intake * 1000 / (Area * aqf_S)); // aqf_S 적용으로 지하수위 조절
    aqfCapa_per = (aqf_Top - gwE_t) * aqf_S;

	// gw_leakage 단위 변환
	theta += (gw_leakage * 1000 / (area_per)) / soil_th_per;

	// 초기화 시점 확인요.
	recharge = flow_inter = 0;
	DT_loop = m_nDT / m_nLoop;

	if(theta > theta_FC_per)
	{
		for(int nIndex = 0; nIndex < m_nLoop; nIndex++)
		{
			ku_per = CalcKU(theta, theta_s_per, theta_r_per, n_mualem);
			rch_per_t = (ku_per * ks_per * DT_loop);
			flow_inter_t = (ku_per * ksi_per * slope * DT_loop);

			if(theta - (rch_per_t / soil_th_per) <= theta_r_per)
				rch_per_t = (theta - theta_r_per) * soil_th_per;

			if(rch_per_t > aqfCapa_per)
				rch_per_t = aqfCapa_per;

			recharge += rch_per_t;
			theta -= (rch_per_t / soil_th_per);
			aqfCapa_per -= rch_per_t;

			if(theta - (flow_inter_t / soil_th_per) <= theta_r_per)
				flow_inter_t = (theta - theta_r_per) * soil_th_per;

			flow_inter += flow_inter_t;
			theta -= (flow_inter_t / soil_th_per);
		}
	}

	flow_sf_per = theta >= theta_s_per ? (theta - theta_s_per) * soil_th_per : 0;
	infiltrate -= flow_sf_per;

	theta -= (flow_sf_per / soil_th_per);

	flow_sf_per += excess_rain;

	if(flow_sf_per > depC_per)
	{
		flow_sf_per -= depC_per;
		dep_per = depC_per;
	}
	else
	{
		dep_per = flow_sf_per;
		flow_sf_per = 0;
	}

	recharge = recharge * Aratio_per;
	flow_inter *= Aratio_per;
	flow_sf_per *= Aratio_per;
	infiltrate *= Aratio_per;
}

void TUrban::CalcGW()
{
	if(riv_th > 0)
	{
		if(gwE_t > rivE)
			flow_gw =  ((gwE_t  - rivE) / riv_th) *  ku_riv * m_nDT * (Area_riv / Area) ;
		else
		{
			float gw_t1, gw_t2, gw_t;

			gw_t1  =  flow_sf_per + flow_sf_imp + flow_inter;
			gw_t2  =  ku_riv  *  m_nDT * Area_riv / Area;
			gw_t = gw_t1 > gw_t2 ? gw_t2 : gw_t1;
			flow_gw = -gw_t;
		}
	}
	else
		flow_gw = 0;

	gwE_t += (-flow_gw + recharge) / aqf_S;

	if(gwE_t > aqf_Top)
	{
        flow_gw += (gwE_t - aqf_Top) * aqf_S;
		gwE_t = aqf_Top;
    }
}

void TUrban::CalcGWMove(void)
{
	if(m_pGWMove)
	{
		float h, h1, h2, m, slope_t;

		h = (gwE_t - aqf_Bot) / 1000.0f;
		h1 = gwE_t / 1000.0f;
		h2 = m_pGWMove->gwE_t / 1000.0f;

		if(slope_method == 0)
		{
			if(h1 > h2)
				slope_t = slope_aqf;
			else
				slope_t = -slope_aqf;
		}
		else
			slope_t = (h1 - h2) / node_len;

		// 단위가 mm/s로 변경됨.(2009.01.20)
		m = ((Kgw * slope_t * conn_len * h) * aqf_S * m_nDT);
		gw_move_t = m * 1000 / Area;
		AddRecharge(-m);
		m_pGWMove->AddRecharge(m);
	}
}

void TUrban::CalcPumping()
{
	float aqfCapa = (gwE_t - aqf_Bot) * aqf_S * Area;

	gw_intake = aqfCapa * 0.3 > intake_rate_dt ? intake_rate_dt : 0;
	gw_leakage = gw_intake * leakage_rate;
	surf_add = gw_intake - gw_leakage;
}

void TUrban::CalcAreaVal()
{
	//float a_imp = Aratio_imp, a_per = Aratio_per;

	nAET_tot = nAET_imp + nAET_per;
	flow_sf = flow_sf_imp + flow_sf_per;
	flow_tot = flow_sf + flow_inter + flow_gw;
}

float TUrban::GetNextRain(void)
{
	return m_pRain->GetNext();
}

float TUrban::GetNextEV(void)
{
	return m_pPev->GetNext();
}

float TUrban::GetLAI()
{
	return LAI[m_pDate->GetMonth() - 1];
}

void TUrban::AddResult(void)
{
	m_pResult->GetAt(0)->SetValue(m_nCount, m_nInflow + m_nImport);
	m_pResult->GetAt(1)->SetValue(m_nCount, m_nRain);
	m_pResult->GetAt(2)->SetValue(m_nCount, nAET_tot);
	m_pResult->GetAt(3)->SetValue(m_nCount, m_nTempETo);
	m_pResult->GetAt(4)->SetValue(m_nCount, flow_sf);
	m_pResult->GetAt(5)->SetValue(m_nCount, flow_inter);
	m_pResult->GetAt(6)->SetValue(m_nCount, flow_gw);
	m_pResult->GetAt(7)->SetValue(m_nCount, flow_sf + flow_inter + flow_gw);
	m_pResult->GetAt(8)->SetValue(m_nCount, infiltrate);
	m_pResult->GetAt(9)->SetValue(m_nCount, recharge);
	m_pResult->GetAt(10)->SetValue(m_nCount, gw_move_t); // 임시 출력 내용.
	m_pResult->GetAt(11)->SetValue(m_nCount, theta);
	m_pResult->GetAt(12)->SetValue(m_nCount, gwE_t / 1000);
	m_pResult->GetAt(13)->SetValue(m_nCount, dep_imp);
	m_pResult->GetAt(14)->SetValue(m_nCount, dep_per);
}

void TUrban::VoidFunc(void)
{
}

void TUrban::AddBalance(void)
{
	TNodeYear *pYear = m_pBalance->FindOrCreateNode(m_nID, m_nType)->FindOrCreateYear(m_pDate->nYear, gwE_t, theta);
	pYear->Add(m_nRain, m_dtInflow, nAET_imp, nAET_per, flow_sf, flow_inter, flow_gw, 0, recharge, gwE_t, theta, infiltrate, gw_move_out - gw_move_in);
	pYear->Calc(Aratio_per, soil_th_per, aqf_S);
}

float TUrban::VoidFloat(void)
{
	return 1;
}

float TUrban::NewtonRaphson(float nInfilt, float delta)
{
	float nOut = nInfilt;
	double cum, bef_value;
	int n;

	bef_value = nInfilt;

	for(n = 0; n < 100; n++)
	{
		cum = nInfilt + Ke * m_nDT + PSI * delta * log((bef_value + PSI * delta) / (nInfilt + PSI * delta)); // DT 확인 요.
		if(fabs(cum - bef_value) < 0.00001)
		{
			nOut = cum;
			break;
		}
		bef_value = cum;
	}

	return nOut;
}

TPaddy::TPaddy(void)
{
	m_nCanAccept = NODE_ALLOWNONE;
	m_nCanOutput = NODE_ALLOWSINGLE;
	Clear();
	m_nType = NODE_PADDY;
}

TPaddy::~TPaddy(void)
{
}

void TPaddy::Clear(void)
{
	TUrban::Clear();

	irr_start_mon = 4;
	irr_start_day = 1;
	irr_end_mon = 9;
	irr_end_day = 30;
	irr_supply = 5000;
	soil_dr_cf = 0.5;
	surf_dr_cf = 4;
	surf_dr_depth = 0.5;
	udgw_dr_cf = 0;
	surf_dr_ht[0] = surf_dr_ht[1] = surf_dr_ht[2] = surf_dr_ht[9] = surf_dr_ht[10] = surf_dr_ht[11] = 0;
	surf_dr_ht[3] = 10;
	surf_dr_ht[4] = 30;
	surf_dr_ht[5] = 60;
	surf_dr_ht[6] = 50;
	surf_dr_ht[7] = 80;
	surf_dr_ht[8] = 20;

	gw_intake_rate = 0.1;
	leakage_rate = 0;

	dr_radius = 50;
	dr_coef_A = 0;
	dr_coef_B = 0.7;
	dr_switch_rain = 10;
}

void TPaddy::Init(void)
{
	TUrban::Init();
	surf_dr_depth = 0;
	flow_pipe = 0;
}

void TPaddy::InitResult(unsigned long nTime, int nInterval, int nCount)
{
	const char* pszItemHeader[] = {"Inflow(㎥)", "Rainfall(mm)", "Actual Evapotranspiration(mm)", "Potential Evapotranspiration(mm)", "Flow_Surface(mm)",
									"Flow_Inter(mm)", "Flow_Groundwater(mm)", "Surf Drainage Culvert(mm)", "Surf Drainage Pipe(mm)", "Flow_Total(mm)",
									"Infiltrate(mm)", "Recharge(mm)", "Groundwater Movement(mm)", "Soil Moisture Content", "Ponding Depth(mm)", "Groundwater Elevation(EL.m)"};

	m_pResult->Clear();

	m_pResult->m_dtStart = nTime;
	m_pResult->m_Header.nInterval = nInterval;
	m_pResult->m_Header.nData = FT_USER;
	m_pResult->m_Header.nCount = nCount;

	// 출력 요소별 설정
	for(int nCol = 0; nCol < (int)(sizeof(pszItemHeader) / sizeof(char*)); nCol++)
	{
		TSeriesItem *pItem = m_pResult->AddItem();
		strcpy_s(pItem->m_Header.szHeader, 50, pszItemHeader[nCol]);
		pItem->m_Header.date = nTime;
		pItem->m_Header.nInterval = nInterval;
		pItem->SetSize(nCount);
	}
}

void TPaddy::Calculate(int nStep)
{
	m_nRain = m_pRain ? m_pRain->GetNext() : 0.0f;
	m_nPev = m_pPev ? m_pPev->GetNext() : 0.0f;
	m_nTempETo = m_nPev;
	infiltrate = 0;
	float nLai = (this->*m_pfnGetLAI)();

	if(ET_method == 1)
		m_nPev *= (nLai > 3 ? 1 : nLai / 3);
	else
		m_nPev *= nLai;

	m_dtInflow = (m_nInflow + m_nImport) * 1000 / Area;
	m_pBalance->FindOrCreateNode(m_nID, m_nType)->FindOrCreateYear(m_pDate->nYear, gwE_t, theta);

	CalcPumping();
	CalcImpervious();
	CalcPaddy();
	CalcAreaVal();

	(this->*m_pfnAddResult)();
	(this->*m_pfnAddBalance)();

	m_nCount++;

	if(m_pLink)
		m_pLink->Calculate(nStep, flow_sf * Area / 1000, flow_inter * Area / 1000, flow_gw * Area / 1000, flow_tot * Area / 1000);

	m_nImport = m_nInflow = m_nSf = m_nGW = m_nInter = m_nLeakage = 0;
}

void TPaddy::CalcPaddy(void)
{
	float paddy_area;
	float ku_per, flow_inter_t, rch_per_t, aqfCapa_per;
	float surf_dr_ht_t = surf_dr_ht[m_pDate->GetMonth() - 1];
	int nCurDay = m_pDate->GetDays(m_pDate->GetYear(), m_pDate->GetMonth(), m_pDate->GetDay());
	int nStartDay = TDate::GetDays(m_pDate->GetYear(), irr_start_mon, irr_start_day);
	int nEndDay = TDate::GetDays(m_pDate->GetYear(), irr_end_mon, irr_end_day);
	BOOL bIrrigate = FALSE;

	if(Aratio_per == 0)
		return;

	// paddy 면적
	paddy_area = Aratio_per * Area;

	// 관계기간 확인
	if(nCurDay >= nStartDay && nCurDay <= nEndDay)
		bIrrigate = TRUE;
	else
		surf_dr_ht_t = depC_per;

	// 담수심 계산. 담수심을 mm로 변환하기 위해 1000배.
	if(m_nRain > 0)	// 강우시에 (관개기간상관 없이)
	{
		dt_inflow = m_dtInflow;
		surf_dr_depth += (m_nRain + m_dtInflow);
	}
	else if(bIrrigate) // 무강우시며 관개기간인경우 지하수 pumping, 유입량을 담수에 추가
	{
		dt_inflow = m_dtInflow;
		surf_dr_depth += ((surf_add) * 1000 / Area) + m_dtInflow;
	}
	else
	{
		gw_intake = 0;
		gw_leakage = 0;
		surf_add = 0;
		dt_inflow = 0;
	}

	if(paddy_area > 0)
	{
		// (2010.06.29) pumping으로 인한 지하수위 변동 추가.
		gwE_t -= (gw_intake * 1000 / (paddy_area * aqf_S));
		// 지하수 펌핑의 누수를 토양에 추가 (10.07.27)
		theta += (gw_leakage * 1000 / (paddy_area * soil_th_per));
		// Import의 누수를 토양에 추가 (10.07.27)
		theta += (m_nLeakage * 1000 / (paddy_area * soil_th_per));
	}

	// 파이프 배수량 계산 추가 (2010.07.13)
	if(m_nRain > dr_switch_rain && dr_radius > 0 && surf_dr_depth > 0)
	{
		// angle : 호의 각도(radian), l : 호의 변의 길이, d : 담수심(담수심이 파이프의 지름보다 높을 경우 파이프의 지름으로 변경)
		float angle, l, d = surf_dr_depth;
		const double pi = 3.1415926535897932384626433832795;
		const double g = 9806.65; // mm/s2
		float Aorf = 0;

		if(surf_dr_depth < dr_radius)
		{
			angle = acos((dr_radius - surf_dr_depth) / dr_radius);
			l = sqrt(2 * dr_radius * surf_dr_depth - surf_dr_depth * surf_dr_depth); // 밑변 길이의 반
			Aorf = dr_radius * dr_radius * angle - l * (dr_radius - surf_dr_depth); // 호넓이 - 하단삼각형 넓이
		}
		else if(surf_dr_depth == dr_radius)
		{
			Aorf = pi * dr_radius * dr_radius / 2; // 반원의 넓이
		}
		else
		{
			d = surf_dr_depth > 2 * dr_radius ? 2 * dr_radius : surf_dr_depth; // 담수심이 파이프 직경보다 높을 경우 파이프 직경으로 변경.
			angle = acos((d - dr_radius) / dr_radius);
			l = sqrt(2 * dr_radius * d - d * d);
			Aorf = (pi * dr_radius * dr_radius * (2 * pi - angle) / (2 * pi)) + l * (d - dr_radius);  // 호넓이 + 상단 삼각형 넓이
		}

		flow_pipe = sqrt(2 * g * surf_dr_depth) * Aorf * dr_coef_A * dr_coef_B * m_nDT;	// 차수를 맞추기 위해선 계산시간(m_nDT)를 곱해줘야 하는 것 아닌지. (그렇게 되면 값이 너무 커짐)
		flow_pipe /= (1000000 * Area * Aratio_per);	// 단위가 부피가 되기 때문에 면적으로 나눠서 mm로 변경함.
	}
	else
		flow_pipe = 0;
	surf_dr_depth -= flow_pipe;

	// 담수심이 물꼬높이 보다 높으면 surface flow으로 유출
	flow_sf_per = surf_dr_depth > surf_dr_ht_t ? surf_dr_cf * sqrt(surf_dr_depth - surf_dr_ht_t) : 0;
	// 2010.06.29 sf가 dr_depth보다 클 경우 처리 추가.
	if(flow_sf_per > surf_dr_depth)
	{
		flow_sf_per = surf_dr_depth;
		surf_dr_depth = 0;
	}
	else
		surf_dr_depth -= flow_sf_per;

	// 실 증발산 계산
	if(surf_dr_depth > m_nPev)
	{
		nAET_per = m_nPev;
		surf_dr_depth -= m_nPev;
		nRes_ETo = 0;
	}
	else if(surf_dr_depth > 0)
	{
		nAET_per = surf_dr_depth;
		nRes_ETo = m_nPev - surf_dr_depth;
		surf_dr_depth = 0;
	}
	else
	{
		nAET_per = 0;
		surf_dr_depth = 0;
		nRes_ETo = m_nPev;
	}

	// 토양층 증발산
	if(theta > theta_W_per)
	{
		float t_temp;

		if(ET_method == 2)
		{
			t_temp = CalcAETSwat(m_nPev, GetLAI(), solcov, nRes_ETo, Aratio_per_plant, soil_th_per, theta, theta_FC_per, theta_W_per);

			if(t_temp > nRes_ETo)
			{
				nAET_per += nRes_ETo;
				theta -= (nRes_ETo / soil_th_per);
			}
			else
			{
				nAET_per += t_temp;
				theta -= (t_temp / soil_th_per);
			}
		}
		else
		{
			t_temp = (theta - theta_r_per) * soil_th_per;

			if(t_temp > nRes_ETo)
			{
				nAET_per += nRes_ETo;
				theta -= (nRes_ETo / soil_th_per);
			}
			else if(t_temp > 0)
			{
				nAET_per += t_temp; //new
				theta = theta_r_per;
			}
			else // 문제있음.
			{
				theta = theta_r_per;
			}
		}
	} //end for if(theta_per > theta_W_per)

	nAET_per *= Aratio_per;

	// 담수심 침투
	float ks_temp = ks_per * m_nDT;

	// 토양수분이 s_per보다 클 수 없으므로 임의 추가.
	if(ks_temp > (theta_s_per - theta) * soil_th_per)
		ks_temp = (theta_s_per - theta) * soil_th_per;

	if(surf_dr_depth > ks_temp)
	{
		infiltrate  = ks_temp;
		surf_dr_depth -= infiltrate;
	}
	else
	{
		infiltrate = surf_dr_depth;
		surf_dr_depth = 0;
	}
	theta += infiltrate / soil_th_per;

	// 토양층 암거배수
	ku_per = CalcKU(theta, theta_s_per, theta_r_per, n_mualem);
	if(m_nRain > 0 && bIrrigate) // 강우가 있을시
		soil_dr_pipe = min(ku_per, udgw_dr_cf * (float)sqrt(surf_dr_depth + soil_th_per * theta / theta_s_per));
	else
		soil_dr_pipe = 0;

	theta -= soil_dr_pipe / soil_th_per;

	// 지하수 함양량 계산 - Pervious 동일
	aqfCapa_per = (aqf_Top - gwE_t) * aqf_S;
	recharge = flow_inter = 0;

	DT_loop = m_nDT / m_nLoop;

	if(theta > theta_FC_per)
	{
		for(int nIndex = 0; nIndex < m_nLoop; nIndex++)
		{
			ku_per = CalcKU(theta, theta_s_per, theta_r_per, n_mualem);
			rch_per_t = ku_per * ks_per * DT_loop;
			flow_inter_t = ku_per * ksi_per * slope * DT_loop;

			if(theta - (rch_per_t / soil_th_per) <= theta_r_per)
				rch_per_t = (theta - theta_r_per) * soil_th_per;

			if(rch_per_t > aqfCapa_per)
				rch_per_t = aqfCapa_per;

			recharge += rch_per_t;
			theta -= (rch_per_t / soil_th_per);
			aqfCapa_per -= rch_per_t; // 추후 지하수위로 나눔. new

			if(theta - (flow_inter_t / soil_th_per) <= theta_r_per)
				flow_inter_t = (theta - theta_r_per) * soil_th_per;

			flow_inter += flow_inter_t;
			theta -= (flow_inter_t / soil_th_per);
		}
	}

	flow_inter += soil_dr_pipe;
	flow_sf_per += flow_pipe;

	// 담수심이 입력치보다 높을 경우 suface flow로 흘려 보냄. (2011.03.07에 추가. 확인 요망)
	if(surf_dr_depth > surf_dr_ht_t)
	{
		flow_sf_per += (surf_dr_depth - surf_dr_ht_t);
		surf_dr_depth = surf_dr_ht_t;
	}

	recharge *= Aratio_per;
	flow_inter *= Aratio_per;
	flow_sf_per *= Aratio_per;
	soil_dr_pipe *= Aratio_per;
	flow_pipe *= Aratio_per;

	CalcGW();
}

void TPaddy::AddResult(void)
{
	m_pResult->GetAt(0)->SetValue(m_nCount, m_nInflow + m_nImport);
	m_pResult->GetAt(1)->SetValue(m_nCount, m_nRain);
	m_pResult->GetAt(2)->SetValue(m_nCount, nAET_tot);
	m_pResult->GetAt(3)->SetValue(m_nCount, m_nTempETo);
	m_pResult->GetAt(4)->SetValue(m_nCount, flow_sf);
	m_pResult->GetAt(5)->SetValue(m_nCount, flow_inter);
	m_pResult->GetAt(6)->SetValue(m_nCount, flow_gw);
	m_pResult->GetAt(7)->SetValue(m_nCount, soil_dr_pipe);
	m_pResult->GetAt(8)->SetValue(m_nCount, flow_pipe);
	m_pResult->GetAt(9)->SetValue(m_nCount, flow_sf + flow_inter + flow_gw);
	m_pResult->GetAt(10)->SetValue(m_nCount, infiltrate);
	m_pResult->GetAt(11)->SetValue(m_nCount, recharge);
	m_pResult->GetAt(12)->SetValue(m_nCount, gw_move_t); // 임시 출력 내용.
	m_pResult->GetAt(13)->SetValue(m_nCount, theta);
	m_pResult->GetAt(14)->SetValue(m_nCount, surf_dr_depth);
	m_pResult->GetAt(15)->SetValue(m_nCount, gwE_t / 1000);
}

void TPaddy::AddBalance(void)
{
//	TNodeYear *pYear = m_pBalance->FindOrCreateNode(m_nID)->FindOrCreateYear(m_Date.nYear, gwE_t, theta, recharge);
	TNodeYear *pYear = m_pBalance->FindOrCreateNode(m_nID, m_nType)->FindOrCreateYear(m_pDate->nYear, gwE_t, theta);
	pYear->Add(m_nRain, dt_inflow, nAET_imp, nAET_per, flow_sf, flow_inter, flow_gw, 0, recharge, gwE_t, theta, infiltrate, gw_move_out - gw_move_in);
	pYear->Calc(Aratio_per, soil_th_per, aqf_S);
}

float TPaddy::CalcKU2(float theta_t, float theta_s, float theta_r, int mualem)
{
	float result;

	if(theta_t > theta_s)
		result = 1;
	else if(theta_t > theta_r)
		result = pow((theta_s - theta) / (theta_s - theta_r), mualem);
	else
		result = 0;

	return result;
}

int TUrban::LoadText(FILE *fp)
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
		//strncpy_s(szToken, szLine, (cFind - &szLine[0]));
		//strcpy_s(szToken2, cFind + 1);
		strncpy(szToken, szLine, (cFind - &szLine[0]));
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
		else if(strcmp(szToken, "Topology") == 0)
		{
			ReadValue(szToken2, nVals, 7);

			Area = nVals[0];
			slope = nVals[1];
			Aratio_imp = nVals[2];
			Aratio_per_plant = nVals[4];
			depC_imp = nVals[5];
			depC_per = nVals[6];
		}
		else if(strcmp(szToken, "Soil") == 0)
		{
			ReadValue(szToken2, nVals, 9);

			theta_per = nVals[0];
			soil_th_per = nVals[1];
			theta_s_per = nVals[2];
			theta_r_per = nVals[3];
			theta_W_per = nVals[5];
			theta_FC_per = nVals[4];
			ks_per = nVals[6];
			ksi_per = nVals[7];
			n_mualem = nVals[8];
		}
		else if(strcmp(szToken, "River") == 0)
		{
			ReadValue(szToken2, nVals, 8);

			gwE = nVals[0];
			rivE = nVals[1];
			riv_th = nVals[2];
			ku_riv = nVals[3];
			Area_riv = nVals[4];
			aqf_S = nVals[5];
			aqf_Top = nVals[6];
			aqf_Bot = nVals[7];
		}
		else if(strcmp(szToken, "Infiltro") == 0)
		{
			ReadValue(szToken2, nVals, 5);
			infilt_method = nVals[0];
			PSI = nVals[1];
			ht_fc = nVals[2];
			ht_fo = nVals[3];
			ht_k = nVals[4];
		}
		else if(strcmp(szToken, "Intake") == 0)
		{
			ReadValue(szToken2, nVals, 2);

			gw_intake_rate = nVals[0];
			leakage_rate = nVals[1];
		}
		else if(strcmp(szToken, "GWout") == 0)
		{
			ReadValue(szToken2, nVals, 6);

			gw_move_node = nVals[0];
			slope_method = nVals[1];
			slope_aqf = nVals[2];
			node_len = nVals[3];
			conn_len = nVals[4];
			Kgw = nVals[5];
		}
		else if(strcmp(szToken, "Evaporation") == 0)
		{
			ReadValue(szToken2, nVals, 14);

			solcov = nVals[0];
			ET_method = nVals[1];
			LAI[0] = nVals[2];
			LAI[1] = nVals[3];
			LAI[2] = nVals[4];
			LAI[3] = nVals[5];
			LAI[4] = nVals[6];
			LAI[5] = nVals[7];
			LAI[6] = nVals[8];
			LAI[7] = nVals[9];
			LAI[8] = nVals[10];
			LAI[9] = nVals[11];
			LAI[10] = nVals[12];
			LAI[11] = nVals[13];
		}
		else if(strcmp(szToken, "Weather") == 0)
		{
			char *cFind = strchr(szToken2, ',');
			int nPos = 0;

			m_nClimates = 0;
			//sscanf_s(szToken2, "%d", &m_nClimates);
			sscanf(szToken2, "%d", &m_nClimates);
			while(cFind)
			{
				char *cFind2 = strchr(cFind + 1, ':');
				char *cFind3 = strchr(cFind2 + 1, ';');
				m_Climates[nPos].nID = atol(cFind + 1);
				m_Climates[nPos].nRain = atol(cFind2 + 1);
				if(cFind3)
					m_Climates[nPos].nEva = atol(cFind3 + 1);
				else
					m_Climates[nPos].nEva = m_Climates[nPos].nRain;
				nPos++;

				cFind = strchr(cFind + 1, ',');
			}
			m_nClimates = nPos;
		}
		else
		{
			ExtraFileContent(szLine);
		}
	}

	return 1;
}

int TPaddy::LoadText(FILE *fp)
{
	float nVals[20];
	char szToken[100];
	char szToken2[1024];
	char *cFind;
	char szLine[1024];

	infilt_method = 0;
	PSI = 0;
	ht_fc = 0;
	ht_fo = 0;
	ht_k = 0;

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
		else if(strcmp(szToken, "Topology") == 0)
		{
			ReadValue(szToken2, nVals, 7);

			Area = nVals[0];
			slope = nVals[1];
			Aratio_imp = nVals[2];
			Aratio_per = nVals[3];
			Aratio_per_plant = nVals[4];
			depC_imp = nVals[5];
			depC_per = nVals[6];
		}
		else if(strcmp(szToken, "Soil") == 0)
		{
			ReadValue(szToken2, nVals, 9);

			theta_per = nVals[0];
			soil_th_per = nVals[1];
			theta_s_per = nVals[2];
			theta_r_per = nVals[3];
			theta_W_per = nVals[4];
			theta_FC_per = nVals[5];
			ks_per = nVals[6];
			ksi_per = nVals[7];
			n_mualem = nVals[8];
		}
		else if(strcmp(szToken, "River") == 0)
		{
			ReadValue(szToken2, nVals, 8);

			gwE = nVals[0];
			rivE = nVals[1];
			riv_th = nVals[2];
			ku_riv = nVals[3];
			Area_riv = nVals[4];
			aqf_S = nVals[5];
			aqf_Top = nVals[6];
			aqf_Bot = nVals[7];
		}
		else if(strcmp(szToken, "Intake") == 0)
		{
			ReadValue(szToken2, nVals, 2);

			gw_intake_rate = nVals[0];
			leakage_rate = nVals[1];
		}
		else if(strcmp(szToken, "GWout") == 0)
		{
			ReadValue(szToken2, nVals, 6);

			gw_move_node = nVals[0];
			slope_method = nVals[1];
			slope_aqf = nVals[2];
			node_len = nVals[3];
			conn_len = nVals[4];
			Kgw = nVals[5];
		}
		else if(strcmp(szToken, "Evaporation") == 0)
		{
			ReadValue(szToken2, nVals, 14);

			solcov = nVals[0];
			ET_method = nVals[1];
			LAI[0] = nVals[2];
			LAI[1] = nVals[3];
			LAI[2] = nVals[4];
			LAI[3] = nVals[5];
			LAI[4] = nVals[6];
			LAI[5] = nVals[7];
			LAI[6] = nVals[8];
			LAI[7] = nVals[9];
			LAI[8] = nVals[10];
			LAI[9] = nVals[11];
			LAI[10] = nVals[12];
			LAI[11] = nVals[13];
		}
		else if(strcmp(szToken, "Weather") == 0)
		{
			char *cFind = strchr(szToken2, ',');
			int nPos = 0;

			m_nClimates = 0;
			//sscanf_s(szToken2, "%d", &m_nClimates);
			sscanf(szToken2, "%d", &m_nClimates);
			while(cFind)
			{
				char *cFind2 = strchr(cFind + 1, ':');
				char *cFind3 = strchr(cFind2 + 1, ';');
				m_Climates[nPos].nID = atol(cFind + 1);
				m_Climates[nPos].nRain = atol(cFind2 + 1);
				if(cFind3)
					m_Climates[nPos].nEva = atol(cFind3 + 1);
				else
					m_Climates[nPos].nEva = m_Climates[nPos].nRain;
				nPos++;

				cFind = strchr(cFind + 1, ',');
			}
			m_nClimates = nPos;
		}
		else if(strcmp(szToken, "Irrigation") == 0)
		{
			ReadValue(szToken2, nVals, 4);

			irr_start_mon = (int)nVals[0];
			irr_start_day = (int)nVals[1];
			irr_end_mon = (int)nVals[2];
			irr_end_day = (int)nVals[3];
		}
		else if(strcmp(szToken, "Coefficient") == 0)
		{
			ReadValue(szToken2, nVals, 6);

			irr_supply = nVals[0];
			surf_dr_cf = nVals[1];
			surf_dr_depth = nVals[2];
			soil_dr_cf = nVals[3];
			udgw_dr_cf = nVals[4];
		}
		else if(strcmp(szToken, "Drain") == 0)
		{
			ReadValue(szToken2, nVals, 12);
			memcpy(surf_dr_ht, nVals, sizeof(float) * 12);
		}
		else
			ExtraFileContent(szLine);
	}

	return 1;
}

BOOL TUrban::RemoveClimate(TBaseNode* pClimate)
{
	int nIndex;
	bool bChange = false;

	for(nIndex = 0; nIndex < 5; nIndex++)
	{
		if(m_Climates[nIndex].pNode == pClimate)
		{
			int nRow;

			for(nRow = nIndex; nRow < 4; nRow++)
			{
				memcpy(&m_Climates[nRow], &m_Climates[nRow + 1], sizeof(NODECOMPOSITE));
			}
			m_nClimates--;
			bChange = true;
		}
	}

	return bChange;
}
