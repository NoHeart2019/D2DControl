// CD2DControlCssUI.h
#pragma once

#include <d2d1.h>
#include <d2d1helper.h>
#include <atlcoll.h>

// ==================== 基础类型（沿用原有定义） ====================

//支持百分比。0.5f就是圆角
struct CD2DRadius {
	float topLeft, topRight, bottomRight, bottomLeft; 
	CD2DRadius(): topLeft(0.0f), topRight(0.0f), bottomRight(0.0f), bottomLeft(0.0f) {}
	CD2DRadius(float uniform): topLeft(uniform), topRight(uniform), bottomRight(uniform), bottomLeft(uniform) {}
};

enum ED2DLineStyle { None, Solid, Dashed, Dotted, Double };

struct CD2DBorderEdge {
	float width;
	ED2DLineStyle style;
	D2D1_COLOR_F color;
	float dash;
	float gap;

	CD2DBorderEdge(float width_, ED2DLineStyle style_, const D2D1_COLOR_F& color_)
		: width(width_), style(style_), color(color_), dash(0.0f), gap(0.0f) {}
	
	CD2DBorderEdge(float width_, ED2DLineStyle style_, const D2D1_COLOR_F& color_, float dash_, float gap_)
		: width(width_), style(style_), color(color_), dash(dash_), gap(gap_) {}


	CD2DBorderEdge()
		: width(0.0f), style(None)
		, color(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f))
		, dash(0.0f), gap(0.0f) {}
};

struct CD2DBorder {
	CD2DBorderEdge top, right, bottom, left;
	CD2DBorder() {}
	CD2DBorder(CD2DBorderEdge uniform): left(uniform), top(uniform), right(uniform), bottom(uniform) {}
};

struct CD2DBackground {
	enum EType { None, Solid, LinearGradient, RadialGradient };
	struct CSolidColor { D2D1_COLOR_F color; };
	struct CGradientStop { float position; D2D1_COLOR_F color; };

	struct CLinearGradient {
		float degrees;
		D2D1_POINT_2F startPoint, endPoint;
		D2D1_GRADIENT_STOP stops[10];
		int stopCount;
		CLinearGradient()
			: degrees(0.0f)
			, startPoint(D2D1::Point2F(0.0f, 0.0f))
			, endPoint(D2D1::Point2F(0.0f, 0.0f))
			, stopCount(0) {}
	};

	struct CRadialGradient {
		D2D1_POINT_2F center, gradientOriginOffset;
		float radiusX, radiusY;
		D2D1_GRADIENT_STOP stops[10];
		int stopCount;
		CRadialGradient()
			: center(D2D1::Point2F(0.0f, 0.0f))
			, gradientOriginOffset(D2D1::Point2F(0.0f, 0.0f))
			, radiusX(0.0f), radiusY(0.0f), stopCount(0) {}
	};

	EType type;
	CSolidColor solid;
	CLinearGradient linear;
	CRadialGradient radial;
	CD2DBackground() : type(None) {}

	CD2DBackground(const D2D1_COLOR_F& c) {type = Solid; solid.color = c;}
};

class CD2DTransform {
public:
	enum EType { none, Translate, Rotate, Scale, Skew };
	
	// 单个变换操作
	struct TransformOp
	{
		EType type;
		float param1;   // translateX, rotateAngle, scaleX, skewX
		float param2;   // translateY, (rotate忽略), scaleY, skewY
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
	
	// 获取最终变换矩阵（按添加顺序复合）
	D2D1_MATRIX_3X2_F GetMatrix() const
	{
		D2D1_MATRIX_3X2_F result = D2D1::Matrix3x2F::Identity();
		size_t count = m_ops.GetCount();

		for (size_t i = 0; i < count; ++i)
		{
			const TransformOp& op = m_ops[i];
			D2D1_MATRIX_3X2_F mat;

			switch (op.type)
			{
			case Translate:
				mat = D2D1::Matrix3x2F::Translation(op.param1, op.param2);
				break;

			case Rotate:
				{
					// 度转弧度（不使用标准库三角函数）
					float radians = op.param1 * (3.14159265358979323846f / 180.0f);
					mat = D2D1::Matrix3x2F::Rotation(radians);
					break;
				}

			case Scale:
				mat = D2D1::Matrix3x2F::Scale(op.param1, op.param2);
				break;

			case Skew:
				mat = D2D1::Matrix3x2F::Skew(op.param1, op.param2);
				break;

			default:
				mat = D2D1::Matrix3x2F::Identity();
				break;
			}

			// 顺序相乘：先加的变换先作用于图形（符合 CSS 从左到右书写顺序）
			result = mat * result;
		}

		return result;
	}

	D2D1_RECT_F TransformRectangle(const D2D1_RECT_F& rect) const
	{
		float left   = rect.left;
		float top    = rect.top;
		float right  = rect.right;
		float bottom = rect.bottom;

		for (size_t i = 0; i < m_ops.GetCount(); ++i)
		{
			const TransformOp& op = m_ops[i];
			switch (op.type)
			{
			case Translate:
				left   += op.param1;
				right  += op.param1;
				top    += op.param2;
				bottom += op.param2;
				break;

			case Scale:
				{
					// 以矩形中心为原点缩放
					float cx = (left + right) * 0.5f;
					float cy = (top + bottom) * 0.5f;
					float hw = (right - left) * 0.5f * fabsf(op.param1);
					float hh = (bottom - top) * 0.5f * fabsf(op.param2);
					left   = cx - hw;
					right  = cx + hw;
					top    = cy - hh;
					bottom = cy + hh;
					break;
				}

			case Rotate:
			case Skew:
				{
					
				}
			}
		}
		return D2D1::RectF(left, top, right, bottom);
	}

	size_t GetOperationCount() const { return m_ops.GetCount(); }
	const CAtlArray<TransformOp>& GetOperations() const { return m_ops; }

private:
	CAtlArray<TransformOp> m_ops;
};


// ==================== 盒模型类型 ====================
struct CD2DMargin : D2D_RECT_F
{
	CD2DMargin() 
	{
		left = top = right = bottom = 0.0f;
	}

	CD2DMargin(float margin) 
	{
		left = top = right = bottom = margin;
	}

	CD2DMargin(float vertical, float horizontal) {top = bottom = vertical;left = right  = horizontal;}
};

struct CD2DPadding : D2D_RECT_F
{
	CD2DPadding() 
	{
		left = top = right = bottom = 0.0f;
	}
	CD2DPadding(float padding) {left = top = right = bottom = padding;}
	CD2DPadding(float vertical, float horizontal) {top = bottom = vertical;left = right  = horizontal;}
	CD2DPadding(float l, float t, float r, float b) {left = l; top = t;  right = r,  bottom = b;}

};

struct CD2DTextStyle
{
	enum LineHeightUnit { Multiplier, Pixel, Percent };
	CAtlString          fontFamily;
	float               fontSize;
	D2D1_COLOR_F        color;
	float lineHeight ;              // 行高值，默认为 1.0（倍数）
	LineHeightUnit lineHeightUnit ;
	DWRITE_FONT_WEIGHT  fontWeight;
	DWRITE_FONT_STYLE   fontStyle;
	DWRITE_FONT_STRETCH fontStretch;
	DWRITE_TEXT_ALIGNMENT      horizontalAlign;
	DWRITE_PARAGRAPH_ALIGNMENT verticalAlign;
	bool                wordWrap;

	CD2DTextStyle()
		: fontFamily(L"")
		, fontSize(14.0f)
		, color(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f))
		, fontWeight(DWRITE_FONT_WEIGHT_NORMAL)
		, fontStyle(DWRITE_FONT_STYLE_NORMAL)
		, fontStretch(DWRITE_FONT_STRETCH_NORMAL)
		, horizontalAlign(DWRITE_TEXT_ALIGNMENT_LEADING)
		, verticalAlign(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
		, wordWrap(false)
		, lineHeight(1.0f)
		, lineHeightUnit(Multiplier)
	{
	}
};

enum ED2DStatus {
	Normal = 0, Hover = 1, Pressed = 2, Disabled = 3,
	Focused = 4, Dragging = 5, Checked = 6, Active = 7, HotTracked = 8,
	DragOver    = 9,   Placeholder = 10, 
	ED2DStatusMax = 11
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
		HotTracked  = 1 << 8,
		DragOver    = 1 << 9  
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
	void SetDragOver(bool on) { Set(DragOver, on); }
	bool IsDragOver() const { return Is(DragOver); }
	ED2DStatus	GetEffectiveState() const          // 实现略
	{
		if (Is(Disabled))   return ED2DStatus::Disabled;
		if (Is(Dragging))   return ED2DStatus::Dragging;   
		if (Is(Pressed))    return ED2DStatus::Pressed;
		if (Is(DragOver))   return ED2DStatus::DragOver;
		if (Is(Hover))      return ED2DStatus::Hover;
		if (Is(Focused))    return ED2DStatus::Focused;
		if (Is(Active))     return ED2DStatus::Active;
		if (Is(Checked))    return ED2DStatus::Checked;
		return ED2DStatus::Normal;
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
	CD2DTextStyle text;   // 文字样式，参与层叠

	CD2DStyle()
		 {}
};

// 布局
struct CD2DAlignment
{
	enum  Horizontal {
		HLeft,
		HCenter,
		HRight
	};

	enum  Vertical {
		VTop,
		VCenter,
		VBottom
	};

	Horizontal horz ;
	Vertical  vert ;

	CD2DAlignment() : horz(HLeft), vert(VTop){};
	CD2DAlignment(Horizontal h, Vertical v) : horz(h), vert(v) {}
};

struct CD2DSizePolicy
{
	enum Mode
	{
		Fixed,      // 使用 value 像素值（value = 0 表示使用控件当前尺寸）
		Auto,       // 根据内容自动计算（内部调用 MeasureContent）
		Fill,       // 均分父容器剩余空间
		Expand,     // 按权重比例分配父容器剩余空间，value 作为权重
		Percent     // 占父容器内容尺寸的百分比，value 为 0～100
	};

	Mode  mode; 
	float value;  // Fixed:像素，Percent:百分比，Expand:权重
	float minValue ;
	float maxValue;

	CD2DSizePolicy() : mode(Auto), value(0.0f), minValue(0.0f), maxValue(FLT_MAX) {};
	CD2DSizePolicy(Mode m, float v = 0.0f) : mode(m), value(v),  minValue(0.0f), maxValue(FLT_MAX)  {}

	// 便捷工厂
	static CD2DSizePolicy FixedSize(float pixels)    { return CD2DSizePolicy(Fixed, pixels); }
	static CD2DSizePolicy AutoSize()                  { return CD2DSizePolicy(Auto); }
	static CD2DSizePolicy FillSize()                  { return CD2DSizePolicy(Fill); }
	static CD2DSizePolicy ExpandWeight(float weight)  { return CD2DSizePolicy(Expand, weight); }
	static CD2DSizePolicy PercentSize(float pct)      { return CD2DSizePolicy(Percent, pct); }
};

struct CD2DPosition
{
	enum Type
	{
		Static,     // 默认流式布局，left/top 无效
		Absolute,   // 绝对定位，相对于容器内容区左上角
		Fixed       // 固定定位
	};

	Type  type ;
	float left ;   // 用于 Relative 的水平偏移（像素）
	float top  ;    // 垂直偏移

	CD2DPosition() : type(CD2DPosition::Static), left(0.0f), top(0.0f){};
	CD2DPosition(Type t, float l = 0.0f, float t_val = 0.0f)
		: type(t), left(l), top(t_val) {}
};


bool IsBorderEdgeSet(const CD2DBorderEdge& edge);
bool IsBackgroundSet(const CD2DBackground& bg);
bool IsRadiusSet(const CD2DRadius& radius);
bool IsTextStyleSet(const CD2DTextStyle& ts);
void ApplyStyle(CD2DStyle& dest, const CD2DStyle& src);

float CalculateActualLineHeight(const CD2DTextStyle& style);

// 装饰空间 (Margin + Border + Padding)
D2D1_RECT_F GetStyleDecoration(const CD2DStyle& style);
D2D1_SIZE_F GetStyleDecorationSize(const CD2DStyle& style);
float GetStyleDecorationLeft(const CD2DStyle& style);
float GetStyleDecorationTop(const CD2DStyle& style);
float GetStyleDecorationRight(const CD2DStyle& style);
float GetStyleDecorationBottmo(const CD2DStyle& style);



class CD2DRender
{
public:
	CD2DRender();
	~CD2DRender();

	bool Initialize();
	// 内部创建单线程 D2D 工厂
	bool InitializeFactories();
	// 创建与 HWND 关联的渲染目标（使用桌面 DPI）
	bool InitializeRenderTarget(HWND hWnd);

	// 开始绘制
	void BeginDraw();

	// 结束绘制
	HRESULT EndDraw();

	// 清除绘制区域，使用指定颜色
	void Clear(const D2D1_COLOR_F& color = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f));

public:
	// 创建绘制状态块
	void CreateDrawingStateBlock();
	// 保存当前绘制状态
	void SaveDrawingState();
	// 恢复之前保存的绘制状态
	void RestoreDrawingState();

	// 添加轴对齐裁剪区域
	void PushAxisAlignedClip(const D2D1_RECT_F& clipRect, D2D1_ANTIALIAS_MODE mode = D2D1_ANTIALIAS_MODE_ALIASED);
	// 移除最近添加的裁剪区域
	void PopAxisAlignedClip();

	// 设置变换矩阵
	void SetTransform(const D2D1_MATRIX_3X2_F& transform);
	// 获取当前变换矩阵
	void GetTransform(D2D1_MATRIX_3X2_F* matrix) ;


	// 调整渲染目标大小（窗口大小改变时调用）
	void Resize(float width, float height);

	// 设置缩放因子
	void SetDpiScale(float dpiSacleX , float dpiScaleY);
	//获取
	void GetDpiScale(float* dpiSacleX , float* dpiScaleY);

	// 获取桌面缩放因子
	void GetDesktopDpiScale();

	// DPI坐标转换函数
	// 将逻辑坐标和物理坐标转换（考虑DPI缩放）
	float ScaleX(float x, BOOL Div = FALSE) const;
	float ScaleY(float y, BOOL Div = FALSE) const;

	D2D1_POINT_2F   ScalePointF(const D2D1_POINT_2F& point, BOOL Div = FALSE) const;
	D2D1_SIZE_F     ScaleSizeF(const D2D1_SIZE_F& size, BOOL Div = FALSE) const;
	D2D1_RECT_F     ScaleRectF(const D2D1_RECT_F& rect, BOOL Div = FALSE) const;


	
	CComPtr<ID2D1PathGeometry> CreateRoundRectGeometry(const D2D1_RECT_F& rect, const CD2DRadius& radius);
	CComPtr<ID2D1Brush> CreateBackgroundBrush(const CD2DBackground& bg, const D2D1_RECT_F& rect);
	CComPtr<ID2D1StrokeStyle> CreateStrokeStyle(ED2DLineStyle style) const;

	ID2D1Factory*        GetFactory()       const { return m_pFactory; }
	ID2D1RenderTarget*   GetRenderTarget()  const { return m_pCompatibleRenderTarget? m_pCompatibleRenderTarget : m_pRenderTarget; }
	float                GetDpiScaleX()     const { return m_dpiScaleX; }
	float                GetDpiScaleY()     const { return m_dpiScaleY; }
	IDWriteFactory* GetWriteFactory() const { return m_pWriteFactory; }
	void SetCompatibleRenderTarget(ID2D1RenderTarget* pRenderTarget){m_pCompatibleRenderTarget = pRenderTarget; }


	// 创建/获取 TextFormat（带缓存）
	CComPtr<IDWriteTextFormat> CreateTextFormat(const CD2DTextStyle& style);
	
	// 创建文本布局（自动处理格式、行高、对齐）
	CComPtr<IDWriteTextLayout> CreateTextLayout(const CAtlString& text, const CD2DTextStyle& style,
		float maxWidth = FLT_MAX, float maxHeight = FLT_MAX) const;

	// 测量文本尺寸（自动创建格式）
	D2D1_SIZE_F MeasureText(const CAtlString& text, const CD2DTextStyle& textStyle, float maxWidth = FLT_MAX, float maxHeight = FLT_MAX) const;

	D2D1_SIZE_F MeasureString(const CAtlString& text, IDWriteTextFormat* pFormat, float maxWidth = FLT_MAX, float maxHeight = FLT_MAX) const;

	// 在指定矩形内绘制文本支持滚动偏移
	void DrawText(const CAtlString& text, const CD2DTextStyle& textStyle, const D2D1_RECT_F& rect, 
		float scrollOffsetY = 0.0f);


public:
	// 创建多边形/折线几何体
	// points    : 顶点数组
	// count     : 顶点数量
	// close     : true 生成闭合多边形，false 生成开放折线
	// filled    : true 表示该图形用于填充（影响 BeginFigure 的参数）
	CComPtr<ID2D1PathGeometry> CreatePolygonGeometry(const D2D1_POINT_2F* points, UINT32 count, 
		bool close = true, bool filled = false) const;

	// 创建开放折线几何体（不闭合、不填充），用于 DrawGeometry 等
	CComPtr<ID2D1PathGeometry> CreatePolylineGeometry(const D2D1_POINT_2F* points, UINT32 count) const;

public:
	CComPtr<ID2D1Factory>      m_pFactory;
	CComPtr<ID2D1HwndRenderTarget> m_pRenderTarget;
	ID2D1RenderTarget*         m_pCompatibleRenderTarget;
	CComPtr<ID2D1DrawingStateBlock> m_pDrawingStateBlock;
	CComPtr<IDWriteFactory> m_pWriteFactory; 
	// 简单的缓存：key 由字体属性拼接而成
	CAtlMap<CAtlString, CComPtr<IDWriteTextFormat>> m_textFormatCache;
	float					   m_dpiScaleX;
	float					   m_dpiScaleY;
};

class CD2DControlUI;

struct CD2DEvent
{
	enum EType {
		Click,
		DoubleClick,
		HoverEnter,
		HoverLeave,
		DragBegin,
		DragMove,
		DragEnd,
		FocusIn,
		FocusOut,
		Checked
	};

	typedef void (WINAPI* EventCallback)(CD2DControlUI* pControl, void* pUserData);

	CD2DEvent(): m_pfnCallback(nullptr), m_pUserData(nullptr) {};
	CD2DEvent(EventCallback fn, void* data = nullptr)
		: m_pfnCallback(fn), m_pUserData(data) {}

	void operator()(CD2DControlUI* pControl) const {
		if (m_pfnCallback)
			m_pfnCallback(pControl, m_pUserData);
	}

	void* GetUserData() const { return m_pUserData; }  
	void  SetEvent(EventCallback fn, void* data = nullptr) {
		m_pfnCallback = fn;
		m_pUserData = data;
	}

	bool IsValid() const { return m_pfnCallback != nullptr; }

	EventCallback m_pfnCallback ;
	void*         m_pUserData  ;
};

//1. 基类 CD2DControlBaseUI（控件基础框架）
//包含位置、可见性、Z‑Order、拖拽支持、交互状态、鼠标/键盘事件虚函数、拖拽虚函数、事件回调系统，以及纯虚绘制接口。
//2. 子类 CD2DControlUI（CSS 样式控件）
//继承CD2DControlBaseUI，添加完整的 CSS 样式系统、盒模型计算和绘制实现。

class CD2DControlBaseUI
{
public:
	// 常用 Z‑Order 层级
	enum ZOrder {
		Z_BACKGROUND = 0,    // 背景层（通常放背景图片、水印等）
		Z_CONTENT    = 100,  // 默认内容层
		Z_OVERLAY    = 200,  // 覆盖层（如 hover 提示、遮罩）
		Z_POPUP      = 300,  // 弹出层（对话框、下拉菜单）
		Z_TOPMOST    = 400   // 最顶层（拖拽中的控件、通知）
	};

public:
	CD2DControlBaseUI();
	virtual ~CD2DControlBaseUI();

	// 布局矩形
	void SetRectangle(const D2D1_RECT_F& rect) ;
	const D2D1_RECT_F& GetRectangle() const { return m_rectangle; }

	// 位置（左上角）
	D2D1_POINT_2F GetPosition() const;
	void SetPosition(float x, float y);

	// 设置/获取定位信息
	void SetPositionType(CD2DPosition::Type type) { m_posInfo.type = type; }
	CD2DPosition::Type GetPositionType() const { return m_posInfo.type; }
	void SetAbsoluteOffset(float left, float top);
	const CD2DPosition& GetPositionInfo() const { return m_posInfo; }
	
	// 中心点
	D2D1_POINT_2F GetCenter() const;
	void SetCenter(float cx, float cy);

	// 宽度
	float GetWidth() const;
	void SetWidth(float width);

	// 高度
	float GetHeight() const;
	void SetHeight(float height);

	// 尺寸（宽度和高度）
	D2D1_SIZE_F GetSize() const;
	void SetSize(float width, float height);

	// 尺寸模式
public:
	// 新增尺寸策略
	void SetWidthPolicy(const CD2DSizePolicy& policy)  { m_widthPolicy = policy; }
	const CD2DSizePolicy& GetWidthPolicy() const       { return m_widthPolicy; }

	void SetHeightPolicy(const CD2DSizePolicy& policy) { m_heightPolicy = policy; }
	const CD2DSizePolicy& GetHeightPolicy() const      { return m_heightPolicy; }

	// 便捷判断（可内联）
	bool IsWidthAuto() const   { return m_widthPolicy.mode == CD2DSizePolicy::Auto; }
	bool IsHeightAuto() const  { return m_heightPolicy.mode == CD2DSizePolicy::Auto; }
	bool IsAutoSize() const    { return m_heightPolicy.mode == CD2DSizePolicy::Auto ||  m_heightPolicy.mode == CD2DSizePolicy::Auto; }

	// 获取纯内容尺寸（带缓存）
	D2D1_SIZE_F GetContentSize(CD2DRender* pRender, float maxWidth = FLT_MAX, float maxHeight = FLT_MAX);

	// 标记内容脏，下次测量重新计算
	void InvalidateContent();

	// 计算期望控件总尺寸（虚函数，子类可加盒模型）
	virtual D2D1_SIZE_F CalcDesiredSize(CD2DRender* pRender, float maxWidth = FLT_MAX, float maxHeight = FLT_MAX);

	// 获取期望尺寸（根据模式返回固定尺寸或计算尺寸，不修改状态）
	D2D1_SIZE_F GetDesiredSize(CD2DRender* pRender, float maxWidth = FLT_MAX, float maxHeight = FLT_MAX)  ;

	// 测量内容所需尺寸
	virtual D2D1_SIZE_F MeasureContent(CD2DRender* pRender, float maxWidth = FLT_MAX, float maxHeight = FLT_MAX) const;

	// 根据内容自动调整控件大小
	void SizeToContent(CD2DRender* pRender, float maxWidth = FLT_MAX, float maxValueHeight = FLT_MAX);

	
	// 可见性
	bool IsVisible() const { return m_isVisible; }
	void SetVisible(bool visible = true) { m_isVisible = visible; }

	// Z‑Order
	void SetZOrder(int z) { m_zOrder = z; }
	int  GetZOrder() const { return m_zOrder; }
	void BringToFront(int currentMaxZ) { m_zOrder = currentMaxZ + 1; }

	// 拖拽开关
	void EnableDrag(bool enable = true) { m_isDragEnabled = enable; }
	bool IsDragEnabled() const { return m_isDragEnabled; }

	// 命中测试
	virtual bool IsHitTest(const D2D1_POINT_2F& point) const;

	// 矩形操作
	void Move(float dx, float dy);
	void MoveTo(float x, float y);
	void OffsetRect(float left, float top, float right, float bottom);

	// 状态设置/查询
	void SetNormal(bool on = true)   { m_status.SetNormal(on); }
	void SetHover(bool on = true)    { m_status.SetHover(on); }
	void SetPressed(bool on = true)  { m_status.SetPressed(on); }
	void SetDisabled(bool on = true) { m_status.SetDisabled(on); }
	void SetDragging(bool on = true) { m_status.SetDragging(on); }
	void SetChecked(bool on = true)  { m_status.SetChecked(on); }
	void SetFocused(bool on = true)  { m_status.SetFocused(on); }

	bool IsNormal() const   { return m_status.IsNormal(); }
	bool IsHover() const    { return m_status.IsHover(); }
	bool IsPressed() const  { return m_status.IsPressed(); }
	bool IsDisabled() const { return m_status.IsDisabled(); }
	bool IsDragging() const { return m_status.IsDragging(); }
	bool IsChecked() const  { return m_status.IsChecked(); }
	bool IsFocused() const  { return m_status.IsFocused(); }

	// 鼠标/键盘事件（虚函数，可重写）
	virtual bool OnMouseDownL(const D2D1_POINT_2F& point);
	virtual bool OnMouseMove(const D2D1_POINT_2F& point);
	virtual bool OnMouseUpL(const D2D1_POINT_2F& point);
	virtual bool OnMouseDownL2(const D2D1_POINT_2F& point);
	virtual bool OnMouseUpL2(const D2D1_POINT_2F& point);
	virtual bool OnMouseLeave(const D2D1_POINT_2F& point);
	virtual bool OnMouseEnter(const D2D1_POINT_2F& point);
	virtual bool OnMouseWheel(float delta, const D2D1_POINT_2F& point);
	virtual bool OnKeyDown(DWORD keyCode);
	virtual bool OnKeyUp(DWORD keyCode);
	virtual bool OnChar(DWORD ch);
	virtual bool OnDpiChanged(float dpiScaleX, float dpiScaleY);

	// 拖拽虚函数（提供默认实现）
	virtual void OnDragBegin(const D2D1_POINT_2F& startPoint);
	virtual void OnDragMove(const D2D1_POINT_2F& delta);
	virtual void OnDragEnd(const D2D1_POINT_2F& endPoint);

	// 事件回调管理
	void SetEventCallback(CD2DEvent::EType type, CD2DEvent event);
	void RemoveEventCallback(CD2DEvent::EType type);

	// 纯虚绘制接口（子类必须实现）
	virtual void DrawControl(CD2DRender* pRender) = 0;

	void* GetUserData() const { return m_userData; }  
	void SetUserData(void * userData){m_userData = userData;}

protected:
	virtual void TriggerEvent(CD2DEvent::EType type);

	D2D1_RECT_F    m_rectangle;      // 内容矩形
	
	D2D1_SIZE_F	   m_contentSize ;   // 内容缓存
	bool		   m_contentDirty ;  // 脏标志 
	bool           m_isVisible ;
	int            m_zOrder ;
	bool           m_isDragEnabled ;
	D2D1_POINT_2F  m_dragStartPoint;
	CD2DStatus     m_status;                      // 交互状态
	static const float DRAG_THRESHOLD;            // 拖拽阈值
	CD2DPosition m_posInfo;  // 定位信息（Static / Absolute / Fixed）
	CD2DSizePolicy m_widthPolicy;   // 默认 Auto
	CD2DSizePolicy m_heightPolicy;  // 默认 Auto;    
	CAtlMap<CD2DEvent::EType, CD2DEvent> m_eventSlots;
	
	void*         m_userData  ;
};

class CD2DControlUI : public CD2DControlBaseUI
{
public:
	CD2DControlUI();
	virtual ~CD2DControlUI();

	// 样式管理
	void SetStyle(ED2DStatus state, const CD2DStyle& style);
	const CD2DStyle& GetStyle(ED2DStatus state) const;
	bool HasStyle(ED2DStatus state) const;
	void ClearAllStyle();

	void SetDefaultStyle(const CD2DStyle& style);
	const CD2DStyle& GetDefaultStyle() const;

	CD2DStyle GetStatusStyle(ED2DStatus status) const;

	// 状态标志
	CD2DStatus& GetStatus() { return m_status; }
	const CD2DStatus& GetStatus() const { return m_status; }
	
	// 最终样式计算
	CD2DStyle CalculateEffectiveStyle() const;

	// 获取指定状态下的有效样式（综合控件自身样式与状态样式）
	CD2DRadius GetEffectiveRadius(const CD2DRadius& radius, const D2D1_RECT_F& borderRect) const;

	// 盒模型区域
	D2D1_RECT_F GetMarginRectangle() const;
	D2D1_RECT_F GetBorderRectangle() const;
	D2D1_RECT_F GetPaddingRectangle() const;
	D2D1_RECT_F GetContentRectangle() const;
	// 获取非内容区域尺寸（水平：左右 margin+border+padding，垂直：上下 margin+border+padding）
	D2D1_SIZE_F GetNonContentSize() const;
	// 根据样式中的边框宽度，返回边框线条中心线所在的矩形（内缩半个线宽）
	D2D1_RECT_F GetInnerBorderRect(const CD2DStyle& style, const D2D1_RECT_F& borderRect) const;

	// 绘制实现（覆盖基类纯虚函数）
	virtual void DrawControl(CD2DRender* pRender) override;
	
	virtual CD2DControlUI* Clone() const ;
protected:
	void DrawBackground(const CD2DStyle& style, const D2D1_RECT_F& borderRect, CD2DRender* pRender);
	void DrawBorder(const CD2DStyle& style, const D2D1_RECT_F& borderRect, CD2DRender* pRender);
	void DrawBorderEdge(const CD2DBorderEdge& edge,
		D2D1_POINT_2F start, D2D1_POINT_2F end,
		CD2DRender* pRender);

	virtual void TriggerEvent(CD2DEvent::EType type);
public:
	virtual bool IsHitTest(const D2D1_POINT_2F& point) const;
	
public:
	void SetText(const CAtlString& text);
	const CAtlString& GetText() const { return m_text; }

	// 重写内容测量（根据文本计算纯尺寸）
	virtual D2D1_SIZE_F MeasureContent(CD2DRender* pRender, float maxWidth = FLT_MAX, float maxHeight = FLT_MAX) const override;

	// 单独测量文本尺寸（公开，可供外部使用）
	D2D1_SIZE_F MeasureText(CD2DRender* pRender, float maxWidth = FLT_MAX, float maxHeight = FLT_MAX) const;

	
	
protected:
	void DrawText(const CD2DStyle& style, const D2D1_RECT_F& contentRect, CD2DRender* pRender);
	
protected:
	CAtlMap<ED2DStatus, CD2DStyle> m_styles;    // 各状态样式
	CD2DStyle m_defaultStyle;                   // 全局默认样式
	CComPtr<ID2D1Layer> m_spLayer;              // 用于变换裁剪（按需创建）
	CAtlString m_text;

	int  m_debugInt;
};