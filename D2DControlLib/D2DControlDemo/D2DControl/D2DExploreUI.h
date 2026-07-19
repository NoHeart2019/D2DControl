#pragma once
#include "D2DContainerUI.h"
#include "D2DScrollUI.h"
#include <atlcoll.h>

class CD2DRender;
class CD2DControlUI;

/**
 * 探索型容器控件（单选 + 可选拖拽排序）
 * 视图：List, Horizontal, Wrap, Grid
 * 仅使用 ATL，不依赖 STL
 */
class CD2DExploreUI : public CD2DControlUI
{
public:
    enum ViewMode      { List, Horizontal, Wrap, Grid };
    enum SelectionMode { SelNone, Single };

    // 拖拽排序回调
    typedef void (WINAPI* ReorderCallback)(CD2DExploreUI* pList, CD2DControlUI* pItem,
                                           int oldIndex, int newIndex, void* pUserData);

    CD2DExploreUI();
    virtual ~CD2DExploreUI();

    // ===== 视图与布局配置 =====
    void SetViewMode(ViewMode mode);
    ViewMode GetViewMode() const { return m_viewMode; }
    void SetColumns(int cols);
    int  GetColumns() const { return m_columns; }
    void SetHorizontalSpacing(float spacing);
    float GetHorizontalSpacing() const { return m_hSpacing; }
    void SetVerticalSpacing(float spacing);
    float GetVerticalSpacing() const { return m_vSpacing; }
    void SetItemWidth(float width);
    float GetItemWidth() const { return m_itemWidth; }
    void SetItemHeight(float height);
    float GetItemHeight() const { return m_itemHeight; }
    void SetWrapAutoFit(bool enable);
    bool IsWrapAutoFit() const { return m_wrapAutoFit; }

    // ===== 项管理 =====
    int  GetItemCount() const { return (int)m_items.GetCount(); }
    CD2DControlUI* GetItemAt(int index) const;
    int  GetItemIndex(CD2DControlUI* pItem) const;
    void AddItem(CD2DControlUI* pItem);
    void RemoveItem(CD2DControlUI* pItem);
    void RemoveAllItems();
	const CAtlArray<CD2DControlUI*>& GetAllItems() const;
	CAtlArray<CD2DControlUI*>& GetAllItems();  

	const CAtlArray<CD2DControlUI*>& GetItems() const {return m_items;}
    // ===== 垂直滚动 =====
    void SetScrollOffsetY(float offset);
    float GetScrollOffsetY() const { return m_scrollOffsetY; }
    float GetTotalContentHeight() const;
    float GetViewportHeight() const;
    void SetScrollBarVisible(bool visible);
    bool IsScrollBarVisible() const;
    CD2DScrollUI* GetScrollBar() { return &m_scrollBar; }
    void SetScrollBarStyle(ED2DStatus state, const CD2DScrollStyle& style);

    // ===== 单选 =====
    void SetSelectionMode(SelectionMode mode);
    SelectionMode GetSelectionMode() const { return m_selectionMode; }
    void SelectItem(int index);
    void ClearSelection();
    int  GetSelectedIndex() const { return m_selectedIndex; }

    // ===== 拖拽排序 =====
    void EnableDragging(bool enable);
    bool IsDraggingEnabled() const { return m_draggingEnabled; }
    bool IsDragging() const { return m_isDragging; }
    void SetOnItemReordered(ReorderCallback callback, void* pUserData = nullptr);

    // ===== 重写基类接口 =====
    virtual D2D1_SIZE_F MeasureContent(CD2DRender* pRender, float maxWidth) const;
    virtual void Layout();
    virtual void DrawControl(CD2DRender* pRender) override;
	virtual void DrawDragGhost(CD2DRender* pRender);
    virtual bool OnMouseDownL(const D2D1_POINT_2F& point) override;
    virtual bool OnMouseMove(const D2D1_POINT_2F& point) override;
    virtual bool OnMouseUpL(const D2D1_POINT_2F& point) override;
    virtual bool OnMouseLeave(const D2D1_POINT_2F& point) override;
    virtual bool OnMouseWheel(float delta, const D2D1_POINT_2F& point) override;
    virtual bool OnKeyDown(DWORD keyCode) override;

protected:
    // ===== 布局实现 =====
    void LayoutList(const D2D1_RECT_F& viewport);
    void LayoutHorizontal(const D2D1_RECT_F& viewport);
    void LayoutWrap(const D2D1_RECT_F& viewport);
    void LayoutGrid(const D2D1_RECT_F& viewport);
    float GetTotalContentHeightInternal() const;
    void SyncScrollBar();

    // ===== 选择辅助 =====
    void UpdateItemChecked(int index, bool checked);

    // ===== 拖拽实现 =====
    static const float DRAG_START_THRESHOLD;  // 5.0f
    void StartDragging(int sourceIndex);
    void CancelDragging();
    void FinishDragging(int targetIndex);
    int  HitTestItem(const D2D1_POINT_2F& contentPoint) const;
    int  GetDragInsertIndex(float contentY) const;
    void UpdateDragGhostPosition(const D2D1_POINT_2F& mouseScreenPos);
    void ProcessEdgeAutoScroll(const D2D1_POINT_2F& mouseScreenPos);
    void ClearDragOverState();

    // ===== 坐标转换 =====
    D2D1_POINT_2F ScreenToContent(const D2D1_POINT_2F& screenPoint) const;

    // ===== 成员变量 =====
    ViewMode      m_viewMode;
    int           m_columns;
    float         m_hSpacing, m_vSpacing;
    float         m_itemWidth, m_itemHeight;
    bool          m_wrapAutoFit;

    CAtlArray<CD2DControlUI*> m_items;

    CD2DScrollUI  m_scrollBar;
    float         m_scrollOffsetY;
    bool          m_scrollBarVisible;
    float         m_totalContentHeight;

    SelectionMode m_selectionMode;
    int           m_selectedIndex;        // 单选当前选中索引，-1 表示无

    // 拖拽状态
    bool          m_draggingEnabled;
    bool          m_isDragging;
    int           m_dragSourceIndex;      // 被拖拽项的原始索引
    D2D1_POINT_2F m_dragStartPoint;      // 拖拽起始点（用于阈值判断）
    int           m_dragInsertIndex;      // 插入指示位置（-1 无效）
	CComPtr<ID2D1Bitmap> m_pDragGhostBitmap;   // 幽灵控件 快照位图
	D2D1_SIZE_F          m_dragGhostSize;      // 位图尺寸（也是源控件尺寸）
    D2D1_POINT_2F m_dragGhostPos;        // 幽灵位置（屏幕坐标）
    ReorderCallback m_reorderCallback;
    void*         m_pReorderUserData;

    // 交互状态
    int           m_hoveredIndex;
    D2D1_POINT_2F m_ptMouseDown;
};