#pragma once

#include "D2DControlUI.h"
#include "D2DScrollBarUI.h"
#include "D2DText.h"
#include <atlcoll.h>
#include <atlbase.h>

class CD2DSelectUI : public CD2DControlUI, public CD2DTextFormat
{
public:
    CD2DSelectUI();
    virtual ~CD2DSelectUI();

    // 滚动回调函数（静态成员函数）
    static void OnScrollCallback(float position, void* userData);

    // 绘制函数（重写基类方法）
    virtual void Render(CD2DRender* pRender) ;
    virtual bool IsPointInRect(const D2D1_POINT_2F& point) const ;

    // 鼠标事件处理（重写基类方法）
    virtual bool OnMouseDownL(const D2D1_POINT_2F& point) override;
    virtual bool OnMouseMove(const D2D1_POINT_2F& point) override;
    virtual bool OnMouseUpL(const D2D1_POINT_2F& point) override;
    virtual bool OnMouseLeave(const D2D1_POINT_2F& point) override;
    virtual bool OnMouseWheel(float delta) override;

    // 组合框特有功能
    void AddItem(const CString& text);
    void InsertItem(int index, const CString& text);
    void RemoveItem(int index);
    void ClearItems();
    int GetItemCount() const;
    CString GetItemText(int index) const;
    void SetItemText(int index, const CString& text);

    void SetCurSel(int index);
    int GetCurSel() const;
    void SetCurSelText(const CString& text);
    CString GetCurSelText() const;

    void ShowDropDown(bool show);
    bool IsDropDownVisible() const;

    void ScrollToItem(int index);

    void SetItemHeight(float height);
    float GetItemHeight() const { return m_itemHeight; }

    void SetDropDownMaxHeight(float height);
    float GetDropDownMaxHeight() const;


    void SetButtonColor(const D2D1_COLOR_F& color);
    D2D1_COLOR_F GetButtonColor() const;

    void SetButtonBorderColor(const D2D1_COLOR_F& color);
    D2D1_COLOR_F GetButtonBorderColor() const;

	CD2DScrollBarUI& GetVerticalScroll();
    // 回调函数类型定义
    typedef void (*OnItemClickCallback)(CD2DSelectUI* pSelect, int itemIndex, void* userData);
    void SetOnItemClickCallback(OnItemClickCallback callback, void* userData = nullptr);

protected:
    // 计算下拉列表的矩形区域
    D2D1_RECT_F GetDropDownRect() const;

    // 计算指定索引项的矩形区域（相对于下拉列表顶部）
    D2D1_RECT_F GetItemRect(int index) const;

    // 根据鼠标位置获取选项索引
    int GetItemIndexFromPoint(const D2D1_POINT_2F& point) const;

    // 绘制下拉按钮
    void DrawDropDownButton(CD2DRender* pRender, const D2D1_RECT_F& buttonRect);

    // 绘制下拉列表
    void DrawDropDownList(CD2DRender* pRender);

    // 绘制选中项文本
    void DrawSelectedText(CD2DRender* pRender, const D2D1_RECT_F& textRect);

    // 更新滚动条
    void UpdateScrollBar();

    // 根据滚动位置计算可见项范围
    void GetVisibleItemsRange(int& startIndex, int& endIndex) const;

private:
    // 选项数据
    CAtlArray<CString> m_items;          // 选项数组
    int                m_curSel;          // 当前选中索引
    bool               m_isDropDownVisible; // 下拉列表是否可见

    // 尺寸相关
    float              m_dropDownMaxHeight; // 下拉列表最大高度
    float              m_itemHeight;        // 每个选项的高度
    float              m_buttonWidth;       // 下拉按钮宽度

    // 颜色相关
    D2D1_COLOR_F       m_selectedBgColor;   // 选中项背景色
    D2D1_COLOR_F       m_buttonColor;       // 下拉按钮颜色
    D2D1_COLOR_F       m_buttonBorderColor; // 下拉按钮边框颜色

    // 状态相关
    int                m_hoverItemIndex;    // 鼠标悬停的选项索引
    bool               m_isDropDownClicked; // 是否点击了下拉按钮（用于状态跟踪）

    // 滚动条相关
    CD2DScrollBarUI    m_verticalScroll;    // 垂直滚动条
    float              m_scrollOffset;      // 滚动偏移量
    bool               m_needScrollBar;     // 是否需要显示滚动条

    // 回调相关
    OnItemClickCallback m_onItemClickCallback; // 选项点击回调函数
};