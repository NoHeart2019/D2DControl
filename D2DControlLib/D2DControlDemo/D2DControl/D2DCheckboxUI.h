// CD2DCheckboxUI.h
#pragma once
#include "D2DControlUI.h"
#include <atlcoll.h>

// 复选框专用样式（组合 box 外观、文本样式、对勾颜色）
struct CD2DCheckboxStyle
{
	CD2DStyle      box;        // 勾选框的背景、边框、圆角等
	CD2DTextStyle  text;       // 文本样式
	D2D1_COLOR_F   checkmark;  // 对勾颜色

	CD2DCheckboxStyle()
	{
		// 默认白色勾选框，浅灰边框，圆角 3
		box.background.type = CD2DBackground::Solid;
		box.background.solid.color = D2D1::ColorF(1.0f, 1.0f, 1.0f);
		box.border.top = CD2DBorderEdge(1.5f, Solid, D2D1::ColorF(0.5f, 0.5f, 0.5f));
		box.border.right = box.border.top;
		box.border.bottom = box.border.top;
		box.border.left = box.border.top;
		box.radius = CD2DRadius(3.0f);

		// 默认文本黑色，14 号
		text.color = D2D1::ColorF(0.0f, 0.0f, 0.0f);
		text.fontSize = 14.0f;

		// 默认白色对勾
		checkmark = D2D1::ColorF(0.2f, 0.5f, 1.0f);
	}
};

class CD2DCheckboxUI : public CD2DControlUI
{
public:
	CD2DCheckboxUI();
	virtual ~CD2DCheckboxUI();

	// ----- 选中状态 -----
	void SetChecked(bool checked)   { GetStatus().SetChecked(checked); }
	bool IsChecked() const          { return GetStatus().IsChecked(); }
	void Toggle()                   { SetChecked(!IsChecked()); }
	// ----- 样式管理 -----
	void SetBoxStyle(ED2DStatus state, const CD2DCheckboxStyle& style);
	const CD2DCheckboxStyle& GetBoxStyle(ED2DStatus state) const;
	void SetDefaultBoxStyle(const CD2DCheckboxStyle& style) { m_defaultBoxStyle = style; }
	const CD2DCheckboxStyle& GetDefaultBoxStyle() const     { return m_defaultBoxStyle; }
	void ApplyCheckBoxStyle(CD2DCheckboxStyle& dest, const CD2DCheckboxStyle& src) const;

	// ----- 回调 -----
	typedef void (WINAPI *CheckedChangedCallback)(CD2DCheckboxUI* pCheckbox, bool checked, void* pUserData);
	void SetOnCheckedChanged(CheckedChangedCallback cb, void* pUserData = nullptr);

	// ----- 重写基类 -----
	virtual void DrawControl(CD2DRender* pRender) override;
	virtual void Layout() ;
	virtual D2D1_SIZE_F MeasureContent(CD2DRender* pRender, float maxWidth = FLT_MAX, float maxHeight = FLT_MAX) const override;

protected:
	// 根据当前状态获取有效样式（先查映射，回退到默认）
	CD2DCheckboxStyle GetEffectiveBoxStyle() const;
	ED2DStatus	GetEffectiveState() const;          // 实现略
	void DrawCheckboxFrame(CD2DRender* pRender, const D2D1_RECT_F& checkRect, const CD2DStyle& boxStyle);
	void DrawCheckmark(CD2DRender* pRender, const D2D1_RECT_F& checkRect, D2D1_COLOR_F color);
	void NotifyCheckedChanged();

private:
	CAtlMap<ED2DStatus, CD2DCheckboxStyle> m_boxStyles;   // 各状态样式
	CD2DCheckboxStyle                     m_defaultBoxStyle; // 默认样式

	CheckedChangedCallback m_checkedCallback;
	void*                  m_pCallbackUserData;
};