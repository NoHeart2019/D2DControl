// D2DImageLoader.cpp
#include "stdafx.h"
#include "D2DImageLoader.h"
#include <wincodec.h>

#pragma comment(lib, "windowscodecs.lib")

CD2DImageLoader::CD2DImageLoader()
    : m_width(0)
    , m_height(0)
{
}

CD2DImageLoader::~CD2DImageLoader()
{
}

bool CD2DImageLoader::LoadFile(const wchar_t* szFilePath)
{
    if (!szFilePath)
        return false;

    HRESULT hr = S_OK;

    // 创建 WIC 工厂
    if (!m_pWICFactory) {
        hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                              CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));
        if (FAILED(hr))
            return false;
    }

    // 创建解码器
    hr = m_pWICFactory->CreateDecoderFromFilename(
        szFilePath,
        nullptr,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &m_pDecoder);
    if (FAILED(hr))
        return false;

    // 获取第一帧（多帧图像如 GIF 通常只取第一帧）
    hr = m_pDecoder->GetFrame(0, &m_pFrame);
    if (FAILED(hr))
        return false;

    // 获取图像尺寸
    hr = m_pFrame->GetSize(&m_width, &m_height);
    if (FAILED(hr))
        return false;

    // 创建格式转换器，转换为 32bppPBGRA（D2D 常用格式）
    hr = m_pWICFactory->CreateFormatConverter(&m_pConverter);
    if (FAILED(hr))
        return false;

    hr = m_pConverter->Initialize(
        m_pFrame,
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0f,
        WICBitmapPaletteTypeCustom);
    if (FAILED(hr))
        return false;

    return true;
}

HRESULT CD2DImageLoader::CreateD2DBitmap(ID2D1RenderTarget* pRT, ID2D1Bitmap** ppBitmap) const
{
    if (!pRT || !ppBitmap)
        return E_POINTER;
    if (!m_pConverter)
        return E_FAIL;

    return pRT->CreateBitmapFromWicBitmap(m_pConverter, nullptr, ppBitmap);
}