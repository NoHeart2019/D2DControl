#include "stdafx.h"
#include "D2DBoxModel.h"

CD2DBoxModel::CD2DBoxModel()
	:m_marginLeft(0.0f),
	m_marginTop(0.0f),
	m_marginRight(0.0f),
	m_marginBottom(0.0f),
	m_paddingLeft(0.0f),
	m_paddingTop(0.0f),
	m_paddingRight(0.0f),
	m_paddingBottom(0.0f),
	m_borderLeft(0.0f),
	m_borderTop(0.0f),
	m_borderRight(0.0f),
	m_borderBottom(0.0f)
{

}

CD2DBoxModel::~CD2DBoxModel()
{


}

void CD2DBoxModel::SetBoxModel( float marginLeft,  float marginTop,  float marginRight,  float marginBottom,float borderLeft,  float borderTop,  float borderRight, float borderBottom, 
	float paddingLeft, float paddingTop, float paddingRight, float paddingBottom)
{
	// 外边距 (Margin)
	m_marginLeft    = marginLeft;
	m_marginTop     = marginTop;
	m_marginRight   = marginRight;
	m_marginBottom  = marginBottom;

	// 内边距 (Padding)
	m_paddingLeft    = paddingLeft;
	m_paddingTop     = paddingTop;
	m_paddingRight   = paddingRight;
	m_paddingBottom  = paddingBottom;

	// 边框 (Border)
	m_borderLeft    = borderLeft;
	m_borderTop     = borderTop;
	m_borderRight   = borderRight;
	m_borderBottom  = borderBottom;
}

// 外边距(margin)相关方法实现
void CD2DBoxModel::SetMargin(float left, float top, float right, float bottom)
{
	m_marginLeft = left;
	m_marginTop = top;
	m_marginRight = right;
	m_marginBottom = bottom;
}

void CD2DBoxModel::SetMarginLeft(float margin)
{
	m_marginLeft = margin;
}

void CD2DBoxModel::SetMarginTop(float margin)
{
	m_marginTop = margin;
}

void CD2DBoxModel::SetMarginRight(float margin)
{
	m_marginRight = margin;
}

void CD2DBoxModel::SetMarginBottom(float margin)
{
	m_marginBottom = margin;
}

float CD2DBoxModel::GetMarginLeft() const
{
	return m_marginLeft;
}

float CD2DBoxModel::GetMarginTop() const
{
	return m_marginTop;
}

float CD2DBoxModel::GetMarginRight() const
{
	return m_marginRight;
}

float CD2DBoxModel::GetMarginBottom() const
{
	return m_marginBottom;
}


// 外边距(margin)相关方法实现
void CD2DBoxModel::SetPadding(float left, float top, float right, float bottom)
{
	m_paddingLeft = left;
	m_paddingTop = top;
	m_paddingRight = right;
	m_paddingBottom = bottom;
}

void CD2DBoxModel::SetPadding(float padding)
{
	m_paddingLeft = padding;
	m_paddingTop = padding;
	m_paddingRight = padding;
	m_paddingBottom = padding;
}

void CD2DBoxModel::SetPadding(float x, float y)
{
	m_paddingLeft = x;
	m_paddingTop = y;
	m_paddingRight = x;
	m_paddingBottom = y;
}

void CD2DBoxModel::SetPaddingLeft(float padding)
{
	m_paddingLeft = padding;
}

void CD2DBoxModel::SetPaddingTop(float padding)
{
	m_paddingTop = padding;
}

void CD2DBoxModel::SetPaddingRight(float padding)
{
	m_paddingRight = padding;
}

void CD2DBoxModel::SetPaddingBottom(float padding)
{
	m_paddingBottom = padding;
}

float CD2DBoxModel::GetPaddingLeft() const {
	return m_paddingLeft;
}

float CD2DBoxModel::GetPaddingTop() const {
	return m_paddingTop;
}

float CD2DBoxModel::GetPaddingRight() const {
	return m_paddingRight;
}

float CD2DBoxModel::GetPaddingBottom() const {
	return m_paddingBottom;
}

// ==================== 边框(Border)相关方法实现 ====================
void CD2DBoxModel::SetBorder(float left, float top, float right, float bottom)
{
	m_borderLeft = left;
	m_borderTop = top;
	m_borderRight = right;
	m_borderBottom = bottom;
}

void CD2DBoxModel::SetBorderLeft(float border)
{
	m_borderLeft = border;
}

void CD2DBoxModel::SetBorderTop(float border)
{
	m_borderTop = border;
}

void CD2DBoxModel::SetBorderRight(float border)
{
	m_borderRight = border;
}

void CD2DBoxModel::SetBorderBottom(float border)
{
	m_borderBottom = border;
}

float CD2DBoxModel::GetBorderLeft() const
{
	return m_borderLeft;
}

float CD2DBoxModel::GetBorderTop() const
{
	return m_borderTop;
}

float CD2DBoxModel::GetBorderRight() const
{
	return m_borderRight;
}

float CD2DBoxModel::GetBorderBottom() const
{
	return m_borderBottom;
}

// ==================== 盒模型区域矩形获取方法实现 ====================


D2D1_RECT_F CD2DBoxModel::GetRectangle() const
{
	return D2D1::RectF();
}

 D2D1_SIZE_F CD2DBoxModel::GetBoxModelSize() const
{
	
	return D2D1::SizeF(m_marginLeft + m_paddingLeft + m_paddingRight + m_marginRight,
					   m_marginTop + m_paddingTop + m_paddingBottom + m_marginBottom);
}


// 获取内容区域矩形（content area）
D2D1_RECT_F CD2DBoxModel::GetContentRectangle() const
{
	// 内容区域 = padding区域 - padding
	D2D1_RECT_F paddingRect = GetPaddingRectangle();
	return D2D1::RectF(
		paddingRect.left + m_paddingLeft,
		paddingRect.top + m_paddingTop,
		paddingRect.right - m_paddingRight,
		paddingRect.bottom - m_paddingBottom
		);
}

// 获取内边距区域矩形（padding area）
D2D1_RECT_F CD2DBoxModel::GetPaddingRectangle() const
{
	// padding区域 = Border区域 - Border	
	D2D1_RECT_F borderRect = GetBorderRectangle();
	
	return D2D1::RectF(borderRect.left   + m_borderLeft,
					   borderRect.top    + m_borderTop,
					   borderRect.right  - m_borderRight,
					   borderRect.bottom - m_borderBottom);
}

// 获取边框区域矩形（border area）
D2D1_RECT_F CD2DBoxModel::GetBorderRectangle() const
{
	// border区域 = margin区域 - margin
	D2D1_RECT_F marginRect = GetMarginRectangle();
	return D2D1::RectF(
		marginRect.left + m_marginLeft,
		marginRect.top +  m_marginTop,
		marginRect.right  -  m_marginRight,
		marginRect.bottom - m_marginBottom   
		);
}

// 获取外边距区域矩形（margin area）
D2D1_RECT_F CD2DBoxModel::GetMarginRectangle() const
{
	// margin区域就是控件的整体矩形
	return GetRectangle();
}

