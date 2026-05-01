// D2DImageLoader.h
#pragma once
 
#include <atlcomcli.h>
#include <wincodec.h>
#include <d2d1.h>

class CD2DImageLoader
{
public:
    CD2DImageLoader();
    ~CD2DImageLoader();

    // 禁止拷贝
    CD2DImageLoader(const CD2DImageLoader&) ;
    CD2DImageLoader& operator=(const CD2DImageLoader&) ;

    // 从文件加载图片（支持 PNG、JPEG 等 WIC 支持的格式）
    bool LoadFile(const wchar_t* szFilePath);

    // 从 WIC 位图创建 D2D 位图（需要传入渲染目标）
    // 注意：调用者负责释放返回的 ID2D1Bitmap
    HRESULT CreateD2DBitmap(ID2D1RenderTarget* pRT, ID2D1Bitmap** ppBitmap) const;

    // 获取原始图像尺寸
    UINT GetWidth() const  { return m_width; }
    UINT GetHeight() const { return m_height; }

    // 获取 WIC 格式转换器（可用于创建位图，但推荐使用上面的 CreateD2DBitmap）
    IWICFormatConverter* GetFormatConverter() const { return m_pConverter; }

private:
    CComPtr<IWICImagingFactory>    m_pWICFactory;   // WIC 工厂
    CComPtr<IWICBitmapDecoder>     m_pDecoder;      // 解码器
    CComPtr<IWICBitmapFrameDecode> m_pFrame;        // 图像帧
    CComPtr<IWICFormatConverter>   m_pConverter;    // 格式转换器（转为 32bppPBGRA）
    UINT                           m_width;         // 图像宽度
    UINT                           m_height;        // 图像高度
};