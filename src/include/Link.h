#pragma once

#include "BaseNode.h"

class CATMODEL_CLASS TLink : public TBaseNode
{
	typedef float (TLink::*ROUTEMETHOD)(int, float);
public:
	TLink(void);
	virtual ~TLink(void);

public:
	void Clear(void);
	void Init(void);
	void Calculate(int nStep, float nSf, float nInter, float nGW, float nTotal);
	void AddImport(int nStep, float nImport, float nLeakage);
	TBaseNode* GetStart() {return m_pStart;};
	TBaseNode* GetEnd() {return m_pEnd;};
	BOOL SetNode(TBaseNode* pStart, TBaseNode* pEnd);
	virtual void operator =(TBaseNode &node);
	void InitResult(unsigned long nTime, int nInterval, int nCount);
	int IsBalance(void) {return 0;};
	virtual int LoadText(FILE* fp);

public:
	int Method;
	float DT;
//	int nDT;
	float X;
	float K;
	float Delta_dis;
	float Routing_So;
	float Routing_N;
	float Peak_t;
	float Routing_B;
	float mann; //!< Kinematic start
	float init_route;
	float slope_riv;
	float length_riv;
	float Bottom_riv;
	float Top_riv;
	float depth_riv; //!< 계산 가능?
	int DT_loop;
	int m_nStartID;
	int m_nEndID;
	TBaseNode *m_pStart;
	TBaseNode *m_pEnd;

private:
	// 공통
	float m_nInput[4];
	float m_nOutput[4];
	float m_nPrevIn[4];
	float m_nPrevOut[4];

	float *m_pnPartIn;
	float *m_pnPartOut;
	int m_nPartCount;
	float m_nC;
	// Muskingum
	float m_nConst1;
	float m_nConst2;
	float m_nConst3;
	// Kinematic
	float m_nALF1;
	float m_nALFA;
	float m_nBM1;
	float m_nKIMAX;
	float m_nML;
	float m_nYRV1[4];
	float m_nUnitRiv;
	float m_nQMax[4];

protected:
	ROUTEMETHOD m_pfRoute;
	float Direct(int nData, float nInput);
	float Cunge(int nData, float nInput);
	float Kinematic(int nData, float nInput);
	float Kinematic2(int nData, float nInput);
	float Muskingum(int nData, float nInput);
	void KinematicSub(int nData);
	float KinematicSub(float nIn, float nOut, float nPrevIn, float nPrevOut);
	float Kinematic3(int nData, float nInput);
	float Celerity(float nQMax);

public:
	int ValidateCheck(void);
};
