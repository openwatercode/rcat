#pragma once

#include "SeriesData.h"
#include "EvaCvtBase.h"

class MODELDATA_CLASS TEvaporation : public TSeriesItem
{
public:
	TEvaporation(void);
	~TEvaporation(void);

public:
	int CalcEvap(TSeriesItem *pWind, TSeriesItem *pTempMin, TSeriesItem *pTempMax, TSeriesItem *pRHum, TSeriesItem *pSolar, TEvaCvtBase *pConvert = NULL);
	int CalcEvap2(TSeriesItem* pWind, TSeriesItem* pTempMin, TSeriesItem* pTempMax, TSeriesItem* pRHum, TSeriesItem* pSolar);

protected:
	float PenmonMontieth(float nWind, float nTempMin, float nTempMax, float nRHumMin, float nSolarHour);

public:
	float m_nDepWind;	//!< 풍속측정 높이
	float m_nPress;		//!< 기압
	float m_nElev;		//!< 해발고도
	float m_nPsyCon;	//!< psychrometric constant
	float m_nLatitude;	//!< 위도

private:
	int m_nDay;
	int m_nYearDays;
public:
	int CalcEvap(TSeries* pSeries);
};
