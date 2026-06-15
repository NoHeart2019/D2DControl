#pragma once


/*
用 Direct2D 实现一组常见 CSS 功能，适用于简单 UI 控件.
编码要求 ：C++, 仅使用Atl库，不准使用std库。函数名尽量符合Css的名称

CD2DBorder：边框样式（四条边，线型，圆角）

CD2DBackground：背景（纯色，线性渐变，径向渐变）

CD2DStatus：状态管理（位标志，为每个状态存储背景、边框、颜色等，并提供回退逻辑）

CD2DTransform：矩阵变换

CD2DBoxModel：沙盒模型

CD2DSizePolicy: 大小测率

CD2DControlUI：多重继承上述类，作为控件基类

CD2DRender: 渲染类。

*/


class CD2DControl

{


};


// CSS: border
class CD2DBorder
{
	enum ELineStyle
	{
		None,
		Solid,
		Dashed,
		Dotted,
		Double,
		// ... 可扩展
	};

	struct CEdge
	{
		float m_width;
		ELineStyle m_style;
		D2D1_COLOR_F m_color;
		float dashLength;   // dashed/dotted 时有效
		float gapLength;    // dashed 时有效
		bool IsVisible() const { return m_style != None && m_width > 0.0f; }
	};

public:
	//边框是否可见
	bool isBorderVisible();

public:
	// 统一设置四边（宽、样式、颜色）
	void SetBorder(float width, ELineStyle style, const D2D1_COLOR_F& color);
	// 设置圆角（四角统一）
	void SetBorderRadius(float radius);
	void SetBorderRadius(float radiusX, float radiuY);

	// 单独设置某个角的圆角
	void SetBorderTopLeftRadius(float radiusX, float radiusY);
	void SetBorderTopRightRadius(float radiusX, float radiusY);
	void SetBorderBottomRightRadius(float radiusX, float radiusY);
	void SetBorderBottomLeftRadius(float radiusX, float radiusY);

	// 单独修改某条边
	void SetBorder(float width, ELineStyle style, const D2D1_COLOR_F& color);
	void SetBorderTop(float width, ELineStyle style, const D2D1_COLOR_F& color);
	void SetBorderRight(float width, ELineStyle style, const D2D1_COLOR_F& color);
	void SetBorderBottom(float width, ELineStyle style, const D2D1_COLOR_F& color);
	void SetBorderLeft(float width, ELineStyle style, const D2D1_COLOR_F& color);
public:

	D2D1_POINT_2F m_borderTopLeftRadius;
	D2D1_POINT_2F m_borderTopRightRadius;
	D2D1_POINT_2F m_borderBottomRightRadius;
	D2D1_POINT_2F m_borderBottomLeftRadius;

	CEdge  m_borderTop;
	CEdge  m_borderRight;
	CEdge  m_borderBottom;
	CEdge  m_borderLeft;
};

class CD2DBackground
{
public:

	enum EType
	{
		None,
		Solid,
		LinearGradient,
		RadialGradient   // 可扩展
	};

	struct SolidColor
	{
		D2D1_COLOR_F color;
	};

	struct GradientStop
	{
		float position;         // 0.0 ~ 1.0
		D2D1_COLOR_F color;
	};

	struct LinearGradient
	{
		D2D1_POINT_2F startPoint;
		D2D1_POINT_2F endPoint;
		D2D1_GRADIENT_STOP		m_backgroundStops[10];
		int						m_backgroundStopsCount;
	};

	struct RadialGradient
	{
		D2D1_POINT_2F center;
		D2D1_POINT_2F gradientOriginOffset;
		float radiusX;
		float radiusY;
		D2D1_GRADIENT_STOP		m_backgroundStops[10];
		int						m_backgroundStopsCount;
	};

	void SetSolidColor(const D2D1_COLOR_F& color);

	// 线性渐变：直接设置起点、终点和停止点
	void SetLinearGradient(const D2D1_POINT_2F& start, const D2D1_POINT_2F& end, D2D1_GRADIENT_STOP stops[], int nStop);

	// 便捷版本：通过角度和矩形设置线性渐变
	void SetLinearGradient(float angle, D2D1_GRADIENT_STOP stops[], int nStop);

	// 径向渐变：完整设置几何参数和停止点
	void SetRadialGradient(const D2D1_POINT_2F& center, const D2D1_POINT_2F& originOffset,
		float radiusX, float radiusY,
		D2D1_GRADIENT_STOP stops[], int nStop);

	bool IsBackgroundVisible();

	EType					m_backgroundType;

	SolidColor m_solid;
	LinearGradient m_linear;
	RadialGradient m_radial;
};

//位掩码枚举 支持多状态同时存在
//设置normal的时，给数组全部初始化。
class CD2DStatus
{
#define CD2DStatusCount  9
	enum EFlag {
		None        = 0,                    // 0
		Normal      = 1 << 0,               // 0x0001
		Hover       = 1 << 1,               // 0x0002  鼠标悬浮
		Pressed     = 1 << 2,               // 0x0004  鼠标按下
		Disabled    = 1 << 3,               // 0x0008  禁用
		Focused     = 1 << 4,               // 0x0010  键盘焦点
		Dragging    = 1 << 5,               // 0x0020  拖拽中
		Checked     = 1 << 6,               // 0x0040  选中（Toggle）
		Active      = 1 << 7,               // 0x0080  激活（按下后未松开）
		HotTracked  = 1 << 8,               // 0x0100  热追踪（Hover + Pressed 组合）
	};

	// 检查某状态是否激活（可检查单个或组合）
	bool Is(int flag) const;
	// 设置/清除一个或多个状态
	void Set(int flag, bool on = true);

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

	void SetHotTracked(bool on = true);
	bool IsHotTracked() const;

	unsigned int   m_statusFlags;

};

class CD2DTransform
{
public:
	// 变换类型
	enum EType
	{
		none,
		Translate,   // 平移
		Rotate,      // 旋转（角度，单位：度）
		Scale,       // 缩放
		Skew         // 倾斜（X/Y 方向角度，单位：度）
	};

	// 单个变换操作
	struct TransformOp
	{
		EType type;
		float param1;   // translateX, rotateAngle, scaleX, skewX
		float param2;   // translateY, (rotate无), scaleY, skewY
	};

	void SetTranslate(float x, float y);

	TransformOp Op;

};


class CD2DSizePolicy
{
public:
	enum  Policy{
		Fixed = 0x0,        // 固定大小
		Minimum = 0x1,      // 最小大小
		Maximum = 0x2,      // 最大大小
		Preferred = 0x3,    // 首选大小 (默认)
		MinimumExpanding = 0x4, // 最小且可扩展 (已过时，但在源码中仍存在)
		Expanding = 0x5,    // 可扩展
		Ignored = 0x6       // 忽略 sizeHint
	};

	CD2DSizePolicy(Policy size)
	{
		m_policy = size;
	}

	CD2DSizePolicy() : m_policy(Fixed) {}

	// 提供接口来修改
	void SetPolicy(Policy p) { m_policy = p; }

	// 提供接口来获取
	Policy GetPolicy() const { return m_policy; }

	// 重载 == 运算符，支持直接比较对象（可选）
	bool operator==(Policy p) const { return m_policy == p; }

	Policy  m_policy;
};

class CD2DRender
{
public:
	CComPtr<ID2D1Factory> m_pFactory;
	CComPtr<IDWriteFactory> m_pDWriteFactory;
	CComPtr<ID2D1HwndRenderTarget> m_pRenderTarget;
	CComPtr<ID2D1DrawingStateBlock> m_pDrawingStateBlock;
	HWND m_hwnd;
	float m_dpiScaleX;
	float m_dpiScaleY;
};

class CD2DControlUI : public CD2DBoxModel, public CD2DStatus, public CD2DSizePolicy{
public:
	CD2DControlUI();
	virtual ~CD2DControlUI();

	void SetPosition(float x, float y);
	virtual void SetSize(float width, float height);
	void SetContentSize(float width, float height);
	void SetRectangle(const D2D1_RECT_F& rect);
	void SetCenter(float x, float y);
	//大小策略


	virtual D2D1_RECT_F GetRectangle() const;
	virtual D2D1_ROUNDED_RECT GetRoundedRectangle() const;

	D2D1_POINT_2F GetPosition() const;
	D2D1_SIZE_F GetSize() const;
	D2D1_POINT_2F GetCenter() const;
	float GetWidth() const;
	float GetHeight() const;

	void Move(float offsetX, float offsetY);
	void MoveTo(float x, float y);
	void Inflate(float dx, float dy);

	void Resize(float newWidth, float newHeight);

	// 检查点是否在矩形内
	bool IsHitTest(const D2D1_POINT_2F& point, const D2D1_RECT_F& rect) const;
	virtual bool IsHitTest(const D2D1_POINT_2F& point) const;
	bool IsHitTest(float x, float y) const;

	// 设置用户数据
	void SetUserData(void* userData);
	void* GetUserData() const;

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


protected:
	D2D1_RECT_F m_rectangle;       

	float m_dpiScaleX;         
	float m_dpiScaleY;        

	void* m_userData;						// 用户数据
};


//使用CSS盒模型来实现
class CD2DBoxModel
{
public:
	CD2DBoxModel();
	virtual ~CD2DBoxModel();


	// 盒模型相关方法
	// 外边距(Margin)相关方法
	void SetMargin(float left, float top, float right, float bottom);
	void SetMarginLeft(float margin);
	void SetMarginTop(float margin);
	void SetMarginRight(float margin);
	void SetMarginBottom(float margin);
	float GetMarginLeft() const;
	float GetMarginTop() const;
	float GetMarginRight() const;
	float GetMarginBottom() const;

	// 内边距(Padding)相关方法
	void SetPadding(float left, float top, float right, float bottom);
	void SetPadding(float padding);
	void SetPadding(float x, float y);
	void SetPaddingLeft(float padding);
	void SetPaddingTop(float padding);
	void SetPaddingRight(float padding);
	void SetPaddingBottom(float padding);
	float GetPaddingLeft() const;
	float GetPaddingTop() const;
	float GetPaddingRight() const;
	float GetPaddingBottom() const;

	// 边框(Border)相关方法
	void SetBorder(float left, float top, float right, float bottom);
	void SetBorderLeft(float border);
	void SetBorderTop(float border);
	void SetBorderRight(float border);
	void SetBorderBottom(float border);
	float GetBorderLeft() const;
	float GetBorderTop() const;
	float GetBorderRight() const;
	float GetBorderBottom() const;

	// 获取不同区域矩形的方法
	D2D1_RECT_F GetContentRectangle() const;    // 获取内容区域矩形
	D2D1_RECT_F GetPaddingRectangle() const;    // 获取内边距区域矩形（包含内容区域）
	D2D1_RECT_F GetBorderRectangle() const;     // 获取边框区域矩形（包含padding区域）
	D2D1_RECT_F GetMarginRectangle() const;     // 获取外边距区域矩形（包含所有内部区域）

	virtual	D2D1_RECT_F GetRectangle() const;
	virtual D2D1_SIZE_F GetBoxModelSize() const;
	void SetBoxModel(float marginLeft, float marginTop, float marginRight, float marginBottom, 
		float borderLeft,  float borderTop,  float borderRight, float borderBottom, 
		float paddingLeft, float paddingTop, float paddingRight, float paddingBottom);


protected:
	// CSS盒模型相关成员变量
	// 外边距(Margin)
	float m_marginLeft;                 // 左边距
	float m_marginTop;                  // 上边距
	float m_marginRight;                // 右边距
	float m_marginBottom;               // 下边距

	// 内边距(Padding)
	float m_paddingLeft;                // 左内边距
	float m_paddingTop;                 // 上内边距
	float m_paddingRight;               // 右内边距
	float m_paddingBottom;              // 下内边距

	// 边框(Border)
	float m_borderLeft;                 // 左边框宽度
	float m_borderTop;                  // 上边框宽度
	float m_borderRight;                // 右边框宽度
	float m_borderBottom;               // 下边框宽度
};
