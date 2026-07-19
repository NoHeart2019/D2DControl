// CD2DScrollUI.h
#pragma once
// ==================== 辅助结构：滚动条内部样式 ====================
#include "D2DControlUI.h"
struct CD2DScrollStyle
{
	CD2DStyle      track;           // 轨道背景、边框、圆角
	CD2DStyle      thumb;           // 滑块背景、边框、圆角
	float          thumbMinLength;  // 滑块最小长度（像素）

	CD2DScrollStyle()
	{
		// 轨道：浅灰色背景，无边框，无圆角
		track.background.type = CD2DBackground::Solid;
		track.background.solid.color = D2D1::ColorF(0.9f, 0.9f, 0.9f);
		track.radius = CD2DRadius(4.0f);

		// 滑块：深灰色背景，小圆角（4px），无边框
		thumb.background.type = CD2DBackground::Solid;
		thumb.background.solid.color = D2D1::ColorF(0.6f, 0.6f, 0.6f);
		thumb.radius = CD2DRadius(4.0f);

		thumbMinLength = 20.0f;
	}
};
// 滚动条控件
class CD2DScrollUI : public CD2DControlUI
{
public:
	// 滚动条方向
	enum Orientation
	{
		Horizontal,     // 水平滚动条
		Vertical        // 垂直滚动条
	};

	CD2DScrollUI();
	virtual ~CD2DScrollUI();

	// ==================== 方向 ====================
	void SetOrientation(Orientation orient);
	Orientation GetOrientation() const;

	// ==================== 滚动参数 ====================
	void SetRange(int minValue, int maxValue);          // 设置滚动范围（最小值，最大值）
	void GetRange(int& minValue, int& maxValue) const;
	void SetValue(int value);                            // 设置当前滚动值
	int  GetValue() const;
	void SetPageSize(int pageSize);                      // 设置视口大小（影响滑块比例）
	int  GetPageSize() const;

	// 便捷接口：根据内容总长度、视口长度、当前位置一次性设置
	void SetScrollInfo(int contentLen, int viewportLen, int pos);

	// ==================== 滚动回调 ====================
	typedef void (*ScrollCallback)(CD2DScrollUI* pScrollBar, int newValue, void* pUserData);
	void SetScrollCallback(ScrollCallback callback, void* pUserData = nullptr);

	// ==================== 样式管理（轨道+滑块） ====================
	void SetScrollStyle(ED2DStatus state, const CD2DScrollStyle& style);   // 设置指定状态的样式
	
	CD2DScrollStyle GetEffectiveScrollStyle() const;                       // 根据当前交互状态获取有效样式

	// ==================== 重写基类方法 ====================
	virtual D2D1_SIZE_F MeasureContent(CD2DRender* pRender, float maxWidth = FLT_MAX, float maxHeight = FLT_MAX) const override;
	virtual void DrawControl(CD2DRender* pRender) override;
	virtual bool OnMouseDownL(const D2D1_POINT_2F& point) override;
	virtual bool OnMouseMove(const D2D1_POINT_2F& point) override;
	virtual bool OnMouseUpL(const D2D1_POINT_2F& point) override;
	virtual bool OnMouseLeave(const D2D1_POINT_2F& point) override;
	virtual bool OnMouseWheel(float delta, const D2D1_POINT_2F& point);

	bool isThumbDragging(){ return m_isDragging;}

protected:
	// 内部辅助函数
	void UpdateThumbRect(const D2D1_RECT_F& trackRect);   // 根据当前滚动值和轨道区域更新滑块矩形
	int  ValueFromPoint(const D2D1_POINT_2F& point) const; // 根据鼠标位置计算对应的滚动值
	void NotifyScroll();                                   // 触发滚动回调

private:
	// 滚动参数
	Orientation m_orientation;      // 滚动条方向（水平/垂直）
	int m_minValue;                 // 滚动最小值
	int m_maxValue;                 // 滚动最大值（最大可滚动位置，不含 pageSize）
	int m_currentValue;             // 当前滚动值（在 [m_minValue, m_maxValue - m_pageSize + 1] 范围内）
	int m_pageSize;                 // 视口大小，影响滑块占轨道的比例（默认 1）

	// 拖拽状态
	bool m_isDragging;              // 是否正在拖拽滑块
	D2D1_POINT_2F m_dragStartPoint; // 拖拽起始时的鼠标位置（逻辑坐标，未缩放）
	int m_dragStartValue;           // 拖拽起始时的滚动值

	// 几何缓存（用于绘制和命中测试）
	D2D1_RECT_F m_thumbRect;        // 滑块矩形（逻辑坐标，单位像素）
	D2D1_RECT_F m_trackRect;        // 轨道矩形（逻辑坐标，在 DrawControl 中计算）

	// 回调
	ScrollCallback m_scrollCallback; // 滚动值改变时的回调函数指针
	void* m_callbackUserData;        // 回调的用户自定义数据

	// 样式系统（轨道 + 滑块）
	CAtlMap<ED2DStatus, CD2DScrollStyle> m_scrollStyles;   // 各交互状态对应的样式映射
	CD2DScrollStyle m_defaultScrollStyle;                  // 默认样式（当未找到特定状态样式时使用）
};

