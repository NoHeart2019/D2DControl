#pragma once


//使用CSS盒模型来实现
class CD2DBoxModel
{
public:
	CD2DBoxModel();
	virtual ~CD2DBoxModel();


	// 盒模型相关方法
	// 外边距(Margin)相关方法
	void SetMargin(float left, float top, float right, float bottom);
	void SetMarginLeft(float margin);
	void SetMarginTop(float margin);
	void SetMarginRight(float margin);
	void SetMarginBottom(float margin);
	float GetMarginLeft() const;
	float GetMarginTop() const;
	float GetMarginRight() const;
	float GetMarginBottom() const;

	// 内边距(Padding)相关方法
	void SetPadding(float left, float top, float right, float bottom);
	void SetPadding(float padding);
	void SetPadding(float x, float y);
	void SetPaddingLeft(float padding);
	void SetPaddingTop(float padding);
	void SetPaddingRight(float padding);
	void SetPaddingBottom(float padding);
	float GetPaddingLeft() const;
	float GetPaddingTop() const;
	float GetPaddingRight() const;
	float GetPaddingBottom() const;

	// 边框(Border)相关方法
	void SetBorder(float left, float top, float right, float bottom);
	void SetBorderLeft(float border);
	void SetBorderTop(float border);
	void SetBorderRight(float border);
	void SetBorderBottom(float border);
	float GetBorderLeft() const;
	float GetBorderTop() const;
	float GetBorderRight() const;
	float GetBorderBottom() const;

	// 获取不同区域矩形的方法
	D2D1_RECT_F GetContentRectangle() const;    // 获取内容区域矩形
	D2D1_RECT_F GetPaddingRectangle() const;    // 获取内边距区域矩形（包含内容区域）
	D2D1_RECT_F GetBorderRectangle() const;     // 获取边框区域矩形（包含padding区域）
	D2D1_RECT_F GetMarginRectangle() const;     // 获取外边距区域矩形（包含所有内部区域）

	virtual	D2D1_RECT_F GetRectangle() const;
	virtual D2D1_SIZE_F GetBoxModelSize() const;
	void SetBoxModel(float marginLeft, float marginTop, float marginRight, float marginBottom, 
		float borderLeft,  float borderTop,  float borderRight, float borderBottom, 
		float paddingLeft, float paddingTop, float paddingRight, float paddingBottom);


protected:
	// CSS盒模型相关成员变量
	// 外边距(Margin)
	float m_marginLeft;                 // 左边距
	float m_marginTop;                  // 上边距
	float m_marginRight;                // 右边距
	float m_marginBottom;               // 下边距

	// 内边距(Padding)
	float m_paddingLeft;                // 左内边距
	float m_paddingTop;                 // 上内边距
	float m_paddingRight;               // 右内边距
	float m_paddingBottom;              // 下内边距

	// 边框(Border)
	float m_borderLeft;                 // 左边框宽度
	float m_borderTop;                  // 上边框宽度
	float m_borderRight;                // 右边框宽度
	float m_borderBottom;               // 下边框宽度
};