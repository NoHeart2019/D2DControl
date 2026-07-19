// CD2DIcon.cpp
#include "stdafx.h"
#include "D2DIcon.h"

CD2DIcon::CD2DIcon()
	: CD2DControlUI()
	, m_scaleMode(IconScaleMode::Uniform)
{
	// 初始策略设置为 AutoSize，会根据图片和 DPI 动态确定
	SetWidthPolicy(CD2DSizePolicy::AutoSize());
	SetHeightPolicy(CD2DSizePolicy::AutoSize());
}

CD2DIcon::~CD2DIcon()
{
}

bool CD2DIcon::LoadFromFile(const CAtlString& filePath)
{
	return m_image.LoadFromFile(filePath);
}

bool CD2DIcon::LoadFromResource(HMODULE hModule, const CAtlString& resName, const CAtlString& resType)
{
	return m_image.LoadFromResource(hModule, resName, resType);
}

// ---------- 布局与测量 ----------
void CD2DIcon::Layout()
{
	// 如果图片有效，确保内容尺寸与图片原始像素（考虑 DPI）一致
	if (m_image.IsValid())
	{
		D2D1_SIZE_U sz = m_image.GetPixelSize();
		float dpiScaleX = 1.0f, dpiScaleY = 1.0f;

		// Layout 没有 Render 参数，但可以通过缓存的方式获取 DPI（如果之前已存储）
		// 这里简单采用无 Render 时的默认值（96 DPI），绘制时会在 DrawControl 中再次矫正
		float contentW = static_cast<float>(sz.width) / dpiScaleX;
		float contentH = static_cast<float>(sz.height) / dpiScaleY;

		// 如果当前尺寸与期望不同，强制修正（保证不失真）
		if (fabs(GetWidth() - contentW) > 0.1f || fabs(GetHeight() - contentH) > 0.1f)
		{
			SetWidth(contentW);
			SetHeight(contentH);
		}
	}
}

D2D1_SIZE_F CD2DIcon::MeasureContent(CD2DRender* pRender, float maxWidth, float maxHeight) const
{
	D2D1_SIZE_U sz = m_image.GetPixelSize();
	if (sz.width == 0 || sz.height == 0)
		return CD2DControlUI::MeasureContent(pRender, maxWidth, maxHeight);

	float dpiScaleX = 1.0f, dpiScaleY = 1.0f;
	if (pRender)
		pRender->GetDpiScale(&dpiScaleX, &dpiScaleY);

	float contentW = static_cast<float>(sz.width) / dpiScaleX;
	float contentH = static_cast<float>(sz.height) / dpiScaleY;

	return D2D1::SizeF(contentW, contentH);
}
// ---------- 绘制 ----------
void CD2DIcon::DrawControl(CD2DRender* pRender)
{
	if (!IsVisible() || !pRender) return;
	ID2D1RenderTarget* pRT = pRender->GetRenderTarget();
	if (!pRT) return;

	CD2DStyle style = CalculateEffectiveStyle();
	D2D1_RECT_F borderRect = GetBorderRectangle();
	DrawBackground(style, borderRect, pRender);
	DrawBorder(style, borderRect, pRender);

	CComPtr<ID2D1Bitmap> spBitmap = m_image.GetBitmap(pRT);
	if (!spBitmap) return;

	D2D1_SIZE_U imageSize = m_image.GetPixelSize();
	if (imageSize.width == 0 || imageSize.height == 0) return;

	// 获取 DPI 缩放因子
	float dpiScaleX = 1.0f, dpiScaleY = 1.0f;
	if (pRender)
		pRender->GetDpiScale(&dpiScaleX, &dpiScaleY);

	// 将像素尺寸转换为 DIP（设备无关像素）
	D2D1_SIZE_F imageDIP;
	imageDIP.width = static_cast<float>(imageSize.width) / dpiScaleX;
	imageDIP.height = static_cast<float>(imageSize.height) / dpiScaleY;

	D2D1_RECT_F contentRect = GetContentRectangle();

	// 根据缩放模式和 DIP 尺寸计算目标矩形
	D2D1_RECT_F destRect = CalculateDestRect(imageDIP, contentRect, m_scaleMode);

	// 绘制（如果目标矩形超出内容区域，可能需要裁剪）
	if (m_scaleMode == IconScaleMode::UniformToFill || m_scaleMode == IconScaleMode::Stretch)
	{
		pRT->PushAxisAlignedClip(contentRect, D2D1_ANTIALIAS_MODE_ALIASED);
		pRT->DrawBitmap(spBitmap, destRect);
		pRT->PopAxisAlignedClip();
	}
	else
	{
		pRT->DrawBitmap(spBitmap, destRect);
	}
}

D2D1_RECT_F CD2DIcon::CalculateDestRect(const D2D1_SIZE_F& imageDIP, const D2D1_RECT_F& contentRect, IconScaleMode mode) const
{
	float imgW = imageDIP.width;
	float imgH = imageDIP.height;
	float rectW = contentRect.right - contentRect.left;
	float rectH = contentRect.bottom - contentRect.top;

	if (imgW <= 0.0f || imgH <= 0.0f)
		return contentRect;

	switch (mode)
	{
	case IconScaleMode::Stretch:
		return contentRect;

	case IconScaleMode::Uniform:
		{
			float scale = min(rectW / imgW, rectH / imgH);
			float w = imgW * scale;
			float h = imgH * scale;
			float x = contentRect.left + (rectW - w) * 0.5f;
			float y = contentRect.top + (rectH - h) * 0.5f;
			return D2D1::RectF(x, y, x + w, y + h);
		}

	case IconScaleMode::UniformToFill:
		{
			float scale = max(rectW / imgW, rectH / imgH);
			float w = imgW * scale;
			float h = imgH * scale;
			float x = contentRect.left + (rectW - w) * 0.5f;
			float y = contentRect.top + (rectH - h) * 0.5f;
			return D2D1::RectF(x, y, x + w, y + h);
		}

	case IconScaleMode::None:
	default:
		// 原始尺寸，放置在内容区域左上角（或居中，可调整）
		float x = contentRect.left;
		float y = contentRect.top;
		return D2D1::RectF(x, y, x + imgW, y + imgH);
	}
}