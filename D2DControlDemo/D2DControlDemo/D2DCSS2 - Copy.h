#pragma once
#include <d2d1.h>
#include <atlbase.h>
#include <atlcom.h>

typedef D2D_RECT_F CD2DMargin;
typedef D2D_RECT_F CD2DPadding;

/*
用 Direct2D 实现一组常见 CSS 功能，适用于简单 UI 控件.
函数名尽量符合CSS属性的名称
边框样式（四条边，线型，圆角）
背景（纯色，线性渐变，径向渐变）
状态 悬浮，按下，拖拽（不同状态对应不同的背景色）
变换： 平移、旋转、缩放、倾斜
*/

#define D2D1PI 3.14159265359f

enum ED2DLineStyle
{
	None,
	Solid,
	Dashed,
	Dotted,
	Double,
};

struct CD2DBorderEdge
{
	float m_width;
	ED2DLineStyle m_style;
	D2D1_COLOR_F m_color;
	float m_dashLength;
	float m_gapLength;

	CD2DBorderEdge() 
		: m_width(0.0f), m_style(None), m_color(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f)), 
		m_dashLength(0.0f), m_gapLength(0.0f) {}
};

struct CD2DRadius
{
	float m_topLeft;
	float m_topRight;
	float m_bottomRight;
	float m_bottomLeft;

	CD2DRadius() 
		: m_topLeft(0.0f), m_topRight(0.0f), m_bottomRight(0.0f), m_bottomLeft(0.0f) {}
};

struct CD2DBorder
{
	CD2DBorderEdge m_top;
	CD2DBorderEdge m_right;
	CD2DBorderEdge m_bottom;
	CD2DBorderEdge m_left;

	CD2DBorder() {}
};

struct CD2DBackground
{
	enum EType { None, Solid, LinearGradient, RadialGradient };

	struct CSolidColor { D2D1_COLOR_F color; };
	struct CGradientStop { float position; D2D1_COLOR_F color; };

	struct CLinearGradient
	{
		D2D1_POINT_2F startPoint;
		D2D1_POINT_2F endPoint;
		float		  angle;
		D2D1_GRADIENT_STOP m_stops[10];
		int m_stopCount;

		CLinearGradient() 
			: startPoint(D2D1::Point2F(0.0f, 0.0f)), endPoint(D2D1::Point2F(0.0f, 0.0f)), m_stopCount(0) {}
	};

	struct CRadialGradient
	{
		D2D1_POINT_2F center;
		D2D1_POINT_2F gradientOriginOffset;
		float radiusX;
		float radiusY;
		D2D1_GRADIENT_STOP m_stops[10];
		int m_stopCount;

		CRadialGradient() 
			: center(D2D1::Point2F(0.0f, 0.0f)), gradientOriginOffset(D2D1::Point2F(0.0f, 0.0f)),
			radiusX(0.0f), radiusY(0.0f), m_stopCount(0) {}
	};

	EType m_type;
	CSolidColor m_solid;
	CLinearGradient m_linear;
	CRadialGradient m_radial;

	CD2DBackground() : m_type(None) {}
};

class CD2DTransform
{
public:
	// 变换类型
	enum EType
	{
		none,
		Translate,   // translate(tx, ty)
		Rotate,      // rotate(angle)  单位：度
		Scale,       // scale(sx, sy)
		Skew         // skew(ax, ay)   单位：度
	};

	// 单个变换操作
	struct TransformOp
	{
		EType type;
		float param1;   // translateX, rotateAngle, scaleX, skewX
		float param2;   // translateY, (rotate忽略), scaleY, skewY
	};

	CD2DTransform();
	CD2DTransform(const CD2DTransform& other);
	CD2DTransform& operator=(const CD2DTransform& other);

	// 添加变换（按顺序累积）
	void SetTranslate(float tx, float ty);
	void SetRotate(float angle);
	void SetScale(float sx, float sy);
	void SetScale(float s);
	void Setkew(float ax, float ay);
	void SetSkewX(float ax);
	void SetSkewY(float ay);

	// 清空所有变换操作
	void Clear();

	// 获取最终变换矩阵
	D2D1_MATRIX_3X2_F GetMatrix() const;

	// 查询操作数量
	size_t GetOperationCount() const;

	// 获取操作列表（只读）
	const CAtlArray<TransformOp>& GetOperations() const;

private:
	CAtlArray<TransformOp> m_ops;
};

enum ED2DStatus 
{
	Normal      = 0,
	Hover       = 1,
	Pressed     = 2,
	Disabled    = 3,
	Focused     = 4,
	Dragging    = 5,
	Checked     = 6,
	Active      = 7,
	HotTracked  = 8,   // 组合状态索引，不实际存储在 m_flags 中

	ED2DStatusMax = 9
};

struct CD2DStatus
{
public:
	enum EFlag 
	{
		None        = 0,
		Normal      = 1 << 0,
		Hover       = 1 << 1,
		Pressed     = 1 << 2,
		Disabled    = 1 << 3,
		Focused     = 1 << 4,
		Dragging    = 1 << 5,
		Checked     = 1 << 6,
		Active      = 1 << 7,
		HotTracked  = 1 << 8,   // 组合状态索引，不实际存储在 m_flags 中
	};

public:
	CD2DStatus();

	// 原始标志操作
	bool Is(unsigned int flag) const;
	void Set(unsigned int flag, bool on = true);

	// 各状态便捷接口
	void SetNormal(bool on = true);
	bool IsNormal() const;

	void SetHover(bool on = true);
	bool IsHover() const;

	void SetPressed(bool on = true);
	bool IsPressed() const;

	void SetDisabled(bool on = true);
	bool IsDisabled() const;

	void SetFocused(bool on = true);
	bool IsFocused() const;

	void SetDragging(bool on = true);
	bool IsDragging() const;

	void SetChecked(bool on = true);
	bool IsChecked() const;

	void SetActive(bool on = true);
	bool IsActive() const;

	// HotTracked 是 Hover + Pressed 的组合，无需单独标志位
	bool IsHotTracked() const;
	void SetHotTracked(bool on = true);   // 同时设置 Hover 和 Pressed

	ED2DStatus GetEffectiveState() const;

	unsigned int m_flag;
};

class CD2DControlUI
{
public:
	CD2DControlUI();
	virtual ~CD2DControlUI();

	// ========== 背景设置 (background) ==========
	void SetBackgroundColor(ED2DStatus state, const D2D1_COLOR_F& color);
	void SetBackgroundLinear(ED2DStatus state, float angle, 
		const D2D1_GRADIENT_STOP* stops, int count);
	void SetBackgroundRadial(ED2DStatus state,const D2D1_POINT_2F& center, float radiusX, float radiusY,
		const D2D1_GRADIENT_STOP* stops, int count);


	void SetHoverBgColor(const D2D1_COLOR_F& color);
	void SetPressedBgColor(const D2D1_COLOR_F& color);
	void SetDragBgColor(const D2D1_COLOR_F& color);

	void SetHoverBgLinear(float angle, const D2D1_GRADIENT_STOP* stops, int count);
	void SetPressedBgLinear(float angle, const D2D1_GRADIENT_STOP* stops, int count);
	void SetDragBgLinear(float angle, const D2D1_GRADIENT_STOP* stops, int count);
	
	void SetHoverBgRadial(const D2D1_POINT_2F& center, float radiusX, float radiusY,
		const D2D1_GRADIENT_STOP* stops, int count);
	void SetPressedBgRadial(const D2D1_POINT_2F& center, float radiusX, float radiusY,
		const D2D1_GRADIENT_STOP* stops, int count);
	void SetDragBgRadial(const D2D1_POINT_2F& center, float radiusX, float radiusY,
		const D2D1_GRADIENT_STOP* stops, int count);

	// 获取当前有效状态的背景
	const CD2DBackground& GetBackground() const;

	
	// ========== 边框设置 (border) ==========
	void SetBorder(float width, ED2DLineStyle style, const D2D1_COLOR_F& color);
	void SetBorderTop(float width, ED2DLineStyle style, const D2D1_COLOR_F& color);
	void SetBorderRight(float width, ED2DLineStyle style, const D2D1_COLOR_F& color);
	void SetBorderBottom(float width, ED2DLineStyle style, const D2D1_COLOR_F& color);
	void SetBorderLeft(float width, ED2DLineStyle style, const D2D1_COLOR_F& color);

	const CD2DBorder& GetBorder() const;
	const CD2DBorderEdge& GetBorderTop() const;
	const CD2DBorderEdge& GetBorderRight() const;
	const CD2DBorderEdge& GetBorderBottom() const;
	const CD2DBorderEdge& GetBorderLeft() const;



	// ========== 圆角设置 (border-radius) ==========
	void SetBorderRadius(float radius);
	void SetBorderTopLeftRadius(float radius);
	void SetBorderTopRightRadius(float radius);
	void SetBorderBottomRightRadius(float radius);
	void SetBorderBottomLeftRadius(float radius);

	const CD2DRadius& GetBorderRadius() const;
	float GetBorderTopLeftRadius() const;
	float GetBorderTopRightRadius() const;
	float GetBorderBottomRightRadius() const;
	float GetBorderBottomLeftRadius() const;

	

	// ========== 边距 (margin) ==========
	void SetMargin(float top, float right, float bottom, float left);
	void SetMarginTop(float top);
	void SetMarginRight(float right);
	void SetMarginBottom(float bottom);
	void SetMarginLeft(float left);

	const CD2DMargin& GetMargin() const;
	float GetMarginTop() const;
	float GetMarginRight() const;
	float GetMarginBottom() const;
	float GetMarginLeft() const;

	// ========== 内边距 (padding) ==========
	void SetPadding(float top, float right, float bottom, float left);
	void SetPaddingTop(float top);
	void SetPaddingRight(float right);
	void SetPaddingBottom(float bottom);
	void SetPaddingLeft(float left);

	const CD2DPadding& GetPadding() const;
	float GetPaddingTop() const;
	float GetPaddingRight() const;
	float GetPaddingBottom() const;
	float GetPaddingLeft() const;

	// 获取不同区域矩形的方法
	D2D1_RECT_F GetContentRectangle() const;    // 获取内容区域矩形
	D2D1_RECT_F GetPaddingRectangle() const;    // 获取内边距区域矩形（包含内容区域）
	D2D1_RECT_F GetBorderRectangle() const;     // 获取边框区域矩形（包含padding区域）
	D2D1_RECT_F GetMarginRectangle() const;     // 获取外边距区域矩形（包含所有内部区域）

	virtual D2D1_SIZE_F GetBoxModelSize() const; //获取 margin+border+padding的大小
	// ========== 状态设置 ==========
	void SetNormal(bool on = true);
	void SetHover(bool on = true);
	void SetPressed(bool on = true);
	void SetDisabled(bool on = true);
	void SetDragging(bool on = true);
	void SetFocused(bool on = true);

	// ========== 状态判断 ==========
	bool IsNormal() const ;
	bool IsHover() const  ;
	bool IsPressed() const ;
	bool IsDisabled() const ;
	bool IsDragging() const ;
	bool IsFocused() const ;

	// ========== 变换 (transform) ==========
	void SetTransformTranslate(ED2DStatus state, float x, float y);
	void SetTransformRotate(ED2DStatus state, float angle);
	void SetTransformScale(ED2DStatus state, float sx, float sy);
	void SetTransformSkew(ED2DStatus state, float ax, float ay);
	// 获取当前有效状态的变换矩阵
	D2D1_MATRIX_3X2_F GetTransformMatrix() const;

	// ========== 矩形设置 ==========
	void SetRectangle(const D2D1_RECT_F& rect);
	void SetRectangle(float left, float top, float right, float bottom);

	virtual D2D1_RECT_F GetRectangle() const;
	virtual D2D1_ROUNDED_RECT GetRoundedRectangle() const;
	
	// ========== 中心点操作 ==========
	void SetCenter(const D2D1_POINT_2F& center);
	void SetCenter(float x, float y);
	D2D1_POINT_2F GetCenter() const;

	// ========== 位置操作 ==========
	void SetPosition(const D2D1_POINT_2F& pos);
	void SetPosition(float x, float y);
	D2D1_POINT_2F GetPosition() const;

	// ========== 尺寸设置 ==========
	void SetSize(float width, float height);
	void SetSize(const D2D1_SIZE_F& size);
	void SetWidth(float width);
	void SetHeight(float height);

	// ========== 尺寸获取 ==========
	D2D1_SIZE_F GetSize() const;
	float GetWidth() const;
	float GetHeight() const;

	//重置大小
	void Resize(float width, float height);
	// ========== 矩形操作 ==========
	void Move(float dx, float dy);
	void MoveTo(float x, float y);
	void OffsetRect(float left, float top, float right, float bottom);

	// 检查点是否在控件内
	virtual bool IsHitTest(const D2D1_POINT_2F& point) const;
	
public:
	bool IsVisible();
	bool SetVisible(bool visble = true);

	void EnableDrag(bool enable = true);
	bool IsDragEnabled() const;

public:
	virtual bool OnMouseDownL(const D2D1_POINT_2F& point);
	virtual bool OnMouseMove(const D2D1_POINT_2F& point);
	virtual bool OnMouseUpL(const D2D1_POINT_2F& point);
	virtual bool OnMouseDownL2(const D2D1_POINT_2F& point);
	virtual bool OnMouseUpL2(const D2D1_POINT_2F& point);

	virtual bool OnMouseLeave(const D2D1_POINT_2F& point);
	virtual bool OnMouseEnter(const D2D1_POINT_2F& point);

	virtual bool OnMouseWheel(float delta);
	virtual bool OnKeyDown(DWORD keyCode);
	virtual bool OnKeyUp(DWORD keyCode);
	virtual bool OnChar(DWORD ch);
	virtual void OnDpiChanged(float dpiScaleX, float dpiScaleY);

	 // ========== 拖拽相关虚函数（供派生类重写） ==========
	virtual void OnDragBegin(const D2D1_POINT_2F& startPoint);
	virtual void OnDragMove(const D2D1_POINT_2F& delta);
	virtual void OnDragEnd(const D2D1_POINT_2F& endPoint);

protected:
	CD2DRadius		m_radius;
	CD2DBorder		m_border;
	CD2DMargin		m_margin;
	CD2DPadding		m_padding;
	CD2DBackground  m_background[ED2DStatusMax];
	CD2DTransform   m_transform[ED2DStatusMax];
	CD2DStatus		m_status;
	D2D1_RECT_F		m_rectangle;

	bool			m_isVisible; 
	bool			m_isDragEnabled;            // 是否启用拖拽功能
	D2D1_POINT_2F m_dragStartPoint;
	static const float DRAG_THRESHOLD;    // 拖拽触发阈值（像素）

	float m_dpiScaleX;        
	float m_dpiScaleY; 
};













class CD2DControlUI
{
	enum ED2DLineStyle
	{
		None,
		Solid,
		Dashed,
		Dotted,
		Double,
	};

	struct CD2DBorderEdge
	{
		float m_width;
		ED2DLineStyle m_style;
		D2D1_COLOR_F m_color;
		float m_dashLength;
		float m_gapLength;

		CD2DBorderEdge() 
			: m_width(0.0f), m_style(None), m_color(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f)), 
			m_dashLength(0.0f), m_gapLength(0.0f) {}
	};

	struct CD2DRadius
	{
		float m_topLeft;
		float m_topRight;
		float m_bottomRight;
		float m_bottomLeft;

		CD2DRadius() 
			: m_topLeft(0.0f), m_topRight(0.0f), m_bottomRight(0.0f), m_bottomLeft(0.0f) {}
	};

	struct CD2DBorder
	{
		CD2DBorderEdge m_top;
		CD2DBorderEdge m_right;
		CD2DBorderEdge m_bottom;
		CD2DBorderEdge m_left;

		CD2DBorder() {}
	};

	struct CD2DBackground
	{
		enum EType { None, Solid, LinearGradient, RadialGradient };

		struct CSolidColor { D2D1_COLOR_F color; };
		struct CGradientStop { float position; D2D1_COLOR_F color; };

		struct CLinearGradient
		{
			D2D1_POINT_2F startPoint;
			D2D1_POINT_2F endPoint;
			D2D1_GRADIENT_STOP m_stops[10];
			int m_stopCount;

			CLinearGradient() 
				: startPoint(D2D1::Point2F(0.0f, 0.0f)), endPoint(D2D1::Point2F(0.0f, 0.0f)), m_stopCount(0) {}
		};

		struct CRadialGradient
		{
			D2D1_POINT_2F center;
			D2D1_POINT_2F gradientOriginOffset;
			float radiusX;
			float radiusY;
			D2D1_GRADIENT_STOP m_stops[10];
			int m_stopCount;

			CRadialGradient() 
				: center(D2D1::Point2F(0.0f, 0.0f)), gradientOriginOffset(D2D1::Point2F(0.0f, 0.0f)),
				radiusX(0.0f), radiusY(0.0f), m_stopCount(0) {}
		};

		EType m_type;
		CSolidColor m_solid;
		CLinearGradient m_linear;
		CRadialGradient m_radial;

		CD2DBackground() : m_type(None) {}
	};

	struct CD2DTransform
	{
		float m_translateX;
		float m_translateY;

		CD2DTransform() : m_translateX(0.0f), m_translateY(0.0f) {}
	};

	// ========== 边框设置 (border) ==========
	void SetBorder(float width, ED2DLineStyle style, const D2D1_COLOR_F& color);
	void SetBorderTop(float width, ED2DLineStyle style, const D2D1_COLOR_F& color);
	void SetBorderRight(float width, ED2DLineStyle style, const D2D1_COLOR_F& color);
	void SetBorderBottom(float width, ED2DLineStyle style, const D2D1_COLOR_F& color);
	void SetBorderLeft(float width, ED2DLineStyle style, const D2D1_COLOR_F& color);

	// ========== 圆角设置 (border-radius) ==========
	void SetBorderRadius(float radius);
	void SetBorderTopLeftRadius(float radius);
	void SetBorderTopRightRadius(float radius);
	void SetBorderBottomRightRadius(float radius);
	void SetBorderBottomLeftRadius(float radius);

	CD2DRadius m_radius;
	CD2DBorder m_border;
	CD2DMargin m_margin;
	CD2DPadding m_padding;
	// ========== 背景设置 (background) ==========
	
	CD2DBackground m_background[10];
	CD2DTransform  m_transform[10];
	CD2DStatus		m_status;
	D2D1_RECT_F m_rectangle;
}