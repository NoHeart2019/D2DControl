#include "stdafx.h"
#include "D2DControlUI.h"
#include "D2DRender.h"
#include "D2DListUI.h"

#define D2D_ALIGNED_SIZE(size, alignment) ((((int)size + (int)alignment-1)/(int)alignment) * (int)alignment)


CD2DListUI::CD2DListUI()
    : m_fItemHeight(30.0f)
	, m_fBottomPadding(0.0f)
    , m_nSelectedIndex(-1)
    , m_nHoverIndex(-1)
    , m_fScrollPosition(0.0f)
    , m_clickItemCallback(nullptr)
	, m_click2ItemCallback(nullptr)
    , m_IsShowScrollBar(false)
{
    // 初始化垂直滚动条
    m_VerticalScrollBar.SetRange(0.0f, 100.0f);
    m_VerticalScrollBar.SetPageSize(20.0f);
    m_VerticalScrollBar.SetStepSize(1.0f);
    m_VerticalScrollBar.SetThumbSize(20.0f);
    
    // 设置滚动条回调函数
    m_VerticalScrollBar.SetScrollCallback(OnScrollCallback, this);
}

CD2DListUI::~CD2DListUI()
{
    // 清空列表项
    ClearItems();
}

// 滚动回调函数实现
void CD2DListUI::OnScrollCallback(float position, void* userData)
{
    CD2DListUI* pListUI = static_cast<CD2DListUI*>(userData);
    if (pListUI)
    {
		float fScrollPosition = position;//(float)D2D_ALIGNED_SIZE(position, pListUI->GetItemHeight());
		pListUI->m_fScrollPosition = fScrollPosition;
		pListUI->m_VerticalScrollBar.m_position = position;
    }
}

void CD2DListUI::Render(CD2DRender* pRender)
{
	// 绘制背景
	CD2DControlUI::Render(pRender);

	pRender->PushAxisAlignedClip(GetContentRectangle());

	
	/*{
	D2D1_RECT_F rect = GetBorderRectangle();

	if (IsBackgroundVisible())
	{
	pRender->FillRectangle(rect, GetBackgroundColor());
	}

	if (IsBorderVisible())
	{
	pRender->DrawRectangle(rect, GetBorderColor(), GetBorderWidth());
	}
	}*/
	D2D1_RECT_F rectangle = GetContentRectangle();
	// 计算滚动条是否需要显示
	float fTotalHeight = m_items.GetCount() * m_fItemHeight;
	float fVisibleHeight = rectangle.bottom - rectangle.top;
	m_IsShowScrollBar = fTotalHeight > fVisibleHeight;

	// 设置滚动条位置和大小
	if (m_IsShowScrollBar)
	{
		//滚动条放在靠近border的padding区
		D2D1_RECT_F borderRect = GetPaddingRectangle();
		float fScrollBarWidth = 20.0f;
		D2D1_RECT_F scrollBarRect = {
			borderRect.right - fScrollBarWidth,
			borderRect.top + GetPaddingTop(),
			borderRect.right ,
			borderRect.bottom - GetPaddingBottom()
		};
		m_VerticalScrollBar.SetRectangle(scrollBarRect);
		float fMaxScrollPos = max(0.0f, fTotalHeight - fVisibleHeight);
		m_VerticalScrollBar.SetRange(0.0f, fMaxScrollPos);
		m_VerticalScrollBar.SetPageSize(fVisibleHeight);
		m_VerticalScrollBar.SetThumbSize(max(10.0f, fVisibleHeight * fVisibleHeight / fTotalHeight));

		
	}
	else
	{
		m_fScrollPosition = 0.0f;
	}

	// 绘制列表项
	if (m_items.GetCount() > 0)
	{
		// 计算可见项索引范围（基于滚动位置和可视高度）
		float firstItemTop = m_fScrollPosition;
		float lastItemBottom = m_fScrollPosition + fVisibleHeight;
		int nFirstVisibleIndex = static_cast<int>(floor(firstItemTop / m_fItemHeight));
		int nLastVisibleIndex = static_cast<int>(ceil(lastItemBottom / m_fItemHeight)) - 1;

		nFirstVisibleIndex = max(0, nFirstVisibleIndex);
		nLastVisibleIndex = min(static_cast<int>(m_items.GetCount()) - 1, nLastVisibleIndex);

		
		for (int i = nFirstVisibleIndex; i <= nLastVisibleIndex; i++)
		{
			CD2DListItemUI* pItem = m_items[i];
			if (!pItem) continue;

			//计算当前项的矩形区域
			D2D1_RECT_F itemRect = GetItemRect(i);

			pItem->SetRectangle(itemRect);
			pItem->SetSelected(i == m_nSelectedIndex);
			pItem->SetHovered(i == m_nHoverIndex);
			pItem->Render(pRender);
		}
	}

	pRender->PopAxisAlignedClip();

	m_VerticalScrollBar.Render(pRender);
}

void CD2DListUI::AddItem(CD2DListItemUI* pItem)
{
    if (pItem)
    {
        m_items.Add(CAutoPtr<CD2DListItemUI>(pItem));
        // 更新滚动条范围
        float fMaxScrollPos = GetMaxScrollPosition();
        m_VerticalScrollBar.SetRange(0.0f, fMaxScrollPos);
    }
}

void CD2DListUI::RemoveItem(int nIndex)
{
    if (nIndex >= 0 && nIndex < m_items.GetCount())
    {
        // 释放项内存 CAutoPtrArray
        //delete m_items[nIndex];
        // 从数组中移除
        m_items.RemoveAt(nIndex);
        
        // 更新选中索引
        if (m_nSelectedIndex == nIndex)
        {
            m_nSelectedIndex = -1;
        }
        else if (m_nSelectedIndex > nIndex)
        {
            m_nSelectedIndex--;
        }
        
        // 更新悬停索引
        if (m_nHoverIndex == nIndex)
        {
            m_nHoverIndex = -1;
        }
        else if (m_nHoverIndex > nIndex)
        {
            m_nHoverIndex--;
        }
        
        // 更新滚动条范围
        float fMaxScrollPos = GetMaxScrollPosition();
        m_VerticalScrollBar.SetRange(0.0f, fMaxScrollPos);
        
        // 确保滚动位置不超出新的最大范围
        if (m_fScrollPosition > fMaxScrollPos)
        {
            SetScrollPosition(fMaxScrollPos);
        }
    }
}

void CD2DListUI::ClearItems()
{
    // 清空数组
    m_items.RemoveAll();
    
    // 重置选中和悬停索引
    m_nSelectedIndex = -1;
    m_nHoverIndex = -1;
    
    // 重置滚动位置
    m_fScrollPosition = 0.0f;
    
    // 更新滚动条
    m_VerticalScrollBar.SetRange(0.0f, 0.0f);
    m_VerticalScrollBar.SetPosition(0.0f);
    
    // 保留回调函数，不进行重置
}

int CD2DListUI::GetItemCount() const
{
    return (int)m_items.GetCount();
}

CD2DListItemUI* CD2DListUI::GetItem(int nIndex)
{
    if (nIndex >= 0 && nIndex < m_items.GetCount())
    {
        return m_items[nIndex];
    }
    return nullptr;
}

void CD2DListUI::SelectItem(int nIndex)
{
    if (nIndex >= 0 && nIndex < m_items.GetCount())
    {
        m_nSelectedIndex = nIndex;
    }
}

void CD2DListUI::UnselectItem()
{
    m_nSelectedIndex = -1;
}

int CD2DListUI::GetSelectedIndex() const
{
    return m_nSelectedIndex;
}

int CD2DListUI::GetHoverIndex() const
{
    return m_nHoverIndex;
}

void CD2DListUI::SetOnClickItemCallback(ClickItemCallback callback)
{
    m_clickItemCallback = callback;
}

CD2DListUI::ClickItemCallback CD2DListUI::GetOnClickCallback() const
{
    return m_clickItemCallback;
}

void CD2DListUI::SetOnClick2ItemCallback(Click2ItemCallback callback)
{
	m_click2ItemCallback = callback;
}

CD2DListUI::Click2ItemCallback CD2DListUI::GetOnClick2Callback() const
{
	return m_click2ItemCallback;
}

void CD2DListUI::SetItemHeight(float fHeight)
{
    m_fItemHeight = fHeight;
    // 更新滚动条范围
    float fMaxScrollPos = GetMaxScrollPosition();
    m_VerticalScrollBar.SetRange(0.0f, fMaxScrollPos);
}

float CD2DListUI::GetItemHeight() const
{
    return m_fItemHeight;
}

void CD2DListUI::SetItemBottomPadding(float fBottomPadding)
{
	m_fBottomPadding = fBottomPadding;
}

float CD2DListUI::GetItemBottomPadding() const
{
	return m_fBottomPadding;
}

void CD2DListUI::SetFont(const wchar_t* fontFamilyName, float fontSize)
{
    if (fontFamilyName)
    {
        // 创建新的文本格式
        //IDWriteTextFormat* pNewTextFormat = CreateTextFormat(fontFamilyName, fontSize);
        //
        //// 释放旧的文本格式
        //if (m_pTextFormat)
        //{
        //    m_pTextFormat.Release();
        //}
        //
        //// 设置新的文本格式
        //m_pTextFormat = pNewTextFormat;
    }
}

void CD2DListUI::SetScrollPosition(float fScrollPos)
{
    // 限制滚动位置在有效范围内
    float fMaxScrollPos = GetMaxScrollPosition();
    
    // 只有当滚动位置实际改变时才更新
    if (fabs(m_fScrollPosition - fScrollPos) > 0.001f)
    {
        m_fScrollPosition = max(0.0f, min(fScrollPos, fMaxScrollPos));
         // 更新滚动条位置
		m_VerticalScrollBar.m_position = m_fScrollPosition;
    }
}

float CD2DListUI::GetScrollPosition() const
{
    return m_fScrollPosition;
}

float CD2DListUI::GetMaxScrollPosition() const
{
    float fTotalHeight = m_items.GetCount() * m_fItemHeight;
    float fVisibleHeight = m_rectangle.bottom - m_rectangle.top;
    return max(0.0f, fTotalHeight - fVisibleHeight);
}

CD2DScrollBarUI& CD2DListUI::GetVerticalScroll()
{
	return m_VerticalScrollBar;
}


bool CD2DListUI::OnMouseDownL(const D2D1_POINT_2F& point)
{
    // 检查是否在控件范围内
    if (!IsVisible() || !IsEnabled() || !IsPointInRect(point))
    {
        return false;
    }
    
    // 检查是否点击了滚动条
    if (m_IsShowScrollBar && m_VerticalScrollBar.IsHitTest(point))
    {
        return m_VerticalScrollBar.OnMouseDownL(point);
    }
    
    // 检查是否点击了列表项
    int nIndex = GetItemIndexAtPoint(point);
    if (nIndex != -1)
    {
        SelectItem(nIndex);
        // 触发点击回调函数
        if (m_clickItemCallback)
        {
            CD2DListItemUI* pItem = GetItem(nIndex);
            if (pItem)
            {
                m_clickItemCallback(this, pItem);
            }
        }
        return true;
    }
    
    return false;
}

bool CD2DListUI::OnMouseMove(const D2D1_POINT_2F& point)
{
     // 检查是否在控件范围内
    if (!IsVisible() || !IsEnabled() )
    {
        return false;
    }

	bool isInRect = IsPointInRect(point);
	
    // 检查是否在滚动条上
    if (m_IsShowScrollBar)
    {
       if(m_VerticalScrollBar.OnMouseMove(point) == true)
			return true;
    }
	if (isInRect)
	{
		if (!IsFocused())
			SetFocused(true);

	}else 
	{
		m_nHoverIndex = -1;
		if (IsFocused())
		{	
			SetFocused(false);
			return true;
		}

		return false;
    }
    // 检查是否在列表项上
    int nIndex = GetItemIndexAtPoint(point);
    if (nIndex != m_nHoverIndex)
    {
        m_nHoverIndex = nIndex;
		return true;
    }
    
    return false;
}

bool CD2DListUI::OnMouseUpL(const D2D1_POINT_2F& point)
{
    // 检查是否在控件范围内
    if (!IsVisible() || !IsEnabled() )
    {
        return false;
    }
    // 滚动条
    if (m_IsShowScrollBar)
    {
        return m_VerticalScrollBar.OnMouseUpL(point);
    }
	//
	if (IsPointInRect(point))
	{
	}else
	{
		return OnMouseLeave(point);
	}

    
    return false;
}

bool CD2DListUI::OnMouseDownL2(const D2D1_POINT_2F& point)
{
	// 检查是否在控件范围内
	if (!IsVisible() || !IsEnabled() || !IsPointInRect(point))
	{
		return false;
	}

	// 检查是否点击了列表项
	int nIndex = GetItemIndexAtPoint(point);
	if (nIndex != -1)
	{
		SelectItem(nIndex);
		// 触发点击回调函数
		if (m_click2ItemCallback)
		{
			CD2DListItemUI* pItem = GetItem(nIndex);
			if (pItem)
			{
				m_click2ItemCallback(this, pItem);
			}
		}
		return true;
	}

	return false;
}

bool CD2DListUI::OnMouseUpL2(const D2D1_POINT_2F& point)
{
	// 检查是否在控件范围内
	if (!IsVisible() || !IsEnabled() || !IsPointInRect(point))
	{
		return false;
	}
	

	return false;
}

bool CD2DListUI::OnMouseLeave(const D2D1_POINT_2F& point)
{
    // 重置悬停索引
    m_nHoverIndex = -1;
    
    // 处理滚动条的鼠标离开事件
    if (m_IsShowScrollBar)
    {
        m_VerticalScrollBar.OnMouseLeave(point);
    }
    
    return true;
}

bool CD2DListUI::OnMouseWheel(float delta)
{
    // 向上滚动为正，向下滚动为负
    float fNewScrollPos = m_fScrollPosition - delta * 2.0f; // 调整滚动速度
    //SetScrollPosition(fNewScrollPos);
	// 处理滚动条的鼠标离开事件
	if (m_IsShowScrollBar)
	{
		m_VerticalScrollBar.OnMouseWheel(delta);
	}
    return true;
}

D2D1_RECT_F CD2DListUI::GetItemRect(int nIndex) const
{
    D2D1_RECT_F rectangle = GetContentRectangle();
    return D2D1::RectF(
        rectangle.left,
        rectangle.top + nIndex * m_fItemHeight - m_fScrollPosition,
        rectangle.right ,
        rectangle.top + (nIndex + 1) * m_fItemHeight - m_fScrollPosition-m_fBottomPadding
    );
} 

int CD2DListUI::GetItemIndexAtPoint(const D2D1_POINT_2F& point) const
{
	D2D1_RECT_F rectangle = GetContentRectangle();

	// 检查点是否在控件范围内
	if (point.x < rectangle.left || point.x > rectangle.right ||
		point.y < rectangle.top || point.y > rectangle.bottom)
	{
		return -1;
	}


	// 计算点击的项索引（基于理论位置）
	float yRelative = point.y - rectangle.top;               // 相对于内容区域顶部的 y 坐标
	float yWithScroll = yRelative + m_fScrollPosition;       // 加上滚动偏移后的 y 坐标（理论位置）
	int nIndex = static_cast<int>(yWithScroll / m_fItemHeight);

	// 确保索引在有效范围内
	if (nIndex >= 0 && nIndex < m_items.GetCount())
	{
		// 计算该项的实际顶部和底部（相对于内容区域顶部）
		float itemTop = nIndex * m_fItemHeight - m_fScrollPosition;
		float itemBottom = (nIndex + 1) * m_fItemHeight - m_fScrollPosition;

		// 可点击区域为 [itemTop, itemBottom - m_fBottomPadding)
		// 即底部 padding 区域不可点击（用于项间间隔）
		float clickableBottom = itemBottom - m_fBottomPadding;

		// 检查点击位置是否在可点击区域内
		if (yRelative >= itemTop && yRelative < clickableBottom)
		{
			return nIndex;
		}
	}

	return -1;
}