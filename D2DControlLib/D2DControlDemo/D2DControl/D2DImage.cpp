// CD2DImage.cpp
#include "stdafx.h"
#include "D2DImage.h"
#include <shlwapi.h>   // SHCreateMemStream

CD2DImage::CD2DImage()
	: m_pWicSource(nullptr)
	, m_pBitmap(nullptr)
	, m_pLastRT(nullptr)
	, m_pixelSize(D2D1::SizeU(0, 0))
, m_bValid(false)
{
}

CD2DImage::~CD2DImage()
{
	Clear();
}

bool CD2DImage::LoadFromFile(const CAtlString& filePath)
{
	Clear();

	CComPtr<IWICImagingFactory> spFactory;
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&spFactory));
	if (FAILED(hr)) return false;

	CComPtr<IWICBitmapDecoder> spDecoder;
	hr = spFactory->CreateDecoderFromFilename(filePath.GetString(), nullptr, GENERIC_READ,
		WICDecodeMetadataCacheOnDemand, &spDecoder);
	if (FAILED(hr)) return false;

	CComPtr<IWICBitmapFrameDecode> spFrame;
	hr = spDecoder->GetFrame(0, &spFrame);
	if (FAILED(hr)) return false;

	return LoadFromWicSource(spFrame);
}

bool CD2DImage::LoadFromResource(HMODULE hModule, const CAtlString& resName, const CAtlString& resType)
{
	Clear();

	HRSRC hRes = FindResource(hModule, resName.GetString(), resType.GetString());
	if (!hRes) return false;

	HGLOBAL hGlobal = LoadResource(hModule, hRes);
	if (!hGlobal) return false;

	DWORD dwSize = SizeofResource(hModule, hRes);
	LPVOID pData = LockResource(hGlobal);
	if (!pData) return false;

	IStream* pStream = SHCreateMemStream(static_cast<const BYTE*>(pData), dwSize);
	if (!pStream) return false;

	CComPtr<IWICImagingFactory> spFactory;
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&spFactory));
	if (FAILED(hr)) { pStream->Release(); return false; }

	CComPtr<IWICBitmapDecoder> spDecoder;
	hr = spFactory->CreateDecoderFromStream(pStream, nullptr, WICDecodeMetadataCacheOnDemand, &spDecoder);
	pStream->Release();
	if (FAILED(hr)) return false;

	CComPtr<IWICBitmapFrameDecode> spFrame;
	hr = spDecoder->GetFrame(0, &spFrame);
	if (FAILED(hr)) return false;

	return LoadFromWicSource(spFrame);
}

bool CD2DImage::LoadFromWicSource(IWICBitmapSource* pSource)
{
	CComPtr<IWICImagingFactory> spFactory;
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&spFactory));
	if (FAILED(hr)) return false;

	// 转换为 32bpp BGRA 格式
	CComPtr<IWICFormatConverter> spConverter;
	hr = spFactory->CreateFormatConverter(&spConverter);
	if (FAILED(hr)) return false;

	hr = spConverter->Initialize(pSource, GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeMedianCut);
	if (FAILED(hr)) return false;

	spConverter->GetSize(&m_pixelSize.width, &m_pixelSize.height);
	m_pWicSource = spConverter;
	m_bValid = true;
	return true;
}

CComPtr<ID2D1Bitmap> CD2DImage::GetBitmap(ID2D1RenderTarget* pRT)
{
	if (!m_bValid || !pRT) return nullptr;

	// 如果已有位图，且渲染目标相同，直接复用
	if (m_pBitmap && m_pLastRT == pRT)
		return m_pBitmap;

	// 否则释放旧位图，重新创建
	m_pBitmap.Release();
	m_pLastRT = pRT;
	HRESULT hr = pRT->CreateBitmapFromWicBitmap(m_pWicSource, nullptr, &m_pBitmap);
	if (FAILED(hr)) return nullptr;

	return m_pBitmap;
}

void CD2DImage::ReleaseBitmap()
{
	m_pBitmap.Release();
}

void CD2DImage::Clear()
{
	m_pBitmap.Release();
	m_pWicSource.Release();
	m_pLastRT.Release();
	m_pixelSize = D2D1::SizeU(0, 0);
	m_bValid = false;
}