#pragma once

#include "D2DBoxModel.h"

class CD2DEditUI : public CD2DControlUI, public CD2DTextFormat {
public:
    CD2DEditUI();
    virtual ~CD2DEditUI();

    // 文本内容相关方法
    void SetText(LPCTSTR text);
    LPCTSTR GetText() const;
    void ClearText();

    // 文本格式相关方法 - 这些方法将使用内部的CD2DTextFormat成员变量
    void SetFontFamily(LPCTSTR fontFamily);
    void SetFontSize(float fontSize);
    void SetFontWeight(DWRITE_FONT_WEIGHT fontWeight);
   
    // 控件行为相关方法
    void SetReadOnly(bool readOnly);
    bool IsReadOnly() const;
    void SetMultiLine(bool multiLine);
    bool IsMultiLine() const;
    void SetWordWrap(bool wordWrap);
    bool IsWordWrap() const;
    void SetMaxLength(int maxLength);
    int GetMaxLength() const;

    // 显示相关方法
    void SetPlaceholderText(LPCTSTR text);
    LPCTSTR GetPlaceholderText() const;
    void SetPasswordMode(bool passwordMode, wchar_t passwordChar = L'*');
    bool IsPasswordMode() const;
    void ShowCaret(bool show);
    bool IsCaretVisible() const;

    // 滚动相关方法
    void ScrollToCaret();
    void ScrollBy(float horizontalDelta, float verticalDelta);
    float GetHorizontalScrollOffset() const;
    float GetVerticalScrollOffset() const;

    // 文本选择相关方法
    void SetSelection(int start, int end);
    void GetSelection(int& start, int& end) const;
    CString GetSelectedText() const;
    void SelectAll();
    void ClearSelection();

    // 渲染器相关方法
    void SetRender(CD2DRender* pRender);
    CD2DRender* GetRender() const;
    
     // 计算行高
    float GetLineHeight() const;
    
    
    // 计算字符位置相关方法 - 旧实现
    int PointToCharIndex(const D2D1_POINT_2F& point) const;
    D2D1_RECT_F CharIndexToRect(int charIndex) const;
    
    // 选择颜色相关方法
    void SetSelectionColor(const D2D1_COLOR_F& color);
    D2D1_COLOR_F GetSelectionColor() const;
    
    // 使用DWrite TextLayout的精确位置计算方法
    bool GetCaretPositionFromCharIndex(int charIndex, D2D1_POINT_2F& caretPosition) const;
    bool GetCharIndexFromPoint(const D2D1_POINT_2F& point, int& charIndex) const;
    
 
    // 绘制方法
    void Render(CD2DRender* pRender);

    // 重写事件处理方法
    virtual bool OnMouseDownL(const D2D1_POINT_2F& point) override;
    virtual bool OnMouseMove(const D2D1_POINT_2F& point) override;
    virtual bool OnMouseUpL(const D2D1_POINT_2F& point) override;
    virtual bool OnMouseLeave(const D2D1_POINT_2F& point) override;
    virtual bool OnMouseWheel(float delta) override;
    virtual void OnDpiChanged(float dpiScaleX, float dpiScaleY) override;

    // 键盘事件处理方法
    bool OnKeyDown(DWORD nChar);
    bool OnChar(DWORD nChar);

    // 剪贴板操作
    void Copy();
    void Cut();
    void Paste();

public:
    // 更新文本格式和布局
    void UpdateTextFormat();
    void UpdateTextLayout();
    
    // 光标和选择区域绘制
    void DrawCaret(CD2DRender* pRender);
    void DrawSelection(CD2DRender* pRender);
    
    // 更新光标闪烁状态
    void UpdateCaretBlinkState();
    
    // 确保光标可见
    void EnsureCaretVisible();

    // 文本内容
    CString m_text;                     // 当前编辑的文本内容
    CString m_placeholder;              // 占位文本（无内容时显示）
    bool m_showPlaceholder;             // 是否显示占位文本
    bool m_isReadOnly;                  // 是否只读
    bool m_isMultiLine;                 // 是否支持多行
    bool m_wordWrap;                    // 是否自动换行
    int m_maxLength;                    // 最大字符长度限制
    int m_tabWidth;                     // Tab键宽度（字符数）
    bool m_passwordMode;                // 是否为密码模式
    wchar_t m_passwordChar;             // 密码显示字符

   
    // 光标相关
    int m_caretPosition;                // 当前光标位置
    int m_selectionStart;               // 选中文本的起始位置
    int m_selectionEnd;                 // 选中文本的结束位置
    bool m_isCaretVisible;              // 光标是否可见
    bool m_isDraggingSelection;         // 是否正在拖动选择文本
    DWORD m_lastCaretBlinkTime;         // 上次光标闪烁时间
    bool m_caretBlinkState;             // 光标闪烁状态

    // 滚动相关
    float m_horizontalScrollOffset;     // 水平滚动偏移量
    float m_verticalScrollOffset;       // 垂直滚动偏移量
    bool m_needHorizontalScrollBar;     // 是否需要水平滚动条
    bool m_needVerticalScrollBar;       // 是否需要垂直滚动条
    float m_scrollBarWidth;             // 滚动条宽度

    // 渲染相关
    CD2DRender* m_pRender;              // 渲染器指针
    CComPtr<IDWriteTextLayout> m_textLayout; // 文本布局对象
	// 使用CD2DTextFormat成员变量处理文本格式相关功能
	CComPtr<IDWriteTextFormat> m_textFormat; // 文本格式对象

    D2D1_COLOR_F m_selectionColor;      // 选择区域的颜色
    
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