#include "stdafx.h"
#include "D2DExploreUI.h"

const float CD2DExploreUI::DRAG_START_THRESHOLD = 5.0f;

// ---------- 构造函数 ----------
CD2DExploreUI::CD2DExploreUI()
	: CD2DControlUI()
	, m_viewMode(List)
	, m_columns(0)
	, m_hSpacing(4.0f)
	, m_vSpacing(4.0f)
	, m_itemWidth(0.0f)
	, m_itemHeight(0.0f)
	, m_wrapAutoFit(false)
	, m_scrollOffsetY(0.0f)
	, m_scrollBarVisible(true)
	, m_totalContentHeight(0.0f)
	, m_selectionMode(Single)
	, m_selectedIndex(-1)
	, m_draggingEnabled(false)       // 默认关闭拖拽
	, m_isDragging(false)
	, m_dragSourceIndex(-1)
	, m_dragInsertIndex(-1)
	, m_pDragGhost(nullptr)
	, m_reorderCallback(nullptr)
	, m_pReorderUserData(nullptr)
	, m_hoveredIndex(-1)
{
	// 初始化滚动条
	m_scrollBar.SetOrientation(CD2DScrollUI::Vertical);
	m_scrollBar.SetVisible(true);

	CD2DScrollStyle defaultStyle;
	defaultStyle.thumb.background.m_type = CD2DBackground::Solid;
	defaultStyle.thumb.background.m_solid.color = D2D1::ColorF(0.6f, 0.6f, 0.6f);
	defaultStyle.thumb.radius = CD2DRadius(4.0f);
	defaultStyle.track.background.m_type = CD2DBackground::Solid;
	defaultStyle.track.background.m_solid.color = D2D1::ColorF(0.9f, 0.9f, 0.9f);
	m_scrollBar.SetScrollStyle(ED2DStatus::Normal, defaultStyle);
}

CD2DExploreUI::~CD2DExploreUI()
{
	CancelDragging();   // 确保清理幽灵控件
}

// ========== 视图设置 ==========
void CD2DExploreUI::SetViewMode(ViewMode mode) { m_viewMode = mode; Layout(); }
void CD2DExploreUI::SetColumns(int cols) { m_columns = cols; if (m_viewMode == Grid) Layout(); }
void CD2DExploreUI::SetHorizontalSpacing(float spacing) { m_hSpacing = spacing; Layout(); }
void CD2DExploreUI::SetVerticalSpacing(float spacing) { m_vSpacing = spacing; Layout(); }
void CD2DExploreUI::SetItemWidth(float width) { m_itemWidth = width; Layout(); }
void CD2DExploreUI::SetItemHeight(float height) { m_itemHeight = height; Layout(); }
void CD2DExploreUI::SetWrapAutoFit(bool enable) { m_wrapAutoFit = enable; Layout(); }

// ========== 项管理 ==========
CD2DControlUI* CD2DExploreUI::GetItemAt(int index) const
{
	if (index >= 0 && index < (int)m_items.GetCount())
		return m_items.GetAt(index);
	return nullptr;
}

int CD2DExploreUI::GetItemIndex(CD2DControlUI* pItem) const
{
	for (int i = 0; i < (int)m_items.GetCount(); ++i)
		if (m_items.GetAt(i) == pItem) return i;
	return -1;
}

void CD2DExploreUI::AddItem(CD2DControlUI* pItem)
{
	if (!pItem) return;
	m_items.Add(pItem);
	Layout();
}

void CD2DExploreUI::RemoveItem(CD2DControlUI* pItem)
{
	int idx = GetItemIndex(pItem);
	if (idx >= 0)
	{
		// 清理关联状态
		if (idx == m_selectedIndex)
		{
			UpdateItemChecked(m_selectedIndex, false);
			m_selectedIndex = -1;
		}
		if (m_hoveredIndex == idx)
			m_hoveredIndex = -1;
		// 如果正在拖拽且移除的正是源项，取消拖拽
		if (m_isDragging && idx == m_dragSourceIndex)
			CancelDragging();
		m_items.RemoveAt(idx);
		Layout();
	}
}

void CD2DExploreUI::RemoveAllItems()
{
	if (m_isDragging) CancelDragging();
	if (m_selectedIndex >= 0)
	{
		UpdateItemChecked(m_selectedIndex, false);
		m_selectedIndex = -1;
	}
	m_hoveredIndex = -1;
	m_items.RemoveAll();
	Layout();
}

// ========== 垂直滚动 ==========
void CD2DExploreUI::SetScrollOffsetY(float offset)
{
	float maxOffset = GetTotalContentHeight() - GetViewportHeight();
	if (maxOffset < 0.0f) maxOffset = 0.0f;
	if (offset < 0.0f) offset = 0.0f;
	if (offset > maxOffset) offset = maxOffset;
	m_scrollOffsetY = offset;
	if (m_scrollBar.GetValue() != (int)m_scrollOffsetY)
		m_scrollBar.SetValue((int)m_scrollOffsetY);
}

void CD2DExploreUI::SetScrollBarVisible(bool visible)
{
	m_scrollBarVisible = visible;
	m_scrollBar.SetVisible(visible);
	Layout();
}

bool CD2DExploreUI::IsScrollBarVisible() const
{
	return m_scrollBarVisible && m_scrollBar.IsVisible();
}

void CD2DExploreUI::SetScrollBarStyle(ED2DStatus state, const CD2DScrollStyle& style)
{
	m_scrollBar.SetScrollStyle(state, style);
}

float CD2DExploreUI::GetTotalContentHeight() const
{
	return m_totalContentHeight;
}

float CD2DExploreUI::GetViewportHeight() const
{
	D2D1_RECT_F contentRect = GetContentRectangle();
	return contentRect.bottom - contentRect.top;
}

void CD2DExploreUI::SyncScrollBar()
{
	float totalH = GetTotalContentHeight();
	float viewH = GetViewportHeight();
	int range = (int)(totalH > viewH ? totalH : viewH);
	m_scrollBar.SetRange(0, range);
	m_scrollBar.SetPageSize((int)viewH);
	m_scrollBar.SetValue((int)m_scrollOffsetY);
}

// ========== 单选 ==========
void CD2DExploreUI::SetSelectionMode(SelectionMode mode)
{
	m_selectionMode = mode;
	if (mode == SelNone)
		ClearSelection();
}

void CD2DExploreUI::SelectItem(int index)
{
	if (m_selectionMode == SelNone) return;
	if (index < 0 || index >= (int)m_items.GetCount()) return;
	if (index == m_selectedIndex) return;

	ClearSelection();
	m_selectedIndex = index;
	UpdateItemChecked(index, true);
}

void CD2DExploreUI::ClearSelection()
{
	if (m_selectedIndex >= 0)
	{
		UpdateItemChecked(m_selectedIndex, false);
		m_selectedIndex = -1;
	}
}

void CD2DExploreUI::UpdateItemChecked(int index, bool checked)
{
	CD2DControlUI* pItem = GetItemAt(index);
	if (pItem)
		pItem->SetChecked(checked);
}

// ========== 拖拽接口 ==========
void CD2DExploreUI::EnableDragging(bool enable)
{
	m_draggingEnabled = enable;
	if (!enable)
		CancelDragging();
}

void CD2DExploreUI::SetOnItemReordered(ReorderCallback callback, void* pUserData)
{
	m_reorderCallback = callback;
	m_pReorderUserData = pUserData;
}

// ========== 布局 ==========
void CD2DExploreUI::Layout()
{
	if (m_items.GetCount() == 0)
	{
		m_totalContentHeight = 0;
		return;
	}

	D2D1_RECT_F contentRect = GetContentRectangle();
	float totalW = contentRect.right - contentRect.left;
	float totalH = contentRect.bottom - contentRect.top;
	if (totalW <= 0.0f || totalH <= 0.0f) return;

	float scrollBarW = 0.0f;
	if (m_scrollBarVisible)
	{
		D2D1_SIZE_F barSize = m_scrollBar.GetDesiredSize(nullptr);
		scrollBarW = barSize.width > 0 ? barSize.width : 10.0f;
	}

	D2D1_RECT_F viewport = contentRect;
	viewport.right -= scrollBarW - m_hSpacing;

	switch (m_viewMode)
	{
	case List:   LayoutList(viewport);   break;
	case Horizontal: LayoutHorizontal(viewport); break;
	case Wrap:   LayoutWrap(viewport);   break;
	case Grid:   LayoutGrid(viewport);   break;
	}

	float totalContentH = GetTotalContentHeightInternal();
	m_totalContentHeight = totalContentH;

	bool needScroll = totalContentH > (viewport.bottom - viewport.top);
	if (needScroll != m_scrollBarVisible)
	{
		m_scrollBarVisible = needScroll;
		if (!needScroll) m_scrollOffsetY = 0;
		Layout();   // 重新布局（宽度可能因滚动条变化）
		return;
	}

	if (m_scrollBarVisible)
	{
		m_scrollBar.SetRectangle(D2D1::RectF(
			contentRect.right, contentRect.top,
			contentRect.right + scrollBarW, contentRect.bottom));
		SyncScrollBar();
	}
	else
	{
		m_scrollBar.SetVisible(false);
	}
}

// ---------- 各布局函数实现（与之前一致，略作缩写） ----------
void CD2DExploreUI::LayoutList(const D2D1_RECT_F& viewport)
{
	float y = viewport.top;
	float availW = viewport.right - viewport.left;
	for (int i = 0; i < (int)m_items.GetCount(); ++i)
	{
		CD2DControlUI* pItem = m_items.GetAt(i);
		if (!pItem || !pItem->IsVisible()) continue;
		if (pItem->IsAutoSize()) pItem->SizeToContent(nullptr);
		D2D1_SIZE_F nc = pItem->GetNonContentSize();
		float tw = m_itemWidth > 0 ? m_itemWidth : pItem->GetDesiredSize(nullptr).width;
		float th = m_itemHeight > 0 ? m_itemHeight : pItem->GetDesiredSize(nullptr).height;
		if (m_itemWidth <= 0) tw = availW;
		float cw = tw - nc.width;  if (cw < 0) cw = 0;
		float ch = th - nc.height; if (ch < 0) ch = 0;
		pItem->SetPosition(viewport.left, y);
		pItem->SetSize(cw, ch);
		y += th + m_vSpacing;
	}
	m_totalContentHeight = y - viewport.top;
}

void CD2DExploreUI::LayoutHorizontal(const D2D1_RECT_F& viewport)
{
	float x = viewport.left;
	float availH = viewport.bottom - viewport.top;
	for (int i = 0; i < (int)m_items.GetCount(); ++i)
	{
		CD2DControlUI* pItem = m_items.GetAt(i);
		if (!pItem || !pItem->IsVisible()) continue;
		if (pItem->IsAutoSize()) pItem->SizeToContent(nullptr);
		D2D1_SIZE_F nc = pItem->GetNonContentSize();
		float tw = m_itemWidth > 0 ? m_itemWidth : pItem->GetDesiredSize(nullptr).width;
		float th = m_itemHeight > 0 ? m_itemHeight : availH;
		float cw = tw - nc.width;  if (cw < 0) cw = 0;
		float ch = th - nc.height; if (ch < 0) ch = 0;
		pItem->SetPosition(x, viewport.top);
		pItem->SetSize(cw, ch);
		x += tw + m_hSpacing;
	}
	m_totalContentHeight = 0;
}

void CD2DExploreUI::LayoutWrap(const D2D1_RECT_F& viewport)
{
	float availW = viewport.right - viewport.left;
	if (availW <= 0.0f) return;

	if (m_wrapAutoFit && m_itemWidth > 0.0f)
	{
		int cols = (int)((availW + m_hSpacing) / (m_itemWidth + m_hSpacing));
		if (cols < 1) cols = 1;
		float colW = (availW - (cols - 1) * m_hSpacing) / cols;
		float x = viewport.left, y = viewport.top;
		int count = 0;
		float rowHeight = 0.0f;
		for (int i = 0; i < (int)m_items.GetCount(); ++i)
		{
			CD2DControlUI* pItem = m_items.GetAt(i);
			if (!pItem || !pItem->IsVisible()) continue;
			if (pItem->IsAutoSize()) pItem->SizeToContent(nullptr);
			D2D1_SIZE_F nc = pItem->GetNonContentSize();
			float th = m_itemHeight > 0 ? m_itemHeight : pItem->GetDesiredSize(nullptr).height;
			float cw = colW - nc.width;  if (cw < 0) cw = 0;
			float ch = th - nc.height;   if (ch < 0) ch = 0;
			pItem->SetPosition(x, y);
			pItem->SetSize(cw, ch);
			if (th > rowHeight) rowHeight = th;
			x += colW + m_hSpacing;
			if (++count % cols == 0)
			{
				y += rowHeight + m_vSpacing;
				x = viewport.left;
				rowHeight = 0.0f;
			}
		}
		if (count % cols != 0) y += rowHeight + m_vSpacing;
		m_totalContentHeight = y - viewport.top;
	}
	else
	{
		float x = viewport.left, y = viewport.top;
		float rowHeight = 0.0f;
		for (int i = 0; i < (int)m_items.GetCount(); ++i)
		{
			CD2DControlUI* pItem = m_items.GetAt(i);
			if (!pItem || !pItem->IsVisible()) continue;
			if (pItem->IsAutoSize()) pItem->SizeToContent(nullptr);
			D2D1_SIZE_F nc = pItem->GetNonContentSize();
			float tw = m_itemWidth > 0 ? m_itemWidth : pItem->GetDesiredSize(nullptr).width;
			float th = m_itemHeight > 0 ? m_itemHeight : pItem->GetDesiredSize(nullptr).height;
			if (x + tw > viewport.right && x > viewport.left)
			{
				y += rowHeight + m_vSpacing;
				x = viewport.left;
				rowHeight = 0.0f;
			}
			float cw = tw - nc.width;  if (cw < 0) cw = 0;
			float ch = th - nc.height; if (ch < 0) ch = 0;
			pItem->SetPosition(x, y);
			pItem->SetSize(cw, ch);
			x += tw + m_hSpacing;
			if (th > rowHeight) rowHeight = th;
		}
		if (rowHeight > 0.0f) y += rowHeight + m_vSpacing;
		m_totalContentHeight = y - viewport.top;
	}
}

void CD2DExploreUI::LayoutGrid(const D2D1_RECT_F& viewport)
{
	int cols = m_columns > 0 ? m_columns : 1;
	float availW = viewport.right - viewport.left;
	float cellW = (availW - (cols - 1) * m_hSpacing) / cols;
	float x = viewport.left, y = viewport.top;
	int count = 0;
	float rowHeight = 0.0f;
	for (int i = 0; i < (int)m_items.GetCount(); ++i)
	{
		CD2DControlUI* pItem = m_items.GetAt(i);
		if (!pItem || !pItem->IsVisible()) continue;
		if (pItem->IsAutoSize()) pItem->SizeToContent(nullptr);
		D2D1_SIZE_F nc = pItem->GetNonContentSize();
		float tw = m_itemWidth > 0 ? m_itemWidth : cellW;
		float th = m_itemHeight > 0 ? m_itemHeight : pItem->GetDesiredSize(nullptr).height;
		float cw = tw - nc.width;  if (cw < 0) cw = 0;
		float ch = th - nc.height; if (ch < 0) ch = 0;
		pItem->SetPosition(x, y);
		pItem->SetSize(cw, ch);
		if (th > rowHeight) rowHeight = th;
		x += cellW + m_hSpacing;
		if (++count % cols == 0)
		{
			y += rowHeight + m_vSpacing;
			x = viewport.left;
			rowHeight = 0.0f;
		}
	}
	if (count % cols != 0) y += rowHeight + m_vSpacing;
	m_totalContentHeight = y - viewport.top;
}

float CD2DExploreUI::GetTotalContentHeightInternal() const
{
	return m_totalContentHeight;
}

D2D1_SIZE_F CD2DExploreUI::MeasureContent(CD2DRender* pRender, float maxWidth) const
{
	return D2D1::SizeF(GetWidth(), GetHeight());
}

// ========== 绘制 ==========
void CD2DExploreUI::DrawControl(CD2DRender* pRender)
{
	if (!IsVisible() || !pRender) return;
	ID2D1RenderTarget* pRT = pRender->GetRenderTarget();
	if (!pRT) return;
	Layout();
	CD2DControlUI::DrawControl(pRender);

	float scrollBarW = m_scrollBarVisible ? 
		(m_scrollBar.GetDesiredSize(pRender).width > 10.0f ? 
		m_scrollBar.GetDesiredSize(pRender).width : 10.0f) : 0.0f;
	D2D1_RECT_F clipRect = GetPaddingRectangle();
	clipRect.right -= scrollBarW;

	pRT->PushAxisAlignedClip(clipRect, D2D1_ANTIALIAS_MODE_ALIASED);

	D2D1_MATRIX_3X2_F oldTransform;
	pRT->GetTransform(&oldTransform);
	pRT->SetTransform(oldTransform * D2D1::Matrix3x2F::Translation(0, -m_scrollOffsetY));

	for (int i = 0; i < (int)m_items.GetCount(); ++i)
	{
		CD2DControlUI* pItem = m_items.GetAt(i);
		if (pItem && pItem->IsVisible())
			pItem->DrawControl(pRender);
	}

	// 绘制插入指示线（可选，这里简化略；实际可画一条横线在 m_dragInsertIndex 对应项的前面）

	pRT->SetTransform(oldTransform);
	pRT->PopAxisAlignedClip();

	if (m_scrollBarVisible)
		m_scrollBar.DrawControl(pRender);

	// 绘制拖拽幽灵控件（不受滚动偏移影响，因为它随鼠标移动，坐标为屏幕坐标）
	if (m_isDragging && m_pDragGhost)
	{
		D2D1_SIZE_F sz = m_pDragGhost->GetDesiredSize(pRender);
		/*m_pDragGhost->SetPosition(m_dragGhostPos.x - sz.width * 0.5f,
			m_dragGhostPos.y - sz.height * 0.5f);*/
		m_pDragGhost->SetPosition(m_dragGhostPos.x ,
			m_dragGhostPos.y );
		m_pDragGhost->DrawControl(pRender);
	}
}

// ========== 鼠标事件（含拖拽逻辑） ==========
bool CD2DExploreUI::OnMouseDownL(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled()) return false;

	// 1. 滚动条优先
	if (m_scrollBarVisible && m_scrollBar.IsHitTest(point))
	{
		bool handled = m_scrollBar.OnMouseDownL(point);
		m_scrollOffsetY = (float)m_scrollBar.GetValue();
		return handled;
	}

	D2D1_POINT_2F contentPoint = ScreenToContent(point);
	int idx = HitTestItem(contentPoint);

	// 2. 拖拽候选准备
	if (m_draggingEnabled && idx >= 0)
	{
		m_dragSourceIndex = idx;
		m_dragStartPoint = point;
		m_isDragging = false;               // 尚未正式启动
		m_dragInsertIndex = idx;
		m_ptMouseDown = point;
		// 暂不传递事件给子控件，也不改变选择，等待移动判定
		return true;
	}

	// 3. 正常单击选择（非拖拽候选）
	if (m_selectionMode == Single)
	{
		if (idx >= 0)
			SelectItem(idx);
		else
			ClearSelection();
	}

	m_ptMouseDown = point;

	if (idx >= 0)
	{
		CD2DControlUI* pItem = m_items.GetAt(idx);
		if (pItem)
			pItem->OnMouseDownL(contentPoint);
	}
	return true;
}

bool CD2DExploreUI::OnMouseMove(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled()) return false;

	// 1. 滚动条拖拽中
	if (m_scrollBarVisible && m_scrollBar.isThumbDragging())
	{
		bool handled = m_scrollBar.OnMouseMove(point);
		m_scrollOffsetY = (float)m_scrollBar.GetValue();
		return handled;
	}

	// 2. 拖拽启动检测
	if (m_draggingEnabled && m_dragSourceIndex >= 0 && !m_isDragging)
	{
		float dx = point.x - m_dragStartPoint.x;
		float dy = point.y - m_dragStartPoint.y;
		if (dx < -DRAG_START_THRESHOLD || dx > DRAG_START_THRESHOLD ||
			dy < -DRAG_START_THRESHOLD || dy > DRAG_START_THRESHOLD)
		{
			StartDragging(m_dragSourceIndex);
		}
	}

	// 3. 拖拽进行中
	if (m_isDragging)
	{
		UpdateDragGhostPosition(point);
		D2D1_POINT_2F contentPoint = ScreenToContent(point);
		int newTarget = HitTestItem(contentPoint);
		ClearDragOverState();
		if (newTarget >= 0 && newTarget < (int)m_items.GetCount())
		{
			CD2DControlUI* pItem = m_items.GetAt(newTarget);
			if (pItem) pItem->GetStatus().SetDragOver(true);
		}
		m_dragInsertIndex = newTarget;
		ProcessEdgeAutoScroll(point);
		return true;
	}

	// 4. 正常悬停
	D2D1_POINT_2F contentPoint = ScreenToContent(point);
	int newHover = HitTestItem(contentPoint);
	if (newHover != m_hoveredIndex)
	{
		if (m_hoveredIndex >= 0)
		{
			CD2DControlUI* pOld = m_items.GetAt(m_hoveredIndex);
			if (pOld) pOld->OnMouseLeave(contentPoint);
		}
		if (newHover >= 0)
		{
			CD2DControlUI* pNew = m_items.GetAt(newHover);
			if (pNew) pNew->OnMouseEnter(contentPoint);
		}
		m_hoveredIndex = newHover;
	}
	if (newHover >= 0)
	{
		CD2DControlUI* pItem = m_items.GetAt(newHover);
		if (pItem) pItem->OnMouseMove(contentPoint);
	}
	return true;
}

bool CD2DExploreUI::OnMouseUpL(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled()) return false;

	// 1. 滚动条释放
	if (m_scrollBarVisible && m_scrollBar.IsPressed())
	{
		bool handled = m_scrollBar.OnMouseUpL(point);
		m_scrollOffsetY = (float)m_scrollBar.GetValue();
		return handled;
	}

	// 2. 拖拽结束
	if (m_isDragging)
	{
		FinishDragging(m_dragInsertIndex);
		return true;
	}

	// 3. 拖拽候选但未启动（轻点未移动）——补做正常点击行为
	if (m_dragSourceIndex >= 0)
	{
		int idx = m_dragSourceIndex;
		m_dragSourceIndex = -1;  // 清除候选标志
		D2D1_POINT_2F contentPoint = ScreenToContent(point);
		// 执行单选（若点击的就是候选源项，则直接选中它；若移动到了空白，则取消）
		if (HitTestItem(contentPoint) == idx)
		{
			SelectItem(idx);
			// 同时传递完整点击序列给子控件
			CD2DControlUI* pItem = m_items.GetAt(idx);
			if (pItem)
			{
				pItem->OnMouseDownL(contentPoint);
				pItem->OnMouseUpL(contentPoint);
			}
		}
		else
		{
			ClearSelection();
		}
		return true;
	}

	// 4. 正常释放：传递给命中的子控件
	D2D1_POINT_2F contentPoint = ScreenToContent(point);
	int idx = HitTestItem(contentPoint);
	if (idx >= 0)
	{
		CD2DControlUI* pItem = m_items.GetAt(idx);
		if (pItem) pItem->OnMouseUpL(contentPoint);
	}
	return true;
}

bool CD2DExploreUI::OnMouseLeave(const D2D1_POINT_2F& point)
{
	if (m_hoveredIndex >= 0)
	{
		CD2DControlUI* pOld = m_items.GetAt(m_hoveredIndex);
		if (pOld) pOld->OnMouseLeave(point);
		m_hoveredIndex = -1;
	}
	if (m_scrollBarVisible)
		m_scrollBar.OnMouseLeave(point);
	return true;
}

bool CD2DExploreUI::OnMouseWheel(float delta, const D2D1_POINT_2F& point)
{
	if (!m_scrollBarVisible) return false;
	float step = 30.0f;
	if (delta > 0)
		SetScrollOffsetY(m_scrollOffsetY - step);
	else
		SetScrollOffsetY(m_scrollOffsetY + step);
	return true;
}

bool CD2DExploreUI::OnKeyDown(DWORD keyCode)
{
	// 无特殊键盘处理，返回 false 让基类或其他控件处理
	return false;
}

// ========== 拖拽子系统实现 ==========
void CD2DExploreUI::StartDragging(int sourceIndex)
{
	if (sourceIndex < 0 || sourceIndex >= (int)m_items.GetCount()) return;
	// 防止重复启动
	if (m_isDragging) CancelDragging();

	CD2DControlUI* pSource = m_items.GetAt(sourceIndex);
	if (!pSource) return;

	// 隐藏源控件
	pSource->SetVisible(false);

	// 创建幽灵控件
	m_pDragGhost = new CD2DControlUI(); 
	// 简单复制外观
	m_pDragGhost->SetText(pSource->GetText());
	CD2DStyle ghostStyle = pSource->CalculateEffectiveStyle();
	m_pDragGhost->SetDefaultStyle(ghostStyle);
	//D2D1_SIZE_F sz = pSource->GetDesiredSize(nullptr);
	//m_pDragGhost->SetSize(sz.width, sz.height);
	m_pDragGhost->SetRectangle(pSource->GetRectangle());
	m_isDragging = true;
	m_dragSourceIndex = sourceIndex;
	m_dragInsertIndex = sourceIndex;  // 初始指示为源位置
}

void CD2DExploreUI::CancelDragging()
{
	ClearDragOverState();
	if (m_pDragGhost)
	{
		delete m_pDragGhost;
		m_pDragGhost = nullptr;
	}
	// 恢复源控件可见
	if (m_isDragging && m_dragSourceIndex >= 0 && m_dragSourceIndex < (int)m_items.GetCount())
	{
		CD2DControlUI* pSource = m_items.GetAt(m_dragSourceIndex);
		if (pSource) pSource->SetVisible(true);
	}
	m_isDragging = false;
	m_dragSourceIndex = -1;
	m_dragInsertIndex = -1;
}

void CD2DExploreUI::FinishDragging(int targetIndex)
{
	if (!m_isDragging) return;
	if (targetIndex < 0) targetIndex = (int)m_items.GetCount(); // 放到末尾
	if (targetIndex > (int)m_items.GetCount()) targetIndex = (int)m_items.GetCount();

	int source = m_dragSourceIndex;
	// 先清理幽灵和拖拽状态，但保留 source 用于后续操作
	ClearDragOverState();
	if (m_pDragGhost)
	{
		delete m_pDragGhost;
		m_pDragGhost = nullptr;
	}
	// 恢复源控件可见（如果后面还要移动，需先可见，或者移动后再设置可见均可）
	CD2DControlUI* pSource = m_items.GetAt(source);
	if (pSource) pSource->SetVisible(true);

	if (targetIndex == source)
	{
		// 未移动
		m_isDragging = false;
		m_dragSourceIndex = -1;
		m_dragInsertIndex = -1;
		Layout();
		return;
	}

	// 执行插入移动（使用 CAtlArray 操作）
	pSource = m_items.GetAt(source);  // 指针仍有效
	m_items.RemoveAt(source);
	int insertIdx = targetIndex;
	if (source < targetIndex) insertIdx--; // 移除后，原目标索引减一
	if (insertIdx < 0) insertIdx = 0;
	m_items.InsertAt(insertIdx, pSource);

	// 若被移动的项恰好是选中项，更新选中索引
	if (m_selectedIndex == source)
		m_selectedIndex = insertIdx;
	else if (m_selectedIndex >= source && m_selectedIndex < insertIdx)
		m_selectedIndex--;  // 中间项左移

	// 触发回调
	if (m_reorderCallback)
		m_reorderCallback(this, pSource, source, insertIdx, m_pReorderUserData);

	// 重置拖拽状态
	m_isDragging = false;
	m_dragSourceIndex = -1;
	m_dragInsertIndex = -1;

	Layout();   // 刷新布局
}

int CD2DExploreUI::HitTestItem(const D2D1_POINT_2F& contentPoint) const
{
	for (int i = 0; i < (int)m_items.GetCount(); ++i)
	{
		CD2DControlUI* pItem = m_items.GetAt(i);
		if (pItem && pItem->IsVisible() && pItem->IsHitTest(contentPoint))
			return i;
	}
	return -1;
}

int CD2DExploreUI::GetDragInsertIndex(float contentY) const
{
	for (int i = 0; i < (int)m_items.GetCount(); ++i)
	{
		CD2DControlUI* pItem = m_items.GetAt(i);
		if (!pItem || !pItem->IsVisible()) continue;
		D2D1_RECT_F rc = pItem->GetRectangle();
		float mid = rc.top + (rc.bottom - rc.top) * 0.5f;
		if (contentY < mid) return i;
	}
	return (int)m_items.GetCount(); // 末尾
}

void CD2DExploreUI::UpdateDragGhostPosition(const D2D1_POINT_2F& mouseScreenPos)
{
	m_dragGhostPos = mouseScreenPos;
}

void CD2DExploreUI::ProcessEdgeAutoScroll(const D2D1_POINT_2F& mouseScreenPos)
{
	D2D1_RECT_F rc = GetContentRectangle();
	float edge = 20.0f;
	float step = 10.0f;
	if (mouseScreenPos.y < rc.top + edge)
		SetScrollOffsetY(m_scrollOffsetY - step);
	else if (mouseScreenPos.y > rc.bottom - edge)
		SetScrollOffsetY(m_scrollOffsetY + step);
}

D2D1_POINT_2F CD2DExploreUI::ScreenToContent(const D2D1_POINT_2F& screenPoint) const
{
	return D2D1::Point2F(screenPoint.x, screenPoint.y + m_scrollOffsetY);
}

void CD2DExploreUI::ClearDragOverState()
{
	for (int i = 0; i < (int)m_items.GetCount(); ++i)
	{
		CD2DControlUI* pItem = m_items.GetAt(i);
		if (pItem) pItem->GetStatus().SetDragOver(false);
	}
}