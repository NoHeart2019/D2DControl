#include "stdafx.h"
#include "D2DScrollBarUI.h"


void CD2DScrollBarUI::OnScrollCallback(float position, void* userData)
{
	CD2DScrollBarUI* This = static_cast<CD2DScrollBarUI*>(userData);
	if (This)
	{
		This->m_position = position;
	}
}

CD2DScrollBarUI::CD2DScrollBarUI(ScrollBarDirection direction)
    : m_direction(direction)
    , m_minRange(0.0f)
    , m_maxRange(100.0f)
    , m_position(0.0f)
    , m_pageSize(50.0f)
    , m_stepSize(10.0f)
    , m_thumbSize(20.0f)
    , m_trackWidth(16.0f)
    , m_dragOffset(0.0f)
    , m_scrollCallback(CD2DScrollBarUI::OnScrollCallback)
    , m_userData(this)
{
    // 设置默认颜色
    m_thumbColor = D2D1::ColorF(0.5f, 0.5f, 0.5f, 1.0f);
    m_thumbHoverColor = D2D1::ColorF(0.6f, 0.6f, 0.6f, 1.0f);
    m_thumbPressedColor = D2D1::ColorF(0.7f, 0.7f, 0.7f, 1.0f);
    m_trackColor = D2D1::ColorF(0.2f, 0.2f, 0.2f, 0.5f);
    m_trackHoverColor = D2D1::ColorF(0.3f, 0.3f, 0.3f, 0.6f);
    
    // 默认背景透明
    SetBackgroundColor(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
    SetBorderVisible(false);
    
    // 初始化鼠标位置
    m_currentMousePos = D2D1::Point2F(-1.0f, -1.0f);
}

CD2DScrollBarUI::~CD2DScrollBarUI()
{
    // 析构函数
}

void CD2DScrollBarUI::Render(CD2DRender* pRender)
{
    if (!pRender || !IsVisible())
        return;

    // 计算轨道和滑块矩形
    D2D1_RECT_F trackRect = CalculateTrackRect();
    D2D1_RECT_F thumbRect = CalculateThumbRect();
    
    // 绘制轨道
    D2D1_COLOR_F trackColorToUse = m_trackColor;
    if (IsHovered() && !m_isDragging)
    {
        trackColorToUse = m_trackHoverColor;
    }
    
    // 根据方向绘制圆角矩形轨道
    float radius = m_direction == ScrollBarDirection::Vertical ? 
        m_trackWidth / 2.0f : m_trackWidth / 2.0f;
    
    pRender->FillRoundedRectangle(trackRect, radius, radius, trackColorToUse);
    
    // 绘制滑块
    D2D1_COLOR_F thumbColorToUse = m_thumbColor;

    if (m_isDragging)
    {
        thumbColorToUse = m_thumbPressedColor;
    }
    else if (IsHovered())
    {
        thumbColorToUse = m_thumbHoverColor;
    }
    
    // 滑块圆角
    float thumbRadius = m_direction == ScrollBarDirection::Vertical ? 
        m_trackWidth / 3.0f : m_trackWidth / 3.0f;
    
    pRender->FillRoundedRectangle(thumbRect, thumbRadius, thumbRadius, thumbColorToUse);
}

void CD2DScrollBarUI::SetDirection(ScrollBarDirection direction)
{
    m_direction = direction;
    // 可能需要重新计算尺寸和位置
}

ScrollBarDirection CD2DScrollBarUI::GetDirection() const
{
    return m_direction;
}

void CD2DScrollBarUI::SetRange(float minRange, float maxRange)
{
    m_minRange = minRange;
    m_maxRange = maxRange > minRange ? maxRange : minRange;
    
    // 确保当前位置在新范围内
    ClampPosition();
}

void CD2DScrollBarUI::GetRange(float& minRange, float& maxRange) const
{
    minRange = m_minRange;
    maxRange = m_maxRange;
}

void CD2DScrollBarUI::SetPosition(float position)
{
    m_position = position;
    ClampPosition();
    TriggerScrollCallback();
}

float CD2DScrollBarUI::GetPosition() const
{
    return m_position;
}

void CD2DScrollBarUI::SetPageSize(float pageSize)
{
    m_pageSize = max(0.0f, pageSize);
}

float CD2DScrollBarUI::GetPageSize() const
{
    return m_pageSize;
}

void CD2DScrollBarUI::SetStepSize(float stepSize)
{
    m_stepSize = max(0.01f, stepSize);
}

float CD2DScrollBarUI::GetStepSize() const
{
    return m_stepSize;
}

void CD2DScrollBarUI::ScrollToStart()
{
    SetPosition(m_minRange);
}

void CD2DScrollBarUI::ScrollToEnd()
{
    SetPosition(m_maxRange);
}

void CD2DScrollBarUI::ScrollPageUp()
{
    SetPosition(m_position - m_pageSize);
}

void CD2DScrollBarUI::ScrollPageDown()
{
    SetPosition(m_position + m_pageSize);
}

void CD2DScrollBarUI::ScrollUp()
{
    SetPosition(m_position - m_stepSize);
}

void CD2DScrollBarUI::ScrollDown()
{
    SetPosition(m_position + m_stepSize);
}

void CD2DScrollBarUI::SetVisible(bool visible)
{
    CD2DControlUI::SetVisible(visible);
}

bool CD2DScrollBarUI::IsVisible() const
{
    // 当没有滚动范围时自动隐藏
    if (m_maxRange <= m_minRange)
    {
        return false;
    }
    return CD2DControlUI::IsVisible();
}

void CD2DScrollBarUI::SetThumbSize(float size)
{
    m_thumbSize = max(10.0f, size);
}

float CD2DScrollBarUI::GetThumbSize() const
{
    return m_thumbSize;
}

//设置滑块是否拖拽
void CD2DScrollBarUI::SetThumbDragging(bool isDragging)
{
	m_isDragging = isDragging;
}

bool CD2DScrollBarUI::IsThumbDragging()
{
	return m_isDragging;
}



void CD2DScrollBarUI::SetTrackWidth(float width)
{
    m_trackWidth = max(4.0f, width);
    
    // 调整控件大小以适应轨道宽度
    D2D1_SIZE_F currentSize = GetSize();
    if (m_direction == ScrollBarDirection::Vertical)
    {
        SetSize(m_trackWidth, currentSize.height);
    }
    else
    {
        SetSize(currentSize.width, m_trackWidth);
    }
}

float CD2DScrollBarUI::GetTrackWidth() const
{
    return m_trackWidth;
}

// 设置滚动条颜色
void CD2DScrollBarUI::SetThumbColor(D2D1_COLOR_F color)
{
    m_thumbColor = color;
}

void CD2DScrollBarUI::SetThumbHoverColor(D2D1_COLOR_F color)
{
    m_thumbHoverColor = color;
}

void CD2DScrollBarUI::SetThumbPressedColor(D2D1_COLOR_F color)
{
    m_thumbPressedColor = color;
}

void CD2DScrollBarUI::SetTrackColor(D2D1_COLOR_F color)
{
    m_trackColor = color;
}

void CD2DScrollBarUI::SetTrackHoverColor(D2D1_COLOR_F color)
{
    m_trackHoverColor = color;
}

void CD2DScrollBarUI::SetTrackPressedColor(D2D1_COLOR_F color)
{
    m_trackColor = color;
}

// 设置用户数据
void CD2DScrollBarUI::SetUserData(void* userData)
{
    m_userData = userData;
}

void* CD2DScrollBarUI::GetUserData() const
{
    return m_userData;
}

bool CD2DScrollBarUI::OnMouseDownL(const D2D1_POINT_2F& point)
{
    if (!IsVisible() || !IsEnabled())
        return false;
    
    m_currentMousePos = point;
    
    // 计算滑块矩形
    D2D1_RECT_F thumbRect = CalculateThumbRect();
    
    // 检查是否点击了滑块
    if (IsPointInRect(point, thumbRect))
    {
        SetDragging(true);
        
        // 计算拖动偏移量
        if (m_direction == ScrollBarDirection::Vertical)
        {
            m_dragOffset = point.y - thumbRect.top;
        }
        else
        {
            m_dragOffset = point.x - thumbRect.left;
        }
        
        return true;
    }
    
    // 检查是否点击了轨道
    D2D1_RECT_F trackRect = CalculateTrackRect();
    if (IsPointInRect(point, trackRect))
    {
        // 根据点击位置设置滑块位置
        UpdateThumbPositionFromPoint(point);
        return true;
    }
    
    return false;
}

bool CD2DScrollBarUI::OnMouseMove(const D2D1_POINT_2F& point)
{
    if (!IsVisible())
        return false;
    
    m_currentMousePos = point;
	
    // 拖动时优先处理
    if (IsDragging())
    {
        UpdateThumbPositionFromPoint(point);
        return true;
    }
    
	// 更新鼠标悬浮状态
	bool isInside = IsPointInRect(point);
	if (isInside && !IsHovered())   // 移入
	{
		SetHovered(true);
		return true;
	}
	else if (!isInside && IsHovered()) // 移出
	{
		return OnMouseLeave(point);
	}
    
    return false;
}

bool CD2DScrollBarUI::OnMouseUpL(const D2D1_POINT_2F& point)
{
    if (!IsVisible())
        return false;
    
    if (IsDragging())
    {
        SetDragging(false);
        return true;
    }
    
    return false;
}

bool CD2DScrollBarUI::OnMouseLeave(const D2D1_POINT_2F& point)
{
    if (!IsVisible())
        return false;
	
	if (IsDragging())
	{
		UpdateThumbPositionFromPoint(point);
		return true;
	}

	m_currentMousePos = D2D1::Point2F(-1.0f, -1.0f);
	
	if (IsHovered())
	{
		SetHovered(false);
		return true;
	}
	
    return false;
}

bool CD2DScrollBarUI::OnMouseWheel(float delta)
{
    if (!IsVisible() || !IsEnabled())
        return false;
    
    // 根据鼠标滚轮方向滚动
    if (delta > 0)
    {
        ScrollUp();
    }
    else
    {
        ScrollDown();
    }
    
    return true;
}

void CD2DScrollBarUI::SetScrollCallback(ScrollCallback callback, void* userData)
{
    m_scrollCallback = callback;
    m_userData = userData;
}

D2D1_RECT_F CD2DScrollBarUI::CalculateThumbRect() const
{
    D2D1_RECT_F rect = GetRectangle();
    D2D1_RECT_F trackRect = CalculateTrackRect();
    
    float totalRange = m_maxRange - m_minRange;
    float thumbSize = m_thumbSize;
    
    // 如果有页面大小，根据页面大小计算滑块大小
    if (m_pageSize > 0 && totalRange > 0)
    {
        float availableSize = m_direction == ScrollBarDirection::Vertical ? 
            trackRect.bottom - trackRect.top : trackRect.right - trackRect.left;
        
        float calculatedThumbSize = (m_pageSize / (totalRange + m_pageSize)) * availableSize;
        thumbSize = max(m_thumbSize, calculatedThumbSize); // 确保滑块至少有最小大小
    }
    
    D2D1_RECT_F thumbRect;
    
    if (m_direction == ScrollBarDirection::Vertical)
    {
        // 垂直滚动条
        float availableHeight = trackRect.bottom - trackRect.top - thumbSize;
        float positionRatio = totalRange > 0 ? (m_position - m_minRange) / totalRange : 0.0f;
        
        float thumbTop = trackRect.top + (availableHeight * positionRatio);
        
        thumbRect = D2D1::RectF(
            trackRect.left + 2.0f,
            thumbTop,
            trackRect.right - 2.0f,
            thumbTop + thumbSize
        );
    }
    else
    {
        // 水平滚动条
        float availableWidth = trackRect.right - trackRect.left - thumbSize;
        float positionRatio = totalRange > 0 ? (m_position - m_minRange) / totalRange : 0.0f;
        
        float thumbLeft = trackRect.left + (availableWidth * positionRatio);
        
        thumbRect = D2D1::RectF(
            thumbLeft,
            trackRect.top + 2.0f,
            thumbLeft + thumbSize,
            trackRect.bottom - 2.0f
        );
    }
    
    return thumbRect;
}

D2D1_RECT_F CD2DScrollBarUI::CalculateTrackRect() const
{
    D2D1_RECT_F rect = GetRectangle();
    
    if (m_direction == ScrollBarDirection::Vertical)
    {
        // 垂直滚动条，轨道居中
        float centerX = rect.left + (rect.right - rect.left) / 2.0f;
        return D2D1::RectF(
            centerX - m_trackWidth / 2.0f,
            rect.top + 2.0f,
            centerX + m_trackWidth / 2.0f-2.0f,
            rect.bottom - 2.0f
        );
    }
    else
    {
        // 水平滚动条，轨道居中
        float centerY = rect.top + (rect.bottom - rect.top) / 2.0f;
        return D2D1::RectF(
            rect.left + 2.0f,
            centerY - m_trackWidth / 2.0f,
            rect.right - 2.0f,
            centerY + m_trackWidth / 2.0f
        );
    }
}

void CD2DScrollBarUI::UpdateThumbPositionFromPoint(const D2D1_POINT_2F& point)
{
    D2D1_RECT_F trackRect = CalculateTrackRect();
    D2D1_RECT_F thumbRect = CalculateThumbRect();
    
    float totalRange = m_maxRange - m_minRange;
    
    if (totalRange <= 0)
        return;
    
    float newPosition;
    
    if (m_direction == ScrollBarDirection::Vertical)
    {
        float availableHeight = trackRect.bottom - trackRect.top - (thumbRect.bottom - thumbRect.top);
        float mousePos = point.y - m_dragOffset - trackRect.top;
        
        float positionRatio = availableHeight > 0 ? mousePos / availableHeight : 0.0f;
        newPosition = m_minRange + (positionRatio * totalRange);
    }
    else
    {
        float availableWidth = trackRect.right - trackRect.left - (thumbRect.right - thumbRect.left);
        float mousePos = point.x - m_dragOffset - trackRect.left;
        
        float positionRatio = availableWidth > 0 ? mousePos / availableWidth : 0.0f;
        newPosition = m_minRange + (positionRatio * totalRange);
    }
    
    SetPosition(newPosition);
}

void CD2DScrollBarUI::ClampPosition()
{
    // 限制位置在最小和最大范围内
    if (m_position < m_minRange)
        m_position = m_minRange;
    if (m_position > m_maxRange)
        m_position = m_maxRange;
}

void CD2DScrollBarUI::TriggerScrollCallback()
{
    if (m_scrollCallback)
    {
        m_scrollCallback(m_position, m_userData);
    }
}