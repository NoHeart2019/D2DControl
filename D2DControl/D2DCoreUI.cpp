#include "stdafx.h"
#include "D2D.h"


CD2DLabel::CD2DLabel()
	: m_textFormat(nullptr)
	, m_boldLength(0)
{
	// 默认透明背景
	SetSizePolicy(CD2DSizePolicy::Preferred);
	SetBackgroundColor(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
	SetForegroundColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f));
	SetBackgroundVisible(false);
	SetBorderVisible(false);
	m_onClickCallback = nullptr;

}

CD2DLabel::~CD2DLabel()
{
}

void CD2DLabel::Render(CD2DRender* render)
{
	if (!m_isVisible || !render || m_text.IsEmpty())
		return;
	//SetBackgroundVisible(true);
	//SetBorder(1.0f, 1.0f, 1.0f, 1.0f);
	CD2DControlUI::Render(render);
	DrawText(render);
}

void CD2DLabel::DrawBorder(CD2DRender* render)
{
	 // 获取边框绘制的矩形区域
	 D2D1_RECT_F borderRect = GetBorderRectangle();

	 float borderTop    = GetBorderTop();    
	 float borderBottom = GetBorderBottom(); 
	 float borderLeft   = GetBorderLeft();   
	 float borderRight  = GetBorderRight();  

	 // 获取画笔颜色 (通常边框颜色和文本颜色可能不同，这里沿用你原有的 GetTextColor()，
	 // 如果有 GetBorderColor() 建议替换)
	 D2D1_COLOR_F color = GetBorderColor(); 

	 // 1. 绘制上边框 (Top)
	 if (borderTop > 0.0f)
	 {
		 render->DrawLine(
			 D2D1::Point2F(borderRect.left, borderRect.top), 
			 D2D1::Point2F(borderRect.right, borderRect.top), 
			 color, 
			 borderTop,
			 IsBorderSolid()
			 );
	 }

	 // 2. 绘制下边框 (Bottom) - 原有逻辑保留并加入判断
	 if (borderBottom > 0.0f)
	 {
		 render->DrawLine(
			 D2D1::Point2F(borderRect.left, borderRect.bottom), 
			 D2D1::Point2F(borderRect.right, borderRect.bottom), 
			 color, 
			 borderBottom,
			 IsBorderSolid()
			 );
	 }

	 // 3. 绘制左边框 (Left)
	 if (borderLeft > 0.0f)
	 {
		 render->DrawLine(
			 D2D1::Point2F(borderRect.left, borderRect.top), 
			 D2D1::Point2F(borderRect.left, borderRect.bottom), 
			 color, 
			 borderLeft,
			 IsBorderSolid()
			 );
	 }

	 // 4. 绘制右边框 (Right)
	 if (borderRight > 0.0f)
	 {
		 render->DrawLine(
			 D2D1::Point2F(borderRect.right, borderRect.top), 
			 D2D1::Point2F(borderRect.right, borderRect.bottom), 
			 color, 
			 borderRight,
			 IsBorderSolid()
			 );
	 }
 }

void CD2DLabel::DrawBackground(CD2DRender* render)
{
	CD2DControlUI::Render(render);
}

void CD2DLabel::DrawText(CD2DRender* render)
{
	if (!m_textFormat)
		m_textFormat = render->CreateTextFormat(
		GetFontName(), 
		GetFontSize(), 
		GetFontWeight(), 
		GetHorizontalAlignment(), 
		GetVerticalAlignment());

	if (!m_textFormat) return;

	CString text = GetText();
	int textLength = text.GetLength();
	D2D1_RECT_F contentRect = GetContentRectangle();
	float layoutWidth = contentRect.right - contentRect.left;
	float layoutHeight = contentRect.bottom - contentRect.top;
	
	CComPtr<IDWriteTextLayout> pTextLayout = render->CreateTextLayout(text,textLength,m_textFormat,layoutWidth,layoutHeight);

	if (!pTextLayout)
		return;
	// 4. 应用行间距 
	if (GetLineSpacing() != 0.0f)
	{
		FLOAT fontSize = GetFontSize();
		FLOAT approximateAscent = fontSize * 0.8f; 

		float line = (GetLineSpacing() > fontSize ?
			GetLineSpacing() :
		fontSize * GetLineSpacing() * render->GetDpiScaleY());

		FLOAT baselineOffset = (line - fontSize) / 2.0f + approximateAscent;

		pTextLayout->SetLineSpacing(
			DWRITE_LINE_SPACING_METHOD_UNIFORM,
			line,
			baselineOffset
			);
	}

	// 5. 【核心】设置局部粗体
	if (m_boldLength > 0)
	{
		DWRITE_TEXT_RANGE boldRange;
		boldRange.startPosition = 0;
		// 确保长度不越界
		boldRange.length = (m_boldLength > (int)textLength) ? textLength : static_cast<UINT32>(m_boldLength);
		// 设置为粗体
		pTextLayout->SetFontWeight(DWRITE_FONT_WEIGHT_BOLD, boldRange);
	}

	CComPtr<ID2D1SolidColorBrush> pBrush = render->CreateBrush(GetTextColor());
	render->GetRenderTarget()->DrawTextLayout(
		D2D1::Point2F(contentRect.left, contentRect.top),
		pTextLayout,
		pBrush
		);

}


void CD2DLabel::SetBoldLength(int boldCharCount)
{
	CString text = GetText();
	int textLen = text.GetLength();
	if (boldCharCount > textLen)
		m_boldLength = textLen;
	else if (boldCharCount < 0)
		m_boldLength = 0;
	else
		m_boldLength = boldCharCount;
}

void CD2DLabel::SetOnClickCallback(OnClickCallback callback, void* userData)
{
	m_onClickCallback = callback;
	SetUserData(userData);
}

bool CD2DLabel::OnMouseDownL(const D2D1_POINT_2F& point) {
	if (!IsVisible() || !IsEnabled() || !IsPointInRect(point)) {
		return false;
	}

	if (m_onClickCallback)
	{
		m_onClickCallback(this, GetUserData());
	}

	return true;
}

D2D1_SIZE_F CD2DLabel::Measure(const D2D1_SIZE_F& availableSize, CD2DRender* pRender)
{
	return MeasureControlSize(pRender, *this, *this, availableSize.width, availableSize.height);
}



CD2DButton::CD2DButton()
{
	SetSizePolicy(CD2DSizePolicy::Preferred);
	SetTranslate(0.0f, -2.0f);
	// 设置默认背景色
	SetBackgroundColor(D2D1::ColorF(0.25f, 0.35f, 0.55f, 1.0f));
	SetForegroundColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f));
	SetBorderVisible(false);
	m_clickCallback = nullptr;
}

CD2DButton::~CD2DButton()
{
}

void CD2DButton::SetText(const CString& text)
{
	m_text = text;
}

CString CD2DButton::GetText() const
{
	return m_text;
}

void CD2DButton::SetTextFormat(IDWriteTextFormat* textFormat)
{
	m_textFormat = textFormat;
}

IDWriteTextFormat* CD2DButton::GetTextFormat()
{
	return m_textFormat;
}

D2D1_SIZE_F CD2DButton::Measure(const D2D1_SIZE_F& availableSize, CD2DRender* pRender)
{
	return MeasureControlSize(pRender, *this, *this, availableSize.width, availableSize.height);
}

void CD2DButton::Render(CD2DRender* render)
{
	if (!m_isVisible || !render)
		return;

	if (!m_textFormat)
		m_textFormat = render->CreateTextFormat(
		GetFontName(), 
		GetFontSize(), 
		GetFontWeight(), 
		GetHorizontalAlignment(), 
		GetVerticalAlignment());

	// 确定当前使用的背景色
	D2D1_COLOR_F bgColor = GetBackgroundColor();
	D2D1_RECT_F rect = GetBorderRectangle();
	m_translateY = 0.0;	
	if (IsPressed())
	{
		bgColor = m_pressedColor;
		m_translateY = 1.0f;
	}
	else if (IsHovered())
	{
		m_translateY = -3.0f;		
		bgColor = m_hoveredColor;
	}
	OffsetRectF(rect, 0, m_translateY);
	
	float radiusY = (GetBorderRadiusY() < 1.0f? (rect.bottom - rect.top) * GetBorderRadiusY() : GetBorderRadiusY());
	float radiusX = radiusY;

	// 绘制圆角矩形背景
	if (IsBackgroundVisible())
		render->FillRoundedRectangle(rect, radiusX, radiusY, bgColor);

	// 绘制边框（如果可见）
	if (IsBorderVisible())
	{
		render->DrawRoundedRectangle(rect, radiusX, radiusY, GetBorderColor(), GetBorderWidth());
	}

	// 绘制文本
	if (!m_text.IsEmpty() && m_textFormat)
	{
		render->DrawText(m_text, rect, GetTextColor(), m_textFormat);
	}
}

bool CD2DButton::OnMouseDownL(const D2D1_POINT_2F& point)
{
	if (!m_isEnabled || !IsPointInRect(point))
		return false;

	SetPressed(true);
	SetDragging(false);
	return true;
}

bool CD2DButton::OnMouseMove(const D2D1_POINT_2F& point)
{
	bool wasOver = IsHovered();
	bool mouseOver = IsPointInRect(point);
	SetHovered(mouseOver);
	if (IsPressed())
	{
		SetDragging(true);
	}

	return wasOver != mouseOver; // 返回是否状态改变
}

bool CD2DButton::OnMouseUpL(const D2D1_POINT_2F& point)
{
	if (!m_isEnabled)
		return false;

	bool isDown =  IsPressed();
	SetPressed(false);
	// 如果按钮被按下并且鼠标在按钮上，触发点击事件
	if (isDown && !IsDragging() && IsPointInRect(point) && m_clickCallback)
	{
		if(m_clickCallback)
			m_clickCallback(this, GetUserData());
		return true;
	}

	SetDragging(false);
	return isDown;
}

void CD2DButton::SetOnClickCallback(OnClickCallback callback, void* userData)
{
	m_clickCallback = callback;
	SetUserData(userData);
}

void CD2DButton::SetCornerRadius(float radius)
{
	m_radiusX = m_radiusY = radius;
}

//checkButton
CD2DCheckButton::CD2DCheckButton()
{
	// 设置默认背景色
	SetBackgroundColor(D2D1::ColorF(0.25f, 0.35f, 0.55f, 1.0f));
	SetForegroundColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f));
	SetBorderVisible(false);
	m_clickCallback = nullptr;
}

CD2DCheckButton::~CD2DCheckButton()
{
}

void CD2DCheckButton::Render(CD2DRender* render)
{
	if (!m_isVisible || !render)
		return;

	if (!m_textFormat)
		m_textFormat = render->CreateTextFormat(
		GetFontName(), 
		GetFontSize(), 
		GetFontWeight(), 
		GetHorizontalAlignment(), 
		GetVerticalAlignment());

	// 确定当前使用的背景色
	D2D1_COLOR_F bgColor = GetNormalColor();
	D2D1_RECT_F rect = GetBorderRectangle();
	
	if (IsSelected())
	{
		bgColor = GetPressedColor();
	}
	else if (IsHovered())
	{		
		bgColor = GetHoveredColor();
	}
	OffsetRectF(rect, 0, m_translateY);
	float radiusY = (GetBorderRadiusY() < 1.0f? (rect.bottom - rect.top) * GetBorderRadiusY() : GetBorderRadiusY());
	float radiusX = radiusY;
	// 绘制圆角矩形背景
	if (IsBackgroundVisible())
		render->FillRoundedRectangle(rect, radiusX, radiusY, bgColor);

	// 绘制边框（如果可见）
	if (IsBorderVisible())
	{
		render->DrawRoundedRectangle(rect, radiusX, radiusY, GetBorderColor(), GetBorderWidth());
	}

	// 绘制文本
	if (!m_text.IsEmpty() && m_textFormat)
	{
		render->DrawText(m_text, rect, GetTextColor(), m_textFormat);
	}
}

bool CD2DCheckButton::OnMouseDownL(const D2D1_POINT_2F& point)
{
	if (!m_isEnabled || !IsPointInRect(point))
		return false;

	SetSelected(true);
	SetPressed(true);
	SetDragging(false);
	return true;
}

bool CD2DCheckButton::OnMouseMove(const D2D1_POINT_2F& point)
{
	bool wasOver = IsHovered();
	bool mouseOver = IsPointInRect(point);
	SetHovered(mouseOver);
	if (IsPressed())
	{
		SetDragging(true);
	}

	return wasOver != mouseOver; // 返回是否状态改变
}

bool CD2DCheckButton::OnMouseUpL(const D2D1_POINT_2F& point)
{
	if (!m_isEnabled)
		return false;

	bool isDown =  IsPressed();
	SetPressed(false);
	// 如果按钮被按下并且鼠标在按钮上，触发点击事件
	if (isDown && !IsDragging() && IsPointInRect(point) && m_clickCallback)
	{
		if(m_clickCallback)
			m_clickCallback(this, GetUserData());
		return true;
	}

	SetDragging(false);
	return isDown;
}

void CD2DCheckButton::SetOnClickCallback(OnClickCallback callback, void* userData)
{
	m_clickCallback = callback;
	SetUserData(userData);
}

void CD2DCheckButton::SetCornerRadius(float radius)
{
	m_radiusX = m_radiusY = radius;
}


void SetLabelRectangle(__inout D2D1_RECT_F& rectangle, CD2DLabel& label, CD2DRender* pRender, bool isReal )
{
	float left = rectangle.left;
	float top = rectangle.top;
	float width = rectangle.right - rectangle.left;
	float height = rectangle.bottom - rectangle.top;
	D2D1_SIZE_F size = pRender->MeasureTextSize(label);
	label.SetPosition(left, top);
	label.SetContentSize(size.width, size.height);
	if (!isReal)
		label.SetSize(width, label.GetSize().height);

	if (size.width && size.height)
	{
		rectangle.left = label.GetRectangle().left;
		rectangle.top = label.GetRectangle().bottom;
	}

}


D2D1_SIZE_F MeasureControlSize(CD2DRender* pRender, CD2DControlUI& control, const CD2DTextFormat& textFormat, float maxWidth , float maxHeight)
{
	// 1. 获取内容本身的原始大小 
	D2D1_SIZE_F boxSize = control.GetBoxModelSize();
	maxWidth  = maxWidth ==  FLT_MAX ? FLT_MAX : maxWidth - boxSize.width ;
	maxHeight = maxHeight == FLT_MAX ? FLT_MAX : maxHeight - boxSize.height ;
	D2D1_SIZE_F contentSize = pRender->MeasureTextSize(textFormat, maxWidth, maxHeight);

	// 2. 获取布局策略
	CD2DSizePolicy policy = control.GetSizePolicy();

	float finalWidth = 0.0f;
	float finalHeight = 0.0f;

	// 3. 根据策略计算内容区域的大小
	switch (policy.m_policy) // 假设 m_Policy 是公开的，或者用 GetPolicy()
	{
	case CD2DSizePolicy::Fixed:
		// 固定大小：直接使用 GetSize()，忽略 availableSize 和内容大小
		return control.GetSize(); 

	case CD2DSizePolicy::Expanding:
		// 可扩展：尽可能占满可用空间，但不能小于内容本身
		finalWidth  = (maxWidth == FLT_MAX) ? contentSize.width : maxWidth;
		finalHeight = (maxHeight == FLT_MAX) ? contentSize.height :maxHeight;
		break;

	case CD2DSizePolicy::Preferred:
	default:
		// 首选大小：使用内容本身的大小
		finalWidth = contentSize.width;
		finalHeight = contentSize.height;
		break;
	}

	control.SetContentSize(finalWidth, finalHeight);

	return control.GetSize();
	
}
