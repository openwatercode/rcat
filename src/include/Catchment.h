#pragma once

#include "BaseNode.h"

class TLink;

/**
* @brief Urban, Forest, Paddy의 최상위 클래스
* @brief 소스노드의 최상위 클래스이다. 하위 클래스와 통합예정
*/
class CATMODEL_CLASS TCatchment : public TBaseNode
{
public:
	TCatchment(void);			//!< 생성자
	virtual ~TCatchment(void);	//!< 소멸자

public:
	virtual void operator = (TBaseNode& node);
	/** @brief 지하수위 값을 돌려주기 위한 가상함수
	* @brief 연관된 노드에서 현재 노드의 지하수위값을 확인하기 위한 가상함수 - 제거예정
	*/
	virtual float GetGWE(int nStep) {return 0;};

public:
	TSeries m_sClimate;				//!< 기상자료 클래스
	NODECOMPOSITE m_Climates[5];	//!< 기상자료 구성 정보
	int m_nClimates;				//!< 기상자료의 수
	float m_nRain;					//!< 계산시 사용될 강우량
	float m_nPev;					//!< 계산시 사용될 증발산량
};

/**
* @brief Urban, Forest 클래스
* @brief Urban, Forest는 내부적으로 동일한 처리를 함으로 하나의 클래스로 처리한다.
*/
class CATMODEL_CLASS TUrban :
	public TCatchment
{
	typedef void (TUrban::*WRITERESULT)(void);			//!< 결과를 저장하기 위한 함수 포인터
	typedef void (TUrban::*ADDRESULT)(void);			//!< 계산 결과 처리를 위한 함수 포인터
	typedef void (TUrban::*ADDBALANCE)(void);			//!< 물수지 계산을 위한 함수 포인터
	typedef float (TUrban::*GETFLOATWITHVOID)(void);
	typedef void (TUrban::*CALCSUB)(void);
public:
	TUrban(void);			//!< 생성자
	virtual ~TUrban(void);	//!< 소멸자

// 모델 변수
public:
	float Area;					//!< 유역 면적
	float slope;				//!< 유역 기울기
	float Aratio_imp;			//!< 유역내 투수지역 면적 비율
	float Aratio_per;			//!< 유역내 불투수지역 면적 비율
	float Aratio_per_plant;		//!< 유역내 식생 면적 비율
	float depC_imp;				//!< 투수지역 저류능
	float depC_per;				//!< 불투수지역 저류능

	// soil
	float theta_per;			//!< 토양 초기 수분율
	float soil_th_per;			//!< 토양 두께
	float theta_s_per;			//!< 토양 포화 수분율
	float theta_r_per;			//!< 토양 잔류 수분율
	float theta_W_per;			//!< 위조점 이하의 수분함량
	float theta_FC_per;			//!< 포장용수량에서의 수분함량
	float ks_per;				//!< 포화 투수계수
	float ksi_per;				//!< 사면방향 포화 투수계수
	int n_mualem;				//!< Mualem식의 지수값

	// aquifer
	float gwE;					//!< 지하수위
	float rivE;					//!< 하천수위
	float riv_th;				//!< 하천 바닥 L께
	float ku_riv;				//!<
	float Area_riv;				//!< 하천 바닥 면적
	float aqf_S;				//!< 대수층 저류계수
	float aqf_Top;				//!< 대수층 상단 표고
	float aqf_Bot;				//!< 대수층 바닥 표고

	// GWMove
	int gw_move_node;			//!< 지하수 이동시 상대 노드 ID (0 : none)
	int slope_method;
	float slope_aqf;			//!< 지하수면 동수구배
	float node_len;				//!< 인접 소유역과의 거리
	float conn_len;				//!< 인접 소유역과의 접속 거리
	float Kgw;					//!< 대수층 투수계수

	// LAI
	float solcov;
	int ET_method;				//!< 증발산 계산 방법
	float LAI[12];				//!< 증발산 계산 매개변수

	// 지하수 pumping
	float gw_intake_rate;		//!<  지하수 펌핑량
	float leakage_rate;			//!< 지하수 누출량

	int DT_loop;

	// Infiltration
	int infilt_method;			//!< 침투 계산 방법 0 : direct, 1 : Green&Amps, 2 : Horton
	float rain_time;

	// Green&Ampt
	float Ke;					//!< 유효투수계수 (일반적으로 ks_per / 2)
	float PSI;					//!< 습윤선에서의 토양흡인수두
	float cum_rain;				//!< 누적강우
	float cum_inf;				//!< 누적침투
	float cum_excess_rain;		//!< 누적초과강우
	float m_nBefRain;			//!< 이전강우
	float bef_inf;				//!< 이전 침투량
	float bef_rate;
	float inf_rate;
	float excess_rain;
//	float Ku; //!< 임시로 ku값 고정.

	// Horton
	float ht_fc;				//!< 종기 침투능
	float ht_fo;				//!< 초기 침투능
	float ht_k;					//!< 토양 감쇄상수

	float area_per;				//!< 투수 유역 면적
	float area_imp;				//!< 불투수 유역 면적

	TUrban *m_pGWMove;			//!< 지하수가 이동될 노드

	float gwE_t;				//!< 계산된 지하수위

	float m_dtInflow;			//!< 유입량 (mm/단위시간)
//	float m_dtLeakage; //!< 누수량 (mm/단위시간)

// 공용 함수
public:
	void Calculate(int nStep);
	virtual void Init();
	void Clear();
	void CalcGWMove();				//!< 지하수 이동을 계산한다.
	virtual void operator =(TBaseNode &node);
	void InitResult(unsigned long nTime, int nInterval, int nCount);
	float GetGWE(int nStep) {return gwE_t;};	//!< 현재 계산된 지하수위를 반환한다.
	void AddRecharge(float nRecharge);			//!< 다른 노드에서 지하수를 유입시킨다.
	virtual float GetArea() {return Area;};		//!< 노드의 면적을 반환한다.
	virtual int LoadText(FILE *fp);
	//BOOL TUrban::RemoveClimate(TBaseNode* pClimate);
	BOOL RemoveClimate(TBaseNode* pClimate);

// 계산에 필요한 지역 변수
protected:
	float nRes_ETo;					//!< 잠재 증발산
	float nAET_per;					//!< 투수지역 증발산
	float nAET_imp;					//!< 불투수지역 증발산
	float nAET_tot;					//!< 증발산

	TSeriesItem* m_pRain;			//!< 강우 시계열 자료
	TSeriesItem* m_pPev;			//!< 증발산 시계열 자료
	float m_nTempETo;				//!< 증발산 계산용 임시 변수
	int m_nCount;

	float dep_imp;					//!< 계산된 투수지역 저류능
	float dep_per;					//!< 계산된 불투수지역 저류능
	float flow_sf;					//!< 지표유출
	float flow_sf_imp;				//!< 불투수 지표유출
	float flow_sf_per;				//!< 투수 지표유출
	float flow_inter;				//!< 중간유출
	float flow_gw;					//!< 지하유출
	float recharge;					//!< 대수 저장량
	float flow_tot;					//!< 전체 유출량
	float infiltrate;				//!< 침투량
	float theta;					//!< 현재 토양 수분

	// gw pumping
	float gw_intake;				//!< 지하수 사용량
	float surf_add;					//!< 지하수 사용량중 지표로 반환된량
	float gw_leakage;				//!< 지하수 누출량
	float intake_rate_dt;			//!< 하루 사용 가능량

	float gw_move_t;				//!< 지하수 이동량
	float gw_move_in;				//!< 지하수 유입량
	float gw_move_out;				//!< 지하수 유출량

// 내부 계산 함수
protected:
	/** @brief 불투수 지역의 유출을 계산한다.
	* @brief Calculate 함수에서 호출된다.
	*/
	void CalcImpervious();
	/** @brief 불투수 지역의 유출을 계산한다.
	* @brief Calculate 함수에서 호출된다.
	*/
	void CalcPervious();
	/** @brief 대수층 계산을 수행한다.
	* @brief Calculate 함수에서 호출된다.
	*/
	void CalcGW();
	/** @brief 면적관련 계산을 수행한다. 강우량을 cms로 변환등*/
	void CalcAreaVal();
	/** @brief 지하수 사용에 대한 계산을 수행한다. */
	void CalcPumping();
	/** @brief 토양 침투중 Green&Ampt 계산을 수행한다.*/
	void CalcGA(float inf_capa);
	/** @brief 토양 침투중 Horton 계산을 수행한다.*/
	void CalcHorton(float inf_capa);
	ADDRESULT m_pfnAddResult;
	ADDBALANCE m_pfnAddBalance;
	GETFLOATWITHVOID m_pfnGetLAI;
	GETFLOATWITHVOID m_pfnGetNextRain;
	GETFLOATWITHVOID m_pfnGetNextEV;
	CALCSUB m_pfImpervious;
	CALCSUB m_pfPervious;
	CALCSUB m_pfGW;
	/** @brief 다음 계산 시간의 강우량을 반환한다*/
	inline float GetNextRain(void);
	/** @brief 다음 계산 시간의 증발량을 반환한다*/
	inline float GetNextEV(void);
	/** @brief 현재 시간의 LAI값을 반환한다.*/
	float GetLAI(void);
	void VoidFunc(void);
	virtual void AddResult(void);
	virtual void AddBalance(void);
	float VoidFloat(void);
	/** @brief 침투 계산법중 Newton-Raphson을 계산한다. */
	float NewtonRaphson(float nInfilt, float delta);
};

/**
* Paddy 처리 클래스
* @brief Urban을 상속하여 기본 계산은 동일하나 논농사에 대한 계산이 추가되었다.
*/

class CATMODEL_CLASS TPaddy :
	public TUrban
{
public:
	TPaddy(void);			//!< 생성자
	virtual ~TPaddy(void);	//!< 소멸자

public:
	void Calculate(int nStep);
	/**
	* @brief Urban의 계산에서 paddy의 계산이 추가되었다.
	*/
	void CalcPaddy(void);
	void Clear(void);
	virtual void Init(void);
	float GetGWE(int nStep) {return gwE;};
	void InitResult(unsigned long nTime, int nInterval, int nCount);
	void AddBalance(void);
	void AddResult(void);
	float CalcKU2(float theta_t, float theta_s, float theta_r, int mualem);
	virtual int LoadText(FILE *fp);

public:
	float irr_supply;			//!< 논에 유입량
	float surf_dr_ht[12];		//!< 월별 물꼬 높이
	float surf_dr_cf;			//!< 물꼬배수 유출계수
	float soil_dr_cf;			//!<
	float udgw_dr_cf;			//!< 암거배수 유출계수
	int irr_start_mon;			//!< 관계 시작월
	int irr_start_day;			//!< 관계 시작일
	int irr_end_mon;			//!< 관계 종료월
	int irr_end_day;			//!< 관계 종료일
	float surf_dr_depth;
	float nOutDepth;			//!< 물꼬 높이
	float dt_inflow;			//!< 파이프
	float dr_radius;			//!< 파이프 반경
	float dr_coef_A;			//!< 파이프 계수
	float dr_coef_B;			//!< 파이프 계수
	float dr_switch_rain;		//!< 파이프 사용 강우량
	float flow_pipe;			//!< 파이프 유출량

protected:
//	float nIrrSupply;
	float soil_dr_pipe;
};
