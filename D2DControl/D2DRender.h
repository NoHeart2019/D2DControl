#pragma once

#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>  
#include <atlbase.h>
#include <atlcoll.h> 
#include <atlcomcli.h>
//#include "D2DGeometryMath.h"

class CD2DTextFormat;

class CD2DRender {
public:
    CD2DRender();
    ~CD2DRender();

    // 初始化Direct2D资源，传入窗口句柄
   
	bool Initialize(HWND hwnd);

    // 单独的资源初始化函数
    bool InitializeFactories();
    bool InitializeRenderTarget(HWND hwnd);
   
	// 检查是否已初始化
	bool IsInitialized() const;

    // 开始绘制
    void BeginDraw();

    // 结束绘制
    HRESULT EndDraw();

    // 清除绘制区域，使用指定颜色
    void Clear(const D2D1_COLOR_F& color = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f));

    // 绘制填充矩形
    void FillRectangle(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color);

    // 绘制矩形边框
    void DrawRectangle(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color, float strokeWidth = 1.0f);

    // 绘制填充圆角矩形
    void FillRoundedRectangle(const D2D1_RECT_F& rect, float radiusX, float radiusY, const D2D1_COLOR_F& color);

    // 绘制圆角矩形边框
    void DrawRoundedRectangle(const D2D1_RECT_F& rect, float radiusX, float radiusY, const D2D1_COLOR_F& color, float strokeWidth = 1.0f);

	// 绘制填充圆角矩形
	void FillRoundedRectangle(const D2D1_ROUNDED_RECT& rect,  const D2D1_COLOR_F& color);
	
	// 绘制圆角矩形边框
	void DrawRoundedRectangle(const D2D1_ROUNDED_RECT& rect, const D2D1_COLOR_F& color, float strokeWidth = 1.0f);


    // 绘制填充椭圆 - 接收矩形参数的版本
    void FillEllipse(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color);
    
    // 绘制椭圆边框 - 接收矩形参数的版本
    void DrawEllipse(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color, float strokeWidth = 1.0f);
    
	void FillEllipse(const D2D1_ELLIPSE& ellipse, const ID2D1Brush* brush);

    // 绘制填充椭圆 - 接收椭圆参数的版本
    void FillEllipse(const D2D1_ELLIPSE& ellipse, const D2D1_COLOR_F& color);
    
    // 绘制椭圆边框 - 接收椭圆参数的版本
    void DrawEllipse(const D2D1_ELLIPSE& ellipse, const D2D1_COLOR_F& color, float strokeWidth = 1.0f);

	//绘制填充几何图形
	void FillGeometry(ID2D1Geometry* geometry, const D2D1_COLOR_F& color);
	void FillGeometry(ID2D1Geometry* geometry,  ID2D1Brush* brush);

	//绘制几何图形的轮廓
	void DrawGeometry(ID2D1Geometry* geometry, const D2D1_COLOR_F& color, float strokeWidth = 1.0f);

    // 绘制直线
	void DrawLine(const D2D1_POINT_2F& point1, const D2D1_POINT_2F& point2,  ID2D1Brush *brush, FLOAT strokeWidth = 1.0f,  ID2D1StrokeStyle *strokeStyle = NULL);

    void DrawLine(float x1, float y1, float x2, float y2, const D2D1_COLOR_F& color, float strokeWidth = 1.0f);

	// 绘制直线
	void DrawLine(const D2D1_POINT_2F& point1, const D2D1_POINT_2F& point2,  const D2D1_COLOR_F& color, float strokeWidth = 1.0f, bool isSolid = true);

	// 绘制多条线段（不闭合）
	void DrawLines(const D2D1_POINT_2F* points, UINT32 pointCount, const D2D1_COLOR_F& color, float strokeWidth = 1.0f);

	// 绘制多边形
	void DrawPolygon(const D2D1_POINT_2F* points, UINT32 pointCount, const D2D1_COLOR_F& color, float strokeWidth = 1.0f, bool isEndOpen = false, ID2D1StrokeStyle* strokeStyle = nullptr);

	//绘制填充多边形
	void FillPolygon(const D2D1_POINT_2F* points, UINT32 pointCount, const D2D1_COLOR_F& color, bool isEndOpen = false);

	/*
		用ID2D1GeometrySink单一几何路径绘制 
		画线条、弧线、三次贝塞尔曲线和二次贝塞尔曲线
	*/
    // 绘制二次贝塞尔曲线
    void DrawQuadraticBezier(const D2D1_POINT_2F& startPoint, const D2D1_QUADRATIC_BEZIER_SEGMENT& QuadraticBezier, const D2D1_COLOR_F& color, float strokeWidth = 1.0f);
    
    // 直接绘制直线段
    void DrawLineSegment(const D2D1_POINT_2F& startPoint, const D2D1_POINT_2F& endPoint, const D2D1_COLOR_F& color, float strokeWidth = 1.0f);
    
    // 直接绘制弧线
    void DrawArcSegment(const D2D1_POINT_2F& startPoint, const D2D1_ARC_SEGMENT& arcSegment, const D2D1_COLOR_F& color, float strokeWidth = 1.0f);
    
    // 直接绘制三次贝塞尔曲线
    void DrawCubicBezier(const D2D1_POINT_2F& startPoint, const D2D1_BEZIER_SEGMENT& bezierSegment, const D2D1_COLOR_F& color, float strokeWidth = 1.0f);

	
	/*
		ID2D1PathGeometry 路径操作

	*/
	// 创建路径几何对象
	CComPtr<ID2D1PathGeometry> CreatePathGeometry();
	// 创建贝塞尔路径.points 贝塞尔曲线点数组
	CComPtr<ID2D1PathGeometry> CreateBezierPath(const D2D1_POINT_2F* points, UINT count);

	//创建多边形几何图形
	CComPtr<ID2D1PathGeometry> CreatePolygonGeometry(const D2D1_POINT_2F points[], int count, bool isOpen = false);
	
    //创建圆角矩形 css
	CComPtr<ID2D1PathGeometry> CreateRoundedGeometry(const D2D1_RECT_F& rect, const D2D1_RECT_F& roundedRadius);

	CComPtr<ID2D1PathGeometry> CreateRoundedBorderGeometry(const D2D1_RECT_F& rect,          // 外矩形
		const D2D1_RECT_F& roundedRadius, // 外圆角半径（left=左上, top=右上, right=右下, bottom=左下）
		const D2D1_RECT_F& border);

	//顶部圆角、底部直线的矩形
	CComPtr<ID2D1PathGeometry> CreateTopRoundedGeometry(D2D1_ROUNDED_RECT& roundRect);
	
	//底部圆角、顶部直线的矩形
	CComPtr<ID2D1PathGeometry> CreateBottomRoundedGeometry(D2D1_ROUNDED_RECT& roundRect);

	//左边圆角，右边直线的矩形
	CComPtr<ID2D1PathGeometry> CreateLeftRoundedGeometry(D2D1_ROUNDED_RECT& roundRect);

	//右边圆角，左边直线的矩形
	CComPtr<ID2D1PathGeometry> CreateRightRoundedGeometry(D2D1_ROUNDED_RECT& roundRect);

	// 辅助：创建扇形几何体（用于圆角填充）
	CComPtr<ID2D1PathGeometry> CreateSectorGeometry(const D2D1_POINT_2F& center, float radius, float startAngle, float sweepAngle, bool isClockwise = true);

public:

    // 字体处理方法
    // 创建文本格式
    CComPtr<IDWriteTextFormat> CreateTextFormat(const wchar_t* fontFamilyName, float fontSize,DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_NORMAL ,DWRITE_TEXT_ALIGNMENT horizontalAlign = DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT verticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    
	CComPtr<IDWriteTextFormat> CreateTextFormat(const CD2DTextFormat& textFormat);

	CComPtr<IDWriteTextLayout> CreateTextLayout(const wchar_t* string, UINT32 stringLength, IDWriteTextFormat* textFormat, FLOAT maxWidth = FLT_MAX, FLOAT maxHeight = FLT_MAX);


	// 计算文本尺寸的方法
	D2D1_SIZE_F MeasureText(const wchar_t* text, IDWriteTextFormat* textFormat = nullptr, float maxWidth = FLT_MAX, float maxHeight = FLT_MAX);
	
	D2D1_SIZE_F MeasureTextSize(const CD2DTextFormat& textFormat, float maxWidth = FLT_MAX, float maxHeight = FLT_MAX);


	int MeasureTextLines(const wchar_t* text, IDWriteTextFormat* textFormat = nullptr, float maxWidth = FLT_MAX, float maxHeight = FLT_MAX);

	D2D1_RECT_F MeasureTextBounds(const wchar_t* text, IDWriteTextFormat* textFormat = nullptr);
	D2D1_RECT_F MeasureTextBounds(const wchar_t* text, const D2D1_RECT_F& layoutRect, IDWriteTextFormat* textFormat = nullptr);
	D2D1_RECT_F MeasureTextLayout(const wchar_t* text, float maxWidth, IDWriteTextFormat* textFormat = nullptr);


    // 绘制文本
    void DrawText(const wchar_t* text, const D2D1_RECT_F& rect, const D2D1_COLOR_F& color, IDWriteTextFormat* textFormat = nullptr);
    
	// 绘制格式化文本（支持换行）
	void DrawTextLayout(float x, float y, const wchar_t* text, const D2D1_COLOR_F& color, float maxWidth = FLT_MAX, float maxHeight  = FLT_MAX, IDWriteTextFormat* textFormat = nullptr);

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


public:
	
	// 创建虚线样式
	CComPtr<ID2D1StrokeStyle> CreateDashStyle(const float* dashes, UINT32 dashCount, float dashOffset = 0.0f);

    // 创建画刷
    CComPtr<ID2D1SolidColorBrush> CreateBrush(const D2D1_COLOR_F& color);

	// 创建线性渐变画刷
	CComPtr<ID2D1LinearGradientBrush> CreateLinearGradientBrush(
		const D2D1_POINT_2F& startPoint,
		const D2D1_POINT_2F& endPoint,
		const D2D1_GRADIENT_STOP* gradientStops,
		UINT gradientStopsCount,
		D2D1_GAMMA  gamma= D2D1_GAMMA_2_2,
		D2D1_EXTEND_MODE extendMode= D2D1_EXTEND_MODE_CLAMP);


	// 创建径向渐变画刷
	CComPtr<ID2D1RadialGradientBrush> CreateRadialGradientBrush(
		const D2D1_POINT_2F& center,
		float radiusX,
		float radiusY,
		const D2D1_POINT_2F& gradientOriginOffset,
		const D2D1_GRADIENT_STOP* gradientStops,
		UINT gradientStopsCount);
   
   CComPtr<ID2D1LinearGradientBrush> CreateLinearBrush(D2D1_RECT_F rectangle, D2D1_COLOR_F color1, D2D1_COLOR_F color2, float angle = 135.0f);

public:
	//DPI处理函数
	// 处理DPI缩放
	void UpdateDpiScale();


	// 调整渲染目标大小（窗口大小改变时调用）
	void Resize(float width, float height);

	// 获取DPI缩放因子
	float GetDpiScaleX() const { return m_dpiScaleX; }

	float GetDpiScaleY() const { return m_dpiScaleY; }

	void SetDpiScale(float dpiSacleX , float dpiScaleY){m_dpiScaleX = dpiSacleX, m_dpiScaleY = dpiScaleY;}

	// DPI坐标转换函数
	// 将逻辑坐标和物理坐标转换（考虑DPI缩放）
	float ScaleX(float x, BOOL Div = FALSE) const;
	float ScaleY(float y, BOOL Div = FALSE) const;

	D2D1_POINT_2F   ScalePointF(const D2D1_POINT_2F& point, BOOL Div = FALSE) const;
	D2D1_SIZE_F     ScaleSizeF(const D2D1_SIZE_F& size, BOOL Div = FALSE) const;
	D2D1_RECT_F     ScaleRectF(const D2D1_RECT_F& rect, BOOL Div = FALSE) const;


	// 获取D2D工厂接口（用于创建几何图形）
	ID2D1Factory* GetD2DFactory() { return m_pFactory; }

	IDWriteFactory* GetFactoryWrite(){return m_pDWriteFactory;};

	// 获取渲染目标接口（用于绘制几何图形）
	ID2D1HwndRenderTarget* GetRenderTarget() { return m_pRenderTarget; }

public:
    CComPtr<ID2D1Factory> m_pFactory;                 // D2D工厂
	CComPtr<IDWriteFactory> m_pDWriteFactory;         // DirectWrite工厂
    CComPtr<ID2D1HwndRenderTarget> m_pRenderTarget;   // 渲染目标
	CComPtr<ID2D1DrawingStateBlock> m_pDrawingStateBlock; // 绘制状态块
	
    HWND m_hwnd;                                      // 关联的窗口句柄
    float m_dpiScaleX;                                // X方向DPI缩放因子
    float m_dpiScaleY;                                // Y方向DPI缩放因

};


    