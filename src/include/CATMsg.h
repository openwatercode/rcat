#pragma once

#define UM_GETPROPERTYLIST WM_USER + 100
#define UM_SETPROPERTYNOTIFY WM_USER + 101
#define UM_CHANGEVIEW WM_USER + 102
#define UM_SELECT WM_USER + 103
#define UM_CHANGEDOCUMENT WM_USER + 104
#define UM_SHOWPROPERTY WM_USER + 105
#define UM_SETWINDOWPOS WM_USER + 106
#define UM_SETPOS WM_USER + 107
#define UM_PROP_FILL WM_USER + 108
#define UM_PROP_SAVE WM_USER + 109
#define UM_BAR_RESIZE WM_USER + 110
#define UM_CHECK_LOCK WM_USER + 111
#define UM_SETCHART WM_USER + 400
#define UM_SETDESC WM_USER + 401
#define UM_CALCULATE WM_USER + 402


const unsigned short CATFILEVERSION = 0x0007;
const unsigned short CATVERSION_KGW = 0x0006;
const unsigned short CATBGRPOSVERSION = 0x0005; // 배경위치 정보 저장 전.
const unsigned short CATOLDVERSION2 = 0x0004; // Climate에서 파일명 크기 변경 (TCHAR -> char)
const unsigned short CATOLDVERSION = 0x0003;

/*
#define NODE_URBAN	1
#define NODE_JUNC	2
#define NODE_LINK	3
#define NODE_CLIMATE	4
*/

// 이전 버전 노드 타입값
/*
#define TYPE_CATCH 1
#define TYPE_FOREST 5
#define TYPE_AGRICULTURE 6
#define TYPE_PADDY 7
#define TYPE_USER 8
#define TYPE_JUNK 2
#define TYPE_OUTLET 9
#define TYPE_LINK 3
#define TYPE_WEATHER 4
*/

/*
#define TYPE_CATCH 1
#define TYPE_FOREST 2
#define TYPE_AGRICULTURE 3
#define TYPE_PADDY 4
#define TYPE_USER 5
#define TYPE_JUNK 6
#define TYPE_OUTLET 7
#define TYPE_LINK 8
#define TYPE_WEATHER 9
*/