#include "stdafx.h"
#include "D2D.h"

CD2DTextAreaUI::CD2DTextAreaUI()
	: m_verticalScrollBar(ScrollBarDirection::Vertical)
	, m_enableVerticalScroll(true)
	, m_scrollBarWidth(16.0f)
	, m_textContentSize(D2D1::SizeF(0.0f, 0.0f))
	, m_scrollPositionY(0.0f)
	, m_isUpdatingScroll(false)
{
	// 设置背景色为白色
	SetBackgroundColor(D2D1::ColorF(D2D1::ColorF::White));
	SetBorderColor(D2D1::ColorF(D2D1::ColorF::Black));
	SetBorderWidth(1.0f);
	SetBorderVisible(true);
	SetPadding(5.0f, 5.0f, 5.0f, 5.0f);

	// 设置滚动条回调
	m_verticalScrollBar.SetScrollCallback(OnVerticalScrollCallback, this);
}

CD2DTextAreaUI::~CD2DTextAreaUI()
{
	// 析构函数
}

void CD2DTextAreaUI::Render(CD2DRender* pRender)
{
	if (!pRender || !IsVisible())
		return;

	// 绘制背景和边框
	D2D1_RECT_F rect = GetRectangle();

	if (IsBackgroundVisible())
	{
		pRender->FillRoundedRectangle(rect, m_radiusX, m_radiusY, GetBackgroundColor());
	}

	if (IsBorderVisible())
	{
		pRender->DrawRoundedRectangle(rect, m_radiusX, m_radiusY, GetBorderColor(), GetBorderWidth());
	}

	// 计算文本边界大小
	CalculateTextBounds(pRender);

	// 更新滚动条状态
	UpdateScrollBars();

	// 绘制垂直滚动条
	if (m_verticalScrollBar.IsVisible())
	{
		m_verticalScrollBar.Render(pRender);
	}

	// 绘制文本
	if (!m_text.IsEmpty())
	{
		D2D1_RECT_F textRect = GetTextDisplayRect();

		// 创建文本格式
		CComPtr<IDWriteTextFormat> textFormat = pRender->CreateTextFormat(m_fontName, m_fontSize, m_fontWeight, m_horizontalAlign, m_verticalAlign);

		// 绘制时应用滚动偏移，并裁剪到可视区域
		D2D1_RECT_F clipRect = GetTextDisplayRect();
		pRender->PushAxisAlignedClip(clipRect); // 限制只在此区域内绘制

		float offsetY = 0.0f;
		if (m_enableVerticalScroll)
		{
			offsetY = m_verticalScrollBar.GetPosition();
		}

		pRender->DrawTextLayout(
			textRect.left,
			textRect.top - offsetY,
			m_text,
			m_textColor,
			textRect.right - textRect.left,
			textRect.bottom - textRect.top,
			textFormat
			);

		pRender->PopAxisAlignedClip(); // 恢复裁剪
	}
}

void CD2DTextAreaUI::EnableVerticalScroll(bool enable)
{
	m_enableVerticalScroll = enable;
	m_verticalScrollBar.SetVisible(enable);
	UpdateScrollBars();
}

bool CD2DTextAreaUI::IsVerticalScrollEnabled() const
{
	return m_enableVerticalScroll;
}

void CD2DTextAreaUI::SetScrollBarWidth(float width)
{
	m_scrollBarWidth = width > 0 ? width : 16.0f;
	UpdateScrollBars();
}

float CD2DTextAreaUI::GetScrollBarWidth() const
{
	return m_scrollBarWidth;
}

CD2DScrollBarUI& CD2DTextAreaUI::GetVerticalScroll()
{
	return m_verticalScrollBar;
}

void CD2DTextAreaUI::ScrollToTop()
{
	m_scrollPositionY = 0.0f;
	m_verticalScrollBar.SetPosition(0.0f);
}

void CD2DTextAreaUI::ScrollToBottom()
{
	D2D1_SIZE_F size = GetSize();
	float maxScrollY = max(0.0f, m_textContentSize.height - (size.height - m_paddingTop - m_paddingBottom));
	m_scrollPositionY = maxScrollY;
	m_verticalScrollBar.SetPosition(maxScrollY);
}

void CD2DTextAreaUI::ScrollBy(float deltaY)
{
	m_scrollPositionY += deltaY;

	// 限制滚动范围
	D2D1_SIZE_F size = GetSize();
	float maxScrollY = max(0.0f, m_textContentSize.height - (size.height - m_paddingTop - m_paddingBottom));
	m_scrollPositionY = max(0.0f, min(m_scrollPositionY, maxScrollY));

	// 更新滚动条位置
	m_verticalScrollBar.SetPosition(m_scrollPositionY);
}

float CD2DTextAreaUI::GetScrollPositionY() const
{
	return m_scrollPositionY;
}

void CD2DTextAreaUI::RecalculateLayout()
{
	m_textContentSize = D2D1::SizeF(0.0f, 0.0f); // 重置大小，稍后重新计算
	UpdateScrollBars();
}

bool CD2DTextAreaUI::OnMouseDownL(const D2D1_POINT_2F& point)
{
	if (!IsEnabled() || !IsVisible())
		return false;

	// 检查是否点击在垂直滚动条上
	if (m_verticalScrollBar.IsVisible() && m_verticalScrollBar.OnMouseDownL(point))
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

bool CD2DTextAreaUI::OnMouseMove(const D2D1_POINT_2F& point)
{
	if (!IsEnabled() || !IsVisible())
		return false;

	// 转发鼠标事件到滚动条
	bool handled = false;

	if (m_verticalScrollBar.IsVisible())
	{
		handled = m_verticalScrollBar.OnMouseMove(point) || handled;
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

bool CD2DTextAreaUI::OnMouseUpL(const D2D1_POINT_2F& point)
{
	if (!IsEnabled() || !IsVisible())
		return false;

	bool handled = false;

	if (m_verticalScrollBar.IsVisible())
	{
		handled = m_verticalScrollBar.OnMouseUpL(point) || handled;
	}

	m_isPressed = false;

	return handled;
}

bool CD2DTextAreaUI::OnMouseLeave(const D2D1_POINT_2F& point)
{
	if (!IsEnabled() || !IsVisible())
		return false;

	m_isHover = false;

	// 通知滚动条鼠标离开
	if (m_verticalScrollBar.IsVisible())
	{
		m_verticalScrollBar.OnMouseLeave(point);
	}

	return true;
}

bool CD2DTextAreaUI::OnMouseWheel(float delta)
{
	if (!IsEnabled() || !IsVisible())
		return false;

	// 垂直滚动
	if (m_verticalScrollBar.IsVisible())
	{
		return m_verticalScrollBar.OnMouseWheel(delta);
	}

	return false;
}

void CD2DTextAreaUI::SetSize(float width, float height)
{
	CD2DControlUI::SetSize(width, height);
	UpdateScrollBars();
}

void CD2DTextAreaUI::OnVerticalScrollCallback(float position, void* userData)
{
	if (userData)
	{
		CD2DTextAreaUI* pThis = static_cast<CD2DTextAreaUI*>(userData);
		pThis->OnVerticalScroll(position);
	}
}

void CD2DTextAreaUI::OnVerticalScroll(float position)
{
	if (m_isUpdatingScroll)
		return;

	m_isUpdatingScroll = true;
	m_scrollPositionY = position;
	m_isUpdatingScroll = false;
}

void CD2DTextAreaUI::CalculateTextBounds(CD2DRender* pRender)
{
	if (!pRender || m_text.IsEmpty())
		return;

	// 获取可用于显示文本的区域大小（包含内边距，但尚未扣除滚动条）
	D2D1_RECT_F textRect = GetTextDisplayRect();
	float availableWidth = textRect.right - textRect.left;
	float availableHeight = textRect.bottom - textRect.top;

	// 创建文本格式
	CComPtr<IDWriteTextFormat> textFormat = pRender->CreateTextFormat(m_fontName, m_fontSize, m_fontWeight, m_horizontalAlign, m_verticalAlign);

	// 测量文本边界（始终使用可用宽度，允许换行，不限制高度）
	m_textContentSize = pRender->MeasureText(m_text, textFormat, availableWidth);
}

void CD2DTextAreaUI::UpdateScrollBars()
{
	D2D1_RECT_F rect = GetRectangle();
	rect.top += m_marginTop;
	rect.bottom -= m_marginBottom;
	D2D1_SIZE_F controlSize = D2D1::SizeF(rect.right - rect.left, rect.bottom - rect.top);

	float availableHeight = controlSize.height - GetMarginBottom()-GetMarginTop() - GetPaddingBottom() -GetPaddingTop();

	// 更新垂直滚动条
	if (m_enableVerticalScroll)
	{
		// 计算是否需要显示垂直滚动条
		bool needVerticalScroll = m_textContentSize.height > availableHeight;

		if (needVerticalScroll)
		{
			// 设置垂直滚动条范围和位置
			float maxScrollY = m_textContentSize.height - availableHeight;
			m_verticalScrollBar.SetRange(0.0f, maxScrollY);
			m_verticalScrollBar.SetPageSize(availableHeight);

			// 设置垂直滚动条大小和位置
			float scrollBarX = controlSize.width - m_scrollBarWidth;
			m_verticalScrollBar.SetPosition(rect.left + scrollBarX, rect.top);
			m_verticalScrollBar.SetSize(m_scrollBarWidth, controlSize.height);
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

D2D1_RECT_F CD2DTextAreaUI::GetTextDisplayRect() const
{
	D2D1_RECT_F rect = GetContentRectangle();

	// 如果显示了垂直滚动条，调整右侧区域
	if (m_verticalScrollBar.IsVisible())
	{
		rect.right -= m_scrollBarWidth;
	}

	return rect;
}