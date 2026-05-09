#pragma once
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>


#define C32RGB(r,g,b)          ((COLORREF)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16)))
#define  rgba(r, g, b, a) D2D1::ColorF((float)r/255.0f, (float)g/255.0f, (float)b/255.0f, (float)a)
#define C32D2RGB(r, g, b) RGB(b, g, r)
#define C32Color2ColorF(Color1) \
	D2D1::ColorF((GetBValue(Color1) << 16 | GetGValue(Color1) << 8 | GetRValue(Color1)), 1.0f)

#define Rect2RectF(Rect)\
		 D2D1::RectF((FLOAT)Rect.left, (FLOAT)Rect.top, (FLOAT)Rect.right, (FLOAT)Rect.bottom)

#define RectF2Rect(Rt){(int)Rt.left, (int)Rt.top, (int)Rt.right, (int)Rt.bottom}

#define GetRectFWidth(Rect)(Rect.right - Rect.left)

#define GetRectFHeight(Rect)(Rect.bottom - Rect.top)




/*
	根据WriteTextFormat 和矩形RECT计算文字在矩形居中显示时的D2D1_RECT_F。
	Original :要在此矩形内居中显示文字。
	Center   :文字居中矩形坐标。


*/

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

inline D2D1_RECT_F OffsetRectFCenter(const D2D1_RECT_F& sourceRect, const D2D1_RECT_F& targetRect) {
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
	D2D1_RECT_F Center ={
		newLeft,     // left
		newTop,      // top
		newLeft + width,  // right
		newTop + height   // bottom
	};

	return Center;
}

inline D2D1_RECT_F OffsetRectF(const D2D1_RECT_F& sourceRect, const D2D1_RECT_F& targetRect) {
	// 计算源矩形的宽度和高度
	FLOAT width = sourceRect.right - sourceRect.left;
	FLOAT height = sourceRect.bottom - sourceRect.top;

	// 构造并返回新的矩形
	D2D1_RECT_F Center ={
		targetRect.left + sourceRect.left,     // left
		targetRect.top + sourceRect.top,      // top
		targetRect.left + sourceRect.left + width,  // right
		targetRect.top + sourceRect.top + height   // bottom
	};

	return Center;
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


