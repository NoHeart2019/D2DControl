// IconUI.h
#pragma once

#include "D2DImageLoader.h"

class CD2DIconUI : public CD2DControlUI, public CD2DTextFormat
{
public:
    CD2DIconUI();
    virtual ~CD2DIconUI();

    // 加载 PNG 图片
    bool LoadImage(const wchar_t* imagePath, CD2DRender* pRender);

    // 绘制
    virtual void Render(CD2DRender* pRender) override;

    // 鼠标事件（仅处理点击）
    virtual bool OnMouseDownL(const D2D1_POINT_2F& point) override;
    virtual bool OnMouseUpL(const D2D1_POINT_2F& point) override;

    // 点击回调类型：参数为控件指针和用户数据
    typedef void (*IconClickCallback)(CD2DIconUI* pIcon, void* userData);
    void SetOnClickCallback(IconClickCallback callback, void* userData = nullptr);

    // 获取原始图像尺寸
    D2D1_SIZE_F GetOriginalImageSize() const;

protected:
    // 计算图像实际绘制矩形（基于控件内容区，保持宽高比）
    D2D1_RECT_F CalculateImageRect() const;

    CComPtr<ID2D1Bitmap> m_pBitmap;   // D2D 位图
    D2D1_SIZE_F          m_imageSize; // 原始图像尺寸

    IconClickCallback    m_clickCallback; // 点击回调
};