#pragma once
#include "D2DControlUI.h"
#include "D2DRender.h"

// 滚动条方向枚举
enum ScrollBarDirection
{
    Vertical,   // 垂直滚动条
    Horizontal  // 水平滚动条
};

class CD2DScrollBarUI : public CD2DControlUI
{
public:
	
	static void OnScrollCallback(float position, void* userData);


    CD2DScrollBarUI(ScrollBarDirection direction = ScrollBarDirection::Vertical);
    virtual ~CD2DScrollBarUI();

	 using CD2DControlUI::SetPosition; // 引入基类所有 SetPosition 重载
    // 绘制方法
    virtual void Render(CD2DRender* pRender);

    // 设置滚动条方向
    void SetDirection(ScrollBarDirection direction);
    ScrollBarDirection GetDirection() const;

    // 设置滚动范围
    void SetRange(float minRange, float maxRange);
    void GetRange(float& minRange, float& maxRange) const;

    // 设置和获取当前位置
    void SetPosition(float position);
    float GetPosition() const;

    // 设置页面大小（可见区域大小）
    void SetPageSize(float pageSize);
    float GetPageSize() const;

    // 设置滚动步长
    void SetStepSize(float stepSize);
    float GetStepSize() const;

    // 滚动到顶部/底部或左/右
    void ScrollToStart();
    void ScrollToEnd();
    
    // 向前/向后滚动一页
    void ScrollPageUp();
    void ScrollPageDown();
    
    // 向前/向后滚动一个步长
    void ScrollUp();
    void ScrollDown();

    // 显示/隐藏滚动条
    void SetVisible(bool visible);
    bool IsVisible() const;

    // 设置滑块宽度/高度
    void SetThumbSize(float size);
    float GetThumbSize() const;

	//设置滑块是否拖拽
	void SetThumbDragging(bool isDragging);

	bool IsThumbDragging();

    // 设置轨道宽度
    void SetTrackWidth(float width);
    float GetTrackWidth() const;

    // 设置滚动条颜色
    void SetThumbColor(D2D1_COLOR_F color);
    void SetThumbHoverColor(D2D1_COLOR_F color);
    void SetThumbPressedColor(D2D1_COLOR_F color);
    void SetTrackColor(D2D1_COLOR_F color);
    void SetTrackHoverColor(D2D1_COLOR_F color);
    void SetTrackPressedColor(D2D1_COLOR_F color);
    
    // 设置用户数据
    void SetUserData(void* userData);
    void* GetUserData() const;
    // 鼠标事件处理
    virtual bool OnMouseDownL(const D2D1_POINT_2F& point);
    virtual bool OnMouseMove(const D2D1_POINT_2F& point);
    virtual bool OnMouseUpL(const D2D1_POINT_2F& point);
    virtual bool OnMouseLeave(const D2D1_POINT_2F& point);
    virtual bool OnMouseWheel(float delta);


    // 回调函数类型定义
    typedef void (*ScrollCallback)(float position, void* userData);
    
    // 设置滚动回调
    void SetScrollCallback(ScrollCallback callback, void* userData = nullptr);

protected:
    // 计算滑块矩形
    D2D1_RECT_F CalculateThumbRect() const;
    
    // 计算轨道矩形
    D2D1_RECT_F CalculateTrackRect() const;
    
    // 根据鼠标位置更新滑块位置
    void UpdateThumbPositionFromPoint(const D2D1_POINT_2F& point);
    
    // 限制位置在有效范围内
    void ClampPosition();
    
    // 触发滚动回调
    void TriggerScrollCallback();

public:
    ScrollBarDirection m_direction;      // 滚动条方向
    float m_minRange;                    // 最小范围
    float m_maxRange;                    // 最大范围
    float m_position;                    // 当前位置
    float m_pageSize;                    // 页面大小
    float m_stepSize;                    // 滚动步长
    float m_thumbSize;                   // 滑块大小
    float m_trackWidth;                  // 轨道宽度
    
  
    float m_dragOffset;                  // 拖动偏移量
    
    // 颜色相关
    D2D1_COLOR_F m_thumbColor;           // 滑块颜色
    D2D1_COLOR_F m_thumbHoverColor;      // 滑块悬浮颜色
    D2D1_COLOR_F m_thumbPressedColor;    // 滑块按下颜色
    D2D1_COLOR_F m_trackColor;           // 轨道颜色
    D2D1_COLOR_F m_trackHoverColor;      // 轨道悬浮颜色
    
    // 回调函数
    ScrollCallback m_scrollCallback;     // 滚动回调函数
    void* m_userData;                    // 用户数据
    
    // 鼠标位置（用于判断悬浮状态）
    D2D1_POINT_2F m_currentMousePos;     // 当前鼠标位置
};