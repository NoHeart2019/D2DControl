#include "stdafx.h"
// CD2DTextArea.cpp
#include "D2DTextArea.h"

CD2DTextArea::CD2DTextArea()
	: CD2DControlUI()
	, m_scrollOffsetY(0.0f)
	, m_textContentHeight(0.0f)
	, m_wordWrap(true)
	, m_scrollBarVisible(false)
{
	m_VScroll.SetOrientation(CD2DScrollUI::Vertical);
	m_VScroll.SetVisible(false);

	CD2DScrollStyle ss;
	ss.thumb.background.type = CD2DBackground::Solid;
	ss.thumb.background.solid.color = D2D1::ColorF(0.6f, 0.6f, 0.6f);
	ss.thumb.radius = CD2DRadius(4.0f);
	ss.track.background.type = CD2DBackground::Solid;
	ss.track.background.solid.color = D2D1::ColorF(0.9f, 0.9f, 0.9f);
	m_VScroll.SetScrollStyle(ED2DStatus::Normal, ss);

}

CD2DTextArea::~CD2DTextArea() {};

void CD2DTextArea::SetText(const CAtlString& text)
{
	if (m_text != text)
	{
		m_text = text;
		m_scrollOffsetY = 0.0f;   // 文本变化时重置滚动位置
		InvalidateContent();
	}
}

const CAtlString& CD2DTextArea::GetText() const { return m_text; }

void CD2DTextArea::SetWordWrap(bool wrap) { m_wordWrap = wrap; }

bool CD2DTextArea::IsWordWrap() const { return m_wordWrap; }

// ==================== 绘制与滚动更新 ====================
void CD2DTextArea::DrawControl(CD2DRender* pRender)
{
	if (!IsVisible() || !pRender) return;
	ID2D1RenderTarget* pRT = pRender->GetRenderTarget();
	if (!pRT) return;

	CD2DStyle style = CalculateEffectiveStyle();
	D2D1_RECT_F borderRect = GetBorderRectangle();
	D2D1_RECT_F contentRect = GetContentRectangle();

	// 绘制背景和边框
	DrawBackground(style, borderRect, pRender);
	DrawBorder(style, borderRect, pRender);

	// 计算文本所需高度，并更新滚动条状态
	UpdateScrollState(pRender);

	// 文本绘制区域（扣除滚动条）
	float scrollBarW = m_scrollBarVisible ? max(10.0f, m_VScroll.GetDesiredSize(nullptr).width) : 0.0f;
	D2D1_RECT_F textRect = contentRect;
	textRect.right -= scrollBarW;

	if (!m_text.IsEmpty())
	{
		CD2DTextStyle textStyle = style.text;
		textStyle.wordWrap = m_wordWrap;

		CComPtr<IDWriteTextFormat> spFormat = pRender->CreateTextFormat(textStyle);
		if (spFormat)
		{
			IDWriteFactory* pDW = pRender->GetWriteFactory();
			CComPtr<IDWriteTextLayout> spLayout;
			float layoutHeight =  
				((style.text.verticalAlign != DWRITE_PARAGRAPH_ALIGNMENT_NEAR) 
				? GetHeight() 
				: FLT_MAX);
			pDW->CreateTextLayout(
				m_text.GetString(), (UINT32)m_text.GetLength(),
				spFormat,
				textRect.right - textRect.left,   // 布局宽度
				layoutHeight,                           // 高度无限
				&spLayout
				);

			if (spLayout)
			{
				// spLayout 为 IDWriteTextLayout*
				float lineH = CalculateActualLineHeight(style.text);

				
				spLayout->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM, lineH, lineH * 0.8f);
				spLayout->SetTextAlignment(textStyle.horizontalAlign);
				spLayout->SetParagraphAlignment(textStyle.verticalAlign);

				// 裁剪到文本区域，并应用滚动偏移
				pRT->PushAxisAlignedClip(textRect, D2D1_ANTIALIAS_MODE_ALIASED);

				D2D1_MATRIX_3X2_F oldTransform;
				pRT->GetTransform(&oldTransform);
				pRT->SetTransform(oldTransform*D2D1::Matrix3x2F::Translation(0.0f, -m_scrollOffsetY));

				ID2D1SolidColorBrush* pBrush = nullptr;
				pRT->CreateSolidColorBrush(textStyle.color, &pBrush);
				if (pBrush)
				{
					pRT->DrawTextLayout(
						D2D1::Point2F(textRect.left, contentRect.top),  // 从内容顶部开始绘制
						spLayout,
						pBrush,
						D2D1_DRAW_TEXT_OPTIONS_CLIP
						);
					pBrush->Release();
				}

				pRT->SetTransform(oldTransform);
				pRT->PopAxisAlignedClip();
			}
		}
	}

	// 在最上层绘制滚动条
	if (m_scrollBarVisible)
		m_VScroll.DrawControl(pRender);
}

void CD2DTextArea::UpdateScrollState(CD2DRender* pRender)
{
	if (!pRender) return;

	D2D1_RECT_F contentRect = GetContentRectangle();
	float viewportHeight = contentRect.bottom - contentRect.top;
	float textWidth = contentRect.right - contentRect.left;

	// 先假设滚动条可见，计算文本宽度
	float scrollBarW = m_scrollBarVisible ? max(10.0f, m_VScroll.GetDesiredSize(nullptr).width) : 0.0f;
	float availTextWidth = textWidth - scrollBarW;
	if (availTextWidth < 0.0f) availTextWidth = 0.0f;

	// 获取文本总高度
	m_textContentHeight = GetTextContentHeight(pRender, availTextWidth);

	// 决定是否需要滚动条
	bool needScroll = m_textContentHeight > viewportHeight;
	if (needScroll != m_scrollBarVisible)
	{
		m_scrollBarVisible = needScroll;
		if (!needScroll) m_scrollOffsetY = 0.0f;
	}

	if (m_scrollBarVisible)
	{
		// 重新计算滚动条宽度（可能因可见性变化而改变）
		scrollBarW = max(10.0f, m_VScroll.GetDesiredSize(nullptr).width);
		D2D1_RECT_F scrollRect = D2D1::RectF(
			contentRect.right - scrollBarW,
			contentRect.top,
			contentRect.right,
			contentRect.bottom
			);
		m_VScroll.SetRectangle(scrollRect);

		// ===== 关键修改：使用总高度作为范围，pageSize 用于限制 =====
		int totalH = static_cast<int>(m_textContentHeight);
		m_VScroll.SetRange(0, totalH);                              // 最大值为内容总高度
		m_VScroll.SetPageSize(static_cast<int>(viewportHeight));    // 视口高度决定滑块大小
		m_VScroll.SetValue(static_cast<int>(m_scrollOffsetY));
		m_VScroll.SetVisible(true);

	}
	else
	{
		m_VScroll.SetVisible(false);
	}
}

float CD2DTextArea::GetTextContentHeight(CD2DRender* pRender, float contentWidth) const
{
	if (m_text.IsEmpty() || !pRender) return 0.0f;
	IDWriteFactory* pDW = pRender->GetWriteFactory();
	if (!pDW) return 0.0f;

	CD2DStyle style = CalculateEffectiveStyle();
	
	CD2DTextStyle textStyle = style.text;
	textStyle.wordWrap = m_wordWrap;
	float layoutHeight =  
		 ((style.text.verticalAlign != DWRITE_PARAGRAPH_ALIGNMENT_NEAR) 
		? GetHeight() 
		: FLT_MAX);
	CComPtr<IDWriteTextFormat> spFormat = pRender->CreateTextFormat(textStyle);
	if (!spFormat) return 0.0f;

	CComPtr<IDWriteTextLayout> spLayout;
	pDW->CreateTextLayout(
		m_text.GetString(), (UINT32)m_text.GetLength(),
		spFormat,
		contentWidth,
		layoutHeight,
		&spLayout
		);
	if (!spLayout) return 0.0f;

	DWRITE_TEXT_METRICS metrics;
	spLayout->GetMetrics(&metrics);
	return metrics.height;
}

// ==================== 鼠标事件 ====================
bool CD2DTextArea::OnMouseDownL(const D2D1_POINT_2F& point)
{
	if (m_scrollBarVisible && m_VScroll.IsHitTest(point))
	{
		bool handled = m_VScroll.OnMouseDownL(point);
		m_scrollOffsetY = static_cast<float>(m_VScroll.GetValue());
		return handled;
	}
	return CD2DControlUI::OnMouseDownL(point);
}

bool CD2DTextArea::OnMouseMove(const D2D1_POINT_2F& point)
{
	if (m_scrollBarVisible && (m_VScroll.IsDragging() || m_VScroll.IsHitTest(point)))
	{
		bool handled = m_VScroll.OnMouseMove(point);
		m_scrollOffsetY = static_cast<float>(m_VScroll.GetValue());
		return handled;
	}
	return CD2DControlUI::OnMouseMove(point);
}

bool CD2DTextArea::OnMouseUpL(const D2D1_POINT_2F& point)
{
	if (m_scrollBarVisible && m_VScroll.IsPressed())
	{
		bool handled = m_VScroll.OnMouseUpL(point);
		m_scrollOffsetY = static_cast<float>(m_VScroll.GetValue());
		return handled;
	}
	return CD2DControlUI::OnMouseUpL(point);
}

bool CD2DTextArea::OnMouseLeave(const D2D1_POINT_2F& point)
{
	if (m_scrollBarVisible)
		m_VScroll.OnMouseLeave(point);
	return CD2DControlUI::OnMouseLeave(point);
}

bool CD2DTextArea::OnMouseWheel(float delta, const D2D1_POINT_2F&)
{
	if (!m_scrollBarVisible) return false;

	D2D1_RECT_F contentRect = GetContentRectangle();
	float viewH = contentRect.bottom - contentRect.top;
	float maxOffset = max(0.0f, m_textContentHeight - viewH);

	float step = 20.0f; // 每次滚动的像素
	float newOffset = m_scrollOffsetY - (delta > 0 ? step : -step); // delta 正数向上滚动
	m_scrollOffsetY = max(0.0f, min(newOffset, maxOffset));
	m_VScroll.SetValue(static_cast<int>(m_scrollOffsetY));
	return true;
}