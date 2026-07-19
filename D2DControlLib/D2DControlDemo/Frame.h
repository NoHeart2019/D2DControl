#include "StickyNotes.h"

#define  IDM_FREE_SOFTWARE						WM_APP + 0x100

#define  IDM_SHOW_WINDOW						WM_APP + 0x200
#define  IDM_HIDE_WINDOW					    WM_APP + 0x201 

class CStickyNotesFrame : public CStickyNotes
{
public:
	CStickyNotesFrame()
	{

		SetDelete();
	}

	~CStickyNotesFrame()
	{
		PostQuitMessage(0); 
	}


	//处理事件
public:
	virtual BOOL CALLBACK OnCreate(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
	{
		CStickyNotes::OnCreate(wParam, lParam, lResult);
		//创建托盘菜单
		CreateTrayMenu();
		//显示托盘
		AddTray(IDI_SMALL, L"小河便签");

		return TRUE;
	}
	
	virtual BOOL CALLBACK OnClose(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
	{
		Hide();
		return TRUE;
	}

	BOOL CALLBACK OnCommand(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
	{
		BOOL Handle = FALSE;

		if (C32CommandIsMenu(wParam, lParam))
		{
			OnCommandMenu(wParam, lResult);
			Handle = TRUE;
		}else if (C32CommandIsCtrl(wParam))
		{

		}else if(C32CommandIsAccelerator(wParam, lParam))
		{


		}

		return Handle;
	}

	BOOL CALLBACK OnContextMenu(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
	{
		BOOL Handle = FALSE;

		ZxDebugEx((__FUNCTION__"():: \n"));
		return Handle;
	}

	BOOL CALLBACK OnCommandMenu(WPARAM wParam, LRESULT& lResult)
	{
		DWORD MenuId = C32CommandMenuId(wParam);

		switch(MenuId)
		{
			//托盘菜单
		case IDM_EXIT:
			DeleteTray();
			DestroyWindow(m_Hwnd);
			break;
		case IDM_SHOW_WINDOW:
			Show(SW_SHOWNORMAL);
			break;
		
		case IDM_FREE_SOFTWARE:
			//CreateDonationWindow(m_Hwnd, L"感谢您的认可，我会继续努力");
			break;

		}

		return TRUE;
	}

	BOOL OnTray( WPARAM wParam, LPARAM lParam, LRESULT& lResult)
	{
		BOOL Handle = FALSE;

		DWORD Msg = LOWORD(lParam);
		CHAR* String = C32DbgGetTrayNotifyString(Msg);
		ZxDebug((__FUNCTION__"():: C32WM_TRAY :wParam:%p  %s (0x%04x)\n", wParam, String,Msg));
		switch(Msg)
		{
		case WM_RBUTTONDOWN:
			{
				// 显示右键菜单
				SetForegroundWindow(m_Hwnd);
				m_TrayMenu.ShowMenu(m_Hwnd);
				Handle = TRUE;
			}
			break;
		case WM_LBUTTONDBLCLK:
			{
				Show(SW_SHOW);
				Handle = TRUE;
			}
			break;
		}

		return Handle;
	}

	virtual BOOL CALLBACK OnUserHandle(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
	{
		BOOL Handle = FALSE;
		switch(uMsg)
		{
		case C32WM_TRAY:
			Handle = OnTray(wParam, lParam, lResult);
			break;

		default:
			Handle = FALSE;
		}
		return Handle;
	}

public:

	
	VOID CreateTrayMenu()
	{
		m_TrayMenu.CreateRootMenu();	
		m_TrayMenu.AppendItemString(IDM_SHOW_WINDOW,	   L"显示");
		m_TrayMenu.AppendItemString(IDM_EXIT,    L"退出");
		
	}

	BOOL AddTray(int iIcon, LPCWSTR Tip)
	{
		// 创建系统托盘图标
		NOTIFYICONDATA nid = { sizeof(nid) };
		nid.hWnd = m_Hwnd;
		nid.uID = 1;
		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nid.uCallbackMessage = C32WM_TRAY;
		nid.hIcon = LoadIcon(C32Hinstance, MAKEINTRESOURCE(iIcon));
		nid.uVersion = 0;
		wcscpy_s(nid.szTip, Tip);
		return Shell_NotifyIcon(NIM_ADD, &nid);
	}

	BOOL DeleteTray()
	{
		NOTIFYICONDATA nid = { sizeof(nid) };
		nid.hWnd = m_Hwnd;
		nid.uID = 1;
		return Shell_NotifyIcon(NIM_DELETE, &nid);
	}


	//Tray 菜单
	C32Menu m_TrayMenu;
};
