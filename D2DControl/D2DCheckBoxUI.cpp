#include "stdafx.h"
#include "D2DControlUI.h"
#include "D2DRender.h"
#include "D2DText.h"
#include "D2DCheckBoxUI.h"

/**
 * @brief 构造函数实现
 * 初始化复选框的基本属性和状态
 */
CD2DCheckBoxUI::CD2DCheckBoxUI() : 
    m_checked(false),
    m_mouseHover(false),
    m_boxSize(16.0f),
    m_spacing(6.0f),
    m_checkChangedCallback(nullptr)
    {
    
    // 设置默认颜色
    m_boxBgColor = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f); // 白色背景
    m_boxBorderColor = D2D1::ColorF(0.4f, 0.4f, 0.4f, 1.0f); // 灰色边框
    m_checkMarkColor = D2D1::ColorF(0.0f, 0.5f, 1.0f, 1.0f); // 蓝色对勾
    m_boxBgColorHover = D2D1::ColorF(0.95f, 0.95f, 0.95f, 1.0f); // 悬停时浅灰色背景
    m_boxBorderColorHover = D2D1::ColorF(0.6f, 0.6f, 0.6f, 1.0f); // 悬停时深灰色边框
    
    // 设置默认字体
    SetTextFont(L"微软雅黑", 8.0f);
    // 设置默认文本颜色
    SetTextColor(D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f)); // 黑色文本
}

/**
 * @brief 析构函数实现
 */
CD2DCheckBoxUI::~CD2DCheckBoxUI() {
    // 不需要特殊清理，基类会处理相关资源
}

/**
 * @brief 绘制函数实现
 * @param render 渲染器指针
 */
void CD2DCheckBoxUI::Render(CD2DRender* pRender) {
	if (!pRender || !IsVisible()) {
		return;
	}
	if (IsBackgroundVisible()) {
		pRender->FillRoundedRectangle(GetRectangle(), m_radiusX, m_radiusY, GetBackgroundColor());
	}

	if (IsBorderVisible()) {
		pRender->DrawRoundedRectangle(GetRectangle(), m_radiusX, m_radiusY, GetBorderColor());
	}

	// 获取控件位置和大小
	float x = GetPosition().x;
	float y = GetPosition().y;
	float width = GetWidth();
	float height = GetHeight();

	// 计算方框位置（居中显示）
	float boxX = x ; 
	float boxY = y + (height - m_boxSize) * 0.5f;

	// 获取要使用的颜色
	D2D1_COLOR_F bgColor = m_mouseHover ? m_boxBgColorHover : m_boxBgColor;
	D2D1_COLOR_F borderColor = m_mouseHover ? m_boxBorderColorHover : m_boxBorderColor;

	// 绘制方框背景
	pRender->FillRectangle(D2D1::RectF(boxX, boxY, boxX + m_boxSize, boxY + m_boxSize), bgColor);

	// 绘制方框边框
	pRender->DrawRectangle(D2D1::RectF(boxX, boxY, boxX + m_boxSize, boxY + m_boxSize), borderColor, 1.0f);

	// 如果选中，绘制对勾
	if (m_checked) {
		// 优化对勾的坐标计算，使其更加自然
		float checkStartX = boxX + m_boxSize * 0.15f;  // 稍微向左移动起始点
		float checkStartY = boxY + m_boxSize * 0.5f;   // 垂直居中起始
		float checkMidX = boxX + m_boxSize * 0.4f;     // 中间点X
		float checkMidY = boxY + m_boxSize * 0.75f;    // 中间点Y
		float checkEndX = boxX + m_boxSize * 0.85f;    // 结束点X
		float checkEndY = boxY + m_boxSize * 0.25f;    // 结束点Y

		D2D1_POINT_2F checkMark[3];
		checkMark[0] = D2D1::Point2F(checkStartX, checkStartY);
		checkMark[1] = D2D1::Point2F(checkMidX, checkMidY);
		checkMark[2] = D2D1::Point2F(checkEndX, checkEndY);
		// 增加线宽并可能使用圆角线条
		float lineWidth = 2.0f; // 增加线宽

		pRender->DrawPolygon(checkMark, 3, m_checkMarkColor, lineWidth, true);
	}

	// 绘制文本
	if (m_text.IsEmpty() == false) {
		// 计算文本位置（方框右侧）
		float textX = boxX + m_boxSize + m_spacing;
		float textY = y;
		float textWidth = width - (m_boxSize + m_spacing); // 调整文本宽度计算
		float textHeight = height;
		D2D1_RECT_F textRect = D2D1::RectF(textX, textY, textX + textWidth, textY + textHeight);

		// 创建文本格式
		if(m_textFormat == nullptr) m_textFormat = pRender->CreateTextFormat(
			GetFontName(),
			GetFontSize(),
			GetFontWeight(),
			GetHorizontalAlignment(),
			GetVerticalAlignment());

		// 绘制文本
		pRender->DrawText(m_text, textRect, GetTextColor(), m_textFormat);
	}
}

/**
 * @brief 设置复选框状态
 * @param checked 是否选中
 */
void CD2DCheckBoxUI::SetChecked(bool checked) {
    if (m_checked != checked) {
        m_checked = checked;
        
        // 如果设置了回调函数，触发回调
        if (m_checkChangedCallback) {
            m_checkChangedCallback(this, m_checked, GetUserData());
        }
    }
}

/**
 * @brief 获取复选框状态
 * @return 是否选中
 */
bool CD2DCheckBoxUI::IsChecked() const {
    return m_checked;
}

/**
 * @brief 设置方框背景颜色
 * @param color 背景颜色
 */
void CD2DCheckBoxUI::SetBoxBackgroundColor(const D2D1_COLOR_F& color) {
    m_boxBgColor = color;
}

/**
 * @brief 获取方框背景颜色
 * @return 背景颜色
 */
D2D1_COLOR_F CD2DCheckBoxUI::GetBoxBackgroundColor() const {
    return m_boxBgColor;
}

/**
 * @brief 设置对勾颜色
 * @param color 对勾颜色
 */
void CD2DCheckBoxUI::SetCheckMarkColor(const D2D1_COLOR_F& color) {
    m_checkMarkColor = color;
}

/**
 * @brief 获取对勾颜色
 * @return 对勾颜色
 */
D2D1_COLOR_F CD2DCheckBoxUI::GetCheckMarkColor() const {
    return m_checkMarkColor;
}

/**
 * @brief 设置方框边框颜色
 * @param color 边框颜色
 */
void CD2DCheckBoxUI::SetBoxBorderColor(const D2D1_COLOR_F& color) {
    m_boxBorderColor = color;
}

/**
 * @brief 获取方框边框颜色
 * @return 边框颜色
 */
D2D1_COLOR_F CD2DCheckBoxUI::GetBoxBorderColor() const {
    return m_boxBorderColor;
}

/**
 * @brief 设置方框大小
 * @param size 方框大小
 */
void CD2DCheckBoxUI::SetBoxSize(float size) {
    m_boxSize = size;
}

/**
 * @brief 获取方框大小
 * @return 方框大小
 */
float CD2DCheckBoxUI::GetBoxSize() const {
    return m_boxSize;
}

/**
 * @brief 设置方框与文本之间的间距
 * @param spacing 间距值
 */
void CD2DCheckBoxUI::SetSpacing(float spacing) {
    m_spacing = spacing;
}

/**
 * @brief 获取方框与文本之间的间距
 * @return 间距值
 */
float CD2DCheckBoxUI::GetSpacing() const {
    return m_spacing;
}

/**
 * @brief 鼠标左键按下事件处理
 * @param point 鼠标坐标点
 * @return 是否处理了事件
 */
bool CD2DCheckBoxUI::OnMouseDownL(const D2D1_POINT_2F& point) {
    if (!IsVisible() || !IsEnabled() || !IsPointInRect(point)) {
        return false;
    }
    
    // 切换选中状态
    SetChecked(!m_checked);
    
    return true;
}

/**
 * @brief 鼠标移动事件处理
 * @param point 鼠标坐标点
 * @return 是否处理了事件
 */
bool CD2DCheckBoxUI::OnMouseMove(const D2D1_POINT_2F& point) {
    if (!IsVisible() || !IsEnabled()) {
        return false;
    }
    
    // 检查鼠标是否在控件范围内
    bool isInside = IsPointInRect(point);
    
    // 更新悬停状态
    if (m_mouseHover != isInside) {
        m_mouseHover = isInside;
        
    }
    return isInside;
}

/**
 * @brief 鼠标离开事件处理
 * @param point 鼠标坐标点
 * @return 是否处理了事件
 */
bool CD2DCheckBoxUI::OnMouseLeave(const D2D1_POINT_2F& point) {
    if (!IsVisible() || !IsEnabled() || !m_mouseHover || IsPointInRect(point)) {
        return false;
    }
    
    // 清除悬停状态
    m_mouseHover = false;
    
    return true;
}

/**
 * @brief 设置选中状态改变回调函数
 * @param callback 回调函数指针
 * @param userData 用户自定义数据
 */
void CD2DCheckBoxUI::SetOnCheckChangedCallback(OnCheckClickCallback callback, void* userData) {
    m_checkChangedCallback = callback;
    SetUserData(userData);
}
