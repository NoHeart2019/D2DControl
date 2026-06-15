#pragma once
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>

BOOL InitC32D2D1();

VOID UnInitC32D2D1();

#define C32RGB(r,g,b)          ((COLORREF)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16)))
#define  rgba(r, g, b, a) D2D1::ColorF((float)r/255.0f, (float)g/255.0f, (float)b/255.0f, (float)a)
#define C32D2RGB(r, g, b) RGB(b, g, r)

#define C32Color2ColorF(Color1) \
	D2D1::ColorF((GetRValue(Color1) << 16 | GetGValue(Color1) << 8 | GetBValue(Color1)), 1.0f)


#define Rect2RectF(Rect)\
		 D2D1::RectF((FLOAT)Rect.left, (FLOAT)Rect.top, (FLOAT)Rect.right, (FLOAT)Rect.bottom)

#define RectF2Rect(Rt){(int)Rt.left, (int)Rt.top, (int)Rt.right, (int)Rt.bottom}

#define GetRectFWidth(Rect)(Rect.right - Rect.left)

#define GetRectFHeight(Rect)(Rect.bottom - Rect.top)

extern ID2D1Factory* m_D2D1Factory;
extern IDWriteFactory* m_D2D1WriteFactory;
extern IDWriteTextFormat* m_D2D1WriteTextFormat;
extern IWICImagingFactory* m_D2D1WICFactory;

/*
	@brief: 根据系统默认字体和字体号创建字体
*/
HRESULT C32D2D1CreateTextFormatSystem(FLOAT FontSize, IDWriteTextFormat** WirteTextFormat);


HRESULT C32D2D1GetTextSize(const WCHAR* text, IDWriteTextFormat* pTextFormat, D2D1_SIZE_F& size);



/*
	根据WriteTextFormat 和矩形RECT计算文字在矩形居中显示时的D2D1_RECT_F。
	Original :要在此矩形内居中显示文字。
	Center   :文字居中矩形坐标。


*/
HRESULT C32D2D1GetTextCenter(CString& Stirng, IDWriteTextFormat* pTextFormat, D2D1_RECT_F& Rect,__inout D2D1_RECT_F& Center);

class C32D2D1TextFormat
{

public:
	C32D2D1TextFormat()
	{

	}

	~C32D2D1TextFormat()
	{


	}
	
	VOID Initialize()
	{
		m_IsSystemFont = TRUE;
		m_IsColor = FALSE;
		m_IsFontBkGndColor = FALSE;
		m_IsFontColor	  = FALSE;
	}

	HRESULT CreateD2Font(LPCWSTR Local,LPCWSTR Family, FLOAT FontSize)
	{
		HRESULT Hr;
		m_IsSystemFont = FALSE;
		m_D2TextFormat.Release();
		Hr= m_D2D1WriteFactory->CreateTextFormat(
			Family,
			NULL,
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			FontSize,
			Local,
			&m_D2TextFormat
			);

		m_D2FontLocalName = Local;
		m_D2FontFamilyName = Family;
		m_D2FontSize = FontSize;
		
		return Hr;
	}
	
	BOOL SetFontBkGndColor(D2D1_COLOR_F Color)
	{
		m_IsColor = TRUE;		
		m_IsFontBkGndColor = TRUE;
		m_D2ColorFontBkGnd = Color;
	}

	BOOL SetFontColor(D2D1_COLOR_F Color)
	{
		m_IsColor = TRUE;
		m_IsFontColor = TRUE;
		m_D2ColorFontBkGnd = Color;
	}

public:
	BOOL		  m_IsSystemFont;		 //是否是系统字体
	BOOL		  m_IsColor;			 //是否使用颜色
	BOOL		  m_IsFontBkGndColor;   //是否字体背景色
	BOOL		  m_IsFontColor;		//是否字体颜色
			
	D2D1_COLOR_F  m_D2ColorFontBkGnd;//字体背景色
	D2D1_COLOR_F  m_D2ColorFont;     //字体颜色;

	FLOAT		  m_D2FontSize;      //字体大小
	CString		  m_D2FontFamilyName; //字体名称
	CString		  m_D2FontLocalName;  //区域名称
	
	CComPtr<IDWriteTextFormat> m_D2TextFormat;

};

// 获取矩形宽度
inline FLOAT RectFWidth(D2D1_RECT_F& rect) {
	return rect.right - rect.left;
}

// 获取矩形高度
inline FLOAT RectFHeight(D2D1_RECT_F& rect) {
	return rect.bottom - rect.top;
}

// 获取矩形四个点
inline D2D1_POINT_2F RectFLt(D2D1_RECT_F& rect) {
	return D2D1::Point2F(rect.left, rect.top);
}

inline D2D1_POINT_2F RectFRb(D2D1_RECT_F& rect) {
	return D2D1::Point2F(rect.right, rect.bottom);
}
inline D2D1_POINT_2F RectFLb(D2D1_RECT_F& rect) {
	return D2D1::Point2F(rect.left, rect.bottom);
}
inline D2D1_POINT_2F RectFRt(D2D1_RECT_F& rect) {
	return D2D1::Point2F(rect.right, rect.top);
}

//矩形中心点
inline D2D1_POINT_2F RectFCenter(D2D1_RECT_F rect)
{
	D2D1_POINT_2F point;
	point.x = (rect.left + rect.right) / 2.0f;
	point.y = (rect.top + rect.bottom) / 2.0f;

	return point;
}

// 判断矩形是否为空（面积为0或无效）
inline bool IsRectEmptyF(const D2D1_RECT_F& rect) {
	return (rect.left >= rect.right || rect.top >= rect.bottom);
}

// 从位置和大小创建矩形
inline D2D1_RECT_F RectFFromSize(FLOAT x, FLOAT y, FLOAT width, FLOAT height) {
	return D2D1::RectF(x, y, x + width, y + height);
}

// 从 D2D1_POINT_2F 和大小创建
inline D2D1_RECT_F RectFFromPointAndSize(const D2D1_POINT_2F& point, const D2D1_SIZE_F& size) {
	return D2D1::RectF(point.x, point.y, point.x + size.width, point.y + size.height);
}

// 判断两个矩形是否相交
inline bool IsRectFIntersect(const D2D1_RECT_F& rect1, const D2D1_RECT_F& rect2) {
	return !(rect1.left >= rect2.right ||
		rect1.right <= rect2.left ||
		rect1.top >= rect2.bottom ||
		rect1.bottom <= rect2.top);
}

// 获取两个矩形的交集，如果不相交则返回空矩形
inline D2D1_RECT_F RectIntersectF(const D2D1_RECT_F& rect1, const D2D1_RECT_F& rect2) {
	return IsRectFIntersect(rect1, rect2)
		? D2D1::RectF(
		max(rect1.left,   rect2.left),
		max(rect1.top,    rect2.top),
		min(rect1.right,  rect2.right),
		min(rect1.bottom, rect2.bottom)
		)
		: D2D1::RectF(0, 0, 0, 0); // 空矩形
}

inline D2D1_RECT_F RectUnionF(const D2D1_RECT_F& rect1, const D2D1_RECT_F& rect2) {
	return D2D1::RectF(
		min(rect1.left,   rect2.left),
		min(rect1.top,    rect2.top),
		max(rect1.right,  rect2.right),
		max(rect1.bottom, rect2.bottom)
		);
}

// 平移矩形
inline void OffsetRectF(D2D1_RECT_F& rect, FLOAT dx, FLOAT dy) {
	rect.left   += dx;
	rect.top    += dy;
	rect.right  += dx;
	rect.bottom += dy;
}

inline void OffsetRectFCenter(D2D1_RECT_F& sourceRect, const D2D1_RECT_F& targetRect) {
	// 计算源矩形的宽度和高度
	FLOAT width = sourceRect.right - sourceRect.left;
	FLOAT height = sourceRect.bottom - sourceRect.top;

	// 计算目标矩形的中心点
	FLOAT centerX = (targetRect.left + targetRect.right) * 0.5f;
	FLOAT centerY = (targetRect.top + targetRect.bottom) * 0.5f;

	// 计算新矩形的左上角坐标，使其以目标中心为中心
	FLOAT newLeft = centerX - (width * 0.5f);
	FLOAT newTop = centerY - (height * 0.5f);

	// 构造并返回新的矩形
	
	sourceRect.left = newLeft;			// left
	sourceRect.top =  newTop;			// top
	sourceRect.right= newLeft + width;  // right
	sourceRect.bottom= newTop + height; // bottom
}

inline void OffsetRectF(D2D1_RECT_F& sourceRect, const D2D1_RECT_F& targetRect) {
	// 计算源矩形的宽度和高度
	FLOAT width = sourceRect.right - sourceRect.left;
	FLOAT height = sourceRect.bottom - sourceRect.top;

	
	float left = targetRect.left + sourceRect.left;     // left
	float top = targetRect.top + sourceRect.top;      // top
	float right =targetRect.left + sourceRect.left + width;  // right
	float bottom = targetRect.top + sourceRect.top + height;   // bottom

	sourceRect.left = left;     // left
	sourceRect.top = top;      // top
	sourceRect.right =right;  // right
	sourceRect.bottom = bottom;   // bottom

}


//移动矩形

inline void MoveRectF(D2D1_RECT_F& rect, FLOAT x, FLOAT y) {
	float dx = RectFWidth(rect);
	float dy =RectFHeight(rect);
	
	rect.left   = x;
	rect.top    = y;

	rect.right  = x + dx;
	rect.bottom = y + dy;
}

// 缩放矩形（以原点为中心）
inline void ScaleRectF(D2D1_RECT_F& rect, FLOAT sx, FLOAT sy) {
	rect.left   *= sx;
	rect.top    *= sy;
	rect.right  *= sx;
	rect.bottom *= sy;
}

// 返回缩放后的新矩形
inline D2D1_RECT_F ScaleRectCopyF(const D2D1_RECT_F& rect, FLOAT sx, FLOAT sy) {
	return D2D1::RectF(rect.left * sx, rect.top * sy, rect.right * sx, rect.bottom * sy);
}

// 居中缩放（以矩形中心为中心缩放）
inline D2D1_RECT_F ScaleRectAroundCenterF(const D2D1_RECT_F& rect, FLOAT scale) {
	FLOAT cx = (rect.left + rect.right) / 2.0f;
	FLOAT cy = (rect.top + rect.bottom) / 2.0f;
	FLOAT hw = (rect.right - rect.left) * scale / 2.0f;
	FLOAT hh = (rect.bottom - rect.top) * scale / 2.0f;
	return D2D1::RectF(cx - hw, cy - hh, cx + hw, cy + hh);
}

//函数增加或减小指定矩形的宽度和高度
inline void InflateRectF(D2D1_RECT_F& rect, float dx, float dy){
	rect.left -= dx;
	rect.right += dx;
	rect.top -= dy;
	rect.bottom += dy;
}

//矩形与角度相交点
inline bool DegressIntersectRectF(float degress, D2D1_RECT_F rect, D2D1_POINT_2F& negative, D2D1_POINT_2F& positive)
{
	// === 计算起点和终点 ===
	float centerX = (rect.left + rect.right) * 0.5f;
	float centerY = (rect.top + rect.bottom) * 0.5f;

	// CSS angle: 0° = up (negative Y), 90° = right (positive X)
	// 转换为数学角度（从 X 轴正方向逆时针）
	float cssToMathAngle = 90.0f - degress; // 因为 CSS 0° 是 Y 负方向
	float rad = cssToMathAngle * static_cast<float>(M_PI) / 180.0f;

	float dirX = cosf(rad);
	float dirY = sinf(rad);

	// 辅助函数：从中心沿方向 (dx, dy) 射线，求与矩形边界的交点
	auto RayIntersectRect = [&](float dx, float dy) -> D2D1_POINT_2F {
		float t1 = (dx > 0) ? (rect.right - centerX) / dx : (dx < 0) ? (rect.left - centerX) / dx : FLT_MAX;
		float t2 = (dy > 0) ? (rect.bottom - centerY) / dy : (dy < 0) ? (rect.top - centerY) / dy : FLT_MAX;
		float t = min(max(0, t1), max(0, t2)); // 取最小正 t
		if (t == FLT_MAX) t = 0; // 防止除零
		return D2D1::Point2F(centerX + dx * t, centerY + dy * t);
	};

	positive   = RayIntersectRect(dirX, dirY);     // 沿 angle 方向
	negative = RayIntersectRect(-dirX, -dirY);   // 沿反方向

	return true;
}
