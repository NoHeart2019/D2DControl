// CD2DIcon.h
#pragma once
#include "D2DControlUI.h"
#include "D2DImage.h"


class CD2DIcon : public CD2DControlUI
{
public:
	enum  IconScaleMode
	{
		Stretch,        // 拉伸填满内容矩形
		Uniform,        // 等比缩放，全部可见（留白）
		UniformToFill,  // 等比缩放，填满矩形（可能裁剪）
		None            // 原始大小，不缩放
	};
public:
	CD2DIcon();
	virtual ~CD2DIcon();

	// 加载图像
	bool LoadFromFile(const CAtlString& filePath);
	bool LoadFromResource(HMODULE hModule, const CAtlString& resName, const CAtlString& resType);

	// 缩放模式
	void SetScaleMode(IconScaleMode mode) { m_scaleMode = mode; Layout(); }
	IconScaleMode GetScaleMode() const { return m_scaleMode; }

	// 重写基类
	virtual void DrawControl(CD2DRender* pRender) override;
	virtual D2D1_SIZE_F MeasureContent(CD2DRender* pRender, float maxWidth = FLT_MAX, float maxHeight = FLT_MAX) const;
	virtual void Layout() ;

protected:
	// 根据缩放模式计算目标矩形
	D2D1_RECT_F CalculateDestRect(const D2D1_SIZE_F& imageDIP, const D2D1_RECT_F& contentRect, IconScaleMode mode) const;

private:
	CD2DImage      m_image;
	IconScaleMode  m_scaleMode;
};