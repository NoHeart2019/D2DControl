
#pragma once

// 前向声明
class CD2DRender;



class CD2DExplorerItemUI : public CD2DItemUI
{
public:
	CD2DExplorerItemUI() {}
	virtual ~CD2DExplorerItemUI() {}
};

// Item 浏览器控件
class CD2DExplorerUI : public CD2DControlUI
{
public:
	// 回调函数类型定义
	typedef void (*OnClickItemCallback)(CD2DExplorerUI* pExplorer, CD2DExplorerItemUI* pItem);

	// 滚动回调函数（静态成员函数）
	static void OnScrollCallback(float position, void* userData);

	CD2DExplorerUI();
	virtual ~CD2DExplorerUI();

public:
	// 布局模式枚举
	enum CD2DExplorerUIMode
	{
		FixedSize,   // 固定大小，间距自适应
		AutoExpand   // 自动扩充，间距固定，Item宽度自适应
	};
	// 绘制方法
	virtual void Render(CD2DRender* pRender);

	// 物品管理
	void AddItem(CD2DExplorerItemUI* pItem);
	void RemoveItem(int nIndex);
	void ClearItems();
	int GetItemCount() const;
	CD2DExplorerItemUI* GetItem(int nIndex);

	// 选择/悬停
	void SelectItem(int nIndex);
	void UnselectItem();
	int GetSelectedIndex() const;
	int GetHoverIndex() const;

	// 布局相关
	void SetItemSize(float fWidth, float fHeight);           // 固定大小模式：实际尺寸；自动模式：参考尺寸
	D2D1_SIZE_F GetItemSize() const;
	void SetItemSpacing(float fHorizontal, float fVertical); // 自动模式下水平间距生效
	void SetExplorerMode(CD2DExplorerUIMode mode);
	CD2DExplorerUIMode GetExplorerMode() const;
	void SetMinItemWidth(float width);                       // 自动扩充模式下最小宽度限制（默认10px）

	// 坐标转换
	D2D1_RECT_F GetItemRect(int nIndex) const;
	int GetItemIndexAtPoint(const D2D1_POINT_2F& point) const;

	// 滚动相关
	void SetScrollPosition(float fScrollPos);
	float GetScrollPosition() const;
	float GetMaxScrollPosition() const;
	void SetScrollWidth(float fScrollBarWidth);
	void SetVerticalScroll(D2D1_RECT_F& borderRect, float fVisibleHeight, float fTotalHeight, float minRange, float maxRange);
	CD2DScrollBarUI& GetVerticalScroll();

	// 回调相关
	void SetOnClickItemCallback(OnClickItemCallback callback, void* userData = nullptr);
	OnClickItemCallback GetOnClickItemCallback() const;

	// 鼠标事件处理
	virtual bool OnMouseDownL(const D2D1_POINT_2F& point);
	virtual bool OnMouseMove(const D2D1_POINT_2F& point);
	virtual bool OnMouseUpL(const D2D1_POINT_2F& point);
	virtual bool OnMouseLeave(const D2D1_POINT_2F& point);
	virtual bool OnMouseWheel(float delta);

private:
	// 更新布局参数（在Render中调用）
	void UpdateLayoutParameters(const D2D1_RECT_F& contentRect);

private:
	// 物品数据
	CAutoPtrArray<CD2DExplorerItemUI> m_items;

	// 布局模式相关
	CD2DExplorerUIMode m_explorerMode;            // 当前布局模式
	D2D1_SIZE_F m_itemSize;             // 固定大小模式：实际尺寸；自动模式：参考尺寸
	D2D1_SIZE_F m_itemSpacing;          // 自动模式：固定水平/垂直间距；固定模式：未使用（间距动态计算）
	float m_minItemWidth;               // 自动扩充模式下允许的最小Item宽度

	
	int     m_calculatedColumns;        // 实际每行列数
	float   m_calculatedItemWidth;      // 实际绘制的Item宽度
	float   m_calculatedItemHeight;     // 实际绘制的Item高度（始终等于 m_itemSize.height）
	float   m_calculatedHorzSpacing;    // 实际使用的水平间距（固定模式为动态值，自动模式为 m_itemSpacing.width）
	float   m_calculatedVertSpacing;    // 实际使用的垂直间距（始终等于 m_itemSpacing.height）

	// 选择与悬停
	int m_nSelectedIndex;
	int m_nHoverIndex;

	// 滚动
	float m_fScrollPosition;
	CD2DScrollBarUI m_scrollV;
	bool m_IsShowScrollBar;
	float m_fScrollBarWidth;

	// 回调
	OnClickItemCallback m_clickCallback;

};