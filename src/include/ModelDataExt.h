#pragma once

#include "../StdAfx.h"
#ifdef WINDLL
  #ifdef MODELDATA_EXPORTS
    #define MODELDATA_CLASS __declspec(dllexport)
  #else
    #define MODELDATA_CLASS __declspec(dllimport)
  #endif
#else
	#define MODELDATA_CLASS
#endif

#define FT_PRECIP 1
#define FT_EVA 2
#define FT_RHUMI 4
#define FT_SOLAR 8
#define FT_TEMPAVG 16
#define FT_WIND 32
#define FT_USER 64
#define FT_TEMPMIN 128
#define FT_TEMPMAX 256
#define FT_EVACALC 512
#define FT_DATE 1024
#define FT_OBSERVE 2048

#define TI_MIN1 1
#define TI_MIN5 5
#define TI_MIN6 6
#define TI_MIN10 10
#define TI_MIN15 15
#define TI_MIN30 30
#define TI_HOUR 60
#define TI_DAY 1440
#define TI_MONTH 44640
#define TI_YEAR 525600

#define TC_SUM 0
#define TC_AVG 1
#define TC_MIN 2
#define TC_MAX 3

typedef struct _tagTimeData
{
	unsigned short nYear;
	BYTE nMonth;	//!< 1-12
	BYTE nDay;		//!< 1-31
	BYTE nHour;		//!< 0-23
	BYTE nMinute;	//!< 0-59
} TIMEDATA;

/*
FILE HEADER DEFINITION

FILETYPE : 2BYTE
DESCRIPT : 100BYTE
DATATYPE : 2BYTE
INTERVALTYPE : 2BYTE
Start Time : 5BYTE
DATACOUNT : 4BYTE
*/

#if !defined(DATATYPE)
#define DATATYPE float
#endif
#define HEADERSIZE 50
#define DESCRIPTSIZE 100

// version 0.1
typedef struct _tagSeriesDump
{
	unsigned short nFile;		//!< 파일종류
	char szDescript[100];		//!< 파일설명
	DWORD dwVersion;			//!< 파일버전
	unsigned short nInterval;	//!< 시간간격
	TIMEDATA date;				//!< 시작시간
	unsigned int nCount;		//!< 데이타 수
} SERIESDUMP;

// version 0.2
typedef struct _tagSeriesHeaderOld
{
	unsigned short nMagic;
	unsigned short nFile;		//!< 파일종류
	char szDescript[DESCRIPTSIZE];		//!< 파일설명
	DWORD dwVersion;			//!< 파일버전
	unsigned short nInterval;	//!< 시간간격
	ULONG date;				//!< 시작시간
	unsigned short nColumn;		//!< column 수
	unsigned int nCount;		//!< 데이타 수
	BYTE Reserved[16];
} SERIESHEADEROLD;

typedef struct _tagSeriesItemHeader
{
	unsigned short nData;
	unsigned short nInterval;
	ULONG date;
	unsigned int nCount;
	char szHeader[HEADERSIZE];
	BYTE Reserved[18];
} SERIESITEMHEADER;

typedef struct _tagSeriesHeader
{
	unsigned short nData;		//!< 데이타종류
	unsigned short nInterval;	//!< 시간간격
	ULONG nTime;
	unsigned int nCount;		//!< 데이타 수
	char szDescript[DESCRIPTSIZE];		//!< 파일설명
	unsigned short nColumn;		//!< column 수
	BYTE Reserved[26];
} SERIESHEADER;

typedef struct _tagSeriesesHeader
{
	unsigned short nMagic;
	DWORD dwVersion;
	ULONG nTime;
	unsigned short nInterval;
	unsigned char nPartNo;
	unsigned char nPartTotal;
	char szName[50];
	char szDescript[DESCRIPTSIZE];
	unsigned short nSeries;
	BYTE Reserved[16];
} SERIESESHEADER;
