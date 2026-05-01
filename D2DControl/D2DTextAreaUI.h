#pragma once
#include "D2DControlUI.h"
#include "D2DRender.h"
#include "D2DScrollBarUI.h"

class CD2DTextAreaUI : public CD2DControlUI, public CD2DTextFormat
{
public:
	CD2DTextAreaUI();
	virtual ~CD2DTextAreaUI();

	// 绘制方法
	virtual void Render(CD2DRender* pRender);

	// 启用/禁用垂直滚动条
	void EnableVerticalScroll(bool enable);
	bool IsVerticalScrollEnabled() const;

	// 设置滚动条宽度
	void SetScrollBarWidth(float width);
	float GetScrollBarWidth() const;

	// 获取垂直滚动条
	CD2DScrollBarUI& GetVerticalScroll();

	// 滚动控制方法
	void ScrollToTop();
	void ScrollToBottom();
	void ScrollBy(float deltaY);

	// 获取当前滚动位置
	float GetScrollPositionY() const;

	// 重新计算文本布局（当文本或控件大小改变时调用）
	void RecalculateLayout();

	// 鼠标事件处理
	virtual bool OnMouseDownL(const D2D1_POINT_2F& point);
	virtual bool OnMouseMove(const D2D1_POINT_2F& point);
	virtual bool OnMouseUpL(const D2D1_POINT_2F& point);
	virtual bool OnMouseLeave(const D2D1_POINT_2F& point);
	virtual bool OnMouseWheel(float delta);

	// 调整大小
	virtual void SetSize(float width, float height);

protected:
	// 滚动回调函数
	static void OnVerticalScrollCallback(float position, void* userData);
	void OnVerticalScroll(float position);

	// 计算文本边界大小
	void CalculateTextBounds(CD2DRender* pRender);

	// 更新滚动条状态
	void UpdateScrollBars();

	// 获取文本显示区域（考虑内边距和滚动条）
	D2D1_RECT_F GetTextDisplayRect() const;

protected:
	CD2DScrollBarUI m_verticalScrollBar;   // 垂直滚动条

	bool m_enableVerticalScroll;     // 是否启用垂直滚动
	float m_scrollBarWidth;          // 滚动条宽度

	D2D1_SIZE_F m_textContentSize;   // 文本内容的总大小
	float m_scrollPositionY;         // 当前垂直滚动位置

	bool m_isUpdatingScroll;         // 是否正在更新滚动位置（防止递归）
};