#pragma once
#include "D2DBoxModel.h"
#include "D2DSizePolicy.h"
#include "D2DRender.h"

class CD2DControlUI : public CD2DBoxModel{
public:
    CD2DControlUI();
    virtual ~CD2DControlUI();

    void SetPosition(float x, float y);
    virtual void SetSize(float width, float height);
	void SetContentSize(float width, float height);
    void SetRectangle(const D2D1_RECT_F& rect);
	void SetBorderRadius(float radiusX, float radiusY);
	void SetCenter(float x, float y);
	void SetTranslate(float x, float y);

	void SetBorderRadius(float radius);
	void SetBorderRadius(float left, float top, float right, float bottom);

	float GetBorderRadiusX() const;
	float GetBorderRadiusY() const;
	float GetTranslateX() const;
	float GetTranslateY() const;
	
	//大小策略
	const CD2DSizePolicy& GetSizePolicy() const;
	void SetSizePolicy(CD2DSizePolicy::Policy policy);


    virtual D2D1_RECT_F GetRectangle() const;
	virtual D2D1_ROUNDED_RECT GetRoundedRectangle() const;

    D2D1_POINT_2F GetPosition() const;
    D2D1_SIZE_F GetSize() const;
	D2D1_POINT_2F GetCenter() const;
	float GetWidth() const;
	float GetHeight() const;

    void Move(float offsetX, float offsetY);
	void MoveTo(float x, float y);
	void Inflate(float dx, float dy);

    void Resize(float newWidth, float newHeight, bool keepCenter);
	// 检查点是否在矩形内
	bool IsPointInRect(const D2D1_POINT_2F& point, const D2D1_RECT_F& rect) const;
	// 检查点是否在矩形的边上
	bool IsPointOnRect(const D2D1_POINT_2F& point, const D2D1_RECT_F& rect, float tolerance = 1.0f) const;

    virtual bool IsPointInRect(const D2D1_POINT_2F& point) const;
    bool IsPointInRect(float x, float y) const;
    bool IsPointOnRectEdge(const D2D1_POINT_2F& point, float tolerance = 1.0f) const;
    bool IsPointOnRectEdge(float x, float y, float tolerance = 1.0f) const;

	// 检查点是否在控件内
	virtual bool IsHitTest(const D2D1_POINT_2F& point) const;

	void SetFocused(bool focused);
	bool IsFocused() const;

    void SetVisible(bool visible);
    bool IsVisible() const;

    void SetEnabled(bool enabled);
    bool IsEnabled() const;

	bool IsHovered()const;
	bool IsDragging()const;
	bool IsPressed()const;
	bool IsSelected()const;

	void SetHovered(bool hover);
	void SetDragging(bool dragging);
	void SetPressed(bool pressed);
    void SetSelected(bool selected);
	

	// 颜色处理方法
	// 设置和获取背景颜色
	void SetBackgroundColor(const D2D1_COLOR_F& color);
	D2D1_COLOR_F GetBackgroundColor() const;

	void SetLinearColor(const D2D1_COLOR_F& color1, const D2D1_COLOR_F& color2, float degress = 135.0f);

	// 前景色相关方法
	void SetForegroundColor(const D2D1_COLOR_F& color);
	D2D1_COLOR_F GetForegroundColor() const;
	
	//状态颜色
	//设置按下颜色
	void SetNormalColor(D2D1_COLOR_F normalColor);
	D2D1_COLOR_F GetNormalColor();

	//设置按下颜色
	void SetPressedColor(D2D1_COLOR_F pressedColor);
	D2D1_COLOR_F GetPressedColor();

	//设置悬浮颜色
	void SetHoveredColor(D2D1_COLOR_F hoveredColor);
	D2D1_COLOR_F GetHoveredColor();

	//设置选中颜色
	void SetSelectedColor(D2D1_COLOR_F selectedColor);
	D2D1_COLOR_F GetSelectedColor();

	// 设置和获取边框颜色
	void SetBorderColor(const D2D1_COLOR_F& color);
	D2D1_COLOR_F GetBorderColor() const;
	
	// 设置和获取边框宽度
	void SetBorderWidth(float width);
	float GetBorderWidth() const;

	// 设置用户数据
	void SetUserData(void* userData);
	void* GetUserData() const;


	// 设置背景是否可见
	void SetBackgroundVisible(bool visible);
	bool IsBackgroundVisible() const;

	// 设置边框是否可见
	void SetBorderVisible(bool visible);
	bool IsBorderVisible() const;

	void SetBorderSolid(bool solid);
	bool IsBorderSolid() const;

	
	virtual D2D1_SIZE_F Measure(const D2D1_SIZE_F& availableSize, CD2DRender* pRender = nullptr);

public:
	virtual void Render(CD2DRender* pRender);
	CComPtr<ID2D1Brush> GetBackgroundBrush(CD2DRender* pRender);	

public:
	virtual bool OnMouseDownL(const D2D1_POINT_2F& point);
	virtual bool OnMouseMove(const D2D1_POINT_2F& point);
	virtual bool OnMouseUpL(const D2D1_POINT_2F& point);
	virtual bool OnMouseDownL2(const D2D1_POINT_2F& point);
	virtual bool OnMouseUpL2(const D2D1_POINT_2F& point);

	virtual bool OnMouseLeave(const D2D1_POINT_2F& point);
	virtual bool OnMouseEnter(const D2D1_POINT_2F& point);

	virtual bool OnMouseWheel(float delta);
	virtual bool OnKeyDown(DWORD keyCode);
	virtual bool OnKeyUp(DWORD keyCode);
	virtual bool OnChar(DWORD ch);
	virtual void OnDpiChanged(float dpiScaleX, float dpiScaleY);


protected:
    D2D1_RECT_F m_rectangle;       
	float m_radiusX;
	float m_radiusY;
	
	D2D1_RECT_F m_borderRadius;

	bool m_isFocused;
    bool m_isVisible;         
    bool m_isEnabled;         
    bool m_isHover;        
    bool m_isPressed; 
	bool m_isSeleced;
	bool m_isDragging; 
	bool m_isBackgroundVisible; // 背景是否可见
	bool m_isBorderVisible;     // 边框是否可见     
	
	
    float m_dpiScaleX;         
    float m_dpiScaleY;        

	// 颜色相关成员变量
	D2D1_COLOR_F m_backgroundColor;			// 默认背景颜色
	D2D1_COLOR_F m_foregroundColor;			// 默认前景色
	
	D2D1_COLOR_F  m_LinearColor[2]; //线性画刷
	float		  m_LinearDegree;	//线性角度

	D2D1_COLOR_F m_borderColor;				// 默认边框颜色
	float		 m_borderWidth;				// 边框宽度
	bool		 m_isBorderSolid;           // 实线
	D2D1_COLOR_F m_normalColor;				// 正常颜色
	D2D1_COLOR_F m_hoveredColor;			// 悬浮颜色
	D2D1_COLOR_F m_pressedColor;            // 按下颜色
	D2D1_COLOR_F m_selectedColor;           // 选中颜色

	//容器计算
	CD2DSizePolicy m_sizePolicy;			// 大小策略
	

	//
	float        m_translateX;				// X 平移
	float        m_translateY;              // Y 平移

	void* m_userData;						// 用户数据
};