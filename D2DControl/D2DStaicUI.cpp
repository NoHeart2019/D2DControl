#include "stdafx.h"
#include "D2D.h"
#include "D2DScrollBarUI.h"
#include "D2DStaticUI.h"

CD2DStaticUI::CD2DStaticUI()
    : m_fontFamily(L"微软雅黑")
    , m_fontSize(14.0f)
    , m_isFontBold(false)
    , m_textColor(D2D1::ColorF(D2D1::ColorF::Black))
    , m_horizontalAlignment(DWRITE_TEXT_ALIGNMENT_LEADING)
    , m_verticalAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
    , m_verticalScrollBar(ScrollBarDirection::Vertical)
    , m_horizontalScrollBar(ScrollBarDirection::Horizontal)
    , m_enableHorizontalScroll(false)
    , m_enableVerticalScroll(true)
    , m_scrollBarWidth(16.0f)
    , m_paddingLeft(5.0f)
    , m_paddingTop(5.0f)
    , m_paddingRight(5.0f)
    , m_paddingBottom(5.0f)
    , m_textContentSize(D2D1::SizeF(0.0f, 0.0f))
    , m_scrollPositionX(0.0f)
    , m_scrollPositionY(0.0f)
    , m_isUpdatingScroll(false)
{
    // 设置背景色为白色
    SetBackgroundColor(D2D1::ColorF(D2D1::ColorF::White));
    SetBorderColor(D2D1::ColorF(D2D1::ColorF::Black));
    SetBorderWidth(1.0f);
    SetBorderVisible(true);

    // 设置滚动条回调
    m_verticalScrollBar.SetScrollCallback(OnVerticalScrollCallback, this);
    m_horizontalScrollBar.SetScrollCallback(OnHorizontalScrollCallback, this);
}

CD2DStaticUI::~CD2DStaticUI()
{
    // 析构函数
}

void CD2DStaticUI::SetText(const CString& text)
{
    m_text = text;
    m_textContentSize = D2D1::SizeF(0.0f, 0.0f); // 重置大小，稍后重新计算
    m_scrollPositionX = 0.0f;
    m_scrollPositionY = 0.0f;
}

CString CD2DStaticUI::GetText() const
{
    return m_text;
}

void CD2DStaticUI::SetFont(const wchar_t* fontFamily, float fontSize, bool isBold)
{
    m_fontFamily = fontFamily;
    m_fontSize = fontSize;
    m_isFontBold = isBold;
    m_textContentSize = D2D1::SizeF(0.0f, 0.0f); // 重置大小，稍后重新计算
}

void CD2DStaticUI::GetFont(CString& fontFamily, float& fontSize, bool& isBold) const
{
    fontFamily = m_fontFamily;
    fontSize = m_fontSize;
    isBold = m_isFontBold;
}

void CD2DStaticUI::SetTextColor(const D2D1_COLOR_F& color)
{
    m_textColor = color;
}

D2D1_COLOR_F CD2DStaticUI::GetTextColor() const
{
    return m_textColor;
}

void CD2DStaticUI::SetTextAlignment(DWRITE_TEXT_ALIGNMENT horizontalAlign, DWRITE_PARAGRAPH_ALIGNMENT verticalAlign)
{
    m_horizontalAlignment = horizontalAlign;
    m_verticalAlignment = verticalAlign;
}

void CD2DStaticUI::EnableHorizontalScroll(bool enable)
{
    m_enableHorizontalScroll = enable;
    m_horizontalScrollBar.SetVisible(enable);
    UpdateScrollBars();
}

bool CD2DStaticUI::IsHorizontalScrollEnabled() const
{
    return m_enableHorizontalScroll;
}

void CD2DStaticUI::EnableVerticalScroll(bool enable)
{
    m_enableVerticalScroll = enable;
    m_verticalScrollBar.SetVisible(enable);
    UpdateScrollBars();
}

bool CD2DStaticUI::IsVerticalScrollEnabled() const
{
    return m_enableVerticalScroll;
}

void CD2DStaticUI::SetScrollBarWidth(float width)
{
    m_scrollBarWidth = width > 0 ? width : 16.0f;
    UpdateScrollBars();
}

float CD2DStaticUI::GetScrollBarWidth() const
{
    return m_scrollBarWidth;
}

void CD2DStaticUI::SetPadding(float left, float top, float right, float bottom)
{
    m_paddingLeft = left;
    m_paddingTop = top;
    m_paddingRight = right;
    m_paddingBottom = bottom;
    m_textContentSize = D2D1::SizeF(0.0f, 0.0f); // 重置大小，稍后重新计算
    UpdateScrollBars();
}

void CD2DStaticUI::GetPadding(float& left, float& top, float& right, float& bottom) const
{
    left = m_paddingLeft;
    top = m_paddingTop;
    right = m_paddingRight;
    bottom = m_paddingBottom;
}

void CD2DStaticUI::ScrollToTop()
{
    m_scrollPositionY = 0.0f;
    m_verticalScrollBar.SetPosition(0.0f);
}

void CD2DStaticUI::ScrollToBottom()
{
    D2D1_SIZE_F size = GetSize();
    float maxScrollY = max(0.0f, m_textContentSize.height - (size.height - m_paddingTop - m_paddingBottom));
    m_scrollPositionY = maxScrollY;
    m_verticalScrollBar.SetPosition(maxScrollY);
}

void CD2DStaticUI::ScrollToLeft()
{
    m_scrollPositionX = 0.0f;
    m_horizontalScrollBar.SetPosition(0.0f);
}

void CD2DStaticUI::ScrollToRight()
{
    D2D1_SIZE_F size = GetSize();
    float maxScrollX = max(0.0f, m_textContentSize.width - (size.width - m_paddingLeft - m_paddingRight));
    m_scrollPositionX = maxScrollX;
    m_horizontalScrollBar.SetPosition(maxScrollX);
}

void CD2DStaticUI::ScrollBy(float deltaX, float deltaY)
{
    m_scrollPositionX += deltaX;
    m_scrollPositionY += deltaY;
    
    // 限制滚动范围
    D2D1_SIZE_F size = GetSize();
    float maxScrollX = max(0.0f, m_textContentSize.width - (size.width - m_paddingLeft - m_paddingRight));
    float maxScrollY = max(0.0f, m_textContentSize.height - (size.height - m_paddingTop - m_paddingBottom));
    
    m_scrollPositionX = max(0.0f, min(m_scrollPositionX, maxScrollX));
    m_scrollPositionY = max(0.0f, min(m_scrollPositionY, maxScrollY));
    
    // 更新滚动条位置
    m_horizontalScrollBar.SetPosition(m_scrollPositionX);
    m_verticalScrollBar.SetPosition(m_scrollPositionY);
}

float CD2DStaticUI::GetScrollPositionX() const
{
    return m_scrollPositionX;
}

float CD2DStaticUI::GetScrollPositionY() const
{
    return m_scrollPositionY;
}

void CD2DStaticUI::RecalculateLayout()
{
    m_textContentSize = D2D1::SizeF(0.0f, 0.0f); // 重置大小，稍后重新计算
    UpdateScrollBars();
}

bool CD2DStaticUI::OnMouseDownL(const D2D1_POINT_2F& point)
{
    if (!IsEnabled() || !IsVisible())
        return false;

    // 检查是否点击在滚动条上
    if (m_verticalScrollBar.IsVisible() && m_verticalScrollBar.OnMouseDownL(point))
    {
        return true;
    }
    
    if (m_horizontalScrollBar.IsVisible() && m_horizontalScrollBar.OnMouseDownL(point))
    {
        return true;
    }

    // 处理文本区域的点击
    if (IsPointInRect(point))
    {
        m_isPressed = true;
        return true;
    }

    return false;
}

bool CD2DStaticUI::OnMouseMove(const D2D1_POINT_2F& point)
{
    if (!IsEnabled() || !IsVisible())
        return false;

    // 转发鼠标事件到滚动条
    bool handled = false;
    
    if (m_verticalScrollBar.IsVisible())
    {
        handled = m_verticalScrollBar.OnMouseMove(point) || handled;
    }
    
    if (m_horizontalScrollBar.IsVisible())
    {
        handled = m_horizontalScrollBar.OnMouseMove(point) || handled;
    }

    // 检查鼠标是否在控件内
    bool isInside = IsPointInRect(point);
    
    if (isInside && !m_isHover)
    {
        m_isHover = true;
    }
    else if (!isInside && m_isHover)
    {
        m_isHover = false;
        OnMouseLeave(point);
    }

    return handled || m_isPressed;
}

bool CD2DStaticUI::OnMouseUpL(const D2D1_POINT_2F& point)
{
    if (!IsEnabled() || !IsVisible())
        return false;

    bool handled = false;
    
    if (m_verticalScrollBar.IsVisible())
    {
        handled = m_verticalScrollBar.OnMouseUpL(point) || handled;
    }
    
    if (m_horizontalScrollBar.IsVisible())
    {
        handled = m_horizontalScrollBar.OnMouseUpL(point) || handled;
    }

    m_isPressed = false;
    
    return handled;
}

bool CD2DStaticUI::OnMouseLeave(const D2D1_POINT_2F& point)
{
    if (!IsEnabled() || !IsVisible())
        return false;

    m_isHover = false;
    
    // 通知滚动条鼠标离开
    if (m_verticalScrollBar.IsVisible())
    {
        m_verticalScrollBar.OnMouseLeave(point);
    }
    
    if (m_horizontalScrollBar.IsVisible())
    {
        m_horizontalScrollBar.OnMouseLeave(point);
    }

    return true;
}

bool CD2DStaticUI::OnMouseWheel(float delta)
{
    if (!IsEnabled() || !IsVisible())
        return false;

    // 默认垂直滚动
    if (m_verticalScrollBar.IsVisible())
    {
        return m_verticalScrollBar.OnMouseWheel(delta);
    }
    // 如果没有启用垂直滚动但启用了水平滚动，则水平滚动
    else if (m_horizontalScrollBar.IsVisible())
    {
        return m_horizontalScrollBar.OnMouseWheel(delta);
    }

    return false;
}

void CD2DStaticUI::SetSize(float width, float height, bool keepCenter)
{
    CD2DControlUI::SetSize(width, height);
	
	D2D1_RECT_F rect = GetRectangle();

		
    UpdateScrollBars();
}

void CD2DStaticUI::OnVerticalScrollCallback(float position, void* userData)
{
    if (userData)
    {
        CD2DStaticUI* pThis = static_cast<CD2DStaticUI*>(userData);
        pThis->OnVerticalScroll(position);
    }
}

void CD2DStaticUI::OnHorizontalScrollCallback(float position, void* userData)
{
    if (userData)
    {
        CD2DStaticUI* pThis = static_cast<CD2DStaticUI*>(userData);
        pThis->OnHorizontalScroll(position);
    }
}

void CD2DStaticUI::OnVerticalScroll(float position)
{
    if (m_isUpdatingScroll)
        return;

    m_isUpdatingScroll = true;
    m_scrollPositionY = position;
    m_isUpdatingScroll = false;
}

void CD2DStaticUI::OnHorizontalScroll(float position)
{
    if (m_isUpdatingScroll)
        return;

    m_isUpdatingScroll = true;
    m_scrollPositionX = position;
    m_isUpdatingScroll = false;
}

void CD2DStaticUI::CalculateTextBounds(CD2DRender* pRender)
{
    if (!pRender || m_text.IsEmpty())
        return;

    // 获取可用于显示文本的区域大小
    D2D1_RECT_F textRect = GetTextDisplayRect();
    float  textWidth = textRect.right - textRect.left;
	float  textHeight = textRect.bottom - textRect.top;


    // 创建文本格式
    DWRITE_FONT_WEIGHT fontWeight = m_isFontBold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL;
    CComPtr<IDWriteTextFormat> textFormat = pRender->CreateTextFormat(m_fontFamily, m_fontSize, fontWeight, m_horizontalAlignment, m_verticalAlignment );

    // 测量文本边界
    if (m_enableVerticalScroll)
    {
        // 如果启用了水平滚动，测量单行文本的宽度和高度
        m_textContentSize = pRender->MeasureText(m_text, textFormat, textWidth);
    }
	if (m_enableHorizontalScroll)
	{
		// 如果启用了水平滚动，测量单行文本的宽度和高度
		m_textContentSize = pRender->MeasureText(m_text, textFormat, FLT_MAX, textHeight);
	}

   
}

void CD2DStaticUI::UpdateScrollBars()
{
    D2D1_SIZE_F controlSize = GetSize();
	D2D1_RECT_F rect = GetRectangle();
    float availableWidth = controlSize.width - m_paddingLeft - m_paddingRight;
    float availableHeight = controlSize.height - m_paddingTop - m_paddingBottom;
    
    // 更新垂直滚动条
    if (m_enableVerticalScroll)
    {
        // 计算是否需要显示垂直滚动条
        bool needVerticalScroll = m_textContentSize.height > availableHeight;
        
        if (needVerticalScroll)
        {
            // 设置垂直滚动条范围和位置
            float maxScrollY = m_textContentSize.height - availableHeight;
            m_verticalScrollBar.SetRange(0, maxScrollY);
            m_verticalScrollBar.SetPageSize(availableHeight);
            
            // 设置垂直滚动条大小和位置
            float scrollBarX = controlSize.width - m_scrollBarWidth;
            m_verticalScrollBar.SetPosition(rect.left + scrollBarX, rect.top);
			

            m_verticalScrollBar.SetSize(m_scrollBarWidth, controlSize.height);
            m_verticalScrollBar.SetVisible(true);
            
            // 调整水平滚动条的宽度（如果需要）
            availableWidth -= m_scrollBarWidth;
        }
        else
        {
            m_verticalScrollBar.SetVisible(false);
            m_scrollPositionY = 0.0f;
        }
    }
    else
    {
        m_verticalScrollBar.SetVisible(false);
    }
    
    // 更新水平滚动条
    if (m_enableHorizontalScroll)
    {
        // 计算是否需要显示水平滚动条
        bool needHorizontalScroll = m_textContentSize.width > availableWidth;
        
        if (needHorizontalScroll)
        {
            // 设置水平滚动条范围和位置
            float maxScrollX = m_textContentSize.width - availableWidth;
            m_horizontalScrollBar.SetRange(0.0f, maxScrollX);
            m_horizontalScrollBar.SetPageSize(availableWidth);
            
            // 设置水平滚动条大小和位置
            float scrollBarY = controlSize.height - m_scrollBarWidth;
            float scrollBarWidth = m_verticalScrollBar.IsVisible() ? 
                controlSize.width - m_scrollBarWidth : controlSize.width;
            m_horizontalScrollBar.SetPosition(rect.left, rect.top + scrollBarY);
            m_horizontalScrollBar.SetSize(scrollBarWidth, m_scrollBarWidth);
            m_horizontalScrollBar.SetVisible(true);
        }
        else
        {
            m_horizontalScrollBar.SetVisible(false);
            m_scrollPositionX = 0.0f;
        }
    }
    else
    {
        m_horizontalScrollBar.SetVisible(false);
    }
}

D2D1_RECT_F CD2DStaticUI::GetTextDisplayRect() const
{
    D2D1_RECT_F rect = GetRectangle();
    
    // 应用内边距
    rect.left += m_paddingLeft;
    rect.top += m_paddingTop;
    rect.right -= m_paddingRight;
    rect.bottom -= m_paddingBottom;
    
    // 如果显示了滚动条，调整显示区域
    if (m_verticalScrollBar.IsVisible())
    {
        rect.right -= m_scrollBarWidth;
    }
    
    if (m_horizontalScrollBar.IsVisible())
    {
        rect.bottom -= m_scrollBarWidth;
    }
    
    return rect;
}

void CD2DStaticUI::Render(CD2DRender* pRender)
{
	if (!pRender || !IsVisible())
		return;

	// 绘制背景和边框
	D2D1_RECT_F rect = GetRectangle();

	if (IsBackgroundVisible())
	{
		pRender->FillRectangle(rect, GetBackgroundColor());
	}

	if (IsBorderVisible())
	{
		pRender->DrawRectangle(rect, GetBorderColor(), GetBorderWidth());
	}

	// 计算文本边界大小（如果还没有计算）
	
	CalculateTextBounds(pRender);
	

	// 更新滚动条状态
	UpdateScrollBars();

	// 绘制滚动条
	if (m_verticalScrollBar.IsVisible())
	{
		m_verticalScrollBar.Render(pRender);
	}

	if (m_horizontalScrollBar.IsVisible())
	{
		m_horizontalScrollBar.Render(pRender);
	}

	// 绘制文本
	if (!m_text.IsEmpty())
	{
		D2D1_RECT_F textRect = GetTextDisplayRect();

		// 创建文本格式
		DWRITE_FONT_WEIGHT fontWeight = m_isFontBold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL;
		CComPtr<IDWriteTextFormat> textFormat = pRender->CreateTextFormat(m_fontFamily,m_fontSize,fontWeight,m_horizontalAlignment,m_verticalAlignment);

        pRender->MeasureText(m_text, textFormat, rect.right - textRect.left, rect.bottom - rect.top);
        
		// 4. 绘制时应用滚动偏移，并裁剪到可视区域
		D2D1_RECT_F clipRect = GetTextDisplayRect();

		pRender->GetRenderTarget()->PushAxisAlignedClip(clipRect, D2D1_ANTIALIAS_MODE_ALIASED); // 限制只在此区域内绘制
		
		float offsetX = 0.0f;
		float offsetY = 0.0f;

		if (m_enableVerticalScroll)
		{	
			offsetY = m_verticalScrollBar.GetPosition();
		}else if (m_enableHorizontalScroll)
		{
			offsetX = m_horizontalScrollBar.GetPosition();
		}

	
		pRender->DrawTextLayout(
		textRect.left- offsetX,
		textRect.top - offsetY ,
		m_text,
		m_textColor,
		textRect.right - textRect.left, 
		textRect.bottom - rect.top,
		textFormat
		);

		pRender->GetRenderTarget()->PopAxisAlignedClip(); // 恢复裁剪
	}
}