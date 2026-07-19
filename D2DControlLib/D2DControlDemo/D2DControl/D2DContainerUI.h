#pragma once
#include "D2DControlUI.h"
#include "D2DScrollUI.h"
#include <atlcoll.h>



// Z‑Order 排序比较函数（供 qsort 使用）
static int __cdecl CompareZOrder(const void* a, const void* b)
{
	CD2DControlUI* pA = *(CD2DControlUI**)a;
	CD2DControlUI* pB = *(CD2DControlUI**)b;
	int zA = pA ? pA->GetZOrder() : 0;
	int zB = pB ? pB->GetZOrder() : 0;
	return zA - zB;   // 升序
}


class CD2DContainerUI : public CD2DControlUI
{
public:
	enum Overflow
	{
		Hidden,       // 裁剪内容，无滚动条
		Auto,         // 内容溢出时自动显示滚动条，否则隐藏
		Scroll        // 始终显示垂直滚动条
	};
	CD2DContainerUI();
	virtual ~CD2DContainerUI();
	// 溢出模式
	
	// 子控件管理（指针由外部管理生命周期）
	void AddChild(CD2DControlUI* pChild);
	void RemoveChild(CD2DControlUI* pChild);
	void ClearChildren();
	void DeleteChildren();

	size_t GetChildCount() const { return m_children.GetCount(); }
	CD2DControlUI* GetChildAt(size_t index) const {return (index < m_children.GetCount()) ? m_children[index] : nullptr;}

	// 将指定子控件提升到最顶层
	void BringChildToFront(CD2DControlUI* pChild);

	void SetOverflow(Overflow overflow);
	Overflow GetOverflow() const { return m_overflow; }

	CD2DScrollUI* GetScrollBar() { return &m_scrollBar; }
	float  GetContentHeight() const { return m_contentHeight; }
	void   SetContentHeight(float h) { m_contentHeight = h; }
	virtual D2D1_SIZE_F MeasureContent(CD2DRender* pRender, float maxWidth = FLT_MAX, float maxHeight = FLT_MAX) const override;
	void UpdateScrollBar(float viewportHeight);
	// 将相对容器可见区域左上角的坐标转换为相对内容区域左上角的坐标（考虑滚动）

	D2D1_POINT_2F TransformPoint(const D2D1_POINT_2F& visiblePoint) const;



	// 重写绘制、命中测试、事件处理
	virtual void DrawControl(CD2DRender* pRender) override;
	virtual bool IsHitTest(const D2D1_POINT_2F& point) const override;

	virtual bool OnMouseDownL(const D2D1_POINT_2F& point) override;
	virtual bool OnMouseMove(const D2D1_POINT_2F& point) override;
	virtual bool OnMouseUpL(const D2D1_POINT_2F& point) override;
	virtual bool OnMouseDownL2(const D2D1_POINT_2F& point) override;
	virtual bool OnMouseUpL2(const D2D1_POINT_2F& point) override;
	virtual bool OnMouseLeave(const D2D1_POINT_2F& point) override;
	virtual bool OnMouseEnter(const D2D1_POINT_2F& point) override;
	virtual bool OnMouseWheel(float delta, const D2D1_POINT_2F& point) override;
	virtual bool OnKeyDown(DWORD keyCode) override;
	virtual bool OnKeyUp(DWORD keyCode) override;
	virtual bool OnChar(DWORD ch) override;

protected:
	// 按 Z‑Order 升序排列子控件
	int GetSortedChildren(const CAtlArray<CD2DControlUI*>& children, CAtlArray<CD2DControlUI*>& sorted);

	// 事件分派辅助：从顶层到底层尝试分派事件，返回消费的控件或 nullptr
	template<typename Func>
	CD2DControlUI* DispatchEvent(const D2D1_POINT_2F& point, Func func);

	virtual void Layout(CD2DRender* pRender);
	


private:
	CAtlArray<CD2DControlUI*> m_children;
	// 设置焦点子控件（会自动清除旧焦点）
	void SetFocusedChild(CD2DControlUI* pChild);
	CD2DControlUI* GetFocusedChild() const { return m_pFocusedChild; }

protected:
	CD2DControlUI* m_pFocusedChild;   // 当前拥有焦点的子控件
	CD2DControlUI* m_pCapturedChild ;   // 当前鼠标捕获的子控件
	CD2DControlUI* m_pHoverChild ;       // 当前悬停的子控件（原有）

	CD2DScrollUI m_scrollBar;
	Overflow m_overflow;
	float m_scrollOffsetY;
	float m_scrollOffsetX; //未使用
	bool m_scrollBarVisible;
	float m_contentHeight;      // 内容总高度（上次布局结果）
	static const float DEFAULT_SCROLLBAR_WIDTH ; // 默认滚动条宽度
};





class CD2DHBoxContainerUI : public CD2DContainerUI
{
public:
	
	CD2DHBoxContainerUI();
	virtual ~CD2DHBoxContainerUI();

	void SetSpacing(float spacing) { m_spacing = spacing;  }
	float GetSpacing() const { return m_spacing; }

	// 主轴对齐（决定整体内容在容器内的水平位置）
	void SetAlignment(const CD2DAlignment& align) { m_align = align;  }
	const CD2DAlignment& GetAlignment() const { return m_align; }

	void SetExpandWeight(float weight) { m_expandWeight = weight; }
	float GetExpandWeight() const { return m_expandWeight; }

	virtual D2D1_SIZE_F MeasureContent(CD2DRender* pRender, float maxWidth = FLT_MAX, float maxHeight = FLT_MAX) const override;

	virtual void DrawControl(CD2DRender* pRender) override;
	void Layout(CD2DRender* pRender) ;
	virtual void Layout1(CD2DRender* pRender);
protected:
	CD2DAlignment    m_align;          // 对齐
	float m_spacing;
	float m_expandWeight;
};
// 垂直布局容器
class CD2DVBoxContainerUI : public CD2DContainerUI
{
public:
	CD2DVBoxContainerUI();
	virtual ~CD2DVBoxContainerUI();

	void SetSpacing(float spacing) { m_spacing = spacing;  }
	float GetSpacing() const { return m_spacing; }

	void SetAlignment(const CD2DAlignment& align) { m_align = align;}
	const CD2DAlignment& GetAlignment() const { return m_align; }

	//// 策略与权重（供父容器布局使用）
	//void SetSizePolicy(CD2DSizePolicy policy) { m_sizePolicy = policy; }
	//CD2DSizePolicy GetSizePolicy() const { return m_sizePolicy; }
	//void SetExpandWeight(float weight) { m_expandWeight = weight; }
	//float GetExpandWeight() const { return m_expandWeight; }

	virtual D2D1_SIZE_F MeasureContent(CD2DRender* pRender, float maxWidth = FLT_MAX, float maxHeight = FLT_MAX) const override;

	virtual void DrawControl(CD2DRender* pRender) override;

	void Layout(CD2DRender* pRender) ;
	void Layout1(CD2DRender* pRender) ;

protected:
	
	CD2DAlignment  m_align;
	float          m_spacing;
};

class CD2DGridContainerUI : public CD2DContainerUI
{
public:
	CD2DGridContainerUI();
	virtual ~CD2DGridContainerUI();

	void SetRows(int rows);
	int GetRows() const { return m_rows; }
	void SetColumns(int cols);
	int GetColumns() const { return m_cols; }
	void SetRowSpacing(float spacing);
	float GetRowSpacing() const { return m_rowSpacing; }
	void SetColumnSpacing(float spacing);
	float GetColumnSpacing() const { return m_colSpacing; }
	void SetCellAlignment(const CD2DAlignment& align);
	const CD2DAlignment& GetCellAlignment() const { return m_cellAlign; }

	virtual void Layout(CD2DRender* pRender) ;

protected:
	int m_rows;
	int m_cols;
	float m_rowSpacing;
	float m_colSpacing;
	CD2DAlignment m_cellAlign;
};