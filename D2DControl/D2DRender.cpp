#include "stdafx.h"
#include "D2DRender.h"
#include "D2DText.h"
// 添加DirectWrite库链接
#pragma comment(lib, "dwrite.lib")

CD2DRender::CD2DRender()
    : m_hwnd(nullptr), m_dpiScaleX(1.0f), m_dpiScaleY(1.0f)
{
	InitializeFactories();
}

CD2DRender::~CD2DRender()
{
    // COM对象会通过CComPtr自动释放
}

// 主要初始化函数 - 作为统一入口
bool CD2DRender::Initialize(HWND hwnd)
{
    if (!hwnd)
        return false;

    m_hwnd = hwnd;

    // 按顺序初始化各部分资源
    
    if (!InitializeRenderTarget(hwnd))
        return false;

    return true;
}

// 初始化D2D和DirectWrite工厂
bool CD2DRender::InitializeFactories()
{
    // 如果工厂已存在，先释放
   
    m_pFactory.Release();
    m_pDWriteFactory.Release();

    // 创建D2D工厂
    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pFactory);
    if (FAILED(hr))
        return false;

    // 创建DirectWrite工厂
    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
    );
    
    if (FAILED(hr))
    {
        m_pFactory.Release();
        return false;
    }

    // 更新DPI缩放因子
    UpdateDpiScale();
	CreateDrawingStateBlock();
    return true;
}

// 初始化渲染目标
bool CD2DRender::InitializeRenderTarget(HWND hwnd)
{
    if (!hwnd || !m_pFactory)
        return false;

    // 如果渲染目标已存在，先释放
    if (m_pRenderTarget)
        m_pRenderTarget.Release();

    // 获取窗口客户区大小
    RECT rc;
    GetClientRect(hwnd, &rc);

    // 创建渲染目标
    D2D1_SIZE_U size = D2D1::SizeU(
        static_cast<UINT32>(rc.right - rc.left),
        static_cast<UINT32>(rc.bottom - rc.top)
    );
    
    // 获取系统DPI
    FLOAT dpiX, dpiY;
    m_pFactory->GetDesktopDpi(&dpiX, &dpiY);

    D2D1_RENDER_TARGET_PROPERTIES renderTargetProps = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_UNKNOWN),
        dpiX,
        dpiY
    );

    D2D1_HWND_RENDER_TARGET_PROPERTIES hwndRenderTargetProps = D2D1::HwndRenderTargetProperties(
        hwnd,
        size
    );

    HRESULT hr = m_pFactory->CreateHwndRenderTarget(
        &renderTargetProps,
        &hwndRenderTargetProps,
        &m_pRenderTarget
    );

    return SUCCEEDED(hr);
}

bool CD2DRender::IsInitialized() const
{
	return m_pRenderTarget != nullptr; 
}

void CD2DRender::BeginDraw()
{
    if (m_pRenderTarget)
    {
        m_pRenderTarget->BeginDraw();
    }
}

HRESULT CD2DRender::EndDraw()
{
    if (m_pRenderTarget)
    {
        return m_pRenderTarget->EndDraw();
    }
    return S_OK;
}

void CD2DRender::Clear(const D2D1_COLOR_F& color)
{
    if (m_pRenderTarget)
    {
        m_pRenderTarget->Clear(color);
    }
}

void CD2DRender::FillRectangle(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color)
{
    if (!m_pRenderTarget)
        return;

    auto brush = CreateBrush(color);
    if (brush)
    {
        m_pRenderTarget->FillRectangle(rect, brush);
    }
}

void CD2DRender::DrawRectangle(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color, float strokeWidth)
{
    if (!m_pRenderTarget)
        return;

    auto brush = CreateBrush(color);
    if (brush)
    {
        m_pRenderTarget->DrawRectangle(rect, brush, strokeWidth);
    }
}

void CD2DRender::FillRoundedRectangle(const D2D1_RECT_F& rect, float radiusX, float radiusY, const D2D1_COLOR_F& color)
{
    if (!m_pRenderTarget)
        return;

    auto brush = CreateBrush(color);
    if (brush)
    {
        D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(rect, radiusX, radiusY);
        m_pRenderTarget->FillRoundedRectangle(roundedRect, brush);
    }
}

void CD2DRender::DrawRoundedRectangle(const D2D1_RECT_F& rect, float radiusX, float radiusY, const D2D1_COLOR_F& color, float strokeWidth)
{
    if (!m_pRenderTarget)
        return;

    auto brush = CreateBrush(color);
    if (brush)
    {
        D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(rect, radiusX, radiusY);
        m_pRenderTarget->DrawRoundedRectangle(roundedRect, brush, strokeWidth);
    }
}

void CD2DRender::FillRoundedRectangle(const D2D1_ROUNDED_RECT& rect, const D2D1_COLOR_F& color)
{
	if (!m_pRenderTarget)
		return;

	auto brush = CreateBrush(color);
	if (brush)
	{
		m_pRenderTarget->FillRoundedRectangle(rect, brush);
	}
}

void CD2DRender::DrawRoundedRectangle(const D2D1_ROUNDED_RECT& roundedRect, const D2D1_COLOR_F& color, float strokeWidth)
{
	if (!m_pRenderTarget)
		return;

	auto brush = CreateBrush(color);
	if (brush)
	{
		//让线条的中心正好落在像素的中心，而不是像素的边缘。
		float offsetX = strokeWidth/2.0f;
		float offsetY = strokeWidth/2.0f;
		D2D1_ROUNDED_RECT round = roundedRect;
		round.rect.left += offsetX;
		round.rect.top += offsetY;
		round.rect.right -= offsetX;
		round.rect.bottom -= offsetY;
		m_pRenderTarget->DrawRoundedRectangle(round, brush, strokeWidth);
	}

}


void CD2DRender::FillEllipse(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color)
{
    if (!m_pRenderTarget)
        return;

    auto brush = CreateBrush(color);
    if (brush)
    {
        // 椭圆的中心和半径由矩形确定
        D2D1_ELLIPSE ellipse = D2D1::Ellipse(
            D2D1::Point2F((rect.left + rect.right) / 2, (rect.top + rect.bottom) / 2),
            (rect.right - rect.left) / 2,
            (rect.bottom - rect.top) / 2
        );
        m_pRenderTarget->FillEllipse(ellipse, brush);
    }
}

void CD2DRender::DrawEllipse(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color, float strokeWidth)
{
    if (!m_pRenderTarget)
        return;

    auto brush = CreateBrush(color);
    if (brush)
    {
        D2D1_ELLIPSE ellipse = D2D1::Ellipse(
            D2D1::Point2F((rect.left + rect.right) / 2, (rect.top + rect.bottom) / 2),
            (rect.right - rect.left) / 2,
            (rect.bottom - rect.top) / 2
        );
        m_pRenderTarget->DrawEllipse(ellipse, brush, strokeWidth);
    }
}

// 新实现的函数：直接接收D2D1_ELLIPSE参数
void CD2DRender::FillEllipse(const D2D1_ELLIPSE& ellipse, const ID2D1Brush* brush)
{
	if (!m_pRenderTarget || !brush)
		return;

	m_pRenderTarget->FillEllipse(ellipse, ( ID2D1Brush*)brush);	
}


void CD2DRender::FillEllipse(const D2D1_ELLIPSE& ellipse, const D2D1_COLOR_F& color)
{
    if (!m_pRenderTarget)
        return;

    auto brush = CreateBrush(color);
    if (brush)
    {
        m_pRenderTarget->FillEllipse(ellipse, brush);
    }
}

void CD2DRender::DrawEllipse(const D2D1_ELLIPSE& ellipse, const D2D1_COLOR_F& color, float strokeWidth)
{
    if (!m_pRenderTarget)
        return;

    auto brush = CreateBrush(color);
    if (brush)
    {
        m_pRenderTarget->DrawEllipse(ellipse, brush, strokeWidth);
    }
}

void CD2DRender::DrawGeometry(ID2D1Geometry* geometry, const D2D1_COLOR_F& color, float strokeWidth )
{
	if (!m_pRenderTarget || !geometry)
		return;

	auto brush = CreateBrush(color);
	if (brush)
	{
		m_pRenderTarget->DrawGeometry(geometry, brush, strokeWidth);
	}

}

void CD2DRender::FillGeometry(ID2D1Geometry* geometry, const D2D1_COLOR_F& color)
{
	if (!m_pRenderTarget)
		return;

	auto brush = CreateBrush(color);
	if (brush)
	{
		m_pRenderTarget->FillGeometry(geometry, brush);
	}
}

void CD2DRender::FillGeometry(ID2D1Geometry* geometry, ID2D1Brush* brush)
{
	if (!m_pRenderTarget || !brush)
		return;

	m_pRenderTarget->FillGeometry(geometry, brush);
}

void CD2DRender::DrawLine(const D2D1_POINT_2F& point1, const D2D1_POINT_2F& point2,  ID2D1Brush *brush, FLOAT strokeWidth ,  ID2D1StrokeStyle *strokeStyle )
{
	if (!m_pRenderTarget || !brush)
		return;

	m_pRenderTarget->DrawLine(
			point1,
			point2,
			brush,
			strokeWidth,
			strokeStyle
			);
}

void CD2DRender::DrawLine(float x1, float y1, float x2, float y2, const D2D1_COLOR_F& color, float strokeWidth)
{
    if (!m_pRenderTarget)
        return;

    auto brush = CreateBrush(color);
    if (brush)
    {
        m_pRenderTarget->DrawLine(
            D2D1::Point2F(x1, y1),
            D2D1::Point2F(x2, y2),
            brush,
            strokeWidth
        );
    }
}

void CD2DRender::DrawLine(const D2D1_POINT_2F& pt1, const D2D1_POINT_2F& pt2,  const D2D1_COLOR_F& color, float strokeWidth, bool isSolid)
{
	if (!m_pRenderTarget)
		return;
	// 虚线数组：[画, 空, 画, 空, ...]
	// 单位是像素（相对于用户空间，不受线条粗细影响，但受变换矩阵影响）
	const FLOAT dashPattern[] = { 3.0f, 3.0f, 3.0f, 3.0f };
	UINT32 dashCount = ARRAYSIZE(dashPattern);
	CComPtr<ID2D1StrokeStyle> stroke = CreateDashStyle(dashPattern, dashCount);

	auto brush = CreateBrush(color);
	if (brush)
	{
		m_pRenderTarget->DrawLine(
			pt1,
			pt2,
			brush,
			strokeWidth,
			isSolid?nullptr:stroke);
	}
}

// 绘制多条线段（不闭合）
void CD2DRender::DrawLines(const D2D1_POINT_2F* points, UINT32 pointCount, const D2D1_COLOR_F& color, float strokeWidth)
{
	if (!m_pRenderTarget || !points || pointCount < 2)
		return;

	CComPtr<ID2D1PathGeometry> pathGeometry;
	HRESULT hr = m_pFactory->CreatePathGeometry(&pathGeometry);
	if (FAILED(hr))
		return;

	CComPtr<ID2D1GeometrySink> sink;
	hr = pathGeometry->Open(&sink);
	if (FAILED(hr))
		return;

	sink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_HOLLOW);
	for (UINT32 i = 1; i < pointCount; i++)
	{
		sink->AddLine(points[i]);
	}
	sink->EndFigure(D2D1_FIGURE_END_OPEN);
	sink->Close();

	CComPtr<ID2D1SolidColorBrush> brush;
	hr = m_pRenderTarget->CreateSolidColorBrush(color, &brush);
	if (FAILED(hr))
		return;

	m_pRenderTarget->DrawGeometry(pathGeometry, brush, strokeWidth, nullptr);
}

void CD2DRender::DrawPolygon(const D2D1_POINT_2F* points, UINT32 pointCount, const D2D1_COLOR_F& color, float strokeWidth, bool isEndOpen, ID2D1StrokeStyle* strokeStyle)
{
	if (!m_pRenderTarget)
		return;

	auto brush = CreateBrush(color);
	if (brush)
	{
		CComPtr<ID2D1PathGeometry> pGeometry = CreatePolygonGeometry(points, pointCount, isEndOpen);
		if (pGeometry)
		{
			m_pRenderTarget->DrawGeometry(pGeometry, brush, strokeWidth, strokeStyle);
		}
	}
}

void CD2DRender::FillPolygon(const D2D1_POINT_2F* points, UINT32 pointCount, const D2D1_COLOR_F& color, bool isEndOpen)
{
	if (!m_pRenderTarget)
		return;

	auto brush = CreateBrush(color);
	if (brush)
	{
		CComPtr<ID2D1PathGeometry> pGeometry = CreatePolygonGeometry(points, pointCount, isEndOpen);
		if (pGeometry)
		{
			m_pRenderTarget->FillGeometry(pGeometry, brush);
		}
	}
}

// 创建路径几何对象
CComPtr<ID2D1PathGeometry> CD2DRender::CreatePathGeometry()
{
	CComPtr<ID2D1PathGeometry> pathGeometry;
	m_pFactory->CreatePathGeometry(&pathGeometry);
	return pathGeometry;
}

// 创建贝塞尔路径.
CComPtr<ID2D1PathGeometry> CD2DRender::CreateBezierPath(const D2D1_POINT_2F* points, UINT count)
{
	if (!points || count < 4)
		return nullptr;

	CComPtr<ID2D1PathGeometry> pathGeometry;
	m_pFactory->CreatePathGeometry(&pathGeometry);
	if (!pathGeometry)
		return nullptr;

	CComPtr<ID2D1GeometrySink> sink;
	pathGeometry->Open(&sink);
	if (!sink)
		return nullptr;

	sink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
	for (UINT i = 1; i < count - 3; i += 3)
	{
		sink->AddBezier(D2D1::BezierSegment(points[i], points[i + 1], points[i + 2]));
	}
	sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	sink->Close();

	return pathGeometry;
}

CComPtr<ID2D1PathGeometry> CD2DRender::CreatePolygonGeometry(const D2D1_POINT_2F points[], int count, bool isOpen)
{
	// 使用路径几何体创建多边形
	CComPtr<ID2D1PathGeometry> pGeometry;
	m_pFactory->CreatePathGeometry(&pGeometry);

	CComPtr<ID2D1GeometrySink> pSink;
	pGeometry->Open(&pSink);

	// 开始绘制路径
	pSink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
	for (size_t i = 1; i < count; i++)
	{
		pSink->AddLine(points[i]);
	}
	D2D1_FIGURE_END FigureEnd = isOpen? D2D1_FIGURE_END_OPEN : D2D1_FIGURE_END_CLOSED;
	pSink->EndFigure(FigureEnd);

	// 关闭几何体并提交更改
	pSink->Close();

	return pGeometry;
}

CComPtr<ID2D1PathGeometry> CD2DRender::CreateTopRoundedGeometry(D2D1_ROUNDED_RECT& roundRect)
{
	// 确保半径不超过宽度的一半
	const D2D1_RECT_F& rect = roundRect.rect;
	FLOAT radiusX = roundRect.radiusX;
	FLOAT radiusY = roundRect.radiusY;
	HRESULT hr;
	CComPtr<ID2D1PathGeometry>  pPathGeometry = CreatePathGeometry();
	if (!pPathGeometry) return pPathGeometry;

	ID2D1GeometrySink* pSink = nullptr;
	hr = pPathGeometry->Open(&pSink);
	if (FAILED(hr)) {
		pPathGeometry.Release();
		return pPathGeometry;
	}

	// 定义关键点
	FLOAT left = rect.left;
	FLOAT top = rect.top;
	FLOAT right = rect.right;
	FLOAT bottom = rect.bottom;
	FLOAT width = rect.right - rect.left;

	// 1. 起点：左下角
	pSink->BeginFigure(D2D1::Point2F(left, bottom), D2D1_FIGURE_BEGIN_FILLED);

	// 2. 左侧直线：从左下角 到 左上角圆角起始点
	pSink->AddLine(D2D1::Point2F(left, top + radiusY));

	// 3. 左上角圆弧
	// 参数: 圆弧终点, 圆弧大小(半径), 旋转角度, 扫描方向, 圆弧大小类型
	D2D1_ARC_SEGMENT arc1 = D2D1::ArcSegment(
		D2D1::Point2F(left + radiusX, top),          // 终点
		D2D1::SizeF(radiusX, radiusY),                // 半径
		0.0f,                                       // 旋转
		D2D1_SWEEP_DIRECTION_CLOCKWISE,             // 顺时针
		D2D1_ARC_SIZE_SMALL                         // 小弧
		);
	pSink->AddArc(arc1);

	// 4. 顶部直线：从左上角圆角结束点 到 右上角圆角起始点
	pSink->AddLine(D2D1::Point2F(right - radiusX, top));

	// 5. 右上角圆弧
	D2D1_ARC_SEGMENT arc2 = D2D1::ArcSegment(
		D2D1::Point2F(right, top + radiusY),         // 终点
		D2D1::SizeF(radiusX, radiusY),                // 半径
		0.0f, 
		D2D1_SWEEP_DIRECTION_CLOCKWISE, 
		D2D1_ARC_SIZE_SMALL
		);
	pSink->AddArc(arc2);

	// 6. 右侧直线：从右上角圆角结束点 到 右下角
	pSink->AddLine(D2D1::Point2F(right, bottom));

	// 7. 底部直线：从右下角 回到 左下角 (闭合)
	pSink->AddLine(D2D1::Point2F(left, bottom));

	pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
	pSink->Close();
	pSink->Release();
	// 清理资源
	return pPathGeometry;
}

CComPtr<ID2D1PathGeometry> CD2DRender::CreateBottomRoundedGeometry(D2D1_ROUNDED_RECT& roundRect)
{
	// 确保半径不超过宽度的一半和高度
	const D2D1_RECT_F& rect = roundRect.rect;
	FLOAT radiusX = roundRect.radiusX;
	FLOAT radiusY = roundRect.radiusY;

	// 安全限制半径，防止图形变形
	FLOAT width = rect.right - rect.left;
	FLOAT height = rect.bottom - rect.top;
	
	HRESULT hr;
	// 创建路径几何体
	CComPtr<ID2D1PathGeometry> pPathGeometry = CreatePathGeometry();
	if (!pPathGeometry) return pPathGeometry;

	ID2D1GeometrySink* pSink = nullptr;
	hr = pPathGeometry->Open(&pSink);
	if (FAILED(hr)) {
		pPathGeometry.Release();
		return pPathGeometry;
	}

	// 定义关键点
	FLOAT left = rect.left;
	FLOAT top = rect.top;
	FLOAT right = rect.right;
	FLOAT bottom = rect.bottom;

	// --- 开始构建路径 (顺时针方向) ---

	// 1. 起点：左上角 (顶部是直线的起点)
	pSink->BeginFigure(D2D1::Point2F(left, top), D2D1_FIGURE_BEGIN_FILLED);

	// 2. 顶部直线：从左上角 到 右上角
	pSink->AddLine(D2D1::Point2F(right, top));

	// 3. 右侧直线：从右上角 到 右下角圆角的起始点
	pSink->AddLine(D2D1::Point2F(right, bottom - radiusY));

	// 4. 右下角圆弧
	// 终点: (right - radiusX, bottom)
	D2D1_ARC_SEGMENT arc1 = D2D1::ArcSegment(
		D2D1::Point2F(right - radiusX, bottom),      // 终点
		D2D1::SizeF(radiusX, radiusY),               // 半径
		0.0f,                                        // 旋转
		D2D1_SWEEP_DIRECTION_CLOCKWISE,              // 顺时针
		D2D1_ARC_SIZE_SMALL                          // 小弧
		);
	pSink->AddArc(arc1);

	// 5. 底部直线：从右下角圆角结束点 到 左下角圆角起始点
	pSink->AddLine(D2D1::Point2F(left + radiusX, bottom));

	// 6. 左下角圆弧
	// 终点: (left, bottom - radiusY) -> 注意：这里要连回左侧直线的底部，但为了闭合，我们实际上是要连回 (left, top) 的路径
	// 修正逻辑：左下角圆弧的终点应该是 (left, bottom - radiusY) 吗？
	// 不，顺时针画完底部直线后，我们在 (left + radiusX, bottom)。
	// 下一个点是左下角的圆弧，终点应该是 (left, bottom - radiusY)。
	D2D1_ARC_SEGMENT arc2 = D2D1::ArcSegment(
		D2D1::Point2F(left, bottom - radiusY),       // 终点
		D2D1::SizeF(radiusX, radiusY),               // 半径
		0.0f, 
		D2D1_SWEEP_DIRECTION_CLOCKWISE, 
		D2D1_ARC_SIZE_SMALL
		);
	pSink->AddArc(arc2);

	// 7. 左侧直线：从左下角圆角结束点 回到 左上角 (起点)
	pSink->AddLine(D2D1::Point2F(left, top));

	// 闭合图形
	pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
	pSink->Close();

	// pSink 由 CComPtr 或 Close() 后释放管理，pPathGeometry 由 CComPtr 自动释放
	// 显式释放 sink 以防万一 (虽然 Close 后通常引用计数会处理，但在原始指针获取时最好释放)
	if (pSink) pSink->Release();

	return pPathGeometry;
}

CComPtr<ID2D1PathGeometry> CD2DRender::CreateLeftRoundedGeometry(D2D1_ROUNDED_RECT& roundRect)
{
	const D2D1_RECT_F& rect = roundRect.rect;
	FLOAT radiusX = roundRect.radiusX;
	FLOAT radiusY = roundRect.radiusY;

	// 安全限制半径：不能超过宽度的一半或高度的一半
	FLOAT width = rect.right - rect.left;
	FLOAT height = rect.bottom - rect.top;

	// 防止半径过大导致图形自交或变形
	if (radiusX > width / 2.0f) radiusX = width / 2.0f;
	if (radiusY > height / 2.0f) radiusY = height / 2.0f;

	// 如果半径为0或负数，直接退化为普通矩形（可选优化）
	if (radiusX <= 0.0f || radiusY <= 0.0f) {
		// 这里可以调用创建普通矩形的逻辑，或者继续执行下面的逻辑（弧会变成直线）
		// 为了代码简洁，我们继续执行，Direct2D通常能处理半径为0的情况，但显式处理更安全
	}

	HRESULT hr;
	CComPtr<ID2D1PathGeometry> pPathGeometry = CreatePathGeometry();
	if (!pPathGeometry) return pPathGeometry;

	ID2D1GeometrySink* pSink = nullptr;
	hr = pPathGeometry->Open(&pSink);
	if (FAILED(hr)) {
		pPathGeometry.Release();
		return pPathGeometry;
	}

	FLOAT left = rect.left;
	FLOAT top = rect.top;
	FLOAT right = rect.right;
	FLOAT bottom = rect.bottom;

	// --- 开始构建路径 (顺时针方向) ---
	// 目标：左上圆角 -> 顶部直线 -> 右上角(直角) -> 右侧直线 -> 右下角(直角) -> 底部直线 -> 左下圆角 -> 闭合

	// 1. 起点：左上角圆角的起始点 (位于顶部边上)
	// 坐标：(left + radiusX, top)
	pSink->BeginFigure(D2D1::Point2F(left + radiusX, top), D2D1_FIGURE_BEGIN_FILLED);

	// 2. 顶部直线：从左上圆角结束点 到 右上角
	pSink->AddLine(D2D1::Point2F(right, top));

	// 3. 右侧直线：从右上角 到 右下角 (右侧全是直角，直接一条线到底)
	pSink->AddLine(D2D1::Point2F(right, bottom));

	// 4. 底部直线：从右下角 到 左下角圆角的起始点
	// 坐标：(left + radiusX, bottom)
	pSink->AddLine(D2D1::Point2F(left + radiusX, bottom));

	// 5. 左下角圆弧
	// 从底部边 (left + radiusX, bottom) 画弧到 左侧边 (left, bottom - radiusY)
	D2D1_ARC_SEGMENT arcBottomLeft = D2D1::ArcSegment(
		D2D1::Point2F(left, bottom - radiusY),       // 终点
		D2D1::SizeF(radiusX, radiusY),               // 半径
		0.0f,                                        // 旋转角度
		D2D1_SWEEP_DIRECTION_CLOCKWISE,              // 顺时针
		D2D1_ARC_SIZE_SMALL                          // 小弧
		);
	pSink->AddArc(arcBottomLeft);

	// 6. 左侧直线：从左下圆角结束点 到 左上圆角起始点
	// 坐标：从 (left, bottom - radiusY) 到 (left, top + radiusY)
	pSink->AddLine(D2D1::Point2F(left, top + radiusY));

	// 7. 左上角圆弧
	// 从左侧边 (left, top + radiusY) 画弧回 起点 (left + radiusX, top)
	D2D1_ARC_SEGMENT arcTopLeft = D2D1::ArcSegment(
		D2D1::Point2F(left + radiusX, top),          // 终点 (即起点)
		D2D1::SizeF(radiusX, radiusY),
		0.0f,
		D2D1_SWEEP_DIRECTION_CLOCKWISE,
		D2D1_ARC_SIZE_SMALL
		);
	pSink->AddArc(arcTopLeft);

	// 闭合图形
	pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
	pSink->Close();

	if (pSink) pSink->Release();

	return pPathGeometry;
}

CComPtr<ID2D1PathGeometry> CD2DRender::CreateRightRoundedGeometry(D2D1_ROUNDED_RECT& roundRect)
{
	const D2D1_RECT_F& rect = roundRect.rect;
	FLOAT radiusX = roundRect.radiusX;
	FLOAT radiusY = roundRect.radiusY;

	// 安全限制半径
	FLOAT width = rect.right - rect.left;
	FLOAT height = rect.bottom - rect.top;

	// 防止半径过大导致图形自交
	if (radiusX > width / 2.0f) radiusX = width / 2.0f;
	if (radiusY > height / 2.0f) radiusY = height / 2.0f;

	HRESULT hr;
	CComPtr<ID2D1PathGeometry> pPathGeometry = CreatePathGeometry();
	if (!pPathGeometry) return pPathGeometry;

	ID2D1GeometrySink* pSink = nullptr;
	hr = pPathGeometry->Open(&pSink);
	if (FAILED(hr)) {
		pPathGeometry.Release();
		return pPathGeometry;
	}

	FLOAT left = rect.left;
	FLOAT top = rect.top;
	FLOAT right = rect.right;
	FLOAT bottom = rect.bottom;

	// --- 开始构建路径 (顺时针方向) ---
	// 目标：左上角(直角) -> 右上角(圆角) -> 右下角(圆角) -> 左下角(直角) -> 闭合

	// 1. 起点：左上角 (left, top)
	// 因为左上角是直角，我们直接从顶点开始
	pSink->BeginFigure(D2D1::Point2F(left, top), D2D1_FIGURE_BEGIN_FILLED);

	// 2. 顶部直线：从左上角 到 右上角圆角的起始点
	// 坐标：(right - radiusX, top)
	pSink->AddLine(D2D1::Point2F(right - radiusX, top));

	// 3. 右上角圆弧
	// 从顶部边 (right - radiusX, top) 画弧到 右侧边 (right, top + radiusY)
	D2D1_ARC_SEGMENT arcTopRight = D2D1::ArcSegment(
		D2D1::Point2F(right, top + radiusY),         // 终点
		D2D1::SizeF(radiusX, radiusY),               // 半径
		0.0f,                                        // 旋转角度
		D2D1_SWEEP_DIRECTION_CLOCKWISE,              // 顺时针
		D2D1_ARC_SIZE_SMALL                          // 小弧
		);
	pSink->AddArc(arcTopRight);

	// 4. 右侧直线：从右上圆角结束点 到 右下圆角起始点
	// 坐标：从 (right, top + radiusY) 到 (right, bottom - radiusY)
	pSink->AddLine(D2D1::Point2F(right, bottom - radiusY));

	// 5. 右下角圆弧
	// 从右侧边 (right, bottom - radiusY) 画弧到 底部边 (right - radiusX, bottom)
	D2D1_ARC_SEGMENT arcBottomRight = D2D1::ArcSegment(
		D2D1::Point2F(right - radiusX, bottom),      // 终点
		D2D1::SizeF(radiusX, radiusY),
		0.0f,
		D2D1_SWEEP_DIRECTION_CLOCKWISE,
		D2D1_ARC_SIZE_SMALL
		);
	pSink->AddArc(arcBottomRight);

	// 6. 底部直线：从右下圆角结束点 到 左下角
	// 坐标：(left, bottom)
	pSink->AddLine(D2D1::Point2F(left, bottom));

	// 7. 左侧直线：从左下角 回到 左上角 (起点)
	// 左侧全是直角，直接一条线拉回起点
	pSink->AddLine(D2D1::Point2F(left, top));

	// 闭合图形
	pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
	pSink->Close();

	if (pSink) pSink->Release();

	return pPathGeometry;
}

CComPtr<ID2D1PathGeometry> CD2DRender::CreateRoundedGeometry(const D2D1_RECT_F& rect, const D2D1_RECT_F& roundedRadius)
{
	FLOAT left   = rect.left;
	FLOAT top    = rect.top;
	FLOAT right  = rect.right;
	FLOAT bottom = rect.bottom;

	FLOAT width  = right - left;
	FLOAT height = bottom - top;

	// 四个角的半径（left=左上, top=右上, right=右下, bottom=左下）
	FLOAT radiusTL = roundedRadius.left;   // 左上
	FLOAT radiusTR = roundedRadius.top;    // 右上
	FLOAT radiusBR = roundedRadius.right;  // 右下
	FLOAT radiusBL = roundedRadius.bottom; // 左下

	// 防止半径过大导致图形自交
	FLOAT maxRadius = min(width, height) / 2.0f;
	radiusTL = min(radiusTL, maxRadius);
	radiusTR = min(radiusTR, maxRadius);
	radiusBR = min(radiusBR, maxRadius);
	radiusBL = min(radiusBL, maxRadius);

	CComPtr<ID2D1PathGeometry> pPathGeometry = CreatePathGeometry();
	if (!pPathGeometry) return pPathGeometry;

	ID2D1GeometrySink* pSink = nullptr;
	HRESULT hr = pPathGeometry->Open(&pSink);
	if (FAILED(hr)) {
		pPathGeometry.Release();
		return pPathGeometry;
	}

	// 起点：左上角圆弧的起点（左边缘结束点）
	FLOAT startX = left;
	FLOAT startY = top + radiusTL;
	pSink->BeginFigure(D2D1::Point2F(startX, startY), D2D1_FIGURE_BEGIN_FILLED);

	// 1. 左上角圆弧
	if (radiusTL > 0.0f) {
		D2D1_ARC_SEGMENT arcTL = D2D1::ArcSegment(
			D2D1::Point2F(left + radiusTL, top),
			D2D1::SizeF(radiusTL, radiusTL),
			0.0f,
			D2D1_SWEEP_DIRECTION_CLOCKWISE,
			D2D1_ARC_SIZE_SMALL);
		pSink->AddArc(arcTL);
	}

	// 2. 顶部直线
	pSink->AddLine(D2D1::Point2F(right - radiusTR, top));

	// 3. 右上角圆弧
	if (radiusTR > 0.0f) {
		D2D1_ARC_SEGMENT arcTR = D2D1::ArcSegment(
			D2D1::Point2F(right, top + radiusTR),
			D2D1::SizeF(radiusTR, radiusTR),
			0.0f,
			D2D1_SWEEP_DIRECTION_CLOCKWISE,
			D2D1_ARC_SIZE_SMALL);
		pSink->AddArc(arcTR);
	}

	// 4. 右侧直线
	pSink->AddLine(D2D1::Point2F(right, bottom - radiusBR));

	// 5. 右下角圆弧
	if (radiusBR > 0.0f) {
		D2D1_ARC_SEGMENT arcBR = D2D1::ArcSegment(
			D2D1::Point2F(right - radiusBR, bottom),
			D2D1::SizeF(radiusBR, radiusBR),
			0.0f,
			D2D1_SWEEP_DIRECTION_CLOCKWISE,
			D2D1_ARC_SIZE_SMALL);
		pSink->AddArc(arcBR);
	}

	// 6. 底部直线
	pSink->AddLine(D2D1::Point2F(left + radiusBL, bottom));

	// 7. 左下角圆弧
	if (radiusBL > 0.0f) {
		D2D1_ARC_SEGMENT arcBL = D2D1::ArcSegment(
			D2D1::Point2F(left, bottom - radiusBL),
			D2D1::SizeF(radiusBL, radiusBL),
			0.0f,
			D2D1_SWEEP_DIRECTION_CLOCKWISE,
			D2D1_ARC_SIZE_SMALL);
		pSink->AddArc(arcBL);
	}

	// 8. 左侧直线（回到起点）
	pSink->AddLine(D2D1::Point2F(left, top + radiusTL));

	// 闭合图形
	pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
	pSink->Close();
	pSink->Release();

	return pPathGeometry;
}

CComPtr<ID2D1PathGeometry> CD2DRender::CreateRoundedBorderGeometry(
	const D2D1_RECT_F& rect,          // 外矩形
	const D2D1_RECT_F& roundedRadius, // 外圆角半径（left=左上, top=右上, right=右下, bottom=左下）
	const D2D1_RECT_F& border)        // 边框宽度（left=左边, top=上边, right=右边, bottom=下边）
{
	// 检查是否有边框（若所有边框宽度<=0，返回空）
	if (border.left <= 0.0f && border.top <= 0.0f &&
		border.right <= 0.0f && border.bottom <= 0.0f) {
			return nullptr;
	}

	// 1. 计算内矩形（向外矩形内部缩进边框宽度）
	D2D1_RECT_F innerRect = {
		rect.left   + border.left/2.0f,
		rect.top    + border.top/2.0f,
		rect.right  - border.right/2.0f,
		rect.bottom - border.bottom/2.0f
	};
	// 防止内矩形无效（宽度或高度非正）
	if (innerRect.left >= innerRect.right || innerRect.top >= innerRect.bottom) {
		return nullptr; // 边框过宽，无法形成有效内矩形
	}

	// 2. 计算内圆角半径（每个角减去相邻两个边框宽度的较大值，且不小于0）
	D2D1_RECT_F innerRadius = roundedRadius;
	
	// 3. 创建外圆角矩形几何体
	CComPtr<ID2D1PathGeometry> outerGeo = CreateRoundedGeometry(rect, roundedRadius);
	if (!outerGeo) return nullptr;

	// 4. 创建内圆角矩形几何体
	CComPtr<ID2D1PathGeometry> innerGeo = CreateRoundedGeometry(innerRect, innerRadius);
	if (!innerGeo) return nullptr;

	// 5. 组合生成边框几何体（外减内）
	CComPtr<ID2D1PathGeometry> borderGeo = CreatePathGeometry();
	if (!borderGeo) return nullptr;

	ID2D1GeometrySink* pSink = nullptr;
	HRESULT hr = borderGeo->Open(&pSink);
	if (FAILED(hr)) return nullptr;
	//pSink->SetFillMode(D2D1_FILL_MODE_WINDING);
	// 使用 EXCLUDE 组合模式，外矩形减去内矩形，得到边框区域
	hr = outerGeo->CombineWithGeometry(
		innerGeo,
		D2D1_COMBINE_MODE_EXCLUDE,
		nullptr,    // 无变换
		0.0f,    // 使用默认容差（或可传容差指针，如 0.001f 以处理浮点误差）
		pSink
		);

	pSink->Close();
	pSink->Release();

	if (FAILED(hr)) return nullptr;

	return borderGeo;
}

// 创建扇形几何体（从 startAngle 顺时针 sweepAngle 弧度）
CComPtr<ID2D1PathGeometry> CD2DRender::CreateSectorGeometry(const D2D1_POINT_2F& center, float radius, float startAngle, float sweepAngle, bool isClockwise)
{
	CComPtr<ID2D1PathGeometry> pGeo = CreatePathGeometry();
	if (!pGeo) return nullptr;

	ID2D1GeometrySink* pSink = nullptr;
	HRESULT hr = pGeo->Open(&pSink);
	if (FAILED(hr)) return nullptr;

	// 计算起始点
	float endAngle = startAngle + sweepAngle;
	D2D1_POINT_2F startPt = {
		center.x + radius * cosf(startAngle),
		center.y + radius * sinf(startAngle)
	};
	D2D1_POINT_2F endPt = {
		center.x + radius * cosf(endAngle),
		center.y + radius * sinf(endAngle)
	};

	pSink->BeginFigure(startPt, D2D1_FIGURE_BEGIN_FILLED);
	pSink->AddLine(center);
	pSink->AddArc(D2D1::ArcSegment(
		endPt,
		D2D1::SizeF(radius, radius),
		0.0f,
		isClockwise ? D2D1_SWEEP_DIRECTION_CLOCKWISE : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
		D2D1_ARC_SIZE_SMALL
		));
	pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
	pSink->Close();
	pSink->Release();

	return pGeo;
}

// 创建虚线样式
CComPtr<ID2D1StrokeStyle> CD2DRender::CreateDashStyle(const float* dashes, UINT32 dashCount, float dashOffset)
{
	if (!m_pFactory)
		return nullptr;

	CComPtr<ID2D1StrokeStyle> strokeStyle;
	D2D1_STROKE_STYLE_PROPERTIES strokeStyleProperties = D2D1::StrokeStyleProperties(
		D2D1_CAP_STYLE_FLAT,
		D2D1_CAP_STYLE_FLAT,
		D2D1_CAP_STYLE_ROUND,
		D2D1_LINE_JOIN_ROUND,
		10.0f,
		D2D1_DASH_STYLE_CUSTOM,
		dashOffset
		);

	HRESULT hr = m_pFactory->CreateStrokeStyle(strokeStyleProperties, dashes, dashCount, &strokeStyle);
	if (SUCCEEDED(hr))
	{
		return strokeStyle;
	}
	return nullptr;
}

CComPtr<ID2D1SolidColorBrush> CD2DRender::CreateBrush(const D2D1_COLOR_F& color)
{
    if (!m_pRenderTarget)
        return nullptr;

    CComPtr<ID2D1SolidColorBrush> brush;
    HRESULT hr = m_pRenderTarget->CreateSolidColorBrush(color, &brush);
    if (SUCCEEDED(hr))
    {
        return brush;
    }
    return nullptr;
}

// 创建线性渐变画刷
CComPtr<ID2D1LinearGradientBrush> CD2DRender::CreateLinearGradientBrush(
	const D2D1_POINT_2F& startPoint,
	const D2D1_POINT_2F& endPoint,
	const D2D1_GRADIENT_STOP* gradientStops,
	UINT gradientStopsCount,
	D2D1_GAMMA gamma,
	D2D1_EXTEND_MODE extendMode)
{
	if (!m_pRenderTarget || !gradientStops || gradientStopsCount == 0)
		return nullptr;

	// 创建渐变停止点集合
	CComPtr<ID2D1GradientStopCollection> gradientStopCollection;
	HRESULT hr = m_pRenderTarget->CreateGradientStopCollection(
		gradientStops,
		gradientStopsCount,
		gamma,
		extendMode,
		&gradientStopCollection
		);

	if (FAILED(hr))
		return nullptr;

	// 创建线性渐变画刷
	D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES linearGradientBrushProperties = 
		D2D1::LinearGradientBrushProperties(startPoint, endPoint);

	CComPtr<ID2D1LinearGradientBrush> linearGradientBrush;
	hr = m_pRenderTarget->CreateLinearGradientBrush(
		linearGradientBrushProperties,
		gradientStopCollection,
		&linearGradientBrush
		);

	if (SUCCEEDED(hr))
	{
		return linearGradientBrush;
	}
	return nullptr;
}

// 创建径向渐变画刷
CComPtr<ID2D1RadialGradientBrush> CD2DRender::CreateRadialGradientBrush(
	const D2D1_POINT_2F& center,
	float radiusX,
	float radiusY,
	const D2D1_POINT_2F& gradientOriginOffset,
	const D2D1_GRADIENT_STOP* gradientStops,
	UINT gradientStopsCount)
{
	if (!m_pRenderTarget || !gradientStops || gradientStopsCount == 0)
		return nullptr;

	// 创建渐变停止点集合
	CComPtr<ID2D1GradientStopCollection> gradientStopCollection;
	HRESULT hr = m_pRenderTarget->CreateGradientStopCollection(
		gradientStops,
		gradientStopsCount,
		D2D1_GAMMA_2_2,
		D2D1_EXTEND_MODE_CLAMP,
		&gradientStopCollection
		);

	if (FAILED(hr))
		return nullptr;

	// 创建径向渐变画刷
	D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES radialGradientBrushProperties = 
		D2D1::RadialGradientBrushProperties(center, gradientOriginOffset, radiusX, radiusY);

	CComPtr<ID2D1RadialGradientBrush> radialGradientBrush;
	hr = m_pRenderTarget->CreateRadialGradientBrush(
		radialGradientBrushProperties,
		gradientStopCollection,
		&radialGradientBrush
		);

	if (SUCCEEDED(hr))
	{
		return radialGradientBrush;
	}
	return nullptr;
}

CComPtr<ID2D1LinearGradientBrush> CD2DRender::CreateLinearBrush(D2D1_RECT_F rectangle, D2D1_COLOR_F color1, D2D1_COLOR_F color2, float angle) // 新增角度参数
{
    // 1. 计算矩形中心点
    float centerX = (rectangle.left + rectangle.right) / 2.0f;
    float centerY = (rectangle.top + rectangle.bottom) / 2.0f;

    // 2. 将角度转换为弧度
    // CSS 角度定义：0deg 指向右边 (3点钟方向)，顺时针旋转
    float radians = angle * 3.14159265f / 180.0f;

    // 3. 计算方向向量
    // 注意：Direct2D 的 Y 轴是向下的，但三角函数的标准定义通常假设 Y 轴向上。
    // 为了匹配 CSS 的视觉效果（135deg 是从左上到右下），我们需要调整 Y 的计算。
    float vx = cosf(radians);
    float vy = sinf(radians); 

    // 4. 确定渐变线的长度
    // 为了保证渐变完全覆盖矩形，线段长度至少要是矩形的对角线长度
    float rectWidth = rectangle.right - rectangle.left;
    float rectHeight = rectangle.bottom - rectangle.top;
    float diagonalLength = sqrtf(rectWidth * rectWidth + rectHeight * rectHeight);

    // 5. 计算起点和终点
    // 我们从中心点向两个相反方向延伸半个对角线长度
    // 这样无论角度如何，渐变都能填满整个矩形区域
    D2D1_POINT_2F startPoint = D2D1::Point2F(
        centerX - (vx * diagonalLength / 2.0f),
        centerY + (vy * diagonalLength / 2.0f) // Y轴向下，所以是 +
    );

    D2D1_POINT_2F endPoint = D2D1::Point2F(
        centerX + (vx * diagonalLength / 2.0f),
        centerY - (vy * diagonalLength / 2.0f) // 相反方向
    );

    // 6. 定义渐变停止点
    D2D1_GRADIENT_STOP gradientStops[] =
    {
        {0.0f, color1},
        {1.0f, color2}
    };

    // 7. 创建并返回画刷
    return CreateLinearGradientBrush(startPoint, endPoint, gradientStops, 2);
}

// 实现字体相关方法
CComPtr<IDWriteTextFormat> CD2DRender::CreateTextFormat(const wchar_t* fontFamilyName, float fontSize, DWRITE_FONT_WEIGHT fontWeight,DWRITE_TEXT_ALIGNMENT horizontalAlign, DWRITE_PARAGRAPH_ALIGNMENT verticalAlign)
{
    if (!m_pDWriteFactory)
        return false;
	CComPtr<IDWriteTextFormat> pTextFormat;
    HRESULT hr = m_pDWriteFactory->CreateTextFormat(
        fontFamilyName,                 // 字体名称
        nullptr,                        // 字体系列（使用默认）
        fontWeight,      // 字体粗细
        DWRITE_FONT_STYLE_NORMAL,       // 字体样式
        DWRITE_FONT_STRETCH_NORMAL,     // 字体拉伸
        fontSize * m_dpiScaleY,         // 字体大小（考虑DPI缩放）
        L"",                            // 区域设置
        &pTextFormat                  // 输出文本格式对象
    );

    if (SUCCEEDED(hr))
    {
        // 设置文本对齐方式
        pTextFormat->SetTextAlignment(horizontalAlign);
        pTextFormat->SetParagraphAlignment(verticalAlign);

		return pTextFormat;
    }

    return nullptr;
}

CComPtr<IDWriteTextFormat> CD2DRender::CreateTextFormat(const CD2DTextFormat& textFormat)
{
	if (!m_pDWriteFactory)
		return false;
	CComPtr<IDWriteTextFormat> pTextFormat;
	HRESULT hr = m_pDWriteFactory->CreateTextFormat(
		textFormat.GetFontName(),                 // 字体名称
		nullptr,                        // 字体系列（使用默认）
		textFormat.GetFontWeight(),      // 字体粗细
		textFormat.GetFontStyle(),       // 字体样式
		textFormat.GetFontStretch(),     // 字体拉伸
		textFormat.GetFontSize() * m_dpiScaleY,         // 字体大小（考虑DPI缩放）
		L"",                            // 区域设置
		&pTextFormat                  // 输出文本格式对象
		);

	if (SUCCEEDED(hr))
	{
		// 设置文本对齐方式
		pTextFormat->SetTextAlignment(textFormat.GetHorizontalAlignment());
		pTextFormat->SetParagraphAlignment(textFormat.GetVerticalAlignment());

		return pTextFormat;
	}

	return nullptr;


}

CComPtr<IDWriteTextLayout> CD2DRender::CreateTextLayout(const wchar_t* string, UINT32 stringLength, IDWriteTextFormat* textFormat, FLOAT maxWidth, FLOAT maxHeight)
{
	if (!m_pDWriteFactory || !string || !textFormat)
		return nullptr;

	// 创建文本布局对象
	CComPtr<IDWriteTextLayout> textLayout;
	HRESULT hr = m_pDWriteFactory->CreateTextLayout(
		string,
		stringLength,
		textFormat,
		maxWidth,
		maxHeight,
		&textLayout
		);

	if (SUCCEEDED(hr))
	{
		return textLayout;
	}
	return nullptr;
}

// 测量文本的大小（宽度和高度）
D2D1_SIZE_F CD2DRender::MeasureText(const wchar_t* text, IDWriteTextFormat* textFormat, float maxWidth, float maxHeight)
{
	if (!m_pDWriteFactory || !text)
		return D2D1::SizeF(0.0f, 0.0f);

	// 如果没有提供文本格式，使用默认的
	if (!textFormat)
		return D2D1::SizeF(0.0f, 0.0f);

	// 创建文本布局对象来测量文本
	CComPtr<IDWriteTextLayout> textLayout;
	HRESULT hr = m_pDWriteFactory->CreateTextLayout(
		text,
		static_cast<UINT32>(wcslen(text)),
		textFormat,
		maxWidth,  // 最大宽度（不限制）
		maxHeight,  // 最大高度（不限制）
		&textLayout
		);

	if (FAILED(hr))
		return D2D1::SizeF(0.0f, 0.0f);

	// 获取文本布局的整体尺寸
	DWRITE_TEXT_METRICS textMetrics;
	hr = textLayout->GetMetrics(&textMetrics);
	if (FAILED(hr))
		return D2D1::SizeF(0.0f, 0.0f);

	return D2D1::SizeF(max(textMetrics.widthIncludingTrailingWhitespace, textMetrics.width)+0.5f, textMetrics.height);
}

D2D1_SIZE_F CD2DRender::MeasureTextSize(const CD2DTextFormat& textFormat, float maxWidth,  float maxHeight)
{
	CComPtr<IDWriteTextFormat> format;
	CString string = textFormat.GetText();
	if (string.IsEmpty())
		return D2D1::SizeF();

	format = CreateTextFormat(
		textFormat.GetFontName(), 
		textFormat.GetFontSize(), 
		textFormat.GetFontWeight(), 
		textFormat.GetHorizontalAlignment(), 
		textFormat.GetVerticalAlignment());

	if (format)
	{
		if (textFormat.GetLineSpacing() != 0.0f)
		{
			FLOAT fontSize = textFormat.GetFontSize();
			FLOAT approximateAscent = fontSize * 0.8f; 

			float line = (textFormat.GetLineSpacing() > fontSize ?
				textFormat.GetLineSpacing() :
			fontSize * textFormat.GetLineSpacing() * GetDpiScaleY());

			FLOAT baselineOffset = (line - fontSize) / 2.0f + approximateAscent;

			format->SetLineSpacing(
				DWRITE_LINE_SPACING_METHOD_UNIFORM,
				line,
				baselineOffset
				);
		}

		float layoutMaxWidth = (maxWidth == FLT_MAX ? textFormat.GetMaxWidth() : maxWidth);
		float layoutMaxHeight = (maxHeight == FLT_MAX ? textFormat.GetMaxHeight() : maxHeight);

		return MeasureText(textFormat.GetText(), format, layoutMaxWidth, layoutMaxHeight);		

	}

	return D2D1::SizeF();
}

// 测量文本的行数（宽度和高度）
int CD2DRender::MeasureTextLines(const wchar_t* text, IDWriteTextFormat* textFormat, float maxWidth, float maxHeight)
{
	if (!m_pDWriteFactory || !text)
		return 0;

	// 如果没有提供文本格式，使用默认的
	if (!textFormat)
		return 0;

	// 创建文本布局对象来测量文本
	CComPtr<IDWriteTextLayout> textLayout;
	HRESULT hr = m_pDWriteFactory->CreateTextLayout(
		text,
		static_cast<UINT32>(wcslen(text)),
		textFormat,
		maxWidth,  // 最大宽度（不限制）
		maxHeight,  // 最大高度（不限制）
		&textLayout
		);

	if (FAILED(hr))
		return 0;

	// 获取文本布局的整体尺寸
	DWRITE_TEXT_METRICS textMetrics;
	hr = textLayout->GetMetrics(&textMetrics);
	if (FAILED(hr))
		return 0;

	return textMetrics.lineCount;
}




// 测量文本的边界矩形（左上角在原点）
D2D1_RECT_F CD2DRender::MeasureTextBounds(const wchar_t* text, IDWriteTextFormat* textFormat)
{
	if (!m_pDWriteFactory || !text)
		return D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f);

	// 如果没有提供文本格式，使用默认的
	if (!textFormat)
		return D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f);

	// 创建文本布局对象来测量文本
	CComPtr<IDWriteTextLayout> textLayout;
	HRESULT hr = m_pDWriteFactory->CreateTextLayout(
		text,
		static_cast<UINT32>(wcslen(text)),
		textFormat,
		FLT_MAX,  // 最大宽度（不限制）
		FLT_MAX,  // 最大高度（不限制）
		&textLayout
		);

	if (FAILED(hr))
		return D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f);

	// 获取文本布局的整体尺寸
	DWRITE_TEXT_METRICS textMetrics;
	hr = textLayout->GetMetrics(&textMetrics);
	if (FAILED(hr))
		return D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f);

	return D2D1::RectF(0.0f, 0.0f, textMetrics.widthIncludingTrailingWhitespace, textMetrics.height);
}

// 测量文本在指定布局矩形内的实际边界
D2D1_RECT_F CD2DRender::MeasureTextBounds(const wchar_t* text, const D2D1_RECT_F& layoutRect, IDWriteTextFormat* textFormat)
{
	if (!m_pDWriteFactory || !text)
		return D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f);

	// 如果没有提供文本格式，使用默认的

	if (!textFormat)
		return D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f);

	// 创建文本布局对象来测量文本
	CComPtr<IDWriteTextLayout> textLayout;
	HRESULT hr = m_pDWriteFactory->CreateTextLayout(
		text,
		static_cast<UINT32>(wcslen(text)),
		textFormat,
		layoutRect.right - layoutRect.left,  // 最大宽度
		FLT_MAX,                             // 最大高度（不限制）
		&textLayout
		);

	if (FAILED(hr))
		return D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f);

	// 获取文本布局的整体尺寸
	DWRITE_TEXT_METRICS textMetrics;
	hr = textLayout->GetMetrics(&textMetrics);
	if (FAILED(hr))
		return D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f);

	// 计算文本在指定布局矩形内的实际位置和大小
	// 考虑文本对齐方式
	DWRITE_TEXT_ALIGNMENT textAlignment;
	DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment;
	textAlignment = textFormat->GetTextAlignment();
	paragraphAlignment =textFormat->GetParagraphAlignment();

	float x = layoutRect.left;
	float y = layoutRect.top;
	float width = textMetrics.widthIncludingTrailingWhitespace;
	float height = textMetrics.height;

	// 根据文本对齐方式调整x坐标
	if (textAlignment == DWRITE_TEXT_ALIGNMENT_CENTER) {
		x = layoutRect.left + (layoutRect.right - layoutRect.left - width) / 2.0f;
	} else if (textAlignment == DWRITE_TEXT_ALIGNMENT_TRAILING) {
		x = layoutRect.right - width;
	}

	// 根据段落对齐方式调整y坐标
	if (paragraphAlignment == DWRITE_PARAGRAPH_ALIGNMENT_CENTER) {
		y = layoutRect.top + (layoutRect.bottom - layoutRect.top - height) / 2.0f;
	} else if (paragraphAlignment == DWRITE_PARAGRAPH_ALIGNMENT_FAR) {
		y = layoutRect.bottom - height;
	}

	return D2D1::RectF(x, y, x + width, y + height);
}

// 测量文本布局的边界矩形（支持自动换行）
D2D1_RECT_F CD2DRender::MeasureTextLayout(const wchar_t* text, float maxWidth, IDWriteTextFormat* textFormat)
{
	if (!m_pDWriteFactory || !text)
		return D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f);

	// 如果没有提供文本格式，使用默认的
	if (!textFormat)
		return D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f);

	// 创建文本布局对象来测量文本
	CComPtr<IDWriteTextLayout> textLayout;
	HRESULT hr = m_pDWriteFactory->CreateTextLayout(
		text,
		static_cast<UINT32>(wcslen(text)),
		textFormat,
		maxWidth,  // 最大宽度
		FLT_MAX,   // 最大高度（不限制）
		&textLayout
		);

	if (FAILED(hr))
		return D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f);

	// 获取文本布局的整体尺寸
	DWRITE_TEXT_METRICS textMetrics;
	hr = textLayout->GetMetrics(&textMetrics);
	if (FAILED(hr))
		return D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f);

	return D2D1::RectF(0.0f, 0.0f, textMetrics.widthIncludingTrailingWhitespace, textMetrics.height);
}

void CD2DRender::DrawText(const wchar_t* text, const D2D1_RECT_F& rect, const D2D1_COLOR_F& color, IDWriteTextFormat* textFormat)
{
    if (!m_pRenderTarget || !text)
        return;

    // 如果没有提供文本格式
    if (!textFormat)
        return;

    // 创建文本颜色画刷
    auto brush = CreateBrush(color);
    if (!brush)
        return;

    // 绘制文本
    m_pRenderTarget->DrawText(
        text,
        static_cast<UINT32>(wcslen(text)),
        textFormat,
        rect,
        brush
    );
}

void CD2DRender::DrawTextLayout(float x, float y, const wchar_t* text, const D2D1_COLOR_F& color, float maxWidth, float maxHeight, IDWriteTextFormat* textFormat)
{
	if (!m_pRenderTarget || !m_pDWriteFactory || !text)
		return;

	// 如果没有提供文本格式，使用默认的
	if (!textFormat)
		return;

	// 创建文本布局对象
	CComPtr<IDWriteTextLayout> textLayout;
	HRESULT hr = m_pDWriteFactory->CreateTextLayout(
		text,
		static_cast<UINT32>(wcslen(text)),
		textFormat,
		maxWidth,
		maxHeight, // 最大高度（不限制）
		&textLayout
		);

	if (FAILED(hr))
		return;

	// 创建文本颜色画刷
	auto brush = CreateBrush(color);
	if (!brush)
		return;

	// 绘制文本布局（支持换行）
	m_pRenderTarget->DrawTextLayout(
		D2D1::Point2F(x, y),
		textLayout,
		brush
		);
}

//
// D2DLineHelper类的实现
//

// 直接绘制二次贝塞尔曲线
void CD2DRender::DrawQuadraticBezier(const D2D1_POINT_2F& startPoint, const D2D1_QUADRATIC_BEZIER_SEGMENT& QuadraticBezier, const D2D1_COLOR_F& color, float strokeWidth)
{
	if (!m_pRenderTarget)
		return;

	// 开始绘制路径
	ID2D1PathGeometry* pGeometry = nullptr;
	ID2D1GeometrySink* pSink = nullptr;

	// 创建路径几何对象
	m_pFactory->CreatePathGeometry(&pGeometry);
	pGeometry->Open(&pSink);

	// 开始绘制：移动到起点
	pSink->BeginFigure(startPoint, D2D1_FIGURE_BEGIN_FILLED);

	// 绘制二次贝塞尔曲线：从起点到终点，由控制点控制曲线形状
	pSink->AddQuadraticBezier(QuadraticBezier);

	// 结束绘制并提交
	pSink->EndFigure(D2D1_FIGURE_END_OPEN);
	pSink->Close();

	// 用画笔绘制路径
	DrawGeometry(pGeometry, color, strokeWidth); 

	// 释放资源
	if (pSink) pSink->Release();
	if (pGeometry) pGeometry->Release();
}

// 直接绘制直线段
void CD2DRender::DrawLineSegment(const D2D1_POINT_2F& startPoint, const D2D1_POINT_2F& endPoint, const D2D1_COLOR_F& color, float strokeWidth)
{
	if (!m_pRenderTarget)
		return;

	// 开始绘制路径
	ID2D1PathGeometry* pGeometry = nullptr;
	ID2D1GeometrySink* pSink = nullptr;

	// 创建路径几何对象
	m_pFactory->CreatePathGeometry(&pGeometry);
	pGeometry->Open(&pSink);

	// 开始绘制：移动到起点
	pSink->BeginFigure(startPoint, D2D1_FIGURE_BEGIN_FILLED);

	// 绘制直线段：从起点到终点
	pSink->AddLine(endPoint);

	// 结束绘制并提交
	pSink->EndFigure(D2D1_FIGURE_END_OPEN);
	pSink->Close();

	// 用画笔绘制路径
	DrawGeometry(pGeometry, color, strokeWidth); 

	// 释放资源
	if (pSink) pSink->Release();
	if (pGeometry) pGeometry->Release();
}

// 直接绘制弧线
void CD2DRender::DrawArcSegment(const D2D1_POINT_2F& startPoint, const D2D1_ARC_SEGMENT& arcSegment, const D2D1_COLOR_F& color, float strokeWidth)
{
	if (!m_pRenderTarget)
		return;

	// 开始绘制路径
	ID2D1PathGeometry* pGeometry = nullptr;
	ID2D1GeometrySink* pSink = nullptr;

	// 创建路径几何对象
	m_pFactory->CreatePathGeometry(&pGeometry);
	pGeometry->Open(&pSink);

	// 开始绘制：移动到起点
	pSink->BeginFigure(startPoint, D2D1_FIGURE_BEGIN_FILLED);

	// 绘制弧线
	pSink->AddArc(arcSegment);

	// 结束绘制并提交
	pSink->EndFigure(D2D1_FIGURE_END_OPEN);
	pSink->Close();

	// 用画笔绘制路径
	DrawGeometry(pGeometry, color, strokeWidth); 

	// 释放资源
	if (pSink) pSink->Release();
	if (pGeometry) pGeometry->Release();
}

// 直接绘制三次贝塞尔曲线
void CD2DRender::DrawCubicBezier(const D2D1_POINT_2F& startPoint, const D2D1_BEZIER_SEGMENT& bezierSegment, const D2D1_COLOR_F& color, float strokeWidth)
{
	if (!m_pRenderTarget)
		return;

	// 开始绘制路径
	ID2D1PathGeometry* pGeometry = nullptr;
	ID2D1GeometrySink* pSink = nullptr;

	// 创建路径几何对象
	m_pFactory->CreatePathGeometry(&pGeometry);
	pGeometry->Open(&pSink);

	// 开始绘制：移动到起点
	pSink->BeginFigure(startPoint, D2D1_FIGURE_BEGIN_FILLED);

	// 绘制三次贝塞尔曲线
	pSink->AddBezier(bezierSegment);

	// 结束绘制并提交
	pSink->EndFigure(D2D1_FIGURE_END_OPEN);
	pSink->Close();

	// 用画笔绘制路径
	DrawGeometry(pGeometry, color, strokeWidth); 

	// 释放资源
	if (pSink) pSink->Release();
	if (pGeometry) pGeometry->Release();
}

// 创建绘制状态块
void CD2DRender::CreateDrawingStateBlock()
{
	if (!m_pFactory)
		return;
	m_pDrawingStateBlock.Release();
	m_pFactory->CreateDrawingStateBlock(&m_pDrawingStateBlock);
}

// 保存当前绘制状态
void CD2DRender::SaveDrawingState()
{
	if (!m_pRenderTarget || !m_pDrawingStateBlock)
		return;

	m_pRenderTarget->SaveDrawingState(m_pDrawingStateBlock);
}

// 恢复之前保存的绘制状态
void CD2DRender::RestoreDrawingState()
{
	if (!m_pRenderTarget || !m_pDrawingStateBlock)
		return;

	m_pRenderTarget->RestoreDrawingState(m_pDrawingStateBlock);
}

// 添加轴对齐裁剪区域
void CD2DRender::PushAxisAlignedClip(const D2D1_RECT_F& clipRect, D2D1_ANTIALIAS_MODE mode)
{
	if (!m_pRenderTarget)
		return;

	m_pRenderTarget->PushAxisAlignedClip(clipRect, mode);
}

// 移除最近添加的裁剪区域
void CD2DRender::PopAxisAlignedClip()
{
	if (!m_pRenderTarget)
		return;

	m_pRenderTarget->PopAxisAlignedClip();
}


// 设置变换矩阵
void CD2DRender::SetTransform(const D2D1_MATRIX_3X2_F& transform)
{
	if (!m_pRenderTarget)
		return;

	m_pRenderTarget->SetTransform(transform);
}

// 获取当前变换矩阵
void CD2DRender::GetTransform(D2D1_MATRIX_3X2_F* matrix) 
{
	if (!m_pRenderTarget)
		return;

	m_pRenderTarget->GetTransform(matrix);
}

// DPI坐标转换函数实现
void CD2DRender::Resize(float width, float height)
{
	if (m_pRenderTarget)
	{
		D2D1_SIZE_U size = D2D1::SizeU(
			static_cast<UINT32>(width),
			static_cast<UINT32>(height)
			);
		m_pRenderTarget->Resize(size);
		UpdateDpiScale();
	}
}

void CD2DRender::UpdateDpiScale()
{
	if (m_pFactory)
	{
		float dpiX, dpiY;
		m_pFactory->GetDesktopDpi(&dpiX, &dpiY);
		m_dpiScaleX = static_cast<float>(dpiX) / 96.0f;
		m_dpiScaleY = static_cast<float>(dpiY) / 96.0f;
	}
}

float CD2DRender::ScaleX(float x, BOOL Div) const
{
	if (Div == FALSE)
		return x * m_dpiScaleX;
	else
		return x / m_dpiScaleX;

}

float CD2DRender::ScaleY(float y, BOOL Div) const
{
	if (Div == FALSE)
		return y * m_dpiScaleY;		
	else	
		return y / m_dpiScaleY;
}

D2D1_POINT_2F CD2DRender::ScalePointF(const D2D1_POINT_2F& point, BOOL Div) const
{
	if (Div == FALSE)
		return D2D1::Point2F(point.x * m_dpiScaleX, point.y * m_dpiScaleY);		
	else
		return D2D1::Point2F(point.x / m_dpiScaleX, point.y / m_dpiScaleY);
		
}

D2D1_SIZE_F CD2DRender::ScaleSizeF(const D2D1_SIZE_F& size, BOOL Div) const
{
	if (Div == FALSE)
		return D2D1::SizeF(size.width * m_dpiScaleX, size.height * m_dpiScaleY);		
	else
		return D2D1::SizeF(size.width / m_dpiScaleX, size.height / m_dpiScaleY);		

}

D2D1_RECT_F CD2DRender::ScaleRectF(const D2D1_RECT_F& rect, BOOL Div) const
{
	if (Div == FALSE)
	 return D2D1::RectF(
        rect.left * m_dpiScaleX,
        rect.top * m_dpiScaleY,
        rect.right * m_dpiScaleX,
        rect.bottom * m_dpiScaleY
    );
	else
    return D2D1::RectF(
        rect.left / m_dpiScaleX,
        rect.top / m_dpiScaleY,
        rect.right / m_dpiScaleX,
        rect.bottom / m_dpiScaleY
    );
}
 
