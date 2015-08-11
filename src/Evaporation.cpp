#include "StdAfx.h"
#include "include/Evaporation.h"
#include <math.h>

const float PI = 3.1415926535897932384626433832795f;

TEvaporation::TEvaporation(void)
{
	m_nYearDays = 365;
	m_nDay = 1;
	m_nPress = 1;
	m_nElev = 1;
	m_nDepWind = 1;
	m_nPsyCon = 1;
	m_nLatitude = 37.5f;
}

TEvaporation::~TEvaporation(void)
{
}

int TEvaporation::CalcEvap(TSeriesItem *pWind, TSeriesItem *pTempMin, TSeriesItem *pTempMax, TSeriesItem *pRHum, TSeriesItem *pSolar, TEvaCvtBase *pConvert)
{
	int nIndex, nCount;
	float nEva;
	TDate time;
	TEvaCvtBase baseCvt;
	TEvaCvtBase *pCvt;
	TDate date;

	if(pConvert == NULL)
		pCvt = &baseCvt;
	else
		pCvt = pConvert;

	Clear();

	memcpy(&m_Header, &(pWind->m_Header), sizeof(m_Header));
	m_Header.nData = FT_EVACALC;
	m_Header.nInterval = TI_DAY;
	strcpy_s(m_Header.szHeader, HEADERSIZE, "calculated eval");

	time = m_Header.date;
	pWind->SetFirstPos(m_Header.date);
	pTempMin->SetFirstPos(m_Header.date);
	pTempMax->SetFirstPos(m_Header.date);
	pRHum->SetFirstPos(m_Header.date);
	pSolar->SetFirstPos(m_Header.date);

	nCount = pWind->GetCount();

	m_nDay = 1;
	m_nYearDays = TDate::GetYearDays(time.nYear);

	for(nIndex = 0; nIndex < nCount; nIndex++)
	{
		nEva = PenmonMontieth(pWind->GetNext(), pTempMin->GetNext(), pTempMax->GetNext(), pRHum->GetNext() / 100, pSolar->GetNext());
		nEva = pCvt->Convert(&time, nEva);
		Add(nEva);
		time.IncreaseTime(2);
	}

	return 0;
}

float TEvaporation::PenmonMontieth(float nWind, float nTempMin, float nTempMax, float nRHumMean, float nSolarHour)
{
	float nWind2, nTempMean, nDelta, nEs, nEa;
	float nTemp1, nTemp2, nTemp3, nTemp4, nTemp5;
	float nGso, nRad, nDr;
	float nRa, nRs, nRso, nRns, nRnl, nRn, nRaDum, nWs, nG;
	float nEto;

	nWind2 = nWind * 4.87f / log(67.8f * m_nDepWind - 5.42f);
	m_nPress = float(101.3f * pow(((293 - 0.0065f * m_nElev) / 293), 5.26f));
	nTempMean = (nTempMin + nTempMax) / 2;
	nDelta = 4098 * (0.6108f * exp((17.27f * nTempMean) / (nTempMean + 237.3f))) / pow((nTempMean + 237.3f), 2);
	m_nPsyCon = (0.001013f * m_nPress) / (0.622f * 2.45f);

	nTemp1 = nDelta / (nDelta + m_nPsyCon * (1 + 0.34f * nWind2));
	nTemp2 = m_nPsyCon / (nDelta * (1 + 0.34f * nWind2));
	nTemp3 = 900 / (nTempMean + 273) * nWind2;

	nEs = ((0.6108f * exp((17.27f * nTempMax) / (nTempMax + 237.3f))) +
		(0.6108f * exp((17.27f * nTempMin) / (nTempMin + 237.3f)))) / 2;
	nEa = nRHumMean  * nEs;

	nTemp4 = nEs - nEa;

	nGso = 0.082f;
	nRad = PI / 180 * m_nLatitude;
	nDr = 1 + 0.033f * cos((2 * PI) * m_nDay / m_nYearDays);
	nRaDum = 0.409f * sin((2 * PI) * m_nDay / m_nYearDays - 1.39f);
	nWs = acos(-tan(nRad) * tan(nRaDum));
	nRa = 24 * 60 / PI * nGso * nDr * (nWs * sin(nRad) * sin(nRaDum) + cos(nRad) * cos(nRaDum) * sin(nWs));
	nRs = (0.25f + 0.5f * (nSolarHour / (24 / PI * nWs))) * nRa;
	nRso = (0.75f + 0.00002f * m_nElev) * nRa;
	nRns = (1 - 0.23f) * nRs;
	nRnl = 4.903f * 0.0000000001f * ((pow((nTempMax + 273.16f), 4) + pow((nTempMin + 273.16f), 4)) / 2)
		* (0.34f - 0.14f * sqrt(nEa)) * (1.35f * (nRs / nRso) - 0.35f);
	nRn = nRns - nRnl;
	nG = 0;

	nTemp5 = 0.408f * (nRn - nG);

	nEto = nTemp1 * nTemp5 + nTemp2 * nTemp3 * nTemp4;

	return nEto;
}

int TEvaporation::CalcEvap2(TSeriesItem* pWind, TSeriesItem* pTempMin, TSeriesItem* pTempMax, TSeriesItem* pRHum, TSeriesItem* pSolar)
{
	int nIndex, nCount;
	float nEva;
	TDate time;
	TDate date;
	TSeriesItem TempMin, TempMax, Wind, Solar, Humi;

	if(pTempMin != pTempMax)
	{
		pTempMin->ConvertInterval(TI_DAY, TC_AVG, &TempMin);
		pTempMax->ConvertInterval(TI_DAY, TC_AVG, &TempMax);
	}
	else
	{
		pTempMin->ConvertInterval(TI_DAY, TC_MIN, &TempMin);
		pTempMax->ConvertInterval(TI_DAY, TC_MAX, &TempMax);
	}
	pWind->ConvertInterval(TI_DAY, TC_AVG, &Wind);
	pSolar->ConvertInterval(TI_DAY, TC_SUM, &Solar);
	pRHum->ConvertInterval(TI_DAY, TC_AVG, &Humi);

	Clear();

	memcpy(&m_Header, &(pWind->m_Header), sizeof(m_Header));
	m_Header.nData = FT_EVACALC;
	m_Header.nInterval = TI_DAY;
	strcpy_s(m_Header.szHeader, HEADERSIZE, "calculated eval");

	time = m_Header.date;
	Wind.SetFirstPos(m_Header.date);
	TempMin.SetFirstPos(m_Header.date);
	TempMax.SetFirstPos(m_Header.date);
	Humi.SetFirstPos(m_Header.date);
	Solar.SetFirstPos(m_Header.date);

	Wind.SetFirstPos(0);
	TempMin.SetFirstPos(0);
	TempMax.SetFirstPos(0);
	Humi.SetFirstPos(0);
	Solar.SetFirstPos(0);

	nCount = Wind.GetCount();

	m_nDay = 1;
	m_nYearDays = TDate::GetYearDays(time.nYear);

	for(nIndex = 0; nIndex < nCount; nIndex++)
	{
		nEva = PenmonMontieth(Wind.GetNext(), TempMin.GetNext(), TempMax.GetNext(), Humi.GetNext() / 100, Solar.GetNext());
		Add(nEva);
		time.IncreaseTime(2);
		m_nDay++;
		if(m_nDay > m_nYearDays)
		{
			m_nYearDays = TDate::GetYearDays(time.nYear);
			m_nDay = 1;
		}
	}

	return 0;
}

int TEvaporation::CalcEvap(TSeries* pSeries)
{
	TSeriesItem *pWind, *pTemp, *pHumi, *pSolar, *pTempMin, *pTempMax;

	pWind = pSeries->FindSeries(FT_WIND);
	pTemp = pSeries->FindSeries(FT_TEMPAVG);
	pTempMin = pSeries->FindSeries(FT_TEMPMIN);
	pTempMax = pSeries->FindSeries(FT_TEMPMAX);
	pSolar = pSeries->FindSeries(FT_SOLAR);
	pHumi = pSeries->FindSeries(FT_RHUMI);

	if(pWind && (pTemp || (pTempMin && pTempMax)) && pSolar && pHumi)
	{
		if(pTempMin && pTempMax)
			return CalcEvap2(pWind, pTempMin, pTempMax, pHumi, pSolar);
		else
			return CalcEvap2(pWind, pTemp, pTemp, pHumi, pSolar);
	}

	return 0;
}
