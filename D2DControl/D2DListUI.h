#pragma once
#include "D2DControlUI.h"
#include "D2DRender.h"
#include "D2DScrollBarUI.h"
#include "D2DItemUI.h"
// 前向声明
class CD2DRender;
class CD2DListItemUI;

// 项
class CD2DListItemUI : public CD2DItemUI
{
    
public:
    // 构造函数
    CD2DListItemUI() 
    {
    }

    virtual ~CD2DListItemUI()
    {
    }
   
};


class CD2DListUI : public CD2DControlUI
{
public:

//点击回调
typedef void (*ClickItemCallback)(CD2DListUI* pList, CD2DListItemUI* pItem);

typedef void (*Click2ItemCallback)(CD2DListUI* pList, CD2DListItemUI* pItem);


public:
    CD2DListUI();
    virtual ~CD2DListUI();

    // 滚动回调函数（静态成员函数）
    static void OnScrollCallback(float position, void* userData);

    // 绘制方法
    virtual void Render(CD2DRender* pRender);

    // 添加物品
    void AddItem(CD2DListItemUI* pItem);
    // 删除物品
    void RemoveItem(int nIndex);
    // 清空列表
    void ClearItems();
    // 获取物品数量
    int GetItemCount() const;
    // 获取指定索引的物品
    CD2DListItemUI* GetItem(int nIndex);

    // 选择物品
    void SelectItem(int nIndex);
    // 取消选择
    void UnselectItem();
    // 获取当前选中的索引
    int GetSelectedIndex() const;
    // 获取当前悬停的索引
    int GetHoverIndex() const;
    
   
    // 计算指定索引项的矩形区域
    D2D1_RECT_F GetItemRect(int nIndex) const;
    
    // 根据坐标获取物品索引
    int GetItemIndexAtPoint(const D2D1_POINT_2F& point) const;

    // 设置项高度
    void SetItemHeight(float fHeight);
    // 获取项高度
    float GetItemHeight() const;

	// 每项高度-底部padding
	void SetItemBottomPadding(float fBottomPadding);
	float GetItemBottomPadding() const;

    // 设置字体
    void SetFont(const wchar_t* fontFamilyName, float fontSize);

    // 滚动相关
    void SetScrollPosition(float fScrollPos);
    float GetScrollPosition() const;
    float GetMaxScrollPosition() const;

	CD2DScrollBarUI& GetVerticalScroll();

    // 设置/获取 点击回调函数
    void SetOnClickItemCallback(ClickItemCallback callback);
    ClickItemCallback GetOnClickCallback() const;

	// 设置点击回调函数
	void SetOnClick2ItemCallback(Click2ItemCallback callback);
	Click2ItemCallback GetOnClick2Callback() const;
    
    // 鼠标事件处理
    virtual bool OnMouseDownL(const D2D1_POINT_2F& point);
    virtual bool OnMouseMove(const D2D1_POINT_2F& point);
    virtual bool OnMouseUpL(const D2D1_POINT_2F& point);
    virtual bool OnMouseLeave(const D2D1_POINT_2F& point);

	virtual bool OnMouseDownL2(const D2D1_POINT_2F& point);
	virtual bool OnMouseUpL2(const D2D1_POINT_2F& point);

    virtual bool OnMouseWheel(float delta);

public:
    CAutoPtrArray<CD2DListItemUI> m_items;  // 列表数组
    float m_fItemHeight;                  // 每项高度
	float m_fBottomPadding;				  // 每项底部Padding
    int m_nSelectedIndex;                // 当前选中项索引
    int m_nHoverIndex;                   // 当前悬停项索引
    float m_fScrollPosition;             // 滚动位置
    // 滚动条相关
    CD2DScrollBarUI m_VerticalScrollBar; // 垂直滚动条
    bool m_IsShowScrollBar;                 // 是否显示滚动条

    ClickItemCallback m_clickItemCallback; // 点击回调
	Click2ItemCallback m_click2ItemCallback; // 点击回调


};