/*
	处理D2D 控件事件
*/
#pragma once
#include "D2DScrollBarUI.h"
class CD2DContainerUI : public CD2DControlUI
{
public:
	static void OnScrollCallback(float position, void* userData);	
public:
	CD2DContainerUI();
	~CD2DContainerUI();
public:
	// 绘制
	virtual void Render(CD2DRender* pRender);

public:
	virtual bool OnMouseDownL(const D2D1_POINT_2F& point);
	virtual bool OnMouseUpL(const D2D1_POINT_2F& point);
	virtual bool OnMouseDownL2(const D2D1_POINT_2F& point);
	virtual bool OnMouseUpL2(const D2D1_POINT_2F& point);

	virtual bool OnMouseMove(const D2D1_POINT_2F& point);
	virtual bool OnMouseLeave(const D2D1_POINT_2F& point);

	virtual bool OnKeyDown(DWORD keyCode);
	virtual bool OnKeyUp(DWORD keyCode);
	virtual bool OnChar(DWORD ch);


    void AddControlUI(CD2DControlUI* controlUI);
	void RemoveAll();

	int GetControlUICount() const;
	CD2DControlUI* GetControlUIAt(int nIndex) ;

	// 垂直滚动条控制
	void EnableVerticalScroll(bool enable);
	bool IsVerticalScrollEnabled() const;
	
	void  SetVerticalScroll();

	void  SetTotalHeight(float fTotalHeight);
	float GetTotalHeight() const;

	void  SetVerticalPosition(float fPosition);
	float GetVerticalPosition();

	CD2DScrollBarUI& GetVerticalScroll();
	const CD2DScrollBarUI& GetVerticalScroll() const;

	// 重新布局（不重新测量，仅应用当前滚动偏移）
	virtual void Relayout();
public:
	// 存储所有控件的数组，用于遍历处理
	CAtlArray<CD2DControlUI*> m_controls;

	CD2DScrollBarUI m_verticalScroll;
	bool            m_enableVerticalScroll;
	float	        m_fTotalHeight;
	float           m_fScrollPosition; 
};