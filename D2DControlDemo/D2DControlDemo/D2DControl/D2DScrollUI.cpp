// CD2DScrollUI.cpp
#include "stdafx.h"
#include "D2DScrollUI.h"
#include <algorithm>

#ifndef FLT_MAX
#define FLT_MAX 3.402823466e+38F
#endif


// ==================== CD2DScrollUI 实现 ====================
CD2DScrollUI::CD2DScrollUI()
	: m_orientation(Vertical)
	, m_minValue(0)
	, m_maxValue(100)
	, m_currentValue(0)
	, m_pageSize(10)
	, m_isDragging(false)
	, m_dragStartPoint(D2D1::Point2F(0, 0))
	, m_dragStartValue(0)
	, m_thumbRect(D2D1::RectF(0, 0, 0, 0))
	, m_trackRect(D2D1::RectF(0, 0, 0, 0))
	, m_scrollCallback(nullptr)
	, m_callbackUserData(nullptr)
{
	// 禁用基类的控件整体拖拽功能
	EnableDrag(false);
	// 设置默认尺寸（垂直滚动条宽16，高100；水平滚动条高16，宽100）
	if (m_orientation == Vertical)
		SetSize(16.0f, 100.0f);
	else
		SetSize(100.0f, 16.0f);
}

CD2DScrollUI::~CD2DScrollUI()
{
}

// ==================== 方向 ====================
void CD2DScrollUI::SetOrientation(Orientation orient)
{
	if (m_orientation == orient)
		return;

	m_orientation = orient;
	// 交换宽高（保持面积大致不变）
	float w = GetWidth(), h = GetHeight();
	if (orient == Vertical)
		SetSize(max(16.0f, w), max(100.0f, h));
	else
		SetSize(max(100.0f, w), max(16.0f, h));

	InvalidateContent();
}

CD2DScrollUI::Orientation CD2DScrollUI::GetOrientation() const
{
	return m_orientation;
}

// ==================== 滚动参数 ====================
void CD2DScrollUI::SetRange(int minValue, int maxValue)
{
	if (minValue >= maxValue)
		maxValue = minValue + 1;
	m_minValue = minValue;
	m_maxValue = maxValue;
	// 钳制当前值
	SetValue(m_currentValue);
	InvalidateContent();
}

void CD2DScrollUI::GetRange(int& minValue, int& maxValue) const
{
	minValue = m_minValue;
	maxValue = m_maxValue;
}

void CD2DScrollUI::SetValue(int value)
{
	// 有效范围：[min, max - pageSize + 1]  （当 pageSize >= 有效范围时，只能为 min）
	int maxAllowed = m_maxValue - m_pageSize + 1;
	if (maxAllowed < m_minValue)
		maxAllowed = m_minValue;
	int newValue = max(m_minValue, min(maxAllowed, value));
	if (newValue == m_currentValue)
		return;
	m_currentValue = newValue;
	InvalidateContent();    // 触发重绘
	NotifyScroll();
}

int CD2DScrollUI::GetValue() const
{
	return m_currentValue;
}

void CD2DScrollUI::SetPageSize(int pageSize)
{
	if (pageSize < 1)
		pageSize = 1;
	m_pageSize = pageSize;
	SetValue(m_currentValue);   // 重新钳制
	InvalidateContent();
}

int CD2DScrollUI::GetPageSize() const
{
	return m_pageSize;
}

void CD2DScrollUI::SetScrollInfo(int contentLen, int viewportLen, int pos)
{
	if (viewportLen <= 0 || contentLen <= 0)
		return;
	int scrollRange = contentLen - viewportLen;
	if (scrollRange < 0)
		scrollRange = 0;
	SetRange(0, scrollRange);
	SetPageSize(viewportLen);
	SetValue(pos);
}

// ==================== 回调 ====================
void CD2DScrollUI::SetScrollCallback(ScrollCallback callback, void* pUserData)
{
	m_scrollCallback = callback;
	m_callbackUserData = pUserData;
}

void CD2DScrollUI::NotifyScroll()
{
	if (m_scrollCallback)
		m_scrollCallback(this, m_currentValue, m_callbackUserData);
}

// ==================== 样式管理 ====================
void CD2DScrollUI::SetScrollStyle(ED2DStatus state, const CD2DScrollStyle& style)
{
	m_scrollStyles.SetAt(state, style);
}


CD2DScrollStyle CD2DScrollUI::GetEffectiveScrollStyle() const
{
	ED2DStatus effectiveState = m_status.GetEffectiveState();
	auto pPair = m_scrollStyles.Lookup(effectiveState);
	if (pPair)
		return pPair->m_value;
	pPair = m_scrollStyles.Lookup(ED2DStatus::Normal);
	if (pPair)
		return pPair->m_value;
	return m_defaultScrollStyle;
}

// ==================== 重写基类方法 ====================
D2D1_SIZE_F CD2DScrollUI::MeasureContent(CD2DRender* pRender, float maxWidth /*= FLT_MAX*/, float maxHeight/* = FLT_MAX*/) const
{
	// 滚动条内容区域的最小尺寸（由方向决定）
	UNREFERENCED_PARAMETER(pRender);
	UNREFERENCED_PARAMETER(maxWidth);

	if (m_orientation == Vertical)
		return D2D1::SizeF(16.0f, 0.0f);   // 宽度固定16，高度由父容器决定（返回0表示不限制）
	else
		return D2D1::SizeF(0.0f, 16.0f);   // 高度固定16，宽度由父容器决定
}

void CD2DScrollUI::DrawControl(CD2DRender* pRender)
{
	if (!pRender || !IsVisible())
		return;

	// 获取整体样式（背景、边框、圆角等）
	CD2DStyle style = CalculateEffectiveStyle();
	D2D1_RECT_F borderRect = GetBorderRectangle();   // 边框矩形（即控件矩形减去外边距）

	// 绘制轨道背景（整体样式）
	DrawBackground(style, borderRect, pRender);
	DrawBorder(style, borderRect, pRender);

	// 轨道实际区域（边框内部，减去边框宽度）
	D2D1_RECT_F innerRect = borderRect;
	innerRect.left   += style.border.m_left.m_width;
	innerRect.top    += style.border.m_top.m_width;
	innerRect.right  -= style.border.m_right.m_width;
	innerRect.bottom -= style.border.m_bottom.m_width;

	// 保存轨道矩形（用于鼠标交互）
	m_trackRect = innerRect;

	// 更新滑块矩形
	UpdateThumbRect(m_trackRect);

	// 获取当前滚动条特有的样式（轨道+滑块）
	CD2DScrollStyle scrollStyle = GetEffectiveScrollStyle();

	// 绘制轨道（如果需要覆盖整体样式，可以在此绘制；这里直接使用整体样式已绘制，若需要单独轨道样式则在此覆盖）
	DrawBackground(scrollStyle.track, m_trackRect, pRender);
	DrawBorder(scrollStyle.track, m_thumbRect, pRender);

	// 滑块边框
	DrawBackground(scrollStyle.thumb, m_thumbRect, pRender);
	DrawBorder(scrollStyle.thumb, m_thumbRect, pRender);
	
}

// ==================== 鼠标事件 ====================
bool CD2DScrollUI::OnMouseDownL(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled())
		return false;

	// 命中测试：是否点在滑块上
	if (point.x >= m_thumbRect.left && point.x <= m_thumbRect.right &&
		point.y >= m_thumbRect.top && point.y <= m_thumbRect.bottom)
	{
		m_isDragging = true;
		m_dragStartPoint = point;
		m_dragStartValue = m_currentValue;
		SetPressed(true);
		InvalidateContent();  // 重绘滑块按下状态
		return true;
	}
	else
	{
		// 点击轨道：跳转到该位置并开始拖拽（便于微调）
		int newValue = ValueFromPoint(point);
		SetValue(newValue);
		// 开始拖拽，以便鼠标移动时能连续改变值
		m_isDragging = true;
		m_dragStartPoint = point;
		m_dragStartValue = m_currentValue;
		SetPressed(true);
		InvalidateContent();
		return true;
	}
	return false;
}

bool CD2DScrollUI::OnMouseMove(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled())
		return false;

	if (m_isDragging)
	{
		// 根据鼠标移动的距离计算滚动值变化
		float delta = 0.0f;
		if (m_orientation == Vertical)
			delta = point.y - m_dragStartPoint.y;
		else
			delta = point.x - m_dragStartPoint.x;

		// 轨道总长
		float trackLength = (m_orientation == Vertical) ?
			(m_trackRect.bottom - m_trackRect.top) : (m_trackRect.right - m_trackRect.left);
		float thumbLength = (m_orientation == Vertical) ?
			(m_thumbRect.bottom - m_thumbRect.top) : (m_thumbRect.right - m_thumbRect.left);
		if (trackLength <= thumbLength)
			return true;

		// 滑动范围（像素） = 轨道长度 - 滑块长度
		float pixelRange = trackLength - thumbLength;
		// 滚动值范围
		int valueRange = m_maxValue - m_minValue;
		if (valueRange <= 0)
			return true;

		// 根据移动像素计算值变化量
		int deltaValue = static_cast<int>(delta / pixelRange * valueRange + 0.5f);
		int newValue = m_dragStartValue + deltaValue;
		SetValue(newValue);
		return true;
	}

	// 可处理 hover 状态变化（如果需要）
	bool bHit = IsHitTest(point);
	if (bHit != IsHover())
	{
		if (bHit)
		{
			SetHover(true);
			InvalidateContent();
		}
		else
		{
			SetHover(false);
			InvalidateContent();
		}
	}

	return bHit;
}

bool CD2DScrollUI::OnMouseUpL(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled())
		return false;

	if (m_isDragging)
	{
		m_isDragging = false;
		SetPressed(false);
		InvalidateContent();
		return true;
	}
	return false;
}

bool CD2DScrollUI::OnMouseLeave(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled())
		return false;

	if (m_isDragging)
	{
		// 拖拽时鼠标离开控件区域，可以选择继续跟踪（保持拖拽状态）或停止。
		// 通常为了用户体验，即使离开控件区域也应继续拖拽，但要求捕获鼠标。
		// 这里简单处理：离开时结束拖拽。
		m_isDragging = false;
		SetPressed(false);
		InvalidateContent();
	}
	if (IsHover())
	{
		SetHover(false);
		InvalidateContent();
	}
	return true;
}

bool CD2DScrollUI::OnMouseWheel(float delta, const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled())
		return false;

	// 滚轮每次滚动 3 个单位（可调整）
	int step = 3;
	if (delta > 0)
		SetValue(m_currentValue - step);
	else if (delta < 0)
		SetValue(m_currentValue + step);
	return true;
}

// ==================== 内部辅助函数 ====================
void CD2DScrollUI::UpdateThumbRect(const D2D1_RECT_F& trackRect)
{
	if (trackRect.right <= trackRect.left || trackRect.bottom <= trackRect.top)
	{
		m_thumbRect = D2D1::RectF(0, 0, 0, 0);
		return;
	}

	float trackLength, thumbLength, thumbPos;
	float totalContent = static_cast<float>(m_maxValue - m_minValue + m_pageSize);
	float ratio = static_cast<float>(m_pageSize) / totalContent;

	// 有效滚动范围（滑块可移动的总步数）
	int effectiveRange = m_maxValue - m_minValue - m_pageSize + 1;
	if (effectiveRange < 0) effectiveRange = 0;

	// 获取样式中的最小滑块长度
	CD2DScrollStyle style = GetEffectiveScrollStyle();
	float minLen = style.thumbMinLength;
	if (m_orientation == Vertical)
	{
		trackLength = trackRect.bottom - trackRect.top;
		thumbLength = trackLength * ratio;
		if (thumbLength < minLen) thumbLength = minLen;
		if (thumbLength > trackLength) thumbLength = trackLength;

		if (effectiveRange == 0)
			thumbPos = 0.0f;
		else
		{
			float valuePos = static_cast<float>(m_currentValue - m_minValue);
			thumbPos = (valuePos / effectiveRange) * (trackLength - thumbLength);
		}

		m_thumbRect = D2D1::RectF(
			trackRect.left,
			trackRect.top + thumbPos,
			trackRect.right,
			trackRect.top + thumbPos + thumbLength
			);
	}
	else // Horizontal
	{
		trackLength = trackRect.right - trackRect.left;
		thumbLength = trackLength * ratio;
		if (thumbLength < minLen) thumbLength = minLen;
		if (thumbLength > trackLength) thumbLength = trackLength;

		if (effectiveRange == 0)
			thumbPos = 0.0f;
		else
		{
			float valuePos = static_cast<float>(m_currentValue - m_minValue);
			thumbPos = (valuePos / effectiveRange) * (trackLength - thumbLength);
		}

		m_thumbRect = D2D1::RectF(
			trackRect.left + thumbPos,
			trackRect.top,
			trackRect.left + thumbPos + thumbLength,
			trackRect.bottom
			);
	}
}

int CD2DScrollUI::ValueFromPoint(const D2D1_POINT_2F& point) const
{
	if (m_trackRect.right <= m_trackRect.left || m_trackRect.bottom <= m_trackRect.top)
		return m_currentValue;

	int effectiveRange = m_maxValue - m_minValue - m_pageSize + 1;
	if (effectiveRange <= 0)
		return m_minValue;

	float trackLength, ratio;
	float thumbLen;

	if (m_orientation == Vertical)
	{
		trackLength = m_trackRect.bottom - m_trackRect.top;
		thumbLen = m_thumbRect.bottom - m_thumbRect.top;
		if (thumbLen >= trackLength)
			return m_minValue;

		float pos = point.y - m_trackRect.top;
		if (pos < 0) pos = 0;
		if (pos > trackLength) pos = trackLength;

		// 使滑块中心对准点击位置
		float effectivePos = pos - thumbLen * 0.5f;
		if (effectivePos < 0) effectivePos = 0;
		if (effectivePos > trackLength - thumbLen) effectivePos = trackLength - thumbLen;

		ratio = effectivePos / (trackLength - thumbLen);
	}
	else // Horizontal
	{
		trackLength = m_trackRect.right - m_trackRect.left;
		thumbLen = m_thumbRect.right - m_thumbRect.left;
		if (thumbLen >= trackLength)
			return m_minValue;

		float pos = point.x - m_trackRect.left;
		if (pos < 0) pos = 0;
		if (pos > trackLength) pos = trackLength;

		float effectivePos = pos - thumbLen * 0.5f;
		if (effectivePos < 0) effectivePos = 0;
		if (effectivePos > trackLength - thumbLen) effectivePos = trackLength - thumbLen;

		ratio = effectivePos / (trackLength - thumbLen);
	}

	int newValue = m_minValue + static_cast<int>(ratio * effectiveRange + 0.5f);
	// 钳制到有效范围
	int maxAllowed = m_minValue + effectiveRange;
	if (newValue < m_minValue) newValue = m_minValue;
	if (newValue > maxAllowed) newValue = maxAllowed;
	return newValue;
}