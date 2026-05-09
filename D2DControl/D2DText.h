#pragma once
/**
 * @brief CD2DTextFormat类 - 专门封装Direct2D文本绘制相关功能的UI组件
 * 
 */
class CD2DTextFormat {
public:
    /**
     * @brief 构造函数
     * 初始化文本相关属性为默认值
     */
    CD2DTextFormat();

    /**
     * @brief 析构函数
     * 释放创建的DirectWrite资源
     */
    ~CD2DTextFormat();
    /**
     * @brief 设置和获取文本内容
     */
    void SetText(const wchar_t* text);
    const CString& GetText() const;

    /**
     * @brief 设置和获取文本颜色
     */
    void SetTextColor(const D2D1_COLOR_F& color);
    D2D1_COLOR_F GetTextColor() const;

    /**
     * @brief 设置和获取文本字体
     */
    bool SetTextFont(const wchar_t* fontFamilyName, float fontSize);
    const wchar_t* GetFontName() const;
    float GetFontSize() const;
	bool SetFontSize(float fontSize);


    /**
     * @brief 设置和获取字体样式属性
     */
    void SetFontWeight(DWRITE_FONT_WEIGHT weight);
    DWRITE_FONT_WEIGHT GetFontWeight() const;

    void SetFontStyle(DWRITE_FONT_STYLE style);
    DWRITE_FONT_STYLE GetFontStyle() const;

    void SetFontStretch(DWRITE_FONT_STRETCH stretch);
    DWRITE_FONT_STRETCH GetFontStretch() const;

	 /**
     * @brief 设置和获取行高度。只保存但是在TextLayout中设置
     */
	void SetLineSpacing(float lineHeight);
	float GetLineSpacing() const;
	

    /**
     * @brief 设置和获取文本对齐方式
     */
    void SetTextAlignment(DWRITE_TEXT_ALIGNMENT horizontalAlign, DWRITE_PARAGRAPH_ALIGNMENT verticalAlign);
    DWRITE_TEXT_ALIGNMENT GetHorizontalAlignment() const;
    DWRITE_PARAGRAPH_ALIGNMENT GetVerticalAlignment() const;
	void SetTextAlignmentCenter();


    /**
     * @brief 设置文本最大宽度（用于自动换行）
     */
    void SetMaxWidth(float maxWidth);
    float GetMaxWidth() const;

	   /**
     * @brief 设置文本最大高度（用于自动换行）
     */
    void SetMaxHeight(float maxHeight);
    float GetMaxHeight() const;

protected:
    // 文本内容相关
    CString m_text;                          // 文本内容
    D2D1_COLOR_F m_textColor;                // 文本颜色
   
    // 字体相关属性
    CString m_fontName;                // 字体名称
    float m_fontSize;                        // 字体大小
    DWRITE_TEXT_ALIGNMENT m_horizontalAlign; // 水平对齐方式
    DWRITE_PARAGRAPH_ALIGNMENT m_verticalAlign; // 垂直对齐方式
    DWRITE_FONT_WEIGHT m_fontWeight;         // 字体粗细
    DWRITE_FONT_STYLE m_fontStyle;           // 字体样式（正常、斜体等）
    DWRITE_FONT_STRETCH m_fontStretch;       // 字体拉伸程度

	float m_maxWidth;                        // 文本最大宽度（用于自动换行）
	float m_maxHeight;                       // 文本最大高度（用于自动换行）

	float m_lineSpacing;					 // 文本行高
};
