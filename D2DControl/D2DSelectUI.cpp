#include "stdafx.h"
#include "D2D.h"
#include "D2DSelectUI.h"

CD2DSelectUI::CD2DSelectUI()
    : m_curSel(-1)
    , m_isDropDownVisible(false)
    , m_dropDownMaxHeight(200.0f)
    , m_itemHeight(30.0f)
    , m_buttonWidth(30.0f)
    , m_selectedBgColor(D2D1::ColorF(0.2f, 0.5f, 0.9f, 0.8f))
    , m_buttonColor(D2D1::ColorF(0.9f, 0.9f, 0.9f, 1.0f))
    , m_buttonBorderColor(D2D1::ColorF(0.5f, 0.5f, 0.5f, 1.0f))
    , m_hoverItemIndex(-1)
    , m_isDropDownClicked(false)
    , m_scrollOffset(0.0f)
    , m_needScrollBar(false)
    , m_onItemClickCallback(nullptr)
{
    // 设置默认背景色和边框
    SetBackgroundColor(D2D1::ColorF(D2D1::ColorF::White));
    SetBorderColor(D2D1::ColorF(D2D1::ColorF::Black));
    SetBorderWidth(1.0f);
    SetBorderVisible(true);
    SetBackgroundVisible(true);

    // 使用CD2DTextUI设置默认字体和颜色
    SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    // 创建垂直滚动条
    m_verticalScroll.SetVisible(true);
    m_verticalScroll.SetScrollCallback(OnScrollCallback, this);
    m_verticalScroll.SetTrackWidth(10.0f);
    m_verticalScroll.SetThumbSize(40.0f);
	m_verticalScroll.SetDirection(ScrollBarDirection::Vertical);

	SetMarginLeft(10.0f);
}

CD2DSelectUI::~CD2DSelectUI()
{
    
}


void CD2DSelectUI::Render(CD2DRender* pRender)
{
	if (!pRender || !IsVisible())
		return;

	D2D1_RECT_F controlRect = GetRectangle();

	// 绘制主控件背景和边框
	if (IsBackgroundVisible())
	{
		pRender->FillRectangle(controlRect, GetBackgroundColor());
	}

	if (IsBorderVisible())
	{
		pRender->DrawRectangle(controlRect, GetBorderColor(), GetBorderWidth());
	}

	// 计算文本区域和按钮区域（局部变量，不存储为成员）
	//D2D1_RECT_F textRect = controlRect;
	D2D1_RECT_F textRect = GetContentRectangle();
	
	D2D1_RECT_F buttonRect = controlRect;

	textRect.right -= m_buttonWidth;
	textRect.left += 5.0f; // 左侧内边距

	buttonRect.left = buttonRect.right - m_buttonWidth;

	// 绘制下拉按钮
	DrawDropDownButton(pRender, buttonRect);

	// 绘制选中项文本
	DrawSelectedText(pRender, textRect);

	// 如果下拉列表可见，绘制下拉列表
	if (m_isDropDownVisible)
	{
		UpdateScrollBar();          // 更新滚动条状态（内部使用实时矩形）
		DrawDropDownList(pRender);
	}
}

// 滚动回调函数实现
void CD2DSelectUI::OnScrollCallback(float position, void* userData)
{
    CD2DSelectUI* pThis = static_cast<CD2DSelectUI*>(userData);
    if (pThis)
    {
        pThis->m_scrollOffset = position;
    }
}

// 更新滚动条
void CD2DSelectUI::UpdateScrollBar()
{
    if (!m_isDropDownVisible)
        return;

    D2D1_RECT_F dropDownRect = GetDropDownRect();
    float totalHeight = m_items.GetCount() * m_itemHeight;
    float visibleHeight = dropDownRect.bottom - dropDownRect.top;

    // 计算是否需要显示滚动条
    m_needScrollBar = (totalHeight > visibleHeight);
    m_verticalScroll.SetVisible(m_needScrollBar);

    if (m_needScrollBar)
    {
        // 设置滚动条参数
        m_verticalScroll.SetRange(0.0f, totalHeight - visibleHeight);
        m_verticalScroll.SetPageSize(visibleHeight);
        m_verticalScroll.SetStepSize(m_itemHeight);

        // 设置滚动条位置和大小
        D2D1_RECT_F scrollBarRect = dropDownRect;
        scrollBarRect.left = scrollBarRect.right - m_verticalScroll.GetTrackWidth()/2.0f - m_verticalScroll.GetBorderWidth();
        m_verticalScroll.SetRectangle(scrollBarRect);

        // 确保滚动位置在有效范围内
        if (m_scrollOffset > totalHeight - visibleHeight)
        {
            m_scrollOffset = totalHeight - visibleHeight;
            m_verticalScroll.SetPosition(m_scrollOffset);
        }
    }
    else
    {
        // 不需要滚动条时，重置滚动偏移
        m_scrollOffset = 0.0f;
        m_verticalScroll.SetPosition(0.0f);
    }
}

// 根据滚动位置计算可见项范围
void CD2DSelectUI::GetVisibleItemsRange(int& startIndex, int& endIndex) const
{
    if (!m_isDropDownVisible)
    {
        startIndex = -1;
        endIndex = -1;
        return;
    }

    D2D1_RECT_F dropDownRect = GetDropDownRect();
    float visibleHeight = dropDownRect.bottom - dropDownRect.top;

    startIndex = static_cast<int>(m_scrollOffset / m_itemHeight);
    int visibleItems = static_cast<int>(ceil(visibleHeight / m_itemHeight));
    endIndex = min(startIndex + visibleItems, (int)m_items.GetCount() - 1);

    startIndex = max(0, startIndex);
    endIndex = max(-1, endIndex);
}

// 滚动到指定项
void CD2DSelectUI::ScrollToItem(int index)
{
    if ( !m_isDropDownVisible || index < 0 || index >= (int)m_items.GetCount())
        return;

    D2D1_RECT_F dropDownRect = GetDropDownRect();
    float visibleHeight = dropDownRect.bottom - dropDownRect.top;
    float itemPosition = index * m_itemHeight;
    float totalHeight = m_items.GetCount() * m_itemHeight;

    if (itemPosition < m_scrollOffset)
    {
        m_scrollOffset = itemPosition;
    }
    else if (itemPosition + m_itemHeight > m_scrollOffset + visibleHeight)
    {
        m_scrollOffset = itemPosition + m_itemHeight - visibleHeight;
    }

    m_scrollOffset = max(0.0f, min(m_scrollOffset, totalHeight - visibleHeight));
    m_verticalScroll.SetPosition(m_scrollOffset);
}



bool CD2DSelectUI::IsPointInRect(const D2D1_POINT_2F& point) const
{
    D2D1_RECT_F rect = m_rectangle;
    if (m_isDropDownVisible)
    {
        D2D1_RECT_F dropDownRect = GetDropDownRect();
        rect.bottom = dropDownRect.bottom;  // 扩展区域包含下拉列表
    }
    return CD2DControlUI::IsPointInRect(point, rect);
}

bool CD2DSelectUI::OnMouseDownL(const D2D1_POINT_2F& point)
{
    if (!IsVisible() || !IsEnabled())
        return false;

    D2D1_RECT_F controlRect = GetRectangle();
    D2D1_RECT_F buttonRect = controlRect;
    buttonRect.left = buttonRect.right - m_buttonWidth;
    D2D1_RECT_F textRect = controlRect;
    textRect.right -= m_buttonWidth;

    // 检查是否点击了下拉按钮
    if (CD2DControlUI::IsPointInRect(point, buttonRect))
    {
        m_isDropDownClicked = true;
        ShowDropDown(!m_isDropDownVisible);
        return true;
    }

    // 检查是否点击了文本框区域（非按钮）
    if (CD2DControlUI::IsPointInRect(point, textRect))
    {
        if (!m_isDropDownVisible)
        {
            ShowDropDown(true);  // 关闭时点击文本框打开下拉列表
        }
        // 若已打开，保持打开状态
        return true;
    }

    // 检查是否点击了下拉列表区域（仅当下拉列表可见时）
    if (m_isDropDownVisible)
    {
        // 首先检查是否点击了滚动条
        if (m_needScrollBar && m_verticalScroll.IsVisible())
        {
            if (m_verticalScroll.OnMouseDownL(point))
            {
                return true;
            }
        }

        D2D1_RECT_F dropDownRect = GetDropDownRect();
        if (CD2DControlUI::IsPointInRect(point, dropDownRect))
        {
            int itemIndex = GetItemIndexFromPoint(point);
            if (itemIndex >= 0 && itemIndex < (int)m_items.GetCount())
            {
                m_curSel = itemIndex;
                if (m_onItemClickCallback)
                {
                    m_onItemClickCallback(this, itemIndex, GetUserData());
                }
                ShowDropDown(false);
                m_hoverItemIndex = -1;
                return true;
            }
        }
        else
        {
            // 点击了下拉列表外的区域，关闭下拉列表
            ShowDropDown(false);
            m_hoverItemIndex = -1;
        }
    }

    return false;
}

bool CD2DSelectUI::OnMouseMove(const D2D1_POINT_2F& point)
{
    if (!IsVisible() || !IsEnabled() || !IsPointInRect(point))
        return false;
		ZxDebugEx((__FUNCTION__"()\n"));
    if (m_isDropDownVisible)
    {
		if (m_needScrollBar && m_verticalScroll.IsVisible() && m_verticalScroll.OnMouseMove(point))
			return true;

        D2D1_RECT_F dropDownRect = GetDropDownRect();
        if (CD2DControlUI::IsPointInRect(point, dropDownRect))
        {
            m_hoverItemIndex = GetItemIndexFromPoint(point);
            return true;
        }
        else
        {
            m_hoverItemIndex = -1;
        }
    }

    return true;
}

bool CD2DSelectUI::OnMouseUpL(const D2D1_POINT_2F& point)
{
    if (!IsVisible() || !IsEnabled())
        return false;

    if (!IsPointInRect(point))
    {
        m_isDropDownClicked = false;
        return true;
    }

    m_isDropDownClicked = false;

    if (m_needScrollBar &&  m_verticalScroll.IsVisible())
    {
        if (m_verticalScroll.OnMouseUpL(point))
        {
            return true;
        }
    }

    return CD2DControlUI::OnMouseUpL(point);
}

bool CD2DSelectUI::OnMouseLeave(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || !IsEnabled())
		return false;

    m_hoverItemIndex = -1;

    if (m_needScrollBar &&  m_verticalScroll.IsVisible())
    {
        m_verticalScroll.OnMouseLeave(point);
    }

    return CD2DControlUI::OnMouseLeave(point);
}

bool CD2DSelectUI::OnMouseWheel(float delta)
{
    if (!IsEnabled() || !m_isDropDownVisible || !m_needScrollBar ||  !m_verticalScroll.IsVisible())
        return false;

    if (m_verticalScroll.OnMouseWheel(delta))
    {
        m_scrollOffset = m_verticalScroll.GetPosition();
        return true;
    }

    return false;
}

void CD2DSelectUI::AddItem(const CString& text)
{
    m_items.Add(text);
    if (m_curSel == -1 && !text.IsEmpty())
    {
        m_curSel = 0;
    }
}

void CD2DSelectUI::InsertItem(int index, const CString& text)
{
    if (index >= 0 && index <= (int)m_items.GetCount())
    {
        m_items.InsertAt(index, text);
        if (m_curSel >= index)
        {
            m_curSel++;
        }
        if (m_curSel == -1 && !text.IsEmpty())
        {
            m_curSel = index;
        }
    }
}

void CD2DSelectUI::RemoveItem(int index)
{
    if (index >= 0 && index < (int)m_items.GetCount())
    {
        m_items.RemoveAt(index);
        if (index == m_curSel || (m_curSel > index && m_curSel >= (int)m_items.GetCount()))
        {
            m_curSel = m_items.GetCount() > 0 ? (m_curSel > 0 ? m_curSel - 1 : 0) : -1;
        }
    }
}

void CD2DSelectUI::ClearItems()
{
    m_items.RemoveAll();
    m_curSel = -1;
}

int CD2DSelectUI::GetItemCount() const
{
    return (int)m_items.GetCount();
}

CString CD2DSelectUI::GetItemText(int index) const
{
    if (index >= 0 && index < (int)m_items.GetCount())
    {
        return m_items[index];
    }
    return CString();
}

void CD2DSelectUI::SetItemText(int index, const CString& text)
{
    if (index >= 0 && index < (int)m_items.GetCount())
    {
        m_items[index] = text;
    }
}

void CD2DSelectUI::SetCurSel(int index)
{
    if (index >= -1 && index < (int)m_items.GetCount())
    {
        m_curSel = index;
        if (m_onItemClickCallback && m_curSel >= 0)
        {
            m_onItemClickCallback(this, m_curSel, GetUserData());
        }
    }
}

int CD2DSelectUI::GetCurSel() const
{
    return m_curSel;
}

void CD2DSelectUI::SetCurSelText(const CString& text)
{
    for (int i = 0; i < (int)m_items.GetCount(); ++i)
    {
        if (m_items[i] == text)
        {
            SetCurSel(i);
            break;
        }
    }
}

CString CD2DSelectUI::GetCurSelText() const
{
    return GetItemText(m_curSel);
}

void CD2DSelectUI::ShowDropDown(bool show)
{
    m_isDropDownVisible = show && (m_items.GetCount() > 0);
    m_hoverItemIndex = -1;
    if (show)
    {
        UpdateScrollBar();  // 显示时立即更新滚动条
    }
}

bool CD2DSelectUI::IsDropDownVisible() const
{
    return m_isDropDownVisible;
}

void CD2DSelectUI::SetItemHeight(float height)
{
    m_itemHeight = height;
}

void CD2DSelectUI::SetDropDownMaxHeight(float height)
{
    m_dropDownMaxHeight = (height > 0) ? height : 200.0f;
}

float CD2DSelectUI::GetDropDownMaxHeight() const
{
    return m_dropDownMaxHeight;
}


void CD2DSelectUI::SetButtonColor(const D2D1_COLOR_F& color)
{
    m_buttonColor = color;
}

D2D1_COLOR_F CD2DSelectUI::GetButtonColor() const
{
    return m_buttonColor;
}

void CD2DSelectUI::SetButtonBorderColor(const D2D1_COLOR_F& color)
{
    m_buttonBorderColor = color;
}

D2D1_COLOR_F CD2DSelectUI::GetButtonBorderColor() const
{
    return m_buttonBorderColor;
}

CD2DScrollBarUI& CD2DSelectUI::GetVerticalScroll()
{
	return m_verticalScroll;
}

void CD2DSelectUI::SetOnItemClickCallback(OnItemClickCallback callback, void* userData)
{
    m_onItemClickCallback = callback;
	SetUserData(userData);
}

D2D1_RECT_F CD2DSelectUI::GetDropDownRect() const
{
    D2D1_RECT_F controlRect = GetRectangle();
    D2D1_RECT_F dropDownRect = controlRect;

    float actualHeight = min(m_itemHeight * m_items.GetCount(), m_dropDownMaxHeight);
    dropDownRect.top = controlRect.bottom;
    dropDownRect.bottom = dropDownRect.top + actualHeight;

    return dropDownRect;
}

D2D1_RECT_F CD2DSelectUI::GetItemRect(int index) const
{
    if (index < 0 || index >= (int)m_items.GetCount())
    {
        return D2D1::RectF(0, 0, 0, 0);
    }

    D2D1_RECT_F dropDownRect = GetDropDownRect();
    D2D1_RECT_F itemRect = dropDownRect;

    itemRect.top += index * m_itemHeight;
    itemRect.bottom = itemRect.top + m_itemHeight;

    return itemRect;
}

int CD2DSelectUI::GetItemIndexFromPoint(const D2D1_POINT_2F& point) const
{
    if (!m_isDropDownVisible)
        return -1;

    D2D1_RECT_F dropDownRect = GetDropDownRect();
    if (!CD2DControlUI::IsPointInRect(point, dropDownRect))
        return -1;

    int index = static_cast<int>((point.y - dropDownRect.top) / m_itemHeight);
    if (m_needScrollBar)
    {
        index += static_cast<int>(m_scrollOffset / m_itemHeight);
    }

    if (index >= 0 && index < (int)m_items.GetCount())
    {
        return index;
    }

    return -1;
}

void CD2DSelectUI::DrawDropDownButton(CD2DRender* pRender, const D2D1_RECT_F& buttonRect)
{
    // 绘制下拉箭头（简单线框）
    float arrowCenterX = (buttonRect.left + buttonRect.right) / 2.0f;
    float arrowCenterY = (buttonRect.top + buttonRect.bottom) / 2.0f;
    float arrowSize = 6.0f;

    D2D1_POINT_2F arrowPoints[3];
    arrowPoints[0] = D2D1::Point2F(arrowCenterX - arrowSize, arrowCenterY - arrowSize / 2);
    arrowPoints[1] = D2D1::Point2F(arrowCenterX + arrowSize, arrowCenterY - arrowSize / 2);
    arrowPoints[2] = D2D1::Point2F(arrowCenterX, arrowCenterY + arrowSize / 2);

    pRender->DrawLine(arrowPoints[0], arrowPoints[2], GetTextColor(), GetBorderWidth());
    pRender->DrawLine(arrowPoints[2], arrowPoints[1], GetTextColor(), GetBorderWidth());
}

void CD2DSelectUI::DrawDropDownList(CD2DRender* pRender)
{
    if (!m_isDropDownVisible || m_items.IsEmpty())
        return;

    D2D1_RECT_F dropDownRect = GetDropDownRect();

    // 绘制下拉列表背景和边框
    pRender->FillRectangle(dropDownRect, GetBackgroundColor());
    pRender->DrawRectangle(dropDownRect, GetBorderColor(), GetBorderWidth());

    // 设置裁剪区域，防止绘制超出列表范围
    pRender->GetRenderTarget()->PushAxisAlignedClip(dropDownRect, D2D1_ANTIALIAS_MODE_ALIASED);

    int startIndex, endIndex;
    GetVisibleItemsRange(startIndex, endIndex);

    for (int i = startIndex; i <= endIndex; i++)
    {
        D2D1_RECT_F itemRect = GetItemRect(i);
        // 根据滚动偏移调整绘制位置
        itemRect.top -= m_scrollOffset;
        itemRect.bottom -= m_scrollOffset;

        // 绘制选中项或悬停项的背景
        if (i == m_curSel)
        {
            pRender->FillRectangle(itemRect, GetSelectedColor());
        }
        else if (i == m_hoverItemIndex)
        {
            D2D1_COLOR_F hoverColor = GetSelectedColor();
            hoverColor.a = 0.5f;
            pRender->FillRectangle(itemRect, hoverColor);
        }

        // 创建文本格式
        CComPtr<IDWriteTextFormat> textFormat = pRender->CreateTextFormat(
            GetFontName(),
            GetFontSize(),
            GetFontWeight(),
            GetHorizontalAlignment(),
            GetVerticalAlignment()
        );

        // 绘制选项文本
		itemRect.left += GetMarginLeft();
		itemRect.right -= GetMarginRight();
        pRender->DrawText(m_items[i], itemRect, GetTextColor(), textFormat);
    }

    pRender->GetRenderTarget()->PopAxisAlignedClip();

    // 绘制滚动条
    if (m_needScrollBar )
    {
        m_verticalScroll.Render(pRender);
    }
}

void CD2DSelectUI::DrawSelectedText(CD2DRender* pRender, const D2D1_RECT_F& textRect)
{
    if (m_curSel >= 0 && m_curSel < (int)m_items.GetCount())
    {
        CComPtr<IDWriteTextFormat> textFormat = pRender->CreateTextFormat(
            GetFontName(),
            GetFontSize(),
            GetFontWeight(),
            GetHorizontalAlignment(),
            GetVerticalAlignment()
        );

        pRender->DrawText(m_items[m_curSel], textRect, GetTextColor(), textFormat);
    }
}