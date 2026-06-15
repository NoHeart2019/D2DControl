// CD2DControlCssUI.cpp
#include "stdafx.h"
#include "D2DControlUI.h"



CD2DRender::CD2DRender() : m_dpiScaleX(1.0f), m_dpiScaleY(1.0f) {Initialize();}
CD2DRender::~CD2DRender() {}

bool CD2DRender::Initialize()
{
	InitializeFactories();
	GetDesktopDpiScale();

	return true;
}

bool CD2DRender::InitializeFactories()
{
	// 创建单线程工厂（适合大多数 UI 场景）
	HRESULT hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		__uuidof(ID2D1Factory),
		nullptr,
		reinterpret_cast<void**>(&m_pFactory)
		);
	if (FAILED(hr))
		return false;

	
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&m_pWriteFactory));
	if (FAILED(hr))
		return false;
	
	return true;
	
}

bool CD2DRender::InitializeRenderTarget(HWND hWnd)
{
	if (!hWnd || !m_pFactory)
		return false;

	// 获取系统DPI
	FLOAT dpiX, dpiY;
	m_pFactory->GetDesktopDpi(&dpiX, &dpiY);

	// 获取窗口客户区尺寸
	RECT rc = {0};
	if (!GetClientRect(hWnd, &rc))
		return false;
	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

	// 创建 HWND RenderTarget，直接使用桌面 DPI 值
	D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
		dpiX, dpiY
		);

	D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = D2D1::HwndRenderTargetProperties(
		hWnd, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY
		);

	m_pRenderTarget.Release(); // 释放旧目标
	HRESULT hr = m_pFactory->CreateHwndRenderTarget(&rtProps, &hwndProps, &m_pRenderTarget);
	return SUCCEEDED(hr);
}

// ==================== 开始绘制 ====================
void CD2DRender::BeginDraw()
{
	if (m_pRenderTarget)
	{
		m_pRenderTarget->BeginDraw();
	}
}

// ==================== 结束绘制 ====================
HRESULT CD2DRender::EndDraw()
{
	if (m_pRenderTarget)
	{
		return m_pRenderTarget->EndDraw();
	}
	return E_FAIL;   // 或 D2DERR_RECREATE_TARGET 等，但返回 E_FAIL 足够标识未初始化
}

void CD2DRender::Resize(float width, float height)
{
	if (m_pRenderTarget)
	{
		D2D1_SIZE_U size = D2D1::SizeU(
			static_cast<UINT32>(width),
			static_cast<UINT32>(height)
			);
		m_pRenderTarget->Resize(size);
		
	}
}


// ==================== 清除绘制区域 ====================
void CD2DRender::Clear(const D2D1_COLOR_F& color /*= D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f)*/)
{
	if (m_pRenderTarget)
	{
		m_pRenderTarget->Clear(color);
	}
}

void CD2DRender::GetDesktopDpiScale()
{
	// 获取桌面 DPI
	float dpiX = 96.0f, dpiY = 96.0f;
	m_pFactory->GetDesktopDpi(&dpiX, &dpiY);

	// 计算缩放因子（相对于 96 DPI）
	m_dpiScaleX = dpiX / 96.0f;
	m_dpiScaleY = dpiY / 96.0f;
}

void CD2DRender::SetDpiScale(float dpiSacleX , float dpiScaleY)
{m_dpiScaleX = dpiSacleX, m_dpiScaleY = dpiScaleY;}

// DPI坐标转换函数实现
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

// ==================== 圆角矩形几何体 ====================
CComPtr<ID2D1PathGeometry> CD2DRender::CreateRoundRectGeometry(const D2D1_RECT_F& rect, const CD2DRadius& radius)
{
	if (!m_pFactory)
		return nullptr;

	CComPtr<ID2D1PathGeometry> spGeometry;
	HRESULT hr = m_pFactory->CreatePathGeometry(&spGeometry);
	if (FAILED(hr)) return nullptr;

	CComPtr<ID2D1GeometrySink> spSink;
	hr = spGeometry->Open(&spSink);
	if (FAILED(hr)) return nullptr;

	// 计算关键点（同上）
	D2D1_POINT_2F p0 = D2D1::Point2F(rect.left + radius.m_topLeft, rect.top);
	D2D1_POINT_2F p1 = D2D1::Point2F(rect.right - radius.m_topRight, rect.top);
	D2D1_POINT_2F p2 = D2D1::Point2F(rect.right, rect.top + radius.m_topRight);
	D2D1_POINT_2F p3 = D2D1::Point2F(rect.right, rect.bottom - radius.m_bottomRight);
	D2D1_POINT_2F p4 = D2D1::Point2F(rect.right - radius.m_bottomRight, rect.bottom);
	D2D1_POINT_2F p5 = D2D1::Point2F(rect.left + radius.m_bottomLeft, rect.bottom);
	D2D1_POINT_2F p6 = D2D1::Point2F(rect.left, rect.bottom - radius.m_bottomLeft);
	D2D1_POINT_2F p7 = D2D1::Point2F(rect.left, rect.top + radius.m_topLeft);

	spSink->BeginFigure(p0, D2D1_FIGURE_BEGIN_FILLED);
	spSink->AddLine(p1);
	if (radius.m_topRight > 0)
		spSink->AddArc(D2D1::ArcSegment(p2, D2D1::SizeF(radius.m_topRight, radius.m_topRight), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
	spSink->AddLine(p3);
	if (radius.m_bottomRight > 0)
		spSink->AddArc(D2D1::ArcSegment(p4, D2D1::SizeF(radius.m_bottomRight, radius.m_bottomRight), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
	spSink->AddLine(p5);
	if (radius.m_bottomLeft > 0)
		spSink->AddArc(D2D1::ArcSegment(p6, D2D1::SizeF(radius.m_bottomLeft, radius.m_bottomLeft), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
	spSink->AddLine(p7);
	if (radius.m_topLeft > 0)
		spSink->AddArc(D2D1::ArcSegment(p0, D2D1::SizeF(radius.m_topLeft, radius.m_topLeft), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
	spSink->EndFigure(D2D1_FIGURE_END_CLOSED);
	spSink->Close();

	// 返回原始指针，由调用者管理（通常返回给智能指针）
	return spGeometry;
}

// ==================== 创建背景画刷 ====================
CComPtr<ID2D1Brush> CD2DRender::CreateBackgroundBrush(const CD2DBackground& bg, const D2D1_RECT_F& rect)
{
	if (!m_pRenderTarget)
		return nullptr;

	switch (bg.m_type)
	{
	case CD2DBackground::Solid:
		{
			CComPtr<ID2D1SolidColorBrush> spBrush;
			m_pRenderTarget->CreateSolidColorBrush(bg.m_solid.color, &spBrush);
			return spBrush.Detach();
		}
	case CD2DBackground::LinearGradient:
		{
			// 获取矩形中心及半对角线长度
			float cx = (rect.left + rect.right) * 0.5f;
			float cy = (rect.top + rect.bottom) * 0.5f;
			float w = rect.right - rect.left;
			float h = rect.bottom - rect.top;
			float halfDiag = 0.5f * sqrtf(w * w + h * h);
			if (halfDiag < 1e-6f) halfDiag = 1.0f;   // 避免零长度

			// 根据弧度计算方向向量（0 弧度 = 正南方向，正角沿屏幕顺时针） CSS deg :bg.m_linear.degrees
			float radians = (bg.m_linear.degrees - 90.0f) * 3.14159265359f  / 180.0f;
			float dx = cosf(radians);
			float dy = sinf(radians);

			D2D1_POINT_2F start = D2D1::Point2F(cx - dx * halfDiag, cy - dy * halfDiag);
			D2D1_POINT_2F end   = D2D1::Point2F(cx + dx * halfDiag, cy + dy * halfDiag);

			// 创建渐变停止点集合
			CComPtr<ID2D1GradientStopCollection> spStops;
			D2D1_GRADIENT_STOP stops[10];
			for (int i = 0; i < bg.m_linear.m_stopCount; ++i)
			{
				stops[i].position = bg.m_linear.m_stops[i].position;
				stops[i].color = bg.m_linear.m_stops[i].color;
			}
			m_pRenderTarget->CreateGradientStopCollection(
				stops, bg.m_linear.m_stopCount, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &spStops);
			if (!spStops)
				return nullptr;

			CComPtr<ID2D1LinearGradientBrush> spBrush;
			m_pRenderTarget->CreateLinearGradientBrush(
				D2D1::LinearGradientBrushProperties(start, end),
				spStops,
				&spBrush);
			return spBrush.Detach();  // 正确方式：直接返回智能指针，自动管理引用
		}
	case CD2DBackground::RadialGradient:
		{
			CComPtr<ID2D1GradientStopCollection> spStops;
			D2D1_GRADIENT_STOP stops[10];
			for (int i = 0; i < bg.m_radial.m_stopCount; ++i)
			{
				stops[i].position = bg.m_radial.m_stops[i].position;
				stops[i].color = bg.m_radial.m_stops[i].color;
			}
			m_pRenderTarget->CreateGradientStopCollection(
				stops, bg.m_radial.m_stopCount, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &spStops);
			if (!spStops)
				return nullptr;
				
			CComPtr<ID2D1RadialGradientBrush> spBrush;
			m_pRenderTarget->CreateRadialGradientBrush(
				D2D1::RadialGradientBrushProperties(
				bg.m_radial.center,
				bg.m_radial.gradientOriginOffset,
				bg.m_radial.radiusX,
				bg.m_radial.radiusY),
				spStops,
				&spBrush);
			return spBrush.Detach();
		}
	default:
		return nullptr;
	}
}

CComPtr<ID2D1StrokeStyle> CD2DRender::CreateStrokeStyle(ED2DLineStyle style) const
{
	if (!m_pFactory) return nullptr;

	CComPtr<ID2D1StrokeStyle> spStroke;
	D2D1_STROKE_STYLE_PROPERTIES props = D2D1::StrokeStyleProperties(
		D2D1_CAP_STYLE_FLAT,
		D2D1_CAP_STYLE_FLAT,
		D2D1_CAP_STYLE_FLAT,
		D2D1_LINE_JOIN_MITER,
		10.0f,
		(style == Dashed) ? D2D1_DASH_STYLE_DASH : (style == Dotted) ? D2D1_DASH_STYLE_DOT : D2D1_DASH_STYLE_SOLID,
		0.0f);

	m_pFactory->CreateStrokeStyle(props, nullptr, 0, &spStroke);
	return spStroke;
}

CComPtr<IDWriteTextFormat> CD2DRender::CreateTextFormat(const CD2DTextStyle& style)
{
	// 构建缓存键
	CAtlString key;
	key.Format(L"%s|%.1f|%d|%d|%d",
		style.fontFamily, style.fontSize,
		style.fontWeight, style.fontStyle, style.fontStretch);

	// 检查缓存 
	/*auto* pPair = m_textFormatCache.Lookup(key);
	if (pPair)
		return pPair->m_value;*/

	// 创建新格式
	CComPtr<IDWriteTextFormat> spFormat;
	HRESULT hr = m_pWriteFactory->CreateTextFormat(
		style.fontFamily,
		nullptr,   // 字体集合
		style.fontWeight,
		style.fontStyle,
		style.fontStretch,
		style.fontSize,
		L"",       // locale
		&spFormat);
	if (FAILED(hr))
		return nullptr;

	// 设置对齐
	spFormat->SetTextAlignment(style.horizontalAlign);
	spFormat->SetParagraphAlignment(style.verticalAlign);
	spFormat->SetWordWrapping(style.wordWrap ? DWRITE_WORD_WRAPPING_WRAP : DWRITE_WORD_WRAPPING_NO_WRAP);

	// 存入缓存
	//m_textFormatCache.SetAt(key, spFormat);
	return spFormat;
}

D2D1_SIZE_F CD2DRender::MeasureString(const CAtlString& text, IDWriteTextFormat* pFormat,
	float maxWidth, float maxHeight) const
{
	if (!pFormat || text.IsEmpty()) return D2D1::SizeF(0, 0);

	IDWriteFactory* pDW = GetWriteFactory();
	ID2D1RenderTarget* pRT = GetRenderTarget();
	if (!pDW || !pRT) return D2D1::SizeF(0, 0);

	CComPtr<IDWriteTextLayout> spLayout;
	pDW->CreateTextLayout(text.GetString(), (UINT32)text.GetLength(),
		pFormat, maxWidth, maxHeight, &spLayout);
	if (!spLayout) return D2D1::SizeF(0, 0);

	DWRITE_TEXT_METRICS metrics = {};
	spLayout->GetMetrics(&metrics);
	return D2D1::SizeF(metrics.width, metrics.height);
}

D2D1_SIZE_F CD2DRender::MeasureText(const CAtlString& text, const CD2DTextStyle& textStyle,
	float maxWidth, float maxHeight) const
{
	if (text.IsEmpty()) return D2D1::SizeF(0.0f, 0.0f);

	CComPtr<IDWriteTextFormat> spFormat = const_cast<CD2DRender*>(this)->CreateTextFormat(textStyle);
	if (!spFormat) return D2D1::SizeF(0.0f, 0.0f);

	// 复用已有的通用测量函数（需要传入格式指针）
	return MeasureString(text, spFormat, maxWidth, maxHeight);
}

// 在指定矩形内绘制文本（自动处理对齐），支持滚动偏移
void CD2DRender::DrawText(const CAtlString& text, const CD2DTextStyle& textStyle, 
	const D2D1_RECT_F& rect, float scrollOffsetY)
{
	if (text.IsEmpty()) return;
	ID2D1RenderTarget* pRT = GetRenderTarget();
	if (!pRT) return;

	CComPtr<IDWriteTextFormat> spFormat = CreateTextFormat(textStyle);
	if (!spFormat) return;

	IDWriteFactory* pDW = GetWriteFactory();
	CComPtr<IDWriteTextLayout> spLayout;
	pDW->CreateTextLayout(
		text.GetString(), (UINT32)text.GetLength(),
		spFormat,
		rect.right - rect.left,   // 布局宽度
		rect.bottom - rect.top,   // 布局高度（用于垂直对齐）
		&spLayout);
	if (!spLayout) return;

	// 水平/垂直对齐已在 CreateTextFormat 中设置，无需再调 SetTextAlignment

	CComPtr<ID2D1SolidColorBrush> spBrush;
	pRT->CreateSolidColorBrush(textStyle.color, &spBrush);
	if (spBrush)
	{
		// 直接以矩形左上角为原点，加上滚动偏移
		pRT->DrawTextLayout(
			D2D1::Point2F(rect.left, rect.top + scrollOffsetY),
			spLayout,
			spBrush);
	}
}

CComPtr<ID2D1PathGeometry> CD2DRender::CreatePolygonGeometry(const D2D1_POINT_2F* points, UINT32 count,
	bool close, bool filled) const
{
	if (!m_pFactory || count < 2) return nullptr;

	CComPtr<ID2D1PathGeometry> spGeometry;
	HRESULT hr = m_pFactory->CreatePathGeometry(&spGeometry);
	if (FAILED(hr)) return nullptr;

	CComPtr<ID2D1GeometrySink> spSink;
	hr = spGeometry->Open(&spSink);
	if (FAILED(hr)) return nullptr;

	// 设置起点与填充模式
	spSink->BeginFigure(points[0], filled ? D2D1_FIGURE_BEGIN_FILLED : D2D1_FIGURE_BEGIN_HOLLOW);

	// 添加后续点
	for (UINT32 i = 1; i < count; ++i)
		spSink->AddLine(points[i]);

	// 结束图形（自动关闭或开放）
	spSink->EndFigure(close ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);
	spSink->Close();

	return spGeometry;
}

CComPtr<ID2D1PathGeometry> CD2DRender::CreatePolylineGeometry(const D2D1_POINT_2F* points, UINT32 count) const
{
	return CreatePolygonGeometry(points, count, false, false);
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

const float CD2DControlBaseUI::DRAG_THRESHOLD = 3.0f;


CD2DControlBaseUI::CD2DControlBaseUI()
	: m_rectangle(D2D1::RectF())
	, m_isVisible(true)
	, m_isDragEnabled(false)
	, m_dragStartPoint(D2D1::Point2F(0.0f, 0.0f))
	, m_contentSize(D2D1::SizeF(0,0))   
	, m_contentDirty(true)
	, m_zOrder(Z_BACKGROUND)
          
{
}

CD2DControlBaseUI::~CD2DControlBaseUI()
{
}

void CD2DControlBaseUI::SetRectangle(const D2D1_RECT_F& rect)
{
	m_rectangle = rect;
	m_posInfo.left = rect.left;
	m_posInfo.top = rect.top;
}


D2D1_POINT_2F CD2DControlBaseUI::GetPosition() const
{
	return D2D1::Point2F(m_rectangle.left, m_rectangle.top);
}

void CD2DControlBaseUI::SetPosition(float x, float y)
{
	float w = m_rectangle.right - m_rectangle.left;
	float h = m_rectangle.bottom - m_rectangle.top;
	m_rectangle.left = x;
	m_rectangle.top = y;
	m_rectangle.right = x + w;
	m_rectangle.bottom = y + h;
}

void CD2DControlBaseUI::SetAbsoluteOffset(float left, float top) {
	m_posInfo.type = CD2DPosition::Absolute;
	m_posInfo.left = left;
	m_posInfo.top  = top;
}

D2D1_POINT_2F CD2DControlBaseUI::GetCenter() const
{
	return D2D1::Point2F(
		(m_rectangle.left + m_rectangle.right) * 0.5f,
		(m_rectangle.top + m_rectangle.bottom) * 0.5f);
}

void CD2DControlBaseUI::SetCenter(float cx, float cy)
{
	float w = m_rectangle.right - m_rectangle.left;
	float h = m_rectangle.bottom - m_rectangle.top;
	m_rectangle.left = cx - w * 0.5f;
	m_rectangle.top = cy - h * 0.5f;
	m_rectangle.right = m_rectangle.left + w;
	m_rectangle.bottom = m_rectangle.top + h;
}

float CD2DControlBaseUI::GetWidth() const
{
	return m_rectangle.right - m_rectangle.left;
}

void CD2DControlBaseUI::SetWidth(float width)
{
	m_rectangle.right = m_rectangle.left + width;

}

float CD2DControlBaseUI::GetHeight() const
{
	return m_rectangle.bottom - m_rectangle.top;
}

void CD2DControlBaseUI::SetHeight(float height)
{
	m_rectangle.bottom = m_rectangle.top + height;

}

D2D1_SIZE_F CD2DControlBaseUI::GetSize() const
{
	return D2D1::SizeF(GetWidth(), GetHeight());
}

void CD2DControlBaseUI::SetSize(float width, float height)
{
	m_rectangle.right = m_rectangle.left + width;
	m_rectangle.bottom = m_rectangle.top + height;
}

D2D1_SIZE_F CD2DControlBaseUI::MeasureContent(CD2DRender* /*pRender*/, float /*maxWidth*/, float) const
{
	// 基类默认内容尺寸就是当前控件尺寸（无内容概念）
	return GetSize();
}

D2D1_SIZE_F CD2DControlBaseUI::GetContentSize(CD2DRender* pRender, float maxWidth, float maxHeight)
{
	if (m_contentDirty)
	{
		m_contentSize = MeasureContent(pRender, maxWidth,  maxHeight);
		m_contentDirty = false;
	}
	return m_contentSize;
}

void CD2DControlBaseUI::InvalidateContent()
{
	m_contentDirty = true;
}


// 根据宽度/高度策略计算期望内容尺寸
D2D1_SIZE_F CD2DControlBaseUI::CalcDesiredSize(CD2DRender* pRender, float maxWidth, float maxHeight)
{
	D2D1_SIZE_F content = MeasureContent(pRender, maxWidth);
	float w = content.width;
	float h = content.height;

	// 宽度策略应用
	if (m_widthPolicy.mode == CD2DSizePolicy::Fixed && m_widthPolicy.value > 0)
		w = m_widthPolicy.value;
	else if (m_widthPolicy.mode != CD2DSizePolicy::Auto && m_widthPolicy.mode != CD2DSizePolicy::Fixed)
		w = m_widthPolicy.minValue;  // Fill/Expand/Percent 不贡献固定宽度，取最小宽度

	w = max(m_widthPolicy.minValue, min(w, m_widthPolicy.maxValue));

	// 高度策略应用
	if (m_heightPolicy.mode == CD2DSizePolicy::Fixed && m_heightPolicy.value > 0)
		h = m_heightPolicy.value;
	else if (m_heightPolicy.mode != CD2DSizePolicy::Auto && m_heightPolicy.mode != CD2DSizePolicy::Fixed)
		h = m_heightPolicy.minValue;

	h = max(m_heightPolicy.minValue, min(h, m_heightPolicy.maxValue));

	return D2D1::SizeF(w, h);
}

// 重写 GetDesiredSize：返回期望总尺寸（内容+装饰）
D2D1_SIZE_F CD2DControlBaseUI::GetDesiredSize(CD2DRender* pRender, float maxWidth, float maxHeight)
{
	// 如果两个方向都是 Auto 或 Fixed，可通过 CalcDesiredSize 获取
	// 否则对于弹性模式，期望尺寸为零或最小，由容器在分配时决定
	//if (m_widthPolicy.mode == CD2DSizePolicy::Fill || m_widthPolicy.mode == CD2DSizePolicy::Expand ||
	//	m_heightPolicy.mode == CD2DSizePolicy::Fill || m_heightPolicy.mode == CD2DSizePolicy::Expand)
	//{
	//	// 弹性控件不贡献固定尺寸，返回最小尺寸
	//	return D2D1::SizeF(m_widthPolicy.minValue, m_heightPolicy.minValue);
	//}
	// 其他情况（Auto, Fixed, Percent）返回计算值
	return CalcDesiredSize(pRender, maxWidth, maxHeight);
}

// 重写 SizeToContent：根据 Auto 策略自动调整尺寸
void CD2DControlBaseUI::SizeToContent(CD2DRender* pRender, float maxValueWidth, float maxValueHeight)
{
	if (m_widthPolicy.mode == CD2DSizePolicy::Auto || m_heightPolicy.mode == CD2DSizePolicy::Auto)
	{
		D2D1_SIZE_F contentSize = MeasureContent(pRender, maxValueWidth);

		if (m_widthPolicy.mode == CD2DSizePolicy::Auto)
		{
			float w = contentSize.width;
			w = max(m_widthPolicy.minValue, min(w, m_widthPolicy.maxValue));
			SetWidth(w);
		}
		if (m_heightPolicy.mode == CD2DSizePolicy::Auto)
		{
			float h = contentSize.height;
			h = max(m_heightPolicy.minValue, min(h, m_heightPolicy.maxValue));
			SetHeight(h);
		}
	}
}

// ========== 命中测试（基类默认仅使用布局矩形） ==========
bool CD2DControlBaseUI::IsHitTest(const D2D1_POINT_2F& point) const
{
	return (point.x >= m_rectangle.left && point.x <= m_rectangle.right &&
		point.y >= m_rectangle.top  && point.y <= m_rectangle.bottom);
}

// ========== 矩形操作 ==========
void CD2DControlBaseUI::Move(float dx, float dy)
{
	m_rectangle.left   += dx;
	m_rectangle.top    += dy;
	m_rectangle.right  += dx;
	m_rectangle.bottom += dy;
}

void CD2DControlBaseUI::MoveTo(float x, float y)
{
	float w = m_rectangle.right - m_rectangle.left;
	float h = m_rectangle.bottom - m_rectangle.top;
	m_rectangle.left   = x;
	m_rectangle.top    = y;
	m_rectangle.right  = x + w;
	m_rectangle.bottom = y + h;
}

void CD2DControlBaseUI::OffsetRect(float left, float top, float right, float bottom)
{
	m_rectangle.left   += left;
	m_rectangle.top    += top;
	m_rectangle.right  -= right;
	m_rectangle.bottom -= bottom;
}

// ========== 鼠标事件（默认实现） ==========
bool CD2DControlBaseUI::OnMouseDownL(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled())
		return false;
	if (!IsHitTest(point))
		return false;

	m_dragStartPoint = point;
	SetPressed(true);
	SetDragging(false);
	//SetChecked(!IsChecked());
	bool ch = IsChecked();
	SetChecked(!ch);
	
	return true;
}

bool CD2DControlBaseUI::OnMouseMove(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled())
		return false;

	// 拖拽中
	if (IsDragging() && IsDragEnabled())
	{
		D2D1_POINT_2F delta = D2D1::Point2F(
			point.x - m_dragStartPoint.x,
			point.y - m_dragStartPoint.y);
		if (delta.x != 0.0f || delta.y != 0.0f)
		{
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
				m_dragStartPoint = point;
				OnDragMove(D2D1::Point2F(dx, dy));
				return true;
			}
		}
		return false;
	}

	// 正常悬停检测
	if (IsHitTest(point))
	{
		if (!IsHover())
		{
			SetHover(true);
			OnMouseEnter(point);
			return true;
		}
	}
	else
	{
		if (IsHover())
		{
			SetHover(false);
			OnMouseLeave(point);
			return true;
		}
	}
	return false;
}

bool CD2DControlBaseUI::OnMouseUpL(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled())
		return false;

	if (IsDragging())
	{
		SetDragging(false);
		OnDragEnd(point);
		return true;
	}

	if (IsPressed())
	{
		SetPressed(false);
		if (IsHitTest(point))
		{
			TriggerEvent(CD2DEvent::Click);
		}
		return true;
	}
	return false;
}

bool CD2DControlBaseUI::OnMouseDownL2(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled())
		return false;
	if (!IsHitTest(point))
		return false;

	TriggerEvent(CD2DEvent::DoubleClick);
	return true;
}

bool CD2DControlBaseUI::OnMouseUpL2(const D2D1_POINT_2F& point)
{
	if (!IsVisible() || IsDisabled())
		return false;
	if (!IsHitTest(point))
		return false;
	return true;
}

bool CD2DControlBaseUI::OnMouseLeave(const D2D1_POINT_2F& /*point*/)
{
	if (!IsDragging())
		SetPressed(false);
	SetHover(false);
	TriggerEvent(CD2DEvent::HoverLeave);
	return false;
}

bool CD2DControlBaseUI::OnMouseEnter(const D2D1_POINT_2F& /*point*/)
{
	if (!IsVisible() || IsDisabled())
		return false;
	SetHover(true);
	TriggerEvent(CD2DEvent::HoverEnter);
	return false;
}

// 以下键盘/滚轮/DPI 事件默认不做额外处理，可被派生类重写
bool CD2DControlBaseUI::OnMouseWheel(float /*delta*/, const D2D1_POINT_2F& /*point*/)   { return false; }
bool CD2DControlBaseUI::OnKeyDown(DWORD /*keyCode*/)    { return true; }
bool CD2DControlBaseUI::OnKeyUp(DWORD /*keyCode*/)      { return true; }
bool CD2DControlBaseUI::OnChar(DWORD /*ch*/)            { return true; }
bool CD2DControlBaseUI::OnDpiChanged(float /*dpiScaleX*/, float /*dpiScaleY*/) { return true; }

// ========== 拖拽虚函数默认实现 ==========
void CD2DControlBaseUI::OnDragBegin(const D2D1_POINT_2F& /*startPoint*/)
{
	SetZOrder(Z_TOPMOST);
	TriggerEvent(CD2DEvent::DragBegin);
}

void CD2DControlBaseUI::OnDragMove(const D2D1_POINT_2F& delta)
{
	Move(delta.x, delta.y);
	TriggerEvent(CD2DEvent::DragMove);
}

void CD2DControlBaseUI::OnDragEnd(const D2D1_POINT_2F& /*endPoint*/)
{
	SetZOrder(0);
	TriggerEvent(CD2DEvent::DragEnd);
}

// ========== 事件回调管理 ==========
void CD2DControlBaseUI::SetEventCallback(CD2DEvent::EType type, CD2DEvent event)
{
	if (event.IsValid())
		m_eventSlots.SetAt(type, event);
	else
		m_eventSlots.RemoveKey(type);
}

void CD2DControlBaseUI::RemoveEventCallback(CD2DEvent::EType type)
{
	m_eventSlots.RemoveKey(type);
}

void CD2DControlBaseUI::TriggerEvent(CD2DEvent::EType type)
{
	
}


// ==================== 内部辅助函数 ====================
bool IsBorderEdgeSet(const CD2DBorderEdge& edge)
{
	return edge.m_style != None
		|| edge.m_width != 0.0f
		|| edge.m_color.a != 0.0f;
}

bool IsRadiusSet(const CD2DRadius& radius)
{
	return radius.m_topLeft != 0.0f
		|| radius.m_topRight != 0.0f
		|| radius.m_bottomRight != 0.0f
		|| radius.m_bottomLeft != 0.0f;
}

bool IsBackgroundSet(const CD2DBackground& bg)
{
	return bg.m_type != CD2DBackground::None;
}

bool IsTransformSet(const CD2DTransform& trans)
{
	return trans.GetOperationCount() > 0;
}

bool IsMarginSet(const D2D1_RECT_F& rect)
{
	return rect.left != 0.0f || rect.top != 0.0f
		|| rect.right != 0.0f || rect.bottom != 0.0f;
}

bool IsPaddingSet(const D2D1_RECT_F& rect)
{
	return rect.left != 0.0f || rect.top != 0.0f
		|| rect.right != 0.0f || rect.bottom != 0.0f;
}

bool IsTextStyleSet(const CD2DTextStyle& ts) {
	return ts.fontSize > 0.0f || !ts.fontFamily.IsEmpty() || ts.color.a != 0.0f;
}



float CalculateActualLineHeight(const CD2DTextStyle& style)
{
	float fontSize = style.fontSize;
	switch (style.lineHeightUnit)
	{
	case CD2DTextStyle::Pixel:    return style.lineHeight;
	case CD2DTextStyle::Percent:  return fontSize * (style.lineHeight / 100.0f);
	case CD2DTextStyle::Multiplier:
	default:                      return fontSize * style.lineHeight;
	}
}


// ==================== CD2DControlUI 实现 ====================
CD2DControlUI::CD2DControlUI()
	: m_styles()            // 空映射
	, m_defaultStyle()      // 默认样式（各项为 0 / None）
	, m_spLayer()           // nullptr（图层按需创建）
	, m_text()              // 空字符串
	, m_debugInt(1234)
{
	// 无需额外操作，所有成员均通过默认构造或基类完成初始化
}

CD2DControlUI::~CD2DControlUI() {}

// ========== 样式存取 ==========
void CD2DControlUI::SetStyle(ED2DStatus state, const CD2DStyle& style)
{
	m_styles.SetAt(state, style);
	InvalidateContent();
}

const CD2DStyle& CD2DControlUI::GetStyle(ED2DStatus state) const
{
	const CAtlMap<ED2DStatus, CD2DStyle>::CPair* pos = m_styles.Lookup(state);
	if (pos != NULL)
		return pos->m_value;
	static const CD2DStyle s_empty;
	return s_empty;
}

bool CD2DControlUI::HasStyle(ED2DStatus state) const
{
	return m_styles.Lookup(state) != NULL;
}

void CD2DControlUI::SetDefaultStyle(const CD2DStyle& style)
{
	m_defaultStyle = style;
}

const CD2DStyle& CD2DControlUI::GetDefaultStyle() const
{
	return m_defaultStyle;
}

// ========== 层叠计算 ==========
CD2DStyle CD2DControlUI::CalculateEffectiveStyle() const
{
	CD2DStyle effective = m_defaultStyle;

	if (m_status.IsNormal())     ApplyStyle(effective, GetStyle(ED2DStatus::Normal));
	if (m_status.IsActive())     ApplyStyle(effective, GetStyle(ED2DStatus::Active));
	if (m_status.IsChecked())    ApplyStyle(effective, GetStyle(ED2DStatus::Checked));
	if (m_status.IsDragging())   ApplyStyle(effective, GetStyle(ED2DStatus::Dragging));
	if (m_status.IsFocused())    ApplyStyle(effective, GetStyle(ED2DStatus::Focused));
	if (m_status.IsHover())      ApplyStyle(effective, GetStyle(ED2DStatus::Hover));
	if (m_status.IsDragOver())  ApplyStyle(effective, GetStyle(DragOver));
	if (m_status.IsPressed())    ApplyStyle(effective, GetStyle(ED2DStatus::Pressed));

	if (m_status.IsHotTracked() && HasStyle(HotTracked))
		ApplyStyle(effective, GetStyle(HotTracked));

	if (m_status.IsDisabled())
		ApplyStyle(effective, GetStyle(Disabled));

	return effective;
}

void CD2DControlUI::ApplyStyle(CD2DStyle& dest, const CD2DStyle& src)
{
	if (IsBorderEdgeSet(src.border.m_top))    dest.border.m_top    = src.border.m_top;
	if (IsBorderEdgeSet(src.border.m_right))  dest.border.m_right  = src.border.m_right;
	if (IsBorderEdgeSet(src.border.m_bottom)) dest.border.m_bottom = src.border.m_bottom;
	if (IsBorderEdgeSet(src.border.m_left))   dest.border.m_left   = src.border.m_left;

	if (IsMarginSet(src.margin))  dest.margin  = src.margin;
	if (IsPaddingSet(src.padding)) dest.padding = src.padding;

	if (IsRadiusSet(src.radius))              dest.radius      = src.radius;
	if (IsBackgroundSet(src.background))      dest.background  = src.background;
	if (IsTransformSet(src.transform))        dest.transform   = src.transform;
	if (IsTextStyleSet(src.text))			  dest.text = src.text;
}

CD2DRadius CD2DControlUI::GetEffectiveRadius(const CD2DRadius& radius, const D2D1_RECT_F& borderRect) const
{
	float w = borderRect.right - borderRect.left;
	float h = borderRect.bottom - borderRect.top;
	float base = min(w, h);   // 百分比基准

	auto convert = [base](float value) -> float {
		if (value > 0.0f && value <= 1.0f)
			return value * base;    // 百分比
		else
			return value;           // 像素
	};

	CD2DRadius result;
	result.m_topLeft     = convert(radius.m_topLeft);
	result.m_topRight    = convert(radius.m_topRight);
	result.m_bottomRight = convert(radius.m_bottomRight);
	result.m_bottomLeft  = convert(radius.m_bottomLeft);
	return result;
}


// ========== 盒模型区域 ==========
D2D1_RECT_F CD2DControlUI::GetMarginRectangle() const {
	CD2DStyle style = CalculateEffectiveStyle();
	D2D1_RECT_F rect = GetBorderRectangle();
	rect.left   -= style.margin.left;
	rect.top    -= style.margin.top;
	rect.right  += style.margin.right;
	rect.bottom += style.margin.bottom;
	return rect;
}

D2D1_RECT_F CD2DControlUI::GetBorderRectangle() const {
	CD2DStyle style = CalculateEffectiveStyle();
	D2D1_RECT_F rect = GetPaddingRectangle();
	rect.left   -= style.border.m_left.m_width;
	rect.top    -= style.border.m_top.m_width;
	rect.right  += style.border.m_right.m_width;
	rect.bottom += style.border.m_bottom.m_width;
	
	return rect;
}

D2D1_RECT_F CD2DControlUI::GetPaddingRectangle() const {
	CD2DStyle style = CalculateEffectiveStyle();
	D2D1_RECT_F rect = GetContentRectangle(); // 内容矩形
	rect.left   -= style.padding.left;
	rect.top    -= style.padding.top;
	rect.right  += style.padding.right;
	rect.bottom += style.padding.bottom;
	return rect;
}

D2D1_RECT_F CD2DControlUI::GetContentRectangle() const {
	CD2DStyle style = CalculateEffectiveStyle();
	D2D1_RECT_F rect = m_rectangle; // 内容矩形
	rect = style.transform.TransformRectangle(rect);
	return rect; // 直接返回内容矩形
}

D2D1_SIZE_F CD2DControlUI::GetNonContentSize() const
{
	CD2DStyle style = CalculateEffectiveStyle();

	float hTotal = style.margin.left + style.margin.right
		+ style.border.m_left.m_width + style.border.m_right.m_width
		+ style.padding.left + style.padding.right;

	float vTotal = style.margin.top + style.margin.bottom
		+ style.border.m_top.m_width + style.border.m_bottom.m_width
		+ style.padding.top + style.padding.bottom;

	return D2D1::SizeF(hTotal, vTotal);
}

D2D1_RECT_F CD2DControlUI::GetInnerBorderRect(const CD2DStyle& style, const D2D1_RECT_F& borderRect) const
{
	float t = style.border.m_top.m_width;
	float r = style.border.m_right.m_width;
	float b = style.border.m_bottom.m_width;
	float l = style.border.m_left.m_width;

	return D2D1::RectF(
		borderRect.left   + l * 0.5f,
		borderRect.top    + t * 0.5f,
		borderRect.right  - r * 0.5f,
		borderRect.bottom - b * 0.5f
		);
}

// 重写命中测试，使用边框矩形（包含 margin/border/padding）
bool CD2DControlUI::IsHitTest(const D2D1_POINT_2F& point) const
{
	D2D1_RECT_F rect = GetBorderRectangle();
	return (point.x >= rect.left && point.x <= rect.right &&
		point.y >= rect.top  && point.y <= rect.bottom);
}

void CD2DControlUI::TriggerEvent(CD2DEvent::EType type)
{
	CD2DEvent event;
	if (m_eventSlots.Lookup(type, event))
		event(this);
}

void CD2DControlUI::SetText(const CAtlString& text)
{
	if (m_text != text)
	{
		m_text = text;
		InvalidateContent();   // 内容变化，标记脏
	}
}

D2D1_SIZE_F CD2DControlUI::MeasureText(CD2DRender* pRender, float maxWidth , float maxHeight ) const
{
	if (!pRender || m_text.IsEmpty()) return D2D1::SizeF(0, 0);
	CD2DStyle style = CalculateEffectiveStyle();
	return pRender->MeasureText(m_text, style.text, maxWidth, maxHeight);
}

D2D1_SIZE_F CD2DControlUI::MeasureContent(CD2DRender* pRender, float maxWidth, float maxHeight) const
{
	// 纯文本测量，不包含盒模型
	if (!m_text.IsEmpty())
		return MeasureText(pRender, maxWidth);
	// 无文本时回退到基类默认（返回当前控件尺寸）
	return CD2DControlBaseUI::MeasureContent(pRender, maxWidth, maxHeight);
}


// ========== 绘制入口 ==========
void CD2DControlUI::DrawControl(CD2DRender* pRender)
{
	if (!pRender) return;
	ID2D1RenderTarget* pRT = pRender->GetRenderTarget();
	if (!pRT) return;

	CD2DStyle style = CalculateEffectiveStyle();
	D2D1_RECT_F borderRect = GetBorderRectangle();
	D2D1_RECT_F contentRect = GetContentRectangle();
	D2D1_RECT_F marginRect = GetMarginRectangle();

	DrawBackground(style, borderRect, pRender);
	DrawBorder(style, borderRect, pRender);
	CD2DStyle style1 = style;
	style1.border.m_top.m_color = D2D1::ColorF(D2D1::ColorF::Red);
	DrawBorder(style1, marginRect, pRender);


	if (!m_text.IsEmpty())
	{
		D2D1_RECT_F contentRect = GetContentRectangle();
		DrawText(style, contentRect, pRender);   // 在最后绘制文字
	}
}

// ========== 背景绘制 ==========
void CD2DControlUI::DrawBackground(const CD2DStyle& style, const D2D1_RECT_F& borderRect, CD2DRender* pRender)
{
	ID2D1RenderTarget* pRT = pRender->GetRenderTarget();
	if (!pRT || style.background.m_type == CD2DBackground::None)
		return;

	CComPtr<ID2D1Brush> spBrush = pRender->CreateBackgroundBrush(style.background, borderRect);
	if (!spBrush) return;

	CComPtr<ID2D1PathGeometry> spClipGeom = pRender->CreateRoundRectGeometry(
		borderRect,
		GetEffectiveRadius(style.radius, borderRect)
		);
	if (spClipGeom)
	{
		pRT->FillGeometry(spClipGeom, spBrush);
	}
	else
	{
		pRT->FillRectangle(&borderRect, spBrush);
	}
}

// ========== 背景边框 ==========
void CD2DControlUI::DrawBorder(const CD2DStyle& style, const D2D1_RECT_F& borderRect, CD2DRender* pRender)
{
	if (!pRender) return;
	ID2D1RenderTarget* pRT = pRender->GetRenderTarget();
	if (!pRT) return;

	// 简单起见，若四边样式完全相同则整体绘制，否则分边绘制（此处沿用原逻辑）
	bool allSame = (IsBorderEdgeSet(style.border.m_top) &&
		IsBorderEdgeSet(style.border.m_right) &&
		IsBorderEdgeSet(style.border.m_bottom) &&
		IsBorderEdgeSet(style.border.m_left));
	if (!allSame)
	{
		// 获取内矩形（边框线条中心线位置）
		D2D1_RECT_F inner = GetInnerBorderRect(style, borderRect);

		// 为消除角部缺口，将线条端点适当延伸（延伸半个邻边宽度）
		float t = style.border.m_top.m_width;
		float r = style.border.m_right.m_width;
		float b = style.border.m_bottom.m_width;
		float l = style.border.m_left.m_width;

		float extTop    = r * 0.5f;
		float extRight  = b * 0.5f;
		float extBottom = l * 0.5f;
		float extLeft   = t * 0.5f;

		DrawBorderEdge(style.border.m_top,
			D2D1::Point2F(inner.left - extLeft, inner.top),
			D2D1::Point2F(inner.right + extTop, inner.top),
			pRender);
		DrawBorderEdge(style.border.m_right,
			D2D1::Point2F(inner.right, inner.top - extTop),
			D2D1::Point2F(inner.right, inner.bottom + extRight),
			pRender);
		DrawBorderEdge(style.border.m_bottom,
			D2D1::Point2F(inner.right + extBottom, inner.bottom),
			D2D1::Point2F(inner.left - extRight, inner.bottom),
			pRender);
		DrawBorderEdge(style.border.m_left,
			D2D1::Point2F(inner.left, inner.bottom + extRight),
			D2D1::Point2F(inner.left, inner.top - extBottom),
			pRender);
	}
	else
	{
		// 圆角矩形：也使用内缩矩形，使边框外沿对齐 borderRect
		D2D1_RECT_F inner = GetInnerBorderRect(style, borderRect);
		CComPtr<ID2D1PathGeometry> spGeom = pRender->CreateRoundRectGeometry(borderRect, GetEffectiveRadius(style.radius, inner));
		if (!spGeom) return;

		const CD2DBorderEdge& firstEdge = style.border.m_top;
		if (firstEdge.m_style == None) return;

		CComPtr<ID2D1SolidColorBrush> spBrush;
		pRT->CreateSolidColorBrush(firstEdge.m_color, &spBrush);
		if (!spBrush) return;

		CComPtr<ID2D1StrokeStyle> spStroke = pRender->CreateStrokeStyle(firstEdge.m_style);
		pRT->DrawGeometry(spGeom, spBrush, firstEdge.m_width, spStroke);
	}
}
// ========== 边框绘制 ==========
void CD2DControlUI::DrawBorderEdge(const CD2DBorderEdge& edge,
	D2D1_POINT_2F start, D2D1_POINT_2F end, CD2DRender* pRender)
{
	ID2D1RenderTarget* pRT = pRender->GetRenderTarget();
	if (edge.m_style == None || edge.m_width <= 0.0f) return;

	CComPtr<ID2D1SolidColorBrush> spBrush;
	pRT->CreateSolidColorBrush(edge.m_color, &spBrush);
	if (!spBrush) return;

	CComPtr<ID2D1StrokeStyle> spStroke = pRender->CreateStrokeStyle(edge.m_style);
	pRT->DrawLine(start, end, spBrush, edge.m_width, spStroke);
}

void CD2DControlUI::DrawText(const CD2DStyle& style, const D2D1_RECT_F& contentRect, CD2DRender* pRender)
{
	ID2D1RenderTarget* pRT = pRender->GetRenderTarget();
	IDWriteFactory* pDWriteFactory = pRender->GetWriteFactory();
	if (!pRT || !pDWriteFactory) return;

	// 创建画刷
	CComPtr<ID2D1SolidColorBrush> spBrush;
	pRT->CreateSolidColorBrush(style.text.color, &spBrush);
	if (!spBrush) return;

	// 获取 TextFormat（带缓存）
	CComPtr<IDWriteTextFormat> spFormat = pRender->CreateTextFormat(style.text);
	if (!spFormat) return;

	// 绘制文本（自动应用对齐方式）
	pRT->DrawText(
		m_text.GetString(),
		m_text.GetLength(),
		spFormat,
		contentRect,
		spBrush,
		D2D1_DRAW_TEXT_OPTIONS_NONE);
	spFormat.Detach();
}