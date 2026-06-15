// CD2DTextArea.h
#pragma once
#include "D2DControlUI.h"
#include "D2DScrollUI.h"

class CD2DTextArea : public CD2DControlUI
{
public:
	CD2DTextArea();
	virtual ~CD2DTextArea();

	void SetText(const CAtlString& text);
	const CAtlString& GetText() const;
	void SetWordWrap(bool wrap);
	bool IsWordWrap() const;

	CD2DScrollUI* GetScrollBar() { return &m_VScroll; }

	virtual void DrawControl(CD2DRender* pRender) override;
	virtual bool OnMouseWheel(float delta, const D2D1_POINT_2F& point) override;
	virtual bool OnMouseDownL(const D2D1_POINT_2F& point) override;
	virtual bool OnMouseMove(const D2D1_POINT_2F& point) override;
	virtual bool OnMouseUpL(const D2D1_POINT_2F& point) override;
	virtual bool OnMouseLeave(const D2D1_POINT_2F& point) override;

private:
	void UpdateScrollState(CD2DRender* pRender);
	float GetTextContentHeight(CD2DRender* pRender, float contentWidth) const;

	CD2DScrollUI m_VScroll;
	float m_scrollOffsetY ;
	float m_textContentHeight;   // 文本布局后总高度（缓存）
	bool m_wordWrap ;
	bool m_scrollBarVisible ;     // 实际可见性（内容超出时自动显示）
};