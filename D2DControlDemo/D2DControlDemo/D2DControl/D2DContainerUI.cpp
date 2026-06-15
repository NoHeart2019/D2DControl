#include "stdafx.h"
#include "D2DContainerUI.h"

const float CD2DContainerUI::DEFAULT_SCROLLBAR_WIDTH = 10.0f;

CD2DContainerUI::CD2DContainerUI():m_pHoverChild(nullptr), m_pCapturedChild(nullptr), m_pFocusedChild(nullptr)
	, m_overflow(Hidden)
	, m_scrollOffsetY(0.0f)
	, m_scrollOffsetX(0.0f)
	, m_scrollBarVisible(false)
	, m_contentHeight(0.0f) {}

CD2DContainerUI::~CD2DContainerUI()
{
	// 子控件生命周期由外部管理，这里不 delete
	m_children.RemoveAll();
}


void CD2DContainerUI::SetOverflow(Overflow overflow)
{
	if (m_overflow != overflow)
	{
		m_overflow = overflow;
		m_scrollOffsetY = 0.0f;
		m_scrollBarVisible = false;
	}
}


// 添加控件（避免重复）
void CD2DContainerUI::AddChild(CD2DControlUI* pControl)
{
	if (!pControl)
		return;
	// 手动检查是否已存在
	for (size_t i = 0; i < m_children.GetCount(); ++i)
	{
		if (m_children[i] == pControl)
			return;   // 已存在，不添加
	}
	m_children.Add(pControl);
}

// 移除控件
void CD2DContainerUI::RemoveChild(CD2DControlUI* pControl)
{
	for (size_t i = 0; i < m_children.GetCount(); ++i)
	{
		if (m_children[i] == pControl)
		{
			m_children.RemoveAt(i);
			break;   // 找到后退出
		}
	}
}

// 清空所有子控件（注意函数名可能是 ClearChildren，您的声明为 ClearControlren，请保持一致）
void CD2DContainerUI::ClearChildren()   // 或改为 ClearChildren
{
	m_children.RemoveAll();
}

void CD2DContainerUI::BringChildToFront(CD2DControlUI* pControl)
{
	if (!pControl) return;
	int maxZ = 0;
	for (size_t i = 0; i < m_children.GetCount(); ++i)
	{
		int z = m_children[i]->GetZOrder();
		if (z > maxZ) maxZ = z;
	}
	pControl->SetZOrder(maxZ + 1);
}

int CD2DContainerUI::GetSortedChildren(const CAtlArray<CD2DControlUI*>& children,
	CAtlArray<CD2DControlUI*>& sorted)
{
	// 拷贝原始顺序
	sorted.Copy(children);

	// 使用排序（相同 ZOrder 保持原顺序）
	if (sorted.GetCount() > 1)
	{
		qsort(sorted.GetData(), sorted.GetCount(),sizeof(CD2DControlUI*), CompareZOrder);
	}

	return static_cast<int>(sorted.GetCount());
}


void CD2DContainerUI::SetFocusedChild(CD2DControlUI* pChild)
{
	if (pChild == m_pFocusedChild) return;
	if (m_pFocusedChild)
		m_pFocusedChild->GetStatus().SetFocused(false);
	m_pFocusedChild = pChild;
	if (m_pFocusedChild)
		m_pFocusedChild->GetStatus().SetFocused(true);
}

D2D1_SIZE_F CD2DContainerUI::MeasureContent(CD2DRender* pRender, float maxWidth, float maxValueHeight) const
{
	float maxVisW = 0.0f;
	float maxHeight = 0.0f;
	D2D1_RECT_F content = GetContentRectangle();   // 获取内容区矩形（位置无实际意义，仅用于坐标参考）

	for (size_t i = 0; i < GetChildCount(); ++i)
	{
		CD2DControlUI* pChild = GetChildAt(i);
		if (!pChild )
			continue;

		D2D1_SIZE_F size = pChild->GetDesiredSize(pRender);
		pChild->SetSize(size.width, size.height);
		CD2DStyle style = pChild->CalculateEffectiveStyle();

		float rightDec = style.margin.right + style.border.m_right.m_width + style.padding.right;
		float botDec = style.margin.bottom + style.border.m_bottom.m_width + style.padding.bottom;
		float leftDec = style.margin.left + style.border.m_left.m_width + style.padding.left;
		float topDec = style.margin.top + style.border.m_top.m_width + style.padding.top;

		float visRight = size.width  + leftDec+ rightDec;
		float visHeight =size.height + botDec + topDec;

		if (visRight > maxVisW) maxVisW = visRight;
		if (visHeight > maxHeight) maxHeight = visHeight;
	}

	// 视觉宽度（最右视觉边缘）相对于内容区左侧
	float totalWidth = maxVisW ;
	float totalHeight = maxHeight;
	return D2D1::SizeF(totalWidth, totalHeight);
}

// 将相对容器可见区域左上角的坐标转换为相对内容区域左上角的坐标（考虑滚动）
D2D1_POINT_2F CD2DContainerUI::TransformPoint(const D2D1_POINT_2F& visiblePoint) const
{
	if (m_scrollBarVisible)   // 或根据 m_overflow != Visible 判断
	{
		return D2D1::Point2F(
			visiblePoint.x + m_scrollOffsetX,
				visiblePoint.y + m_scrollOffsetY
		);
	}
	return visiblePoint;   // 无滚动时不变
}

void CD2DContainerUI::UpdateScrollBar(float viewportHeight)
{
	bool allowScroll = (m_overflow == Auto || m_overflow == Scroll);
	if (!allowScroll)
	{
		m_scrollBarVisible = false;
		m_scrollBar.SetVisible(false);
		return;
	}

	float contentH = m_contentHeight;   // 由 Layout 流程中先调用 MeasureContentHeight 填充
	bool needBar = (m_overflow == Scroll) || (contentH > viewportHeight);

	if (needBar != m_scrollBarVisible)
	{
		m_scrollBarVisible = needBar;
		if (!needBar) m_scrollOffsetY = 0.0f;
	}

	if (m_scrollBarVisible)
	{
		// 滚动条占据宽度（这里用固定 10px，也可从 GetDesiredSize 获取）
		float barWidth = 10.0f;
		D2D1_RECT_F contentRect = GetContentRectangle();
		D2D1_RECT_F barRect = D2D1::RectF(contentRect.right - barWidth, contentRect.top,
			contentRect.right, contentRect.bottom);
		m_scrollBar.SetRectangle(barRect);

		// 滚动范围 = 内容总高度 - 视口高度（不小于0）
		int maxScroll = static_cast<int>(max(0.0f, contentH));
		m_scrollBar.SetRange(0, maxScroll);
		m_scrollBar.SetPageSize(static_cast<int>(viewportHeight));
		m_scrollBar.SetValue(static_cast<int>(m_scrollOffsetY));
		m_scrollBar.SetVisible(true);
	}
	else
	{
		m_scrollBar.SetVisible(false);
	}
}

void CD2DContainerUI::Layout(CD2DRender* pRender)
{
	if (GetChildCount() == 0) return;
	if (!pRender) return;

	D2D1_RECT_F content = GetContentRectangle();

	// 仅处理绝对定位的子控件，其他子控件不做布局（由派生类处理）
	for (size_t i = 0; i < GetChildCount(); ++i)
	{
		CD2DControlUI* pChild = GetChildAt(i);
		if (!pChild || !pChild->IsVisible()) continue;

		if (pChild->GetPositionType() == CD2DPosition::Absolute)
		{
			// 绝对定位子控件：计算尺寸并放置
			const CD2DSizePolicy& wp = pChild->GetWidthPolicy();
			const CD2DSizePolicy& hp = pChild->GetHeightPolicy();
			float cw = pChild->GetWidth();
			float ch = pChild->GetHeight();

			// 宽度
			if (wp.mode == CD2DSizePolicy::Auto) {
				pChild->SizeToContent(pRender, content.right - content.left);
				cw = pChild->GetWidth();
			} else if (wp.mode == CD2DSizePolicy::Fixed) {
				cw = (wp.value > 0.0f) ? wp.value : cw;
			} else if (wp.mode == CD2DSizePolicy::Percent) {
				cw = (content.right - content.left) * (wp.value / 100.0f);
			}
			cw = max(wp.minValue, min(cw, wp.maxValue));

			// 高度
			if (hp.mode == CD2DSizePolicy::Auto) {
				pChild->SizeToContent(pRender, cw);
				ch = pChild->GetHeight();
			} else if (hp.mode == CD2DSizePolicy::Fixed) {
				ch = (hp.value > 0.0f) ? hp.value : ch;
			} else if (hp.mode == CD2DSizePolicy::Percent) {
				ch = (content.bottom - content.top) * (hp.value / 100.0f);
			}
			ch = max(hp.minValue, min(ch, hp.maxValue));

			CD2DPosition pos = pChild->GetPositionInfo();
			pChild->SetPosition(content.left + pos.left, content.top + pos.top);
			pChild->SetSize(cw, ch);
		}
	}

	m_contentHeight = MeasureContent(pRender).height;
	
	UpdateScrollBar(this->GetSize().height);
	
	
}


// 绘制：先绘制容器自身背景/边框，再绘制子控件
void CD2DContainerUI::DrawControl(CD2DRender* pRender)
{
	if (!IsVisible() || !pRender) return;
	ID2D1RenderTarget* pRT = pRender ? pRender->GetRenderTarget() : nullptr;
	if (!pRT) return;
	Layout(pRender);
	CD2DStyle style = CalculateEffectiveStyle();
	D2D1_RECT_F borderRect = GetBorderRectangle();
	D2D1_RECT_F paddingRect = GetPaddingRectangle();
	 D2D1_RECT_F contentRect = GetContentRectangle();

	// 1. 绘制容器自身的背景和边框
	DrawBackground(style, borderRect, pRender);
	DrawBorder(style, borderRect, pRender);
	CD2DStyle style1 = style;
	style1.border.m_top.m_color= D2D1::ColorF(D2D1::ColorF::Green);
	DrawBorder(style1, GetContentRectangle(), pRender);
	float scrollBarW = m_scrollBarVisible ? DEFAULT_SCROLLBAR_WIDTH : 0.0f;
	D2D1_RECT_F clipRect = contentRect;
	
	// 2. 裁剪到内容区域，防止子控件溢出
	clipRect.right -= scrollBarW;
	pRT->PushAxisAlignedClip(clipRect, D2D1_ANTIALIAS_MODE_ALIASED);

	// 应用滚动偏移
	D2D1_MATRIX_3X2_F oldTransform;
	pRT->GetTransform(&oldTransform);
	pRT->SetTransform(oldTransform * D2D1::Matrix3x2F::Translation(0, -m_scrollOffsetY));

	CAtlArray<CD2DControlUI*> sorted;
	GetSortedChildren(m_children, sorted);

	// 升序绘制：ZOrder 小的先画，大的后画 → 大的显示在上层
	for (size_t i = 0; i < sorted.GetCount(); ++i)
	{
		CD2DControlUI* pChild = sorted[i];
		if (pChild && pChild->IsVisible())
			pChild->DrawControl(pRender);
	}

	pRT->SetTransform(oldTransform);
	pRT->PopAxisAlignedClip();

	// 绘制滚动条（最上层）
	if (m_scrollBarVisible)
		m_scrollBar.DrawControl(pRender);

}

bool CD2DContainerUI::IsHitTest(const D2D1_POINT_2F& point) const
{
	return CD2DControlUI::IsHitTest(point);
}

// ========== 事件分发模板辅助 ==========
template<typename Func>
CD2DControlUI* CD2DContainerUI::DispatchEvent(const D2D1_POINT_2F& point, Func func)
{
	CAtlArray<CD2DControlUI*> sorted;
	GetSortedChildren(m_children, sorted);   // 获得按 ZOrder 升序的稳定副本
	// ① 容器命中测试（使用可见区域坐标，无需转换）
	if (!this->IsHitTest(point))
		return nullptr;

	// ② 统一转换为内容区域坐标
	D2D1_POINT_2F contentPt = TransformPoint(point);

	// 从上层（ZOrder 大）向下层遍历，保证上层控件优先响应事件
	for (size_t i = sorted.GetCount(); i-- > 0; )
	{
		CD2DControlUI* pChild = sorted[i];
		if (pChild && pChild->IsVisible() && pChild->IsHitTest(contentPt))
		{
			if (func(pChild, contentPt))
				return pChild;  // 事件被消费
		}
	}
	return nullptr;
}

// ========== 鼠标事件转发 ==========
bool CD2DContainerUI::OnMouseDownL2(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled()) return false;
	auto handler = [](CD2DControlUI* ctrl, const D2D1_POINT_2F& pt) -> bool {
		return ctrl->OnMouseDownL2(pt);
	};
	CD2DControlUI* target = DispatchEvent(point, handler);

	SetFocusedChild(target);

	if (target)
	{
		m_pCapturedChild = target;
		return true;
	}
	
	return CD2DControlUI::OnMouseDownL2(point);
}

bool CD2DContainerUI::OnMouseUpL2(const D2D1_POINT_2F& point)
{
	auto handler = [](CD2DControlUI* ctrl, const D2D1_POINT_2F& pt) -> bool {
		return ctrl->OnMouseUpL2(pt);
	};
	CD2DControlUI* target = DispatchEvent(point, handler);
	if (target)
		return true;
	return CD2DControlUI::OnMouseUpL2(point);
}

bool CD2DContainerUI::OnMouseDownL(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled())
		return false;
	if (m_scrollBarVisible && m_scrollBar.IsHitTest(point))
	{
		bool handled = m_scrollBar.OnMouseDownL(point);
		m_scrollOffsetY = static_cast<float>(m_scrollBar.GetValue());
		return handled;
	}
	// 分派给子控件
	auto handler = [](CD2DControlUI* ctrl, const D2D1_POINT_2F& pt) -> bool {
		return ctrl->OnMouseDownL(pt);
	};
	CD2DControlUI* target = DispatchEvent(point, handler);
	SetFocusedChild(target);
	if (target)
	{
		m_pCapturedChild = target;    // 捕获此控件

		return true;
	}

	return CD2DControlUI::OnMouseDownL(point);
}

bool CD2DContainerUI::OnMouseMove(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled())
		return false;
	if (m_scrollBarVisible && m_scrollBar.isThumbDragging())
	{
		bool handled = m_scrollBar.OnMouseMove(point);
		m_scrollOffsetY = static_cast<float>(m_scrollBar.GetValue());
		return handled;
	}
	D2D1_POINT_2F contentPt = TransformPoint(point);

	// 有捕获的子控件时，优先发送移动事件给它，忽略悬停切换
	if (m_pCapturedChild)
	{
		if (m_pCapturedChild->IsPressed() || m_pCapturedChild->IsDragging())
			return m_pCapturedChild->OnMouseMove(contentPt);
		else
			m_pCapturedChild = nullptr;   // 子控件状态不正常，放弃捕获
	}

	CAtlArray<CD2DControlUI*> sorted;
	GetSortedChildren(m_children, sorted);   // 获得按 ZOrder 升序的稳定副本

	CD2DControlUI* pNewHover = nullptr;
	if (this->IsHitTest(point))
	{
		for (size_t i = sorted.GetCount(); i-- > 0; )
		{
			CD2DControlUI* pChild = sorted[i];
			if (pChild && pChild->IsVisible() && pChild->IsHitTest(contentPt))
			{
				pNewHover = pChild;
				break;
			}
		}

	}
	
	if (pNewHover != m_pHoverChild)
	{
		if (m_pHoverChild)
			m_pHoverChild->OnMouseLeave(contentPt);
		if (pNewHover)
			pNewHover->OnMouseEnter(contentPt);
		m_pHoverChild = pNewHover;
	}

	if (pNewHover)
		pNewHover->OnMouseMove(contentPt);

	return true;
}

bool CD2DContainerUI::OnMouseUpL(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled())
		return false;
	if (m_scrollBarVisible && m_scrollBar.IsPressed())
	{
		bool handled = m_scrollBar.OnMouseUpL(point);
		m_scrollOffsetY = static_cast<float>(m_scrollBar.GetValue());
		return handled;
	}
	D2D1_POINT_2F contentPt = TransformPoint(point);

	if (m_pCapturedChild)
	{
		bool b = m_pCapturedChild->OnMouseUpL(contentPt);
		m_pCapturedChild = nullptr;
		return b;
	}

	auto handler = [](CD2DControlUI* ctrl, const D2D1_POINT_2F& pt) -> bool {
		return ctrl->OnMouseUpL(pt);
	};
	CD2DControlUI* target = DispatchEvent(point, handler);
	if (target)
		return true;

	return CD2DControlUI::OnMouseUpL(point);
}

bool CD2DContainerUI::OnMouseLeave(const D2D1_POINT_2F& point)
{
	if (m_scrollBarVisible)
		m_scrollBar.OnMouseLeave(point);
	D2D1_POINT_2F contentPt = TransformPoint(point);

	// 鼠标离开容器，强制结束捕获（可模拟抬起）
	if (m_pCapturedChild)
	{
		m_pCapturedChild->OnMouseUpL(contentPt);
		m_pCapturedChild = nullptr;
	}
	if (m_pHoverChild)
	{
		m_pHoverChild->OnMouseLeave(contentPt);
		m_pHoverChild = nullptr;
	}

	if (m_pFocusedChild)
	{
		m_pFocusedChild->OnMouseLeave(contentPt);
		m_pFocusedChild = nullptr;
	}
	

	return CD2DControlUI::OnMouseLeave(point);
}

bool CD2DContainerUI::OnMouseEnter(const D2D1_POINT_2F& point)
{
	return CD2DControlUI::OnMouseEnter(point);
}

bool CD2DContainerUI::OnMouseWheel(float delta, const D2D1_POINT_2F& point)
{
	if(!IsHitTest(point))
		return true;

	// 1. 容器自身滚动条滑块交互（若滚动条可见且鼠标命中滑块）
	if (m_scrollBarVisible && m_scrollBar.IsHitTest(point))
	{
		float step = 30.0f; // 每次滚动的像素
		float newOffset = m_scrollOffsetY - (delta > 0 ? step : -step);
		float maxOffset = max(0.0f, m_contentHeight - GetHeight());
		m_scrollOffsetY = max(0.0f, min(newOffset, maxOffset));
		m_scrollBar.SetValue(static_cast<int>(m_scrollOffsetY));
		return true;
	}

	// 2. 将屏幕坐标转换为内容坐标（考虑垂直滚动偏移）
	D2D1_POINT_2F contentPt = TransformPoint(point);

	// 3. 若有捕获的子控件，优先转发给它
	if (m_pCapturedChild)
	{
		if (m_pCapturedChild->OnMouseWheel(delta, contentPt))
			return true;
		// 捕获时可根据需要决定是否继续向下分发，这里选择直接返回（独占）
		//return true; // 或 false，由你决定
	}

	// 4. 按 Z 序从上往下分发给命中的子控件
	CAtlArray<CD2DControlUI*> sorted;
	GetSortedChildren(m_children, sorted);
	for (size_t i = sorted.GetCount(); i-- > 0; )
	{
		CD2DControlUI* pChild = sorted[i];
		if (pChild && pChild->IsVisible() && pChild->IsHitTest(point))
		{
			if (pChild->OnMouseWheel(delta, contentPt))
				return true;
		}
	}

	// 5. 没有任何子控件处理，且容器自身可滚动时，由容器自身滚动
	if (m_scrollBarVisible)
	{
		float step = 30.0f;
		float newOffset = m_scrollOffsetY - (delta > 0 ? step : -step);
		float maxOffset = max(0.0f, m_contentHeight - GetHeight());
		m_scrollOffsetY = max(0.0f, min(newOffset, maxOffset));
		m_scrollBar.SetValue(static_cast<int>(m_scrollOffsetY));
		return true;
	}

	// 6. 全部未处理，交给基类
	return CD2DControlUI::OnMouseWheel(delta, point);
}

bool CD2DContainerUI::OnChar(DWORD ch)
{
	// 如果有焦点子控件，优先转发给它
	if (m_pFocusedChild)
		return m_pFocusedChild->OnChar(ch);
	return CD2DControlUI::OnChar(ch);
}

bool CD2DContainerUI::OnKeyDown(DWORD keyCode)
{
	if (m_pFocusedChild)
		return m_pFocusedChild->OnKeyDown(keyCode);
	return CD2DControlUI::OnKeyDown(keyCode);
}

bool CD2DContainerUI::OnKeyUp(DWORD keyCode)
{
	if (m_pFocusedChild)
		return m_pFocusedChild->OnKeyUp(keyCode);
	return CD2DControlUI::OnKeyUp(keyCode);
}

CD2DHBoxContainerUI::CD2DHBoxContainerUI()
	: CD2DContainerUI()
	, m_align(CD2DAlignment::HLeft, CD2DAlignment::VTop)  // 默认主轴靠左，交叉轴拉伸
	, m_spacing(5.0f)
	, m_expandWeight(1.0f)
{
}

CD2DHBoxContainerUI::~CD2DHBoxContainerUI()
{
}

D2D1_SIZE_F CD2DHBoxContainerUI::MeasureContent(CD2DRender* pRender, float maxWidth, float maxHeight) const
{
	float totalVisW = 0.0f;
	float maxVisH = 0.0f;
	int visCount = 0;

	for (size_t i = 0; i < GetChildCount(); ++i)
	{
		CD2DControlUI* pChild = GetChildAt(i);
		if (!pChild || !pChild->IsVisible())
			continue;

		D2D1_SIZE_F contentSize = pChild->GetSize();
		CD2DStyle style = pChild->CalculateEffectiveStyle();

		float hDecor = style.margin.left + style.margin.right
			+ style.border.m_left.m_width + style.border.m_right.m_width
			+ style.padding.left + style.padding.right;
		float vDecor = style.margin.top + style.margin.bottom
			+ style.border.m_top.m_width + style.border.m_bottom.m_width
			+ style.padding.top + style.padding.bottom;

		float visW = contentSize.width + hDecor;
		float visH = contentSize.height + vDecor;

		totalVisW += visW;
		if (visH > maxVisH) maxVisH = visH;
		visCount++;
	}

	if (visCount == 0)
		return D2D1::SizeF(0.0f, 0.0f);

	float totalW = totalVisW + (visCount + 1) * m_spacing;
	return D2D1::SizeF(totalW, maxVisH);
}

void CD2DHBoxContainerUI::DrawControl(CD2DRender* pRender)
{
	Layout(pRender);
	CD2DContainerUI::DrawControl(pRender);
}

void CD2DHBoxContainerUI::Layout1(CD2DRender* pRender)
{
	if (GetChildCount() == 0) return;
	if (!pRender) return;

	D2D1_RECT_F content = GetContentRectangle();
	float availW = content.right - content.left;
	float availH = content.bottom - content.top;

	// ⭐ 关键修复：检测容器是否需要自动测量宽度
	bool bNeedMeasure = (GetWidthPolicy().mode == CD2DSizePolicy::Auto) && (availW <= 0.0f);

	if (bNeedMeasure)
	{
		// 使用虚拟的大宽度进行测量
		if (availH <= 0.0f)
			availH = 600.0f;  // 默认高度
		availW = 10000.0f;
	}
	else if (availW <= 0.0f || availH <= 0.0f)
		return;

	struct StaticInfo
	{
		CD2DControlUI* child;
		float contentW, contentH;
		float visW, visH;
		CD2DSizePolicy widthPolicy;
		CD2DSizePolicy heightPolicy;
		float leftDec, rightDec, topDec, botDec;
	};

	CAtlArray<StaticInfo> fixedItems;
	CAtlArray<StaticInfo> flexItems;
	CAtlArray<CD2DControlUI*> absoluteChildren;

	// ========== 第一遍：计算所有控件的大小 ==========
	for (size_t i = 0; i < GetChildCount(); ++i)
	{
		CD2DControlUI* pChild = GetChildAt(i);
		if (!pChild || !pChild->IsVisible()) continue;

		if (pChild->GetPositionType() == CD2DPosition::Absolute)
		{
			absoluteChildren.Add(pChild);
			continue;
		}

		const CD2DSizePolicy& wp = pChild->GetWidthPolicy();
		const CD2DSizePolicy& hp = pChild->GetHeightPolicy();
		CD2DStyle style = pChild->CalculateEffectiveStyle();

		float leftDec  = style.margin.left + style.border.m_left.m_width + style.padding.left;
		float rightDec = style.margin.right + style.border.m_right.m_width + style.padding.right;
		float topDec   = style.margin.top + style.border.m_top.m_width + style.padding.top;
		float botDec   = style.margin.bottom + style.border.m_bottom.m_width + style.padding.bottom;

		// ---- 高度处理（交叉轴 Y）----
		float contentH = 0.0f;
		switch (hp.mode)
		{
		case CD2DSizePolicy::Auto:
			{
				float contentAvailH = max(0.0f, availH - topDec - botDec);
				pChild->SizeToContent(pRender, contentAvailH);
				contentH = pChild->GetHeight();
				break;
			}
		case CD2DSizePolicy::Fixed:
			contentH = (hp.value > 0.0f) ? hp.value : pChild->GetHeight();
			break;
		case CD2DSizePolicy::Percent:
			contentH = max(0.0f, availH - topDec - botDec) * (hp.value / 100.0f);
			break;
		case CD2DSizePolicy::Fill:
		case CD2DSizePolicy::Expand:
			contentH = max(0.0f, availH - topDec - botDec);
			break;
		}
		contentH = max(hp.minValue, min(contentH, hp.maxValue));
		pChild->SetHeight(contentH);

		// ---- 宽度处理（主轴 X）----
		float contentW = 0.0f;
		switch (wp.mode)
		{
		case CD2DSizePolicy::Auto:
			{
				// ⭐ 关键：使用已计算的内容高度进行测量
				pChild->SizeToContent(pRender, contentH);
				contentW = pChild->GetWidth();
				break;
			}
		case CD2DSizePolicy::Fixed:
			contentW = (wp.value > 0.0f) ? wp.value : pChild->GetWidth();
			break;
		case CD2DSizePolicy::Percent:
			contentW = max(0.0f, availW - leftDec - rightDec) * (wp.value / 100.0f);
			break;
		case CD2DSizePolicy::Fill:
		case CD2DSizePolicy::Expand:
			contentW = 0.0f;  // 弹性分配，稍后计算
			break;
		}

		if (wp.mode != CD2DSizePolicy::Fill && wp.mode != CD2DSizePolicy::Expand)
		{
			contentW = max(wp.minValue, min(contentW, wp.maxValue));
		}
		pChild->SetWidth(contentW);

		float visW = contentW + leftDec + rightDec;
		float visH = contentH + topDec + botDec;

		StaticInfo info;
		info.child = pChild;
		info.contentW = contentW;
		info.contentH = contentH;
		info.visW = visW;
		info.visH = visH;
		info.widthPolicy = wp;
		info.heightPolicy = hp;
		info.leftDec = leftDec;
		info.rightDec = rightDec;
		info.topDec = topDec;
		info.botDec = botDec;

		if (wp.mode == CD2DSizePolicy::Fixed || wp.mode == CD2DSizePolicy::Auto || wp.mode == CD2DSizePolicy::Percent)
			fixedItems.Add(info);
		else
			flexItems.Add(info);
	}

	// 计算固定宽度总视觉宽度
	float totalFixedVisW = 0.0f;
	for (size_t i = 0; i < fixedItems.GetCount(); ++i)
		totalFixedVisW += fixedItems[i].visW;

	size_t totalStaticCount = fixedItems.GetCount() + flexItems.GetCount();

	// ⭐ 如果是测量模式，现在计算并设置容器的实际宽度
	if (bNeedMeasure)
	{
		CD2DStyle containerStyle = CalculateEffectiveStyle();
		float containerLeftDec = containerStyle.margin.left + containerStyle.border.m_left.m_width + containerStyle.padding.left;
		float containerRightDec = containerStyle.margin.right + containerStyle.border.m_right.m_width + containerStyle.padding.right;

		float totalSpacing = (totalStaticCount > 0) ? (totalStaticCount - 1) * m_spacing : 0.0f;

		// ⭐ 计算容器需要的总宽度
		float measuredW = 0.0f;

		if (totalStaticCount > 0)
		{
			// 固定元素宽度 + 弹性元素宽度（此时为0）+ 间距 + 容器装饰
			measuredW = totalFixedVisW + totalSpacing + containerLeftDec + containerRightDec;
		}
		else
		{
			// 只有装饰
			measuredW = containerLeftDec + containerRightDec;
		}

		// ⭐ 必须执行：设置容器的实际宽度
		SetWidth(measuredW);

		// ⭐ 重新获取容器内容矩形
		content = GetContentRectangle();
		availW = content.right - content.left;

		bNeedMeasure = false;
	}

	if (totalStaticCount == 0)
	{
		// 处理绝对定位控件
		for (size_t i = 0; i < absoluteChildren.GetCount(); ++i)
		{
			CD2DControlUI* pChild = absoluteChildren[i];
			const CD2DSizePolicy& wp = pChild->GetWidthPolicy();
			const CD2DSizePolicy& hp = pChild->GetHeightPolicy();
			CD2DStyle style = pChild->CalculateEffectiveStyle();

			float leftDec  = style.margin.left + style.border.m_left.m_width + style.padding.left;
			float rightDec = style.margin.right + style.border.m_right.m_width + style.padding.right;
			float topDec   = style.margin.top + style.border.m_top.m_width + style.padding.top;
			float botDec   = style.margin.bottom + style.border.m_bottom.m_width + style.padding.bottom;

			float cw, ch;

			if (wp.mode == CD2DSizePolicy::Auto)
			{
				float contentAvailW = max(0.0f, availW - leftDec - rightDec);
				pChild->SizeToContent(pRender, contentAvailW);
				cw = pChild->GetWidth();
			}
			else if (wp.mode == CD2DSizePolicy::Fixed)
				cw = (wp.value > 0.0f) ? wp.value : pChild->GetWidth();
			else if (wp.mode == CD2DSizePolicy::Percent)
				cw = availW * (wp.value / 100.0f);
			else
				cw = pChild->GetWidth();

			cw = max(wp.minValue, min(cw, wp.maxValue));

			if (hp.mode == CD2DSizePolicy::Auto)
			{
				pChild->SizeToContent(pRender, cw);
				ch = pChild->GetHeight();
			}
			else if (hp.mode == CD2DSizePolicy::Fixed)
				ch = (hp.value > 0.0f) ? hp.value : pChild->GetHeight();
			else if (hp.mode == CD2DSizePolicy::Percent)
				ch = availH * (hp.value / 100.0f);
			else
				ch = pChild->GetHeight();

			ch = max(hp.minValue, min(ch, hp.maxValue));

			CD2DPosition pos = pChild->GetPositionInfo();
			pChild->SetPosition(content.left + pos.left, content.top + pos.top);
			pChild->SetSize(cw, ch);
		}
		return;
	}

	// 在弹性分配之前，计算弹性控件的装饰总宽度
	float flexHorizDec = 0.0f;
	for (size_t i = 0; i < flexItems.GetCount(); ++i)
		flexHorizDec += (flexItems[i].leftDec + flexItems[i].rightDec);

	float totalSpacing = (totalStaticCount - 1) * m_spacing;

	// 剩余宽度 = 可用宽度 - 固定控件视觉宽度 - 间距 - 弹性控件装饰
	float remainingW = availW - totalFixedVisW - totalSpacing - flexHorizDec;
	if (remainingW < 0.0f) remainingW = 0.0f;

	// ========== 弹性分配（Fill / Expand）==========
	float totalExpandWeight = 0.0f;
	int fillCount = 0;
	for (size_t i = 0; i < flexItems.GetCount(); ++i)
	{
		if (flexItems[i].widthPolicy.mode == CD2DSizePolicy::Expand)
			totalExpandWeight += flexItems[i].widthPolicy.value;
		else if (flexItems[i].widthPolicy.mode == CD2DSizePolicy::Fill)
			fillCount++;
	}

	for (size_t i = 0; i < flexItems.GetCount(); ++i)
	{
		StaticInfo& item = flexItems[i];
		float allocW = 0.0f;
		if (item.widthPolicy.mode == CD2DSizePolicy::Expand)
		{
			if (totalExpandWeight > 0.0f)
				allocW = remainingW * (item.widthPolicy.value / totalExpandWeight);
		}
		else if (item.widthPolicy.mode == CD2DSizePolicy::Fill)
		{
			if (fillCount > 0)
				allocW = remainingW / fillCount;
		}
		allocW = max(item.widthPolicy.minValue, min(allocW, item.widthPolicy.maxValue));
		item.contentW = allocW;
		item.child->SetWidth(allocW);
		item.visW = allocW + item.leftDec + item.rightDec;
	}

	// 合并固定和弹性控件，按原始顺序
	CAtlArray<StaticInfo*> orderedItems;
	for (size_t i = 0; i < GetChildCount(); ++i)
	{
		CD2DControlUI* pChild = GetChildAt(i);
		if (!pChild || !pChild->IsVisible() || pChild->GetPositionType() == CD2DPosition::Absolute)
			continue;

		StaticInfo* found = nullptr;
		for (size_t j = 0; j < fixedItems.GetCount(); ++j)
		{
			if (fixedItems[j].child == pChild)
			{
				found = &fixedItems[j];
				break;
			}
		}
		if (!found)
		{
			for (size_t j = 0; j < flexItems.GetCount(); ++j)
			{
				if (flexItems[j].child == pChild)
				{
					found = &flexItems[j];
					break;
				}
			}
		}
		if (found)
			orderedItems.Add(found);
	}

	// 计算总视觉宽度
	float totalVisW = 0.0f;
	for (size_t i = 0; i < orderedItems.GetCount(); ++i)
		totalVisW += orderedItems[i]->visW;

	// 主轴起始位置（水平）
	float xVis = content.left;
	switch (m_align.horz)
	{
	case CD2DAlignment::HLeft:    
		xVis = content.left; 
		break;
	case CD2DAlignment::HCenter: 
		xVis = content.left + (availW - totalVisW) * 0.5f; 
		break;
	case CD2DAlignment::HRight:  
		xVis = content.right - totalVisW; 
		break;
	}

	// ========== 第二遍：设置位置和高度（交叉轴）==========
	for (size_t i = 0; i < orderedItems.GetCount(); ++i)
	{
		StaticInfo* item = orderedItems[i];
		CD2DControlUI* pChild = item->child;

		// 交叉轴垂直位置
		float yVis = content.top;
		switch (m_align.vert)
		{
		case CD2DAlignment::VTop:    
			yVis = content.top; 
			break;
		case CD2DAlignment::VCenter: 
			yVis = content.top + (availH - item->visH) * 0.5f; 
			break;
		case CD2DAlignment::VBottom: 
			yVis = content.bottom - item->visH; 
			break;
		}

		// ⭐ 位置计算：视觉位置 + 装饰 = 内容位置
		float x = xVis + item->leftDec;
		float y = yVis + item->topDec;

		pChild->SetPosition(x, y);
		// 内容尺寸已在第一遍设置

		xVis += item->visW + m_spacing;
	}

	// 处理绝对定位控件
	for (size_t i = 0; i < absoluteChildren.GetCount(); ++i)
	{
		CD2DControlUI* pChild = absoluteChildren[i];
		const CD2DSizePolicy& wp = pChild->GetWidthPolicy();
		const CD2DSizePolicy& hp = pChild->GetHeightPolicy();
		CD2DStyle style = pChild->CalculateEffectiveStyle();

		float leftDec  = style.margin.left + style.border.m_left.m_width + style.padding.left;
		float rightDec = style.margin.right + style.border.m_right.m_width + style.padding.right;
		float topDec   = style.margin.top + style.border.m_top.m_width + style.padding.top;
		float botDec   = style.margin.bottom + style.border.m_bottom.m_width + style.padding.bottom;

		float cw, ch;

		if (wp.mode == CD2DSizePolicy::Auto)
		{
			float contentAvailW = max(0.0f, availW - leftDec - rightDec);
			pChild->SizeToContent(pRender, contentAvailW);
			cw = pChild->GetWidth();
		}
		else if (wp.mode == CD2DSizePolicy::Fixed)
			cw = (wp.value > 0.0f) ? wp.value : pChild->GetWidth();
		else if (wp.mode == CD2DSizePolicy::Percent)
			cw = availW * (wp.value / 100.0f);
		else
			cw = pChild->GetWidth();

		cw = max(wp.minValue, min(cw, wp.maxValue));

		if (hp.mode == CD2DSizePolicy::Auto)
		{
			pChild->SizeToContent(pRender, cw);
			ch = pChild->GetHeight();
		}
		else if (hp.mode == CD2DSizePolicy::Fixed)
			ch = (hp.value > 0.0f) ? hp.value : pChild->GetHeight();
		else if (hp.mode == CD2DSizePolicy::Percent)
			ch = availH * (hp.value / 100.0f);
		else
			ch = pChild->GetHeight();

		ch = max(hp.minValue, min(ch, hp.maxValue));

		CD2DPosition pos = pChild->GetPositionInfo();
		pChild->SetPosition(content.left + pos.left, content.top + pos.top);
		pChild->SetSize(cw, ch);
	}

	
	
}

void CD2DHBoxContainerUI::Layout(CD2DRender* pRender)
{
	Layout1(pRender);
	// 处理溢出和滚动条
	m_contentHeight = MeasureContent(pRender).height;
	UpdateScrollBar(this->GetSize().height);
}

CD2DVBoxContainerUI::CD2DVBoxContainerUI()
	: m_sizePolicy(CD2DSizePolicy::Fixed)
	, m_expandWeight(1.0f)
	, m_spacing(5.0f)
{
}

CD2DVBoxContainerUI::~CD2DVBoxContainerUI()
{

}

D2D1_SIZE_F CD2DVBoxContainerUI::MeasureContent(CD2DRender* pRender, float maxWidth, float maxHeight) const
{
	float totalVisH = 0.0f;
	float maxVisW = 0.0f;
	int visCount = 0;

	for (size_t i = 0; i < GetChildCount(); ++i)
	{
		CD2DControlUI* pChild = GetChildAt(i);
		if (!pChild || !pChild->IsVisible() )
			continue;

		D2D1_SIZE_F contentSize = pChild->GetSize();
		CD2DStyle style = pChild->CalculateEffectiveStyle();

		float vDecor = style.margin.top + style.margin.bottom
			+ style.border.m_top.m_width + style.border.m_bottom.m_width
			+ style.padding.top + style.padding.bottom;
		float hDecor = style.margin.left + style.margin.right
			+ style.border.m_left.m_width + style.border.m_right.m_width
			+ style.padding.left + style.padding.right;

		float visH = contentSize.height + vDecor;
		float visW = contentSize.width + hDecor;

		totalVisH += visH;
		if (visW > maxVisW) maxVisW = visW;
		visCount++;
	}

	if (visCount == 0)
		return D2D1::SizeF(0.0f, 0.0f);

	// 总高度 = 所有视觉高度 + 间距（子控件之间 + 首尾与容器边缘）
	float totalH = totalVisH + (visCount + 1) * m_spacing;
	return D2D1::SizeF(maxVisW, totalH);
}

void CD2DVBoxContainerUI::Layout1(CD2DRender* pRender)
{
	if (GetChildCount() == 0) return;
	if (!pRender) return;

	D2D1_RECT_F content = GetContentRectangle();
	float availW = content.right - content.left;
	float availH = content.bottom - content.top;

	// ⭐ 第一次检查：如果容器自身是 Auto 且尺寸未设置
	bool bNeedMeasure = (GetHeightPolicy().mode == CD2DSizePolicy::Auto) && (availH <= 0.0f);

	if (bNeedMeasure && availW > 0.0f)
	{
		// 第一阶段：测量模式（使用一个虚拟的大高度）
		availH = 10000.0f;  // 足够大的虚拟高度
	}
	else if (availW <= 0.0f || availH <= 0.0f)
		return;

	struct StaticInfo
	{
		CD2DControlUI* child;
		float contentW, contentH;
		float visW, visH;
		CD2DSizePolicy widthPolicy;
		CD2DSizePolicy heightPolicy;
		float leftDec, rightDec, topDec, botDec;
	};

	CAtlArray<StaticInfo> fixedItems;
	CAtlArray<StaticInfo> flexItems;
	CAtlArray<CD2DControlUI*> absoluteChildren;

	// ========== 第一遍：计算所有控件的大小 ==========
	for (size_t i = 0; i < GetChildCount(); ++i)
	{
		CD2DControlUI* pChild = GetChildAt(i);
		if (!pChild || !pChild->IsVisible()) continue;

		if (pChild->GetPositionType() == CD2DPosition::Absolute)
		{
			absoluteChildren.Add(pChild);
			continue;
		}

		const CD2DSizePolicy& wp = pChild->GetWidthPolicy();
		const CD2DSizePolicy& hp = pChild->GetHeightPolicy();
		CD2DStyle style = pChild->CalculateEffectiveStyle();

		float leftDec  = style.margin.left + style.border.m_left.m_width + style.padding.left;
		float rightDec = style.margin.right + style.border.m_right.m_width + style.padding.right;
		float topDec   = style.margin.top + style.border.m_top.m_width + style.padding.top;
		float botDec   = style.margin.bottom + style.border.m_bottom.m_width + style.padding.bottom;

		// ---- 宽度处理（交叉轴 X）----
		float contentW = 0.0f;
		switch (wp.mode)
		{
		case CD2DSizePolicy::Auto:
			{
				float contentAvailW = max(0.0f, availW - leftDec - rightDec);
				pChild->SizeToContent(pRender, contentAvailW);
				contentW = pChild->GetWidth();
				break;
			}
		case CD2DSizePolicy::Fixed:
			contentW = (wp.value > 0.0f) ? wp.value : pChild->GetWidth();
			break;
		case CD2DSizePolicy::Percent:
			contentW = max(0.0f, availW - leftDec - rightDec) * (wp.value / 100.0f);
			break;
		case CD2DSizePolicy::Fill:
		case CD2DSizePolicy::Expand:
			contentW = max(0.0f, availW - leftDec - rightDec);
			break;
		}
		contentW = max(wp.minValue, min(contentW, wp.maxValue));
		pChild->SetWidth(contentW);

		// ---- 高度处理（主轴 Y）----
		float contentH = 0.0f;
		switch (hp.mode)
		{
		case CD2DSizePolicy::Auto:
			{
				// ⭐ 关键：使用已计算的内容宽度进行测量
				pChild->SizeToContent(pRender, contentW);
				contentH = pChild->GetHeight();
				break;
			}
		case CD2DSizePolicy::Fixed:
			contentH = (hp.value > 0.0f) ? hp.value : pChild->GetHeight();
			break;
		case CD2DSizePolicy::Percent:
			contentH = max(0.0f, availH - topDec - botDec) * (hp.value / 100.0f);
			break;
		case CD2DSizePolicy::Fill:
		case CD2DSizePolicy::Expand:
			contentH = 0.0f;  // 弹性分配，稍后计算
			break;
		}

		if (hp.mode != CD2DSizePolicy::Fill && hp.mode != CD2DSizePolicy::Expand)
		{
			contentH = max(hp.minValue, min(contentH, hp.maxValue));
		}
		pChild->SetHeight(contentH);

		float visW = contentW + leftDec + rightDec;
		float visH = contentH + topDec + botDec;

		StaticInfo info;
		info.child = pChild;
		info.contentW = contentW;
		info.contentH = contentH;
		info.visW = visW;
		info.visH = visH;
		info.widthPolicy = wp;
		info.heightPolicy = hp;
		info.leftDec = leftDec;
		info.rightDec = rightDec;
		info.topDec = topDec;
		info.botDec = botDec;

		if (hp.mode == CD2DSizePolicy::Fixed || hp.mode == CD2DSizePolicy::Auto || hp.mode == CD2DSizePolicy::Percent)
			fixedItems.Add(info);
		else
			flexItems.Add(info);
	}

	// 计算固定高度总视觉高度
	float totalFixedVisH = 0.0f;
	for (size_t i = 0; i < fixedItems.GetCount(); ++i)
		totalFixedVisH += fixedItems[i].visH;

	size_t totalStaticCount = fixedItems.GetCount() + flexItems.GetCount();

	// ⭐ 如果是测量模式，计算容器实际需要的高度
	if (bNeedMeasure && totalStaticCount > 0)
	{
		CD2DStyle containerStyle = CalculateEffectiveStyle();
		float containerTopDec = containerStyle.margin.top + containerStyle.border.m_top.m_width + containerStyle.padding.top;
		float containerBotDec = containerStyle.margin.bottom + containerStyle.border.m_bottom.m_width + containerStyle.padding.bottom;

		float totalSpacing = (totalStaticCount - 1) * m_spacing;
		float measuredH = totalFixedVisH + totalSpacing + containerTopDec + containerBotDec;

		// 设置容器的实际高度
		SetHeight(measuredH);

		// 重新计算 content rectangle
		content = GetContentRectangle();
		availH = content.bottom - content.top;

		bNeedMeasure = false;  // 标记已测量
	}

	if (totalStaticCount == 0)
	{
		// 处理绝对定位控件（只有绝对定位控件的情况）
		for (size_t i = 0; i < absoluteChildren.GetCount(); ++i)
		{
			CD2DControlUI* pChild = absoluteChildren[i];
			const CD2DSizePolicy& wp = pChild->GetWidthPolicy();
			const CD2DSizePolicy& hp = pChild->GetHeightPolicy();
			CD2DStyle style = pChild->CalculateEffectiveStyle();

			float leftDec  = style.margin.left + style.border.m_left.m_width + style.padding.left;
			float rightDec = style.margin.right + style.border.m_right.m_width + style.padding.right;
			float topDec   = style.margin.top + style.border.m_top.m_width + style.padding.top;
			float botDec   = style.margin.bottom + style.border.m_bottom.m_width + style.padding.bottom;

			float cw, ch;

			if (wp.mode == CD2DSizePolicy::Auto) 
			{ 
				float contentAvailW = max(0.0f, availW - leftDec - rightDec);
				pChild->SizeToContent(pRender, contentAvailW); 
				cw = pChild->GetWidth(); 
			}
			else if (wp.mode == CD2DSizePolicy::Fixed) 
				cw = (wp.value > 0.0f) ? wp.value : pChild->GetWidth();
			else if (wp.mode == CD2DSizePolicy::Percent)
				cw = availW * (wp.value / 100.0f);
			else 
				cw = pChild->GetWidth();

			cw = max(wp.minValue, min(cw, wp.maxValue));

			if (hp.mode == CD2DSizePolicy::Auto) 
			{ 
				pChild->SizeToContent(pRender, cw); 
				ch = pChild->GetHeight(); 
			}
			else if (hp.mode == CD2DSizePolicy::Fixed) 
				ch = (hp.value > 0.0f) ? hp.value : pChild->GetHeight();
			else if (hp.mode == CD2DSizePolicy::Percent)
				ch = availH * (hp.value / 100.0f);
			else 
				ch = pChild->GetHeight();

			ch = max(hp.minValue, min(ch, hp.maxValue));

			CD2DPosition pos = pChild->GetPositionInfo();
			pChild->SetPosition(content.left + pos.left, content.top + pos.top);
			pChild->SetSize(cw, ch);
		}
		return;
	}

	float totalSpacing = (totalStaticCount - 1) * m_spacing;
	float flexVertDec = 0.0f;
	for (size_t i = 0; i < flexItems.GetCount(); ++i)
		flexVertDec += (flexItems[i].topDec + flexItems[i].botDec);

	float remainingH = availH - totalFixedVisH - totalSpacing - flexVertDec;
	if (remainingH < 0.0f) remainingH = 0.0f;

	// ========== 弹性分配（Fill / Expand）==========
	float totalExpandWeight = 0.0f;
	int fillCount = 0;
	for (size_t i = 0; i < flexItems.GetCount(); ++i)
	{
		if (flexItems[i].heightPolicy.mode == CD2DSizePolicy::Expand)
			totalExpandWeight += flexItems[i].heightPolicy.value;
		else if (flexItems[i].heightPolicy.mode == CD2DSizePolicy::Fill)
			fillCount++;
	}

	for (size_t i = 0; i < flexItems.GetCount(); ++i)
	{
		StaticInfo& item = flexItems[i];
		float allocH = 0.0f;
		if (item.heightPolicy.mode == CD2DSizePolicy::Expand)
		{
			if (totalExpandWeight > 0.0f)
				allocH = remainingH * (item.heightPolicy.value / totalExpandWeight);
		}
		else if (item.heightPolicy.mode == CD2DSizePolicy::Fill)
		{
			if (fillCount > 0)
				allocH = remainingH / fillCount;
		}
		allocH = max(item.heightPolicy.minValue, min(allocH, item.heightPolicy.maxValue));
		item.contentH = allocH;
		item.child->SetHeight(allocH);
		item.visH = allocH + item.topDec + item.botDec;
	}

	// 合并固定和弹性控件，按原始顺序
	CAtlArray<StaticInfo*> orderedItems;
	for (size_t i = 0; i < GetChildCount(); ++i)
	{
		CD2DControlUI* pChild = GetChildAt(i);
		if (!pChild)
			continue;

		StaticInfo* found = nullptr;
		for (size_t j = 0; j < fixedItems.GetCount(); ++j)
		{
			if (fixedItems[j].child == pChild) 
			{ 
				found = &fixedItems[j]; 
				break; 
			}
		}
		if (!found)
		{
			for (size_t j = 0; j < flexItems.GetCount(); ++j)
			{
				if (flexItems[j].child == pChild) 
				{ 
					found = &flexItems[j]; 
					break; 
				}
			}
		}
		if (found) 
			orderedItems.Add(found);
	}

	// 计算总视觉高度
	float totalVisH = 0.0f;
	for (size_t i = 0; i < orderedItems.GetCount(); ++i)
		totalVisH += orderedItems[i]->visH;

	// 主轴起始位置（垂直）
	float yVis = content.top;
	switch (m_align.vert)
	{
	case CD2DAlignment::VTop:    
		yVis = content.top; 
		break;
	case CD2DAlignment::VCenter: 
		yVis = content.top + (availH - totalVisH) * 0.5f; 
		break;
	case CD2DAlignment::VBottom: 
		yVis = content.bottom - totalVisH; 
		break;
	}

	// ========== 第二遍：设置位置和宽度（交叉轴）==========
	for (size_t i = 0; i < orderedItems.GetCount(); ++i)
	{
		StaticInfo* item = orderedItems[i];
		CD2DControlUI* pChild = item->child;

		// 交叉轴水平位置
		float xVis = content.left;
		switch (m_align.horz)
		{
		case CD2DAlignment::HLeft:   
			xVis = content.left; 
			break;
		case CD2DAlignment::HCenter: 
			xVis = content.left + (availW - item->visW) * 0.5f; 
			break;
		case CD2DAlignment::HRight:  
			xVis = content.right - item->visW; 
			break;
		}

		// ⭐ 位置计算：视觉位置 + 装饰 = 内容位置
		float x = xVis + item->leftDec;
		float y = yVis + item->topDec;

		pChild->SetPosition(x, y);
		// 内容尺寸已在第一遍设置

		yVis += item->visH + m_spacing;
	}

	// 处理绝对定位控件
	for (size_t i = 0; i < absoluteChildren.GetCount(); ++i)
	{
		CD2DControlUI* pChild = absoluteChildren[i];
		const CD2DSizePolicy& wp = pChild->GetWidthPolicy();
		const CD2DSizePolicy& hp = pChild->GetHeightPolicy();
		CD2DStyle style = pChild->CalculateEffectiveStyle();

		float leftDec  = style.margin.left + style.border.m_left.m_width + style.padding.left;
		float rightDec = style.margin.right + style.border.m_right.m_width + style.padding.right;
		float topDec   = style.margin.top + style.border.m_top.m_width + style.padding.top;
		float botDec   = style.margin.bottom + style.border.m_bottom.m_width + style.padding.bottom;

		float cw, ch;

		if (wp.mode == CD2DSizePolicy::Auto) 
		{ 
			float contentAvailW = max(0.0f, availW - leftDec - rightDec);
			pChild->SizeToContent(pRender, contentAvailW); 
			cw = pChild->GetWidth(); 
		}
		else if (wp.mode == CD2DSizePolicy::Fixed) 
			cw = (wp.value > 0.0f) ? wp.value : pChild->GetWidth();
		else if (wp.mode == CD2DSizePolicy::Percent)
			cw = availW * (wp.value / 100.0f);
		else 
			cw = pChild->GetWidth();

		cw = max(wp.minValue, min(cw, wp.maxValue));

		if (hp.mode == CD2DSizePolicy::Auto) 
		{ 
			pChild->SizeToContent(pRender, cw); 
			ch = pChild->GetHeight(); 
		}
		else if (hp.mode == CD2DSizePolicy::Fixed) 
			ch = (hp.value > 0.0f) ? hp.value : pChild->GetHeight();
		else if (hp.mode == CD2DSizePolicy::Percent)
			ch = availH * (hp.value / 100.0f);
		else 
			ch = pChild->GetHeight();

		ch = max(hp.minValue, min(ch, hp.maxValue));

		CD2DPosition pos = pChild->GetPositionInfo();
		pChild->SetPosition(content.left + pos.left, content.top + pos.top);
		pChild->SetSize(cw, ch);
	}
	
}

void CD2DVBoxContainerUI::Layout(CD2DRender* pRender)
{
	Layout1(pRender);
	// 处理溢出和滚动条
	m_contentHeight = MeasureContent(pRender).height;
	UpdateScrollBar(this->GetSize().height);
}

void CD2DGridContainerUI::Layout(CD2DRender* pRender)
{
	if (GetChildCount() == 0) return;
	if (!pRender) return;

	D2D1_RECT_F content = GetContentRectangle();
	float availW = content.right - content.left;
	float availH = content.bottom - content.top;
	if (availW <= 0.0f || availH <= 0.0f) return;

	// 收集 Static 控件（绝对定位先暂存）
	CAtlArray<CD2DControlUI*> staticChildren;
	CAtlArray<CD2DControlUI*> absoluteChildren;

	for (size_t i = 0; i < GetChildCount(); ++i)
	{
		CD2DControlUI* pChild = GetChildAt(i);
		if (!pChild || !pChild->IsVisible()) continue;
		if (pChild->GetPositionType() == CD2DPosition::Absolute)
			absoluteChildren.Add(pChild);
		else
			staticChildren.Add(pChild);
	}

	int count = (int)staticChildren.GetCount();
	if (count == 0)
	{
		// 只处理绝对定位
		for (size_t i = 0; i < absoluteChildren.GetCount(); ++i)
		{
			CD2DControlUI* pChild = absoluteChildren[i];
			// 绝对定位处理（同前）
			const CD2DSizePolicy& wp = pChild->GetWidthPolicy();
			const CD2DSizePolicy& hp = pChild->GetHeightPolicy();
			float cw, ch;
			if (wp.mode == CD2DSizePolicy::Auto) { pChild->SizeToContent(pRender, availW); cw = pChild->GetWidth(); }
			else if (wp.mode == CD2DSizePolicy::Fixed) cw = (wp.value > 0.0f) ? wp.value : pChild->GetWidth();
			else cw = pChild->GetWidth();
			if (hp.mode == CD2DSizePolicy::Auto) { pChild->SizeToContent(pRender, cw); ch = pChild->GetHeight(); }
			else if (hp.mode == CD2DSizePolicy::Fixed) ch = (hp.value > 0.0f) ? hp.value : pChild->GetHeight();
			else ch = pChild->GetHeight();
			CD2DPosition pos = pChild->GetPositionInfo();
			pChild->SetPosition(content.left + pos.left, content.top + pos.top);
			pChild->SetSize(cw, ch);
		}
		return;
	}

	// 计算行列数
	int rows = m_rows, cols = m_cols;
	if (rows <= 0 && cols <= 0) {
		cols = 1;
		rows = (count + cols - 1) / cols;
	} else if (rows > 0 && cols <= 0) {
		cols = (count + rows - 1) / rows;
	} else if (cols > 0 && rows <= 0) {
		rows = (count + cols - 1) / cols;
	}

	// 单元格大小
	float cellW = (availW - (cols - 1) * m_colSpacing) / cols;
	float cellH = (availH - (rows - 1) * m_rowSpacing) / rows;
	if (cellW < 0.0f) cellW = 0.0f;
	if (cellH < 0.0f) cellH = 0.0f;

	// 遍历子控件，按先行后列排列
	for (int idx = 0; idx < count; ++idx)
	{
		CD2DControlUI* pChild = staticChildren[idx];
		CD2DStyle style = pChild->CalculateEffectiveStyle();
		const CD2DSizePolicy& wp = pChild->GetWidthPolicy();
		const CD2DSizePolicy& hp = pChild->GetHeightPolicy();

		// 内容大小初始值
		float contentW, contentH;

		// 宽度处理（受单元格宽度影响）
		if (wp.mode == CD2DSizePolicy::Auto)
		{
			pChild->SizeToContent(pRender, cellW);
			contentW = pChild->GetWidth();
		}
		else if (wp.mode == CD2DSizePolicy::Fixed)
		{
			contentW = (wp.value > 0.0f) ? wp.value : pChild->GetWidth();
		}
		else if (wp.mode == CD2DSizePolicy::Percent)
		{
			contentW = cellW * (wp.value / 100.0f);
		}
		else // Fill/Expand 在网格中通常拉伸至单元格宽度（或按权重分配，但网格没有剩余空间分配，所以直接拉伸）
		{
			contentW = cellW; // 简单拉伸
		}
		contentW = max(wp.minValue, min(contentW, wp.maxValue));

		// 高度处理（类似）
		if (hp.mode == CD2DSizePolicy::Auto)
		{
			pChild->SizeToContent(pRender, contentW);
			contentH = pChild->GetHeight();
		}
		else if (hp.mode == CD2DSizePolicy::Fixed)
		{
			contentH = (hp.value > 0.0f) ? hp.value : pChild->GetHeight();
		}
		else if (hp.mode == CD2DSizePolicy::Percent)
		{
			contentH = cellH * (hp.value / 100.0f);
		}
		else
		{
			contentH = cellH;
		}
		contentH = max(hp.minValue, min(contentH, hp.maxValue));

		// 装饰
		float leftDec  = style.margin.left + style.border.m_left.m_width + style.padding.left;
		float rightDec = style.margin.right + style.border.m_right.m_width + style.padding.right;
		float topDec   = style.margin.top + style.border.m_top.m_width + style.padding.top;
		float botDec   = style.margin.bottom + style.border.m_bottom.m_width + style.padding.bottom;

		float visW = contentW + leftDec + rightDec;
		float visH = contentH + topDec + botDec;

		// 当前单元格行列
		int r = idx / cols;
		int c = idx % cols;

		float cellLeft = content.left + c * (cellW + m_colSpacing);
		float cellTop  = content.top + r * (cellH + m_rowSpacing);

		// 单元格内对齐（视觉矩形对齐）
		float visLeft = cellLeft;
		switch (m_cellAlign.horz)
		{
		case CD2DAlignment::HLeft:   visLeft = cellLeft; break;
		case CD2DAlignment::HCenter: visLeft = cellLeft + (cellW - visW) * 0.5f; break;
		case CD2DAlignment::HRight:  visLeft = cellLeft + cellW - visW; break;
		}
		float visTop = cellTop;
		switch (m_cellAlign.vert)
		{
		case CD2DAlignment::VTop:    visTop = cellTop; break;
		case CD2DAlignment::VCenter: visTop = cellTop + (cellH - visH) * 0.5f; break;
		case CD2DAlignment::VBottom: visTop = cellTop + cellH - visH; break;
		}

		// 内容矩形左上角
		float x = visLeft + leftDec;
		float y = visTop + topDec;

		pChild->SetPosition(x, y);
		pChild->SetSize(contentW, contentH);
	}

	// 处理绝对定位控件（最后绘制，保证在最上层）
	for (size_t i = 0; i < absoluteChildren.GetCount(); ++i)
	{
		CD2DControlUI* pChild = absoluteChildren[i];
		const CD2DSizePolicy& wp = pChild->GetWidthPolicy();
		const CD2DSizePolicy& hp = pChild->GetHeightPolicy();
		float cw, ch;
		if (wp.mode == CD2DSizePolicy::Auto) { pChild->SizeToContent(pRender, availW); cw = pChild->GetWidth(); }
		else if (wp.mode == CD2DSizePolicy::Fixed) cw = (wp.value > 0.0f) ? wp.value : pChild->GetWidth();
		else cw = pChild->GetWidth();
		if (hp.mode == CD2DSizePolicy::Auto) { pChild->SizeToContent(pRender, cw); ch = pChild->GetHeight(); }
		else if (hp.mode == CD2DSizePolicy::Fixed) ch = (hp.value > 0.0f) ? hp.value : pChild->GetHeight();
		else ch = pChild->GetHeight();
		CD2DPosition pos = pChild->GetPositionInfo();
		pChild->SetPosition(content.left + pos.left, content.top + pos.top);
		pChild->SetSize(cw, ch);
	}
}

void CD2DVBoxContainerUI::DrawControl(CD2DRender* pRender)
{
	Layout(pRender);
	CD2DContainerUI::DrawControl(pRender);
}

CD2DGridContainerUI::CD2DGridContainerUI(): m_rowSpacing(5.0f), m_colSpacing(5.0f),m_cellAlign(CD2DAlignment::HCenter, CD2DAlignment::VCenter) {}

CD2DGridContainerUI::~CD2DGridContainerUI() {}

