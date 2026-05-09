#include "stdafx.h"
#include "D2DContainerUI.h"
#include "D2DLayout.h"
#include <float.h>      // for FLT_MAX

static inline float clamp(float value, float low, float high)
{
	return value < low ? low : (value > high ? high : value);
}

CD2DVertContainerUI::CD2DVertContainerUI()
	: m_alignment(CD2DAlignment::ALIGN_CENTER, CD2DAlignment::ALIGN_TOP)
	
{
	SetSizePolicy(CD2DSizePolicy::Expanding);
	EnableVerticalScroll(false);
}

CD2DVertContainerUI::~CD2DVertContainerUI()
{
}

void CD2DVertContainerUI::SetSizePolicy(CD2DSizePolicy::Policy policy)
{
	if (m_sizePolicy == policy)
		return;
	m_sizePolicy = policy;

	if (policy == CD2DSizePolicy::Fixed)
	{
		// Fixed 模式：强制关闭滚动条
		EnableVerticalScroll(true);
	}
	else // Expanding
	{
		// Expanding 模式：默认关闭滚动功能
		EnableVerticalScroll(false);
	}

}

D2D1_SIZE_F CD2DVertContainerUI::Measure(const D2D1_SIZE_F& availableSize, CD2DRender* pRender)
{
	// 1. 获取容器的盒模型占位尺寸（margin + border + padding）
	D2D1_SIZE_F boxSize = GetBoxModelSize(); 

	// 2. 内容区可用尺寸 = 父级可用尺寸 - 盒模型占位尺寸
	float width  = (availableSize.width  == FLT_MAX) ? FLT_MAX : availableSize.width  - boxSize.width;
	float height = (availableSize.height == FLT_MAX) ? FLT_MAX : availableSize.height - boxSize.height;
	if (width < 0)  width = 0;
	if (height < 0) height = 0;

	// 3. 第一次遍历：测量所有子控件的理想尺寸（包括子控件自己的盒模型）
	CAtlArray<D2D1_SIZE_F> sizes;   // 子控件需要的总尺寸（包含 margin）
	float totalHeight = 0.0f;
	float maxWidth    = 0.0f;

	for (size_t i = 0; i < m_controls.GetCount(); ++i)
	{
		CD2DControlUI* control = m_controls[i];
		// 测量子控件：垂直布局中，高度无限制（FLT_MAX），宽度受限于内容区可用宽度
		//size 已经包含了margin padding size的大小
		D2D1_SIZE_F size = control->Measure(D2D1::SizeF(width, FLT_MAX), pRender);

		sizes.Add(size);
		totalHeight += size.height;
		if (size.width > maxWidth)
			maxWidth = size.width;
	}

	// 4. 确定容器自己的内容区尺寸
	//    宽度：如果可用宽度无限，则取所有子控件最大宽度；否则取可用宽度（不允许超出）
	float finalContentWidth = (width == FLT_MAX) ? maxWidth : width;
	//    高度：所有子控件高度之和（垂直布局不允许超出，若超出则截断或滚动，此处简单处理）
	float finalContentHeight = totalHeight;
	
	// 5. 获取内容区的实际矩形（已经扣除 padding/border，但未扣除 margin）
	D2D1_RECT_F contentRect = GetContentRectangle();
	float currentY = contentRect.top - GetVerticalPosition();   // 当前 Y 起始位置
	for (size_t i = 0; i < m_controls.GetCount(); ++i)
	{
		CD2DControlUI* control = m_controls[i];
		D2D1_SIZE_F  size = sizes[i];
		contentRect.top = currentY;
	
		D2D1_POINT_2F point = m_alignment.CalculatePosition(contentRect, size);
		ZxDebugEx((__FUNCTION__"() {%.2f %.2f %.2f %.2f}(%.2f %.2f) \n", 
			control->GetPosition().x, control->GetPosition().y,
			control->GetWidth(), control->GetHeight(),
			width, height));
		// 设置子控件的最终矩形（注意：这里设置的是子控件的外框，包含其 margin）
		control->SetPosition(point.x, point.y);
		control->SetSize(size.width, size.height);

		// 更新当前 Y 累计高度（用于下一个子控件）
		currentY += size.height;
	}
	
	if (m_sizePolicy == CD2DSizePolicy::Expanding)
		SetContentSize(finalContentWidth, finalContentHeight);
	else
	{
		SetTotalHeight(finalContentHeight);
		SetVerticalScroll();
	}

	return GetSize();
}

void CD2DVertContainerUI::Render(CD2DRender* pRender)
{
	if (!IsVisible())
		return;

	if (m_sizePolicy == CD2DSizePolicy::Fixed)
		pRender->PushAxisAlignedClip(GetBorderRectangle());
	
	CD2DContainerUI::Render(pRender);
	for (size_t i = 0; i < m_controls.GetCount(); i++)
	{
		CD2DControlUI* control = m_controls[i];
		if (control == &GetVerticalScroll())
			continue;
		control->Render(pRender);
	}

	if (m_sizePolicy == CD2DSizePolicy::Fixed)
		pRender->PopAxisAlignedClip();

	GetVerticalScroll().Render(pRender);

}

void CD2DVertContainerUI::Relayout()
{
	D2D1_RECT_F contentRect = GetContentRectangle();
	float currentY = contentRect.top - GetVerticalPosition();   // 当前 Y 起始位置

	for (size_t i = 0, idx = 0; i < m_controls.GetCount(); ++i)
	{
		CD2DControlUI* control = m_controls[i];
		if (control == &GetVerticalScroll()) continue;
		D2D1_SIZE_F size = control->GetSize();
		control->SetPosition(control->GetPosition().x, currentY);
		currentY += size.height;
	}

}


CD2DHoriContainerUI::CD2DHoriContainerUI()
	: m_alignment(CD2DAlignment::ALIGN_CENTER, CD2DAlignment::ALIGN_TOP)

{
	SetSizePolicy(CD2DSizePolicy::Expanding);
	EnableVerticalScroll(false);
}

CD2DHoriContainerUI::~CD2DHoriContainerUI()
{
}

void CD2DHoriContainerUI::SetSizePolicy(CD2DSizePolicy::Policy policy)
{
	if (m_sizePolicy == policy)
		return;
	m_sizePolicy = policy;

	if (policy == CD2DSizePolicy::Fixed)
	{
		// Fixed 模式：强制关闭滚动条
		EnableVerticalScroll(true);
	}
	else // Expanding
	{
		// Expanding 模式：默认关闭滚动功能
		EnableVerticalScroll(false);
	}

}

D2D1_SIZE_F CD2DHoriContainerUI::Measure(const D2D1_SIZE_F& availableSize, CD2DRender* pRender)
{
	// 1. 盒模型占位
	D2D1_SIZE_F boxSize = GetBoxModelSize();

	// 2. 内容区可用尺寸（父级可用减去盒模型）
	float availWidth  = (availableSize.width  == FLT_MAX) ? FLT_MAX : availableSize.width  - boxSize.width;
	float availHeight = (availableSize.height == FLT_MAX) ? FLT_MAX : availableSize.height - boxSize.height;
	if (availWidth  < 0) availWidth  = 0;
	if (availHeight < 0) availHeight = 0;

	// 3. 测量所有子控件（高度受限，宽度无限）
	CAtlArray<D2D1_SIZE_F> sizes;
	float totalWidth = 0.0f;
	float maxHeight  = 0.0f;

	for (size_t i = 0; i < m_controls.GetCount(); ++i)
	{
		CD2DControlUI* control = m_controls[i];
		// 水平布局：宽度无限，高度受限于内容区可用高度
		D2D1_SIZE_F size = control->Measure(D2D1::SizeF(FLT_MAX, availHeight), pRender);
		sizes.Add(size);
		totalWidth += size.width;
		if (size.height > maxHeight)
			maxHeight = size.height;
	}

	// 4. 确定容器自己的内容区尺寸
	float finalContentWidth  = totalWidth;   // 水平容器内容宽度 = 所有子控件宽度之和
	float finalContentHeight = (availHeight == FLT_MAX) ? maxHeight : availHeight;  // 高度优先受限于父级

	// 5. 计算水平起始偏移（根据容器水平对齐方式）
	D2D1_RECT_F contentRect = GetContentRectangle();  // 已扣除 padding/border
	float startX = contentRect.left;
	float contentAreaWidth = contentRect.right - contentRect.left;

	// 根据容器自身的水平对齐方式（注意：这里不是子控件的对齐，而是整体内容的对齐）
	// 假设 m_horizontalAlignment 是容器的水平对齐枚举（Left/Center/Right）
	switch (m_alignment.GetHorizontal())
	{
	case CD2DAlignment::ALIGN_CENTER:
		startX = contentRect.left + (contentAreaWidth - totalWidth) / 2.0f;
		break;
	case CD2DAlignment::ALIGN_RIGHT:
		startX = contentRect.right - totalWidth;
		break;
	default: // Left
		startX = contentRect.left;
		break;
	}

	// 6. 布局子控件（水平排列，应用整体偏移）
	float currentX = startX;
	for (size_t i = 0; i < m_controls.GetCount(); ++i)
	{
		CD2DControlUI* control = m_controls[i];
		D2D1_SIZE_F size = sizes[i];

		// 构造候选矩形：宽度为子控件宽度，高度为内容区高度（用于垂直对齐）
		D2D1_RECT_F candidateRect;
		candidateRect.left   = currentX;
		candidateRect.right  = currentX + size.width;
		candidateRect.top    = contentRect.top;
		candidateRect.bottom = contentRect.top + finalContentHeight;

		// 计算子控件在候选矩形内的最终位置（通常处理垂直方向的对齐，水平方向不变）
		D2D1_POINT_2F point = m_alignment.CalculatePosition(candidateRect, size);

		control->SetPosition(point.x, point.y);
		control->SetSize(size.width, size.height);

		currentX += size.width;
	}

	// 7. 根据大小策略设置容器内容尺寸或滚动
	if (m_sizePolicy == CD2DSizePolicy::Expanding)
	{
		SetContentSize(finalContentWidth, finalContentHeight);
	}
	else // Fixed
	{
		
	}

	// 8. 返回容器总尺寸（内容区 + 盒模型）
	
	return GetSize();
}

void CD2DHoriContainerUI::Render(CD2DRender* pRender)
{
	if (!IsVisible())
		return;

	SetBackgroundColor(D2D1::ColorF(0x123456));
	SetBackgroundVisible(true);


	if (m_sizePolicy == CD2DSizePolicy::Fixed)
		pRender->PushAxisAlignedClip(GetBorderRectangle());

	CD2DContainerUI::Render(pRender);
	for (size_t i = 0; i < m_controls.GetCount(); i++)
	{
		CD2DControlUI* control = m_controls[i];
		if (control == &GetVerticalScroll())
			continue;
		control->Render(pRender);
	}

	if (m_sizePolicy == CD2DSizePolicy::Fixed)
		pRender->PopAxisAlignedClip();

	GetVerticalScroll().Render(pRender);

}

void CD2DHoriContainerUI::Relayout()
{
	D2D1_RECT_F contentRect = GetContentRectangle();
	float currentY = contentRect.top - GetVerticalPosition();   // 当前 Y 起始位置

	for (size_t i = 0, idx = 0; i < m_controls.GetCount(); ++i)
	{
		CD2DControlUI* control = m_controls[i];
		if (control == &GetVerticalScroll()) continue;
		D2D1_SIZE_F size = control->GetSize();
		control->SetPosition(control->GetPosition().x, currentY);
		currentY += size.height;
	}

}

