#include "stdafx.h"
#include "D2DControlUI.h"
#include "D2DRender.h"
#include "D2DScrollBarUI.h"
#include "D2DItemUI.h"
#include "D2DExplorerUI.h"

	//-----------------------------------------------------------------------------
	// 构造与析构
	//-----------------------------------------------------------------------------
CD2DExplorerUI::CD2DExplorerUI()
	: m_explorerMode(FixedSize)
	, m_itemSize(D2D1::SizeF(100.0f, 100.0f))
	, m_itemSpacing(D2D1::SizeF(0.0f, 0.0f))
	, m_minItemWidth(10.0f)
	, m_calculatedColumns(1)
	, m_calculatedItemWidth(100.0f)
	, m_calculatedItemHeight(100.0f)
	, m_calculatedHorzSpacing(10.0f)
	, m_calculatedVertSpacing(10.0f)
	, m_nSelectedIndex(-1)
	, m_nHoverIndex(-1)
	, m_fScrollPosition(0.0f)
	, m_IsShowScrollBar(false)
	, m_fScrollBarWidth(20.0f)
	, m_clickCallback(nullptr)
{
	// 初始化垂直滚动条
	m_scrollV.SetRange(0.0f, 100.0f);
	m_scrollV.SetPageSize(20.0f);
	m_scrollV.SetStepSize(1.0f);
	m_scrollV.SetThumbSize(20.0f);
	SetPadding(0.0f, 0.0f, 20.0f, 0.0f);
	m_scrollV.SetScrollCallback(OnScrollCallback, this);
	//SetExplorerMode(AutoExpand);
}

CD2DExplorerUI::~CD2DExplorerUI()
{
	ClearItems();
}

//-----------------------------------------------------------------------------
// 滚动回调
//-----------------------------------------------------------------------------
void CD2DExplorerUI::OnScrollCallback(float position, void* userData)
{
	CD2DExplorerUI* pExplorerUI = static_cast<CD2DExplorerUI*>(userData);
	if (pExplorerUI)
	{
		pExplorerUI->m_fScrollPosition = position;
		pExplorerUI->m_scrollV.m_position = position;
	}
}

//-----------------------------------------------------------------------------
// 物品管理
//-----------------------------------------------------------------------------
void CD2DExplorerUI::AddItem(CD2DExplorerItemUI* pItem)
{
	if (pItem)
	{
		m_items.Add(CAutoPtr<CD2DExplorerItemUI> (pItem));
		float fMaxScrollPos = GetMaxScrollPosition();
		m_scrollV.SetRange(0.0f, fMaxScrollPos);
	}
}

void CD2DExplorerUI::RemoveItem(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_items.GetCount())
	{
		delete m_items[nIndex];
		m_items.RemoveAt(nIndex);

		if (m_nSelectedIndex == nIndex)
			m_nSelectedIndex = -1;
		else if (m_nSelectedIndex > nIndex)
			m_nSelectedIndex--;

		if (m_nHoverIndex == nIndex)
			m_nHoverIndex = -1;
		else if (m_nHoverIndex > nIndex)
			m_nHoverIndex--;

		float fMaxScrollPos = GetMaxScrollPosition();
		m_scrollV.SetRange(0.0f, fMaxScrollPos);
		if (m_fScrollPosition > fMaxScrollPos)
			SetScrollPosition(fMaxScrollPos);
	}
}

void CD2DExplorerUI::ClearItems()
{
	m_items.RemoveAll();

	m_nSelectedIndex = -1;
	m_nHoverIndex = -1;
	m_fScrollPosition = 0.0f;
	m_scrollV.SetRange(0.0f, 0.0f);
	m_scrollV.SetPosition(0.0f);
}

int CD2DExplorerUI::GetItemCount() const
{
	return (int)m_items.GetCount();
}

CD2DExplorerItemUI* CD2DExplorerUI::GetItem(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_items.GetCount())
		return m_items[nIndex];
	return nullptr;
}

//-----------------------------------------------------------------------------
// 选择与悬停
//-----------------------------------------------------------------------------
void CD2DExplorerUI::SelectItem(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_items.GetCount())
		m_nSelectedIndex = nIndex;
}

void CD2DExplorerUI::UnselectItem()
{
	m_nSelectedIndex = -1;
}

int CD2DExplorerUI::GetSelectedIndex() const
{
	return m_nSelectedIndex;
}

int CD2DExplorerUI::GetHoverIndex() const
{
	return m_nHoverIndex;
}

//-----------------------------------------------------------------------------
// 布局相关接口
//-----------------------------------------------------------------------------
void CD2DExplorerUI::SetItemSize(float fWidth, float fHeight)
{
	m_itemSize.width = fWidth;
	m_itemSize.height = fHeight;
	
}

D2D1_SIZE_F CD2DExplorerUI::GetItemSize() const
{
	return m_itemSize;
}

void CD2DExplorerUI::SetItemSpacing(float fHorizontal, float fVertical)
{
	m_itemSpacing.width = max(0.0f, fHorizontal);
	m_itemSpacing.height = max(0.0f, fVertical);
}

void CD2DExplorerUI::SetExplorerMode(CD2DExplorerUIMode mode)
{
	if (m_explorerMode != mode)
	{
		m_explorerMode = mode;
	}
}

CD2DExplorerUI::CD2DExplorerUIMode CD2DExplorerUI::GetExplorerMode() const
{
	return m_explorerMode;
}

void CD2DExplorerUI::SetMinItemWidth(float width)
{
	m_minItemWidth = max(1.0f, width);
}

//-----------------------------------------------------------------------------
// 滚动相关
//-----------------------------------------------------------------------------
void CD2DExplorerUI::SetScrollPosition(float fScrollPos)
{
	float fMaxScrollPos = GetMaxScrollPosition();
	if (fabs(m_fScrollPosition - fScrollPos) > 0.001f)
	{
		m_fScrollPosition = max(0.0f, min(fScrollPos, fMaxScrollPos));
		m_scrollV.m_position = m_fScrollPosition;
	}
}

float CD2DExplorerUI::GetScrollPosition() const
{
	return m_fScrollPosition;
}


void CD2DExplorerUI::SetScrollWidth(float fScrollBarWidth)
{
	m_fScrollBarWidth = fScrollBarWidth;
}

void CD2DExplorerUI::SetVerticalScroll(D2D1_RECT_F& borderRect, float fVisibleHeight, float fTotalHeight, float minRange, float maxRange)
{
	D2D1_RECT_F scrollBarRect = {
		borderRect.right - m_fScrollBarWidth,
		borderRect.top,
		borderRect.right,
		borderRect.bottom
	};
	m_scrollV.SetRectangle(scrollBarRect);
	m_scrollV.SetRange(minRange, maxRange);
	m_scrollV.SetPageSize(fVisibleHeight);
	m_scrollV.SetThumbSize(max(10.0f, fVisibleHeight * fVisibleHeight / fTotalHeight));
}

CD2DScrollBarUI& CD2DExplorerUI::GetVerticalScroll()
{
	return m_scrollV;
}

//-----------------------------------------------------------------------------
// 回调
//-----------------------------------------------------------------------------
void CD2DExplorerUI::SetOnClickItemCallback(OnClickItemCallback callback, void* userData)
{
	m_clickCallback = callback;
	SetUserData(userData);
}

CD2DExplorerUI::OnClickItemCallback CD2DExplorerUI::GetOnClickItemCallback() const
{
	return m_clickCallback;
}

void CD2DExplorerUI::UpdateLayoutParameters(const D2D1_RECT_F& contentRect)
{
	float contentWidth = contentRect.right - contentRect.left;
	float contentHeight = contentRect.bottom - contentRect.top;

	// 水平间距（左右两侧及中间）
	float horzPadding = m_itemSpacing.width;
	// 垂直间距（上下两侧及中间）
	float vertPadding = m_itemSpacing.height;

	// 边界保护：如果宽度无法容纳至少一个 Item 加两侧间距，强制为 1 列
	if (contentWidth <= 2.0f * horzPadding)
	{
		m_calculatedColumns = 1;
		m_calculatedItemWidth = m_itemSize.width;
		m_calculatedItemHeight = m_itemSize.height;
		m_calculatedHorzSpacing = horzPadding;
		m_calculatedVertSpacing = vertPadding;
		return;
	}

	// 固定间距（首尾及中间都使用此值）
	m_calculatedHorzSpacing = horzPadding;
	m_calculatedVertSpacing = vertPadding;

	if (m_explorerMode == FixedSize)
	{
		// --- 固定大小模式：宽度固定，间距动态 ---
		float itemWidth = m_itemSize.width;

		// 可用宽度 = 内容宽度 - 左右两侧间距
		float availableWidth = contentWidth ;
		// 每个 Item 占据的空间 = 自身宽度 + 右侧间距（最后一项右侧间距被包含在 availableWidth 的右端）
		int columns = max(1, (int)((availableWidth + horzPadding) / (itemWidth + horzPadding)));

		m_calculatedColumns = columns;
		m_calculatedItemWidth = itemWidth;
		m_calculatedItemHeight = m_itemSize.height;
	}
	else // AutoExpand
	{
		// --- 自动扩充模式：间距固定，宽度自动调整 ---
		float refWidth = m_itemSize.width;
		float availableWidth = contentWidth ;
		int columns = max(1, (int)((availableWidth + horzPadding) / (refWidth + horzPadding)));

		// 尝试减少列数，直到计算出的宽度不小于最小宽度
		while (columns > 1)
		{
			// 公式：总宽度 = (columns+1)*horzPadding + columns*itemWidth
			float itemWidth = (contentWidth - (columns - 1) * horzPadding) / columns;
			if (itemWidth >= m_minItemWidth)
				break;
			columns--;
		}

		m_calculatedColumns = columns;
		m_calculatedItemHeight = m_itemSize.height;

		if (columns > 0)
		{
			m_calculatedItemWidth = (contentWidth - (columns - 1) * horzPadding) / columns;
			m_calculatedItemWidth = max(m_minItemWidth, m_calculatedItemWidth);
		}
		else
		{
			m_calculatedItemWidth = m_minItemWidth;
		}
	}

	// 垂直方向：同样采用首尾间距模式，行数计算类似（但垂直方向没有自动高度模式，高度固定）
	// 注意：垂直方向的总高度计算依赖行数和间距，在 GetMaxScrollPosition 和 Render 中处理。
}

D2D1_RECT_F CD2DExplorerUI::GetItemRect(int nIndex) const
{
	if (nIndex < 0 || nIndex >= m_items.GetCount())
		return D2D1::RectF(0, 0, 0, 0);

	D2D1_RECT_F contentRect = GetContentRectangle();

	// 恢复为标准的先行后列逻辑（除非你之前特意要求改为先列后行，否则通常 UI 都是先行后列）
	// 如果你确实需要“先填满列”，请改回 col = nIndex / m_calculatedColumns
	int row = nIndex / m_calculatedColumns;
	int col = nIndex % m_calculatedColumns;

	// 核心修改：起始位置加上 Padding
	float startX = contentRect.left ; 
	float startY = contentRect.top  - m_fScrollPosition;

	// 步进计算：Index * (尺寸 + 间距)
	// 注意：这里的间距就是 Padding，因为我们在 UpdateLayoutParameters 里已经把它们设为一样了
	float x = startX + col * (m_calculatedItemWidth + m_calculatedHorzSpacing);
	float y = startY + row * (m_calculatedItemHeight + m_calculatedVertSpacing);

	return D2D1::RectF(x, y, x + m_calculatedItemWidth, y + m_calculatedItemHeight);
}

int CD2DExplorerUI::GetItemIndexAtPoint(const D2D1_POINT_2F& point) const
{
	D2D1_RECT_F contentRect = GetContentRectangle();
	if (!IsPointInRect(point))
		return -1;

	if (m_IsShowScrollBar && point.x >= contentRect.right - m_fScrollBarWidth)
		return -1;

	// 核心修改：计算相对坐标时减去 Padding
	float localX = point.x - contentRect.left ;
	float localY = point.y - contentRect.top  + m_fScrollPosition;

	if (localX < 0 || localY < 0)
		return -1;

	float itemW = m_calculatedItemWidth;
	float itemH = m_calculatedItemHeight;
	float horzSpacing = m_calculatedHorzSpacing;
	float vertSpacing = m_calculatedVertSpacing;

	// 计算列
	int col = (int)(localX / (itemW + horzSpacing));
	if (col < 0 || col >= m_calculatedColumns)
		return -1;

	// 碰撞检测
	float itemLeft = col * (itemW + horzSpacing);
	if (localX < itemLeft || localX >= itemLeft + itemW)
		return -1;

	// 计算行
	int row = (int)(localY / (itemH + vertSpacing));
	if (row < 0)
		return -1;

	float itemTop = row * (itemH + vertSpacing);
	if (localY < itemTop || localY >= itemTop + itemH)
		return -1;

	int index = row * m_calculatedColumns + col;
	if (index >= 0 && index < m_items.GetCount())
		return index;

	return -1;
}

float CD2DExplorerUI::GetMaxScrollPosition() const
{
	D2D1_RECT_F contentRect = GetContentRectangle();
	float fVisibleHeight = contentRect.bottom - contentRect.top;

	int nItems = (int)m_items.GetCount();
	if (nItems == 0)
		return 0.0f;

	int nRows = (nItems + m_calculatedColumns - 1) / m_calculatedColumns;

	// 核心修改：总高度计算需要加上底部的 Padding
	// 总高 = N行 * (高 + 间距) + 底部间距
	// 注意：这里不需要像之前那样减去一个间距，因为现在首尾都有间距
	float fTotalHeight = nRows * (m_calculatedItemHeight + m_calculatedVertSpacing) + m_calculatedVertSpacing;

	return max(0.0f, fTotalHeight - fVisibleHeight);
}

bool CD2DExplorerUI::OnMouseDownL(const D2D1_POINT_2F& point)
{
	if (!m_isEnabled || !m_isVisible)
		return false;
	if (!IsPointInRect(point))
		return false;

	if (m_IsShowScrollBar && m_scrollV.IsHitTest(point))
		return m_scrollV.OnMouseDownL(point);

	int nIndex = GetItemIndexAtPoint(point);
	if (nIndex != -1)
	{
		SelectItem(nIndex);
		if (m_clickCallback)
		{
			CD2DExplorerItemUI* pItem = GetItem(nIndex);
			if (pItem)
				m_clickCallback(this, pItem);
		}
		return true;
	}
	return false;
}

bool CD2DExplorerUI::OnMouseMove(const D2D1_POINT_2F& point)
{
	if (!m_isEnabled || !m_isVisible)
		return false;

	if (IsPointInRect(point))
	{
		int nIndex = GetItemIndexAtPoint(point);
		if (nIndex != m_nHoverIndex)
		{
			m_nHoverIndex = nIndex;
			return true;
		}
		else
		{
			if (m_IsShowScrollBar)
				return m_scrollV.OnMouseMove(point);
		}
	}
	else
	{
		return OnMouseLeave(point);
	}
	return false;
}

bool CD2DExplorerUI::OnMouseUpL(const D2D1_POINT_2F& point)
{
	if (!m_isEnabled || !m_isVisible)
		return false;
	if (m_IsShowScrollBar)
		return m_scrollV.OnMouseUpL(point);
	return false;
}

bool CD2DExplorerUI::OnMouseLeave(const D2D1_POINT_2F& point)
{
	m_nHoverIndex = -1;
	if (m_IsShowScrollBar)
		m_scrollV.OnMouseLeave(point);
	return true;
}

bool CD2DExplorerUI::OnMouseWheel(float delta)
{
	float fNewScrollPos = m_fScrollPosition - delta * 2.0f;
	SetScrollPosition(fNewScrollPos);
	if (m_IsShowScrollBar)
		m_scrollV.OnMouseWheel(delta);
	return true;
}

//-----------------------------------------------------------------------------
// 渲染
//-----------------------------------------------------------------------------
void CD2DExplorerUI::Render(CD2DRender* pRender)
{
	if (!IsVisible() || !pRender)
		return;

	D2D1_RECT_F rect = GetContentRectangle();
	// 绘制背景（调试用）
	CD2DControlUI::Render(pRender);

	// 更新布局参数（必须在绘制前，因为可能内容区域宽度变化）
	UpdateLayoutParameters(rect);

	// 计算总行数、总高度，决定是否显示滚动条
	int nItems = (int)m_items.GetCount();
	int nRows = (nItems + m_calculatedColumns - 1) / m_calculatedColumns;

	// 新逻辑：Padding 模式下，总高度 = 行数 * (高 + 间距) + 底部间距
	float fTotalHeight = nRows * (m_calculatedItemHeight + m_calculatedVertSpacing);
	float fVisibleHeight = rect.bottom - rect.top;
	m_IsShowScrollBar = (fTotalHeight > fVisibleHeight);

	if (m_IsShowScrollBar)
	{

		D2D1_RECT_F borderRect = GetPaddingRectangle();
		float fMaxScrollPos = max(0.0f, fTotalHeight - fVisibleHeight);
		SetVerticalScroll(borderRect, fVisibleHeight, fTotalHeight, 0.0f, fMaxScrollPos);
		m_fScrollPosition = m_scrollV.GetPosition();
		m_scrollV.Render(pRender);
	}
	else
	{
		m_fScrollPosition = 0.0f;
		m_scrollV.SetPosition(0.0f);
	}

	// 裁剪到内容区域
	pRender->PushAxisAlignedClip(GetRectangle());

	// --- 修改部分开始 ---
	// 优化绘制：计算可见范围
	// 必须使用 (ItemHeight + Spacing) 作为步长，因为行高包含了间距
	float rowStep = m_calculatedItemHeight + m_calculatedVertSpacing;

	// 计算第一行可见行：(滚动位置 / 行步长) - 1 (减1是为了保险，防止边界漏画)
	int nFirstVisibleRow = max(0, (int)(m_fScrollPosition / rowStep) - 1);

	// 计算最后一行可见行：((滚动位置 + 可见高度) / 行步长) + 1
	int nLastVisibleRow = min(nRows - 1, (int)((m_fScrollPosition + fVisibleHeight) / rowStep) + 1);
	// --- 修改部分结束 ---

	for (int row = nFirstVisibleRow; row <= nLastVisibleRow; ++row)
	{
		for (int col = 0; col < m_calculatedColumns; ++col)
		{
			int nIndex = row * m_calculatedColumns + col;
			if (nIndex >= nItems)
				break;

			CD2DItemUI* pItem = m_items[nIndex];
			if (!pItem)
				continue;

			D2D1_RECT_F itemRect = GetItemRect(nIndex);
			pItem->SetRectangle(itemRect);
			pItem->SetSelected(nIndex == m_nSelectedIndex);
			pItem->SetHovered(nIndex == m_nHoverIndex);
			pItem->Render(pRender);
		}
	}

	pRender->PopAxisAlignedClip();
}