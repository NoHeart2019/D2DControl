#pragma once
#include "C32Debug.h"


class C32String;

/*
	输出消息或通知以及对应的结构信息

*/





VOID WINAPI C32DbgOutRect(RECT& rc, CHAR* Pre = NULL);

C32String  C32DbgGetStringRect(RECT& rc);

VOID WINAPI C32DbgOutRectPosSize(RECT& rc, CHAR* Pre);



/*
	虚拟按键的值
*/
LPCSTR GetVirtualKeyString(DWORD Value);

/*

*/

VOID WINAPI C32DbgOutMsgPaint(LPPAINTSTRUCT lpst);


/*
	windows message
*/
CHAR* C32DbgGetWindowMsgString(UINT uMsg);

VOID C32DbgOutWinowsMsgFilter(USHORT uMsg, WPARAM wParam, LPARAM lParam, PCHAR Prefix);

VOID C32DbgOutWinowsMsg(USHORT uMsg, PCHAR Function);

CHAR* GetGenericNotifyString(int NotifyCode);


CHAR* C32DbgGetTreeViewItemStateString(UINT State);


CHAR* C32DbgGetTreeViewMsgString(UINT uMsg);

VOID C32DbgOutTreeViewMsg(int MessageCode);

VOID C32DbgOutTreeViewNotify(int NotificationCode);

CHAR* C32GetListViewNotifyString(int NotificationCode);


/*
	Header Control


*/

CHAR* C32DbgGetHeaderNotifyString(int NotifyCode);

VOID C32DbgOutHeaderNotify(int NotifyCode);

VOID C32DbgOutHeaderNotifyNmHeader(LPNMHEADER lpnmhdr);


VOID C32DbgOutListViewNotify(int Nm);

VOID C32DbgOutNmListView(LPNMLISTVIEW lpnmlv);


/*
	Tab Controls
*/
CHAR* C32DbgGetTabCtrlNotifyString(int Nm);

VOID C32DbgOutTabCtrlNotify(int Nm);

/*
	Tooltip Controls
*/
CHAR* C32DbgGetToolTipCtrlNotifyString(int Nm);

VOID C32DbgOutToolTipCtrlNotify(int Nm);


CHAR* C32DbgGetTrayNotifyString(DWORD NotifyCode);


extern BOOL g_C32DbgFlagLvNotifyFilter;
extern BOOL g_C32DbgFlagLvNotify;
extern BOOL g_C32DbgFlagHeaderNotify ;
extern BOOL g_C32DbgFlagWindowMsg ;
extern BOOL g_C32DbgFlagHeaderNotifyFilter ;
extern BOOL g_C32DbgFlagTabCtrlNotify ;
extern BOOL g_C32DbgFlagToolTipCtrlNotify;
extern BOOL g_C32DbgFlagWmFilter;


/*
	WM_MEASUREITEM 
*/
LPCSTR C32GetOwnerDrawCtrlType(UINT CtlType);


/*
	输出字体信息
*/
C32String C32FontGetString(HFONT hFont);

C32String C32FontGetString(HDC hDc);

C32String C32FontGetString(HWND Hwnd);