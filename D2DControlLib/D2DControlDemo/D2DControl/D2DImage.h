// CD2DImage.h
#pragma once
#include <d2d1.h>
#include <wincodec.h>
#include <atlcomcli.h>
#include <atlstr.h>

class CD2DImage
{
public:
	CD2DImage();
	~CD2DImage();

	// 从文件加载
	bool LoadFromFile(const CAtlString& filePath);
	// 从资源加载（如 IDB_PNG1）
	bool LoadFromResource(HMODULE hModule, const CAtlString& resName, const CAtlString& resType);

	// 获取与渲染目标兼容的位图（按需创建）
	CComPtr<ID2D1Bitmap> GetBitmap(ID2D1RenderTarget* pRT);

	// 图像原始像素尺寸
	D2D1_SIZE_U GetPixelSize() const { return m_pixelSize; }

	// 是否加载成功
	bool IsValid() const { return m_bValid; }

	// 释放位图缓存（设备丢失时调用）
	void ReleaseBitmap();

private:
	// 从 WIC 源完成公共初始化
	bool LoadFromWicSource(IWICBitmapSource* pSource);
	// 清空所有资源
	void Clear();

private:
	CComPtr<IWICBitmapSource> m_pWicSource;   // WIC 格式转换后的源
	CComPtr<ID2D1Bitmap>      m_pBitmap;      // 缓存的 D2D 位图
	CComPtr<ID2D1RenderTarget> m_pLastRT;   // 上次创建位图时使用的渲染目标;  
	D2D1_SIZE_U               m_pixelSize;     // 图像像素尺寸
	bool                      m_bValid;        // 是否加载成功
};