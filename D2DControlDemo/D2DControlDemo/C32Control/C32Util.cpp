#include "stdafx.h"
#include "C32Util.h"

 
C32SystemMetrics   theSysMetrics;
C32SysObject       theSysObject;
C32Monitor		   theMonitor;
C32DPI			   theDPI;

//程序使用的图标
int				   theIcon;
int				   theIconSmall;
float			   theScaleDpiX;
float			   theScaleDpiY;

CString C32GetWindowsClass(HWND Hwnd)
{
	CString ClassName;
	LPWSTR Ptr = ClassName.GetBufferSetLength(256);

	UINT nChar = RealGetWindowClass(Hwnd, Ptr, 256);
	ClassName.ReleaseBuffer(nChar);

	return ClassName;
}

//获取程序当前所在显示器的分辨率大小，可以动态的获取程序所在显示器的分辨率
SIZE C32GetDesktopSize(HWND hWnd) {
	SIZE size = {};
	
	//MONITOR_DEFAULTTONEAREST 返回值是最接近该点的屏幕句柄
	//MONITOR_DEFAULTTOPRIMARY 返回值是主屏幕的句柄
	//如果其中一个屏幕包含该点，则返回值是该屏幕的HMONITOR句柄。如果没有一个屏幕包含该点，则返回值取决于dwFlags的值
	DWORD dwFlag = (hWnd ?MONITOR_DEFAULTTONEAREST : MONITOR_DEFAULTTOPRIMARY);
	HMONITOR hMonitor = MonitorFromWindow(hWnd, dwFlag);
	MONITORINFOEX miex;
	miex.cbSize = sizeof(miex);
	if (!GetMonitorInfo(hMonitor, &miex))
		return size;

	DEVMODE dm;
	dm.dmSize = sizeof(dm);
	dm.dmDriverExtra = 0;

	//ENUM_CURRENT_SETTINGS 检索显示设备的当前设置
	//ENUM_REGISTRY_SETTINGS 检索当前存储在注册表中的显示设备的设置
	if (!EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &dm))
		return size;

	size.cx = dm.dmPelsWidth;
	size.cy = dm.dmPelsHeight;
	return size;
}

BOOL C32SetCenterPos(HWND Hwnd, HWND Parent)
{
	RECT Rect1 = {}, Rect = {};
	GetWindowRect(Hwnd, &Rect1);
	
	if (Parent == NULL) //桌面居中
	{
		SIZE Size = C32GetDesktopSize(NULL);
		Rect.left = Rect.top = 0;
		Rect.right = Size.cx;
		Rect.bottom = Size.cy;

						
	}else
	{
		GetWindowRect(Parent, &Rect);
		
	}
	
	int X = (Rect.left + Rect.right)/2 - C32GetRectWidth(Rect1)/2;
	int Y = (Rect.top + Rect.bottom)/2 - C32GetRectHeight(Rect1)/2;

	return ::SetWindowPos(Hwnd, NULL, X, Y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

}

BOOL C32SetCenterPosDesktop(HWND Hwnd)
{
	return C32SetCenterPos(Hwnd, NULL);
}

int C32GetTaskbarHeight() 
{
	// 获取主显示器的工作区尺寸
	RECT WorkArea;
	int Height;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &WorkArea, 0);
	// 获取屏幕的总尺寸
	int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	// 计算任务栏占据的空间

	Height = ScreenHeight - (WorkArea.bottom - WorkArea.top);

	return (Height < 0 ? 0 : Height);
}

BOOL C32SetClipboard(CString& String, HWND Hwnd)
{
	CStringA Str = C32W2A(String);
	int ByteSize = Str.GetLength() + 1;
	if ( !OpenClipboard(Hwnd))
	{
		return FALSE;
	}
	// Remove the current Clipboard contents
	if( !EmptyClipboard())
	{
		return FALSE;
	}
	// Get the currently selected data
	HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, ByteSize);
	if (hGlob == NULL)
	{
		CloseClipboard();
		return FALSE;
	}
	GlobalLock(hGlob);
	strcpy_s((char*)hGlob, ByteSize, Str.GetString());
	GlobalUnlock(hGlob);
	// For the appropriate data formats...
	if ( ::SetClipboardData( CF_TEXT, hGlob ) == NULL )
	{
		CloseClipboard();
		GlobalFree(hGlob);
		return FALSE;
	}
	CloseClipboard();

	return TRUE;
}

int  C32GetTextLength(HWND Hwnd, LPCWSTR String, HFONT hFont)
{
	SIZE Size = {};
	HDC  Hdc = NULL;
    TEXTMETRIC Tm={};
	if (String == NULL || Hwnd == NULL)
		return 0;
	
	Hdc = GetDC(Hwnd);

	WCHAR FontName[256]={};
	int FontNameLength =  GetTextFace(Hdc, 256, FontName);
	BOOL Ret = GetTextMetrics(Hdc,&Tm);
	if (Hdc)
	{
		HFONT hFont0 = SelectFont(Hdc, hFont);
		GetTextExtentPoint32(Hdc, String, (int)wcslen(String), &Size);	
		SelectFont(Hdc, hFont0);
	}
	ReleaseDC(Hwnd, Hdc);
	ZxDebugEx((__FUNCTION__"Font Name : %ws Width: %d \n", FontName, Size.cx));

	return Size.cx;
}

int  C32GetTextLength(HDC Hdc, LPCWSTR String)
{
	SIZE Size = {};
	TEXTMETRIC Tm={};
	if (String == NULL || Hdc == NULL)
		return 0;
	WCHAR FontName[256]={};
	int FontNameLength =  GetTextFace(Hdc, 256, FontName);
	BOOL Ret = GetTextMetrics(Hdc,&Tm);
	if (Hdc)
	{

		GetTextExtentPoint32(Hdc, String, (int)wcslen(String), &Size);	
	}
	ZxDebugEx((__FUNCTION__"Font Name : %ws Width: %d \n", FontName, Size.cx));
	return Size.cx;
}

BOOL C32ChooseColor(HWND hwndOwner, _Out_ COLORREF* Color)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	COLORREF acrCustClr[16] = {}; // array of custom colors 
	DWORD rgbCurrent = 0;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hwndOwner;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = rgbCurrent;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (ChooseColor(&cc)==TRUE) 
	{
		*Color = cc.rgbResult;
		return TRUE;
	}

	return FALSE;
}

BOOL C32ChooseFont(HWND hwndOwner, _Inout_ LPLOGFONT LogFont, __out int* FontSize)
{
	CHOOSEFONT cf;            // common dialog box structure
	static LOGFONT lf;        // logical font structure
	static DWORD rgbCurrent;  // current text color
	// Initialize CHOOSEFONT
	ZeroMemory(&cf, sizeof(cf));
	cf.lStructSize = sizeof (cf);
	cf.hwndOwner = hwndOwner;
	cf.lpLogFont = LogFont;
	cf.rgbColors = rgbCurrent;
	cf.Flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;

	if (ChooseFont(&cf)==TRUE)
	{
		if (FontSize)
			*FontSize =cf.iPointSize/10;
		return TRUE;
	}

	return FALSE;
}
//https://zhuanlan.zhihu.com/p/528345722
//Windows获取屏幕缩放比例(放大率）DPI 分辨率等的相关源代码小辣椒高效Office

int C32GetFontHeightInPixels(HDC hdc, int lfHeight)  
{  
	// 获取设备的DPI值（每英寸像素数，Y轴）  
	int dpiY = GetDeviceCaps(hdc, LOGPIXELSY);  
	dpiY = dpiY/ (dpiY/USER_DEFAULT_SCREEN_DPI);
	 
	// 注意：lfHeight是负数时，表示字符高度  
	// 我们需要将其转换为正数进行计算，因为MulDiv期望正数输入  
	// 然后，我们使用MulDiv来进行精确的整数除法，避免浮点数运算  
	// MulDiv(a, b, c) 相当于 (a * b) / c，但结果更精确  
	return MulDiv(-lfHeight, dpiY, 72);  
} 

HFONT C32GetSysFont()
{
	return theSysObject.GetLogicFont();
}


int C32GetFontSize(HWND hwndOwner, LPLOGFONT LogFont)
{
	int FontSize = 0;
	HDC Hdc = GetDC(hwndOwner);
	if (Hdc == NULL)
		return 0;

	HFONT hFont = CreateFontIndirect(LogFont);
	if (hFont)
	{
		//C32SelectObject Sel(Hdc, hFont);
		HFONT hGdiObj1 = SelectFont(Hdc, hFont);
		//FontSize =-LogFont->lfHeight*GetDeviceCaps(Hdc, LOGPIXELSY)/72.0 + 0.5;

		FontSize = C32GetFontHeightInPixels(Hdc, LogFont->lfHeight);

		SelectFont(Hdc, hGdiObj1);
		DeleteFont(hFont);
	}
	ReleaseDC(hwndOwner, Hdc);
	return FontSize;
}

int C32GetFontSize(HWND hwndOwner, LPLOGFONT LogFont, int Height)
{
	int FontSize = 0;
	HDC Hdc = GetDC(hwndOwner);
	if (Hdc == NULL)
		return 0;
	LogFont->lfHeight = Height;
	HFONT hFont = CreateFontIndirect(LogFont);
	if (hFont)
	{
		HFONT hGdiObj1 = SelectFont(Hdc, hFont);
		TEXTMETRIC Tm={};
		GetTextMetrics(Hdc, &Tm);
		FontSize = Height - Tm.tmDescent;
		SelectFont(Hdc, hGdiObj1);
		DeleteFont(hFont);
	}
	ReleaseDC(hwndOwner, Hdc);
	return FontSize;
}

HFONT C32CreateFont(LPCWSTR FontName, int FontSize)
{
	C32Dc  dc(NULL); 
	int nPointSize = 14; // 用户指定字号
	int nHeight = -MulDiv(nPointSize, GetDeviceCaps(dc, LOGPIXELSY), 72);

	LOGFONT lf = {0};
	lf.lfHeight = nHeight;
	lf.lfWidth = 0;               // 宽度由系统自动计算
	lf.lfEscapement = 0;          // 文本旋转角度（0 表示正常）
	lf.lfOrientation = 0;
	lf.lfWeight = FW_NORMAL;      // 字体粗细（常规）
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	wcscpy_s(lf.lfFaceName, FontName); // 宋体的英文名称
	HFONT hFont = CreateFontIndirect(&lf);

	return hFont;
}

int C32GetFontHeight(HFONT hFont)
{
	LOGFONT LogFont = {};
	int Size;
	int SizeOfLogFont = sizeof(LOGFONT);

	Size = GetObject(hFont, SizeOfLogFont, (PVOID)&LogFont);
	if (Size)
	{
		return LogFont.lfHeight;
	}

	return 0;
}

BOOL C32GetFontInfo(HGDIOBJ  hFont)
{
	LOGFONT LogFont = {};
	int Size;
	int SizeOfLogFont = sizeof(LOGFONT);

	Size = GetObject(hFont, SizeOfLogFont, (PVOID)&LogFont);
	if (Size)
	{
		return TRUE;
	}

	return FALSE;
}

HFONT  C32CreateSystemFont(FLOAT Scale)
{
	LOGFONT Font = {};
	HFONT   hFont = NULL;
	memcpy(&Font,&theSysMetrics.m_NonClientMetrics.lfCaptionFont, sizeof(LOGFONT));
	Font.lfHeight = (LONG)(Scale* Font.lfHeight);
	
	return CreateFontIndirect(&Font);
}



/*
	在矩形SrcRect Draw 图片
	如果SrcRect的高度和宽度大于图片。图片上下居中
*/
BOOL C32GetDrawIconRect(int IconCx, int IconCy, RECT SrcRect, __out RECT * IconRect)
{
	int SrcCx = C32GetRectWidth(SrcRect);
	int SrcCy = C32GetRectHeight(SrcRect);
	
	int Cx = (IconCx > SrcCx ? SrcCx : IconCx);
	int Cy = (IconCy > SrcCy ? SrcCy : IconCy);
	
	int yPos = SrcRect.top + (SrcCy - Cy)/2;
	int yPos1 = yPos + Cy;

	int xPos = SrcRect.left + (SrcCx - Cx)/2;
	int xPos1 = xPos + Cx; 

	IconRect->left = xPos;
	IconRect->top = yPos;
	IconRect->right = xPos1;
	IconRect->bottom = yPos1;

	return TRUE;
}

BOOL C32GetDrawIconRect(SIZE Size, RECT SrcRect, __out RECT * IconRect)
{
	return C32GetDrawIconRect(Size.cx, Size.cy, SrcRect, IconRect);

}

BOOL C32SetSizeInRectCenter(SIZE Size, RECT SrcRect, __out RECT * IconRect)
{
	return C32GetDrawIconRect(Size, SrcRect, IconRect);

}

BOOL C32SetMenuOwnerDraw(__in HMENU hMnu, __in UINT uPosition, __in UINT uFlags,__in UINT_PTR uIDNewItem)
{
	UINT State = GetMenuState(hMnu, uPosition, uFlags);
	State |= MF_OWNERDRAW;

	return ModifyMenu(hMnu, uPosition, State|uFlags,uIDNewItem, NULL);
}


BOOL C32FontGetTextExtentPoint32(__in HDC hdc,__in_ecount(c) LPCWSTR lpString,__in int c,__out LPSIZE psizl, LPLOGFONT LogFont)
{
	BOOL Ret = FALSE;
	HFONT hFont = CreateFontIndirect(LogFont);
	HFONT hFont0 =SelectFont(hdc, hFont);

	Ret = GetTextExtentPoint32(hdc, lpString, c, psizl);

	SelectFont(hdc, hFont0);
	DeleteFont(hFont);

	return Ret;
}

BOOL C32FontGetTextExtentPoint32(__in HWND hwnd,__in_ecount(c) LPCWSTR lpString,__in int c,__out LPSIZE psizl, LPLOGFONT LogFont)
{
	BOOL Ret = FALSE;
	C32Dc   Dc(hwnd);
	C32Font Font;
	Font.CreateFontIndirect(LogFont);
	C32SelectObject So(Dc, Font);
	Ret = C32FontGetTextExtentPoint32(Dc, lpString, c, psizl, LogFont);


	return Ret;
}

FLOAT C32GetDpiFontSize()
{
	return theDPI.m_FontSize;
}

VOID C32DrawLine(HDC hdc,POINT StartPoint, POINT EndPoint, int PenSize, COLORREF PenColor)
{

	// 创建一个画笔，设置颜色和宽度。这里以红色为例，宽度为2。
	HPEN hPen = CreatePen(PS_SOLID, PenSize, PenColor);
	// 获取当前画笔以便之后恢复
	HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

	// 如果需要，可以在这里移动到起始位置
	MoveToEx(hdc, StartPoint.x, StartPoint.y, NULL);

	// 使用 LineTo 画出直线
	LineTo(hdc, EndPoint.x, EndPoint.y);

	// 恢复之前的画笔
	SelectObject(hdc, hOldPen);
	// 删除新创建的画笔，避免内存泄漏
	DeleteObject(hPen);

}

VOID C32TrackMouseEvent(DWORD dwFlags, HWND  hwndTrack, DWORD dwHoverTime)
{

	TRACKMOUSEEVENT Tme = {};
	Tme.cbSize = sizeof(TRACKMOUSEEVENT);
	Tme.dwFlags = dwFlags;
	Tme.hwndTrack = hwndTrack;
	Tme.dwHoverTime = dwHoverTime;
	TrackMouseEvent(&Tme);
}


VOID C32DPI::Initialize()
{
	theSysMetrics.GetFontLanguage(&m_LogFont, m_LocalString);
	m_FontString = m_LogFont.lfFaceName;
	m_FontScale = theMonitor.m_FontScale;
	m_WindowScale = theMonitor.m_WindowScale;
	m_FontSize =  24;//MulDiv(-m_LogFont.lfHeight, theMonitor.m_LogPixel, 96);
}

VOID SetAppIcon(int iIcon, int iSmallIocn)
{
	theIcon = iIcon;
	theIconSmall = iSmallIocn;

}

VOID SetDpiScale(float dpiScaleX, float dpiScaleY)
{
	theScaleDpiX = dpiScaleX;
	theScaleDpiY = dpiScaleY;

}

int  GetDpiScaleX(int x)
{
	return  x * (int)theScaleDpiX;
}

int  GetDpiScaleY(int y)
{
	return  y * (int)theScaleDpiY;
}

SIZE GetNoClientSize(HWND hwnd)
{
	RECT client = {}, window = {};
	GetWindowRect(hwnd, &window);
	GetClientRect(hwnd, &client);

	int height1 = C32GetRectHeight(client);
	int width1 = C32GetRectWidth(client);

	int height2 = C32GetRectHeight(window);
	int width2 = C32GetRectWidth(window);
	SIZE size = {width2 - width1, height2 - height1}; 

	return size;
}


VOID InitC32Util()
{
	theSysObject.Initialize();
	theDPI.Initialize();
}
