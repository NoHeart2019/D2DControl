#pragma once
#include "D2DControlUI.h"
#include "D2DScrollUI.h"
#include <atlcoll.h>


// 列表项三态样式

class CD2DListUI : public CD2DControlUI
{
public:
	CD2DListUI();
	virtual ~CD2DListUI();

	// 数据管理
	void AddItem(CD2DControlUI* pItem);
	void RemoveItem(int index);
	void ClearItems();
	int  GetItemCount() const { return (int)m_items.GetCount(); }
	CD2DControlUI* GetItem(int index) const;

	// 选择
	void SetSelectedIndex(int index);
	int  GetSelectedIndex() const { return m_selectedIndex; }
	CD2DControlUI* GetSelectedItem() const;

	// 外观
	void SetItemHeight(float height);
	void SetItemSpacing(float spacing);          // 项与项之间的间距
	void SetMaxVisibleItems(int count);          // 0 = 根据列表高度全部显示
	int GetMaxVisibleItems() const;

	// 重写基类
	virtual void DrawControl(CD2DRender* pRender) override;
	virtual bool OnMouseDownL(const D2D1_POINT_2F& point) override;
	virtual bool OnMouseMove(const D2D1_POINT_2F& point) override;
	virtual bool OnMouseUpL(const D2D1_POINT_2F& point) override;
	virtual bool OnMouseWheel(float delta, const D2D1_POINT_2F& point) override;
	virtual bool OnMouseLeave(const D2D1_POINT_2F& point) override;
	virtual void Layout() ;

	// 计算指定数量项的理论总高度（不考虑最大可见项数）
	float CalculateContentHeight(int itemCount) const;

protected:
	int  HitTestItem(const D2D1_POINT_2F& contentPoint) const;
	void GetVisibleRange(int& first, int& last) const;
	void NotifySelectionChanged();

private:
	CAtlArray<CD2DControlUI*> m_items;          // 所有项（外部管理生命周期）
	int m_selectedIndex;                        // -1 表示无

	

	float m_itemHeight;                         // 每项内容高度（像素）
	float m_itemSpacing;                        // 项之间的垂直间距
	int   m_maxVisibleItems;                    // 0 表示不限制，显示所有
	float m_scrollOffsetY;                      // 垂直滚动偏移（像素）
	int   m_totalItemCount;                     // 总项数（m_items.GetCount() 缓存）

	CD2DScrollUI m_scrollBar;
	bool m_scrollBarVisible;

	int  m_hoveredIndex;                        // 当前悬停项索引，-1 无

};