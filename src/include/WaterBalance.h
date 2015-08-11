#pragma once

#include "ModelDataExt.h"
#include "arraytempl.h"

class MODELDATA_CLASS TNodeYear
{
public:
	TNodeYear(void);

public:
	void Init(float nGW, float nSoil);
	void Calc(float nAreaPer, float nSoilDepth, float nAqfS);
//	void Add(float nRain, float nETImp, float nETPer, float nTotal, float nSurf, float nInter, float nGW, float nRech, float nGWL, float nSoil);
	void Add(float nRain, float nImport, float nETImp, float nETPer, float nSurf, float nInter, float nGW, float nRes, float nRech, float nGWL, float nSoil, float nInfiltrate, float gw_move);

public:
	int m_nYear;

	double m_nRain;
	double m_nImport;

	double m_nET;
	double m_nET_Imp;
	double m_nET_Per;

	double m_nTotalRunoff;
	double m_nSurfaceRunoff;
	double m_nInterflow;
	double m_nGroundwater;
	double m_nReserv;

	double m_nRecharge;

	double m_nSoilVariation;
	double m_nGroundStorage;
	double m_nErrorBalance;

	double m_nInitGW, m_nLastGW;
	double m_nInitSoil, m_nLastSoil;
	double m_nGWMove;

	double m_nInfiltrate;

	int m_nCount;
};

class MODELDATA_CLASS TNodeBalance : public PTRARRAY<TNodeYear, 10>
{
public:
	TNodeBalance(int nNodeID);
	~TNodeBalance();

public:
	TNodeYear* FindOrCreateYear(int nYear, float nGW, float nSoil);
	int GetNodeID() {return m_nNodeID;};
	int GetStartYear() {return m_nStartYear;};
	int GetEndYear() {return m_nEndYear;};

private:
	int m_nNodeID;
	int m_nStartYear;
	int m_nEndYear;

public:
	char m_szName[100];
	char m_szOutlet[100];
	float m_nArea;
	int m_nType;
};

class MODELDATA_CLASS TWaterBalance : public PTRARRAY<TNodeBalance, 10>
{
public:
	TWaterBalance(void);
	~TWaterBalance(void);

public:
	TNodeBalance* FindOrCreateNode(int nNodeID, int nType);
	int GetStartYear(void);
	int GetEndYear(void);
	TNodeBalance* FindNode(int nNodeID);
};
