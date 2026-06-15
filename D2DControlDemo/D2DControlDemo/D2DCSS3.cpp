// CD2DControlCssUI.cpp
#include "D2DCSS3.h"

// ==================== 内部辅助函数 ====================
static bool IsBorderEdgeSet(const CD2DBorderEdge& edge) {
	return edge.m_style != None
		|| edge.m_width != 0.0f
		|| edge.m_color.a != 0.0f;
}

static bool IsRadiusSet(const CD2DRadius& radius) {
	return radius.m_topLeft != 0.0f
		|| radius.m_topRight != 0.0f
		|| radius.m_bottomRight != 0.0f
		|| radius.m_bottomLeft != 0.0f;
}

static bool IsBackgroundSet(const CD2DBackground& bg) {
	return bg.m_type != CD2DBackground::None;
}

static bool IsTransformSet(const CD2DTransform& trans) {
	return trans.GetOperationCount() > 0;
}

// 判断矩形四边是否全为 0（即未设置）
static bool IsMarginSet(const D2D1_RECT_F& rect) {
	return rect.left != 0.0f || rect.top != 0.0f 
		|| rect.right != 0.0f || rect.bottom != 0.0f;
}

static bool IsPaddingSet(const D2D1_RECT_F& rect) {
	return rect.left != 0.0f || rect.top != 0.0f 
		|| rect.right != 0.0f || rect.bottom != 0.0f;
}

// ==================== ApplyStyle ====================
void CD2DControlCssUI::ApplyStyle(CD2DStyle& dest, const CD2DStyle& src) {
	// 边框
	if (IsBorderEdgeSet(src.border.m_top))    dest.border.m_top = src.border.m_top;
	if (IsBorderEdgeSet(src.border.m_right))  dest.border.m_right = src.border.m_right;
	if (IsBorderEdgeSet(src.border.m_bottom)) dest.border.m_bottom = src.border.m_bottom;
	if (IsBorderEdgeSet(src.border.m_left))   dest.border.m_left = src.border.m_left;

	// 盒模型（使用显式标志）
	if (IsPaddingSet(src.margin))  { dest.margin = src.margin; }
	if (IsPaddingSet(src.padding)) { dest.padding = src.padding; }

	// 其它属性
	if (IsRadiusSet(src.radius))         dest.radius = src.radius;
	if (IsBackgroundSet(src.background)) dest.background = src.background;
	if (IsTransformSet(src.transform))   dest.transform = src.transform;
}

// ==================== 构造/析构 ====================
CD2DControlCssUI::CD2DControlCssUI()
	: m_rectangle(D2D1::RectF()) {}

CD2DControlCssUI::~CD2DControlCssUI() {}

// ==================== 样式存取 ====================
void CD2DControlCssUI::SetStyle(ED2DStatus state, const CD2DStyle& style) {
	m_styles.SetAt(state, style);
}

const CD2DStyle& CD2DControlCssUI::GetStyle(ED2DStatus state) const {
	POSITION pos = m_styles.Lookup(state);
	if (pos != NULL) {
		return m_styles.GetAt(pos).m_value;   // CAtlMap 元素为 CPair，取其值
	}
	static const CD2DStyle s_empty;
	return s_empty;
}

bool CD2DControlCssUI::HasStyle(ED2DStatus state) const {
	return m_styles.Lookup(state) != NULL;
}

// ==================== 默认样式 ====================
void CD2DControlCssUI::SetDefaultStyle(const CD2DStyle& style) {
	m_defaultStyle = style;
}

const CD2DStyle& CD2DControlCssUI::GetDefaultStyle() const {
	return m_defaultStyle;
}

// ==================== 层叠计算 ====================
CD2DStyle CD2DControlCssUI::CalculateEffectiveStyle() const {
	CD2DStyle effective = m_defaultStyle;

	if (m_status.IsNormal())    ApplyStyle(effective, GetStyle(Normal));
	if (m_status.IsActive())    ApplyStyle(effective, GetStyle(Active));
	if (m_status.IsChecked())   ApplyStyle(effective, GetStyle(Checked));
	if (m_status.IsDragging())  ApplyStyle(effective, GetStyle(Dragging));
	if (m_status.IsFocused())   ApplyStyle(effective, GetStyle(Focused));
	if (m_status.IsHover())     ApplyStyle(effective, GetStyle(Hover));
	if (m_status.IsPressed())   ApplyStyle(effective, GetStyle(Pressed));

	// HotTracked 组合状态优先级高于单独的 Hover 和 Pressed
	if (m_status.IsHotTracked() && HasStyle(HotTracked)) {
		ApplyStyle(effective, GetStyle(HotTracked));
	}

	// Disabled 最高优先级
	if (m_status.IsDisabled())  ApplyStyle(effective, GetStyle(Disabled));

	return effective;
}