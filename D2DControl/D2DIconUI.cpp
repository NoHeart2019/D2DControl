// IconUI.cpp
#include "stdafx.h"

#include "D2D.h"
#include "D2DIconUI.h"

CD2DIconUI::CD2DIconUI()
    : m_imageSize(D2D1::SizeF())
    , m_clickCallback(nullptr)
{
}

CD2DIconUI::~CD2DIconUI()
{
    m_pBitmap.Release();
}

bool CD2DIconUI::LoadImage(const wchar_t* imagePath, CD2DRender* pRender)
{
    if (!imagePath || !pRender || !pRender->GetRenderTarget()) {
        return false;
    }
	
    // 释放旧资源
    m_pBitmap.Release();

    // 使用已有图像加载器加载 PNG
    CD2DImageLoader loader;
    if (loader.LoadFile(imagePath) == false) {
        return false;
    }

    m_imageSize.width  = static_cast<float>(loader.GetWidth());
    m_imageSize.height = static_cast<float>(loader.GetHeight());

    // 从 WIC 位图创建 D2D 位图

    HRESULT hr = pRender->GetRenderTarget()->CreateBitmapFromWicBitmap(
        loader.GetFormatConverter(),
        nullptr,
        &m_pBitmap
    );

    return SUCCEEDED(hr);
}

void CD2DIconUI::Render(CD2DRender* pRender)
{
    if (!pRender || !IsVisible() || !m_pBitmap) {
        return;
    }

    D2D1_RECT_F contentRect = GetContentRectangle();

    // 绘制背景（如果启用）
    if (IsBackgroundVisible()) {
        pRender->FillRectangle(contentRect, m_backgroundColor);
    }

    // 计算图像绘制区域
    D2D1_RECT_F imageRect = CalculateImageRect();

    // 裁剪到控件内容区域，避免图像超出
    pRender->PushAxisAlignedClip(contentRect);

    // 绘制图像
    pRender->GetRenderTarget()->DrawBitmap(
        m_pBitmap,
        imageRect,
        1.0f,                                 // 不透明度
        D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
        nullptr                               // 源矩形（全图）
    );

    pRender->PopAxisAlignedClip();

    // 绘制边框（如果启用）
    if (IsBorderVisible()) {
        pRender->DrawRectangle(contentRect, m_borderColor, m_borderWidth);
    }
}

D2D1_RECT_F CD2DIconUI::CalculateImageRect() const
{
    // 无有效图像时返回空矩形
    if (!m_pBitmap || m_imageSize.width <= 0.0f || m_imageSize.height <= 0.0f) {
        return D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f);
    }

    D2D1_RECT_F contentRect = GetContentRectangle();
    float contentW = contentRect.right - contentRect.left;
    float contentH = contentRect.bottom - contentRect.top;

    if (contentW <= 0.0f || contentH <= 0.0f) {
        return contentRect;   // 无效区域，直接返回原矩形
    }

    float imgW = m_imageSize.width;
    float imgH = m_imageSize.height;

    // 缩放比例：过大则缩放到完全显示，过小则保持原尺寸
    float scale = 1.0f;
    if (imgW > contentW || imgH > contentH) {
        float scaleX = contentW / imgW;
        float scaleY = contentH / imgH;
        scale = min(scaleX, scaleY);
    }

    float drawW = imgW * scale;
    float drawH = imgH * scale;

    // 居中偏移
    float offsetX = (contentW - drawW) * 0.5f;
    float offsetY = (contentH - drawH) * 0.5f;

    return D2D1::RectF(
        contentRect.left + offsetX,
        contentRect.top  + offsetY,
        contentRect.left + offsetX + drawW,
        contentRect.top  + offsetY + drawH
    );
}

bool CD2DIconUI::OnMouseDownL(const D2D1_POINT_2F& point)
{
    // 仅在控件启用、可见且点在内容区域内时，记录按下状态
    if (!IsEnabled() || !IsVisible())
        return false;

    if (IsPointInRect(point, GetContentRectangle())) {
		SetPressed(true);
        return true;
    }
    return false;
}

bool CD2DIconUI::OnMouseUpL(const D2D1_POINT_2F& point)
{
    if (!IsEnabled() || !IsVisible())
        return false;

    // 只有之前按下过，才可能触发点击
    if (!IsPressed()) {
       SetPressed(false);
        return false;
    }

	SetPressed(false);


    // 检查释放点是否在图像绘制区域内，若是则触发回调
    D2D1_RECT_F imageRect = CalculateImageRect();
    if (IsPointInRect(point, imageRect) && m_clickCallback) {
        m_clickCallback(this, m_userData);
        return true;
    }
    return false;
}

void CD2DIconUI::SetOnClickCallback(IconClickCallback callback, void* userData)
{
    m_clickCallback = callback;
    SetUserData(userData);
}

D2D1_SIZE_F CD2DIconUI::GetOriginalImageSize() const
{
    return m_imageSize;
}