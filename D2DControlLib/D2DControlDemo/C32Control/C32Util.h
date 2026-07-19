#pragma once
#include <Windows.h>


CString C32GetWindowsClass(HWND Hwnd);

//获取程序当前所在显示器的分辨率大小，可以动态的获取程序所在显示器的分辨率
// 如果hWnd 为NULL,则为主显示器的大小
//
SIZE C32GetDesktopSize(HWND hWnd = NULL);

/*
	窗口(Hwnd)在父窗口中居中显示.
	Parent = NULL 是窗口在桌面居中显示
*/
BOOL C32SetCenterPos(HWND Hwnd, HWND Parent);

BOOL C32SetCenterPosDesktop(HWND Hwnd);

/*
	获得任务栏的高度
*/
int C32GetTaskbarHeight(); 




//复制到剪切板
BOOL C32SetClipboard(CString& String, HWND Hwnd);

int  C32GetTextLength( HWND Hwnd, LPCWSTR String, HFONT hFont = NULL);

int  C32GetTextLength(HDC Hdc, LPCWSTR String);


/*
	查找替换对话框
*/
BOOL C32FindTextDlg(HWND hwndOwner);

BOOL C32ChooseColor(HWND hwndOwner, _Out_ COLORREF* Color);

BOOL C32ChooseFont(HWND hwndOwner, _Inout_ LPLOGFONT LogFont, __out int* FontSize);

/*
	字体
*/

HFONT C32GetSysFont();

int C32GetFontHeightInPixels(HDC hdc, int lfHeight);

int C32GetFontSize(HWND hwndOwner, LPLOGFONT LogFont);

int C32GetFontSize(HWND hwndOwner, LPLOGFONT LogFont, int Height);

/*
	根据字体名称和字体大小来创建字体。
*/
HFONT C32CreateFont(LPCWSTR FontName, int FontSize);

int C32GetFontSize(HWND hwnd, int Height);

int C32GetFontHeight(HFONT hFont);

BOOL C32GetFontInfo(HGDIOBJ  hFont);


BOOL C32FontGetTextExtentPoint32(__in HDC hdc,__in_ecount(c) LPCWSTR lpString,__in int c,__out LPSIZE psizl, LPLOGFONT LogFont);

BOOL C32FontGetTextExtentPoint32(__in HWND hwnd,__in_ecount(c) LPCWSTR lpString,__in int c,__out LPSIZE psizl, LPLOGFONT LogFont);

/*
	全局变量中取
*/
FLOAT C32GetDpiFontSize();

/*
	将系统字体复制一份，改变字体中Height来创建新的字体。
	为了方便将用缩放的方式*/
HFONT  C32CreateSystemFont(FLOAT Scale);



/*
	在矩形SrcRect Draw 图片
	如果SrcRect的高度和宽度大于图片。图片上下居中
*/
BOOL C32GetDrawIconRect(int IconCx, int IconCy, RECT SrcRect, __out RECT * IconRect);

BOOL C32GetDrawIconRect(SIZE Size, RECT SrcRect, __out RECT * IconRect);

BOOL C32SetSizeInRectCenter(SIZE Size, RECT SrcRect, __out RECT * IconRect);

//菜单
BOOL C32SetMenuOwnerDraw(__in HMENU hMnu, __in UINT uPosition, __in UINT uFlags,__in UINT_PTR uIDNewItem);



/*
	GDI 对象。直线，平面图形
*/
VOID C32DrawLine(HDC hdc,POINT StartPoint, POINT EndPoint, int PenSize, COLORREF PenColor);

/*
	鼠标
*/
VOID C32TrackMouseEvent(DWORD dwFlags, HWND  hwndTrack, DWORD dwHoverTime = 0);


/*
	@brief: 系统字体
*/
class C32SysFont
{
public:
	C32SysFont()
	{
		m_Font = NULL;
		RtlZeroMemory(&m_LogFont, sizeof(LOGFONT));
	}

	~C32SysFont()
	{
		Unitialize();
	}

	VOID Unitialize()
	{
		if (m_Font)
		{
			DeleteFont(m_Font);
			m_Font = NULL;
		}
	}

	VOID Initialize()
	{
		InitSysFont();
	}

	VOID InitSysFont()
	{
		NONCLIENTMETRICS NcMetrics ={};
		NcMetrics.cbSize = sizeof(NONCLIENTMETRICS);

		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0,
							(PVOID)&NcMetrics, 0);
		
		m_LogFont = NcMetrics.lfCaptionFont;
		m_Font = CreateFontIndirect(&m_LogFont);

		ZxDebugEx((__FUNCTION__"()::FaceName:%ws  \n", NcMetrics.lfCaptionFont.lfFaceName));
	}

	HFONT GetLogicFont()
	{
		return m_Font;
	}

	LPLOGFONT GetLogFont()
	{
		return &m_LogFont;
	}

	int GetFontSize()
	{
		return abs(m_LogFont.lfHeight);
	}

	WCHAR* GetFontName()
	{
		return m_LogFont.lfFaceName;
	}

	HFONT   m_Font;
	LOGFONT m_LogFont;

};

class C32Monitor
{
public:
	C32Monitor()
	{
		m_Cx = m_Cy = 0;
		m_LogPixel = 96;
		m_FontScale = 1.0f;
		m_WindowScale = 1.0f;

		Initialize();
	}

	~C32Monitor()
	{


	}
public:
	
	VOID InitLogPixel()
	{
		HDC hdcScreen = GetDC(NULL);
		m_LogPixel = GetDeviceCaps(hdcScreen, LOGPIXELSX);
		m_LogPixelY = GetDeviceCaps(hdcScreen, LOGPIXELSY);

		DeleteObject(hdcScreen);
	}

	VOID InitScale()
	{
		m_FontScale = ((FLOAT)m_LogPixel/96.0f);
		m_WindowScale = ((FLOAT)m_LogPixel/96.0f*1.5f);
	}

	VOID Initialize()
	{
		InitLogPixel();
		InitScale();

		Debug();
	}

	VOID Debug()
	{
		ZxDebugEx((__FUNCTION__"():: m_Cx = %d, m_Cy = %d, m_LogPixel = %d m_FontScale = %f m_WindowScale = %f \n",
				m_Cx, m_Cy, m_LogPixel, m_FontScale, m_WindowScale));
	}

public:
	int		m_Cx; //
	int		m_Cy;
	int		m_LogPixel;//每个逻辑英寸沿屏幕宽度的像素数
	int     m_LogPixelY;
	FLOAT    m_FontScale;
	FLOAT	 m_WindowScale;
};

/*
	@brief: 系统颜色
*/
class C32SysColor
{
public:
	C32SysColor()
	{
		m_SysColorWindow = GetSysColor(COLOR_WINDOW);
		m_SysColorWindowText = GetSysColor(COLOR_WINDOWTEXT);
		m_SysColorWindowFrame = GetSysColor(COLOR_WINDOWFRAME);
	}

	~C32SysColor()
	{


	}
public:
	COLORREF  m_SysColorWindow;
	COLORREF  m_SysColorWindowText;
	COLORREF  m_SysColorWindowFrame;

};

/*
	@brief: 系统 屏幕，图标，滚动条等的大小。
*/
class C32SystemMetrics : public C32SysColor
{

public:
	C32SystemMetrics()
	{
		Initialize();

	}
	
	~C32SystemMetrics()
	{


	}
	
	VOID Initialize()
	{
		m_CyMenu = GetSystemMetrics(SM_CYMENU);
		m_CyMenuCheck = GetSystemMetrics(SM_CYMENUCHECK);
		m_CyMenuSize = GetSystemMetrics(SM_CYMENUSIZE);
		
		m_CxScreen = GetSystemMetrics(SM_CXFULLSCREEN);
		m_CyScreen = GetSystemMetrics(SM_CYFULLSCREEN);

		m_CxSize = GetSystemMetrics(SM_CXSIZE);
		m_CySize = GetSystemMetrics(SM_CYSIZE);


		m_CxFrame = GetSystemMetrics(SM_CXFRAME);
		m_CyFrame = GetSystemMetrics(SM_CYFRAME);


		m_CxSmIcon = GetSystemMetrics(SM_CXSMICON);
		m_CySmIcon = GetSystemMetrics(SM_CYSMICON);
		m_CxIcon = GetSystemMetrics(SM_CXICON);
		m_CyIcon = GetSystemMetrics(SM_CYICON);
		m_CyCaption = GetSystemMetrics(SM_CYCAPTION);
		m_CyBorder = GetSystemMetrics(SM_CYBORDER);

		//滚动条
		m_CxVscroll = GetSystemMetrics(SM_CXVSCROLL);
		m_CyVscroll = GetSystemMetrics(SM_CYVSCROLL);

		ZxDebugEx(("SM_CYMENU: %d \n", m_CyMenu));
		ZxDebugEx(("SM_CYMENUCHECK: %d \n", m_CyMenuCheck));
		ZxDebugEx(("SM_CYMENUSIZE: %d \n", m_CyMenuSize));
		ZxDebugEx(("SM_CXSMICON: %d \n", m_CxSmIcon));
		ZxDebugEx(("SM_CYSMICON: %d \n", m_CySmIcon));
		ZxDebugEx(("SM_CXICON: %d \n", m_CxIcon));
		ZxDebugEx(("SM_CXICON: %d \n", m_CyIcon));
		ZxDebugEx(("SM_CXVSCROLL: %d \n", m_CxVscroll));
		ZxDebugEx(("SM_CYVSCROLL: %d \n", m_CyVscroll));
		ZxDebugEx(("SM_CYCAPTION: %d \n", m_CyCaption));

		GetNonClientMetrics();
		InitLanguage();
		
	}

	VOID GetNonClientMetrics()
	{
		m_NonClientMetrics.cbSize = sizeof(NONCLIENTMETRICS);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, (PVOID)&m_NonClientMetrics, 0);
	}

	VOID InitLanguage()
	{
		//Windows 应用程序的国际化
		m_LangId = GetSystemDefaultLangID();
		WCHAR Buffer[8]={};
		int Ret = GetSystemDefaultLocaleName(Buffer, 8);
		m_LangString = Buffer;
	}

	VOID GetFontLanguage(LPLOGFONT Font, CString& Lang)
	{
		memcpy(Font, &m_NonClientMetrics.lfCaptionFont, sizeof(LOGFONT));
		
		Lang += m_LangString;
	}

	VOID GetSmIcon(PSIZE Size)
	{
		Size->cx = m_CxIcon;
		Size->cy = m_CyIcon;
	}

public:
	//LoadIcon 函数只能加载具有SM_CXICON和SM_CYICON指定尺寸的图标
	int m_CxIcon;  //图标的系统大宽度（以像素为单位）。
	int m_CyIcon;  //图标的系统高度（以像素为单位）。

	int m_CxSmIcon; //图标的系统小宽度（以像素为单位）。 小图标通常显示在窗口标题和小图标视图中。
	int m_CySmIcon; //图标的系统小高度（以像素为单位）。 小图标通常显示在窗口标题和小图标视图中.
	//窗口
	int m_CxMin; //窗口的最小宽度（以像素为单位）。
	int m_CyMin; //窗口的最小高度（以像素为单位）。

	int m_CyMinimized;//最小化窗口的高度（以像素为单位）。
	int m_CxMinimized; //最小化窗口的宽度（以像素为单位）

	int  m_CxFrame;    //可调整大小的窗口周边的大小边框的粗细（以像素为单位）。 SM_CXSIZEFRAME是水平边框的宽度，
	int  m_CyFrame;    //SM_CYSIZEFRAME是垂直边框的高度。
	
	int  m_CxSize;	 //窗口中按钮的宽度描述文字或标题栏（以像素为单位)
	int  m_CySize;     //窗口中按钮的高度描述文字或标题栏（以像素为单位)

	//主显示监视器
	int m_CyMaximized;//主显示监视器上最大化的顶级窗口的默认高度（以像素为单位）。
	int m_CxMaximized; //主显示监视器上最大化的顶级窗口的默认宽度（以像素为单位）。

	//屏幕
	int m_CxScreen;   //主显示器的屏幕宽度（以像素为单位）。 这是通过调用 GetDeviceCaps 获取的相同值，
	int m_CyScreen;   //主显示器的屏幕高度（以像素为单位）。 这是通过调用 GetDeviceCaps 获取的相同值

	//滚动条
	int m_CxHscroll; //水平滚动条上箭头位图的宽度（以像素为单位）。
	int m_CyHscroll; // 水平滚动条的高度（以像素为单位）
		
	int m_CxVscroll; //垂直滚动条的宽度（以像素为单位）。
	int m_CyVscroll; //垂直滚动条上箭头位图的高度（以像素为单位）。
		
	int m_CxHthumb;  //水平滚动条中拇指框的宽度（以像素为单位）。
	int m_CyHthumb;  //垂直滚动条中拇指框的高度（以像素为单位）。

	//菜单SM_CYMENU SM_CYMENUSIZE
	
	int m_CyMenu;	    //单行菜单栏的高度（以像素为单位）
	int m_CyMenuCheck;  //默认菜单的高度检查标记位图（以像素为单位）
	int m_CyMenuSize;   //菜单栏按钮（例如在多个文档界面中使用的子窗口关闭按钮）的高度（以像素为单位）
	int m_CyCaption;	//描述文字区域的高度（以像素为单位）
	int m_CyTaskbar;    //任务栏的高度
	int m_CyBorder;

	LANGID			 m_LangId;
	CString			 m_LangString;
	
	NONCLIENTMETRICS m_NonClientMetrics; //Retrieves the metrics associated with the nonclient area of nonminimized windows

	//DPI 相关
	int				 m_LogPixel;
};

/*
	@brief: 系统Gdi对象

*/
class C32SysObject : public C32SysFont
{

public:
	C32SysObject()
	{

	}
	
	~C32SysObject()
	{
	}

	VOID Initialize()
	{	
		C32SysFont::Initialize();
	}

	VOID Destroy()
	{
		
		
	}

	VOID Debug()
	{
	
	}

};

class C32DPI
{
public:
	C32DPI()
	{

	}
	
	~C32DPI()
	{

	}

	VOID Initialize();
	

public:
	LOGFONT m_LogFont;
	CString m_FontString;
	CString m_LocalString;
	FLOAT	m_FontSize;
	FLOAT   m_FontScale;
	FLOAT   m_WindowScale;
};

extern C32SystemMetrics   theSysMetrics;
extern C32SysObject		  theSysObject;
extern C32Monitor		  theMonitor;
extern C32DPI			  theDPI;

extern int				  theIcon;
extern int				  theIconSmall;
extern float			   theScaleDpiX;
extern float			   theScaleDpiY;

VOID InitC32Util();

//设置APPIcon
VOID SetAppIcon(int iIcon, int iSmallIocn);

//暂时设置DPIScale来适应DPI不同
VOID SetDpiScale(float dpiScaleX, float dpiScaleY);

int  GetDpiScaleX(int x);

int  GetDpiScaleY(int y);

SIZE GetNoClientSize(HWND hwnd);