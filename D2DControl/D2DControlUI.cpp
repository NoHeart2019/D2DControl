#include "stdafx.h"
#include "D2DControlUI.h"
#include "D2DRender.h"
CD2DControlUI::CD2DControlUI()
    : m_rectangle(D2D1::RectF()),
	  m_borderRadius(D2D1::RectF()),
	  m_isVisible(true),
      m_isEnabled(true),
      m_isHover(false),
      m_isPressed(false),
	  m_isDragging(false),
	  m_isSeleced(false),
	  m_translateX(0.0f),
	  m_translateY(0.0f),
      m_dpiScaleX(1.0f),
      m_dpiScaleY(1.0f),
	  m_radiusX(0.0f),
	  m_radiusY(0.0f),
	  m_borderWidth(1.0f),
	  m_foregroundColor(D2D1::ColorF(D2D1::ColorF::White)),
	  m_backgroundColor(D2D1::ColorF(D2D1::ColorF::White)),
	  m_borderColor(D2D1::ColorF(D2D1::ColorF::Black)),
	  m_normalColor(D2D1::ColorF(D2D1::ColorF::White)),
	  m_hoveredColor(D2D1::ColorF(D2D1::ColorF::White)),
	  m_pressedColor(D2D1::ColorF(D2D1::ColorF::White)),
	  m_isBackgroundVisible(true),
	  m_isBorderVisible(true),
	  m_isBorderSolid(true)
{
    m_rectangle = D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f);
	m_LinearDegree = -1.0f;
}

CD2DControlUI::~CD2DControlUI()
{
  
}

void CD2DControlUI::SetPosition(float x, float y)
{
    float width = m_rectangle.right - m_rectangle.left;
    float height = m_rectangle.bottom - m_rectangle.top;
    m_rectangle = D2D1::RectF(x, y, x + width, y + height);
}

void CD2DControlUI::SetSize(float width, float height)
{
    m_rectangle.right = m_rectangle.left + width;
    m_rectangle.bottom = m_rectangle.top + height;
}

void CD2DControlUI::SetContentSize(float width, float height)
{
	m_rectangle.right  = m_rectangle.left + width   
						 + GetMarginLeft()  + GetPaddingLeft()  +  GetBorderLeft()
						 + GetMarginRight() + GetPaddingRight() +  GetBorderRight();
	m_rectangle.bottom = m_rectangle.top  +	height
						 + GetMarginBottom() + GetPaddingBottom() +GetBorderBottom()
						 + GetMarginTop()    + GetPaddingTop()    + GetBorderTop();
}

void CD2DControlUI::SetRectangle(const D2D1_RECT_F& rect)
{
    m_rectangle = rect;
}

D2D1_RECT_F CD2DControlUI::GetRectangle() const
{
    return m_rectangle;
}

D2D1_ROUNDED_RECT CD2DControlUI::GetRoundedRectangle() const
{
	return D2D1::RoundedRect(m_rectangle, m_radiusX, m_radiusY);
}


void CD2DControlUI::SetBorderRadius(float radius)
{
	m_radiusX = radius;
	m_radiusY = radius;

	m_borderRadius.left = m_borderRadius.right = radius;
	m_borderRadius.top = m_borderRadius.bottom = radius;
}

void CD2DControlUI::SetBorderRadius(float radiusX, float radiusY)
{
	m_radiusX = radiusX;
	m_radiusY = radiusY;
	
	m_borderRadius.left = m_borderRadius.right = radiusX;
	m_borderRadius.top = m_borderRadius.bottom = radiusY;
}

void CD2DControlUI::SetBorderRadius(float left, float top, float right, float bottom)
{
	m_borderRadius.left = left;
	m_borderRadius.right = right;
	m_borderRadius.top = top;
	m_borderRadius.bottom = bottom;
}


D2D1_POINT_2F CD2DControlUI::GetPosition() const
{
    return D2D1::Point2F(m_rectangle.left, m_rectangle.top);
}

D2D1_SIZE_F CD2DControlUI::GetSize() const
{
    return D2D1::SizeF(
        m_rectangle.right - m_rectangle.left,
        m_rectangle.bottom - m_rectangle.top
    );
}

D2D1_POINT_2F CD2DControlUI::GetCenter() const
{
	return D2D1::Point2F(
		(m_rectangle.left + m_rectangle.right) / 2.0f,
		(m_rectangle.top + m_rectangle.bottom) / 2.0f
		);
}

void CD2DControlUI::SetCenter(float x, float y)
{
	float width = m_rectangle.right - m_rectangle.left;
	float height = m_rectangle.bottom - m_rectangle.top;
	m_rectangle = D2D1::RectF(
		x - width / 2.0f,
		y - height / 2.0f,
		x + width / 2.0f,
		y + height / 2.0f
		);
}

float CD2DControlUI::GetWidth() const
{
	return m_rectangle.right - m_rectangle.left;
}

float CD2DControlUI::GetHeight() const
{
	return m_rectangle.bottom - m_rectangle.top;
}

void CD2DControlUI::SetTranslate(float x, float y)
{
	m_translateX = x;
	m_translateY = y;

}

float CD2DControlUI::GetTranslateX() const { return m_translateX; }
float CD2DControlUI::GetTranslateY() const { return m_translateY; }

float CD2DControlUI::GetBorderRadiusX() const { return m_radiusX; }
float CD2DControlUI::GetBorderRadiusY() const { return m_radiusY; }

//大小策略
const CD2DSizePolicy& CD2DControlUI::GetSizePolicy() const
{
	return m_sizePolicy;
}

void CD2DControlUI::SetSizePolicy(CD2DSizePolicy::Policy policy)
{
	m_sizePolicy = policy;
}



void CD2DControlUI::Move(float offsetX, float offsetY)
{
    m_rectangle.left += offsetX;
    m_rectangle.right += offsetX;
    m_rectangle.top += offsetY;
    m_rectangle.bottom += offsetY;
}

void CD2DControlUI::Inflate(float dx, float dy)
{
	m_rectangle.left -= dx;
	m_rectangle.right += dx;
	m_rectangle.top -= dy;
	m_rectangle.bottom += dy;
}

void CD2DControlUI::MoveTo(float x, float y)
{
    SetPosition(x, y);
}

void CD2DControlUI::Resize(float newWidth, float newHeight, bool keepCenter)
{
    if (keepCenter) {
        float centerX = (m_rectangle.left + m_rectangle.right) / 2.0f;
        float centerY = (m_rectangle.top + m_rectangle.bottom) / 2.0f;
        m_rectangle.left = centerX - newWidth / 2.0f;
        m_rectangle.right = centerX + newWidth / 2.0f;
        m_rectangle.top = centerY - newHeight / 2.0f;
        m_rectangle.bottom = centerY + newHeight / 2.0f;
    } else {
        SetSize(newWidth, newHeight);
    }
}

// 检查点是否在矩形内
bool CD2DControlUI::IsPointInRect(const D2D1_POINT_2F& point, const D2D1_RECT_F& rect) const
{
	return point.x >= rect.left && point.x <= rect.right &&
		point.y >= rect.top && point.y <= rect.bottom;
}

bool CD2DControlUI::IsPointInRect(const D2D1_POINT_2F& point) const
{
    return IsPointInRect(point, m_rectangle);
}

bool CD2DControlUI::IsPointInRect(float x, float y) const
{
    return IsPointInRect(D2D1::Point2F(x, y));
}

bool CD2DControlUI::IsPointOnRect(const D2D1_POINT_2F& point, const D2D1_RECT_F& rect, float tolerance) const
{
	// 检查点是否在矩形的任何一条边上
	bool onLeftEdge = (fabs(point.x - rect.left) <= tolerance) && (point.y >= rect.top - tolerance) && (point.y <= rect.bottom + tolerance);
	bool onRightEdge = (fabs(point.x - rect.right) <= tolerance) && (point.y >= rect.top - tolerance) && (point.y <= rect.bottom + tolerance);
	bool onTopEdge = (fabs(point.y - rect.top) <= tolerance) && (point.x >= rect.left - tolerance) && (point.x <= rect.right + tolerance);
	bool onBottomEdge = (fabs(point.y - rect.bottom) <= tolerance) && (point.x >= rect.left - tolerance) && (point.x <= rect.right + tolerance);

	return onLeftEdge || onRightEdge || onTopEdge || onBottomEdge;
}

bool CD2DControlUI::IsPointOnRectEdge(const D2D1_POINT_2F& point, float tolerance) const
{
    return IsPointOnRect(point, m_rectangle, tolerance);
}

bool CD2DControlUI::IsPointOnRectEdge(float x, float y, float tolerance) const
{
    return IsPointOnRectEdge(D2D1::Point2F(x, y), tolerance);
}

bool CD2DControlUI::IsHitTest(const D2D1_POINT_2F& point) const
{
	return IsPointInRect(point);
}

void CD2DControlUI::SetVisible(bool visible)
{
    m_isVisible = visible;
}

bool CD2DControlUI::IsVisible() const
{
    return m_isVisible;
}

void CD2DControlUI::SetEnabled(bool enabled)
{
    m_isEnabled = enabled;
    if (!enabled) {
        //m_currentState = C32ControlState::Disabled;
    } else {
       // UpdateC32ControlState();
    }
}

bool CD2DControlUI::IsEnabled() const
{
    return m_isEnabled;
}

bool CD2DControlUI::IsHovered() const
{
	return m_isHover;
}

bool CD2DControlUI::IsDragging() const
{
	return m_isDragging;
}

void CD2DControlUI::SetBackgroundColor(const D2D1_COLOR_F& color)
{
	m_backgroundColor = color;
	m_LinearDegree = -1.0f;

}

void CD2DControlUI::SetLinearColor(const D2D1_COLOR_F& color1, const D2D1_COLOR_F& color2, float degree)
{
	m_LinearColor[0] = color1;
	m_LinearColor[1] = color2;
	m_LinearDegree = degree;
}



D2D1_COLOR_F CD2DControlUI::GetBackgroundColor() const
{
	return m_backgroundColor;
}

void CD2DControlUI::SetBorderColor(const D2D1_COLOR_F& color)
{
	m_borderColor = color;
}

D2D1_COLOR_F CD2DControlUI::GetBorderColor() const
{
	return m_borderColor;
}

void CD2DControlUI::SetBorderWidth(float width)
{
	m_borderWidth = width;
}

float CD2DControlUI::GetBorderWidth() const
{
	return m_borderWidth;
}

void CD2DControlUI::SetBackgroundVisible(bool visible)
{
	m_isBackgroundVisible = visible;
}

bool CD2DControlUI::IsBackgroundVisible() const
{
	return m_isBackgroundVisible;
}

void CD2DControlUI::SetForegroundColor(const D2D1_COLOR_F& color)
{
	// 设置所有状态的前景色为相同值
	m_foregroundColor = color;
}

D2D1_COLOR_F CD2DControlUI::GetForegroundColor() const
{
	return m_foregroundColor;
}

void CD2DControlUI::SetNormalColor(D2D1_COLOR_F normalColor)
{
	m_normalColor = normalColor;
}

D2D1_COLOR_F CD2DControlUI::GetNormalColor()
{
	return m_normalColor;
}


void CD2DControlUI::SetPressedColor(D2D1_COLOR_F pressedColor)
{
	m_pressedColor = pressedColor;
}

D2D1_COLOR_F CD2DControlUI::GetPressedColor()
{
	return m_pressedColor;
}

void CD2DControlUI::SetHoveredColor(D2D1_COLOR_F hoverColor)
{
	m_hoveredColor = hoverColor;
}

D2D1_COLOR_F CD2DControlUI::GetHoveredColor()
{
	return m_hoveredColor;
}

void CD2DControlUI::SetSelectedColor(D2D1_COLOR_F selectColor)
{
	m_selectedColor = selectColor;
}

D2D1_COLOR_F CD2DControlUI::GetSelectedColor()
{
	return m_selectedColor;
}

// 设置用户数据
void CD2DControlUI::SetUserData(void* userData)
{
	m_userData = userData;
}

void* CD2DControlUI::GetUserData() const
{
	return m_userData;
}

void CD2DControlUI::SetBorderVisible(bool visible)
{
	m_isBorderVisible = visible;
}

bool CD2DControlUI::IsBorderVisible() const
{
	return m_isBorderVisible;
}

void CD2DControlUI::SetBorderSolid(bool solid)
{
	m_isBorderSolid = solid;
}

bool CD2DControlUI::IsBorderSolid() const
{
	return m_isBorderSolid;
}


void CD2DControlUI::SetFocused(bool focused)
{
	m_isFocused = focused;
}

bool CD2DControlUI::IsFocused() const
{
	return m_isFocused;
}

bool CD2DControlUI::IsPressed() const
{
	return m_isPressed;
}

bool CD2DControlUI::IsSelected() const
{
	return m_isSeleced;
}

void CD2DControlUI::SetHovered(bool hover)
{
	m_isHover = hover;
}

void CD2DControlUI::SetDragging(bool dragging)
{
	m_isDragging = dragging;
}

void CD2DControlUI::SetPressed(bool pressed)
{
	m_isPressed = pressed;
}

void CD2DControlUI::SetSelected(bool selected)
{
	m_isSeleced = selected;
}

CComPtr<ID2D1Brush>  CD2DControlUI::GetBackgroundBrush(CD2DRender* pRender)
{
	CComPtr<ID2D1Brush> brush;

	if (m_LinearDegree >= 0.0f)
	{
		brush = pRender->CreateLinearBrush(GetBorderRectangle(), m_LinearColor[0], m_LinearColor[1], m_LinearDegree);
	}else{
		brush = pRender->CreateBrush(GetBackgroundColor());
	}
	return brush;
}

void CD2DControlUI::Render(CD2DRender* pRender)
{
	D2D1_RECT_F rectangle = GetPaddingRectangle();
	
	FLOAT left   = rectangle.left;
	FLOAT top    = rectangle.top;
	FLOAT right  = rectangle.right;
	FLOAT bottom = rectangle.bottom;
	float width = right - left;
	float height = bottom - top;


	FLOAT radiusTopLeft     = m_borderRadius.left;   
	FLOAT radiusTopRight    = m_borderRadius.top;    
	FLOAT radiusBottomRight = m_borderRadius.right;  
	FLOAT radiusBottomLeft  = m_borderRadius.bottom; 
	
	radiusTopLeft     = (radiusTopLeft < 1.0f? min(width, height) * radiusTopLeft : radiusTopLeft);   
	radiusTopRight    = (radiusTopRight < 1.0f?    min(width, height) * radiusTopRight : radiusTopRight);     
	radiusBottomRight = (radiusBottomRight < 1.0f? min(width, height) * radiusBottomRight : radiusBottomRight);  
	radiusBottomLeft  = (radiusBottomLeft < 1.0f?  min(width, height) * radiusBottomLeft : radiusBottomLeft); 
	
	D2D1_RECT_F radius = {radiusTopLeft, radiusTopRight, radiusBottomRight, radiusBottomLeft};
	D2D1_RECT_F border = {GetBorderLeft(), GetBorderTop(), GetBorderRight(), GetBorderBottom()};
	if (IsBackgroundVisible())
	{
		CComPtr<ID2D1PathGeometry> path = pRender->CreateRoundedGeometry(rectangle, radius);
		
		pRender->FillGeometry(path, GetBackgroundBrush(pRender));	
	}
	if (border.left == border.top && border.right == border.bottom && border.left == border.right && border.left == 0)
		return;
	
	if (border.left >0.0f && border.top > 0.0f &&   border.right > 0.0f && border.bottom > 0.0f)
	{
		CComPtr<ID2D1PathGeometry> path = pRender->CreateRoundedGeometry(rectangle, radius);
		pRender->DrawGeometry(path, GetBorderColor(), border.left);		
	}
	else 
	{
			// 1. 绘制上边框 (Top)
			if (border.top > 0.0f)
			{
				pRender->DrawLine(
					D2D1::Point2F(left, top  + radiusTopLeft), 
					D2D1::Point2F(right, top + radiusTopRight), 
					GetBorderColor(), 
					border.top,
					IsBorderSolid()
					);
			}
		
			// 2. 绘制下边框 (Bottom) - 原有逻辑保留并加入判断
			if (border.bottom > 0.0f)
			{
				pRender->DrawLine(
					D2D1::Point2F(left,  bottom - radiusBottomLeft), 
					D2D1::Point2F(right, bottom - radiusBottomRight),
					GetBorderColor(),
					border.bottom ,
					IsBorderSolid()
					);
			}
		
			// 3. 绘制左边框 (Left)
			if (border.left > 0.0f)
			{
				pRender->DrawLine(
					D2D1::Point2F(left, top - radiusTopLeft), 
					D2D1::Point2F(left, bottom - radiusBottomLeft), 
					GetBorderColor(), 
					border.left,
					IsBorderSolid()
					);
			}
		
			// 4. 绘制右边框 (Right)
			if (border.right > 0.0f)
			{
				pRender->DrawLine(
					D2D1::Point2F(right, top - radiusTopRight), 
					D2D1::Point2F(right, bottom - radiusBottomRight), 
					GetBorderColor(), 
					border.right,
					IsBorderSolid()
					);
			}
	}


}

D2D1_SIZE_F CD2DControlUI::Measure(const D2D1_SIZE_F& availableSize, CD2DRender* pRender)
{
	return D2D1::SizeF();
}


bool CD2DControlUI::OnMouseDownL(const D2D1_POINT_2F& point)
{
    if (!m_isEnabled || !m_isVisible) {
        return false;
    }

    if (IsPointInRect(point)) {
		SetPressed(true);
        SetFocused(true);
        return true;
    }
    
    return false;
}

bool CD2DControlUI::OnMouseMove(const D2D1_POINT_2F& point)
{
    if (!m_isEnabled || !m_isVisible) {
        return false;
    }

    bool isPointInRect = IsPointInRect(point);
	if (isPointInRect != IsFocused())
	{
		SetFocused(!IsFocused());
	}

    if (isPointInRect && !IsHovered()) {
       SetHovered(true);
        if (!IsPressed()) {
          
        }
        return true;
    }
    else if (!isPointInRect && IsHovered()) {
        SetHovered(false);
        if (!IsPressed()) {
            
        }
        return true;
    } else if (!isPointInRect && IsPressed())
	{
		SetDragging(true);
		return true;
	}
    
    return false;
}

bool CD2DControlUI::OnMouseUpL(const D2D1_POINT_2F& point)
{
    if (!m_isEnabled || !m_isVisible) {
        return false;
    }
	//ZxDebugEx((__FUNCTION__"():: m_isHover : %d m_isPressed :%d \n", m_isHover, m_isPressed));

    bool wasMouseDown = IsPressed();
	
	SetDragging(false);
    if (wasMouseDown) {
        if (IsPointInRect(point)) {
          SetPressed(false);
        } else {
            SetFocused(false);
        }
        return true;
    }
    
    return false;
}

bool CD2DControlUI::OnMouseDownL2(const D2D1_POINT_2F& point)
{
	if (!m_isEnabled || !m_isVisible) {
		return false;
	}

	if (IsPointInRect(point)) {
		return true;
	}

	return false;
}

bool CD2DControlUI::OnMouseUpL2(const D2D1_POINT_2F& point)
{
	if (!m_isEnabled || !m_isVisible) {
		return false;
	}
	//ZxDebugEx((__FUNCTION__"():: m_isHover : %d m_isPressed :%d \n", m_isHover, m_isPressed));

	if (IsPointInRect(point)) {
		
		return true;
	}

	return false;
}

bool CD2DControlUI::OnMouseLeave(const D2D1_POINT_2F& point)
{
    if (!m_isEnabled || !m_isVisible) {
        return false;
    }
	if (IsPointInRect(point) == false) 
		return false;
	SetFocused(false);
	SetHovered(false);
    if (!IsPressed()) {
        
    }
    
    return true;
}

bool CD2DControlUI::OnMouseEnter(const D2D1_POINT_2F& point)
{
	if (!m_isEnabled || !m_isVisible) {
		return false;
	}
	
	if (IsPointInRect(point) == true) 
	{
		SetFocused(true);
		return true;
	}
	
	return false;
}

bool CD2DControlUI::OnMouseWheel(float delta)
{
    if (!m_isEnabled || !m_isVisible || !m_isFocused) {
        return false;
    }
    
    return false;
}

bool CD2DControlUI::OnKeyDown(DWORD keyCode)
{
	if (!m_isEnabled || !m_isVisible) {
		return false;
	}

	return false;
}

bool CD2DControlUI::OnKeyUp(DWORD keyCode)
{
	if (!m_isEnabled || !m_isVisible) {
		return false;
	}

	return false;
}

bool CD2DControlUI::OnChar(DWORD ch)
{
	if (!m_isEnabled || !m_isVisible) {
		return false;
	}

	return false;
}

void CD2DControlUI::OnDpiChanged(float dpiScaleX, float dpiScaleY)
{
   
    m_dpiScaleX = dpiScaleX;
    m_dpiScaleY = dpiScaleY;
   
    float width = (m_rectangle.right - m_rectangle.left) * dpiScaleX ;
    float height = (m_rectangle.bottom - m_rectangle.top) * dpiScaleY;
    
}
