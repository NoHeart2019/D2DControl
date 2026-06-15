// CD2DControlCssUI.h
#pragma once

#include <d2d1.h>
#include <d2d1helper.h>
#include <atlcoll.h>

// ==================== 基础类型（沿用原有定义） ====================
enum ED2DLineStyle { None, Solid, Dashed, Dotted, Double };

struct CD2DBorderEdge {
	float m_width;
	ED2DLineStyle m_style;
	D2D1_COLOR_F m_color;
	float m_dashLength;
	float m_gapLength;

	CD2DBorderEdge()
		: m_width(0.0f), m_style(None)
		, m_color(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f))
		, m_dashLength(0.0f), m_gapLength(0.0f) {}
};

struct CD2DRadius {
	float m_topLeft, m_topRight, m_bottomRight, m_bottomLeft;
	CD2DRadius()
		: m_topLeft(0.0f), m_topRight(0.0f)
		, m_bottomRight(0.0f), m_bottomLeft(0.0f) {}
	CD2DRadius(float uniform)
		: m_topLeft(uniform), m_topRight(uniform)
		, m_bottomRight(uniform), m_bottomLeft(uniform) {}
};

struct CD2DBorder {
	CD2DBorderEdge m_top, m_right, m_bottom, m_left;
	CD2DBorder() {}
};

struct CD2DBackground {
	enum EType { None, Solid, LinearGradient, RadialGradient };
	struct CSolidColor { D2D1_COLOR_F color; };
	struct CGradientStop { float position; D2D1_COLOR_F color; };

	struct CLinearGradient {
		D2D1_POINT_2F startPoint, endPoint;
		float angle;
		D2D1_GRADIENT_STOP m_stops[10];
		int m_stopCount;
		CLinearGradient()
			: startPoint(D2D1::Point2F(0.0f, 0.0f))
			, endPoint(D2D1::Point2F(0.0f, 0.0f))
			, m_stopCount(0) {}
	};

	struct CRadialGradient {
		D2D1_POINT_2F center, gradientOriginOffset;
		float radiusX, radiusY;
		D2D1_GRADIENT_STOP m_stops[10];
		int m_stopCount;
		CRadialGradient()
			: center(D2D1::Point2F(0.0f, 0.0f))
			, gradientOriginOffset(D2D1::Point2F(0.0f, 0.0f))
			, radiusX(0.0f), radiusY(0.0f), m_stopCount(0) {}
	};

	EType m_type;
	CSolidColor m_solid;
	CLinearGradient m_linear;
	CRadialGradient m_radial;
	CD2DBackground() : m_type(None) {}
};

class CD2DTransform {
public:
	enum EType { none, Translate, Rotate, Scale, Skew };
	struct TransformOp {
		EType type;
		float param1, param2;
	};

	CD2DTransform() {}
	CD2DTransform(const CD2DTransform& other) { m_ops.Copy(other.m_ops); }
	CD2DTransform& operator=(const CD2DTransform& other) {
		if (this != &other) m_ops.Copy(other.m_ops);
		return *this;
	}

	void SetTranslate(float tx, float ty) {
		TransformOp op = { Translate, tx, ty }; m_ops.Add(op);
	}
	void SetRotate(float angle) {
		TransformOp op = { Rotate, angle, 0.0f }; m_ops.Add(op);
	}
	void SetScale(float sx, float sy) {
		TransformOp op = { Scale, sx, sy }; m_ops.Add(op);
	}
	void SetScale(float s) { SetScale(s, s); }
	void SetSkew(float ax, float ay) {
		TransformOp op = { Skew, ax, ay }; m_ops.Add(op);
	}
	void SetSkewX(float ax) { SetSkew(ax, 0.0f); }
	void SetSkewY(float ay) { SetSkew(0.0f, ay); }
	void Clear() { m_ops.RemoveAll(); }
	D2D1_MATRIX_3X2_F GetMatrix() const; // 实现略
	size_t GetOperationCount() const { return m_ops.GetCount(); }
	const CAtlArray<TransformOp>& GetOperations() const { return m_ops; }

private:
	CAtlArray<TransformOp> m_ops;
};

enum ED2DStatus {
	Normal = 0, Hover = 1, Pressed = 2, Disabled = 3,
	Focused = 4, Dragging = 5, Checked = 6, Active = 7, HotTracked = 8,
	ED2DStatusMax = 9
};

struct CD2DStatus {
	enum EFlag {
		None        = 0,
		Normal      = 1 << 0,
		Hover       = 1 << 1,
		Pressed     = 1 << 2,
		Disabled    = 1 << 3,
		Focused     = 1 << 4,
		Dragging    = 1 << 5,
		Checked     = 1 << 6,
		Active      = 1 << 7,
		HotTracked  = 1 << 8
	};

	unsigned int m_flag;

	CD2DStatus() : m_flag(Normal) {}
	bool Is(unsigned int flag) const { return (m_flag & flag) != 0; }
	void Set(unsigned int flag, bool on = true) {
		if (on) m_flag |= flag; else m_flag &= ~flag;
	}
	void SetNormal(bool on = true) { Set(Normal, on); }
	bool IsNormal() const { return Is(Normal); }
	void SetHover(bool on = true) { Set(Hover, on); }
	bool IsHover() const { return Is(Hover); }
	void SetPressed(bool on = true) { Set(Pressed, on); }
	bool IsPressed() const { return Is(Pressed); }
	void SetDisabled(bool on = true) { Set(Disabled, on); }
	bool IsDisabled() const { return Is(Disabled); }
	void SetFocused(bool on = true) { Set(Focused, on); }
	bool IsFocused() const { return Is(Focused); }
	void SetDragging(bool on = true) { Set(Dragging, on); }
	bool IsDragging() const { return Is(Dragging); }
	void SetChecked(bool on = true) { Set(Checked, on); }
	bool IsChecked() const { return Is(Checked); }
	void SetActive(bool on = true) { Set(Active, on); }
	bool IsActive() const { return Is(Active); }
	bool IsHotTracked() const { return IsHover() && IsPressed(); }
	void SetHotTracked(bool on = true) {
		SetHover(on); SetPressed(on);
	}
	 GetEffectiveState() const; 
	ED2DStatus	GetEffectiveState() const          // 实现略
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
	
};

// ==================== 盒模型类型 ====================
struct CD2DMargin : D2D_RECT_F
{
	CD2DMargin() 
	{
		left = top = right = bottom = 0.0f;
	}
};

struct CD2DPadding : D2D_RECT_F
{
	CD2DPadding() 
	{
		left = top = right = bottom = 0.0f;
	}
};

// ==================== 可层叠样式 ====================
struct CD2DStyle {
	CD2DMargin    margin;
	CD2DBorder    border;
	CD2DPadding   padding;
	CD2DRadius    radius;
	CD2DBackground background;
	CD2DTransform transform;

	CD2DStyle()
		: margin(D2D1::RectF()), padding(D2D1::RectF()) {}
};

// ==================== 核心控件类 ====================
class CD2DControlCssUI {
public:
	CD2DControlCssUI();
	~CD2DControlCssUI();

	// 布局矩形
	void SetRect(const D2D1_RECT_F& rect) { m_rectangle = rect; }
	const D2D1_RECT_F& GetRect() const { return m_rectangle; }

	// 样式管理
	void SetStyle(ED2DStatus state, const CD2DStyle& style);
	const CD2DStyle& GetStyle(ED2DStatus state) const;
	bool HasStyle(ED2DStatus state) const;

	// 默认样式（优先级最低）
	void SetDefaultStyle(const CD2DStyle& style);
	const CD2DStyle& GetDefaultStyle() const;

	// 状态标志
	CD2DStatus& GetStatus() { return m_status; }
	const CD2DStatus& GetStatus() const { return m_status; }

	// 计算最终生效样式
	CD2DStyle CalculateEffectiveStyle() const;

	// 辅助：样式叠加
	static void ApplyStyle(CD2DStyle& dest, const CD2DStyle& src);

private:
	D2D1_RECT_F m_rectangle;                    // 控件区域
	CAtlMap<ED2DStatus, CD2DStyle> m_styles;    // 各状态样式（使用ATL映射）
	CD2DStyle m_defaultStyle;                   // 全局默认
	CD2DStatus m_status;                        // 当前激活状态
};