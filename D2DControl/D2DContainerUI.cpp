#include "stdafx.h"
#include "D2D.h"

//垂直滚动条

void CD2DContainerUI::OnScrollCallback(float position, void* userData)
{
	CD2DContainerUI* This = static_cast<CD2DContainerUI*>(userData);
	if (This)
	{
		float fScrollPosition = position;
		This->m_fScrollPosition = fScrollPosition;
		This->Relayout();
		ZxDebug((__FUNCTION__"()::fScrollPostion = %.2f \n", fScrollPosition));

	}
}

CD2DContainerUI::CD2DContainerUI()
	: m_enableVerticalScroll(true)
	, m_fScrollPosition(0.0f)
	, m_fTotalHeight(0.0f)
{
	AddControlUI(&m_verticalScroll);
	m_verticalScroll.SetVisible(false);
	m_verticalScroll.SetScrollCallback(OnScrollCallback, this);

}

CD2DContainerUI::~CD2DContainerUI()
{

}

void CD2DContainerUI::Render(CD2DRender* pRender)
{
	if (!pRender || !IsVisible()) return;
	
	CD2DControlUI::Render(pRender);
	// 绘制背景和边框
	/*D2D1_RECT_F rect = GetBorderRectangle();

	if (IsBackgroundVisible())
	{
	pRender->FillRectangle(rect, GetBackgroundColor());
	}

	if (IsBorderVisible())
	{
	pRender->DrawRectangle(rect, GetBorderColor(), GetBorderWidth());
	}*/

}

bool CD2DContainerUI::OnMouseDownL(const D2D1_POINT_2F& point)
{
	bool handled = false;
	
	if (!IsVisible()|| !IsEnabled() || !IsHitTest(point)) return false;

	//确保前面的控件先接收到事件
	for (int i = 0; i < (int)m_controls.GetCount(); i++)
	{
		CD2DControlUI* control = m_controls[i];
		if (!control->IsVisible() || !control->IsEnabled())
			continue;

		bool result = false;
		result = control->OnMouseDownL(point);
		if (result)
		{
			handled = true;
			break;
		}
	}

	return handled;
}

bool CD2DContainerUI::OnMouseUpL(const D2D1_POINT_2F& point)
{
	bool handled = false;
	if (!IsVisible()|| !IsEnabled()) return false;

	// 从后往前遍历控件，确保前面的控件先接收到事件
	for (int i = (int)m_controls.GetCount() - 1; i >= 0; i--)
	{
		CD2DControlUI* control = m_controls[i];
		if (!control->IsVisible() || !control->IsEnabled())
			continue;

		bool result = false;
		result = control->OnMouseUpL(point);

		if (result)
		{
			handled = true;
			//InvalidateRect(NULL, FALSE);
		}
	}

	return handled;
}

bool CD2DContainerUI::OnMouseDownL2(const D2D1_POINT_2F& point)
{
	bool handled = false;

	if (!IsVisible()|| !IsEnabled() || !IsHitTest(point)) return false;

	//确保前面的控件先接收到事件
	for (int i = 0; i < (int)m_controls.GetCount(); i++)
	{
		CD2DControlUI* control = m_controls[i];
		if (!control->IsVisible() || !control->IsEnabled())
			continue;

		bool result = false;
		result = control->OnMouseDownL2(point);
		if (result)
		{
			handled = true;
			break;
		}
	}

	return handled;
}

bool CD2DContainerUI::OnMouseUpL2(const D2D1_POINT_2F& point)
{
	bool handled = false;
	if (!IsVisible()|| !IsEnabled()) return false;

	// 从后往前遍历控件，确保前面的控件先接收到事件
	for (int i = (int)m_controls.GetCount() - 1; i >= 0; i--)
	{
		CD2DControlUI* control = m_controls[i];
		if (!control->IsVisible() || !control->IsEnabled())
			continue;

		bool result = false;
		result = control->OnMouseUpL2(point);

		if (result)
		{
			handled = true;
			//InvalidateRect(NULL, FALSE);
		}
	}

	return handled;
}

bool CD2DContainerUI::OnMouseMove(const D2D1_POINT_2F& point)
{
	bool handled = false;
	if (!IsVisible()|| !IsEnabled()) return false;

	// 从后往前遍历控件，确保前面的控件先接收到事件
	for (int i = (int)m_controls.GetCount() - 1; i >= 0; i--)
	{
		CD2DControlUI* control = m_controls[i];
		if (!control->IsVisible() || !control->IsEnabled())
			continue;

		bool result = false;
		result = control->OnMouseMove(point);
		if (result)
		{ 
			handled = true;
			break;
		}
	}

	return handled;
}

bool CD2DContainerUI::OnMouseLeave(const D2D1_POINT_2F& point)
{
	bool handled = false;
	if (!IsVisible()|| !IsEnabled()) return false;

	// 从后往前遍历控件，确保前面的控件先接收到事件
	for (int i = (int)m_controls.GetCount() - 1; i >= 0; i--)
	{
		CD2DControlUI* control = m_controls[i];
		if (!control->IsVisible() || !control->IsEnabled())
			continue;

		bool result = false;
		result = control->OnMouseLeave(point);
		if (result)
		{
			handled = true;
			//InvalidateRect(NULL, FALSE);
		}
	}

	return handled;
}

bool CD2DContainerUI::OnKeyDown(DWORD keyCode)
{
	bool handled = false;
	if (!IsVisible()|| !IsEnabled()) return false;

	//确保前面的控件先接收到事件
	for (int i = 0; i < (int)m_controls.GetCount(); i++)
	{
		CD2DControlUI* control = m_controls[i];
		if (!control->IsVisible() || !control->IsEnabled())
			continue;
		
		//判断是否是编辑框，用动态转换类CD2DEditUI
		/*CD2DEditUI* pEdit = dynamic_cast<CD2DEditUI*>(control);
		if (pEdit != nullptr)
		{*/
			bool result = false;
			result = control->OnKeyDown(keyCode);
			if (result)
			{
				handled = true;
				break;
			}
		//}
		
		
	}
	return handled;
}

bool CD2DContainerUI::OnKeyUp(DWORD keyCode)
{
	bool handled = false;
	if (!IsVisible()|| !IsEnabled()) return false;

	//确保前面的控件先接收到事件
	for (int i = 0; i < (int)m_controls.GetCount(); i++)
	{
		CD2DControlUI* control = m_controls[i];
		if (!control->IsVisible() || !control->IsEnabled())
			continue;

		bool result = false;
		result = control->OnKeyUp(keyCode);
		if (result)
		{
			handled = true;
			break;
		}
	}
	return handled;
}

bool CD2DContainerUI::OnChar(DWORD ch)
{
	bool handled = false;
	if (!IsVisible()|| !IsEnabled()) return false;

	//确保前面的控件先接收到事件
	for (int i = 0; i < (int)m_controls.GetCount(); i++)
	{
		CD2DControlUI* control = m_controls[i];
		if (!control->IsVisible() || !control->IsEnabled())
			continue;
			
		bool result = false;
		result = control->OnChar(ch);
		if (result)
		{
			handled = true;
			break;
		}
	}
	return handled;
}

void CD2DContainerUI::AddControlUI(CD2DControlUI* controlUI)
{
	m_controls.Add(controlUI);
}

int CD2DContainerUI::GetControlUICount() const
{
	return (int)m_controls.GetCount();
}

CD2DControlUI* CD2DContainerUI::GetControlUIAt(int nIndex) 
{
	return m_controls[nIndex];
}

void CD2DContainerUI::RemoveAll()
{
	m_controls.RemoveAll();
}

CD2DScrollBarUI& CD2DContainerUI::GetVerticalScroll()
{
	return m_verticalScroll;
}

const CD2DScrollBarUI& CD2DContainerUI::GetVerticalScroll() const
{
	return m_verticalScroll;
}

void CD2DContainerUI::SetTotalHeight(float fTotalHeight)
{

	m_fTotalHeight = fTotalHeight;
}

float CD2DContainerUI::GetTotalHeight() const
{
	return m_fTotalHeight;
}

void  CD2DContainerUI::SetVerticalPosition(float fPosition)
{
	m_fScrollPosition = fPosition;
	m_verticalScroll.SetPosition(fPosition);
}

float CD2DContainerUI::GetVerticalPosition()
{
	return m_fScrollPosition;
}

void CD2DContainerUI::SetVerticalScroll()
{
	float fVisibleHeight = GetBorderRectangle().bottom - GetBorderRectangle().top;
	bool IsShowScrollBar = m_fTotalHeight > fVisibleHeight;

	//滚动条是否开启
	if (!m_enableVerticalScroll)
		return;

	// 设置滚动条位置和大小
	if (IsShowScrollBar)
	{
		// 计算列表内容区域（减去滚动条宽度）
		float fScrollBarWidth = 12.0f;
		D2D1_RECT_F contentRect = GetBorderRectangle();
		
		// 设置滚动条位置和大小
		D2D1_RECT_F scrollBarRect = {
			contentRect.right - fScrollBarWidth,
			contentRect.top,
			contentRect.right,
			contentRect.bottom
		};
		 
		m_verticalScroll.SetRectangle(scrollBarRect);
		
		// 更新滚动条的范围和页面大小
		float fmaxRange = m_fTotalHeight - fVisibleHeight;

		m_verticalScroll.SetRange(0.0f, fmaxRange);
		m_verticalScroll.SetPageSize(fVisibleHeight);
		m_verticalScroll.SetThumbSize(max(10.0f, fVisibleHeight * fVisibleHeight / m_fTotalHeight));
		m_verticalScroll.SetVisible(true);
		m_fScrollPosition = m_verticalScroll.GetPosition();
		
	}
	else
	{
		// 整个控件区域都是内容区域
		m_fScrollPosition = 0.0f; // 当没有滚动条时，滚动位置重置为0
		m_verticalScroll.SetVisible(false);
	}

}

void CD2DContainerUI::EnableVerticalScroll(bool enable)
{
	m_enableVerticalScroll = enable;
}

bool CD2DContainerUI::IsVerticalScrollEnabled() const
{
	return m_enableVerticalScroll;
}

void CD2DContainerUI::Relayout()
{

}





