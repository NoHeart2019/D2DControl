#include "stdafx.h"
#include "C32Control.h"
#include <atltypes.h>

 
class C32Window;
    
HINSTANCE C32Menu::hInstance = NULL;
HINSTANCE C32ImageList::hInstance = NULL;
HINSTANCE C32TreeView::hInstance = NULL; //
HINSTANCE C32Explorer::hInstance = NULL;
HINSTANCE C32Icon::hInstance = NULL;
HINSTANCE C32ListBox::hInstance = NULL;
HINSTANCE C32Dialog::hInstance = NULL;
HINSTANCE C32DialogProgress::hInstance = NULL;
DefenderGetOpenFileName C32DialogFile::m_GetOpenFileName = NULL;
DefenderGetOpenFileName C32DialogFile::m_GetSaveFileName = NULL;

BOOL C32Window::m_MainIsExit = FALSE;

LONG_PTR C32ControlId = 0; 
HFONT C32SysGdiObject::m_SysFont = NULL;
HFONT C32SysGdiObject::m_SysGuiFont = NULL;

ULONG_PTR C32GdiPlus::m_GdiToken;
Gdiplus::GdiplusStartupInput C32GdiPlus::m_GdiInput;
Gdiplus::GdiplusStartupOutput C32GdiPlus::m_GdiOutput;

HINSTANCE C32Hinstance;
HWND      C32Hwnd;
C32Container* C32MainFrame;
UINT C32FindReplaceMsg;

HFONT		C32SystemFont;

 C32ThemeStyle::E32ThemeStyle C32ThemeStyle::m_ThemeStyle;

 DWORD    C32ThemeStyle::m_ColorMode = TRANSPARENT;
 COLORREF C32ThemeStyle::m_ColorBk;
 COLORREF C32ThemeStyle::m_ColorFontBk; 
 COLORREF C32ThemeStyle::m_ColorFont;
 COLORREF C32ThemeStyle::m_ColorSplit;
 HBRUSH	  C32ThemeStyle::m_HbrBkGnd;
 HFONT    C32ThemeStyle::m_FontCustom;
 

  C32ThemeStyleList  theTmeStyleList;

 VOID C32ThemeStyleList::SetThemeStyleNormal()
 {
	DWORD Style = BS_OWNERDRAW;
	for(size_t i = 0; i < m_BtnArray.GetCount(); i++)
	{
		m_BtnArray[i]->ClearStyle(BS_OWNERDRAW);
	}

	for(size_t i = 0; i < m_ListViewArray.GetCount(); i++)
	{
		m_ListViewArray[i]->SetTextBkColor(CLR_DEFAULT);
		m_ListViewArray[i]->SetTextColor(CLR_DEFAULT);
	}
 }

 VOID C32ThemeStyleList::SetThemeStyleDark()
 {
	 DWORD Style = BS_OWNERDRAW;
	 for(size_t i = 0; i < m_BtnArray.GetCount(); i++)
	 {
		 m_BtnArray[i]->AddStyle(Style);
		 m_BtnArray[i]->SetSubClass();
	 }

	 for(size_t i = 0; i < m_BoxArray.GetCount(); i++)
	 {
		 //m_BoxArray[i]->SetSubClass();
	 }


	 for(size_t i = 0; i < m_ListViewArray.GetCount(); i++)
	 {
		 m_ListViewArray[i]->SetBkColor(C32ThemeStyle::m_ColorBk);
		 m_ListViewArray[i]->SetTextBkColor(C32ThemeStyle::m_ColorBk);
		 m_ListViewArray[i]->SetTextColor(C32ThemeStyle::m_ColorFont);
		 //C32ListView* xx = m_ListViewArray[i];
		 if (m_ListViewArray[i]->IsViewDetail())
			m_ListViewArray[i]->m_Header.SetSubClass();
	 }

	 for(size_t i = 0; i < m_StatusBarArray.GetCount(); i++)
	 {
		m_StatusBarArray[i]->SetOwnerDraw();
	 }

	 for(size_t i = 0; i < m_RichEditArray.GetCount(); i++)
	 {
		m_RichEditArray[i]->SetBackgroundColor(0, C32ThemeStyle::m_ColorBk);
		
	 }


	 //for(size_t i = 0; i < m_DataTimeArray.GetCount(); i++)
	 //{

		//m_DataTimeArray[i]->SetMonthCalColor(MCSC_TEXT, C32ColorWhite);
		////m_DataTimeArray[i]->SetMonthCalColor(MCSC_TRAILINGTEXT, C32ColorDarkBlack2);

		//m_DataTimeArray[i]->SetMonthCalColor(MCSC_BACKGROUND, C32ColorDarkBlack2);
		////m_DataTimeArray[i]->SetMonthCalColor(MCSC_TITLEBK, C32ColorDarkBlack2);
		////m_DataTimeArray[i]->SetMonthCalColor(MCSC_TITLETEXT, C32ColorDarkBlack2);
		////m_DataTimeArray[i]->SetMonthCalColor(MCSC_MONTHBK, C32ColorDarkBlack2);

	 //}
 }

 
LRESULT WINAPI C32Button::C32SubClassProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,
	 UINT_PTR uIdSubclass,DWORD_PTR dwRefData)
 {
	BOOL Handle = FALSE;
	LRESULT lResult = 0;
	C32Button* This = (C32Button*)uIdSubclass;
	
	 ZxDebugEx((__FUNCTION__"():: %ws uMsg:%s  (%04x) wParam: %08x lParam: %08x\n", \
		 This->m_StringText.GetString(),C32DbgGetWindowMsgString(uMsg),								\
		 uMsg, wParam,lParam));
	switch (uMsg)
	{
		case WM_SETCURSOR:
			if (C32ThemeStyle::IsThemeStyleDark())
			{
				HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
				SetCursor(hCursor);
				Handle = TRUE; 
				lResult = TRUE;
			}
		break;

	}
	if (Handle == TRUE)
		return lResult;
	 return DefSubclassProc(hWnd, uMsg, wParam, lParam);
 }


LRESULT C32GroupBox::C32SubClassProcBox(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,
	UINT_PTR uIdSubclass,DWORD_PTR dwRefData)
{
	BOOL Handle = FALSE;
	LRESULT lResult = 0;
	C32GroupBox* This = (C32GroupBox*)uIdSubclass;

	ZxDebug((__FUNCTION__"()::%ws uMsg:%s  (%04x) wParam: %08x lParam: %08x\n", \
		This->m_StringText.GetString(),C32DbgGetWindowMsgString(uMsg),								\
		uMsg, wParam,lParam));
	switch (uMsg)
	{
	case WM_PAINT:
		{
			//Handle = TRUE;

			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			RECT rect = ps.rcPaint;
			//// 选择自定义字体（如果有）
			C32SelectObject SelFont(hdc, C32ThemeStyle::m_FontCustom);
			// 获取字体度量信息
			TEXTMETRIC tm;
			GetTextMetrics(hdc, &tm);
			int ascent = tm.tmAscent;
			int descent = tm.tmDescent;
			int fontHeight = tm.tmHeight;

			// 计算文字垂直中心位置（与矩形顶部对齐）
			int centerY = rect.top + tm.tmHeight / 2;
	
			// 定义绘制区域
			CRect textRect(rect.left, rect.top, rect.right, rect.bottom);
			textRect.DeflateRect(10, 0, 10, 0); // 可选：左右内边距

			
			HBRUSH Brush = CreateSolidBrush(C32ThemeStyle::m_ColorFont);
			rect.top += centerY;
			FrameRect(hdc, &rect, Brush);
			DeleteObject(Brush);
			// 绘制文字
			CString text = This->GetText();
			// 设置文本样式
			SetBkColor(hdc, C32ThemeStyle::m_ColorBk);
			SetTextColor(hdc, C32ThemeStyle::m_ColorFont);
			SetBkMode(hdc, OPAQUE);
			DrawText(hdc, text, text.GetLength(), textRect, DT_LEFT  | DT_SINGLELINE);
			
			EndPaint(hWnd, &ps);
			
		}
	}
	if (Handle == TRUE)
		return lResult;
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT C32Header::C32SubClassProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,
	UINT_PTR uIdSubclass,DWORD_PTR dwRefData)
{
	BOOL Handle = FALSE;
	LRESULT lResult = 0;
	C32Header* This = (C32Header*)uIdSubclass;
	
	ZxDebug((__FUNCTION__"()::%ws uMsg:%s  (%04x) wParam: %08x lParam: %08x\n", \
		This->m_StringText.GetString(),C32DbgGetWindowMsgString(uMsg),								\
		uMsg, wParam,lParam));
	switch (uMsg)
	{
			
	case WM_PAINT:
		{
			if (C32ThemeStyle::IsThemeStyleDark())
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);
				RECT rect = ps.rcPaint;
				//选择自定义字体（如果有）
				C32SelectObject SelFont(hdc, C32ThemeStyle::m_FontCustom);
				// 设置文本样式
				SetBkColor(hdc, C32ThemeStyle::m_ColorBk);
				SetTextColor(hdc, C32ThemeStyle::m_ColorFont);
				SetBkMode(hdc, OPAQUE);
				int Count = This->GetCount();
			
				for(int i = 0; i < Count; i++)
				{
					RECT Rect = {};
					This->GetItemRect(i, &Rect);
					CString Text = This->GetItemString(i);
					RECT TextRect = Rect;
					TextRect.right -= 3;
					DrawText(hdc, Text, Text.GetLength(), &TextRect, DT_CENTER|DT_END_ELLIPSIS|DT_VCENTER|DT_SINGLELINE);
					POINT Split1 = {TextRect.right, TextRect.top};
					POINT Split2 =  {TextRect.right, TextRect.bottom};
					//分割线
					C32DrawLine(hdc,Split1, Split2, 2, C32ThemeStyle::m_ColorFont);
				}

				EndPaint(hWnd, &ps);
				Handle = TRUE;

			}
			

		}
		break;
	
	 	case WM_ERASEBKGND:
		{
			if (C32ThemeStyle::IsThemeStyleDark())
			{
				Handle = TRUE;
				This->GetPos();
				//HBRUSH Brush = CreateSolidBrush(0x00008AF3);
				FillRect((HDC)wParam, &This->m_ClientRect, C32ThemeStyle::m_HbrBkGnd);
				lResult = TRUE;
			}
		}

		break;
	}
	if (Handle == TRUE)
		return lResult;
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

//LRESULT C32StatusBar::C32SubClassProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,
//	UINT_PTR uIdSubclass,DWORD_PTR dwRefData)
//{
//	BOOL Handle = FALSE;
//	LRESULT lResult = 0;
//	C32StatusBar* This = (C32StatusBar*)uIdSubclass;
//
//	ZxDebugEx((__FUNCTION__"():: uMsg:%s  (%04x) wParam: %08x lParam: %08x\n", \
//		C32DbgGetWindowMsgString(uMsg),								\
//		uMsg, wParam,lParam));
//	switch (uMsg)
//	{
//	case WM_PAINT:
//		{
//			PAINTSTRUCT ps;
//			HDC hdc = BeginPaint(hWnd, &ps);
//
//			// 设置新的背景颜色
//			HBRUSH hBrush = CreateSolidBrush(RGB(144, 238, 144)); // 绿色背景
//			FillRect(hdc, &ps.rcPaint, hBrush);
//			DeleteObject(hBrush);
//			int Type = 0;
//			RECT Rect = {};
//			CString Text = This->GetText(0, &Type);
//			This->GetRect(0, &Rect);
//			DrawText(hdc, Text, -1, &Rect, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
//			EndPaint(hWnd, &ps);
//			return 0;
//		}
//	}
//	if (Handle == TRUE)
//		return lResult;
//	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
//}

BOOL CALLBACK C32StatusBar::OnDrawItem(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	BOOL Handle = FALSE;

	lResult = 0;
	LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;
	 
	if (C32ThemeStyle::IsThemeStyleDark())
	{
		//ASSERT(lpdis->CtlType == 0);
		HBRUSH  Brush = CreateSolidBrush(C32ThemeStyle::m_ColorBk);
		//lpdis->CtlType == 0
		if (1)
		{
			HDC Hdc = lpdis->hDC; 
			RECT Rect = lpdis->rcItem;
			int  Pane = (int)lpdis->itemID;
			// 态绘制背景
			
			FillRect(Hdc, &Rect, Brush);
			// 绘制按钮文本
			CString Text = GetTextOD(Pane);
			
			SetTextColor(Hdc, C32ThemeStyle::m_ColorFont); // 白色文字
			SetBkMode(Hdc, m_ColorMode);          // 文字背景透明
			DrawText(Hdc, Text, -1, &Rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		}

		DeleteObject(Brush);
	}

	return Handle;
}



VOID  C32FreeWindowList(CAtlArray<C32Window*>& List, BOOL DeletePtr)
{
	if (DeletePtr)
	{
		for(size_t i = 0; i < List.GetCount(); i++)
		{
			delete List[i];
			List[i] = NULL;
		}
	}
	List.RemoveAll();
}
  
VOID C32CreateTree(C32TreeView& Tree, HWND hwndParent, int xPos, int yPos, int Cx, int Cy)
{
	DWORD Style = WS_CHILD | WS_VISIBLE;
	Style |= TVS_LINESATROOT  | TVS_HASBUTTONS | TVS_FULLROWSELECT|TVS_DISABLEDRAGDROP;
	Style |= TVS_TRACKSELECT ;
	Tree.Create(0, Style,
		xPos, yPos, Cx, Cy, 
		(HWND)hwndParent);

	Tree.SetSubclass();
	Tree.SetExplorerTheme();
	//Tree.m_ImageListNormal.CreateNormal();
	//Tree.m_ImageListState.CreateNormal();

}

VOID C32CreateList(C32ListView& List, HWND hwndParent, int xPos, int yPos, int Cx, int Cy)
{
	DWORD Style = WS_CHILD | WS_VISIBLE  ;
	Style |=   LVS_REPORT   ;
	DWORD ExStyle = LVS_EX_FULLROWSELECT|LVS_EX_LABELTIP ;
	List.Create(ExStyle, Style,
		xPos, yPos, Cx, Cy, 
		(HWND)hwndParent);
		  
	List.SetExtendedStyle(ExStyle);
	List.SetListHeader();
	List.SetSubclass();
	//List.SetExplorerTheme();

}

VOID C32CreateStatus(C32StatusBar& Status, HWND hwndParent)
{
	DWORD Style3 = WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP | SBARS_TOOLTIPS ;
	Status.CreateStatus(WS_EX_WINDOWEDGE, Style3, hwndParent);
	Status.SetExplorerTheme();
	
	INT I = 0;
}

BOOL C32Header::OnNotifyItemStateIconClick(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	BOOL Handle = FALSE;
	//如果此Header 是 ListView 的Header，将ListView 所有的Item check box 设置 状态
	LPNMHEADER lpnmhdr = (LPNMHEADER)lParam;
	if (m_List)
	{
		if(IsCheckState(lpnmhdr->iItem))
			m_List->ResetListCheckState();
		else
			m_List->SetListCheckState();
	}
	if (m_OnNotifyItemStateIconClick != NULL)
	{
		Handle = m_OnNotifyItemStateIconClick(lpnmhdr, lResult, this);
	}
	return Handle;
}

HWND C32TreeView::Create(DWORD  dwExStyle, DWORD dwStyle,int X, int Y, int Width, int Height, HWND Parent)
{
	m_Rect.left = X, m_Rect.top = Y, m_Rect.right = X + Width, m_Rect.bottom = Y + Height;
	m_Cx = Width, m_Cy = Height;

	m_Parent = Parent;

	HWND hwnd = CreateWindowEx(dwExStyle,
					WC_TREEVIEW,
					L"C32TreeView",
					dwStyle,
					X,
					Y,
					Width,
					Height,
					Parent,
					(HMENU)NULL,
					C32TreeView::hInstance,
					this);
	ZxDebug((__FUNCTION__"():: hwnd: %p This:%p Code:%d\n", hwnd, this, GetLastError()));
	m_Hwnd = hwnd;
	
	return hwnd;
}

VOID C32TreeView::DestroyView()
{
	if (m_Hwnd)
	{
		DestroyWindow(m_Hwnd);
		m_Hwnd = NULL;
	}

}

VOID C32TreeView::SetExtendedStyles(DWORD dwExStyle)
{
	if (m_Hwnd)
	{
		TreeView_SetExtendedStyle(m_Hwnd, dwExStyle, 0);
	}
}

VOID C32TreeView::AddExtendedStyles(DWORD dwExStyle)
{
	if (m_Hwnd)
	{
		DWORD Ex = TreeView_GetExtendedStyle(m_Hwnd);
		Ex |= dwExStyle;
		TreeView_SetExtendedStyle(m_Hwnd, dwExStyle, Ex);
	}
}

DWORD C32TreeView::GetExtendedStyles()
{
	DWORD ExSytle = 0;

	if (m_Hwnd)
		ExSytle = TreeView_GetExtendedStyle(m_Hwnd);
		
	return ExSytle;
}

BOOL C32TreeView::SetSubclass()
{
	BOOL Ret = FALSE;

	if (m_Hwnd)
	{
		Ret	= SetWindowSubclass(m_Hwnd, C32TreeView::TreeSubclassProc, (UINT_PTR)this, (DWORD_PTR)this);
	}

	return FALSE;
}


HTREEITEM C32TreeView::InsertItem(HTREEITEM Parent, HTREEITEM InsertAfter, LPWSTR Text,
									int iImage, int iSelectedImage, int cChildren, LPARAM lParam)
{
	TVINSERTSTRUCT its = {};
	if (Text == NULL)
		Text = L"";
	TVITEM & it = its.item;
	it.mask = TVIF_TEXT;

	if (iImage != TV_MASK_NO_BIT)
	{
		it.mask |= TVIF_IMAGE;
		it.mask |= TVIF_SELECTEDIMAGE;
	}
	if (cChildren != TV_MASK_NO_BIT)
		it.mask |= TVIF_CHILDREN;

	if (lParam != NULL)
		it.mask |= TVIF_PARAM;

	it.pszText = Text;
	it.cchTextMax = (int)wcslen(Text);
	it.iImage = iImage;
	it.iSelectedImage = iImage ;//iSelectedImage;
	it.cChildren = cChildren;
	it.lParam = lParam;

	its.hParent = Parent;
	its.hInsertAfter = InsertAfter;
	
	return TreeView_InsertItem(m_Hwnd, &its);
}

HTREEITEM C32TreeView::InsertItemRoot(HTREEITEM InsertAfter, LPWSTR Text, 
					int iImage, int iSelectedImage, int cChildren , LPARAM lParam)
{
	return InsertItem(TVI_ROOT, InsertAfter, Text, iImage, iSelectedImage,cChildren,lParam);
}

HTREEITEM C32TreeView::InsertItemAsyn(HTREEITEM Parent, HTREEITEM InsertAfter, LPWSTR Text,
	int iImage, int iSelectedImage, int cChildren, LPARAM lParam)
{
	TVINSERTSTRUCT its = {};

	TVITEM & it = its.item;
	it.mask = TVIF_TEXT;
	it.mask |= TVIF_DI_SETITEM;
	if (iImage != TV_MASK_NO_BIT)
		it.mask |= TVIF_IMAGE;
	if (iSelectedImage != TV_MASK_NO_BIT)
		it.mask |= TVIF_SELECTEDIMAGE;
	if (cChildren != TV_MASK_NO_BIT)
		it.mask |= TVIF_CHILDREN;

	if (lParam != NULL)
		it.mask |= TVIF_PARAM;

	it.pszText = LPSTR_TEXTCALLBACK ;
	it.cchTextMax = 0;
	it.iImage = I_IMAGECALLBACK ;
	it.iSelectedImage = I_IMAGECALLBACK;
	it.cChildren = I_CHILDRENCALLBACK;
	it.lParam = lParam;

	its.hParent = Parent;
	its.hInsertAfter = InsertAfter;

	return TreeView_InsertItem(m_Hwnd, &its);
}

//处理通知

LRESULT  CALLBACK C32TreeView::TreeSubclassProc (HWND hWnd, UINT uMsg, WPARAM wParam,
	LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	//C32DbgOutTreeViewMsg(uMsg);
	BOOL bSub = FALSE;
	LRESULT Result = 0;
	C32TreeView* This = (C32TreeView*)dwRefData;
	switch(uMsg)
	{
	case WM_COMMAND:
		bSub = This->OnCommand(wParam, lParam, Result);
		break;
	case WM_NOTIFY:
		bSub = This->OnNotify(wParam, lParam, Result);
		break;
	}
	if (bSub == FALSE)
	{
		return DefSubclassProc(hWnd, uMsg, wParam, lParam);
	}
	return (LRESULT)bSub;
}

 BOOL CALLBACK C32TreeView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	BOOL bNotify = FALSE;
	int NotifyCode = 0;
	LPNMTREEVIEW NmTv = NULL;
	LPNMHDR lphdr = (LPNMHDR)lParam;
	NotifyCode = lphdr->code;
	C32DbgOutTreeViewNotify(NotifyCode);

	
	if (lphdr->idFrom != (ULONG_PTR)this->m_Hwnd && lphdr->hwndFrom != this->m_Hwnd)
	{
		DebugBreak();
		return FALSE;
	}
	
	switch(NotifyCode)
	{
		/*case NM_SETCURSOR:
			bNotify = OnNotifyNmSetCursor((LPNMMOUSE)lphdr, lResult); 
		break;

		case NM_CLICK:
			bNotify = OnNotifyNmClick((LPNMHDR)lphdr, lResult); 
			break;*/
		case TVN_SELCHANGED:
			bNotify = OnNotifySelChanged((LPNMTREEVIEW)lphdr, lResult); 
			break;
		case TVN_SELCHANGING:
			bNotify = OnNotifySelChanging((LPNMTREEVIEW)lphdr, lResult); 
			break;
		case TVN_ITEMCHANGING:
			bNotify = OnNotifyItemChanging((NMTVITEMCHANGE*)lphdr); 
			break;
		case TVN_ITEMCHANGED:
			bNotify = OnNotifyItemChanged((NMTVITEMCHANGE*)lphdr, lResult); 
			break;
		case NM_CUSTOMDRAW:
			bNotify = OnNotifyCustomDraw((LPNMTVCUSTOMDRAW)lphdr, lResult);
			break;
		case TVN_GETDISPINFO:
			bNotify = OnNotifyGetDispInfo((LPNMTVDISPINFO)lphdr);
			break;
		
			 

	}

	return bNotify;	
}

 BOOL CALLBACK C32TreeView::OnNotifyGetDispInfo(LPNMTVDISPINFO NMTvDispInfo)
 {
	TVITEM & it = NMTvDispInfo->item;
	ZxDebug(("TvItem->mask %04x state: %04x ->lParam : %p\n",it.mask, it.state ,(LPVOID)it.lParam));
	
	if(it.mask == 0x40)
		return TRUE;

	if (it.mask & TVIF_CHILDREN)
	{
		it.cChildren = AsynGetChildren(it);
	}
	
	if (it.mask & TVIF_TEXT)
	{
		it.pszText = AsynGetText(it);
	}

	if (it.mask & TVIF_IMAGE)
	{
		it.iImage = AsynGetImage(it);
	}
	if (it.mask & TVIF_SELECTEDIMAGE)
	{
		//it.iImage = AsynGetImage(it);
		it.iSelectedImage = AsynGetSelectedImage(it);
	}
	it.mask |= TVIF_DI_SETITEM;
	if (it.mask & TVIF_STATE)
	{
		DebugBreak();
	}

	return TRUE;
 }
 
 HINSTANCE  C32ListView::hInstance;

 int C32ListView::InsertColumn(int iCol, int iSubItem, LPWSTR Text,int Format, int Cx,  int iImage, int CxMin)
 {
	BOOL Index = -1;
	
	LVCOLUMN lvc={};
	if (iImage != -1)
		lvc.mask |= LVCF_IMAGE;

	lvc.mask |=   LVCF_FMT | LVCF_TEXT | LVCF_WIDTH |  LVCF_SUBITEM | LVCF_MINWIDTH ;
	lvc.fmt = Format;
	lvc.pszText = Text;
	lvc.cchTextMax = (int)wcslen(Text);
	lvc.cx = Cx;
	lvc.iSubItem = iSubItem;
	lvc.iImage = iImage;
	lvc.cxMin = CxMin;



	if (iCol == 0)
	{
		if(ListView_SetColumn(m_Hwnd, iCol, &lvc) == TRUE)
			return 0;
	}
	Index = ListView_InsertColumn(m_Hwnd, iCol, &lvc);
	return Index;

 }
  
 int C32ListView::InsertItem(int iItem, LPCWSTR Text,  int iImage, LPARAM lParam)
 {
	int Index = -1;

	LVITEM lvi = {};

	if (iImage != -1)
		lvi.mask |= LVIF_IMAGE;

	lvi.mask |= LVIF_TEXT |LVIF_PARAM ;

	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	lvi.pszText = (LPWSTR)Text;
	lvi.cchTextMax = (int)wcslen(Text);
	lvi.lParam = lParam;
	lvi.iImage = iImage;
	
	Index = ListView_InsertItem(m_Hwnd, &lvi);
	
	{
		LVITEM lvi1={};
		lvi1.mask = LVIF_PARAM;
		lvi1.iItem = Index;
		BOOL Ret = ListView_GetItem(m_Hwnd, &lvi1);
		Ret = Ret;
	}
	
	return Index;
 }

 int C32ListView::InsertItem(int iItem, LPCWSTR Text,  int iImage, int iGroupId, LPARAM lParam)
 {
	 int Index = -1;

	 LVITEM lvi = {};

	 if (iImage != -1)
		 lvi.mask |= LVIF_IMAGE;
	 if (iGroupId != -1)
		 lvi.mask |= LVIF_GROUPID;

	 lvi.mask |= LVIF_TEXT |LVIF_PARAM ;

	 lvi.iItem = iItem;
	 lvi.iSubItem = 0;
	 lvi.pszText = (LPWSTR)Text;
	 lvi.cchTextMax = (int)wcslen(Text);
	 lvi.lParam = lParam;
	 lvi.iImage = iImage;
	 lvi.iGroupId = iGroupId;

	 Index = ListView_InsertItem(m_Hwnd, &lvi);

	 {
		 LVITEM lvi1={};
		 lvi1.mask = LVIF_PARAM;
		 lvi1.iItem = Index;
		 BOOL Ret = ListView_GetItem(m_Hwnd, &lvi1);
		 Ret = Ret;
	 }

	 return Index;
 }


 int C32ListView::InsertItem(int iItem, LPCWSTR Text, LPARAM lParam )
 {
	 return InsertItem(iItem, (LPCWSTR)Text, -1, lParam);
 }

 BOOL  C32ListView::InsertSubItem(int iItem, int iSubItem, LPCWSTR Text, int Image )
 {
	 return InsertSubItem(iItem, iSubItem, (LPWSTR)Text, Image);
 }


 BOOL C32ListView::InsertSubItem(int iItem, int iSubItem, LPWSTR Text, int iImage)
 {
	 BOOL bSet =FALSE;

	 LVITEM lvi = {};
	 if (iImage != -1)
		lvi.mask |= LVIF_IMAGE;
	 
	 lvi.mask |=  LVIF_TEXT  ;
	 lvi.iItem = iItem;
	 lvi.iSubItem = iSubItem;
	 lvi.pszText = Text;
	 lvi.cchTextMax = (int)wcslen(Text);
	
	 bSet = ListView_SetItem(m_Hwnd, &lvi);

	 {
		 LVITEM lvi1={};
		 lvi1.mask = LVIF_PARAM;
		 lvi1.iItem = iItem;
		 BOOL Ret = ListView_GetItem(m_Hwnd, &lvi1);
		 Ret = Ret;
	 }
	 return bSet;
 }

 int C32ListView::InsertItemAsyn(int iItem, LPCWSTR Text, int iImage, LPARAM lParam)
 {
	 int Index = -1;

	 LVITEM lvi = {};
	// lvi.mask = LVIF_DI_SETITEM;
	/* if (iImage != -1)
		 lvi.mask |= LVIF_IMAGE;*/

	
	 lvi.mask |= LVIF_PARAM;
	 lvi.mask |=  LVIF_TEXT;
	 lvi.mask |=  LVIF_IMAGE;
	
	 lvi.iItem = iItem;
	 lvi.iSubItem = 0;
	 lvi.pszText = LPSTR_TEXTCALLBACK;
	 lvi.cchTextMax = 260;
	 lvi.lParam = lParam;
	 lvi.iImage = I_IMAGECALLBACK ;
	 Index = ListView_InsertItem(m_Hwnd, &lvi);

	 {
		lvi.mask = LVIF_PARAM;
		ListView_SetItem(m_Hwnd, &lvi);
		LPARAM llParam = NULL;
		BOOL Ret = GetItemLParam(iItem, 0, &llParam);

		int xxxx = 0;
	 }

	 return Index;
 }

 int C32ListView::InsertSubItemAsyn(int iItem, int iSubItem, LPWSTR Text, int iImage, LPARAM lParam)
 {
	 int Index = -1;

	 LVITEM lvi = {};
	 if (iImage != -1)
		 lvi.mask |= LVIF_IMAGE;

	 lvi.mask |=  LVIF_TEXT | LVIF_PARAM  ;
	 lvi.iItem = iItem;
	 lvi.iSubItem = iSubItem;
	 lvi.pszText = LPSTR_TEXTCALLBACK;
	 lvi.iImage = I_IMAGECALLBACK ;
	 lvi.lParam = lParam;
	 //lvi.cchTextMax = (int)wcslen(Text);

	 Index = ListView_SetItem(m_Hwnd, &lvi);

	 return Index;
 }

 VOID InitC32Control(HINSTANCE hInstance)
 {
	 C32Hinstance = hInstance;
	 C32Menu::hInstance = hInstance;
	 C32ImageList::hInstance = hInstance;
	 C32TreeView::hInstance = hInstance;
	 C32Explorer::hInstance = hInstance;
	 C32Icon::hInstance = hInstance;
	 C32ListBox::hInstance = hInstance;
	 C32ListView::hInstance = hInstance;

	 C32Dialog::hInstance = hInstance;
	 C32DialogProgress::hInstance = hInstance;
	 C32ListBox::Initialize();
	 C32Explorer::Initialize();
	 C32GdiPlus::Initialize();
	 C32SysGdiObject::Initialize();
	 C32Container::Initialize();

	 C32DialogFile::m_GetOpenFileName = (DefenderGetOpenFileName)C32GetDllFunction(L"Comdlg32.dll", "GetOpenFileNameW");
	 C32DialogFile::m_GetSaveFileName = (DefenderGetOpenFileName)C32GetDllFunction(L"Comdlg32.dll", "GetSaveFileNameW");

	 //SetProcessDPIAware();
	 C32FindReplaceMsg = RegisterWindowMessage(FINDMSGSTRING);
	 C32SystemFont = (HFONT)C32GetSystemFont();

 }

 VOID UnInitC32Control()
 {
	if (C32SystemFont)
	{
		DeleteFont(C32SystemFont);
		C32SystemFont = NULL;
	}
 }

 
