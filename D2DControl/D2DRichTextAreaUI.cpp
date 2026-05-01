#include "stdafx.h"
#include "D2DText.h"
#include "D2DControlUI.h"
#include "D2DRender.h"
#include "D2DScrollBarUI.h"
#include "D2DRichTextAreaUI.h"
#include <algorithm>   // 仅用于 max/min，可改为宏

CD2DRichTextAreaUI::CD2DRichTextAreaUI()
    : m_verticalScrollBar(ScrollBarDirection::Vertical)
    , m_enableVerticalScroll(true)
    , m_scrollBarWidth(12.0f)
    , m_textTotalHeight(0.0f)
    , m_textTotalWidth(0.0f)
    , m_scrollPositionY(0.0f)
    , m_isUpdatingScroll(false)
    , m_lastWidth(0.0f)
    , m_lastHeight(0.0f)
    , m_lineSpacingMultiplier(0.0f)
{
    // 设置控件默认外观
    SetBackgroundColor(D2D1::ColorF(D2D1::ColorF::White));
    SetBorderColor(D2D1::ColorF(D2D1::ColorF::Black));
    SetBorderWidth(1.0f);
    SetBorderVisible(true);
    SetPadding(5.0f, 5.0f, 5.0f, 5.0f);

    // 初始化各样式默认字体
    // 正文
    m_styleRanges[StyleBody].SetFont(L"Microsoft YaHei", 14.0f);
    // H1（居中处理将在布局时完成）
    m_styleRanges[StyleH1].SetFont(L"Microsoft YaHei", 28.0f, DWRITE_FONT_WEIGHT_BOLD);
    // H2
    m_styleRanges[StyleH2].SetFont(L"Microsoft YaHei", 22.0f, DWRITE_FONT_WEIGHT_SEMI_BOLD);
    // H3
    m_styleRanges[StyleH3].SetFont(L"Microsoft YaHei", 18.0f, DWRITE_FONT_WEIGHT_SEMI_BOLD);
    // H4
    m_styleRanges[StyleH4].SetFont(L"Microsoft YaHei", 16.0f, DWRITE_FONT_WEIGHT_SEMI_BOLD);

    // 滚动条回调
    m_verticalScrollBar.SetScrollCallback(OnVerticalScrollCallback, this);
}

CD2DRichTextAreaUI::~CD2DRichTextAreaUI()
{
}

void CD2DRichTextAreaUI::AddText(const CString& text, TextBlockStyle style, bool bNewParagraph)
{
    if (text.IsEmpty())
        return;
   int posion = m_text.GetLength();
   m_text += text;
   DWRITE_TEXT_RANGE rang = {posion, text.GetLength()};
   m_styleRanges[style].AddRange(rang);
    // 标记布局需要重建
   m_pTextLayout.Release();
}

void CD2DRichTextAreaUI::SetText(const CString& text)
{
	CD2DTextFormat::SetText(text);
	m_pTextLayout.Release();
}


void CD2DRichTextAreaUI::Clear()
{
    m_pTextLayout = NULL;
    m_text.Empty();
    for (int i = 0; i < StyleCount; ++i)
        m_styleRanges[i].ClearRanges();
    m_textTotalHeight = 0.0f;
    m_textTotalWidth = 0.0f;
    UpdateScrollBars();
}

void CD2DRichTextAreaUI::SetStyleFont(TextBlockStyle style, LPCTSTR lpszFontName, float fontSize,
                                   DWRITE_FONT_WEIGHT fontWeight, DWRITE_FONT_STYLE fontStyle)
{
    if (style >= 0 && style < StyleCount)
        m_styleRanges[style].SetFont(lpszFontName, fontSize, fontWeight, fontStyle);
    m_pTextLayout = NULL; // 样式改变需重建
}

void CD2DRichTextAreaUI::GetStyleFont(TextBlockStyle style, CString& fontName, float& fontSize,
                                   DWRITE_FONT_WEIGHT& fontWeight, DWRITE_FONT_STYLE& fontStyle) const
{
    if (style >= 0 && style < StyleCount)
    {
        const CStyleRange& sr = m_styleRanges[style];
        fontName = sr.m_fontName;
        fontSize = sr.m_fontSize;
        fontWeight = sr.m_fontWeight;
        fontStyle = sr.m_fontStyle;
    }
}

void CD2DRichTextAreaUI::EnableVerticalScroll(bool enable)
{
    m_enableVerticalScroll = enable;
    m_verticalScrollBar.SetVisible(enable);
    UpdateScrollBars();
}

bool CD2DRichTextAreaUI::IsVerticalScrollEnabled() const
{
    return m_enableVerticalScroll;
}

void CD2DRichTextAreaUI::SetScrollBarWidth(float width)
{
    m_scrollBarWidth = (width > 0) ? width : 16.0f;
    UpdateScrollBars();
}

float CD2DRichTextAreaUI::GetScrollBarWidth() const
{
    return m_scrollBarWidth;
}

CD2DScrollBarUI& CD2DRichTextAreaUI::GetVerticalScroll()
{
    return m_verticalScrollBar;
}

void CD2DRichTextAreaUI::ScrollToTop()
{
    m_scrollPositionY = 0.0f;
    m_verticalScrollBar.SetPosition(0.0f);
}

void CD2DRichTextAreaUI::ScrollToBottom()
{
    D2D1_RECT_F rect = GetTextDisplayRect();
    float availableHeight = rect.bottom - rect.top;
    float maxScrollY = max(0.0f, m_textTotalHeight - availableHeight);
    m_scrollPositionY = maxScrollY;
    m_verticalScrollBar.SetPosition(maxScrollY);
}

void CD2DRichTextAreaUI::ScrollBy(float deltaY)
{
    D2D1_RECT_F rect = GetTextDisplayRect();
    float availableHeight = rect.bottom - rect.top;
    float maxScrollY = max(0.0f, m_textTotalHeight - availableHeight);
    float newPos = m_scrollPositionY + deltaY;
    newPos = max(0.0f, min(newPos, maxScrollY));
    if (newPos != m_scrollPositionY)
    {
        m_scrollPositionY = newPos;
        m_verticalScrollBar.SetPosition(m_scrollPositionY);
    }
}

float CD2DRichTextAreaUI::GetScrollPositionY() const
{
    return m_scrollPositionY;
}

float CD2DRichTextAreaUI::GetTextTotalHeight() const
{
	return m_textTotalHeight;
}

void CD2DRichTextAreaUI::RebuildLayout(CD2DRender* pRender)
{
    // 1. 清空所有范围，重建完整文本
    // 2. 获取文本显示区域的宽度
    D2D1_RECT_F displayRect = GetTextDisplayRect();
    float layoutWidth = max(1.0f, displayRect.right - displayRect.left);

    // 3. 创建 TextLayout（暂不设置格式）
	m_textFormat.Release();
    m_textFormat = pRender->CreateTextFormat(
								GetFontName(), 
								GetFontSize(), 
								GetFontWeight(), 
								GetHorizontalAlignment(), 
								GetVerticalAlignment());
	
	m_pTextLayout.Release();
	m_pTextLayout = pRender->CreateTextLayout(
								m_text, (UINT32)m_text.GetLength(),
								m_textFormat, layoutWidth);

    if (!m_pTextLayout)
        return;

	
    // 4. 为每种样式设置字体属性
    for (int i = 0; i < StyleCount; ++i)
    {
        const CStyleRange& sr = m_styleRanges[i];
        const CAtlArray<DWRITE_TEXT_RANGE>& ranges = sr.m_ranges;
        if (ranges.GetCount() == 0)
            continue;

        // 设置字体族、大小、粗细、样式
        for (size_t j = 0; j < ranges.GetCount(); ++j)
        {
            const DWRITE_TEXT_RANGE& range = ranges[j];
            m_pTextLayout->SetFontFamilyName(sr.m_fontName, range);
            m_pTextLayout->SetFontSize(sr.m_fontSize, range);
            m_pTextLayout->SetFontWeight(sr.m_fontWeight, range);
            m_pTextLayout->SetFontStyle(sr.m_fontStyle, range);
        }
    }

    
	// 5. 设置全局行间距
	if (m_lineSpacingMultiplier > 0.0f)
	{
		FLOAT fontSize = GetFontSize();
		FLOAT approximateAscent = fontSize * 0.8f; 
		SetLineSpacing(m_lineSpacingMultiplier);
		float line = (GetLineSpacing() > fontSize ?
			GetLineSpacing() :
		fontSize * GetLineSpacing() * pRender->GetDpiScaleY());

		FLOAT baselineOffset = (line - fontSize) / 2.0f + approximateAscent;

		m_pTextLayout->SetLineSpacing(
			DWRITE_LINE_SPACING_METHOD_UNIFORM,
			line,
			baselineOffset
			);
	}
	

    // 6. 获取实际尺寸
    DWRITE_TEXT_METRICS metrics;
    if (SUCCEEDED(m_pTextLayout->GetMetrics(&metrics)))
    {
        m_textTotalHeight = metrics.height;
        m_textTotalWidth = metrics.width;
    }
    else
    {
        m_textTotalHeight = 0.0f;
        m_textTotalWidth = 0.0f;
    }

	if (!IsVerticalScrollEnabled())
	{
		SetContentSize(layoutWidth, m_textTotalHeight);
	}

    UpdateScrollBars();
}

void CD2DRichTextAreaUI::Render(CD2DRender* pRender)
{
    if (!pRender || !IsVisible())
        return;

    // 绘制背景和边框
    D2D1_RECT_F rect = GetBorderRectangle();
    CD2DControlUI::Render(pRender);

    // 检查控件大小是否变化
    D2D1_SIZE_F sz = GetSize();
    if (sz.width != m_lastWidth || sz.height != m_lastHeight)
    {
        m_lastWidth = sz.width;
        m_lastHeight = sz.height;
        m_pTextLayout = NULL; // 强制重建布局
    }

    // 确保布局存在
    if (!m_pTextLayout)
        RebuildLayout(pRender);

    // 更新滚动条
    UpdateScrollBars();

    // 绘制垂直滚动条
    m_verticalScrollBar.Render(pRender);

    // 绘制文本（应用滚动偏移和裁剪）
    if (m_pTextLayout)
    {
        D2D1_RECT_F textRect = GetTextDisplayRect();
        pRender->PushAxisAlignedClip(textRect);

        float drawX = textRect.left;
        float drawY = textRect.top - m_scrollPositionY;
		
		auto brush = pRender->CreateBrush(GetTextColor());
        pRender->GetRenderTarget()->DrawTextLayout(D2D1::Point2F(drawX, drawY), m_pTextLayout, brush);

        pRender->PopAxisAlignedClip();
    }
}

void CD2DRichTextAreaUI::SetSize(float width, float height)
{
    CD2DControlUI::SetSize(width, height);
    m_pTextLayout = NULL; // 尺寸变化需重建布局
    UpdateScrollBars();
}

bool CD2DRichTextAreaUI::OnMouseWheel(float delta)
{
    if (!IsEnabled() || !IsVisible())
        return false;
    if (m_verticalScrollBar.IsVisible())
        return m_verticalScrollBar.OnMouseWheel(delta);
    return false;
}

bool CD2DRichTextAreaUI::OnMouseDownL(const D2D1_POINT_2F& point)
{
    if (!IsEnabled() || !IsVisible())
        return false;

    if (m_verticalScrollBar.OnMouseDownL(point))
        return true;
    
	if (IsPointInRect(point))
        return true;
    
    return false;
}

bool CD2DRichTextAreaUI::OnMouseMove(const D2D1_POINT_2F& point)
{
    if (!IsEnabled() || !IsVisible())
        return false;
	
    bool handled = false;
	bool isInside = IsPointInRect(point); //移入
	if (isInside)
	{
		if (m_verticalScrollBar.IsVisible())
			handled = m_verticalScrollBar.OnMouseMove(point);
		if (handled)
			return true;
	}else
	{
		return OnMouseLeave(point);			
	}

    return handled ;
}

bool CD2DRichTextAreaUI::OnMouseUpL(const D2D1_POINT_2F& point)
{
    if (!IsEnabled() || !IsVisible())
        return false;
    bool handled = false;
    if (m_verticalScrollBar.IsVisible())
        handled = m_verticalScrollBar.OnMouseUpL(point) || handled;

    return handled;
}

bool CD2DRichTextAreaUI::OnMouseLeave(const D2D1_POINT_2F& point)
{
    if (!IsEnabled() || !IsVisible())
        return false;
   
    //处理滚动条
	if (m_verticalScrollBar.IsVisible())
		m_verticalScrollBar.OnMouseLeave(point);

    return true;
}

void CD2DRichTextAreaUI::UpdateScrollBars()
{
    D2D1_RECT_F rect = GetTextDisplayRect();
    float availableHeight = rect.bottom - rect.top;

    if (m_enableVerticalScroll)
    {
        bool needVerticalScroll = (m_textTotalHeight > availableHeight);
        if (needVerticalScroll)
        {
            float maxScrollY = m_textTotalHeight - availableHeight;
            m_verticalScrollBar.SetRange(0.0f, maxScrollY);
            m_verticalScrollBar.SetPageSize(availableHeight);
            m_verticalScrollBar.SetPosition(m_scrollPositionY);

            // 设置滚动条位置和大小
            D2D1_RECT_F controlRect = GetBorderRectangle();
            controlRect.top += GetPaddingTop();
            controlRect.bottom -= GetPaddingBottom();
            float scrollBarX = (controlRect.right - controlRect.left) - m_scrollBarWidth-2.0f;
            m_verticalScrollBar.SetPosition(controlRect.left + scrollBarX, controlRect.top);
            m_verticalScrollBar.SetSize(m_scrollBarWidth, controlRect.bottom - controlRect.top);
            m_verticalScrollBar.SetVisible(true);
        }
        else
        {
            m_verticalScrollBar.SetVisible(false);
            m_scrollPositionY = 0.0f;
            m_verticalScrollBar.SetPosition(0.0f);
        }
    }
    else
    {
        m_verticalScrollBar.SetVisible(false);
        m_scrollPositionY = 0.0f;
    }
}

D2D1_RECT_F CD2DRichTextAreaUI::GetTextDisplayRect() const
{
    D2D1_RECT_F rect = GetContentRectangle(); // 扣除内边距的区域
	//滚动条在content矩形的外面Padding处
	 /*if (m_verticalScrollBar.IsVisible())
	rect.right -= m_scrollBarWidth;*/
    return rect;
}

void CD2DRichTextAreaUI::OnVerticalScrollCallback(float position, void* userData)
{
    if (userData)
    {
        CD2DRichTextAreaUI* pThis = static_cast<CD2DRichTextAreaUI*>(userData);
        pThis->OnVerticalScroll(position);
    }
}

void CD2DRichTextAreaUI::OnVerticalScroll(float position)
{
    if (m_isUpdatingScroll)
        return;
    m_isUpdatingScroll = true;
    m_scrollPositionY = position;
    m_isUpdatingScroll = false;
}