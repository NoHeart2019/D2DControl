// D2DControlDemo.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "C32Control/C32Include.h"
#include "D2DControlDemo.h"
#include "D2DControlPane.h"
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_D2DCONTROLDEMO, szWindowClass, MAX_LOADSTRING);
	InitC32(hInstance);
	theLog.SetLoggingEnabled(false);
	theLog.SetLogLevel(LogLevelInfo);
	SetAppIcon(IDI_SMALL, IDI_SMALL);
	CoInitialize(NULL);
	CD2DWindow::Register();
	CD2DPane::Register();
	CD2DPane* Frame = new CD2DPane();
	//CStickyNotesFrame* Frame = new CStickyNotesFrame();
	//CAINavigator* Frame = new CAINavigator();
	Frame->SetDelete();
	DWORD style = WS_OVERLAPPEDWINDOW;
	Frame->CreateUI(0, L"Direct2D Control UI", style, 0, 0, 1200, 1300, NULL);
	C32SetCenterPosDesktop(Frame->m_Hwnd);
	Frame->Show(SW_MAXIMIZE);
	Frame->Loop();
}


