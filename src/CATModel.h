// CATModel.cpp : DLL 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "include/ModelManager.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

// BOOL APIENTRY DllMain( HMODULE hModule,
//                        DWORD  ul_reason_for_call,
//                        LPVOID lpReserved
// 					 )
// {
//     return TRUE;
// }

/**
* 모형실행을 위한 외부 노출 함수
* @param infile : 모형실행 입력파일(Text)
* @param outfile : 모형결과 저장 파일
* @param format : 모형결과 저장 형식
*             NULL or "" - 결과를 바이너리로 저장
*             [노드명:필드번호1,필드번호2...]... - 노드명의 지정필드를 텍스트파일로 출력
*             [*:*] - 전체 결과를 테스트파일로 출력
*/

int __cdecl Run(char* infile, char* outfile, char* format)
{
	TModelManager model;
	int nRet = 0;
	char szInput[MAX_PATH], szOutFile[MAX_PATH], szFormat[MAX_PATH];
	char szModule[200], *cSlash, *cFind;

	memset(szOutFile, 0, sizeof(szOutFile));
	memset(szFormat, 0, sizeof(szFormat));

#ifdef _UNICODE
	MultiByteToWideChar(CP_ACP, 0, outfile, strlen(outfile), szOutFile, MAX_PATH);
	MultiByteToWideChar(CP_ACP, 0, format, strlen(format), szFormat, MAX_PATH);
#else
	//strcpy_s(szOutFile, outfile);
	//strcpy_s(szFormat, format);
	strcpy(szOutFile, outfile);
	strcpy(szFormat, format);
#endif

	memset(szModule, 0, sizeof(szModule));
	GetModuleFileName(NULL, szModule, MAX_PATH);

	cFind = strrchr(szModule, '\\');
	if(cFind)
		*cFind = '\0';

	cSlash = strstr(infile, ":\\");
	if(cSlash == NULL)
		cSlash = strstr(infile, ":/");

	if(cSlash == NULL)
	{
	  //strcpy_s(szInput, szModule);
	  strcpy(szInput, szModule);

		cFind = strrchr(infile, '\\');
		if(cFind == NULL)
			cFind = strrchr(infile, '/');

		if(cFind)
		  //strcat_s(szInput, cFind);
		  strcat(szInput, cFind);
		else
		{
		  //strcat_s(szInput, "\\");
		  //strcat_s(szInput, infile);
		  strcat(szInput, "\\");
		  strcat(szInput, infile);
		}
	}
	else
	  //strcpy_s(szInput, infile);
	  strcpy(szInput, infile);

	cSlash = strstr(outfile, ":\\");
	if(cSlash == NULL)
		cSlash = strstr(outfile, ":/");

	if(cSlash == NULL)
	{

	  //strcpy_s(szOutFile, szModule);
	  strcpy(szOutFile, szModule);

		cFind = strrchr(outfile, '\\');
		if(cFind == NULL)
			cFind = strrchr(outfile, '/');

		if(cFind)
			//strcat_s(szOutFile, cFind);
			strcat(szOutFile, cFind);
		else
		{
			//strcat_s(szOutFile, "\\");
			//strcat_s(szOutFile, outfile);
			strcat(szOutFile, "\\");
			strcat(szOutFile, outfile);
		}
	}
	else
		//strcpy_s(szOutFile, infile);
		strcpy(szOutFile, infile);


	model.LoadText(szInput);
	model.Calculate(FALSE);

	TSerieses *pResult = model.GetResult();

	if(pResult->GetCount() > 0)
	{
		char *szFileName = strrchr(szOutFile, '\\');

		if(szFileName)
			pResult->SetName(szFileName + 1);

		if(format == NULL || strlen(format) == 0)
		{
			pResult->Save(szOutFile);
		}
		else if(format && strlen(format) > 0)
		{
			pResult->SaveText(szOutFile, szFormat);
		}
		else
			nRet = 2;
	}
	else
		nRet = 1;

	return nRet;
}

int __cdecl RunR(char** infile, char** outfile, char** format)
{
	char szIn[MAX_PATH], szOut[MAX_PATH], szFormat[MAX_PATH];

	memset(szIn, 0, sizeof(szIn));
	memset(szOut, 0, sizeof(szOut));
	memset(szFormat, 0, sizeof(szFormat));

	//strcpy_s(szIn, *infile);
	//strcpy_s(szOut, *outfile);
	strcpy(szIn, *infile);
	strcpy(szOut, *outfile);
	if(format)
		//strcpy_s(szFormat, *format);
		strcpy(szFormat, *format);

	return Run(szIn, szOut, szFormat);
}

void __stdcall RunModelA(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	char szIn[MAX_PATH], szOut[MAX_PATH], szFormat[500], *cFind, *cFind2;
	char szMsg[1024];

	memset(szIn, 0, sizeof(szIn));
	memset(szOut, 0, sizeof(szOut));
	memset(szFormat, 0, sizeof(szFormat));

	cFind = strchr(lpszCmdLine, ' ');
	strncpy_s(szIn, MAX_PATH - 1, lpszCmdLine, cFind - lpszCmdLine);
	cFind2 = strchr(cFind + 1, ' ');
	strncpy_s(szOut, MAX_PATH - 1, cFind + 1, cFind2 - cFind - 1);
	//strcpy_s(szFormat, cFind2 + 1);
	strcpy(szFormat, cFind2 + 1);

	//sprintf_s(szMsg, "In = [%s]\nOut = [%s]\nFormat = [%s]", szIn, szOut, szFormat);
	sprintf(szMsg, "In = [%s]\nOut = [%s]\nFormat = [%s]", szIn, szOut, szFormat);

	//MessageBox(NULL, szMsg, "Test", 0);
	Run(szIn, szOut, szFormat);
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

