#pragma once

#include "ModelDataExt.h"
#include "SeriesData.h"

class MODELDATA_CLASS TEvaCvtBase
{
public:
	TEvaCvtBase(void);
	virtual ~TEvaCvtBase(void);

public:
	virtual float Convert(TDate* pTime, float val) {return val;};
};

class MODELDATA_CLASS TEvaMonthConvert : public TEvaCvtBase
{
public:
	TEvaMonthConvert();
	~TEvaMonthConvert();

public:
	virtual float Convert(TDate* pTime, float val);
	void SetParameter(float *pVals);
	void SetParameter(int nIndex, float val);

private:
	float m_nRates[12];
};

class MODELDATA_CLASS TEvaSwatConverter : public TEvaCvtBase
{
public:

public:
	virtual float Convert(TDate* pTime, float val);
};

class MODELDATA_CLASS TEvaFAOConverter : public TEvaCvtBase
{
public:

public:
	virtual float Convert(TDate* pTime, float val);
};
