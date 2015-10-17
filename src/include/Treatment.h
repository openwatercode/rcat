#pragma once
#include "Catchment.h"

/**
* @brief Import 노드
* @brief 지정한양만큼의 유량 또는 시계열 자료로 설정된 유량을 내보낸다.
*/
class CATMODEL_CLASS TImport :
	public TBaseNode
{
	typedef void (TImport::*CALCSTEP)(int);
public:
	TImport(void);
	virtual ~TImport(void);

public:
	void Clear(void);
	void Init(void);
	void Calculate(int nStep);
	/**
	* @brief 시계열 자료 파일을 설정한다.
	* @param szFile : 시계열 자료 파일명이며, 시계열 자료 포맷이어야 한다.
	*/
	void SetSeriesFileA(char* szFile);
	/**
	* @brief 시계열 자료 파일을 설정한다.(유니코드)
	* @param szFile : 시계열 자료 파일명이며, 시계열 자료 포맷이어야 한다.
	*/
	void SetSeriesFileW(wchar_t* szFile);
	/**
	* @brief 시계열 자표 파일이 설정되어 있고, 유출 형태가 시계열인경우 자료를 읽는다.
	*/
	void LoadSeries(void);
	void InitResult(ULONG nTime, int nInterval, int nCount);
	int IsBalance() {return 0;}
	virtual int LoadText(FILE *fp);

protected:
	/**
	* @brief 유출 형태가 Contant인 경우 호출되며 동일한 값으로 유출한다.
	*/
	void CalcConst(int nStep);
	/**
	* @brief 유출 형태가 Series인 경우 호출되며 파일에 저장되어 있는 값이 유출된다.
	*/
	void CalcSeries(int nStep);
	/**
	* @brief 아무 계산도 하지 않으며 유출값은 0이다.
	*/
	void CalcNone(int nStep) {};

public:
	int type;					//!< 유출 형태 0 : constant, 1 : time series;
	TSerieses m_Series;			//!< 시계열 자료
	int m_nTable;				//!< 시계열자료의 테이블 번호
	int m_nData;				//!< 시계열자료의 필드 번호
	char m_szSeries[MAX_PATH];	//!< 시계열 자료 파일명
	float m_nConst;				//!< Constant인 경우 유출량
	float m_nLeakage;			//!< 누출량

protected:
	TSeriesItem *m_pData;		//!< 시계열자료
	CALCSTEP m_pfCalc;
	float m_nOut;				//!< 유출량
	float m_nConst_dt;			//!< 일유출량 한계
};

/**
* @brief Infiltro 노드 클래스
* @brief 침투시설에 처리를 위한 클래스
*/

class CATMODEL_CLASS TInfiltro :
	public TBaseNode
{
public:
	TInfiltro(void);			//!< 생성자
	virtual ~TInfiltro(void);	//!< 소멸자

public:
	void Calculate(int nStep);
	void Clear(void);
	void Init(void);
//	void AddValue(int nIndex, float nSf, float nGW, float nInter, float nTotal);
//	void CalcGWMove(void);
	void InitResult(ULONG nTime, int nInterval, int nCount);
	virtual float GetArea() {return area;};
	int IsBalance() {return 2;};
	virtual int LoadText(FILE *fp);

protected:
	void AddResult(int nStep);
	void AddBalance(void);

public:	// 입력변수
	float area;				//!< 면적
	float aqf_top;			//!< 대수층 상단 표고
	float aqf_S_coef;		//!< 대수층 저류계수
	float potential;		//!< 최대침투능 m3/day
	float aqf_bot;			//!< 대수층 하단 표고
	int in_node_id;			//!< 외부 유입 노드 ID
	TCatchment *m_pInNode;	//!< 외부 유입 노드 포인터
	TBaseNode *m_pGWMove;	//!< 지하수 이동 노드
	int DT;

	// 지하수 유거 변수
	int gw_node_id;			//!< 지하수 이동시 상대 노드 ID (0 : none)
	int slope_method;		// 사용유무 확인 필요
	float slope_aqf;		//!< 대수층 기울기
	float node_len;			//!< 인접노드와의 거리
	float conn_len;			//!< 인접 노드와의 접경 거리
	float Kgw;				//!< 지하수 이동 계수

protected:	// 계산시 사용되는 변수
	float aqf_cap;			//!< 대수층 잉여공극량
	float surf_inf;			//!< 지표면 유입량
	float surf_out;			//!< 지표면 유출량
	float gwE;				//!< 지하수위
	float infiltro_inf;		//!< 침투량 (함양량)
	float potential_L;		//!< 잠재 유출량 저장 변수
	float gw_move_t;		//!< 지하수 이동 량
};

/**
** @brief BioRetential 노드 클래스
* 침투녹지 처리 클래스. Infiltration에 식생이 포함되어 있는 형태와 유사하다.
*/
class CATMODEL_CLASS TBioRetention : public TInfiltro
{
public:
	TBioRetention(void);			//!< 생성자
	~TBioRetention(void);			//!< 소멸자

public:
	void Calculate(int nStep);
	void Clear(void);
	void Init(void);
	void InitResult(ULONG nTime, int nInterval, int nCount);
	int IsBalance() {return 2;};
	virtual int LoadText(FILE *fp);

public:
	NODECOMPOSITE m_Climates[5];	//!< 기상자료 노드 목록
	int m_nClimate;					//!< 기상자료 노드 수
	TSeries m_sClimate;				//!< 기상자료
	float LAI[12];					//!< 증발산 계수

protected:
	void AddResult(int nStep);
	void AddBalance();
	float GetLAI(void);

private:
	TSeriesItem* m_pRain;			//!< 강우 시계열 자료
	TSeriesItem* m_pPev;			//!< 증발 시계열 자료
	float surf_ev;					//!< 증발량
	float m_nRain;					//!< 강우량
	float m_nPev;					//!< 잠재 증발산량
};

/**
* @brief WetLand 노드 클래스
* 습지 노드. 유입량이 크면 초과분은 하류로 유출하고 나머지 보관
*/

class CATMODEL_CLASS TWetLand :
	public TBaseNode
{
public:
	TWetLand(void);
	virtual ~TWetLand(void);

public:
	void Calculate(int nStep);
	void Clear(void);
	void Init(void);
	void InitResult(ULONG nTime, int nInterval, int nCount);
	/**
	* @brief 저류지의 가능량을 계산하고 유출량을 계산한다.
	* @param in_vol : 유입량
	* @param out_wl : 저수위
	* @param out_area : 저수면적
	*/
	void CalcRate(float in_vol, float &out_wl, float &out_area);
	float GetArea() {return area;};
	int IsBalance() {return 2;};
	virtual int LoadText(FILE *fp);

public:
	float vol_init;					//!< 초기 저류량
	float vol_max;					//!< 최대 저류량
	float bypass;					//!< 홍수기 우회량
	float aqf_ks;					//!< 사용안함
	float pipe_ht;					//!< 방류구 높이
	float pipe_area;				//!< 방류구 단면적
	float pipe_coef;				//!< 방류구 유출 계수
	int rate_count;					//!< 사용안함
	float wl_rate[20][3];			//!< 수위-저수량 관계
	NODECOMPOSITE m_Climates[5];	//!< 기상자료 목록
	int m_nClimate;					//!< 기상자료 수
	TSeries m_sClimate;				//!< 기상자료
	int recharge_id;				//!< 일부 저수량 되돌릴 노드 ID
	TUrban* m_pRechargeNode;		//!< 일부 저수량 되돌릴 노드 포인터

protected:
	float aqf_loss;					//!< 손실량
	float sur_rain;					//!< 지표 강우량
	float bypass_step;				//!< bypass cms단위에서 m3/단위시간으로 변환값
	float bypass_out;				//!< bypass된 값.
	float spill_out;				//!< splillway 유출량
	float area;						//!< 면적
	float pipe_out;					//!< 방류구 유출량
	float surf_inf;					//!< 침투량
	float surf_ev;					//!< 증발량
	float surf_evV;					//!< 잠재증발량
	float vol;						//!< 저류량
	float wl;						//!< 저수위
	float out_flow;					//!< 유출량
	TSeriesItem* m_pRain;			//!< 강우 자료
	TSeriesItem* m_pPev;			//!< 증발산 자료
	float m_nRain;					//!< 현재 강우
	float m_nPev;					//!< 형재 증발산
	float fullArea;					//!< 전체 면적

protected:
	virtual void AddResult(int nStep);
	void AddBalance(void);
};

/**
* @brief Pond 노드 클래스
* 저류시설 노드이며, Offline 및 Online 저류시설로 나누어진다.
*/
class CATMODEL_CLASS TPond :
	public TBaseNode
{
	typedef void (TPond::*CALCULATE)(void);
	typedef float (TPond::*GETINTAKE)(void);
	/**
	* @brief 저류시설의 Offline 및 Online 계산에 사용할 수 있도록한 구조체
	*/
	typedef struct {
		float wl;			//!< 저수위
		float storage;		//!< 저수량
		float spil;			//!< 물넘이
		float pipe;			//!< 방류구
		float total;		//!< 전체 유량
		float si_spil;		//!< 물넘이 유출량
		float si_pipe;		//!< 방류구 유출량
		float si_total;		//!< 전체 유출량
	} WLOUT;
public:
	TPond(void);
	virtual ~TPond(void);

public:
	void Calculate(int nStep);
	void Init(void);
	void Clear(void);
	void InitResult(ULONG nTime, int nInterval, int nCount);
	/**
	* @brief 취수량 시계열 자료 파일을 지정한다.
	* @param szFile : 취수량이 저장된 시계열 자료 파일명
	*/
	void SetSeriesFileA(char* szFile);
	/**
	* @brief 취수량 시계열 자료 파일을 지정한다.(유니코드)
	* @param szFile : 취수량이 저장된 시계열 자료 파일명
	*/
	void SetSeriesFileW(wchar_t* szFile);
	/**
	* @brief 취수방법을 시계열 자료로 선택하고 시계열 자료 파일을 지정했을 경우 자료를 읽어들인다.
	*/
	void LoadSeries(void);
	float GetArea() {return fullArea;};
	int IsBalance() {return 2;};
	virtual int LoadText(FILE *fp);

protected:
	CALCULATE m_pfCalculate;
	GETINTAKE m_pfIntake;
	/**
	* @brief Online 습지 방류를 계산한다.
	* 습지종류에 따라 Calculate에서 호출된다.
	*/
	void CalcOnline(void);
	/**
	* @brief Offline 습지 방류를 계산한다.
	* 습지종류에 따라 Calculate에서 호출된다.
	*/
	void CalcOffline(void);
	/**
	* @brief 계산에 필요한 초기값을 계산한다.
	*/
	inline void CalcInitial(void);
	/**
	* @brief 누출량을 계산한다.
	*/
	inline void CalcLoss(void);
	/**
	* @brief 저수량에 따른 수위를 계산한다.
	*/
	inline float CalcStorage(void);
	/**
	* @brief 유입량에 따른 유출을 계산하고 저수위 및 저류량을 계산한다.
	* @param in_vol : 유입량
	* @param out_wl : 계산된 저수위
	* @param out_area : 계산된 저수면적
	*/
	void CalcRate(float in_vol, float &out_wl, float &out_area);
	void AddResult(int nStep);
	void AddBalance(void);
	/**
	* @brief 수위에 따른 저수량을 계산한다.
	* @param nHeight : 수위
	* @return nHeight에 따른 저수량
	*/
	float CalcVolByHeight(float nHeight);
	/**
	* @brief Constant로 취수량을 지정한 경우 계산 시간에 맞는 취수량을 반환한다.
	*/
	float GetIntakeConst(void);
	/**
	* @brief 취수량을 시계열로 지정한 경우 저장된 시계열에서 취수량을 반환한다.
	*/
	float GetIntakeSeries(void);
	/**
	* @brief Puls를 이용한 저수량 산정을 위한 테이블을 만들어 둔다.
	*/
	void CalcPulsTable(void);
	/**
	* @brief Puls 계산 테이블을 이용하여 저수량을 계산한다.
	*/
	void CalcPuls(void);
	/**
	* @brief 유출량을 계산한다.
	* @param spil : 물넘이 방류량
	* @param pipe : 방류구 방류량
	* @param total : 전체 방류량
	*/
	void CalcOutflow(float &spil, float &pipe, float &total);
	/**
	* @brief Storage EQ.을 사용하여 저수량을 산정한다.
	*/
	void CalcSI(float spil, float pipe, float total, float &si_spil, float &si_pipe, float &si_total);
	/**
	* @brief Storage EQ.을 사용하여 유출량을 산정한다.
	* @param si_spil : 계산된 물넘이 유량
	* @param si_pipe : 계산된 방류구 유량
	* @param si_total : 계산전 전체 유량
	* @param spil : 물넘이 유출량
	* @param pipe : 방류구 유출량
	* @param total : 전체 유출량
	*/
	void GetOutflowFromSI(float si_spil, float si_pipe, float si_total, float &spil, float &pipe, float &total);

public:
	int type;				//!< 습지 계산 방법 (0 : Offline, 1 : OnLine)
	int method;				//!< 유출계산방법. (0 : None, 1 : Pulse, 2 : Modified Pulse)
	float surf_inf;			//!< 저류지 유입량
	float surf_ev;			//!< 저류지 증발량
	float area;				//!< 저류지 면적
	float vol_eff;			//!< 유효저류량
	float spill_out;		//!< 물넘이 방류량
	float spill_ht;			//!< 물넘이 높이
	float spill_length;		//!< 물넘이 길이
	float spill_coef;		//!< 물넘이 월류계수
	float pipe_out;			//!< 방류구 방류량
	float pipe_ht;			//!< 방류구 높이
	float pipe_area;		//!< 방류구 면적
	float pipe_coef;		//!< 방류구 유출계수
	float aqf_ks;			//!< 바닥면 포화투수계수
	float intake_vol;		//!< 취수량
	int rate_count;
	float wl_rate[20][3];	//!< 저수위, 저류량, 면적 관계
	float vol_init;			//!< 초기 저류량
	float aqf_loss;			//!< 누수율
	float offline_max;		//!< offline 유입량 임계치
	float offline_ratio;	//!< offline 본류유량 비율
	float offline_dnQ;		//!< offline 유입유량
	float offline_out;		//!< offline 방류 조건 유량
	float sur_rain;			//!< 면적 강우
	float intake;			//!< 취수량
	NODECOMPOSITE m_Climates[5];	//!< 기상자료 강우, 기상 1개씩만 사용됨.
	int m_nClimate;			//!< 기상자료 수량
	TSeries m_sClimate;		//!< 기상자료
	TSerieses m_sIntake;	//!< 시계열 취수량의 경우 자료
	float intake_vol_t;		//!< 현재 취수량 계산
	int intake_type;		//!< 취수 형태 (Constant : 0, 시계열 : 1)
	int m_nTable;			//!< 시계열 자료의 경우 시계열 파일의 테이블
	int m_nData;			//!< 시계열 자료의 경우 필드번호
	char m_szIntake[MAX_PATH];	//!< 취수량 시계열 파일명
	int supply_id;			//!< 유입노드 ID
	TBaseNode *m_pSupplyNode;	//!< 유입 노드 포인터
	int recharge_id;			//!< Recharge 노드 ID
	TUrban *m_pRechargeNode;	//!< Recharge 노드 포인터

private:
	TSeriesItem* m_pRain;	//!< 강우 시계열 자료
	TSeriesItem* m_pPev;	//!< 증발산 시계열 자료
	TSeriesItem* m_pIntake;	//!< 취수량 시계열 자료
	float wl;				//!< 저수위
	float m_nRain;			//!< 강우
	float m_nPev;			//!< 잠재증발산
	float vol;				//!< 저류량
	float surf_evV;			//!< 증발산량
	float out_flow;			//!< 방류량
	float max_vol;			//!< 최대 저수량
	float offline_max_step;	//!< 계산시간간격의 offline 최대 유입량
	float offline_out_step;	//!< 계산시간간격의 offline 최대 유출량
	WLOUT *m_pTable;		//!< 저류량 산정을 위한 테이블
	int m_nTables;			//!< 저류량 산정 테이블의 자료수
	float prev_inf;			//!< 이전 유입량
	float prev_out;			//!< 이전 유출량
	float prev_spil;		//!< 이전 물넘이량
	float prev_pipe;		//!< 이전 방류구 방류량
	float prev_si_spil;		//!< 이전 Storage 저류량 산정의 물넘이량
	float prev_si_pipe;		//!< 이전 Storage 저류량 산정의 방류구량
	float prev_si_total;	//!< 이전 Storage 저류량 산정의 전체 유출량
	float puls_spil;		//!< 이전 Puls 저류량 산정의 물넘이량
	float puls_pipe;		//!< 이전 Plus 저류량 산정의 방류구량
	float puls_total;		//!< 이전 Puls 저류량 산정의 전체 유출량
	float fullArea;			//!< 저류지 전체 면적
};

/**
* @brief RainTank 클래스
* @brief 빗물저장 탱크의 처리 노드를 구현한다.
*/
class CATMODEL_CLASS TRainTank :
	public TBaseNode
{
	typedef float (TRainTank::*GETUSE)(void);
public:
	TRainTank(void);			//!< 생성자
	virtual ~TRainTank(void);	//!< 소멸자

public:
	void Calculate(int nStep);
	void Clear(void);
	void Init(void);
//	void AddValue(int nIndex, float nSf, float nGW, float nInter, float nTotal);
	void InitResult(ULONG nTime, int nInterval, int nCount);
	int IsBalance() {return 2;};
	virtual int LoadText(FILE *fp);

protected:
	void AddResult(int nStep);
	void AddBalance(void);
	float GetUseConst(void);
	float GetUseSeries(void);
	GETUSE m_pfUse;

public:
	float inf;					//!< 유입량
	float vol;					//!< 저수량
	float vol_min;				//!< 최소유입량
	float use;					//!< 사용량
	float cap;					//!< 저류용량
	float out;					//!< 유출량
	float vol_init;				//!< 초기저수량
	int use_type;				//!< 취수형태 (0 : Constant, 1 : 시계열)
	int m_nTable;				//!< 사용량 시계열 파일의 테이블 번호
	int m_nData;				//!< 사용량 시계열 파일의 필드 번호
	char m_szUse[MAX_PATH];		//!< 사용량 시계열 파일명
	int supply_id;				//!< 유입 노드 ID
	TBaseNode* m_pSupplyNode;	//!< 유입 노드 포인터

protected:
	float spill;				//!< 방류량
	float use_t;				//!< 사용량
	float use_dt;				//!< 사용가능량
	TSerieses m_sUse;			//!< 시계열 사용량 파일 자료
	TSeriesItem *m_pUse;		//!< 시계열 사용량
public:
	void SetSeriesFileA(char* szFile);	//!< 사용량 시계열 파일을 지정한다.
	void SetSeriesFileW(wchar_t* szFile);	//!< 사용량 시계열 파일을 지정한다.
	/*
	* 사용량을 시계열로 설정하고 파일을 지정했을경우 자료를 읽어들인다.
	*/
	void LoadSeries(void);
};

/**
* @brief Recycle 노드 클래스
* @brief 물의 재사용을 위한 처리를 구현한다.
*/
class CATMODEL_CLASS TRecycle :
	public TBaseNode
{
public:
	TRecycle(void);					//!< 생성자
	virtual ~TRecycle(void);		//!< 소멸자

public:
	void Calculate(int nStep);
	void Clear(void);
	void Init(void);
	void InitResult(ULONG nTime, int nInterval, int nCount);
	int IsBalance() {return 2;};
	virtual int LoadText(FILE *fp);

protected:
	void AddBalance(void);
	void AddResult(int nStep);

public:
	NODECOMPOSITE m_Recursive[5];	//!< 재사용 노드 목록. 최대 5곳까지 지정가능한다.
	int m_nRecursive;				//!< 재사용 노드 수
	float rec_cond;					//!< 본류대비 취수 제한율
	float rec_intake;				//!< 취수량
	float rec_intake_dt;			//!< 취수 가능량

protected:
	float surf_inf;					//!< 유입량
	float surf_out;					//!< 유출량
	float surf_node[5];				//!< 재사용 노드로 유출된 유량
};
