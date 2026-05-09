#pragma once


class CD2DStaticUI : public CD2DControlUI
{
public:
    CD2DStaticUI();
    virtual ~CD2DStaticUI();

    // 绘制方法
    virtual void Render(CD2DRender* pRender);

    // 设置文本内容
    void SetText(const CString& text);
    CString GetText() const;

    // 设置字体属性
    void SetFont(const wchar_t* fontFamily, float fontSize, bool isBold = false);
    void GetFont(CString& fontFamily, float& fontSize, bool& isBold) const;

    // 设置文本颜色
    void SetTextColor(const D2D1_COLOR_F& color);
    D2D1_COLOR_F GetTextColor() const;

    // 设置文本对齐方式
    void SetTextAlignment(DWRITE_TEXT_ALIGNMENT horizontalAlign, DWRITE_PARAGRAPH_ALIGNMENT verticalAlign);

    // 启用/禁用水平滚动条
    void EnableHorizontalScroll(bool enable);
    bool IsHorizontalScrollEnabled() const;

    // 启用/禁用垂直滚动条
    void EnableVerticalScroll(bool enable);
    bool IsVerticalScrollEnabled() const;

    // 设置滚动条宽度
    void SetScrollBarWidth(float width);
    float GetScrollBarWidth() const;

    // 设置内边距
    void SetPadding(float left, float top, float right, float bottom);
    void GetPadding(float& left, float& top, float& right, float& bottom) const;

    // 滚动控制方法
    void ScrollToTop();
    void ScrollToBottom();
    void ScrollToLeft();
    void ScrollToRight();
    void ScrollBy(float deltaX, float deltaY);

    // 获取当前滚动位置
    float GetScrollPositionX() const;
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
    virtual void SetSize(float width, float height, bool keepCenter = false);

protected:
    // 滚动回调函数
    static void OnVerticalScrollCallback(float position, void* userData);
    static void OnHorizontalScrollCallback(float position, void* userData);
    void OnVerticalScroll(float position);
    void OnHorizontalScroll(float position);

    // 计算文本边界大小
    void CalculateTextBounds(CD2DRender* pRender);

    // 更新滚动条状态
    void UpdateScrollBars();

    // 获取文本显示区域（考虑内边距和滚动条）
    D2D1_RECT_F GetTextDisplayRect() const;

protected:
    CString m_text;                  // 文本内容
    CString m_fontFamily;            // 字体名称
    float m_fontSize;                // 字体大小
    bool m_isFontBold;               // 是否粗体
    D2D1_COLOR_F m_textColor;        // 文本颜色
    DWRITE_TEXT_ALIGNMENT m_horizontalAlignment;    // 水平对齐方式
    DWRITE_PARAGRAPH_ALIGNMENT m_verticalAlignment;  // 垂直对齐方式

    CD2DScrollBarUI m_verticalScrollBar;   // 垂直滚动条
    CD2DScrollBarUI m_horizontalScrollBar; // 水平滚动条

    bool m_enableHorizontalScroll;   // 是否启用水平滚动
    bool m_enableVerticalScroll;     // 是否启用垂直滚动
    float m_scrollBarWidth;          // 滚动条宽度

    float m_paddingLeft;             // 左边距
    float m_paddingTop;              // 上边距
    float m_paddingRight;            // 右边距
    float m_paddingBottom;           // 下边距

    D2D1_SIZE_F m_textContentSize;   // 文本内容的总大小
    float m_scrollPositionX;         // 当前水平滚动位置
    float m_scrollPositionY;         // 当前垂直滚动位置

    bool m_isUpdatingScroll;         // 是否正在更新滚动位置（防止递归）
};