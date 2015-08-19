#pragma once

#include "../StdAfx.h"
#ifdef WINDLL
  #ifdef CATMODEL_EXPORTS
    #define CATMODEL_CLASS __declspec(dllexport)
  #else
    #define CATMODEL_CLASS __declspec(dllimport)
  #endif
#else
  #define CATMODEL_CLASS
#endif

#include "SeriesData.h"
#include "WaterBalance.h"

/*
class TUrban;
class TLink;

extern "C"
{
	void SetDefaultInputVar(TUrban& urban);
//	int CalcCATModel(LPVARINPUT pInput);
	void SetDefaultLinkVar(TLink& link);
};
*/
