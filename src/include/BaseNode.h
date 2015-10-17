/*
작성자 : 차영룡
목  적 : 노드의 기본적인 특성을 포함한 상위 클래스
설  명 : 각 노드는 TBaseNode를 상속받아 생성된다.
*/

#pragma once

#include "CatModelExt.h"
#include "SeriesData.h"
#include "WaterBalance.h"

/**
* 노드 종류
* 2바이트를 사용하며
* 상위 1바이트는 노드의 종류로서 소스(Catchment), 처리(Treatment), 정션, 기상등으로 분류하며
* 하위 1바이트로 세부 노드 종류를 설정한다.
*/

#define NODE_SOURCE 0x0100
#define NODE_TREATMENT 0x0200
#define NODE_JUNCTION 0x0400
#define NODE_CLIMATE 0x0800
#define NODE_MODEL 0x1000
#define NODE_LINKAGE 0x2000

#define NODE_URBAN 0x0101
#define NODE_FOREST 0x0102
#define NODE_PADDY 0x0104

#define NODE_IMPORT 0x0201
#define NODE_INFILTRO 0x0202
#define NODE_WETLAND 0x0204
#define NODE_POND 0x0208
#define NODE_RAINTANK 0x0211
#define NODE_RECYCLE 0x0212
#define NODE_BIORETENTION 0x0240

#define NODE_JUNC 0x0401
#define NODE_OUTLET 0x0402

#define NODE_LINK 0x2001

#define NODE_ALLOWNONE 0
#define NODE_ALLOWSINGLE 1
#define NODE_ALLOWMULTI 2

class TBaseNode;
class TLink;
class TUrban;

/**
* @brief 다중 노드 정보
* @details 여러 노드의 정보가 필요한 경우 사용한다.
*/
typedef struct _tagNodeComposite
{
	int nID;			//!< 노드의 ID
	TBaseNode* pNode;	//!< 노드의 포인터
	int nMethod;		//!< 계산방법. (sum, avg ...)
	float nRain;		//!< 강우 기상자료 사용 비율 (0~1)
	float nEva;			//!< 증발 기상자료 사용 비율 (0~1)
} NODECOMPOSITE;


/**
* 노드 최상위 클래스.
*/
class CATMODEL_CLASS TBaseNode
{
public:
	/// 생성자.
	TBaseNode(void);

	/// 소멸자.
	virtual ~TBaseNode(void);

	// 오버로드할 함수 모음
public:
	/// 노드 초기화.
	virtual void Clear();
	/// 계산 초기화.
	virtual void Init();
	/**
	* @brief 해당순서의 계산을 수행한다
	* @param nStep : 계산순서를 나타내는 인덱스. (사용하지 않음)
	*/
	virtual void Calculate(int nStep) {};
	/**
	* @brief 노드 ID 설정.
	* @param nID : 지정할 노드의 ID (정수)
	*/
	virtual void SetID(int nID);
	/**
	* @brief 이전 노드에서 계산된 유출량을 입력받는다
	* @param nIndex : 계산순서 (정수)
	* @param nSf : 지표수 유출량 (실수)
	* @param nGW : 지하수 유출량 (실수)
	* @param nInter : 중간 유출량 (실수)
	* @param nTotal : 전체 유출량 (실수).
	*/
	virtual void AddValue(int nIndex, float nSf, float nGW, float nInter, float nTotal);
	/**
	* @brief Import 노드에서 입력받는다
	* @param nIndex : 계산숫서 (정수)
	* @param nImport : 유입량 (실수)
	* @param nLeakage : 손실량 (실수).
	*/
	virtual void AddImport(int nIndex, float nImport, float nLeakage);
	/**
	* @brief =연산자 오버로드.
	* 다른 노드 클래스에서 오버로드되어 노드를 복사할 수 있도록한다.
	* @param node : 복사하고자 하는 원본 노드
	*/
	virtual void operator =(TBaseNode& node);
	/**
	* @brief 계산결과 변수를 초기화 한다.
	* 파라미터는 결과파일 헤더를 초기화하기 위해 사용된다.
	* @param nTime : 모형 계산 시간
	* @param nInterval : 계산 간격
	* @param nCount :
	*/
	virtual void InitResult(ULONG nTime, int nInterval, int nCount) {};
	/**
	* @brief 지하수 이동을 계산한다.
	*/
	virtual void CalcGWMove() {};
	/**
	* @brief 물수지 계산여부를 지정한다.
	* 각 노드에서 이 함수를 재지정하여 물수지 계산을 할지를 결정한다.
	* @return 0 : 물수지 계산 안함, 1 : 소스노드 물수지, 2 : 처리노드 물수지
	*/
	virtual int IsBalance() {return 1;};
	/**
	* @brief 노드의 면적을 돌려준다.
	*/
	virtual float GetArea() {return 0;};
	/**
	* @brief 텍스트 파일 포맷의 입력자료를 읽어들이다.
	* @param fp : 파일 포인터. 텍스트 입력파일을 읽기 모드로 열어야 한다.
	*/
	virtual int LoadText(FILE *fp) {return 0;};
	/**
	* @brief 텍스트 파일 자료를 읽는 도중 기본적으로 알수없는 내용이 나올 때 호출된다.
	* 클래스를 확장하여 추가 자료가 필요할 때 사용하기 위한 가상함수이다.
	* @param szLine : LoadText 함수에서 처리하지 못한 줄을 돌려준다.
	*/
	virtual int ExtraFileContent(char* szLine) {return 0;};

public:
	/** 노드의 이름을 반환한다.
	* @brief 노드 속성에서 입력된 노드 이름
	* @return 함수의 이름을 포함한 문자열
	*/
	char* GetName() {return m_szName;};
	/** 노드 설명을 반환한다.
	* @brief 노드 속성에서 입력된 노드 설명
	* @return 함수의 설명을 포함한 문자열
	*/
	char* GetDesc() {return m_szDesc;};
	/** @brief 노드의 이름을 설정한다.
	* @param szName : 설정할 노드 이름
	*/
	void SetName(char* szName);
	/** @brief 노드의 설명을 설정한다.
	* @param szDesc : 설정할 노드의 설명
	*/
	void SetDesc(char* szDesc);
	/** @brief 노드의 이름을 설정한다. (유니코드)
	* @param szName : 설정할 노드 이름
	*/
	void SetName(wchar_t* szName);
	/** @brief 노드의 설명을 설정한다. (유니코드)
	* @param szDesc : 설정할 노드의 설명
	*/
	void SetDesc(wchar_t* szDesc);
	/** @brief 노드의 ID 번호를 돌려준다.
	* @brief 각 노드의 유일한 ID 번호를 가지고 있으며, 각 노드의 연결은 이 ID를 이용한다.
	* @return 4바이트 정수형 ID 값
	*/
	int GetID() {return m_nID;};
	/** @brief 노드의 타입을 돌려준다.
	* @brief 모든 노드 종류는 별도의 형태값을 가지고 있으며 상위 1바이트는 구분, 하위 1바이트는 종류를 나타낸다.
	* @brief 상위 1바이트는 소스(Catchment), 처리(Treatment), 정션, 기상등으로 분류하며
	* @brief 하위 1바이트는 노드 세부 분류를 나타낸다. 따라서 Urban 노드의 경우 0x0101로 나타난다.
	* @return 4바이트 정수형 노드 형태이나 하위 2바이트만 사용
	*/
	int GetType() {return m_nType;};
	/** @brief 노드의 상위 종류를 돌려준다.
	* @brief 소스, 처리, 정션, 기상 노드인지를 알려준다.
	* @return 4q바이트 정수. 하위 2바이트만 사용
	*/
	int GetMajorType() {return (m_nType & 0xff00);};
	/** @brief 노드의 기본이름 설정
	* @brief 각 노드의 기본이름을 설정해준다. SetName을 통해 이름을 변경한다.
	* @param nNo : 기본이름 뒤에 붙을 숫자
	*/
	void SetDefName(int nNo);
	/** @brief 노드가 링크의 출력부분과 연결 가능한지 여부 반환
	* @brief 링크 연결시 이 함수의 반환값이 1이면 링크의 출력에 연결가능하다.
	* @return 4바이트 정수. 0이면 링크의 출력에 연결할 수 없고, 1이면 연결가능한다.
	*/
	int CanAccept() {return m_nCanAccept;};
	/** @brief 노드가 링크의 입력으로 사용가능한지 여부 반환
	* @brief 링크 연결시 이 함수의 반환값이 1이면 링크의 입력에 연결가능하다.
	* @return 4바이트 정수. 0이면 링크의 입력에 연결할 수 없고, 1이면 연결가능한다.
	*/
	int CanOutput() {return m_nCanOutput;};
	/** @brief 연결된 링크를 반환한다.
	* @brief 노드의 출력과 연결된 링크를 반환한다.
	* @brief 각 노드는 계산된 결과를 여기서 반환된 링크로 유출한다.
	* @return 연결된 링크노드 포인터
	*/
	TLink* GetLink() {return m_pLink;};
	/** @brief 링크를 노드와 연결한다.
	* @brief 유출 과정에 맞춰 링크를 연결한다.
	* @param pLink : 연결할 링크 노드 포인터
	*/
	void SetLink(TLink* pLink) {m_pLink = pLink;};
	/** @brief 현재 노드와 링크로 연결된 다음 노드를 돌려준다.
	* @return 연결된 노드의 포인터
	*/
	TBaseNode* GetOutput();
	/** @brief 일 단위 값을 계산단위값으로 변경한다.
	* @param nVal : 변환하고자 하는 값
	* @return 계산시간 간격으로 변환된 실수형 값
	*/
  //inline float ConvertDay2Step(float nVal);
	float ConvertDay2Step(float nVal);
	/** @brief 모형실행 시간을 설정한다.
	* @param pDate : 설정하고자 하는 시간
	*/
	void SetDate(TDate* pDate) {m_pDate = pDate;};
	/** @brief 파일에서 읽은 자료를 해석한다.
	* @param szLine : 텍스트 파일에서 읽어들은 한줄
	* @param pVals : 텍스트를 해석해서 실수로 변환 내요
	* @param nCount : 자료의 수
	*/
	static int ReadValue(char* szLine, float *pVals, int nCount);
	/**
	* @brief 입력된 문자가 공백 또는 줄바꿈 문자인지 확인한다.
	* @param c : 확인하고자 하는 문자
	* @return 입력된 문자가 공백 또는 줄바꿈이면 TRUE, 아니면 FALSE
	*/
	static BOOL IsWhiteChar(char c);
	/**
	* @brief 입력된 문자열의 앞뒤 공백 및 라인피드를 제거한다.
	* @param szToken : Trim하고자 하는 문자열
	*/
	static void Trim(char *szToken);
	/**
	* @brief 지정한 파일이 존재하는지 확인한다.
	* @param szFile : 찾고자 하는 파일명
	*/
	static BOOL IsFileExist(char* szFile);

protected:
	/**
	* @brief KU 계산
	* @brief 입력받은 값을 이용하여 KU 계산을 수행하고 값을 돌려준다.
	* @param theta_t : 현재 수분량
	* @param theta_s : 최고 수분량
	* @param theta_r : 최저 수분량
	* @param mualem : mualem 계수.
	* @return 계산된 KU값.
	*/
	float CalcKU(float theta_t, float theta_s, float theta_r, int mualem);
	/**
	* @brief AET 증발산 계산
	* @param nETo
    * @param nPlantRatio
    * @param soil_th
    * @param soil_theta
    * @param soil_FC
    * @param soil_W
	* @param nPev 잠재증발산
	* @param nLai 식생변수
	* @param covsol : .
	*/
	float CalcAETSwat(float nPev, float nLai, float covsol, float nETo, float nPlantRatio, float soil_th, float soil_theta, float soil_FC, float soil_W);

public:
	char m_szName[100];			//!< 노드명 변수
	char m_szDesc[200];			//!< 노드설명 변수
	char m_szDefName[50];		//!< 기본 노드명 형태
	int m_nID;					//!< 노드 ID
	int m_nType;				//!< 노드 종류
	TSeries *m_pResult;			//!< 결과 저장
	int m_nDT;					//!< 계산 간격
	int m_nLoop;				//!< 모형실행 iteration
	TWaterBalance *m_pBalance;	//!< 물수지 계산 결과

protected:
	int m_nCanAccept;			//!< 링크의 Out부분 연결 가능 여부
	int m_nCanOutput;			//!< 링크의 Input 부분 가능 여부
	TLink *m_pLink;				//!< 연결될 링크
	float m_nSf;				//!< Surface flow
	float m_nGW;				//!< Groundwater flow
	float m_nInter;				//!< Interflow
	float m_nInflow;			//!< 유입량
	float m_nImport;			//!< Import된 유량
	float m_nLeakage;			//!< 손실량
	TDate* m_pDate;				//!< 계산날짜
};

/**
* @brief 기상노드
* @brief 기상자료를 관리하기 위한 노드
*/
class CATMODEL_CLASS TClimate :
  public TBaseNode
{
public:
	TClimate(void);
	virtual ~TClimate(void);

public:
	/**
	* @brief 모형 실행환경 설정
	* @brief 설정된 환경에 맞춰 계산용 자료를 생성
	* @param nStart : 모형 시작 시간
	* @param nEnd : 모형 종료 시간
	* @param nInterval : 모형 실행 간격
	*/
	void SetModelEnv(ULONG nStart, ULONG nEnd, unsigned short nInterval);
	/**
	* @brief 기상자료 파일에서 값을 읽어온다.
	* @param szFile : 기상자료가 저장된 파일명
	*/
	void SetClimateFileA(char* szFile);
	/**
	* @brief 기상자료 파일에서 값을 읽어온다. (유니코드용)
	* @param szFile : 기상자료가 저장된 파일명
	*/
	void SetClimateFileW(wchar_t* szFile);
	/**
	* @brief 기상자료 파일에서 값을 읽어온다.
	* @param szFile : 기상자료가 저장된 파일명
	*/
	void SetClimateFile(LPCTSTR szFile);
	void SetEvaFileA(char* szFile);
	void SetEvaFileW(wchar_t* szFile);
	/**
	* @brief 증발산 자료파일을 지정한다.
	* @param szFile : 증발산 자료가 저장된 파일명
	*/
	void SetEvaFile(LPCTSTR szFile);
	/**
	* @brief 설정된 기상자료 파일명을 반환한다.
	* @return 기상자료 파일명이 포함된 문자열
	*/
	void GetClimateFile(LPTSTR szFile);
	/**
	* @brief 설정된 증발산 자표 파일명을 반환한다.
	* @return 증발산 자료 파일명이 포함된 문자열
	*/
	void GetEvaFile(LPTSTR szFile);
	/**
	* @brief 파일에서 기상자료 및 증발산 자료를 읽어들인다.
	* 강우 및 증발산 자료를 읽어서 지정한 값으로 변환한다.
	*/
	int LoadSeries(void);
	virtual void operator =(TBaseNode &node);												//!< =연산자 정의 (노드 복사)
	int IsBalance() {return 0;};															//!< 기상노드는 물수지 계산 없음
	virtual int LoadText(FILE* fp);

public:
	TSeries m_sClimateSrc;																	//!< 기상자료 원본
	TSeries m_sClimate;																		//!< 모형 계산을 위한 기상자료
	bool m_bUseCalc;																		//!< 증발산 계산 여부
	char m_szClimate[MAX_PATH];																//!< 기상자료 파일
	char m_szEva[MAX_PATH];																	//!< 증발산 자료 파일
	float m_nLat;																			//!< 위도
	float m_nHeight;																		//!< 고드
	float m_nWindHeight;																	//!< 풍속계 높이
};
