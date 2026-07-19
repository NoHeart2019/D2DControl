#include "stdafx.h"
#include "D2DSelectUI.h"


CD2DSelectUI::CD2DSelectUI()
	: CD2DContainerUI()
	, m_droppedDown(false)
	, m_selectionCallback(nullptr)
	, m_pCallbackUserData(nullptr)
{
	// 显示区域绝对定位，用于绘制文本（也可以直接绘制，这里作为占位）
	m_displayArea.SetPositionType(CD2DPosition::Absolute);
	AddChild(&m_displayArea);

	// 下拉列表初始隐藏
	m_dropList.SetVisible(false);
	m_dropList.SetPositionType(CD2DPosition::Absolute);
	AddChild(&m_dropList);

	// 设置列表选择回调

	// 默认尺寸策略
	SetWidthPolicy(CD2DSizePolicy::FixedSize(150));
	SetHeightPolicy(CD2DSizePolicy::FixedSize(30));
}

CD2DSelectUI::~CD2DSelectUI()
{
}

// ---------- 选项管理 ----------
void CD2DSelectUI::AddItem(CD2DControlUI* pItem)
{
	if (!pItem) return;

	// 将项加入下拉列表
	m_dropList.AddItem(pItem);

	// 如果是第一项，默认选中
	if (m_dropList.GetItemCount() == 1)
		SetSelectedIndex(0);

	Layout();
}

void CD2DSelectUI::RemoveItem(int index)
{
	m_dropList.RemoveItem(index);
	Layout();
}

void CD2DSelectUI::ClearItems()
{
	m_dropList.ClearItems();
	Layout();
}

int CD2DSelectUI::GetItemCount() const
{
	return m_dropList.GetItemCount();
}

CD2DControlUI* CD2DSelectUI::GetItem(int index) const
{
	return m_dropList.GetItem(index);
}

// ---------- 选中项 ----------
void CD2DSelectUI::SetSelectedIndex(int index)
{
	m_dropList.SetSelectedIndex(index);
	UpdateDisplayText();
	NotifySelectionChanged();
}

int CD2DSelectUI::GetSelectedIndex() const
{
	return m_dropList.GetSelectedIndex();
}

CD2DControlUI* CD2DSelectUI::GetSelectedItem() const
{
	return m_dropList.GetSelectedItem();
}

CAtlString CD2DSelectUI::GetSelectedText() const
{
	CD2DControlUI* pItem = m_dropList.GetSelectedItem();
	if (pItem)
		return pItem->GetText();
	return L"";
}

// ---------- 外观设置 ----------
void CD2DSelectUI::SetItemHeight(float height)
{
	m_dropList.SetItemHeight(height);
	Layout();
}

void CD2DSelectUI::SetMaxDropDownItems(int count)
{
	m_dropList.SetMaxVisibleItems(count);
	Layout();
}

int CD2DSelectUI::GetMaxDropDownItems() const
{
	return m_dropList.GetMaxVisibleItems(); // 假设 CD2DListUI 添加了 GetMaxVisibleItems
}

// ---------- 样式管理 ----------
void CD2DSelectUI::SetSelectStyle(ED2DStatus state, const CD2DSelectStyle& style)
{
	m_selectStyles.SetAt(state, style);
}

const CD2DSelectStyle& CD2DSelectUI::GetSelectStyle(ED2DStatus state) const
{
	const auto* pPair = m_selectStyles.Lookup(state);
	if (pPair)
		return pPair->m_value;
	return m_defaultSelectStyle;
}

void CD2DSelectUI::SetDefaultSelectStyle(const CD2DSelectStyle& style)
{
	m_defaultSelectStyle = style;
	// 将样式应用到内部控件
	m_displayArea.SetDefaultStyle(style.trigger);
	m_dropList.SetDefaultStyle(style.dropDown);
}

const CD2DSelectStyle& CD2DSelectUI::GetDefaultSelectStyle() const
{
	return m_defaultSelectStyle;
}

// ---------- 回调 ----------
void CD2DSelectUI::SetOnSelectionChanged(SelectionCallback cb, void* pUserData)
{
	m_selectionCallback = cb;
	m_pCallbackUserData = pUserData;
}

bool CD2DSelectUI::IsHitTest(const D2D1_POINT_2F& point) const
{
	// 1. 检查触发器矩形（内容区域）
	if (CD2DControlUI::IsHitTest(point))
		return true;

	// 2. 如果下拉展开，再检查下拉列表矩形
	if (m_droppedDown && m_dropList.IsVisible())
	{
		D2D1_RECT_F dropRect = m_dropList.GetRectangle(); // 相对于本控件
		if (point.x >= dropRect.left && point.x <= dropRect.right &&
			point.y >= dropRect.top && point.y <= dropRect.bottom)
			return true;
	}

	return false;
}

// ---------- 布局 ----------
void CD2DSelectUI::Layout()
{
	D2D1_RECT_F content = GetContentRectangle();
	float w = content.right - content.left;
	float h = content.bottom - content.top;
	if (w <= 0 || h <= 0) return;

	const float arrowWidth = 20.0f;
	// 显示区域（左侧文本区）
	m_displayArea.SetPosition(content.left, content.top);
	m_displayArea.SetSize(w - arrowWidth, h);

	// 下拉列表位置和大小
	if (m_droppedDown)
	{
		m_dropList.SetPosition(content.left, content.bottom);
		m_dropList.SetWidth(w);
		int c = m_dropList.GetMaxVisibleItems();
		c = c == 0 ? m_dropList.GetItemCount(): c; 
		float h = m_dropList.CalculateContentHeight(c); // 保持设定
		//m_dropList.Layout(); // 计算自身高度
		//float listH = m_dropList.GetHeight();
		m_dropList.SetSize(w, h);
		m_dropList.SetVisible(true);
	}
	else
	{
		m_dropList.SetVisible(false);
	}
}

D2D1_SIZE_F CD2DSelectUI::MeasureContent(CD2DRender* pRender, float maxWidth,  float maxHeight) const
{
	// 返回理想宽度：显示文本宽度 + 箭头宽度
	const CD2DSelectStyle& style = GetEffectiveSelectStyle();
	float textW = 0, textH = 0;
	CAtlString text = GetSelectedText();
	if (!text.IsEmpty() && pRender)
	{
		CComPtr<IDWriteTextFormat> spFmt = pRender->CreateTextFormat(style.trigger.text);
		if (spFmt)
		{
			D2D1_SIZE_F sz = pRender->MeasureString(text, spFmt, maxWidth);
			textW = sz.width;
			textH = sz.height;
		}
	}
	const float arrowW = 20.0f;
	const float padding = 4.0f; // 简单的内边距估算
	float idealW = textW + arrowW + padding * 2;
	float idealH = max(textH, 20.0f) + padding * 2;
	return D2D1::SizeF(idealW, idealH);
}

// ---------- 绘制 ----------
void CD2DSelectUI::DrawControl(CD2DRender* pRender)
{
	if (!IsVisible() || !pRender) return;
	ID2D1RenderTarget* pRT = pRender->GetRenderTarget();
	if (!pRT) return;

	const CD2DSelectStyle& style = GetEffectiveSelectStyle();
	D2D1_RECT_F borderRect = GetBorderRectangle();
	D2D1_RECT_F contentRect = GetContentRectangle();

	// 绘制触发器背景和边框
	DrawBackground(style.trigger, borderRect, pRender);
	DrawBorder(style.trigger, borderRect, pRender);

	// 绘制显示区域（当前选中文本）
	// 注意：m_displayArea 是一个子控件，我们可以直接调用它的 DrawControl，
	// 但为了箭头，我们也可以单独绘制文本，避免嵌套控件干扰。这里我们直接绘制文本。
	CAtlString displayText = GetSelectedText();
	if (!displayText.IsEmpty())
	{
		CD2DTextStyle textStyle = style.trigger.text;
		CComPtr<IDWriteTextFormat> spFmt = pRender->CreateTextFormat(textStyle);
		if (spFmt)
		{
			// 文本区域：左侧留边距，右侧留给箭头
			const float arrowW = 20.0f;
			D2D1_RECT_F textRect = contentRect;
			textRect.right -= arrowW;
			textRect.left += 4.0f; // 左内边距
			textRect.top += 2.0f;
			textRect.bottom -= 2.0f;

			CComPtr<IDWriteTextLayout> spLayout;
			IDWriteFactory* pDW = pRender->GetWriteFactory();
			pDW->CreateTextLayout(
				displayText.GetString(), (UINT32)displayText.GetLength(),
				spFmt,
				textRect.right - textRect.left,
				textRect.bottom - textRect.top,
				&spLayout);
			if (spLayout)
			{
				// 垂直居中
				DWRITE_TEXT_METRICS metrics;
				spLayout->GetMetrics(&metrics);
				float y = textRect.top + (textRect.bottom - textRect.top - metrics.height) * 0.5f;

				CComPtr<ID2D1SolidColorBrush> spBrush;
				pRT->CreateSolidColorBrush(textStyle.color, &spBrush);
				if (spBrush)
				{
					pRT->DrawTextLayout(D2D1::Point2F(textRect.left, y), spLayout, spBrush);
				}
			}
		}
	}

	// 绘制下拉箭头（右侧）
	const float arrowAreaW = 20.0f;
	D2D1_RECT_F arrowRect = contentRect;
	arrowRect.left = arrowRect.right - arrowAreaW;
	DrawArrow(pRender, arrowRect, style.arrowColor);

	// 绘制下拉列表（如果展开）
	if (m_droppedDown)
		m_dropList.DrawControl(pRender);
}

void CD2DSelectUI::DrawArrow(CD2DRender* pRender, const D2D1_RECT_F& arrowRect, D2D1_COLOR_F color)
{
	ID2D1RenderTarget* pRT = pRender->GetRenderTarget();
	if (!pRT) return;

	float cx = arrowRect.left + (arrowRect.right - arrowRect.left) * 0.5f;
	float cy = arrowRect.top + (arrowRect.bottom - arrowRect.top) * 0.5f;
	float size = 4.0f;

	D2D1_POINT_2F pts[3] = {
		{ cx - size, cy - size * 0.5f },
		{ cx,        cy + size * 0.5f },
		{ cx + size, cy - size * 0.5f }
	};

	CComPtr<ID2D1PathGeometry> spGeom = pRender->CreatePolylineGeometry(pts, 3);
	if (!spGeom) return;

	CComPtr<ID2D1SolidColorBrush> spBrush;
	pRT->CreateSolidColorBrush(color, &spBrush);
	if (spBrush)
		pRT->DrawGeometry(spGeom, spBrush, 1.5f);
}
// ---------- 样式获取 ----------
const CD2DSelectStyle& CD2DSelectUI::GetEffectiveSelectStyle() const
{
	// 获取当前控件状态对应的样式（优先使用映射，否则回退到默认）
	ED2DStatus currentState = GetStatus().GetEffectiveState();
	const auto* pPair = m_selectStyles.Lookup(currentState);
	if (pPair) return pPair->m_value;

	// 回退到 Normal 状态
	pPair = m_selectStyles.Lookup(Normal);
	if (pPair) return pPair->m_value;

	return m_defaultSelectStyle;
}

// ---------- 鼠标事件 ----------
bool CD2DSelectUI::OnMouseDownL(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled()) return false;

	if (m_droppedDown)
	{
		// 检查点击是否在下拉列表区域内
		if (m_dropList.IsVisible())
		{
			
			if (m_dropList.IsHitTest(point))
			{
				// 转发给列表自己处理（内部会选中并触发回调）
				m_dropList.OnMouseDownL(point);
				HideDropDown();
				return true;
			}
		}
		// 点击其他地方，关闭下拉
		HideDropDown();
		return true;
	}
	else
	{
		// 下拉未显示，展开
		ShowDropDown();
		return true;
	}
}

bool CD2DSelectUI::OnMouseUpL(const D2D1_POINT_2F& point)
{
	// 不处理，留给基类
	return CD2DControlUI::OnMouseUpL(point);
}

bool CD2DSelectUI::OnMouseWheel(float delta, const D2D1_POINT_2F& point)
{
	if (m_droppedDown)
	{
		// 将滚轮事件传递给下拉列表
		if (m_dropList.IsHitTest(point))
		{
			return m_dropList.OnMouseWheel(delta, point);
		}
	}
	return false;
}

bool CD2DSelectUI::OnMouseLeave(const D2D1_POINT_2F& point)
{
	// 鼠标离开时隐藏下拉（简化处理）
	HideDropDown();
	return CD2DControlUI::OnMouseLeave(point);
}

// ---------- 下拉控制 ----------
void CD2DSelectUI::ShowDropDown()
{
	m_droppedDown = true;
	Layout();
}

void CD2DSelectUI::HideDropDown()
{
	m_droppedDown = false;
	Layout();
}

// ---------- 内部辅助 ----------
void CD2DSelectUI::UpdateDisplayText()
{
	// 显示区域文本已在绘制中直接使用 GetSelectedText()，无需额外存储
}

void CD2DSelectUI::NotifySelectionChanged()
{
	if (m_selectionCallback)
		m_selectionCallback(this, GetSelectedIndex(), m_pCallbackUserData);
}

// 静态回调：当列表选择变化时，同步到控件
void CD2DSelectUI::OnDropListSelection(CD2DListUI* pList, int newIndex, void* pUserData)
{
	CD2DSelectUI* pThis = static_cast<CD2DSelectUI*>(pUserData);
	if (pThis)
	{
		pThis->SetSelectedIndex(newIndex);
		pThis->HideDropDown();
	}
}