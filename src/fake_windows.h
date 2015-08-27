#ifndef __FAKE__WINDOWS_H__
#define __FAKE__WINDOWS_H__

#include <stdio.h>
#include <R_ext/Boolean.h>

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int UINT;
typedef long LONG;
typedef unsigned long DWORD, ULONG, LRESULT, LPARAM, WPARAM;

typedef int BOOL;
//enum { FALSE, TRUE };

typedef struct _HANDLE {} *HANDLE;
typedef struct _HBRUSH {} *HBRUSH;
typedef struct _HCURSOR {} *HCURSOR;
typedef struct _HEAP {} *HEAP;
typedef struct _HICON {} *HICON;
typedef struct _HINSTANCE {} *HINSTANCE;
typedef struct _HMENU {} *HMENU;
typedef struct _HMODULE {} *HMODULE;
typedef struct _HWND {} *HWND;

enum { INFINITE, CS_HREDRAW, CS_VREDRAW, COLOR_WINDOW, GWL_USERDATA, HWND_TOP, SPI_GETWORKAREA,
       WS_CHILD, WS_VISIBLE, SM_CYCAPTION, SM_CYFRAME, SM_CXSCREEN, SM_CYSCREEN,
       SW_SHOWNORMAL, SW_MINIMIZE, WM_SIZE, WM_SETICON, ICON_BIG, WS_OVERLAPPEDWINDOW,
       WM_CREATE, WM_TIMER, WM_CLOSE, WM_DESTROY, WM_QUIT
     };

typedef void *LPVOID;

#ifdef UNICODE
typedef wchar_t TCHAR;
#define TEXT(x) L##x
#else
typedef char TCHAR;
#define TEXT(x) x
#endif

typedef TCHAR *LPTSTR;
typedef const TCHAR *LPCTSTR;

typedef char *LPSTR;
typedef const char *LPCSTR;
#define IDI_INFORMATION TEXT("IDI_INFORMATION")
#define IDI_WARNING TEXT("IDI_WARNING")
#define IDI_ERROR TEXT("IDI_ERROR")

typedef void (*LPPROC)(void);

#define WINAPI
#define CALLBACK

struct WNDCLASSEX
{
    int cbSize;
    int style;
    LRESULT CALLBACK(*lpfnWndProc)(HWND, UINT, WPARAM, LPARAM);
    int cbClsExtra;
    int cbWndExtra;
    HINSTANCE hInstance;
    HICON hIcon;
    HCURSOR hCursor;
    HBRUSH hbrBackground;
    LPCTSTR lpszMenuName;
    LPCTSTR lpszClassName;
    HICON hIconSm;
};

struct RECT
{
    LONG left, right, top, bottom;
};

struct MSG
{
};

typedef struct
{
    LPVOID lpCreateParams;
} CREATESTRUCT, *LPCREATESTRUCT;

/*
//inline HANDLE CreateEvent(LPVOID, BOOL, BOOL, LPVOID) { return 0; }
inline HANDLE CreateThread(LPVOID, int, DWORD WINAPI(*)(LPVOID), LPVOID, int, LPVOID) { return 0; }
inline int WaitForSingleObject(HANDLE, int) { return 0; }
inline int RegisterClassEx(WNDCLASSEX *) { return 0; }
//inline int SetWindowLong(HWND, int, LONG) { return 0; }
//inline LPARAM MAKELPARAM(unsigned short, unsigned short) { return 0; }
//inline HWND CreateWindow(LPCTSTR, LPVOID, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID) { return 0; }
//inline LRESULT SendMessage(HWND, UINT, WPARAM, LPARAM) { return 0; }
//inline LONG GetSystemMetrics(int) { return 0; }
//inline int SetWindowPos(HWND, int, LONG, LONG, LONG, LONG, int) { return 0; }
//inline int SystemParametersInfo(int, int, LPVOID, int) { return 0; }
//inline int ShowWindow(HWND, int) { return 0; }
//inline int UpdateWindow(HWND) { return 0; }
//inline int SetEvent(HANDLE) { return 0; }
inline BOOL GetMessage(MSG *, LPVOID, int, int) { return FALSE; }
//inline int DispatchMessage(MSG *) { return 0; }
//inline int GetClientRect(HWND, RECT *) { return 0; }
//inline HICON LoadIcon(HINSTANCE, LPCTSTR) { return 0; }
//inline unsigned lstrlenA(LPCSTR) { return 0; }
//inline int lstrcmpA(LPCSTR, LPCSTR) { return 0; }
//inline int lstrcpyA(LPSTR, LPCSTR) { return 0; }
//inline int lstrcatA(LPSTR, LPCSTR) { return 0; }
//#define wsprintfA sprintf
//inline int SetWindowTextA(HWND, LPCSTR) { return 0; }
//inline LPVOID HeapAlloc(HEAP, int, ULONG) { return 0; }
//inline HEAP GetProcessHeap() { return 0; }
//inline int HeapFree(HEAP, int, LPVOID) { return 0; }
//inline int DestroyWindow(HWND) { return 0; }
//inline LONG GetWindowLong(HWND, int) { return 0; }
//inline LRESULT CALLBACK DefWindowProc(HWND, UINT, WPARAM, LPARAM) { return 0; }
//inline HMODULE LoadLibraryA(LPCSTR) { return 0; }
//inline LPPROC GetProcAddress(HMODULE, LPCSTR) { return 0; }
//inline int SetTimer(HWND, unsigned, unsigned, unsigned) { return 0; }
//inline int KillTimer(HWND, unsigned) { return 0; }
//inline DWORD GetTickCount() { return 0; }
//inline int ExitProcess(int) { return 0; }
//inline bool IsIconic(HWND) { return 0; }
//inline HWND GetForegroundWindow() { return 0; }
*/

#define MAX_PATH 260

#ifndef CP_UTF8
#define CP_UTF7	65000   /* UTF-7 translation.  */
#define CP_UTF8	65001   /* UTF-8 translation.  */
#endif
#ifndef CP_UTF16
#define CP_UTF16  65002	/* UTF-16 translation.  */
#endif
#ifndef CP_ACP
#define CP_ACP	0	/* Default to ANSI code page.  */
#endif
#ifndef CP_OEM
#define CP_OEM  1	/* Default OEM code page. */
#endif

#define strcpy_s(d, n, s)                   snprintf(d, n, "%s", s)
#define strncpy_s(d, dn, s, sn)             strncpy(d, s, sn < dn ? sn : dn)
#define fopen_s(pFile, filename, mode)      (int)(((*(pFile))=fopen((filename),(mode)))==NULL)
#define errno_t                             bool
#define sprintf_s                           snprintf
#define fprintf_s                           fprintf
#define lstrlen                             strlen
#define lstrcpy                             strcpy
#define lstrcmpi                            strcasecmp

#endif // __FAKE__WINDOWS_H__


/*
#ifndef __WINDOWS2LINUX_H__
#define __WINDOWS2LINUX_H__

// LINUX SPECIFIC DEFINITIONS
//
// Data types conversions
//
#include <stdlib.h>
#include <string.h>
//#include "basicDataTypeConversions.h"

#ifdef __cplusplus
namespace avxsynth {
#endif // __cplusplus
//
// purposefully define the following MSFT definitions
// to mean nothing (as they do not mean anything on Linux)
//
//#define __stdcall
//#define __cdecl
#define noreturn
//#define __declspec(x)
#define STDAPI       extern "C" HRESULT
#define STDMETHODIMP HRESULT __stdcall
#define STDMETHODIMP_(x) x __stdcall

#define STDMETHOD(x)    virtual HRESULT x
#define STDMETHOD_(a, x) virtual a x

#ifndef TRUE
#define TRUE  true
#endif

#ifndef FALSE
#define FALSE false
#endif

#define S_OK                (0x00000000)
#define S_FALSE             (0x00000001)
#define E_NOINTERFACE       (0X80004002)
#define E_POINTER           (0x80004003)
#define E_FAIL              (0x80004005)
#define E_OUTOFMEMORY       (0x8007000E)

#define INVALID_HANDLE_VALUE    ((HANDLE)((LONG_PTR)-1))
#define FAILED(hr)              ((hr) & 0x80000000)
#define SUCCEEDED(hr)           (!FAILED(hr))


//
// Functions
//
#define MAKEDWORD(a,b,c,d) ((a << 24) | (b << 16) | (c << 8) | (d))
#define MAKEWORD(a,b) ((a << 8) | (b))

#define lstrlen                             strlen
#define lstrcpy                             strcpy
#define lstrcmpi                            strcasecmp
#define _stricmp                            strcasecmp
#define InterlockedIncrement(x)             __sync_fetch_and_add((x), 1)
#define InterlockedDecrement(x)             __sync_fetch_and_sub((x), 1)
// Windows uses (new, old) ordering but GCC has (old, new)
#define InterlockedCompareExchange(x,y,z)   __sync_val_compare_and_swap(x,z,y)

#define UInt32x32To64(a, b)                 ( (uint64_t) ( ((uint64_t)((uint32_t)(a))) * ((uint32_t)(b))  ) )
#define Int64ShrlMod32(a, b)                ( (uint64_t) ( (uint64_t)(a) >> (b) ) )
#define Int32x32To64(a, b)                  ((__int64)(((__int64)((long)(a))) * ((long)(b))))

#define MulDiv(nNumber, nNumerator, nDenominator)   (int32_t) (((int64_t) (nNumber) * (int64_t) (nNumerator) + (int64_t) ((nDenominator)/2)) / (int64_t) (nDenominator))

#ifdef __cplusplus
} // namespace avxsynth
#endif // __cplusplus

#endif //  __WINDOWS2LINUX_H__
*/
