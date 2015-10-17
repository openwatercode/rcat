#pragma once

#include "BaseNode.h"

class CATMODEL_CLASS TJunc : public TBaseNode
{
public:
	TJunc(void);
	virtual ~TJunc(void);

public:
	TBaseNode *pOutput;
	unsigned short nStep;
	NODECOMPOSITE m_Drain;	//!< 합류식으로 가는 양의 입력값은 m3/day

public:
	virtual void operator =(TBaseNode &node);
	void InitResult(ULONG nTime, int nInterval, int nCount);
	void Calculate(int nStep);
	void Init(void);
	void Clear(void);
	int IsBalance(void) {return 0;};
	virtual int LoadText(FILE *fp);

protected:
	float step_drain;
	float flow_drain;

protected:
	void AddResult(int nStep);
};
