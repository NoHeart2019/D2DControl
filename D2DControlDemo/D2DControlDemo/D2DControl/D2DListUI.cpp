#include "stdafx.h"
#include "D2DListUI.h"



CD2DListUI::CD2DListUI()
	: CD2DControlUI()
	, m_selectedIndex(-1)
	, m_itemHeight(24.0f)
	, m_itemSpacing(3.0f)
	, m_maxVisibleItems(0)
	, m_scrollOffsetY(0.0f)
	, m_totalItemCount(0)
	, m_scrollBarVisible(false)
	, m_hoveredIndex(-1)
	, m_selectionCallback(nullptr)
	, m_pCallbackUserData(nullptr)
{
	SetWidthPolicy(CD2DSizePolicy::AutoSize());
	SetHeightPolicy(CD2DSizePolicy::AutoSize());

	
}

CD2DListUI::~CD2DListUI()
{
	ClearItems();   // 清空数组，不 delete 对象
}

void CD2DListUI::AddItem(CD2DControlUI* pItem)
{
	if (!pItem) return;

	pItem->SetDefaultStyle(m_itemStyle.normal);
	pItem->SetStyle(ED2DStatus::Hover, m_itemStyle.hover);
	pItem->SetStyle(ED2DStatus::Checked, m_itemStyle.checked);
	pItem->SetHeightPolicy(CD2DSizePolicy::FixedSize(m_itemHeight));

	m_items.Add(pItem);
	m_totalItemCount = static_cast<int>(m_items.GetCount());

	if (m_selectedIndex < 0)
		SetSelectedIndex(0);

	Layout();
}

void CD2DListUI::RemoveItem(int index)
{
	if (index < 0 || index >= m_totalItemCount) return;

	if (index == m_selectedIndex)
	{
		m_selectedIndex = -1;
		if (m_totalItemCount > 1)
		{
			int newSel = (index < m_totalItemCount - 1) ? index : index - 1;
			SetSelectedIndex(newSel);
		}
	}
	else if (index < m_selectedIndex)
		m_selectedIndex--;

	m_items.RemoveAt(index);
	m_totalItemCount--;
	Layout();
}

void CD2DListUI::ClearItems()
{
	m_items.RemoveAll();
	m_totalItemCount = 0;
	m_selectedIndex = -1;
	m_hoveredIndex = -1;
	m_scrollOffsetY = 0.0f;

	m_scrollBar.SetValue(0);
	m_scrollBarVisible = false;
	m_scrollBar.SetVisible(false);

	Layout();
}

CD2DControlUI* CD2DListUI::GetItem(int index) const
{
	if (index < 0 || index >= m_totalItemCount) return nullptr;
	return m_items[index];
}

void CD2DListUI::SetSelectedIndex(int index)
{
	if (index < 0 || index >= m_totalItemCount || index == m_selectedIndex) return;

	if (m_selectedIndex >= 0)
	{
		CD2DControlUI* pOld = m_items[m_selectedIndex];
		if (pOld) pOld->SetChecked(false);
	}

	m_selectedIndex = index;
	CD2DControlUI* pNew = m_items[m_selectedIndex];
	if (pNew) pNew->SetChecked(true);

	NotifySelectionChanged();
}

CD2DControlUI* CD2DListUI::GetSelectedItem() const
{
	return GetItem(m_selectedIndex);
}

void CD2DListUI::SetOnSelectionChanged(SelectionCallback cb, void* pData)
{
	m_selectionCallback = cb;
	m_pCallbackUserData = pData;
}

void CD2DListUI::SetItemHeight(float height)
{
	if (height <= 0.0f) return;
	m_itemHeight = height;
	for (int i = 0; i < m_totalItemCount; ++i)
	{
		CD2DControlUI* pItem = m_items[i];
		if (pItem) pItem->SetHeightPolicy(CD2DSizePolicy::FixedSize(m_itemHeight));
	}
	Layout();
}

void CD2DListUI::SetItemSpacing(float spacing)
{
	m_itemSpacing = spacing;
	Layout();
}

void CD2DListUI::SetMaxVisibleItems(int count)
{
	m_maxVisibleItems = count;
	Layout();
}

int CD2DListUI::GetMaxVisibleItems() const
{
	return m_maxVisibleItems;
}

void CD2DListUI::SetItemStyle(const CD2DItemStyle& style)
{
	m_itemStyle = style;
	for (int i = 0; i < m_totalItemCount; ++i)
	{
		CD2DControlUI* pItem = m_items[i];
		if (pItem)
		{
			pItem->SetDefaultStyle(m_itemStyle.normal);
			pItem->SetStyle(ED2DStatus::Hover, m_itemStyle.hover);
			pItem->SetStyle(ED2DStatus::Checked, m_itemStyle.checked);
		}
	}
}

// ==================== 布局 ====================
void CD2DListUI::Layout()
{
	if (m_totalItemCount == 0) return;

	D2D1_RECT_F content = GetContentRectangle();
	float availW = content.right - content.left;
	float availH = content.bottom - content.top;
	if (availW <= 0.0f || availH <= 0.0f) return;

	float rowHeight = m_itemHeight + m_itemSpacing;
	float totalContentH = m_totalItemCount * m_itemHeight + (m_totalItemCount - 1) * m_itemSpacing;
	bool needScroll = (totalContentH > availH);

	float listWidth = availW;
	const float BAR_WIDTH = 10.0f;
	if (needScroll)
	{
		listWidth -= BAR_WIDTH;
		if (listWidth < 0.0f) listWidth = 0.0f;
	}

	for (int i = 0; i < m_totalItemCount; ++i)
	{
		CD2DControlUI* pItem = m_items[i];
		if (!pItem) continue;

		float y = content.top + i * rowHeight;
		pItem->SetPosition(content.left, y);
		pItem->SetWidthPolicy(CD2DSizePolicy::FixedSize(listWidth));
		pItem->SetHeightPolicy(CD2DSizePolicy::FixedSize(m_itemHeight));
		pItem->SetSize(listWidth, m_itemHeight);

	}

	m_scrollBarVisible = needScroll;
	m_scrollBar.SetVisible(needScroll);
	if (needScroll)
	{
		D2D1_RECT_F barRect = D2D1::RectF(content.right - BAR_WIDTH, content.top,
			content.right, content.bottom);
		m_scrollBar.SetRectangle(barRect);

		int maxScroll = static_cast<int>(totalContentH );
		if (maxScroll < 0) maxScroll = 0;
		m_scrollBar.SetRange(0, maxScroll);
		m_scrollBar.SetPageSize(static_cast<int>(availH));
		m_scrollBar.SetValue(static_cast<int>(m_scrollOffsetY));

		if (m_scrollOffsetY > static_cast<float>(maxScroll))
			m_scrollOffsetY = static_cast<float>(maxScroll);
	}
	else
	{
		m_scrollOffsetY = 0.0f;
	}
}

float CD2DListUI::CalculateContentHeight(int itemCount) const
{
	if (itemCount <= 0) return 0.0f;
	return itemCount * m_itemHeight + (itemCount - 1) * m_itemSpacing;
}

// ==================== 绘制 ====================
void CD2DListUI::DrawControl(CD2DRender* pRender)
{
	Layout();
	if (!IsVisible() || !pRender) return;
	ID2D1RenderTarget* pRT = pRender->GetRenderTarget();
	if (!pRT) return;

	CD2DStyle style = CalculateEffectiveStyle();
	D2D1_RECT_F borderRect = GetBorderRectangle();
	D2D1_RECT_F contentRect = GetContentRectangle();

	CD2DControlUI::DrawControl(pRender);

	D2D1_RECT_F clipRect = contentRect;
	if (m_scrollBarVisible)
		clipRect.right -= 10.0f;
	if (m_items.GetCount() == 0)
		return;

	pRT->PushAxisAlignedClip(clipRect, D2D1_ANTIALIAS_MODE_ALIASED);

	D2D1_MATRIX_3X2_F oldTransform;
	pRT->GetTransform(&oldTransform);
	pRT->SetTransform(oldTransform * D2D1::Matrix3x2F::Translation(0.0f, -m_scrollOffsetY));

	int first, last;
	GetVisibleRange(first, last);
	for (int i = first; i <= last; ++i)
	{
		CD2DControlUI* pItem = m_items[i];
		if (pItem && pItem->IsVisible())
			pItem->DrawControl(pRender);
	}

	pRT->SetTransform(oldTransform);
	pRT->PopAxisAlignedClip();

	if (m_scrollBarVisible)
		m_scrollBar.DrawControl(pRender);
}

void CD2DListUI::GetVisibleRange(int& first, int& last) const
{
	first = last = -1;
	if (m_totalItemCount == 0) return;

	D2D1_RECT_F content = GetContentRectangle();
	float viewTop = m_scrollOffsetY;
	float viewHeight = content.bottom - content.top;
	float viewBottom = viewTop + viewHeight;

	float rowHeight = m_itemHeight + m_itemSpacing;

	first = static_cast<int>(viewTop / rowHeight);
	if (first < 0) first = 0;

	// 计算最后一个可见行：viewBottom / rowHeight 向下取整，但需判断是否跨在间距上
	last = static_cast<int>((viewBottom - 0.001f) / rowHeight);
	if (last >= m_totalItemCount) last = m_totalItemCount - 1;

	// 调整：如果 viewBottom 刚好穿过一个项与下一个项之间的间距，last 可能指向不存在的项，但经上面限制后没问题。
	if (first > last)
	{
		first = -1;
		last = -1;
	}
}

// ==================== 鼠标事件 ====================
bool CD2DListUI::OnMouseDownL(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled()) return false;

	if (m_scrollBarVisible && m_scrollBar.IsHitTest(point))
	{
		bool handled = m_scrollBar.OnMouseDownL(point);
		m_scrollOffsetY = static_cast<float>(m_scrollBar.GetValue());
		return handled;
	}

	D2D1_POINT_2F contentPoint = { point.x, point.y + m_scrollOffsetY };
	int idx = HitTestItem(contentPoint);
	if (idx >= 0)
	{
		SetSelectedIndex(idx);
		return true;
	}
	return false;
}

bool CD2DListUI::OnMouseMove(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled()) return false;

	if (m_scrollBarVisible && m_scrollBar.isThumbDragging())
	{
		bool handled = m_scrollBar.OnMouseMove(point);
		m_scrollOffsetY = static_cast<float>(m_scrollBar.GetValue());
		return handled;
	}

	D2D1_POINT_2F contentPoint = { point.x, point.y + m_scrollOffsetY };
	int idx = HitTestItem(contentPoint);

	if (idx != m_hoveredIndex)
	{
		if (m_hoveredIndex >= 0)
		{
			CD2DControlUI* pOld = m_items[m_hoveredIndex];
			if (pOld) pOld->SetHover(false);
		}
		m_hoveredIndex = idx;
		if (m_hoveredIndex >= 0)
		{
			CD2DControlUI* pNew = m_items[m_hoveredIndex];
			if (pNew) pNew->SetHover(true);
		}
	}
	return true;
}

bool CD2DListUI::OnMouseUpL(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled()) return false;

	if (m_scrollBarVisible && m_scrollBar.IsPressed())
	{
		bool handled = m_scrollBar.OnMouseUpL(point);
		m_scrollOffsetY = static_cast<float>(m_scrollBar.GetValue());
		return handled;
	}

	// 无额外操作，交给基类
	return CD2DControlUI::OnMouseUpL(point);
}

bool CD2DListUI::OnMouseWheel(float delta, const D2D1_POINT_2F& point)
{
	if (!m_scrollBarVisible) return false;

	float step = m_itemHeight + m_itemSpacing;  // 滚动一个完整行
	float newOffset = m_scrollOffsetY - (delta > 0 ? step : -step);

	D2D1_RECT_F content = GetContentRectangle();
	float maxOffset = max(0.0f, m_totalItemCount * m_itemHeight + (m_totalItemCount - 1) * m_itemSpacing - (content.bottom - content.top));
	newOffset = max(0.0f, min(newOffset, maxOffset));

	if (fabs(newOffset - m_scrollOffsetY) > 0.001f)
	{
		m_scrollOffsetY = newOffset;
		m_scrollBar.SetValue(static_cast<int>(newOffset));

		D2D1_POINT_2F contentPoint = { point.x, point.y + m_scrollOffsetY };
		int idx = HitTestItem(contentPoint);
		if (idx != m_hoveredIndex)
		{
			if (m_hoveredIndex >= 0)
			{
				CD2DControlUI* pOld = m_items[m_hoveredIndex];
				if (pOld) pOld->SetHover(false);
			}
			m_hoveredIndex = idx;
			if (m_hoveredIndex >= 0)
			{
				CD2DControlUI* pNew = m_items[m_hoveredIndex];
				if (pNew) pNew->SetHover(true);
			}
		}
	}
	return true;
}

bool CD2DListUI::OnMouseLeave(const D2D1_POINT_2F& point)
{
	if (m_hoveredIndex >= 0)
	{
		CD2DControlUI* pOld = m_items[m_hoveredIndex];
		if (pOld) pOld->SetHover(false);
		m_hoveredIndex = -1;
	}
	if (m_scrollBarVisible)
		m_scrollBar.OnMouseLeave(point);
	return true;
}

// ==================== 辅助 ====================
int CD2DListUI::HitTestItem(const D2D1_POINT_2F& contentPoint) const
{
	if (m_totalItemCount == 0) return -1;

	D2D1_RECT_F content = GetContentRectangle();
	float effectiveWidth = content.right - content.left;
	if (m_scrollBarVisible) effectiveWidth -= 10.0f;

	if (contentPoint.x < content.left || contentPoint.x > content.left + effectiveWidth)
		return -1;

	float relY = contentPoint.y - content.top;
	if (relY < 0.0f) return -1;

	float rowHeight = m_itemHeight + m_itemSpacing;
	int idx = static_cast<int>(relY / rowHeight);
	if (idx >= m_totalItemCount) return -1;

	// 检查是否落在间距区域内（项内容区域之外）
	float offsetInRow = relY - idx * rowHeight;
	if (offsetInRow >= m_itemHeight)
		return -1;  // 在间距内，不算命中

	return idx;
}

void CD2DListUI::NotifySelectionChanged()
{
	if (m_selectionCallback)
		m_selectionCallback(this, m_selectedIndex, m_pCallbackUserData);
}