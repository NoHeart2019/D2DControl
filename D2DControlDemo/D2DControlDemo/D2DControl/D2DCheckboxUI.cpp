// CD2DCheckboxUI.cpp
#include "stdafx.h"
#include "D2DCheckboxUI.h"
#include <algorithm>

CD2DCheckboxUI::CD2DCheckboxUI()
	: CD2DControlUI()
	, m_defaultBoxStyle()        // 使用默认构造
	, m_checkedCallback(nullptr)
	, m_pCallbackUserData(nullptr)
{
	// 默认尺寸策略：宽度自动，高度固定为 24（可覆写）
	SetWidthPolicy(CD2DSizePolicy::AutoSize());
	SetHeightPolicy(CD2DSizePolicy::FixedSize(24.0f));
}

CD2DCheckboxUI::~CD2DCheckboxUI()
{
}

void CD2DCheckboxUI::SetBoxStyle(ED2DStatus state, const CD2DCheckboxStyle& style)
{
	m_boxStyles.SetAt(state, style);
}

const CD2DCheckboxStyle& CD2DCheckboxUI::GetBoxStyle(ED2DStatus state) const
{
	const CAtlMap<ED2DStatus, CD2DCheckboxStyle>::CPair* pPair = m_boxStyles.Lookup(state);
	if (pPair)
		return pPair->m_value;
	// 未设置时返回默认样式
	return m_defaultBoxStyle;
}


ED2DStatus	CD2DCheckboxUI::GetEffectiveState() const          // 实现略
{
	return ED2DStatus::Normal;
}
// ========== 样式获取 ==========
CD2DCheckboxStyle CD2DCheckboxUI::GetEffectiveBoxStyle() const
{
	 CD2DStatus dd;
	 dd.SetFocused(true);
	// 起始为默认样式
	CD2DCheckboxStyle effective = m_defaultBoxStyle;

	// 再叠加当前控件实际状态（例如 Checked, Hover 等）
	ED2DStatus currentState = m_status.GetEffectiveState();
	if (m_status.IsNormal())     ApplyCheckBoxStyle(effective, GetBoxStyle(ED2DStatus::Normal));
	if (m_status.IsHover())      ApplyCheckBoxStyle(effective, GetBoxStyle(ED2DStatus::Hover));
	if (m_status.IsChecked())    ApplyCheckBoxStyle(effective, GetBoxStyle(ED2DStatus::Checked));
	
	return effective;
	
}

void CD2DCheckboxUI::ApplyCheckBoxStyle(CD2DCheckboxStyle& dest, const CD2DCheckboxStyle& src) const
{
	ApplyStyle(dest.box, src.box);
	if (IsTextStyleSet(src.text))
		dest.text = src.text;
	if (src.checkmark.a > 0.0f)
		dest.checkmark = src.checkmark;
}


void CD2DCheckboxUI::SetOnCheckedChanged(CheckedChangedCallback cb, void* pUserData)
{
	m_checkedCallback = cb;
	m_pCallbackUserData = pUserData;
}

// ========== 布局与测量 ==========
void CD2DCheckboxUI::Layout()
{
	// 如果宽/高为 Auto，则根据内容计算理想尺寸
	bool autoW = (GetWidthPolicy().mode == CD2DSizePolicy::Auto);
	bool autoH = (GetHeightPolicy().mode == CD2DSizePolicy::Auto);

	if (autoW || autoH)
	{
		// 获取当前有效样式（用于计算文本占用的空间）
		const CD2DCheckboxStyle& boxStyle = GetEffectiveBoxStyle();

		float textW = 0.0f, textH = 0.0f;
		if (!m_text.IsEmpty())
		{
			// 测量文本尺寸（仅高度，宽度无限制）
			D2D1_SIZE_F textSize = MeasureText(nullptr, FLT_MAX);   // pRender为nullptr时内部会做保护，直接返回0
			textW = textSize.width;
			textH = textSize.height;
		}

		const float boxSize = 16.0f;   // 勾选框固定大小
		const float spacing = 6.0f;    // 勾选框与文本间距

		float idealW = boxSize + spacing + textW;
		float idealH = max(boxSize, textH);

		if (autoW)
			SetWidth(idealW);
		if (autoH)
			SetHeight(idealH);
	}
}

D2D1_SIZE_F CD2DCheckboxUI::MeasureContent(CD2DRender* pRender, float maxWidth, float maxHeight) const
{
	const CD2DCheckboxStyle& boxStyle = GetEffectiveBoxStyle();

	float textW = 0.0f, textH = 0.0f;
	if (!m_text.IsEmpty())
	{
		// 使用提供的 pRender 或内部的缓存（这里简单调用 MeasureText）
		D2D1_SIZE_F textSize = pRender->MeasureText(m_text, boxStyle.text, maxWidth, maxHeight);
		textW = textSize.width;
		textH = textSize.height;
	}

	const float boxSize = 16.0f;
	const float spacing = 6.0f;

	float w = boxSize + spacing + textW;
	float h = max(boxSize, textH);

	return D2D1::SizeF(w, h);
}

// ========== 绘制 ==========
void CD2DCheckboxUI::DrawControl(CD2DRender* pRender)
{
	if (!IsVisible() || !pRender) return;
	ID2D1RenderTarget* pRT = pRender->GetRenderTarget();
	if (!pRT) return;

	CD2DCheckboxStyle boxStyle = GetEffectiveBoxStyle();
	D2D1_RECT_F contentRect = GetContentRectangle();

	const float boxSize = 18.0f;
	const float spacing = 6.0f;

	// 勾选框矩形（垂直居中）
	D2D1_RECT_F checkRect;
	checkRect.left   = contentRect.left;
	checkRect.top    = contentRect.top + (contentRect.bottom - contentRect.top - boxSize) * 0.5f;
	checkRect.right  = checkRect.left + boxSize;
	checkRect.bottom = checkRect.top + boxSize;

	// 1. 绘制勾选框背景与边框
	DrawCheckboxFrame(pRender, checkRect, boxStyle.box);

	// 2. 如果选中，绘制对勾
	if (IsChecked())
		DrawCheckmark(pRender, checkRect, boxStyle.checkmark);

	// 3. 绘制文本
	if (!m_text.IsEmpty())
	{
		D2D1_RECT_F textRect;
		textRect.left   = checkRect.right + spacing;
		textRect.top    = checkRect.top;
		textRect.right  = contentRect.right;
		textRect.bottom = contentRect.bottom;

		pRender->DrawText(m_text, boxStyle.text, textRect);
	}
	
}

void CD2DCheckboxUI::DrawCheckboxFrame(CD2DRender* pRender, const D2D1_RECT_F& checkRect, const CD2DStyle& boxStyle)
{
	ID2D1RenderTarget* pRT = pRender->GetRenderTarget();
	if (!pRT) return;

	// 背景填充
	if (boxStyle.background.m_type != CD2DBackground::None)
	{
		CComPtr<ID2D1Brush> spBrush = pRender->CreateBackgroundBrush(boxStyle.background, checkRect);
		if (spBrush)
		{
			if (boxStyle.radius.m_topLeft > 0 || boxStyle.radius.m_topRight > 0 ||
				boxStyle.radius.m_bottomRight > 0 || boxStyle.radius.m_bottomLeft > 0)
			{
				CComPtr<ID2D1PathGeometry> spGeom = pRender->CreateRoundRectGeometry(checkRect, boxStyle.radius);
				if (spGeom)
					pRT->FillGeometry(spGeom, spBrush);
				else
					pRT->FillRectangle(checkRect, spBrush);
			}
			else
			{
				pRT->FillRectangle(checkRect, spBrush);
			}
		}
	}

	// 边框绘制（调用基类的 DrawBorder 方法，传入当前样式）
	DrawBorder(boxStyle, checkRect, pRender);
}

void CD2DCheckboxUI::DrawCheckmark(CD2DRender* pRender, const D2D1_RECT_F& checkRect, D2D1_COLOR_F color)
{
	ID2D1RenderTarget* pRT = pRender->GetRenderTarget();
	ID2D1Factory* pFactory = pRender->GetFactory();
	if (!pRT || !pFactory) return;

	float w = checkRect.right - checkRect.left;
	float h = checkRect.bottom - checkRect.top;

	// 对勾的四个关键点（起点 → 转折点1 → 转折点2 → 终点，近似平滑曲线）
	D2D1_POINT_2F pts[3] = {
		{ checkRect.left + w * 0.15f, checkRect.top + h * 0.55f },
		{ checkRect.left + w * 0.45f, checkRect.bottom - h * 0.15f },
		{ checkRect.right - w * 0.15f, checkRect.top + h * 0.25f }
	};

	// 创建画刷
	CComPtr<ID2D1SolidColorBrush> spBrush;
	pRT->CreateSolidColorBrush(color, &spBrush);
	if (!spBrush) return;

	// 创建路径几何
	CComPtr<ID2D1PathGeometry> spGeom = pRender->CreatePolylineGeometry(pts, 3);
	if (spGeom)
		pRT->DrawGeometry(spGeom, spBrush, 2.0f);

}

void CD2DCheckboxUI::NotifyCheckedChanged()
{
	if (m_checkedCallback)
		m_checkedCallback(this, IsChecked(), m_pCallbackUserData);
}