#include "StdAfx.h"
#include "include/EvaCvtBase.h"
#include <math.h>


TEvaCvtBase::TEvaCvtBase(void)
{
}

TEvaCvtBase::~TEvaCvtBase(void)
{
}

TEvaMonthConvert::TEvaMonthConvert()
{
	for(int nIndex = 0; nIndex < 12; nIndex++)
		m_nRates[nIndex] = 1.0f;
}

TEvaMonthConvert::~TEvaMonthConvert()
{
}

void TEvaMonthConvert::SetParameter(float *pVals)
{
	memcpy(m_nRates, pVals, sizeof(float) * 12);
}

float TEvaMonthConvert::Convert(TDate *pTime, float val)
{
	return m_nRates[pTime->nMonth - 1] * val;
}

void TEvaMonthConvert::SetParameter(int nIndex, float val)
{
	m_nRates[nIndex] = val;
}

float TEvaSwatConverter::Convert(TDate *pTime, float val)
{
	return 0;
/*
	if(LAI > 3.0)
		ET_t = res_Eto;
	else
		ET_t = res_Eto * LAI / 3.0;

	ET_t *= Aratio_per_plant;
	Cov = exp(-0.00005 * (solcov + 0.1));
	m_nET_s = res_ETo * Cov;
	temp = m_nET_s * (res_ETo / ((m_nET_S) + ET_t + VU10)); // 확인필요
	m_nET_s = min(m_nET_s, temp); // 확인필요
	m_nET_s = max(m_nET_s, 0);

	if(res_ETo < m_nET_s + ET_t)
	{
		m_nET_s = ETo * m_nET_s / (m_nET_s + ET_t);
		ET_t = ETo * ET_t / (m_nET_s + ET_t);
	}

	if(theta_per < theta_FC_per)
	{
		m_nET_s = m_nET_s * exp(2.5 * (theta_per - theta_FC_per) / (theta_FC_per - theta_W_per));
		m_nET_s = min(m_nET_s, 0.8 * (theta_per - theta_W_per) * soil_th);
		m_nAET_per = ET_t + m_nET_s;
	}
*/
}

float TEvaFAOConverter::Convert(TDate *pTime, float val)
{
	return 0;
}
