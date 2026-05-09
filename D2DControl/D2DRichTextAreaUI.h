#pragma once


// 文本块样式枚举
enum TextBlockStyle
{
    StyleBody = 0,  // 正文
    StyleH1,        // 一级标题（居中）
    StyleH2,
    StyleH3,
    StyleH4,
    StyleCount      // 用于数组大小
};

// 样式与范围封装类
class CStyleRange
{
public:
    CString          m_fontName;         // 字体族名
    float            m_fontSize;         // 字号
    DWRITE_FONT_WEIGHT m_fontWeight;     // 粗细
    DWRITE_FONT_STYLE  m_fontStyle;      // 样式
    CAtlArray<DWRITE_TEXT_RANGE> m_ranges; // 该样式覆盖的文本范围

    CStyleRange()
        : m_fontSize(14.0f)
        , m_fontWeight(DWRITE_FONT_WEIGHT_NORMAL)
        , m_fontStyle(DWRITE_FONT_STYLE_NORMAL)
    {
        m_fontName = L"Microsoft YaHei";
    }

    // 便捷赋值函数
    void SetFont(LPCTSTR lpszName, float size,
                 DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_NORMAL,
                 DWRITE_FONT_STYLE style = DWRITE_FONT_STYLE_NORMAL)
    {
        m_fontName = lpszName;
        m_fontSize = size;
        m_fontWeight = weight;
        m_fontStyle = style;
    }

    void ClearRanges() { m_ranges.RemoveAll(); }
    void AddRange(const DWRITE_TEXT_RANGE& range) { m_ranges.Add(range); }
};

class CD2DRichTextAreaUI : public CD2DControlUI, public CD2DTextFormat
{
public:
    CD2DRichTextAreaUI();
    virtual ~CD2DRichTextAreaUI();

    // 添加文本块（可随时调用）
    void AddText(const CString& text, TextBlockStyle style = StyleBody, bool bNewParagraph = TRUE);

	void SetText(const CString& text);

    void Clear();   // 清空所有文本

    // 设置某个样式的字体属性
    void SetStyleFont(TextBlockStyle style, LPCTSTR lpszFontName, float fontSize,
                      DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_NORMAL,
                      DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL);

    // 获取某个样式的当前字体配置
    void GetStyleFont(TextBlockStyle style, CString& fontName, float& fontSize,
                      DWRITE_FONT_WEIGHT& fontWeight, DWRITE_FONT_STYLE& fontStyle) const;

    // 设置全局行间距倍数（默认1.2）
    void SetLineSpacing(float multiplier) { m_lineSpacingMultiplier = multiplier; }
    float GetLineSpacing() const { return m_lineSpacingMultiplier; }

    // 垂直滚动条控制
    void EnableVerticalScroll(bool enable);
    bool IsVerticalScrollEnabled() const;

    void SetScrollBarWidth(float width);
    float GetScrollBarWidth() const;
    CD2DScrollBarUI& GetVerticalScroll();

    // 滚动方法
    void ScrollToTop();
    void ScrollToBottom();
    void ScrollBy(float deltaY);
    float GetScrollPositionY() const;

	float GetTextTotalHeight() const;

    // 强制重建布局（内容或样式改变后需调用，但通常由控件内部自动处理）
    void RebuildLayout(CD2DRender* pRender);

    // 控件重载
    virtual void Render(CD2DRender* pRender);
    virtual void SetSize(float width, float height);
    virtual bool OnMouseWheel(float delta);
    virtual bool OnMouseDownL(const D2D1_POINT_2F& point);
    virtual bool OnMouseMove(const D2D1_POINT_2F& point);
    virtual bool OnMouseUpL(const D2D1_POINT_2F& point);
    virtual bool OnMouseLeave(const D2D1_POINT_2F& point);

protected:
    // 更新滚动条状态
    void UpdateScrollBars();

    // 获取文本显示区域（扣除内边距和滚动条）
    D2D1_RECT_F GetTextDisplayRect() const;

    // 垂直滚动条回调
    static void OnVerticalScrollCallback(float position, void* userData);
    void OnVerticalScroll(float position);

public:
   
    CStyleRange          m_styleRanges[StyleCount]; // 按样式分类的范围
    float				 m_lineSpacingMultiplier;   // 行间距倍数

    // 滚动条
    CD2DScrollBarUI m_verticalScrollBar;
    bool            m_enableVerticalScroll;
    float           m_scrollBarWidth;

    // 布局对象
	CComPtr<IDWriteTextFormat> m_textFormat;
    CComPtr<IDWriteTextLayout> m_pTextLayout;
    float m_textTotalHeight;   // 文本总高度
    float m_textTotalWidth;    // 文本总宽度
    float m_scrollPositionY;   // 当前滚动位置
    bool  m_isUpdatingScroll;  // 防止递归更新

    // 缓存控件大小
    float m_lastWidth;
    float m_lastHeight;
};