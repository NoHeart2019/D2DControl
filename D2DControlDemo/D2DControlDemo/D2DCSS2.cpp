#include "stdafx.h"
#include "D2DCSS2.h"

CD2DStatus::CD2DStatus()
	: m_flag(None)
{
}

bool CD2DStatus::Is(unsigned int flag) const
{
	return (m_flag & flag) != 0;
}

void CD2DStatus::Set(unsigned int flag, bool on)
{
	if (on)
		m_flag |= flag;
	else
		m_flag &= ~flag;
}

void CD2DStatus::SetNormal(bool on)
{
	if (on)
	{
		m_flag |= Normal;
		// Normal 状态时清除其他互斥状态
		m_flag &= ~(Hover | Pressed | Disabled | Focused | Dragging | Checked | Active);
	}
	else
	{
		m_flag &= ~Normal;
	}
}

bool CD2DStatus::IsNormal() const
{
	return Is(Normal);
}

void CD2DStatus::SetHover(bool on)
{
	Set(Hover, on);
	if (on)
		SetNormal(false);
}

bool CD2DStatus::IsHover() const
{
	return Is(Hover);
}

void CD2DStatus::SetPressed(bool on)
{
	Set(Pressed, on);
	if (on)
		SetNormal(false);
}

bool CD2DStatus::IsPressed() const
{
	return Is(Pressed);
}

void CD2DStatus::SetDisabled(bool on)
{
	Set(Disabled, on);
	if (on)
		SetNormal(false);
}

bool CD2DStatus::IsDisabled() const
{
	return Is(Disabled);
}

void CD2DStatus::SetFocused(bool on)
{
	Set(Focused, on);
	if (on)
		SetNormal(false);
}

bool CD2DStatus::IsFocused() const
{
	return Is(Focused);
}

void CD2DStatus::SetDragging(bool on)
{
	Set(Dragging, on);
	if (on)
		SetNormal(false);
}

bool CD2DStatus::IsDragging() const
{
	return Is(Dragging);
}

void CD2DStatus::SetChecked(bool on)
{
	Set(Checked, on);
	if (on)
		SetNormal(false);
}

bool CD2DStatus::IsChecked() const
{
	return Is(Checked);
}

void CD2DStatus::SetActive(bool on)
{
	Set(Active, on);
	if (on)
		SetNormal(false);
}

bool CD2DStatus::IsActive() const
{
	return Is(Active);
}

bool CD2DStatus::IsHotTracked() const
{
	return Is(Hover) && Is(Pressed);
}

void CD2DStatus::SetHotTracked(bool on)
{
	SetHover(on);
	SetPressed(on);
}

ED2DStatus CD2DStatus::GetEffectiveState() const
{
	if (Is(Disabled))   return Disabled;
	if (Is(Dragging))   return Dragging;   // 提前
	if (Is(Pressed))    return Pressed;
	if (Is(Hover))      return Hover;
	if (Is(Focused))    return Focused;
	if (Is(Active))     return Active;
	if (Is(Checked))    return Checked;
	return Normal;
}

CD2DTransform::CD2DTransform()
{
}

// 拷贝构造函数：深拷贝 m_ops 数组
CD2DTransform::CD2DTransform(const CD2DTransform& other)
{
	m_ops.Copy(other.m_ops); 
}

CD2DTransform& CD2DTransform::operator=(const CD2DTransform& other)
{
	if (this != &other)
	{
		m_ops.Copy(other.m_ops); 
	}
	return *this;
}


void CD2DTransform::SetTranslate(float tx, float ty)
{
	TransformOp op;
	op.type = Translate;
	op.param1 = tx;
	op.param2 = ty;
	m_ops.Add(op);
}

void CD2DTransform::SetRotate(float angle)
{
	TransformOp op;
	op.type = Rotate;
	op.param1 = angle;
	op.param2 = 0.0f;
	m_ops.Add(op);
}

void CD2DTransform::SetScale(float sx, float sy)
{
	TransformOp op;
	op.type = Scale;
	op.param1 = sx;
	op.param2 = sy;
	m_ops.Add(op);
}

void CD2DTransform::SetScale(float s)
{
	SetScale(s, s);
}

void CD2DTransform::SetSkew(float ax, float ay)
{
	TransformOp op;
	op.type = Skew;
	op.param1 = ax;
	op.param2 = ay;
	m_ops.Add(op);
}

void CD2DTransform::SetSkewX(float ax)
{
	SetSkew(ax, 0.0f);
}

void CD2DTransform::SetSkewY(float ay)
{
	SetSkew(0.0f, ay);
}

void CD2DTransform::Clear()
{
	m_ops.RemoveAll();
}

D2D1_MATRIX_3X2_F CD2DTransform::GetMatrix() const
{
	D2D1_MATRIX_3X2_F mat = D2D1::Matrix3x2F::Identity();

	for (int i = 0; i < m_ops.GetCount(); i++)
	{
		const TransformOp& op = m_ops[i];

		switch (op.type)
		{
		case Translate:
			{
				D2D1_MATRIX_3X2_F t = D2D1::Matrix3x2F::Translation(op.param1, op.param2);
				mat = mat * t;
				break;
			}
		case Rotate:
			{
				float rad = op.param1 * D2D1PI / 180.0f;
				D2D1_MATRIX_3X2_F r = D2D1::Matrix3x2F::Rotation(rad);
				mat = mat * r;
				break;
			}
		case Scale:
			{
				D2D1_MATRIX_3X2_F s = D2D1::Matrix3x2F::Scale(op.param1, op.param2, D2D1::Point2F(0, 0));
				mat = mat * s;
				break;
			}
		case Skew:
			{
				float radX = op.param1 * D2D1PI / 180.0f;
				float radY = op.param2 * D2D1PI / 180.0f;
				D2D1_MATRIX_3X2_F sk = D2D1::Matrix3x2F::Skew(radX, radY, D2D1::Point2F(0, 0));
				mat = mat * sk;
				break;
			}
		default:
			break;
		}
	}

	return mat;
}

size_t CD2DTransform::GetOperationCount() const
{
	return m_ops.GetCount();
}

const CAtlArray<CD2DTransform::TransformOp>& CD2DTransform::GetOperations() const
{
	return m_ops;
}

const float CD2DControlUI::DRAG_THRESHOLD = 3.0f;


CD2DControlUI::CD2DControlUI()
	: m_status()
	, m_isVisible(true)
	, m_isDragEnabled(false)          // 默认不支持拖拽
	, m_dragStartPoint(D2D1::Point2F(0.0f, 0.0f))
{
	m_rectangle = D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f);
	for (int i = 0; i < ED2DStatusMax; i++)
	{
		m_background[i].m_type = CD2DBackground::None;
		m_transform[i].Clear();
	}
}
CD2DControlUI::~CD2DControlUI()
{
}

// ========== 背景设置 ==========
// ========== 背景设置 (background) ==========
void CD2DControlUI::SetBackgroundColor(ED2DStatus state, const D2D1_COLOR_F& color)
{
	int idx = static_cast<int>(state);
	if (idx < 0 || idx >= ED2DStatusMax)
		return;

	m_background[idx].m_type = CD2DBackground::Solid;
	m_background[idx].m_solid.color = color;
}

void CD2DControlUI::SetBackgroundLinear(ED2DStatus state, float angle,
	const D2D1_GRADIENT_STOP* stops, int count)
{
	int idx = static_cast<int>(state);
	if (idx < 0 || idx >= ED2DStatusMax)
		return;
	if (count <= 0 || count > 10)
		return;

	m_background[idx].m_type = CD2DBackground::LinearGradient;
	m_background[idx].m_linear.angle = angle;

	// 根据角度计算渐变线的起点和终点（使用足够长的线来覆盖控件）
	float rad = angle * (3.14159265359f / 180.0f);
	float len = 1000.0f;
	m_background[idx].m_linear.startPoint = D2D1::Point2F(0.0f, 0.0f);
	m_background[idx].m_linear.endPoint = D2D1::Point2F(len * cosf(rad), len * sinf(rad));

	// 拷贝渐变停止点
	m_background[idx].m_linear.m_stopCount = count;
	for (int i = 0; i < count; ++i)
	{
		m_background[idx].m_linear.m_stops[i] = stops[i];
	}
}

void CD2DControlUI::SetBackgroundRadial(ED2DStatus state,
	const D2D1_POINT_2F& center, float radiusX, float radiusY,
	const D2D1_GRADIENT_STOP* stops, int count)
{
	int idx = static_cast<int>(state);
	if (idx < 0 || idx >= ED2DStatusMax)
		return;
	if (count <= 0 || count > 10)
		return;

	m_background[idx].m_type = CD2DBackground::RadialGradient;
	m_background[idx].m_radial.center = center;
	m_background[idx].m_radial.radiusX = radiusX;
	m_background[idx].m_radial.radiusY = radiusY;
	m_background[idx].m_radial.gradientOriginOffset = D2D1::Point2F(0.0f, 0.0f);

	// 拷贝渐变停止点
	m_background[idx].m_radial.m_stopCount = count;
	for (int i = 0; i < count; ++i)
	{
		m_background[idx].m_radial.m_stops[i] = stops[i];
	}
}

// ========== 便捷方法：Hover / Pressed / Drag 背景 ==========
void CD2DControlUI::SetHoverBgColor(const D2D1_COLOR_F& color)
{
	SetBackgroundColor(Hover, color);
}

void CD2DControlUI::SetPressedBgColor(const D2D1_COLOR_F& color)
{
	SetBackgroundColor(Pressed, color);
}

void CD2DControlUI::SetDragBgColor(const D2D1_COLOR_F& color)
{
	SetBackgroundColor(Dragging, color);
}

void CD2DControlUI::SetHoverBgLinear(float angle, const D2D1_GRADIENT_STOP* stops, int count)
{
	SetBackgroundLinear(Hover, angle, stops, count);
}

void CD2DControlUI::SetPressedBgLinear(float angle, const D2D1_GRADIENT_STOP* stops, int count)
{
	SetBackgroundLinear(Pressed, angle, stops, count);
}

void CD2DControlUI::SetDragBgLinear(float angle, const D2D1_GRADIENT_STOP* stops, int count)
{
	SetBackgroundLinear(Dragging, angle, stops, count);
}

void CD2DControlUI::SetHoverBgRadial(const D2D1_POINT_2F& center, float radiusX, float radiusY,
	const D2D1_GRADIENT_STOP* stops, int count)
{
	SetBackgroundRadial(Hover, center, radiusX, radiusY, stops, count);
}

void CD2DControlUI::SetPressedBgRadial(const D2D1_POINT_2F& center, float radiusX, float radiusY,
	const D2D1_GRADIENT_STOP* stops, int count)
{
	SetBackgroundRadial(Pressed, center, radiusX, radiusY, stops, count);
}

void CD2DControlUI::SetDragBgRadial(const D2D1_POINT_2F& center, float radiusX, float radiusY,
	const D2D1_GRADIENT_STOP* stops, int count)
{
	SetBackgroundRadial(Dragging, center, radiusX, radiusY, stops, count);
}
// ========== 边框设置 ==========
void CD2DControlUI::SetBorder(float width, ED2DLineStyle style, const D2D1_COLOR_F& color)
{
	SetBorderTop(width, style, color);
	SetBorderRight(width, style, color);
	SetBorderBottom(width, style, color);
	SetBorderLeft(width, style, color);
}

void CD2DControlUI::SetBorderTop(float width, ED2DLineStyle style, const D2D1_COLOR_F& color)
{
	m_border.m_top.m_width = width;
	m_border.m_top.m_style = style;
	m_border.m_top.m_color = color;
	if (style == Dashed || style == Dotted)
	{
		m_border.m_top.m_dashLength = 4.0f;
		m_border.m_top.m_gapLength = 2.0f;
	}
}

void CD2DControlUI::SetBorderRight(float width, ED2DLineStyle style, const D2D1_COLOR_F& color)
{
	m_border.m_right.m_width = width;
	m_border.m_right.m_style = style;
	m_border.m_right.m_color = color;
	if (style == Dashed || style == Dotted)
	{
		m_border.m_right.m_dashLength = 4.0f;
		m_border.m_right.m_gapLength = 2.0f;
	}
}

void CD2DControlUI::SetBorderBottom(float width, ED2DLineStyle style, const D2D1_COLOR_F& color)
{
	m_border.m_bottom.m_width = width;
	m_border.m_bottom.m_style = style;
	m_border.m_bottom.m_color = color;
	if (style == Dashed || style == Dotted)
	{
		m_border.m_bottom.m_dashLength = 4.0f;
		m_border.m_bottom.m_gapLength = 2.0f;
	}
}

void CD2DControlUI::SetBorderLeft(float width, ED2DLineStyle style, const D2D1_COLOR_F& color)
{
	m_border.m_left.m_width = width;
	m_border.m_left.m_style = style;
	m_border.m_left.m_color = color;
	if (style == Dashed || style == Dotted)
	{
		m_border.m_left.m_dashLength = 4.0f;
		m_border.m_left.m_gapLength = 2.0f;
	}
}

const CD2DBorder& CD2DControlUI::GetBorder() const { return m_border; }
const CD2DBorderEdge& CD2DControlUI::GetBorderTop() const { return m_border.m_top; }
const CD2DBorderEdge& CD2DControlUI::GetBorderRight() const { return m_border.m_right; }
const CD2DBorderEdge& CD2DControlUI::GetBorderBottom() const { return m_border.m_bottom; }
const CD2DBorderEdge& CD2DControlUI::GetBorderLeft() const { return m_border.m_left; }

// ========== 圆角设置 ==========
void CD2DControlUI::SetBorderRadius(float radius)
{
	SetBorderTopLeftRadius(radius);
	SetBorderTopRightRadius(radius);
	SetBorderBottomRightRadius(radius);
	SetBorderBottomLeftRadius(radius);
}

void CD2DControlUI::SetBorderTopLeftRadius(float radius) { m_radius.m_topLeft = radius; }
void CD2DControlUI::SetBorderTopRightRadius(float radius) { m_radius.m_topRight = radius; }
void CD2DControlUI::SetBorderBottomRightRadius(float radius) { m_radius.m_bottomRight = radius; }
void CD2DControlUI::SetBorderBottomLeftRadius(float radius) { m_radius.m_bottomLeft = radius; }

const CD2DRadius& CD2DControlUI::GetBorderRadius() const { return m_radius; }
float CD2DControlUI::GetBorderTopLeftRadius() const { return m_radius.m_topLeft; }
float CD2DControlUI::GetBorderTopRightRadius() const { return m_radius.m_topRight; }
float CD2DControlUI::GetBorderBottomRightRadius() const { return m_radius.m_bottomRight; }
float CD2DControlUI::GetBorderBottomLeftRadius() const { return m_radius.m_bottomLeft; }

// ========== 边距设置 ==========
void CD2DControlUI::SetMargin(float top, float right, float bottom, float left)
{
	m_margin.top = top;
	m_margin.right = right;
	m_margin.bottom = bottom;
	m_margin.left = left;
}

void CD2DControlUI::SetMarginTop(float top) { m_margin.top = top; }
void CD2DControlUI::SetMarginRight(float right) { m_margin.right = right; }
void CD2DControlUI::SetMarginBottom(float bottom) { m_margin.bottom = bottom; }
void CD2DControlUI::SetMarginLeft(float left) { m_margin.left = left; }

const CD2DMargin& CD2DControlUI::GetMargin() const { return m_margin; }
float CD2DControlUI::GetMarginTop() const { return m_margin.top; }
float CD2DControlUI::GetMarginRight() const { return m_margin.right; }
float CD2DControlUI::GetMarginBottom() const { return m_margin.bottom; }
float CD2DControlUI::GetMarginLeft() const { return m_margin.left; }

// ========== 内边距设置 ==========
void CD2DControlUI::SetPadding(float top, float right, float bottom, float left)
{
	m_padding.top = top;
	m_padding.right = right;
	m_padding.bottom = bottom;
	m_padding.left = left;
}

void CD2DControlUI::SetPaddingTop(float top) { m_padding.top = top; }
void CD2DControlUI::SetPaddingRight(float right) { m_padding.right = right; }
void CD2DControlUI::SetPaddingBottom(float bottom) { m_padding.bottom = bottom; }
void CD2DControlUI::SetPaddingLeft(float left) { m_padding.left = left; }

const CD2DPadding& CD2DControlUI::GetPadding() const { return m_padding; }
float CD2DControlUI::GetPaddingTop() const { return m_padding.top; }
float CD2DControlUI::GetPaddingRight() const { return m_padding.right; }
float CD2DControlUI::GetPaddingBottom() const { return m_padding.bottom; }
float CD2DControlUI::GetPaddingLeft() const { return m_padding.left; }

// ========== 区域矩形 ==========
D2D1_RECT_F CD2DControlUI::GetContentRectangle() const
{
	D2D1_RECT_F r = m_rectangle;
	r.left += m_margin.left + m_border.m_left.m_width + m_padding.left;
	r.top += m_margin.top + m_border.m_top.m_width + m_padding.top;
	r.right -= m_margin.right + m_border.m_right.m_width + m_padding.right;
	r.bottom -= m_margin.bottom + m_border.m_bottom.m_width + m_padding.bottom;
	return r;
}

D2D1_RECT_F CD2DControlUI::GetPaddingRectangle() const
{
	D2D1_RECT_F r = m_rectangle;
	r.left += m_margin.left + m_border.m_left.m_width;
	r.top += m_margin.top + m_border.m_top.m_width;
	r.right -= m_margin.right + m_border.m_right.m_width;
	r.bottom -= m_margin.bottom + m_border.m_bottom.m_width;
	return r;
}

D2D1_RECT_F CD2DControlUI::GetBorderRectangle() const
{
	D2D1_RECT_F r = m_rectangle;
	r.left += m_margin.left;
	r.top += m_margin.top;
	r.right -= m_margin.right;
	r.bottom -= m_margin.bottom;
	return r;
}

D2D1_RECT_F CD2DControlUI::GetMarginRectangle() const
{
	return m_rectangle;
}

D2D1_SIZE_F CD2DControlUI::GetBoxModelSize() const
{
	float width = m_rectangle.right - m_rectangle.left;
	float height = m_rectangle.bottom - m_rectangle.top;

	width += m_margin.left + m_margin.right;
	width += m_border.m_left.m_width + m_border.m_right.m_width;
	width += m_padding.left + m_padding.right;

	height += m_margin.top + m_margin.bottom;
	height += m_border.m_top.m_width + m_border.m_bottom.m_width;
	height += m_padding.top + m_padding.bottom;

	return D2D1::SizeF(width, height);
}

// ========== 状态设置 ==========
void CD2DControlUI::SetNormal(bool on) { m_status.SetNormal(on); }
void CD2DControlUI::SetHover(bool on) { m_status.SetHover(on); }
void CD2DControlUI::SetPressed(bool on) { m_status.SetPressed(on); }
void CD2DControlUI::SetDisabled(bool on) { m_status.SetDisabled(on); }
void CD2DControlUI::SetDragging(bool on) { m_status.SetDragging(on); }
void CD2DControlUI::SetFocused(bool on) { m_status.SetFocused(on); }

// ========== 状态判断 ==========
bool CD2DControlUI::IsNormal() const { return m_status.IsNormal(); }
bool CD2DControlUI::IsHover() const { return m_status.IsHover(); }
bool CD2DControlUI::IsPressed() const { return m_status.IsPressed(); }
bool CD2DControlUI::IsDisabled() const { return m_status.IsDisabled(); }
bool CD2DControlUI::IsDragging() const { return m_status.IsDragging(); }
bool CD2DControlUI::IsFocused() const { return m_status.IsFocused(); }

// ========== 变换 ==========

// 为指定状态设置平移变换
void CD2DControlUI::SetTransformTranslate(ED2DStatus state, float x, float y)
{
	int idx = static_cast<int>(state);
	if (idx >= 0 && idx < ED2DStatusMax)
		m_transform[idx].SetTranslate(x, y);
}

void CD2DControlUI::SetTransformRotate(ED2DStatus state, float angle)
{
	int idx = static_cast<int>(state);
	if (idx >= 0 && idx < ED2DStatusMax)
		m_transform[idx].SetRotate(angle);
}

void CD2DControlUI::SetTransformScale(ED2DStatus state, float sx, float sy)
{
	int idx = static_cast<int>(state);
	if (idx >= 0 && idx < ED2DStatusMax)
		m_transform[idx].SetScale(sx, sy);
}

void CD2DControlUI::SetTransformSkew(ED2DStatus state, float ax, float ay)
{
	int idx = static_cast<int>(state);
	if (idx >= 0 && idx < ED2DStatusMax)
		m_transform[idx].SetSkew(ax, ay);
}

D2D1_MATRIX_3X2_F CD2DControlUI::GetTransformMatrix() const
{
	int idx = static_cast<int>(m_status.GetEffectiveState());
	return m_transform[idx].GetMatrix();
}

// ========== 矩形设置 ==========
void CD2DControlUI::SetRectangle(const D2D1_RECT_F& rect)
{
	m_rectangle = rect;
}

void CD2DControlUI::SetRectangle(float left, float top, float right, float bottom)
{
	m_rectangle = D2D1::RectF(left, top, right, bottom);
}

D2D1_RECT_F CD2DControlUI::GetRectangle() const
{
	return m_rectangle;
}

D2D1_ROUNDED_RECT CD2DControlUI::GetRoundedRectangle() const
{
	D2D1_ROUNDED_RECT rr;
	rr.rect = m_rectangle;
	rr.radiusX = m_radius.m_topLeft;
	rr.radiusY = m_radius.m_topLeft;

	return rr;
}

// ========== 中心点操作 ==========
void CD2DControlUI::SetCenter(const D2D1_POINT_2F& center)
{
	float w = m_rectangle.right - m_rectangle.left;
	float h = m_rectangle.bottom - m_rectangle.top;
	m_rectangle.left = center.x - w / 2.0f;
	m_rectangle.top = center.y - h / 2.0f;
	m_rectangle.right = center.x + w / 2.0f;
	m_rectangle.bottom = center.y + h / 2.0f;
}

void CD2DControlUI::SetCenter(float x, float y)
{
	SetCenter(D2D1::Point2F(x, y));
}

D2D1_POINT_2F CD2DControlUI::GetCenter() const
{
	return D2D1::Point2F(
		(m_rectangle.left + m_rectangle.right) / 2.0f,
		(m_rectangle.top + m_rectangle.bottom) / 2.0f
		);
}

// ========== 位置操作 ==========
void CD2DControlUI::SetPosition(const D2D1_POINT_2F& pos)
{
	float w = m_rectangle.right - m_rectangle.left;
	float h = m_rectangle.bottom - m_rectangle.top;
	m_rectangle.left = pos.x;
	m_rectangle.top = pos.y;
	m_rectangle.right = pos.x + w;
	m_rectangle.bottom = pos.y + h;
}

void CD2DControlUI::SetPosition(float x, float y)
{
	SetPosition(D2D1::Point2F(x, y));
}

D2D1_POINT_2F CD2DControlUI::GetPosition() const
{
	return D2D1::Point2F(m_rectangle.left, m_rectangle.top);
}

// ========== 尺寸设置 ==========
void CD2DControlUI::SetSize(float width, float height)
{
	m_rectangle.right = m_rectangle.left + width;
	m_rectangle.bottom = m_rectangle.top + height;
}

void CD2DControlUI::SetSize(const D2D1_SIZE_F& size)
{
	SetSize(size.width, size.height);
}

void CD2DControlUI::SetWidth(float width)
{
	m_rectangle.right = m_rectangle.left + width;
}

void CD2DControlUI::SetHeight(float height)
{
	m_rectangle.bottom = m_rectangle.top + height;
}

// ========== 尺寸获取 ==========
D2D1_SIZE_F CD2DControlUI::GetSize() const
{
	return D2D1::SizeF(
		m_rectangle.right - m_rectangle.left,
		m_rectangle.bottom - m_rectangle.top
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

void CD2DControlUI::Resize(float width, float height)
{
	
	float centerX = (m_rectangle.left + m_rectangle.right) / 2.0f;
	float centerY = (m_rectangle.top + m_rectangle.bottom) / 2.0f;
	m_rectangle.left = centerX - width / 2.0f;
	m_rectangle.right = centerX + width / 2.0f;
	m_rectangle.top = centerY - height / 2.0f;
	m_rectangle.bottom = centerY + height / 2.0f;
	
}

// ========== 矩形操作 ==========
void CD2DControlUI::Move(float dx, float dy)
{
	m_rectangle.left += dx;
	m_rectangle.top += dy;
	m_rectangle.right += dx;
	m_rectangle.bottom += dy;
}

void CD2DControlUI::MoveTo(float x, float y)
{
	float w = m_rectangle.right - m_rectangle.left;
	float h = m_rectangle.bottom - m_rectangle.top;
	m_rectangle.left = x;
	m_rectangle.top = y;
	m_rectangle.right = x + w;
	m_rectangle.bottom = y + h;
}

void CD2DControlUI::OffsetRect(float left, float top, float right, float bottom)
{
	m_rectangle.left += left;
	m_rectangle.top += top;
	m_rectangle.right -= right;
	m_rectangle.bottom -= bottom;
}


void CD2DControlUI::SetVisible(bool visible)
{
	m_isVisible = visible;
}

bool CD2DControlUI::IsVisible() const
{
	return m_isVisible;
}

void CD2DControlUI::EnableDrag(bool enable)
{
	m_isDragEnabled = enable;
}

bool CD2DControlUI::IsDragEnabled() const
{
	return m_isDragEnabled;
}

bool CD2DControlUI::IsHitTest(const D2D1_POINT_2F& point) const
{
	// 使用边框矩形（包含边框、内边距和内容区域）
	D2D1_RECT_F borderRect = GetBorderRectangle();

	// 简单矩形包含检测
	if (point.x >= borderRect.left && point.x <= borderRect.right &&
		point.y >= borderRect.top  && point.y <= borderRect.bottom)
	{
		return true;
	}
	return false;
}

bool CD2DControlUI::OnMouseDownL(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled())
		return false;

	if (!IsHitTest(point))
		return false;

	// 记录拖拽起点，进入按下状态
	m_dragStartPoint = point;
	SetPressed(true);
	SetDragging(false);  // 初始未开始拖拽

	return true;  // 消费事件
}

bool CD2DControlUI::OnMouseMove(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled())
		return false;

	// 拖拽中（仅可拖拽控件）
	if (IsDragging() && IsDragEnabled())
	{
		D2D1_POINT_2F delta = D2D1::Point2F(
			point.x - m_dragStartPoint.x,
			point.y - m_dragStartPoint.y);
		if (delta.x != 0.0f || delta.y != 0.0f)
		{
			Move(delta.x, delta.y);
			m_dragStartPoint = point;
			OnDragMove(delta);
		}
		return true;
	}

	// 按下状态，尝试进入拖拽
	if (IsPressed())
	{
		if (IsDragEnabled())
		{
			float dx = point.x - m_dragStartPoint.x;
			float dy = point.y - m_dragStartPoint.y;
			if (fabsf(dx) > DRAG_THRESHOLD || fabsf(dy) > DRAG_THRESHOLD)
			{
				SetPressed(false);
				SetDragging(true);
				OnDragBegin(m_dragStartPoint);
				Move(dx, dy);
				m_dragStartPoint = point;
				OnDragMove(D2D1::Point2F(dx, dy));
				return true;
			}
		}
		return false;
	}

	// 正常悬停
	if (IsHitTest(point))
	{
		if (!IsHover()) SetHover(true);
	}
	else
	{
		if (IsHover()) SetHover(false);
	}

	return false;
}

bool CD2DControlUI::OnMouseUpL(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled())
		return false;

	// 拖拽结束
	if (IsDragging())
	{
		SetDragging(false);
		OnDragEnd(point);
		return true;
	}

	// 普通点击结束
	if (IsPressed())
	{
		SetPressed(false);
		if (IsHitTest(point))
		{
			// 可在此触发点击回调
		}
		return true;
	}

	return false;
}

bool CD2DControlUI::OnMouseDownL2(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled())
		return false;
	if (!IsHitTest(point))
		return false;
	return true;
}

bool CD2DControlUI::OnMouseUpL2(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled())
		return false;
	if (!IsHitTest(point))
		return false;
	return true;
}

bool CD2DControlUI::OnMouseLeave(const D2D1_POINT_2F& /*point*/)
{
	// 拖拽状态下不清除 Pressed 或 Dragging
	if (!IsDragging())
	{
		SetPressed(false);
	}
	SetHover(false);
	return false;
}

bool CD2DControlUI::OnMouseEnter(const D2D1_POINT_2F& /*point*/)
{
	if (!IsVisible() || IsDisabled())
		return false;
	SetHover(true);
	return false;
}

void CD2DControlUI::OnDragBegin(const D2D1_POINT_2F& /*startPoint*/) {}
void CD2DControlUI::OnDragMove(const D2D1_POINT_2F& /*delta*/) {}
void CD2DControlUI::OnDragEnd(const D2D1_POINT_2F& /*endPoint*/) {}