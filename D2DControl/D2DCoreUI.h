#pragma once


class CD2DLabel : public CD2DControlUI, public CD2DTextFormat {
public:
	CD2DLabel();
	virtual ~CD2DLabel();

	
	void SetTextFormat(IDWriteTextFormat* textFormat)
	{	
		m_textFormat = textFormat;
	}

	IDWriteTextFormat* GetTextFormat(){return m_textFormat;}


	//前多少个字符需要加粗 (基于字符数)
	void SetBoldLength(int boldCharCount); 

	// 获取当前粗体长度
	int GetBoldLength() const { return m_boldLength;}

public:
	// 绘制标签
	virtual void Render(CD2DRender* render);

	virtual void DrawBackground(CD2DRender* render);

	virtual void DrawText(CD2DRender* render);

	virtual void DrawBorder(CD2DRender* render);


	typedef void (*OnClickCallback)(CD2DLabel* pLabel, void* userData);

	void SetOnClickCallback(OnClickCallback callback, void* userData);

	virtual D2D1_SIZE_F Measure(const D2D1_SIZE_F& availableSize, CD2DRender* pRender);


public:
	virtual bool OnMouseDownL(const D2D1_POINT_2F& point);
	


public:
	OnClickCallback            m_onClickCallback; //点击回调
	CComPtr<IDWriteTextFormat> m_textFormat;
	int					   m_boldLength;     // 粗体部分的字符长度
};

class CD2DButton : public CD2DControlUI, public CD2DTextFormat {
public:
	CD2DButton();
	virtual ~CD2DButton();

	void SetText(const CString& text);
	CString GetText() const;

	void SetTextFormat(IDWriteTextFormat* textFormat);

	IDWriteTextFormat* GetTextFormat();

	virtual D2D1_SIZE_F Measure(const D2D1_SIZE_F& availableSize, CD2DRender* pRender);

	// 绘制按钮
	virtual void Render(CD2DRender* render);

	// 鼠标事件处理
	virtual bool OnMouseDownL(const D2D1_POINT_2F& point);
	virtual bool OnMouseMove(const D2D1_POINT_2F& point);
	virtual bool OnMouseUpL(const D2D1_POINT_2F& point);

	// 按钮点击回调
	typedef void (WINAPI*  OnClickCallback)(CD2DButton* pButton,  void* userData);

	void SetOnClickCallback(OnClickCallback callback, void* userData = nullptr);


	// 设置按钮圆角
	void SetCornerRadius(float radius);

private:
	CComPtr<IDWriteTextFormat> m_textFormat;
	OnClickCallback m_clickCallback;
};

class CD2DCheckButton : public CD2DControlUI, public CD2DTextFormat {
public:
	CD2DCheckButton();
	virtual ~CD2DCheckButton();

	// 绘制按钮
	virtual void Render(CD2DRender* render);

	// 鼠标事件处理
	virtual bool OnMouseDownL(const D2D1_POINT_2F& point);
	virtual bool OnMouseMove(const D2D1_POINT_2F& point);
	virtual bool OnMouseUpL(const D2D1_POINT_2F& point);

	// 按钮点击回调
	typedef void (WINAPI*  OnClickCallback)(CD2DCheckButton* pButton, void* userData);

	void SetOnClickCallback(OnClickCallback callback, void* userData = nullptr);


	// 设置按钮圆角
	void SetCornerRadius(float radius);

private:
	
	OnClickCallback m_clickCallback;
	CComPtr<IDWriteTextFormat> m_textFormat;
};

void SetLabelRectangle(__inout D2D1_RECT_F& rectangle, CD2DLabel& label, CD2DRender* pRender, bool isReal = false);

D2D1_SIZE_F MeasureControlSize(CD2DRender* pRender, CD2DControlUI& control, const CD2DTextFormat& textFormat, float maxWidth = FLT_MAX, float maxHeight = FLT_MAX);
