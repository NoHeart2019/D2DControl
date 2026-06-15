#include "stdafx.h"
#include <d2d1.h>


#include "D2DTest.h"
#include "StyleManager.h"


void SetScrollStyle(CD2DScrollUI& scroll)
{// 1. 定义 Normal 样式
	CD2DScrollStyle normalStyle;
	normalStyle.track.background.m_type = CD2DBackground::Solid;
	normalStyle.track.background.m_solid.color = D2D1::ColorF(0.9f, 0.9f, 0.9f);  // 浅灰
	normalStyle.thumb.background.m_type = CD2DBackground::Solid;
	normalStyle.thumb.background.m_solid.color = D2D1::ColorF(0.6f, 0.6f, 0.6f);  // 中灰
	normalStyle.thumbMinLength = 20.0f;
	scroll.SetScrollStyle(ED2DStatus::Normal, normalStyle);

	// 2. 定义 Hover 样式（滑块变深）
	CD2DScrollStyle hoverStyle = normalStyle;
	hoverStyle.thumb.background.m_solid.color = D2D1::ColorF(0.4f, 0.4f, 0.4f);   // 深灰
	scroll.SetScrollStyle(ED2DStatus::Hover, hoverStyle);

	// 3. 定义 Pressed 样式（滑块变蓝）
	CD2DScrollStyle pressedStyle = normalStyle;
	pressedStyle.thumb.background.m_solid.color = D2D1::ColorF(0.2f, 0.4f, 0.8f);  // 蓝色
	scroll.SetScrollStyle(ED2DStatus::Pressed, pressedStyle);


	CD2DScrollStyle disabledStyle = normalStyle;
	disabledStyle.thumb.background.m_solid.color = D2D1::ColorF(0.8f, 0.8f, 0.8f); // 浅灰
	scroll.SetScrollStyle(ED2DStatus::Disabled, disabledStyle);

}

// --- 按钮通用样式设置函数 ---
void SetupButton (CD2DControlUI& btn, const CAtlString& text, D2D1_COLOR_F bgColor)
{
	btn.SetText(text);

	// 默认样式
	CD2DStyle normal;
	normal.background.m_type = CD2DBackground::Solid;
	normal.background.m_solid.color = bgColor;
	normal.border.m_top = CD2DBorderEdge(1.5f, Solid, D2D1::ColorF(0.4f, 0.4f, 0.4f));
	normal.border.m_right = normal.border.m_top;
	normal.border.m_bottom = normal.border.m_top;
	normal.border.m_left = normal.border.m_top;
	normal.radius = CD2DRadius(0.5f);
	normal.text.color = D2D1::ColorF(0.0f, 0.0f, 0.0f);
	normal.text.fontFamily=L"Segoe UI";
	normal.text.fontSize = 14.0f;
	normal.text.horizontalAlign = DWRITE_TEXT_ALIGNMENT_CENTER;
	normal.text.verticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
	btn.SetStyle(ED2DStatus::Normal, normal);

	// 悬停样式：背景变亮，边框变蓝
	CD2DStyle hover;
	hover.background.m_type = CD2DBackground::Solid;
	hover.background.m_solid.color = D2D1::ColorF(bgColor.r * 1.1f, bgColor.g * 1.1f, bgColor.b * 1.1f);
	hover.border.m_top = CD2DBorderEdge( 1.5f, Solid, D2D1::ColorF(0.2f, 0.6f, 1.0f) );
	hover.border.m_right = hover.border.m_top;
	hover.border.m_bottom = hover.border.m_top;
	hover.border.m_left = hover.border.m_top;
	btn.SetStyle(ED2DStatus::Hover, hover);

	// 按下样式：背景变暗
	CD2DStyle pressed;
	pressed.background.m_type = CD2DBackground::Solid;
	pressed.background.m_solid.color = D2D1::ColorF(bgColor.r * 0.8f, bgColor.g * 0.8f, bgColor.b * 0.8f);
	btn.SetStyle(ED2DStatus::Pressed, pressed);

	btn.SetDefaultStyle(normal);
};

// ========== 单击回调 ==========
void WINAPI OnControl1Click(CD2DControlUI* pControl, void* pUserData)
{
	UNREFERENCED_PARAMETER(pUserData);
	OutputDebugStringA("Event: Click\n");
}

// ========== 双击回调 ==========
void WINAPI OnControl1DoubleClick(CD2DControlUI* pControl, void* pUserData)
{
	UNREFERENCED_PARAMETER(pUserData);
	OutputDebugStringA("Event: DoubleClick\n");
	// 双击时输出用户自定义数据（这里演示 userData 用法）
	const char* msg = static_cast<const char*>(pUserData);
	if (msg) OutputDebugStringA(msg);
}

// ========== 悬停进入/离开 ==========
void WINAPI OnControl1HoverEnter(CD2DControlUI* pControl, void* pUserData)
{
	UNREFERENCED_PARAMETER(pUserData);
	OutputDebugStringA("Event: HoverEnter\n");
}

void WINAPI OnControl1HoverLeave(CD2DControlUI* pControl, void* pUserData)
{
	UNREFERENCED_PARAMETER(pUserData);
	OutputDebugStringA("Event: HoverLeave\n");
}

// ========== 拖拽回调 ==========
void WINAPI OnControl1DragBegin(CD2DControlUI* pControl, void* pUserData)
{
	UNREFERENCED_PARAMETER(pUserData);
	OutputDebugStringA("Event: DragBegin\n");
}

void WINAPI OnControl1DragMove(CD2DControlUI* pControl, void* pUserData)
{
	UNREFERENCED_PARAMETER(pUserData);
	OutputDebugStringA("Event: DragMove\n");
}

void WINAPI OnControl1DragEnd(CD2DControlUI* pControl, void* pUserData)
{
	UNREFERENCED_PARAMETER(pUserData);
	OutputDebugStringA("Event: DragEnd\n");
}

// ========== 3. 绑定事件回调 ==========
static void WINAPI ClickHandler(CD2DControlUI* pControl, void* pUserData) {
	
	CAtlString msg;
	msg.Format(L"点击了控件: %s\n", pControl->GetText());
	OutputDebugString(msg);
};


static void WINAPI DoubleClickHandler(CD2DControlUI* pControl, void* pUserData) {
	CAtlString* pName = static_cast<CAtlString*>(pUserData);
	CAtlString msg;
	msg.Format(L"双击了控件: %s\n", *pName);
	OutputDebugString(msg);
};

static void WINAPI DragHandler(CD2DControlUI* pControl, void* pUserData) {
	CAtlString* pName = static_cast<CAtlString*>(pUserData);
	CAtlString msg;
	msg.Format(L"拖拽移动: %s\n", *pName);
	OutputDebugString(msg);
};

static void WINAPI ListItemSelect(CD2DListUI* pList, int newIndex, void* pUserData) {
	CAtlString msg;
	/*msg.Format(L"列表选择变化：新索引 = %d, 文本 = %s\n",
		newIndex, pList->GetItem(newIndex)->GetText().GetString());*/
	OutputDebugString(msg);
}

// 测试回调函数
void TestScrollCallback(CD2DScrollUI* pScrollBar, int newValue, void* pUserData)
{
	int* pCounter = (int*)pUserData;
	(*pCounter)++;
	ZxDebugEx(("Scroll callback: newValue = %d\n", pScrollBar->GetValue()));
}

#pragma comment(lib, "winmm.lib")

DWORD CD2DTest::Register()
{
	return RegisterClassEx32(L"CD2DTest");
}

CD2DTest::CD2DTest()
{
	
	//container.AddControl(&control2);

}

CD2DTest::~CD2DTest()
{

}

BOOL CD2DTest::CreateUI(DWORD ExStyle, LPCWSTR lpwszWindowName,DWORD Style, int xPos, int yPos, int Cx, int Cy, HWND hwnd)
{

	Create(ExStyle, L"CD2DTest", lpwszWindowName, Style, xPos, yPos, Cx, Cy, hwnd);

	return (m_Hwnd != NULL);
}

// 创建一个外观类似 HTML 默认复选框的 CD2DCheckbox
VOID CD2DTest::InitializeStyleBox()
{
	// ---- Normal 状态 ----
	normalBoxStyle.box.background.m_type = CD2DBackground::Solid;
	normalBoxStyle.box.background.m_solid.color = D2D1::ColorF(1.0f, 1.0f, 1.0f);         // 白底
	normalBoxStyle.box.border.m_top = CD2DBorderEdge(1.0f, Solid, D2D1::ColorF(0.5f,0.5f,0.5f)); // 灰边框
	normalBoxStyle.box.border.m_right = normalBoxStyle.box.border.m_top;
	normalBoxStyle.box.border.m_bottom = normalBoxStyle.box.border.m_top;
	normalBoxStyle.box.border.m_left = normalBoxStyle.box.border.m_top;
	normalBoxStyle.box.radius = CD2DRadius(2.0f);                                      // 小圆角
	normalBoxStyle.text.color = D2D1::ColorF(0.0f, 0.0f, 0.0f);                        // 黑色文字
	normalBoxStyle.text.fontFamily = L"Segoe UI";
	normalBoxStyle.text.fontSize = 14.0f;
	normalBoxStyle.checkmark = D2D1::ColorF(0.0f, 0.0f, 0.0f);                         // 黑色对勾（未选中不显示，但设定好颜色）
	checkbox.SetBoxStyle(Hover, normalBoxStyle);
	

	// ---- Hover 状态 ----
	
	hoverBoxStyle.box.background.m_type = CD2DBackground::Solid;
	hoverBoxStyle.box.background.m_solid.color = D2D1::ColorF(0.95f, 0.95f, 0.95f);    // 浅灰底
	hoverBoxStyle.box.border.m_top = CD2DBorderEdge(1.0f, Solid, D2D1::ColorF(0.3f,0.3f,0.3f));
	hoverBoxStyle.box.border.m_right = hoverBoxStyle.box.border.m_top;
	hoverBoxStyle.box.border.m_bottom = hoverBoxStyle.box.border.m_top;
	hoverBoxStyle.box.border.m_left = hoverBoxStyle.box.border.m_top;
	hoverBoxStyle.box.radius = CD2DRadius(2.0f);
	hoverBoxStyle.text.color = D2D1::ColorF(0.0f, 0.0f, 0.0f);
	hoverBoxStyle.text.fontSize = 14.0f;
	hoverBoxStyle.checkmark = D2D1::ColorF(0.0f, 0.0f, 0.0f);                         // 悬停时对勾颜色不变（黑色）
	checkbox.SetBoxStyle(Hover, hoverBoxStyle);

	// ---- Checked 状态 ----
	
	checkedBoxStyle.box.background.m_type = CD2DBackground::Solid;
	checkedBoxStyle.box.background.m_solid.color = D2D1::ColorF(0.0f, 0.47f, 0.91f);   // 蓝色背景（类似系统主题）
	checkedBoxStyle.box.border.m_top = CD2DBorderEdge(1.0f, Solid, D2D1::ColorF(0.0f,0.36f,0.73f));
	checkedBoxStyle.box.border.m_right = checkedBoxStyle.box.border.m_top;
	checkedBoxStyle.box.border.m_bottom = checkedBoxStyle.box.border.m_top;
	checkedBoxStyle.box.border.m_left = checkedBoxStyle.box.border.m_top;
	checkedBoxStyle.box.radius = CD2DRadius(2.0f);
	checkedBoxStyle.text.color = D2D1::ColorF(0.0f, 0.0f, 0.0f);                        // 文字仍为黑色
	checkedBoxStyle.text.fontSize = 14.0f;
	checkedBoxStyle.checkmark = D2D1::ColorF(1.0f, 1.0f, 1.0f);                         // 白色对勾，与蓝底形成对比
	checkbox.SetBoxStyle(ED2DStatus::Checked, checkedBoxStyle);

	// 可选：设置默认样式（回退用）
	checkbox.SetDefaultBoxStyle(normalBoxStyle);

	checkbox.SetText(L"记住密码");

	checkbox.SetDefaultStyle(defaultStyle);
}

VOID CD2DTest::InitializeStyleEdit()
{
	edit.SetStyle(ED2DStatus::Focused, focusStyle);
	edit.SetMode(CD2DTextEdit::SingleLine);
	edit.SetPlaceholder(L"请输入内容");
	edit.SetRender(this);
	SetScrollStyle(*edit.GetScrollBar());
	edit.SetDefaultStyle(defaultStyle);
	edit.SetWidthPolicy(CD2DSizePolicy::FixedSize(120.0f));
	edit.SetHeightPolicy(CD2DSizePolicy::FixedSize(40.0f));

	editStyle = defaultStyle;
	editStyle.text.horizontalAlign = DWRITE_TEXT_ALIGNMENT_LEADING;
	editStyle.text.verticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;


}

VOID CD2DTest::InitializeStyleSelect()
{
	// 设置样式
	normalSelectStyle.trigger.background.m_solid.color = D2D1::ColorF(0.96f, 0.96f, 0.96f);
	normalSelectStyle.trigger.border.m_top = CD2DBorderEdge(1.5f, Solid, D2D1::ColorF(0.4f, 0.4f, 0.4f));
	normalSelectStyle.trigger.border.m_right = normalSelectStyle.trigger.border.m_top;
	normalSelectStyle.trigger.border.m_bottom = normalSelectStyle.trigger.border.m_top;
	normalSelectStyle.trigger.border.m_left = normalSelectStyle.trigger.border.m_top;
	normalSelectStyle.trigger.radius = CD2DRadius(6.0f);
	normalSelectStyle.trigger.text.color = D2D1::ColorF(0.0f, 0.0f, 0.0f);
	normalSelectStyle.trigger.text.fontSize = 14.0f;
	normalSelectStyle.arrowColor = D2D1::ColorF(0.3f, 0.3f, 0.3f);


	select.SetSelectStyle(Normal, normalSelectStyle);
	select.SetDefaultSelectStyle(normalSelectStyle);

	// 添加选项（简单的文本控件）
	select.AddItem(new CD2DControlUI()); // 实际项目中可预先设置文本和样式
	select.AddItem(new CD2DControlUI());
	select.AddItem(new CD2DControlUI());
	select.AddItem(new CD2DControlUI());
	select.GetItem(0)->SetText(L"苹果");
	select.GetItem(1)->SetText(L"香蕉");
	select.GetItem(2)->SetText(L"樱桃");
	select.GetItem(3)->SetText(L"火龙果");
	select.SetMaxDropDownItems(3);
	// 设置默认选中项
	select.SetSelectedIndex(0);


}

VOID CD2DTest::InitializeStyleTextArea()
{
	textArea.SetWidthPolicy(CD2DSizePolicy::FixedSize(300));
	textArea.SetHeightPolicy(CD2DSizePolicy::FixedSize(150));

	textArea.SetText(L"这是一段很长的文本，用于测试多行显示。文本会自动换行，并且当内容超出区域时会出现垂直滚动条。这是一段很长的文本，用于测试多行显示。文本会自动换行，并且当内容超出区域时会出现垂直滚动条。这是一段很长的文本，用于测试多行显示。文本会自动换行，并且当内容超出区域时会出现垂直滚动条。这是一段很长的文本，用于测试多行显示。文本会自动换行，并且当内容超出区域时会出现垂直滚动条。这是一段很长的文本，用于测试多行显示。文本会自动换行，并且当内容超出区域时会出现垂直滚动条。这是一段很长的文本，用于测试多行显示。文本会自动换行，并且当内容超出区域时会出现垂直滚动条。这是一段很长的文本，用于测试多行显示。文本会自动换行，并且当内容超出区域时会出现垂直滚动条。这是一段很长的文本，用于测试多行显示。文本会自动换行，并且当内容超出区域时会出现垂直滚动条。这是一段很长的文本，用于测试多行显示。文本会自动换行，并且当内容超出区域时会出现垂直滚动条。这是一段很长的文本，用于测试多行显示。文本会自动换行，并且当内容超出区域时会出现垂直滚动条。这是一段很长的文本，用于测试多行显示。文本会自动换行，并且当内容超出区域时会出现垂直滚动条。这是一段很长的文本，用于测试多行显示。文本会自动换行，并且当内容超出区域时会出现垂直滚动条。这是一段很长的文本，用于测试多行显示。文本会自动换行，并且当内容超出区域时会出现垂直滚动条。");
	// 1. 设置默认样式（类似 CSS 的 .textarea）
	// 背景：白色
	textAreaStyle.background.m_type = CD2DBackground::Solid;
	textAreaStyle.background.m_solid.color = D2D1::ColorF(1.0f, 1.0f, 1.0f); 
	// 边框：1px 实线灰色
	textAreaStyle.border.m_top    = CD2DBorderEdge(1.5f, Solid, D2D1::ColorF(0.6f, 0.6f, 0.6f));
	textAreaStyle.border.m_right  = textAreaStyle.border.m_top;
	textAreaStyle.border.m_bottom = textAreaStyle.border.m_top;
	textAreaStyle.border.m_left   = textAreaStyle.border.m_top;
	// 圆角 6px
	textAreaStyle.radius = CD2DRadius(6.0f);
	// 内边距 10px
	textAreaStyle.padding = CD2DPadding(10.0f);
	// 文字样式：深灰色，Segoe UI，14号
	textAreaStyle.text.color = D2D1::ColorF(0.2f, 0.2f, 0.2f);
	textAreaStyle.text.fontFamily = L"Segoe UI";
	textAreaStyle.text.fontSize = 14.0f;
	textAreaStyle.text.wordWrap = true;   // 自动换行
	// 设置行高 (1.5倍)
	textAreaStyle.text.lineHeight = 1.5f;
	textArea.SetDefaultStyle(textAreaStyle);

	// 2. 设置焦点样式（获得焦点时边框变蓝，背景微蓝）
	CD2DStyle focusedStyle;
	focusedStyle.background.m_type = CD2DBackground::Solid;
	focusedStyle.background.m_solid.color = D2D1::ColorF(0.98f, 0.98f, 1.0f);
	focusedStyle.border.m_top    = CD2DBorderEdge(2.0f, Solid, D2D1::ColorF(0.2f, 0.5f, 1.0f));
	focusedStyle.border.m_right  = focusedStyle.border.m_top;
	focusedStyle.border.m_bottom = focusedStyle.border.m_top;
	focusedStyle.border.m_left   = focusedStyle.border.m_top;
	// 文字颜色保持不变（继承）
	textArea.SetStyle(ED2DStatus::Focused, focusedStyle);

}

VOID CD2DTest::InitializeStyleExplorer()
{
	StyleManager& styleMgr =  StyleManager::Get();
	for (int i = 0; i < 38; ++i)
	{
		CD2DControlUI* pItem = new CD2DControlUI();
		CAtlString text;
		text.Format(L"Item: %d", i);
		pItem->SetText(text);
		pItem->SetEventCallback(CD2DEvent::Click, CD2DEvent(OnControl1Click));
		
		styleMgr.ApplyListItemStyle(*(CD2DControlUI*)pItem, L"default-item");
		pItem->EnableDrag(true);
		listview.AddItem(pItem);
	}

	listview.EnableDragging(true);
	listview.SetViewMode(CD2DExploreUI::Wrap);
	listview.SetSelectionMode(CD2DExploreUI::Single);
	listview.SetWrapAutoFit(true);
	listview.SetHorizontalSpacing(8.0f);
	listview.SetVerticalSpacing(8.0f);
	listview.SetColumns(4);
	listview.SetItemWidth(80.0f);
	listview.SetItemHeight(60.0f);
	listview.SetDefaultStyle(defaultStyle);
	listview.SetWidthPolicy(CD2DSizePolicy::FixedSize(390.0f));    // 宽度自适应
	listview.SetHeightPolicy(CD2DSizePolicy::FixedSize(190.0f));    // 宽度自适应




}



VOID CD2DTest::InitializeStyleDefault()
{

	// 1.1 外边距：上下左右统一 5px
	defaultStyle.margin    = CD2DMargin(12.0f);

	// 1.2 边框：四边均为 2px 实线深灰色

	defaultStyle.border.m_top    = CD2DBorderEdge(2.0f, Solid, D2D1::ColorF(0.4f, 0.4f, 0.4f));
	defaultStyle.border.m_right  = defaultStyle.border.m_top;
	defaultStyle.border.m_bottom = defaultStyle.border.m_top;
	defaultStyle.border.m_left   = defaultStyle.border.m_top;

	// 1.3 内边距：上下左右统一 10px
	defaultStyle.padding    = CD2DPadding(8.0f);


	// 1.4 圆角：4px
	defaultStyle.radius = CD2DRadius( 8.0f );

	// 1.5 背景：浅灰色纯色
	defaultStyle.background.m_type = CD2DBackground::Solid;
	defaultStyle.background.m_solid.color = D2D1::ColorF(0.9f, 0.9f, 0.9f);

	//1.6文字
	defaultStyle.text.fontFamily = L"Segoe UI";
	defaultStyle.text.color = D2D1::ColorF(0.2f, 0.2f, 0.2f);
	defaultStyle.text.fontSize = 14.0f;
	defaultStyle.text.horizontalAlign = DWRITE_TEXT_ALIGNMENT_CENTER;
	defaultStyle.text.verticalAlign   = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;

	// ==================== 2. 设置各交互状态样式（只改需要变化的部分） ====================
	// 2.1 Normal 状态：显式指定背景色（与默认相同，仅作示范）
	normalStyle.background.m_type = CD2DBackground::Solid;
	normalStyle.background.m_solid.color = D2D1::ColorF(0.9f, 0.9f, 0.9f);
	normalStyle.text.fontFamily = L"Segoe UI";
	normalStyle.text.color = D2D1::ColorF(0.2f, 0.2f, 0.2f);
	normalStyle.text.fontSize = 14.0f;
	normalStyle.text.horizontalAlign = DWRITE_TEXT_ALIGNMENT_CENTER;
	normalStyle.text.verticalAlign   = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;

	// 2.2 Hover 状态：背景变浅蓝，边框变蓝，其它属性继承默认
	hoverStyle.text =  normalStyle.text;
	hoverStyle.text.color = D2D1::ColorF(1.0f, 0.0f, 0.0f); // 悬停变红

	hoverStyle.background.m_type = CD2DBackground::Solid;
	hoverStyle.background.m_solid.color = D2D1::ColorF(0.8f, 0.9f, 1.0f);	
	hoverStyle.border.m_top    = CD2DBorderEdge(2.0f, Solid, D2D1::ColorF(0.5f, 0.6f, 0.4f));
	hoverStyle.border.m_right  = hoverStyle.border.m_top;
	hoverStyle.border.m_bottom = hoverStyle.border.m_top;
	hoverStyle.border.m_left   = hoverStyle.border.m_top;	
	hoverStyle.transform.SetTranslate(0.0f, -2.0f);

	pressedStyle.background.m_type = CD2DBackground::Solid;
	pressedStyle.background.m_solid.color = D2D1::ColorF(0.6f, 0.7f, 0.9f);


	checkedStyle.background.m_type = CD2DBackground::Solid;
	checkedStyle.background.m_solid.color = D2D1::ColorF(0.65f, 0.75f, 0.95f);

	disabledStyle.background.m_type = CD2DBackground::Solid;
	disabledStyle.background.m_solid.color = D2D1::ColorF(0.85f, 0.85f, 0.85f);

	focusStyle.background.m_solid.color = D2D1::ColorF(0.98f, 0.98f, 1.0f); // 淡蓝背景
	focusStyle.border.m_top = CD2DBorderEdge(1.5f, Solid, D2D1::ColorF(0.2f, 0.5f, 1.0f));
	focusStyle.border.m_right = focusStyle.border.m_top;
	focusStyle.border.m_bottom = focusStyle.border.m_top;
	focusStyle.border.m_left = focusStyle.border.m_top;


	containerStyle.background.m_type = CD2DBackground::Solid;
	containerStyle.background.m_solid.color = D2D1::ColorF(0.95f, 0.95f, 0.95f);
	containerStyle.border.m_top = CD2DBorderEdge( 1.0f, Solid, D2D1::ColorF(0.7f, 0.7f, 0.7f));
	containerStyle.border.m_right = containerStyle.border.m_top;
	containerStyle.border.m_bottom = containerStyle.border.m_top;
	containerStyle.border.m_left = containerStyle.border.m_top;
	containerStyle.radius = CD2DRadius(8.0f);
	containerStyle.margin = CD2DMargin(10.0f);
	containerStyle.padding = CD2DPadding(10.0f);

	containerStyle1 = containerStyle;
	containerStyle1.background.m_solid.color = D2D1::ColorF(0.85f, 0.85f, 0.85f);

	containerStyle2 = containerStyle;
	containerStyle2.background.m_solid.color = D2D1::ColorF(0.65f, 0.65f, 0.65f);

	focusStyle.transform.SetScale(1.05f, 1.05f);

}

VOID CD2DTest::InitializeStyle()
{
	InitializeStyleDefault();
	
	itemStyle.normal = normalStyle;
	itemStyle.hover = checkedStyle;
	itemStyle.checked = checkedStyle;

	StyleManager::Reigster();

	InitializeStyleBox();
	InitializeStyleEdit();
	InitializeStyleTextArea();
	InitializeStyleSelect();
	InitializeStyleExplorer();

}

VOID CD2DTest::InitializeControls()
{
	InitializeStyle();
	control.SetDefaultStyle(defaultStyle);
	control.SetStyle(Normal, normalStyle);
	control.SetStyle(Hover, hoverStyle);
	control.SetStyle(Pressed, pressedStyle);
	control.SetStyle(ED2DStatus::Checked, checkedStyle);
	control.SetStyle(Disabled, disabledStyle);
	control.SetRectangle(D2D1::RectF(50.0f, 50.0f, 200.0f, 200.0f));
	control.SetText(L"确认");

	//3.3 开启拖拽
	control.EnableDrag(true);
	
	// 单击（无用户数据）
	control.SetEventCallback(CD2DEvent::Click, CD2DEvent(OnControl1Click));

	// 双击（带用户数据）
	const char* doubleClickMsg = "Double-click data";
	control.SetEventCallback(CD2DEvent::DoubleClick,
		CD2DEvent(OnControl1DoubleClick, (void*)doubleClickMsg));

	// 悬停进入/离开（无用户数据）
	control.SetEventCallback(CD2DEvent::HoverEnter, CD2DEvent(OnControl1HoverEnter));
	control.SetEventCallback(CD2DEvent::HoverLeave, CD2DEvent(OnControl1HoverLeave));
	// 拖拽（无用户数据）
	control.SetEventCallback(CD2DEvent::DragBegin, CD2DEvent(OnControl1DragBegin));
	control.SetEventCallback(CD2DEvent::DragMove, CD2DEvent(OnControl1DragMove));
	control.SetEventCallback(CD2DEvent::DragEnd, CD2DEvent(OnControl1DragEnd));

	// ==================== control2 样式设置：线性渐变背景 + 虚线边框（宽度变化） ====================
	container.SetDefaultStyle(containerStyle);
	scroll.SetOrientation(CD2DScrollUI::Vertical);
	scroll.SetVisible(true);
	scroll.SetRectangle(D2D1::RectF(300, 220, 330, 520));  // 宽100，高200
	static int callbackCount = 0;
	scroll.SetScrollCallback(TestScrollCallback, &callbackCount);
	scroll.SetRange(0, 100);
	scroll.SetPageSize(20);
	scroll.SetValue(80);
	scroll.SetOrientation(CD2DScrollUI::Vertical);
	container.SetRectangle(D2D1::RectF(50.0f, 240.0f, 350.0f, 520.0f));
	
	StyleManager& styleMgr = StyleManager::Get();
	
	
	
	styleMgr.ApplyButtonStyle(btn1, L"blue-button");
	styleMgr.ApplyButtonStyle(btn2, L"blue-button");
	styleMgr.ApplyButtonStyle(btn3, L"blue-button");
	styleMgr.ApplyButtonStyle(btn4, L"blue-button");
	styleMgr.ApplyButtonStyle(btn5, L"blue-button");
	styleMgr.ApplyButtonStyle(btn6, L"blue-button");

	btn1.SetEventCallback(CD2DEvent::Click, CD2DEvent(ClickHandler));
	btn2.SetEventCallback(CD2DEvent::Click, CD2DEvent(ClickHandler));
	btn3.SetEventCallback(CD2DEvent::Click, CD2DEvent(ClickHandler));
	btn4.SetEventCallback(CD2DEvent::Click, CD2DEvent(ClickHandler));
	btn5.SetEventCallback(CD2DEvent::Click, CD2DEvent(ClickHandler));
	btn6.SetEventCallback(CD2DEvent::Click, CD2DEvent(ClickHandler));

	btn1.SetText(L"按钮 A");
	btn2.SetText(L"按钮 B");
	btn3.SetText(L"按钮 C");
	btn4.SetText(L"按钮 D");
	btn5.SetText(L"按钮 E");
	btn6.SetText(L"按钮 F");

	

	btn3.EnableDrag(true);

	hbox.SetDefaultStyle(containerStyle1);
	hbox1.SetDefaultStyle(containerStyle2);
	hbox2.SetDefaultStyle(containerStyle2);

	vbox.SetDefaultStyle(containerStyle2);
	// 列表项（在循环中应用样式）

	hbox.SetText(L"hbox");
	hbox1.SetText(L"hbox1");
	hbox2.SetText(L"hbox2");

	vbox.SetText(L"vbox");

	
	
	// 5. 设置最大可见项数为 5（这样会显示垂直滚动条）
	//list.SetMaxVisibleItems(5);
	// 项高度设置为 30 像素
	
	list.SetItemHeight(30.0f);
	list.SetWidthPolicy(CD2DSizePolicy::FixedSize(150));
	list.SetHeightPolicy(CD2DSizePolicy::FixedSize(150));
	// 6. 选择变化回调
	//list.SetOnSelectionChanged(ListItemSelect, nullptr);
	list.SetDefaultStyle(defaultStyle);
	list.SetItemStyle(itemStyle);
	for (int i = 0; i < 38; ++i)
	{
		CD2DControlUI* pItem = new CD2DControlUI();
		CAtlString text;
		text.Format(L"Item: %d", i);
		pItem->SetText(text);

		pItem->EnableDrag(true);
		list.AddItem(pItem);
	}


	container1.SetDefaultStyle(containerStyle1); 
	container2.SetDefaultStyle(containerStyle2); 
	container.SetDefaultStyle(containerStyle); 

	// 容器与布局
	btn1.SetWidthPolicy(CD2DSizePolicy::AutoSize());    // 宽度自适应
	btn1.SetHeightPolicy(CD2DSizePolicy::AutoSize()); // 高度固定 30 像素

	btn2.SetWidthPolicy(CD2DSizePolicy::AutoSize());    // 宽度自适应
	btn2.SetHeightPolicy(CD2DSizePolicy::AutoSize());    // 宽度自适应

	btn6.SetAbsoluteOffset(20, 20);
	btn6.SetWidthPolicy(CD2DSizePolicy::FixedSize(50));    // 宽度填充剩余
	btn6.SetHeightPolicy(CD2DSizePolicy::FixedSize(50));    // 宽度填充剩余
	
	btn5.SetWidthPolicy(CD2DSizePolicy::FillSize());    // 宽度自适应
	btn5.SetHeightPolicy(CD2DSizePolicy::AutoSize());    // 宽度自适应


	btn3.SetWidthPolicy(CD2DSizePolicy::AutoSize());    // 宽度自适应
	btn3.SetHeightPolicy(CD2DSizePolicy::AutoSize());    // 宽度自适应

	

	hbox1.SetWidthPolicy(CD2DSizePolicy::AutoSize()); // 高度自适应
	hbox1.SetHeightPolicy(CD2DSizePolicy::AutoSize()); // 高度自适应
	 
	hbox2.SetHeightPolicy(CD2DSizePolicy::FixedSize(280.0f)); // 高度自适应
	hbox2.SetWidthPolicy(CD2DSizePolicy::PercentSize(80.0f)); // 高度自适应

	vbox.SetHeightPolicy(CD2DSizePolicy::AutoSize()); // 高度自适应
	vbox.SetWidthPolicy(CD2DSizePolicy::ExpandWeight(1.0f)); // 高度自适应

	select.SetWidthPolicy(CD2DSizePolicy::FixedSize(80.0f)); // 高度自适应
	select.SetHeightPolicy(CD2DSizePolicy::FixedSize(40.0f)); // 高度自适应


	hbox1.AddChild(&btn1);
	hbox1.AddChild(&btn2);
	hbox1.AddChild(&checkbox);
	hbox1.AddChild(&edit);
	hbox1.AddChild(&select);
	hbox2.AddChild(&list);
	hbox2.AddChild(&textArea);
	hbox2.AddChild(&listview);
	vbox.AddChild(&btn5);
	
	vbox.AddChild(&btn3);
	vbox.AddChild(&btn4);
	
	container.AddChild(&hbox1);
	container.AddChild(&hbox2);
	container.AddChild(&vbox);
	container.SetOverflow(CD2DContainerUI::Auto); // 自动显示滚动条
	vbox.SetOverflow(CD2DContainerUI::Scroll); // 自动显示滚动条
	hbox2.SetOverflow(CD2DContainerUI::Auto); // 自动显示滚动条
	
}

VOID CD2DTest::SetControlsLayout()
{
	D2D1_RECT_F rectangle = GetRectangle();
	D2D1_SIZE_F size = GetSize();
	//box.Layout();
	//list.Layout();
	container.SetPosition(0.0f, 0.0f);
	container.SetSize(size.width, size.height);
}

BOOL CALLBACK CD2DTest::OnCreate(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	InitializeRenderTarget(m_Hwnd);	
	InitializeControls();
	SetControlsLayout();

	

	return TRUE;
}

BOOL CALLBACK CD2DTest::OnPaint(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	PAINTSTRUCT ps;
	BeginPaint(m_Hwnd, &ps);
	BeginDraw();


	Clear(D2D1::ColorF(D2D1::ColorF::White));
	
	//DrawControl(this);
	//control.DrawControl(this);
	//control2.DrawControl(this);
	//list.DrawControl(this);
	container.DrawControl(this);
	//m_box.DrawControl(this);
	EndDraw();

	EndPaint(m_Hwnd, &ps);

	return TRUE;
}

BOOL CALLBACK CD2DTest::OnSize(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	float width = GET_X_LPARAM_F(lParam);
	float height = GET_Y_LPARAM_F(lParam);
	CD2DRender::Resize(width, height);
	
	SetRectangle(D2D1::RectF(0.0f, 0.0f, ScaleX(width, TRUE), ScaleY(height, TRUE)));
	SetControlsLayout();
	return TRUE;
}

BOOL CALLBACK CD2DTest::OnLButtonDown(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	D2D1_POINT_2F point = D2D1::Point2F(ScaleX((float)GET_X_LPARAM_F(lParam), TRUE),
										ScaleY((float)GET_Y_LPARAM_F(lParam), TRUE));
	
	control.OnMouseDownL(point);
	control2.OnMouseDownL(point);

	container.OnMouseDownL(point);

	InvalidateRect(NULL, FALSE);


	return TRUE;
}

BOOL CALLBACK CD2DTest::OnLButtonUp(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	D2D1_POINT_2F point = D2D1::Point2F((float)ScaleX(GET_X_LPARAM_F(lParam), TRUE),
		(float)ScaleY(GET_Y_LPARAM_F(lParam), TRUE));
	
	control.OnMouseUpL(point);
	control2.OnMouseUpL(point);

	container.OnMouseUpL(point);
	
	InvalidateRect(NULL, FALSE);

	return TRUE;
}

BOOL CALLBACK CD2DTest::OnLButtonDown2(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	D2D1_POINT_2F point = D2D1::Point2F(ScaleX((float)GET_X_LPARAM_F(lParam), TRUE),
		ScaleY((float)GET_Y_LPARAM_F(lParam), TRUE));


	control.OnMouseDownL2(point);
	control2.OnMouseDownL2(point);

	container.OnMouseDownL2(point);

	InvalidateRect(NULL, FALSE);


	return TRUE;
}

BOOL CALLBACK CD2DTest::OnLButtonUp2(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	D2D1_POINT_2F point = D2D1::Point2F((float)ScaleX(GET_X_LPARAM_F(lParam), TRUE),
		(float)ScaleY(GET_Y_LPARAM_F(lParam), TRUE));
	
	control.OnMouseUpL2(point);
	control2.OnMouseUpL2(point);

	container.OnMouseUpL2(point);
	

	InvalidateRect(NULL, FALSE);

	return TRUE;
}

BOOL CALLBACK CD2DTest::OnMouseMove(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	D2D1_POINT_2F point = D2D1::Point2F((float)ScaleX(GET_X_LPARAM_F(lParam), TRUE),
		(float)ScaleY(GET_Y_LPARAM_F(lParam), TRUE));

	control.OnMouseMove(point);
	control2.OnMouseMove(point);

	container.OnMouseMove(point);

	
	InvalidateRect(NULL, FALSE);

	return TRUE;
}

BOOL CALLBACK CD2DTest::OnKeyDown(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	UINT keyDown = (UINT)wParam;
	
	control.OnKeyDown(keyDown);
	control2.OnKeyDown(keyDown);

	container.OnKeyDown(keyDown);

	InvalidateRect(NULL, FALSE);


	return TRUE;
}

BOOL CALLBACK CD2DTest::OnChar(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	UINT uChar = (UINT)wParam;

	
	control.OnChar(uChar);
	control2.OnChar(uChar);

	container.OnChar(uChar);

	InvalidateRect(NULL, FALSE);


	return TRUE;
}

BOOL CALLBACK CD2DTest::OnMouseWheel(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	POINT pos = {};
	D2D1_POINT_2F point = D2D1::Point2F(ScaleX((float)GET_X_LPARAM_F(lParam), TRUE),
		ScaleY((float)GET_Y_LPARAM_F(lParam), TRUE));
	
	GetCursorPos(&pos);
	D2D1_POINT_2F cursor = D2D1::Point2F(ScaleX((float)pos.x, TRUE),
		ScaleY((float)pos.y, TRUE));


	//fwKeys = GET_KEYSTATE_WPARAM(wParam);
	short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

	container.OnMouseWheel((float)zDelta, point);

	InvalidateRect(NULL, FALSE);


	return TRUE;
}


VOID CD2DTestTest()
{
	CD2DTest* melody = new CD2DTest();
	melody->SetDelete();
	DWORD style = WS_OVERLAPPEDWINDOW;
	melody->CreateUI(0, L"赛博念珠", style, 0, 0, 1200, 1300, NULL);
	C32SetCenterPosDesktop(melody->m_Hwnd);
	melody->Show(SW_MAXIMIZE);


}

