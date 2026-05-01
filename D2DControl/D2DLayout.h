#pragma once
   // 假设基类在此
#include "D2DAlignment.h"        // 刚才的 CD2DAlignment 类

class CD2DVertContainerUI : public CD2DContainerUI
{
public:
	CD2DVertContainerUI();
	virtual ~CD2DVertContainerUI();

	// 大小策略
	void SetSizePolicy(CD2DSizePolicy::Policy policy);
	CD2DSizePolicy GetSizePolicy() const { return m_sizePolicy; }

	// 设置整个容器的对齐方式（影响子控件水平/垂直分布）
	void SetAlignment(const CD2DAlignment& align) { m_alignment = align; }
	CD2DAlignment GetAlignment() const { return m_alignment; }

	

	// 便捷方法
	void SetHorizontalAlign(CD2DAlignment::D2D_ALIGNMENT horz) { m_alignment.SetHorizontal(horz); }
	void SetVerticalAlign(CD2DAlignment::D2D_ALIGNMENT vert)   { m_alignment.SetVertical(vert); }

	// 重写 Measure：计算自身尺寸并布局子控件
	virtual D2D1_SIZE_F Measure(const D2D1_SIZE_F& availableSize, CD2DRender* pRender) ;

	virtual void Render(CD2DRender* pRender);

	virtual void Relayout();

private:
	CD2DAlignment m_alignment;
};

class CD2DHoriContainerUI : public CD2DContainerUI
{
public:
	CD2DHoriContainerUI();
	virtual ~CD2DHoriContainerUI();

	// 大小策略
	void SetSizePolicy(CD2DSizePolicy::Policy policy);
	CD2DSizePolicy GetSizePolicy() const { return m_sizePolicy; }

	// 设置整个容器的对齐方式（影响子控件水平/垂直分布）
	void SetAlignment(const CD2DAlignment& align) { m_alignment = align; }
	CD2DAlignment GetAlignment() const { return m_alignment; }



	// 便捷方法
	void SetHorizontalAlign(CD2DAlignment::D2D_ALIGNMENT horz) { m_alignment.SetHorizontal(horz); }
	void SetVerticalAlign(CD2DAlignment::D2D_ALIGNMENT vert)   { m_alignment.SetVertical(vert); }

	// 重写 Measure：计算自身尺寸并布局子控件
	virtual D2D1_SIZE_F Measure(const D2D1_SIZE_F& availableSize, CD2DRender* pRender) ;

	virtual void Render(CD2DRender* pRender);

	virtual void Relayout();

private:
	CD2DAlignment m_alignment;
};

