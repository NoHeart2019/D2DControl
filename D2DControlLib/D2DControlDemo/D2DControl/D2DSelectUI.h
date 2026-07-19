// CD2DSelectUI.h
#pragma once
#include "D2DContainerUI.h"
#include "D2DListUI.h"
#include <atlcoll.h>

// 下拉选择控件的专用样式
struct CD2DSelectStyle
{
	CD2DStyle      trigger;     // 触发器（显示区域）的样式
	D2D1_COLOR_F   arrowColor;  // 下拉箭头颜色
	CD2DStyle      dropDown;    // 下拉面板样式

	CD2DSelectStyle()
	{
		// 触发器默认样式
		trigger.background.type = CD2DBackground::Solid;
		trigger.background.solid.color = D2D1::ColorF(1.0f, 1.0f, 1.0f);
		trigger.border.top = CD2DBorderEdge(1.0f, Solid, D2D1::ColorF(0.5f, 0.5f, 0.5f));
		trigger.border.right = trigger.border.top;
		trigger.border.bottom = trigger.border.top;
		trigger.border.left = trigger.border.top;
		trigger.radius = CD2DRadius(4.0f);
		trigger.text.color = D2D1::ColorF(0.2f, 0.2f, 0.2f);
		trigger.text.fontSize = 14.0f;

		arrowColor = D2D1::ColorF(0.4f, 0.4f, 0.4f);   // 灰色箭头

		// 下拉面板默认样式
		dropDown.background.type = CD2DBackground::Solid;
		dropDown.background.solid.color = D2D1::ColorF(1.0f, 1.0f, 1.0f);
		dropDown.border.top = CD2DBorderEdge(1.0f, Solid, D2D1::ColorF(0.5f, 0.5f, 0.5f));
		dropDown.border.right = dropDown.border.top;
		dropDown.border.bottom = dropDown.border.top;
		dropDown.border.left = dropDown.border.top;

		// item 使用 CD2DItemStyle 默认构造函数
	}
};

class CD2DSelectUI : public CD2DContainerUI
{
public:
	CD2DSelectUI();
	virtual ~CD2DSelectUI();

	// 选项管理（使用 CD2DControlUI*，允许任意复杂项）
	void AddItem(CD2DControlUI* pItem);
	void RemoveItem(int index);
	void ClearItems();
	int  GetItemCount() const;
	CD2DControlUI* GetItem(int index) const;

	// 选中项
	void SetSelectedIndex(int index);
	int  GetSelectedIndex() const;
	CD2DControlUI* GetSelectedItem() const;
	CAtlString GetSelectedText() const;

	// 下拉列表外观控制
	void SetItemHeight(float height);
	void SetMaxDropDownItems(int count);   // 0 表示自动（显示所有项）
	int  GetMaxDropDownItems() const;

	// 样式管理
	void SetSelectStyle(ED2DStatus state, const CD2DSelectStyle& style);
	const CD2DSelectStyle& GetSelectStyle(ED2DStatus state) const;
	void SetDefaultSelectStyle(const CD2DSelectStyle& style);
	const CD2DSelectStyle& GetDefaultSelectStyle() const ;

	// 选择变化回调
	typedef void (WINAPI *SelectionCallback)(CD2DSelectUI* pSelect, int newIndex, void* pUserData);
	void SetOnSelectionChanged(SelectionCallback cb, void* pUserData = nullptr);

	// 重写基类
	virtual bool IsHitTest(const D2D1_POINT_2F& point) const override;
	virtual void DrawControl(CD2DRender* pRender) override;
	virtual void Layout() ;
	virtual D2D1_SIZE_F MeasureContent(CD2DRender* pRender, float maxWidth = FLT_MAX, float maxHeight = FLT_MAX) const override;
	virtual bool OnMouseDownL(const D2D1_POINT_2F& point) override;
	virtual bool OnMouseLeave(const D2D1_POINT_2F& point) override;
	virtual bool OnMouseUpL(const D2D1_POINT_2F& point) override;
	virtual bool OnMouseWheel(float delta, const D2D1_POINT_2F& point) override;
protected:
	void ShowDropDown();
	void HideDropDown();
	void UpdateDisplayText();
	void NotifySelectionChanged();
	void DrawArrow(CD2DRender* pRender, const D2D1_RECT_F& arrowRect, D2D1_COLOR_F color);
	const CD2DSelectStyle& GetEffectiveSelectStyle() const;

	// 内部列表选择回调
	static void WINAPI OnDropListSelection(CD2DListUI* pList, int newIndex, void* pUserData);

private:
	CD2DControlUI m_displayArea;            // 触发器显示区域（用于绘制当前选中项或占位文本）
	CD2DListUI    m_dropList;               // 下拉列表控件
	bool          m_droppedDown;            // 下拉是否展开

	CAtlMap<ED2DStatus, CD2DSelectStyle> m_selectStyles;   // 各状态样式
	CD2DSelectStyle                     m_defaultSelectStyle; // 默认样式

	SelectionCallback m_selectionCallback;
	void*             m_pCallbackUserData;
};