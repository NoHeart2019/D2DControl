#include "stdafx.h"
#include <d2d1.h>


#include "D2DControlPane.h"
#include "StyleManager.h"

// ========== 单击回调 ==========
void WINAPI OnEventClick(CD2DControlUI* pControl, void* pUserData)
{
	UNREFERENCED_PARAMETER(pUserData);
	OutputDebugStringA("Event: Click\n");
}

// ========== 双击回调 ==========
void WINAPI OnEventDoubleClick(CD2DControlUI* pControl, void* pUserData)
{
	UNREFERENCED_PARAMETER(pUserData);
	OutputDebugStringA("Event: DoubleClick\n");
	// 双击时输出用户自定义数据（这里演示 userData 用法）
	const char* msg = static_cast<const char*>(pUserData);
	if (msg) OutputDebugStringA(msg);
}

// ========== 悬停进入/离开 ==========
void WINAPI OnEventHoverEnter(CD2DControlUI* pControl, void* pUserData)
{
	UNREFERENCED_PARAMETER(pUserData);
	OutputDebugStringA("Event: HoverEnter\n");
}

void WINAPI OnEventHoverLeave(CD2DControlUI* pControl, void* pUserData)
{
	UNREFERENCED_PARAMETER(pUserData);
	OutputDebugStringA("Event: HoverLeave\n");
}

// ========== 拖拽回调 ==========
void WINAPI OnEventDragBegin(CD2DControlUI* pControl, void* pUserData)
{
	UNREFERENCED_PARAMETER(pUserData);
	OutputDebugStringA("Event: DragBegin\n");
}

void WINAPI OnEventDragMove(CD2DControlUI* pControl, void* pUserData)
{
	UNREFERENCED_PARAMETER(pUserData);
	OutputDebugStringA("Event: DragMove\n");
}

void WINAPI OnEventDragEnd(CD2DControlUI* pControl, void* pUserData)
{
	UNREFERENCED_PARAMETER(pUserData);
	OutputDebugStringA("Event: DragEnd\n");
}

// ========== 3. 绑定事件回调 ==========

#pragma comment(lib, "winmm.lib")

DWORD CD2DPane::Register()
{
	return RegisterClassEx32(L"CD2DPane");
}

CD2DPane::CD2DPane()
{
	
	//root.AddControl(&control2);

}

CD2DPane::~CD2DPane()
{
	PostQuitMessage(0);
}

BOOL CD2DPane::CreateUI(DWORD ExStyle, LPCWSTR lpwszWindowName,DWORD Style, int xPos, int yPos, int Cx, int Cy, HWND hwnd)
{

	Create(ExStyle, L"CD2DPane", lpwszWindowName, Style, xPos, yPos, Cx, Cy, hwnd);

	return (m_Hwnd != NULL);
}

// 创建一个外观类似 HTML 默认复选框的 CD2DCheckbox


VOID CD2DPane::InitializeControls()
{
	RegisterPaneStyles();
	InitializeIcon();
	InitializeButton();
	InitializeList();
	InitializeExplorer();
	InitializeTextArea();
	InitializeTextEdit();
}

VOID CD2DPane::SetControlsLayout()
{
	D2D1_RECT_F rectangle = GetRectangle();
	D2D1_SIZE_F size = GetSize();
	D2D1_POINT_2F center = GetCenter();
	root.SetSize(size.width*0.8f, size.height*0.8f);
	root.SetCenter(center.x, center.y);

}

BOOL CALLBACK CD2DPane::OnCreate(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	InitializeRenderTarget(m_Hwnd);	
	InitializeControls();
	SetControlsLayout();
	root.SetOverflow(CD2DContainerUI::Auto);
	

	return TRUE;
}

BOOL CALLBACK CD2DPane::OnPaint(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	PAINTSTRUCT ps;
	BeginPaint(m_Hwnd, &ps);
	BeginDraw();


	Clear(D2D1::ColorF(D2D1::ColorF::White));
	
	//DrawControl(this);
	//control.DrawControl(this);
	//control2.DrawControl(this);
	//list.DrawControl(this);
	root.DrawControl(this);
	//m_box.DrawControl(this);
	EndDraw();

	EndPaint(m_Hwnd, &ps);

	return TRUE;
}

BOOL CALLBACK CD2DPane::OnSize(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	float width = GET_X_LPARAM_F(lParam);
	float height = GET_Y_LPARAM_F(lParam);
	CD2DRender::Resize(width, height);
	
	SetRectangle(D2D1::RectF(0.0f, 0.0f, ScaleX(width, TRUE), ScaleY(height, TRUE)));
	SetControlsLayout();
	return TRUE;
}

BOOL CALLBACK CD2DPane::OnLButtonDown(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	D2D1_POINT_2F point = D2D1::Point2F(ScaleX((float)GET_X_LPARAM_F(lParam), TRUE),
										ScaleY((float)GET_Y_LPARAM_F(lParam), TRUE));
	
	control.OnMouseDownL(point);

	root.OnMouseDownL(point);

	InvalidateRect(NULL, FALSE);


	return TRUE;
}

BOOL CALLBACK CD2DPane::OnLButtonUp(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	D2D1_POINT_2F point = D2D1::Point2F((float)ScaleX(GET_X_LPARAM_F(lParam), TRUE),
		(float)ScaleY(GET_Y_LPARAM_F(lParam), TRUE));
	
	control.OnMouseUpL(point);

	root.OnMouseUpL(point);
	
	InvalidateRect(NULL, FALSE);

	return TRUE;
}

BOOL CALLBACK CD2DPane::OnLButtonDown2(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	D2D1_POINT_2F point = D2D1::Point2F(ScaleX((float)GET_X_LPARAM_F(lParam), TRUE),
		ScaleY((float)GET_Y_LPARAM_F(lParam), TRUE));


	control.OnMouseDownL2(point);

	root.OnMouseDownL2(point);

	InvalidateRect(NULL, FALSE);


	return TRUE;
}

BOOL CALLBACK CD2DPane::OnLButtonUp2(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	D2D1_POINT_2F point = D2D1::Point2F((float)ScaleX(GET_X_LPARAM_F(lParam), TRUE),
		(float)ScaleY(GET_Y_LPARAM_F(lParam), TRUE));
	
	control.OnMouseUpL2(point);

	root.OnMouseUpL2(point);
	

	InvalidateRect(NULL, FALSE);

	return TRUE;
}

BOOL CALLBACK CD2DPane::OnMouseMove(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	D2D1_POINT_2F point = D2D1::Point2F((float)ScaleX(GET_X_LPARAM_F(lParam), TRUE),
		(float)ScaleY(GET_Y_LPARAM_F(lParam), TRUE));

	control.OnMouseMove(point);

	root.OnMouseMove(point);

	
	InvalidateRect(NULL, FALSE);

	return TRUE;
}

BOOL CALLBACK CD2DPane::OnKeyDown(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	UINT keyDown = (UINT)wParam;
	
	control.OnKeyDown(keyDown);

	root.OnKeyDown(keyDown);

	InvalidateRect(NULL, FALSE);


	return TRUE;
}

BOOL CALLBACK CD2DPane::OnChar(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	UINT uChar = (UINT)wParam;

	
	control.OnChar(uChar);

	root.OnChar(uChar);

	InvalidateRect(NULL, FALSE);


	return TRUE;
}

BOOL CALLBACK CD2DPane::OnMouseWheel(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	POINT pos = {};
	D2D1_POINT_2F point = D2D1::Point2F(ScaleX((float)GET_X_LPARAM_F(lParam), TRUE),
		ScaleY((float)GET_Y_LPARAM_F(lParam), TRUE));
	
	GetCursorPos(&pos);
	D2D1_POINT_2F cursor = D2D1::Point2F(ScaleX((float)pos.x, TRUE),
		ScaleY((float)pos.y, TRUE));


	//fwKeys = GET_KEYSTATE_WPARAM(wParam);
	short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

	root.OnMouseWheel((float)zDelta, point);

	InvalidateRect(NULL, FALSE);


	return TRUE;
}

void CD2DPane::InitializeIcon()
{
	// ===== 2. 创建容器（动态分配） =====
	auto* hbox = new CD2DHBoxContainerUI();
	root.AddChild(hbox);
	hbox->SetSpacing(12.0f);
	hbox->SetWidthPolicy(CD2DSizePolicy::FillSize());

	StyleManager::SetControlStyle(hbox, L"hbox-container");  // 应用容器样式

	// ===== 3. 创建四个图标（动态分配），各自应用尺寸策略和类样式 =====
	auto* iconFile = new CD2DIcon();
	iconFile->SetWidthPolicy(CD2DSizePolicy::AutoSize());
	iconFile->SetHeightPolicy(CD2DSizePolicy::AutoSize());
	iconFile->LoadFromFile(L"res/StickyNotes.png");
	iconFile->SetScaleMode(CD2DIcon::Uniform);
	StyleManager::SetControlStyle(iconFile, L"icon-uniform");
	hbox->AddChild(iconFile);

	auto* iconRes = new CD2DIcon();
	iconRes->SetWidthPolicy(CD2DSizePolicy::AutoSize());
	iconRes->SetHeightPolicy(CD2DSizePolicy::AutoSize());
	//iconRes->LoadFromResource(GetModuleHandle(L""), MAKEINTRESOURCE(idi_s), L"PNG");
	iconRes->LoadFromFile(L"res/StickyNotes.png");
	iconRes->SetScaleMode(CD2DIcon::Stretch);
	StyleManager::SetControlStyle(iconRes, L"icon-stretch");
	hbox->AddChild(iconRes);

	auto* iconFill = new CD2DIcon();
	iconFill->SetWidthPolicy(CD2DSizePolicy::FixedSize(60.0f));
	iconFill->SetHeightPolicy(CD2DSizePolicy::FixedSize(60.0f));
	iconFill->LoadFromFile(L"res/ShanCaiTongZi.png");
	iconFill->SetScaleMode(CD2DIcon::UniformToFill);
	StyleManager::SetControlStyle(iconFill, L"icon-fill");
	hbox->AddChild(iconFill);

	auto* iconNone = new CD2DIcon();
	iconNone->SetWidthPolicy(CD2DSizePolicy::AutoSize());
	iconNone->SetHeightPolicy(CD2DSizePolicy::AutoSize());
	iconNone->LoadFromFile(L"res/StickyNotes.png");
	iconNone->SetScaleMode(CD2DIcon::None);
	StyleManager::SetControlStyle(iconNone, L"icon-none");
	hbox->AddChild(iconNone);

	
}

void CD2DPane::InitializeButton()
{
	
	// ========== 2. 创建水平布局容器 ==========
	auto* hbox = new CD2DHBoxContainerUI();
	hbox->SetWidthPolicy(CD2DSizePolicy::FillSize());            // 固定容器高度
	StyleManager::SetControlStyle(hbox, L"toolbar");
	root.AddChild(hbox);


	// ========== 3. 创建按钮并应用类样式 ==========

	// 确定按钮 ( .btn )
	auto* btnOK = new CD2DControlUI();
	btnOK->SetText(L"确定");
	btnOK->SetWidthPolicy(CD2DSizePolicy::AutoSize());   // 宽度随文字自适应
	btnOK->SetHeightPolicy(CD2DSizePolicy::FixedSize(36)); // 固定高度
	StyleManager::SetControlStyle(btnOK, L"btn");
	hbox->AddChild(btnOK);

	// 取消按钮 ( .btn .btn-cancel ) —— 组合类
	auto* btnCancel = new CD2DControlUI();
	btnCancel->SetText(L"取消");
	btnCancel->SetWidthPolicy(CD2DSizePolicy::AutoSize());
	btnCancel->SetHeightPolicy(CD2DSizePolicy::FixedSize(36));
	StyleManager::SetControlStyle(btnCancel, L"btn btn-cancel"); // 同时拥有 btn 和 btn-cancel 样式
	hbox->AddChild(btnCancel);

	// 删除按钮 ( .btn .btn-delete )
	auto* btnDelete = new CD2DControlUI();
	btnDelete->SetText(L"删除");
	btnDelete->SetWidthPolicy(CD2DSizePolicy::AutoSize());
	btnDelete->SetHeightPolicy(CD2DSizePolicy::FixedSize(36));
	StyleManager::SetControlStyle(btnDelete, L"btn btn-delete");
	hbox->AddChild(btnDelete);

	hbox->AddChild(&edit);
}

void CD2DPane::InitializeExplorer()
{
	// ========== 3. 创建 CD2DExploreUI 并设置 Wrap 模式 ==========
	
	StyleManager::SetControlStyle(&explorer, L"explore-box");
	
	explorer.SetViewMode(CD2DExploreUI::Wrap);          // Wrap 自动换行
	explorer.SetWrapAutoFit(true);                      // 自动填满行
	explorer.SetItemWidth(100.0f);                      // 期望项宽度（自动列数）
	explorer.SetHorizontalSpacing(10.0f);
	explorer.SetVerticalSpacing(10.0f);
	explorer.SetWidthPolicy(CD2DSizePolicy::FixedSize(400.0f));            // 固定容器高度
	explorer.SetHeightPolicy(CD2DSizePolicy::FixedSize(240.0f));
	// 启用拖拽排序（允许交换位置）
	explorer.EnableDragging(true);

	// ========== 4. 添加若干子控件（项） ==========
	const int itemCount = 12;
	for (int i = 0; i < itemCount; ++i)
	{
		auto* pItem = new CD2DControlUI();
		CAtlString text;
		text.Format(L"item : %d ", i);
		pItem->SetText(text);
		pItem->SetWidthPolicy(CD2DSizePolicy::AutoSize());
		pItem->SetHeightPolicy(CD2DSizePolicy::FixedSize(60));   // 固定高度 60px
		StyleManager::SetControlStyle(pItem, L"explore-item");
		explorer.AddItem(pItem);

	}

}

void CD2DPane::InitializeList()
{
	auto* hbox = new CD2DHBoxContainerUI();
	StyleManager::SetControlStyle(hbox, L"toolbar");
	hbox->SetWidthPolicy(CD2DSizePolicy::ExpandWeight(1.0f));            // 固定容器高度
	root.AddChild(hbox);
	CD2DListUI* list = new CD2DListUI;
	list->SetWidthPolicy(CD2DSizePolicy::FixedSize(200.0f));            // 固定容器高度
	list->SetHeightPolicy(CD2DSizePolicy::FixedSize(240.0f));
	StyleManager::SetControlStyle(list, L"list-box");             // 应用列表自身样式
	hbox->AddChild(list);
	hbox->AddChild(&explorer);
	hbox->AddChild(&textArea);
	// 设置列表属性
	list->SetItemHeight(30.0f);
	list->SetItemSpacing(4.0f);
	list->SetMaxVisibleItems(5);  // 超过5项出现滚动条

	LPCWSTR itemTexts[] = {
		L"语文", L"数学", L"英语",
		L"物理", L"化学", L"生物",
		L"历史", L"政治", L"体育",
	};

	for (size_t i = 0; i < ARRAYSIZE(itemTexts); ++i)
	{
		CD2DControlUI* pItem = new CD2DControlUI();
		pItem->SetText(itemTexts[i]);
		// 应用类样式（Normal/Hover/Checked 都会自动匹配）
		StyleManager::SetControlStyle(pItem, L"list-item");
		list->AddItem(pItem);
	}
}

void CD2DPane::InitializeTextArea()
{
	StyleManager::SetControlStyle(&textArea, L"text-area");

	// 可选：设置尺寸策略
	textArea.SetWidthPolicy(CD2DSizePolicy::ExpandWeight(1.0f));            // 固定容器高度
	textArea.SetHeightPolicy(CD2DSizePolicy::FixedSize(240.0f));

	// 设置文本内容（包含多行和长句子以测试换行与滚动）
	textArea.SetText(L"这是一段多行文本示例。你可以在这里输入很长的内容，它会自动换行。\n"
		L"第二行：Direct2D 文本布局支持多种对齐方式。\n"
		L"第三行：当你需要显示大量文字时，可以开启滚动条。\n"
		L"第四行：让我们继续添加一些文字来填满这个区域……\n"
		L"第五行：现在文本已经足够多，应该可以看到垂直滚动条了。");
}

void CD2DPane::InitializeTextEdit()
{
	StyleManager::SetControlStyle(&edit, L"edit");

	// 设置为多行模式，并开启自动换行
	edit.SetMode(CD2DTextEdit::MultiLine);
	edit.SetWordWrap(true);
	edit.SetRender(this);
	// 设置占位符文本
	edit.SetPlaceholder(L"请输入内容...");

	// 设置最大字符数（限制20个字符）
	edit.SetMaxChars(20);

	// 设置初始文本（少于20字符，观察占位符消失）
	edit.SetText(L"Hello, World!");

	// 尺寸策略：宽度固定300，高度自适应（AutoSize 可让编辑框根据内容自动扩展）
	edit.SetWidthPolicy(CD2DSizePolicy::FixedSize(120));
	edit.SetHeightPolicy(CD2DSizePolicy::FixedSize(35.0f));

}

void RegisterPaneStyles()
{
	{
		// ===== 1. 注册类样式 =====
		ClassStyleSet iconUniform;
		iconUniform.defaultStyle.background.type = CD2DBackground::Solid;
		iconUniform.defaultStyle.background.solid.color = D2D1::ColorF(0.9f, 0.9f, 0.9f);
		// 可以添加状态样式，如 Hover 高亮
		StyleManager::Get().RegisterClassStyle(L"icon-uniform", iconUniform);

		ClassStyleSet iconStretch;
		iconStretch.defaultStyle.background.type = CD2DBackground::Solid;
		iconStretch.defaultStyle.background.solid.color = D2D1::ColorF(0.8f, 0.9f, 1.0f);
		StyleManager::Get().RegisterClassStyle(L"icon-stretch", iconStretch);

		ClassStyleSet iconFill;
		iconFill.defaultStyle.background.type = CD2DBackground::Solid;
		iconFill.defaultStyle.background.solid.color = D2D1::ColorF(0.9f, 0.9f, 0.8f);
		iconFill.defaultStyle.border = CD2DBorder(CD2DBorderEdge(1.0f, Solid, D2D1::ColorF(0.5f,0.5f,0.5f))); 
		iconFill.defaultStyle.radius = CD2DRadius(0.5f);
		StyleManager::Get().RegisterClassStyle(L"icon-fill", iconFill);

		ClassStyleSet iconNone;
		iconNone.defaultStyle.background.type = CD2DBackground::None;  // 透明
		StyleManager::Get().RegisterClassStyle(L"icon-none", iconNone);

		// 容器样式
		ClassStyleSet hboxStyle;
		hboxStyle.defaultStyle.background.type = CD2DBackground::Solid;
		hboxStyle.defaultStyle.background.solid.color = D2D1::ColorF(0.95f, 0.95f, 0.95f);
		hboxStyle.defaultStyle.radius = CD2DRadius(8.0f);
		hboxStyle.defaultStyle.padding = CD2DPadding(16.0f, 6.0f, 16.0f, 6.0f);            // 内边距

		StyleManager::Get().RegisterClassStyle(L"hbox-container", hboxStyle);
	}

	// ========== 1. 注册类样式 ==========
	// 基础按钮样式 ( .btn )
	{
		ClassStyleSet btnBase;
		btnBase.defaultStyle.background.type = CD2DBackground::Solid;
		btnBase.defaultStyle.background.solid.color = D2D1::ColorF(0.2f, 0.5f, 1.0f);  // 蓝色
		btnBase.defaultStyle.text.color = D2D1::ColorF(1.0f, 1.0f, 1.0f);                // 白色文字
		btnBase.defaultStyle.text.fontSize = 14.0f;
		btnBase.defaultStyle.text.horizontalAlign = DWRITE_TEXT_ALIGNMENT_CENTER;
		btnBase.defaultStyle.text.verticalAlign   = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
		btnBase.defaultStyle.radius = CD2DRadius(4.0f);                                    // 圆角 4px
		btnBase.defaultStyle.padding = CD2DPadding(16.0f, 6.0f, 16.0f, 6.0f);            // 内边距

		// 悬停状态 ( .btn:hover )
		CD2DStyle btnHover;
		btnHover.background.type = CD2DBackground::Solid;
		btnHover.background.solid.color = D2D1::ColorF(0.3f, 0.6f, 1.0f);
		btnBase.stateStyles[ED2DStatus::Hover] = btnHover;

		// 按下状态 ( .btn:active )
		CD2DStyle btnPressed;
		btnPressed.background.type = CD2DBackground::Solid;
		btnPressed.background.solid.color = D2D1::ColorF(0.1f, 0.4f, 0.9f);
		btnBase.stateStyles[ED2DStatus::Pressed] = btnPressed;

		StyleManager::Get().RegisterClassStyle(L"btn", btnBase);

		// 取消按钮样式 ( .btn-cancel ) ，继承 .btn 的部分样式，只需覆写颜色
		ClassStyleSet btnCancel;
		btnCancel.defaultStyle.background.solid.color = D2D1::ColorF(0.9f, 0.3f, 0.3f); // 红色
		CD2DStyle cancelHover;
		cancelHover.background.solid.color = D2D1::ColorF(1.0f, 0.4f, 0.4f);
		btnCancel.stateStyles[ED2DStatus::Hover] = cancelHover;
		CD2DStyle cancelPressed;
		cancelPressed.background.type = CD2DBackground::Solid;
		cancelPressed.background.solid.color = D2D1::ColorF(0.8f, 0.2f, 0.2f);
		btnCancel.stateStyles[ED2DStatus::Pressed] = cancelPressed;
		StyleManager::Get().RegisterClassStyle(L"btn-cancel", btnCancel);

		// 删除按钮样式 ( .btn-delete ) ，橙色系
		ClassStyleSet btnDelete;
		btnDelete.defaultStyle.background.solid.color = D2D1::ColorF(1.0f, 0.6f, 0.1f);
		CD2DStyle deleteHover;
		deleteHover.background.type = CD2DBackground::Solid;
		deleteHover.background.solid.color = D2D1::ColorF(1.0f, 0.0f, 0.0f);
		deleteHover.transform.SetTranslate(0.0f, -2.0f);
		btnDelete.stateStyles[ED2DStatus::Hover] = deleteHover;
		CD2DStyle deletePressed;
		deletePressed.background.type = CD2DBackground::Solid;
		deletePressed.background.solid.color = D2D1::ColorF(0.9f, 0.5f, 0.0f);
		btnDelete.stateStyles[ED2DStatus::Pressed] = deletePressed;
		StyleManager::Get().RegisterClassStyle(L"btn-delete", btnDelete);
		// 容器自身样式（可选）
		ClassStyleSet toolbarStyle;
		toolbarStyle.defaultStyle.background.type = CD2DBackground::Solid;
		toolbarStyle.defaultStyle.background.solid.color = D2D1::ColorF(0.95f, 0.95f, 0.95f);
		toolbarStyle.defaultStyle.radius = CD2DRadius(8.0f);
		toolbarStyle.defaultStyle.padding = CD2DPadding(10, 10, 10, 10);
		StyleManager::Get().RegisterClassStyle(L"toolbar", toolbarStyle);
		}


	{

		// ========== 1. 注册列表项类样式 ==========
		ClassStyleSet itemStyle;
		itemStyle.defaultStyle.background.type = CD2DBackground::Solid;
		itemStyle.defaultStyle.background.solid.color = D2D1::ColorF(1.0f, 1.0f, 1.0f);   // 白底
		itemStyle.defaultStyle.text.color = D2D1::ColorF(0.1f, 0.1f, 0.1f);                 // 黑字
		itemStyle.defaultStyle.text.fontSize = 14.0f;
		itemStyle.defaultStyle.padding = CD2DPadding(8.0f, 4.0f, 8.0f, 4.0f);

		// 悬停状态 (Hover)
		CD2DStyle hoverStyle;
		hoverStyle.background.type = CD2DBackground::Solid;
		hoverStyle.background.solid.color = D2D1::ColorF(0.88f, 0.94f, 1.0f);             // 浅蓝
		itemStyle.stateStyles[ED2DStatus::Hover] = hoverStyle;

		// 选中状态 (Checked)
		CD2DStyle checkedStyle;
		checkedStyle.background.type = CD2DBackground::Solid;
		checkedStyle.background.solid.color = D2D1::ColorF(0.2f, 0.5f, 1.0f);             // 蓝底
		checkedStyle.text.color = D2D1::ColorF(1.0f, 1.0f, 1.0f);                           // 白字
		itemStyle.stateStyles[ED2DStatus::Checked] = checkedStyle;

		StyleManager::Get().RegisterClassStyle(L"list-item", itemStyle);

		// ========== 2. 注册列表自身类样式（可选） ==========
		ClassStyleSet listStyleSet;
		listStyleSet.defaultStyle.background.type = CD2DBackground::Solid;
		listStyleSet.defaultStyle.background.solid.color = D2D1::ColorF(0.96f, 0.96f, 0.96f);
		listStyleSet.defaultStyle.border = CD2DBorder(CD2DBorderEdge(1.0f, Solid, D2D1::ColorF(0.7f, 0.7f, 0.7f)));
		listStyleSet.defaultStyle.radius = CD2DRadius(6.0f);
		listStyleSet.defaultStyle.padding = CD2DPadding(4.0f, 4.0f, 4.0f, 4.0f);
		StyleManager::Get().RegisterClassStyle(L"list-box", listStyleSet);

	}
	
	{
		// ========== 1. 注册项样式 ( .explore-item ) ==========
		ClassStyleSet itemClass;
		itemClass.defaultStyle.background.type = CD2DBackground::Solid;
		itemClass.defaultStyle.background.solid.color = D2D1::ColorF(0.95f, 0.95f, 0.95f);
		itemClass.defaultStyle.border = CD2DBorder(CD2DBorderEdge(1.5f, Solid, D2D1::ColorF(0.6f, 0.6f, 0.6f)));
		itemClass.defaultStyle.radius = CD2DRadius(6.0f);
		itemClass.defaultStyle.text.color = D2D1::ColorF(0.2f, 0.2f, 0.2f);
		itemClass.defaultStyle.text.fontSize = 14.0f;
		itemClass.defaultStyle.text.horizontalAlign = DWRITE_TEXT_ALIGNMENT_CENTER;
		itemClass.defaultStyle.text.verticalAlign   = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
		itemClass.defaultStyle.padding =  CD2DPadding(8.0f, 4.0f, 8.0f, 4.0f);

		// 悬停状态
		CD2DStyle hoverItem;
		hoverItem.background.type = CD2DBackground::Solid;
		hoverItem.background.solid.color = D2D1::ColorF(0.8f, 0.9f, 1.0f);
		hoverItem.border = CD2DBorder(CD2DBorderEdge(1.5f, Solid, D2D1::ColorF(0.2f, 0.5f, 1.0f)));
		itemClass.stateStyles[ED2DStatus::Hover] = hoverItem;



		// 选中状态
		CD2DStyle checkedItem;
		checkedItem.background.type = CD2DBackground::Solid;
		checkedItem.background.solid.color = D2D1::ColorF(0.3f, 0.6f, 1.0f);
		checkedItem.border = CD2DBorder(CD2DBorderEdge(1.5f, Solid, D2D1::ColorF(0.2f, 0.5f, 1.0f)));
		checkedItem.text.color = D2D1::ColorF(1.0f, 1.0f, 1.0f);
		itemClass.stateStyles[ED2DStatus::Checked] = checkedItem;

		// 在 itemClass 的 stateStyles 中追加：
		// 拖拽中状态 (Dragging)：半透明，弱化显示
		CD2DStyle draggingStyle;
		draggingStyle.background.solid.color = D2D1::ColorF(0.85f, 0.85f, 0.85f, 0.5f);  // 灰色半透明
		draggingStyle.border = CD2DBorder(CD2DBorderEdge(1.5f, Dashed, D2D1::ColorF(0.5f, 0.5f, 0.5f)));
		itemClass.stateStyles[ED2DStatus::Dragging] = draggingStyle;

		// 拖拽悬停状态 (DragOver)：高亮蓝色边框
		CD2DStyle dragOverStyle;
		dragOverStyle.background.solid.color = D2D1::ColorF(0.9f, 0.95f, 1.0f);           // 极浅蓝背景
		dragOverStyle.border = CD2DBorder(CD2DBorderEdge(2.0f, Solid, D2D1::ColorF(0.2f, 0.5f, 1.0f)));  // 蓝色加粗边框
		itemClass.stateStyles[ED2DStatus::DragOver] = dragOverStyle;


		StyleManager::Get().RegisterClassStyle(L"explore-item", itemClass);

		// ========== 2. 注册容器自身样式 ( .explore-box ) ==========
		ClassStyleSet boxClass;
		boxClass.defaultStyle.background.type = CD2DBackground::Solid;
		boxClass.defaultStyle.background.solid.color = D2D1::ColorF(0.98f, 0.98f, 0.98f);
		boxClass.defaultStyle.border = CD2DBorder(CD2DBorderEdge(1.0f, Solid, D2D1::ColorF(0.8f, 0.8f, 0.8f)));
		boxClass.defaultStyle.radius = CD2DRadius(8.0f);
		boxClass.defaultStyle.padding =  CD2DPadding(8.0f, 4.0f, 8.0f, 4.0f);;
		StyleManager::Get().RegisterClassStyle(L"explore-box", boxClass);


	}

	{

		// ========== 1. 注册文本区域类样式 ==========
		// 普通状态样式
		ClassStyleSet textAreaStyle;
		textAreaStyle.defaultStyle.background.type = CD2DBackground::Solid;
		textAreaStyle.defaultStyle.background.solid.color = D2D1::ColorF(1.0f, 1.0f, 1.0f);   // 白色背景
		textAreaStyle.defaultStyle.border = CD2DBorder(CD2DBorderEdge(1.5f, Solid, D2D1::ColorF(0.6f, 0.6f, 0.6f)));
		textAreaStyle.defaultStyle.radius = CD2DRadius(6.0f);
		textAreaStyle.defaultStyle.padding = CD2DPadding(10.0f, 8.0f, 10.0f, 8.0f);
		textAreaStyle.defaultStyle.text.color = D2D1::ColorF(0.2f, 0.2f, 0.2f);                 // 深灰色文字
		textAreaStyle.defaultStyle.text.fontSize = 26.0f;
		textAreaStyle.defaultStyle.text.wordWrap = true;                                         // 自动换行

		// 焦点状态样式
		CD2DStyle focusedStyle;
		focusedStyle.border = CD2DBorder(CD2DBorderEdge(2.0f, Solid, D2D1::ColorF(0.2f, 0.5f, 1.0f)));
		textAreaStyle.stateStyles[ED2DStatus::Focused] = focusedStyle;

		StyleManager::Get().RegisterClassStyle(L"text-area", textAreaStyle);
	}

	{
		// ========== 1. 注册编辑框类样式 ==========
		ClassStyleSet editClass;
		editClass.defaultStyle.background.type = CD2DBackground::Solid;
		editClass.defaultStyle.background.solid.color = D2D1::ColorF(1.0f, 1.0f, 1.0f);   // 白色背景
		editClass.defaultStyle.border = CD2DBorder(CD2DBorderEdge(1.5f, Solid, D2D1::ColorF(0.6f, 0.6f, 0.6f)));
		editClass.defaultStyle.radius = CD2DRadius(4.0f);
		editClass.defaultStyle.padding = CD2DPadding(8.0f, 4.0f, 8.0f, 4.0f);
		editClass.defaultStyle.text.color = D2D1::ColorF(0.1f, 0.1f, 0.1f);
		editClass.defaultStyle.text.fontSize = 14.0f;

		// 焦点状态
		CD2DStyle focusedStyle;
		focusedStyle.border = CD2DBorder(CD2DBorderEdge(2.0f, Solid, D2D1::ColorF(0.2f, 0.5f, 1.0f)));
		editClass.stateStyles[ED2DStatus::Focused] = focusedStyle;

		// 只读状态
		CD2DStyle readOnlyStyle;
		readOnlyStyle.background.solid.color = D2D1::ColorF(0.95f, 0.95f, 0.95f);
		readOnlyStyle.text.color = D2D1::ColorF(0.5f, 0.5f, 0.5f);
		editClass.stateStyles[ED2DStatus::Disabled] = readOnlyStyle;   // 借用 Disabled 状态表示只读样式（也可新加状态）

		// 注册类名 "edit"
		StyleManager::Get().RegisterClassStyle(L"edit", editClass);

	}

}


VOID CD2DPaneTest()
{
	CD2DPane* melody = new CD2DPane();
	melody->SetDelete();
	DWORD style = WS_OVERLAPPEDWINDOW;
	melody->CreateUI(0, L"赛博念珠", style, 0, 0, 1200, 1300, NULL);
	C32SetCenterPosDesktop(melody->m_Hwnd);
	melody->Show(SW_MAXIMIZE);
}

