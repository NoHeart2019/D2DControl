#include "stdafx.h"
#include "D2DControlUI.h"
#include "D2DRender.h"
#include "D2DText.h"
#include "D2DEditUI.h"
#include "D2DBoxModel.h"

CD2DEditUI::CD2DEditUI()
    : CD2DTextFormat(),
      m_text(_T("")),
      m_placeholder(_T("")),
      m_showPlaceholder(false),
      m_isReadOnly(false),
      m_isMultiLine(false),
      m_wordWrap(false),
      m_maxLength(0),
      m_tabWidth(4),
      m_passwordMode(false),
      m_passwordChar(L'*'),
      m_caretPosition(0),
      m_selectionStart(0),
      m_selectionEnd(0),
      m_isCaretVisible(false),
      m_isDraggingSelection(false),
      m_lastCaretBlinkTime(0),
      m_caretBlinkState(true),
      m_horizontalScrollOffset(0.0f),
      m_verticalScrollOffset(0.0f),
      m_needHorizontalScrollBar(false),
      m_needVerticalScrollBar(false),
      m_scrollBarWidth(16.0f),
      m_pRender(nullptr),
	  m_selectionColor(D2D1::ColorF(D2D1::ColorF::Blue, 0.3f))
{ 
    // 初始化文本格式
    UpdateTextFormat();
    // 设置默认颜色
    SetBackgroundColor(D2D1::ColorF(D2D1::ColorF::White));
    SetForegroundColor(D2D1::ColorF(D2D1::ColorF::Black));
    SetBorderColor(D2D1::ColorF(D2D1::ColorF::Gray));
 }

CD2DEditUI::~CD2DEditUI()
{
    // 释放COM对象将由CComPtr自动处理
}

// 文本内容相关方法
void CD2DEditUI::SetText(LPCTSTR text)
{
    if (m_isReadOnly)
        return;
    
    if (m_maxLength > 0 && text && _tcslen(text) > m_maxLength)
        m_text = CString(text).Left(m_maxLength);
    else
        m_text = text;
    
    m_caretPosition = m_text.GetLength();
    m_selectionStart = m_caretPosition;
    m_selectionEnd = m_caretPosition;
    
	UpdateTextFormat();
    UpdateTextLayout();
    EnsureCaretVisible();
}

LPCTSTR CD2DEditUI::GetText() const
{
    return m_text;
}

void CD2DEditUI::ClearText()
{
    if (m_isReadOnly)
        return;
    
    m_text.Empty();
    m_caretPosition = 0;
    m_selectionStart = 0;
    m_selectionEnd = 0;
    
    UpdateTextLayout();
}

// 文本格式相关方法 - 使用从CD2DTextFormat继承的方法
void CD2DEditUI::SetFontFamily(LPCTSTR fontFamily)
{
    SetTextFont(fontFamily, GetFontSize());
    UpdateTextFormat();
    UpdateTextLayout();
}

void CD2DEditUI::SetFontSize(float fontSize)
{
    CD2DTextFormat::SetTextFont(CD2DTextFormat::GetFontName(), fontSize);
    UpdateTextFormat();
    UpdateTextLayout();
}

void CD2DEditUI::SetFontWeight(DWRITE_FONT_WEIGHT fontWeight)
{
    CD2DTextFormat::SetFontWeight(fontWeight);
    UpdateTextFormat();
    UpdateTextLayout();
}

// 控件行为相关方法
void CD2DEditUI::SetReadOnly(bool readOnly)
{
    m_isReadOnly = readOnly;
}

bool CD2DEditUI::IsReadOnly() const
{
    return m_isReadOnly;
}

void CD2DEditUI::SetMultiLine(bool multiLine)
{
    m_isMultiLine = multiLine;
    UpdateTextLayout();
   
    EnsureCaretVisible();
}

bool CD2DEditUI::IsMultiLine() const
{
    return m_isMultiLine;
}

void CD2DEditUI::SetWordWrap(bool wordWrap)
{
    m_wordWrap = wordWrap;
    UpdateTextLayout();
}

bool CD2DEditUI::IsWordWrap() const
{
    return m_wordWrap;
}

void CD2DEditUI::SetMaxLength(int maxLength)
{
    m_maxLength = maxLength;
    if (m_maxLength > 0 && m_text.GetLength() > m_maxLength) {
        m_text = m_text.Left(m_maxLength);
        m_caretPosition = m_text.GetLength();
        m_selectionStart = m_caretPosition;
        m_selectionEnd = m_caretPosition;
        UpdateTextLayout();
    }
}

int CD2DEditUI::GetMaxLength() const
{
    return m_maxLength;
}

// 显示相关方法
void CD2DEditUI::SetPlaceholderText(LPCTSTR text)
{
    m_placeholder = text;
    m_showPlaceholder = m_text.IsEmpty();
}

LPCTSTR CD2DEditUI::GetPlaceholderText() const
{
    return m_placeholder;
}

void CD2DEditUI::SetPasswordMode(bool passwordMode, wchar_t passwordChar)
{
    m_passwordMode = passwordMode;
    m_passwordChar = passwordChar;
    UpdateTextLayout();
}

bool CD2DEditUI::IsPasswordMode() const
{
    return m_passwordMode;
}

void CD2DEditUI::ShowCaret(bool show)
{
    m_isCaretVisible = show;
}

bool CD2DEditUI::IsCaretVisible() const
{
    return m_isCaretVisible;
}

// 滚动相关方法
void CD2DEditUI::ScrollToCaret()
{
    EnsureCaretVisible();
}

void CD2DEditUI::ScrollBy(float horizontalDelta, float verticalDelta)
{
    m_horizontalScrollOffset += horizontalDelta;
    m_verticalScrollOffset += verticalDelta;
    
    // 确保滚动偏移量在有效范围内
    if (m_horizontalScrollOffset < 0) m_horizontalScrollOffset = 0;
    if (m_verticalScrollOffset < 0) m_verticalScrollOffset = 0;
    
    // 限制最大滚动量（需要根据文本内容计算）
    // 此处简化处理
}

float CD2DEditUI::GetHorizontalScrollOffset() const
{
    return m_horizontalScrollOffset;
}

float CD2DEditUI::GetVerticalScrollOffset() const
{
    return m_verticalScrollOffset;
}

// 文本选择相关方法
void CD2DEditUI::SetSelection(int start, int end)
{
    m_selectionStart = start;
    m_selectionEnd = end;
    
    // 确保选择范围有效
    if (m_selectionStart < 0) m_selectionStart = 0;
    if (m_selectionEnd > m_text.GetLength()) m_selectionEnd = m_text.GetLength();
    
    // 确保start <= end
    if (m_selectionStart > m_selectionEnd) {
        int temp = m_selectionStart;
        m_selectionStart = m_selectionEnd;
        m_selectionEnd = temp;
    }
    
    m_caretPosition = m_selectionEnd;
}

void CD2DEditUI::GetSelection(int& start, int& end) const
{
    start = m_selectionStart;
    end = m_selectionEnd;
}

CString CD2DEditUI::GetSelectedText() const
{
    if (m_selectionStart == m_selectionEnd)
        return _T("");
    
    return m_text.Mid(m_selectionStart, m_selectionEnd - m_selectionStart);
}

void CD2DEditUI::SelectAll()
{
    m_selectionStart = 0;
    m_selectionEnd = m_text.GetLength();
    m_caretPosition = m_selectionEnd;
}

void CD2DEditUI::ClearSelection()
{
    m_selectionStart = m_caretPosition;
    m_selectionEnd = m_caretPosition;
}

// 渲染器相关方法
void CD2DEditUI::SetRender(CD2DRender* pRender)
{
    m_pRender = pRender;
    UpdateTextFormat();
    UpdateTextLayout();
}

CD2DRender* CD2DEditUI::GetRender() const
{
    return m_pRender;
}


// 计算行高
float CD2DEditUI::GetLineHeight() const
{
    return m_pRender->MeasureText(_T("A"), m_textFormat).height;
}

// 计算字符位置相关方法
int CD2DEditUI::PointToCharIndex(const D2D1_POINT_2F& point) const
{
    // 首先尝试使用基于IDWriteTextLayout的精确计算
    int charIndex = 0;
    if (GetCharIndexFromPoint(point, charIndex)) {
        return charIndex;
    }
    
    // 如果textLayout不可用，回退到原来的实现
    if (!m_pRender || !m_textFormat)
        return 0;

	//如果点不在文字显示范围。返回最后一个字符。容错处理
    return m_text.GetLength();
}

D2D1_RECT_F CD2DEditUI::CharIndexToRect(int charIndex) const
{
    // 首先尝试使用基于IDWriteTextLayout的精确计算
    D2D1_POINT_2F caretPosition = D2D1::Point2F(0, 0);
    if (GetCaretPositionFromCharIndex(charIndex, caretPosition)) {
        // 根据获取到的光标位置创建矩形
        float lineHeight = GetLineHeight();
        float caretWidth = max(1.0f, GetFontSize() * 0.1f);  // 保持与原代码一致的最小光标宽度
        
        // 创建光标矩形（保持与原代码一致的行高比例）
        return D2D1::RectF(
            caretPosition.x,
            caretPosition.y,
            caretPosition.x + caretWidth,
            caretPosition.y + lineHeight
        );
    }
    
    // 如果textLayout不可用，回退到原来的实现
    if (!m_pRender || !m_textFormat || charIndex < 0)
        return D2D1::RectF(0, 0, 0, 0);

    return D2D1::RectF(0, 0, 0, 0);
}

// 绘制方法
void CD2DEditUI::Render(CD2DRender* pRender)
{
    if (!pRender || !IsVisible())
        return;
    
    // 保存当前渲染器引用
    if (!m_pRender)
        m_pRender = pRender;
    
    // 绘制背景（整个margin区域）
    if (IsBackgroundVisible()) {
        pRender->FillRectangle(GetMarginRectangle(), GetBackgroundColor());
    }
    
    // 绘制边框（border区域）
    if (IsBorderVisible()) {
        // 使用新定义的边框颜色和宽度
        pRender->DrawRectangle(GetBorderRectangle(), m_borderColor, GetBorderWidth());
    }
    
    // 更新光标闪烁状态
    UpdateCaretBlinkState();
    
    // 绘制文本或占位符（content区域）
    D2D1_RECT_F textRect = GetContentRectangle();
    pRender->GetRenderTarget()->PushAxisAlignedClip(textRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    if (m_showPlaceholder && m_text.IsEmpty()) {
        // 绘制占位符文本（灰色）
        D2D1_COLOR_F placeholderColor = D2D1::ColorF(D2D1::ColorF::Gray);
        pRender->DrawText(m_placeholder, textRect, placeholderColor, m_textFormat);
    } else if (!m_text.IsEmpty()) {
        // 绘制选中文本（如果有选择）
        DrawSelection(pRender);
        
        // 准备要显示的文本
        CString displayText = m_text;
        if (m_passwordMode) {
            // 替换为密码字符
            CString passwordText;
            for (int i = 0; i < m_text.GetLength(); i++) {
                passwordText += m_passwordChar;
            }
            displayText = passwordText;
        }
        
        // 绘制文本
        pRender->DrawText(displayText, textRect, GetForegroundColor(), m_textFormat);
    }
    
    // 绘制光标（如果可见且不是只读）
    if (m_isCaretVisible && m_caretBlinkState && !m_isReadOnly) {
        DrawCaret(pRender);
    }
	pRender->GetRenderTarget()->PopAxisAlignedClip();
}

// 重写事件处理方法
bool CD2DEditUI::OnMouseDownL(const D2D1_POINT_2F& point)
{
    if (!m_isEnabled || !m_isVisible)
        return false;
    
    if (IsPointInRect(point)) {
        // 获取点击位置的字符索引
        int charIndex = PointToCharIndex(point);
        
        // 设置选择范围
        m_selectionStart = charIndex;
        m_selectionEnd = charIndex;
        m_caretPosition = charIndex;
        m_isDraggingSelection = true;
        
        // 显示光标
        m_isCaretVisible = true;
        m_caretBlinkState = true;
        m_lastCaretBlinkTime = GetTickCount();
        
        return true;
    }else{
		 m_isCaretVisible = false;
		 m_caretBlinkState = false;

	}

    
    return CD2DControlUI::OnMouseDownL(point);
}

bool CD2DEditUI::OnMouseMove(const D2D1_POINT_2F& point)
{
    if (!m_isEnabled || !m_isVisible)
        return false;
    
    // 处理文本选择拖动
    if (m_isDraggingSelection && IsPointInRect(point)) {
        int charIndex = PointToCharIndex(point);
        m_selectionEnd = charIndex;
        m_caretPosition = charIndex;
        
        // 确保start <= end
        if (m_selectionStart > m_selectionEnd) {
            int temp = m_selectionStart;
            m_selectionStart = m_selectionEnd;
            m_selectionEnd = temp;
        }
        
        return true;
    }
    
    return CD2DControlUI::OnMouseMove(point);
}

bool CD2DEditUI::OnMouseUpL(const D2D1_POINT_2F& point)
{
    m_isDraggingSelection = false;
    
    return CD2DControlUI::OnMouseUpL(point);
}

bool CD2DEditUI::OnMouseLeave(const D2D1_POINT_2F& point)
{
    return CD2DControlUI::OnMouseLeave(point);
}

bool CD2DEditUI::OnMouseWheel(float delta)
{
    if (!m_isEnabled || !m_isVisible)
        return false;
    
    // 处理鼠标滚轮滚动
    float scrollAmount = 20.0f; // 每次滚动的像素数
    if (delta > 0) {
        // 向上滚动
        ScrollBy(0, -scrollAmount);
    } else {
        // 向下滚动
        ScrollBy(0, scrollAmount);
    }
    
    return true;
}

void CD2DEditUI::OnDpiChanged(float dpiScaleX, float dpiScaleY)
{
    CD2DControlUI::OnDpiChanged(dpiScaleX, dpiScaleY);
    
    // 重新计算文本格式和布局
    UpdateTextFormat();
    UpdateTextLayout();
}

// 键盘事件处理方法
bool CD2DEditUI::OnKeyDown(DWORD nChar)
{
	if (m_isReadOnly|| !m_isCaretVisible || !m_isEnabled || !m_isVisible)
		return false;
	bool isControlPressed =  (GetKeyState(VK_CONTROL) & 0x8000) != 0;
	bool isShiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

    if (m_isReadOnly)
        return false;
    
    bool handled = false;
    
    switch (nChar) {
    case VK_LEFT: // 左箭头
        if (isControlPressed) {
            // 按单词移动
            if (m_caretPosition > 0) {
                int newPosition = m_caretPosition - 1;
                while (newPosition > 0 /*&& !isspace(m_text[newPosition - 1])*/)
                    newPosition--;
                
                if (isShiftPressed) {
                    // Shift键按下时更新选择区域
                    if (m_selectionStart == m_selectionEnd) {
                        // 开始新的选择
                        m_selectionStart = m_caretPosition;
                        m_selectionEnd = newPosition;
                    } else {
                        // 扩展现有选择
                        m_selectionEnd = newPosition;
                    }
                } else {
                    // Shift键未按下时清除选择
                    m_selectionStart = newPosition;
                    m_selectionEnd = newPosition;
                }
                m_caretPosition = newPosition;
                handled = true;
            }
        } else {
            if (m_caretPosition > 0) {
                int newPosition = m_caretPosition - 1;
                
                if (isShiftPressed) {
                    // Shift键按下时更新选择区域
                    if (m_selectionStart == m_selectionEnd) {
                        // 开始新的选择
                        m_selectionStart = m_caretPosition;
                        m_selectionEnd = newPosition;
                    } else {
                        // 扩展现有选择
                        m_selectionEnd = newPosition;
                    }
                } else {
                    // Shift键未按下时清除选择
                    m_selectionStart = newPosition;
                    m_selectionEnd = newPosition;
                }
                m_caretPosition = newPosition;
                handled = true;
            }
        }
        break;
    
    case VK_RIGHT: // 右箭头
        if (isControlPressed) {
            // 按单词移动
            if (m_caretPosition < m_text.GetLength()) {
                int newPosition = m_caretPosition;
                while (newPosition < m_text.GetLength() /*&& !isspace(m_text[newPosition])*/)
                    newPosition++;
                while (newPosition < m_text.GetLength() /*&& isspace(m_text[newPosition])*/)
                    newPosition++;
                
                if (isShiftPressed) {
                    // Shift键按下时更新选择区域
                    if (m_selectionStart == m_selectionEnd) {
                        // 开始新的选择
                        m_selectionStart = m_caretPosition;
                        m_selectionEnd = newPosition;
                    } else {
                        // 扩展现有选择
                        m_selectionEnd = newPosition;
                    }
                } else {
                    // Shift键未按下时清除选择
                    m_selectionStart = newPosition;
                    m_selectionEnd = newPosition;
                }
                m_caretPosition = newPosition;
                handled = true;
            }
        } else {
            if (m_caretPosition < m_text.GetLength()) {
                int newPosition = m_caretPosition + 1;
                
                if (isShiftPressed) {
                    // Shift键按下时更新选择区域
                    if (m_selectionStart == m_selectionEnd) {
                        // 开始新的选择
                        m_selectionStart = m_caretPosition;
                        m_selectionEnd = newPosition;
                    } else {
                        // 扩展现有选择
                        m_selectionEnd = newPosition;
                    }
                } else {
                    // Shift键未按下时清除选择
                    m_selectionStart = newPosition;
                    m_selectionEnd = newPosition;
                }
                m_caretPosition = newPosition;
                handled = true;
            }
        }
        break;
    
    case VK_UP: // 上箭头
        if (m_isMultiLine) {
            int newPosition = m_caretPosition;
            
            // 优先使用m_textLayout进行精确计算
            if (m_textLayout && !m_text.IsEmpty()) {
                // 1. 获取当前光标位置的坐标
                D2D1_POINT_2F currentPos = D2D1::Point2F(0, 0);
                if (GetCaretPositionFromCharIndex(m_caretPosition, currentPos)) {
                    // 2. 创建向上移动一行的新坐标（向上移动一行的高度）
                    D2D1_POINT_2F newPos = currentPos;
                    float lineHeight = GetLineHeight();
                    newPos.y -= lineHeight;
                    
                    // 3. 使用HitTestPoint获取新坐标对应的字符索引
                    D2D1_POINT_2F textLayoutPos = newPos;
                    D2D1_RECT_F rect = GetRectangle();
                    
                    // 调整坐标为文本布局坐标系
                    textLayoutPos.x -= (rect.left + 4.0f - m_horizontalScrollOffset);
                    textLayoutPos.y -= (rect.top + 4.0f - m_verticalScrollOffset);
                    
                    BOOL isTrailingHit = FALSE;
                    BOOL isInside = FALSE;
                    DWRITE_HIT_TEST_METRICS hitTestMetrics = {};
                    
                    HRESULT hr = m_textLayout->HitTestPoint(
                        textLayoutPos.x,
                        textLayoutPos.y,
                        &isTrailingHit,
                        &isInside,
                        &hitTestMetrics
                    );
                    
                    // 从HitTestMetrics获取字符索引
                    newPosition = hitTestMetrics.textPosition;
                    
                    // 确保新位置有效
                    if (SUCCEEDED(hr) && newPosition >= 0 && newPosition <= m_text.GetLength()) {
                        // 检查是否确实移动到了上一行
                        DWRITE_TEXT_METRICS textMetrics = {0};
                        if (SUCCEEDED(m_textLayout->GetMetrics(&textMetrics))) {
                            // 4. 更新选择区域
                            if (isShiftPressed) {
                                // Shift键按下时更新选择区域
                                if (m_selectionStart == m_selectionEnd) {
                                    // 开始新的选择
                                    m_selectionStart = m_caretPosition;
                                    m_selectionEnd = newPosition;
                                } else {
                                    // 扩展现有选择
                                    m_selectionEnd = newPosition;
                                }
                            } else {
                                // Shift键未按下时清除选择
                                m_selectionStart = newPosition;
                                m_selectionEnd = newPosition;
                            }
                            
                            m_caretPosition = newPosition;
                            handled = true;
                        }
                    }
                }
            }
        }
        break;
    
    case VK_DOWN: // 下箭头
        if (m_isMultiLine) {
            int newPosition = m_caretPosition;
            
            // 优先使用m_textLayout进行精确计算
            if (m_textLayout && !m_text.IsEmpty()) {
                // 1. 获取当前光标位置的坐标
                D2D1_POINT_2F currentPos = D2D1::Point2F(0, 0);
                if (GetCaretPositionFromCharIndex(m_caretPosition, currentPos)) {
                    // 2. 创建向下移动一行的新坐标（向下移动一行的高度）
                    D2D1_POINT_2F newPos = currentPos;
                    float lineHeight = GetLineHeight();
                    newPos.y += lineHeight;
                    
                    // 3. 使用HitTestPoint获取新坐标对应的字符索引
                    D2D1_POINT_2F textLayoutPos = newPos;
                    D2D1_RECT_F rect = GetRectangle();
                    
                    // 调整坐标为文本布局坐标系
                    textLayoutPos.x -= (rect.left + 4.0f - m_horizontalScrollOffset);
                    textLayoutPos.y -= (rect.top + 4.0f - m_verticalScrollOffset);
                    
                    BOOL isTrailingHit = FALSE;
                    BOOL isInside = FALSE;
                    DWRITE_HIT_TEST_METRICS hitTestMetrics = {};
                    
                    HRESULT hr = m_textLayout->HitTestPoint(
                        textLayoutPos.x,
                        textLayoutPos.y,
                        &isTrailingHit,
                        &isInside,
                        &hitTestMetrics
                    );
                    
                    // 从HitTestMetrics获取字符索引
                    newPosition = hitTestMetrics.textPosition;
                    
                    // 确保新位置有效
                    if (SUCCEEDED(hr) && newPosition >= 0 && newPosition <= m_text.GetLength()) {
                        // 4. 更新选择区域
                        if (isShiftPressed) {
                            // Shift键按下时更新选择区域
                            if (m_selectionStart == m_selectionEnd) {
                                // 开始新的选择
                                m_selectionStart = m_caretPosition;
                                m_selectionEnd = newPosition;
                            } else {
                                // 扩展现有选择
                                m_selectionEnd = newPosition;
                            }
                        } else {
                            // Shift键未按下时清除选择
                            m_selectionStart = newPosition;
                            m_selectionEnd = newPosition;
                        }
                        
                        m_caretPosition = newPosition;
                        handled = true;
                    }
                }
            }
        }
        break;
    
    case VK_HOME: // Home
		{
        int newPosition = m_caretPosition;
        D2D1_RECT_F rect = GetRectangle();
        if (isControlPressed) {
            // 移动到文本开头
            newPosition = 0;
        } else {
            // 优先使用m_textLayout获取行首位置
            if (m_textLayout && !m_text.IsEmpty()) {
                // 使用HitTestTextPosition获取当前光标位置的行信息
                BOOL isTrailingHit = FALSE;
                BOOL isInside = FALSE;
                DWRITE_HIT_TEST_METRICS hitTestMetrics = {};
                D2D1_POINT_2F caretPoint;
                
                // 首先通过光标位置获取行信息
                HRESULT hr = m_textLayout->HitTestTextPosition(
                    m_caretPosition,
                    isTrailingHit,
                    &caretPoint.x,
                    &caretPoint.y,
                    &hitTestMetrics
                );
                
                if (SUCCEEDED(hr)) {
                    // 获取当前行的起始位置
					hr = m_textLayout->HitTestPoint(rect.left, caretPoint.y, &isTrailingHit, &isInside, &hitTestMetrics);
					if (SUCCEEDED(hr))
					{
						newPosition = hitTestMetrics.textPosition;
					}
                } 
            }
        }
        
        // 更新选择区域
        if (isShiftPressed) {
            // Shift键按下时更新选择区域
            if (m_selectionStart == m_selectionEnd) {
                // 开始新的选择
                m_selectionStart = m_caretPosition;
                m_selectionEnd = newPosition;
            } else {
                // 扩展现有选择
                m_selectionEnd = newPosition;
            }
        } else {
            // Shift键未按下时清除选择
            m_selectionStart = newPosition;
            m_selectionEnd = newPosition;
        }
        m_caretPosition = newPosition;
        handled = true;
	}
        break;
    
    case VK_END: // End
		{
        int newPosition = m_caretPosition;
        
        if (isControlPressed) {
            // 移动到文本结尾
            newPosition = m_text.GetLength();
        } else {
            // 优先使用m_textLayout获取行尾位置
            if (m_textLayout && !m_text.IsEmpty()) {
                // 使用HitTestTextPosition获取当前光标位置的行信息
                BOOL isTrailingHit = FALSE;
                BOOL isInside = FALSE;
                DWRITE_HIT_TEST_METRICS hitTestMetrics = {};
                D2D1_POINT_2F caretPoint;
                
                // 首先通过光标位置获取行信息
                HRESULT hr = m_textLayout->HitTestTextPosition(
                    m_caretPosition,
                    isTrailingHit,
                    &caretPoint.x,
                    &caretPoint.y,
                    &hitTestMetrics
                );
                
                if (SUCCEEDED(hr)) {
                    // 获取当前行的结束位置
					hr = m_textLayout->HitTestPoint(GetRectangle().right, caretPoint.y, &isTrailingHit, &isInside, &hitTestMetrics);
					if (SUCCEEDED(hr))
					{
						newPosition = hitTestMetrics.textPosition + hitTestMetrics.length;
					}
                    
                } 
            } 
        }
        
        // 更新选择区域
        if (isShiftPressed) {
            // Shift键按下时更新选择区域
            if (m_selectionStart == m_selectionEnd) {
                // 开始新的选择
                m_selectionStart = m_caretPosition;
                m_selectionEnd = newPosition;
            } else {
                // 扩展现有选择
                m_selectionEnd = newPosition;
            }
        } else {
            // Shift键未按下时清除选择
            m_selectionStart = newPosition;
            m_selectionEnd = newPosition;
        }
        m_caretPosition = newPosition;
        handled = true;
	}
        break;
    
    case VK_BACK: // 退格键
        if (m_text.GetLength() > 0) {
            if (m_selectionStart != m_selectionEnd) {
                // 删除选中的文本
                m_text.Delete(m_selectionStart, m_selectionEnd - m_selectionStart);
                m_caretPosition = m_selectionStart;
                m_selectionEnd = m_selectionStart;
            } else if (m_caretPosition > 0) {
                // 删除光标前的字符
                m_text.Delete(m_caretPosition - 1, 1);
                m_caretPosition--;
            }
            UpdateTextLayout();
            handled = true;
        }
        break;
    
    case VK_DELETE: // Delete键
        if (m_text.GetLength() > 0) {
            if (m_selectionStart != m_selectionEnd) {
                // 删除选中的文本
                m_text.Delete(m_selectionStart, m_selectionEnd - m_selectionStart);
                m_caretPosition = m_selectionStart;
                m_selectionEnd = m_selectionStart;
            } else if (m_caretPosition < m_text.GetLength()) {
                // 删除光标后的字符
                m_text.Delete(m_caretPosition, 1);
            }
            UpdateTextLayout();
            handled = true;
        }
        break;
    
    case VK_RETURN: // 回车键
        if (m_isMultiLine) {
            // 在光标位置插入换行符
            if (m_selectionStart != m_selectionEnd) {
                // 先删除选中的文本
                m_text.Delete(m_selectionStart, m_selectionEnd - m_selectionStart);
                m_caretPosition = m_selectionStart;
                m_selectionEnd = m_selectionStart;
            }
            
            m_text.Insert(m_caretPosition, _T("\n"));
            m_caretPosition++;
            
            UpdateTextLayout();
            handled = true;
        }
        break;
    
    case VK_TAB: // Tab键
        if (m_selectionStart != m_selectionEnd) {
            // 先删除选中的文本
            m_text.Delete(m_selectionStart, m_selectionEnd - m_selectionStart);
            m_caretPosition = m_selectionStart;
            m_selectionEnd = m_selectionStart;
        }
        
        // 插入Tab字符
       { CString tabStr;
        for (int i = 0; i < m_tabWidth; i++) {
            tabStr += _T(" ");
        }
        
        m_text.Insert(m_caretPosition, tabStr);
        m_caretPosition += m_tabWidth;
        }
        UpdateTextLayout();
        handled = true;
        break;
    
    case 'A': // Ctrl+A 选择全部
        if (isControlPressed) {
            SelectAll();
            handled = true;
        }
        break;
    
    case 'C': // Ctrl+C 复制
        if (isControlPressed) {
            Copy();
            handled = true;
        }
        break;
    
    case 'X': // Ctrl+X 剪切
        if (isControlPressed) {
            Cut();
            handled = true;
        }
        break;
    
    case 'V': // Ctrl+V 粘贴
        if (isControlPressed) {
            Paste();
            handled = true;
        }
        break;
    }
    
    // 确保光标可见
    if (handled) {
        EnsureCaretVisible();
        
        // 重置光标闪烁状态
        m_caretBlinkState = true;
        m_lastCaretBlinkTime = GetTickCount();
        
        // 更新占位符显示状态
        m_showPlaceholder = m_text.IsEmpty();
    }
    
    return handled;
}

bool CD2DEditUI::OnChar(DWORD nChar)
{
    if (m_isReadOnly|| !m_isCaretVisible || !m_isEnabled || !m_isVisible)
        return false;
	
    // 过滤控制字符
    if (nChar < 32) // 控制字符
        return false;
    
    // 检查最大长度限制
    if (m_maxLength > 0 && m_text.GetLength() >= m_maxLength && m_selectionStart == m_selectionEnd)
        return false;
    
    // 如果有选中的文本，先删除
    if (m_selectionStart != m_selectionEnd) {
        m_text.Delete(m_selectionStart, m_selectionEnd - m_selectionStart);
        m_caretPosition = m_selectionStart;
        m_selectionEnd = m_selectionStart;
    }
    
    // 在光标位置插入字符
    m_text.Insert(m_caretPosition, (WCHAR)nChar);
    m_caretPosition++;
    
    // 更新文本布局
    UpdateTextLayout();
    
    // 确保光标可见
    EnsureCaretVisible();
    
    // 重置光标闪烁状态
    m_caretBlinkState = true;
    m_lastCaretBlinkTime = GetTickCount();
    
    // 更新占位符显示状态
    m_showPlaceholder = m_text.IsEmpty();
    
    return true;
}

// 剪贴板操作
void CD2DEditUI::Copy()
{
    if (m_selectionStart == m_selectionEnd)
        return;
    
    // 获取选中的文本
    CString selectedText = GetSelectedText();
    
    // 复制到剪贴板
    if (OpenClipboard(nullptr)) {
        EmptyClipboard();
        
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (selectedText.GetLength() + 1) * sizeof(wchar_t));
        if (hMem) {
            wchar_t* pText = static_cast<wchar_t*>(GlobalLock(hMem));
            if (pText) {
                wcscpy_s(pText, selectedText.GetLength() + 1, static_cast<LPCWSTR>(selectedText));
                GlobalUnlock(hMem);
                SetClipboardData(CF_UNICODETEXT, hMem);
            }
        }
        
        CloseClipboard();
    }
}

void CD2DEditUI::Cut()
{
    if (m_isReadOnly)
        return;
    
    // 先复制
    Copy();
    
    // 再删除
    if (m_selectionStart != m_selectionEnd) {
        m_text.Delete(m_selectionStart, m_selectionEnd - m_selectionStart);
        m_caretPosition = m_selectionStart;
        m_selectionEnd = m_selectionStart;
        
        UpdateTextLayout();
        EnsureCaretVisible();
    }
}

void CD2DEditUI::Paste()
{
    if (m_isReadOnly)
        return;
    
    if (OpenClipboard(nullptr)) {
        HANDLE hData = GetClipboardData(CF_UNICODETEXT);
        if (hData) {
            wchar_t* pText = static_cast<wchar_t*>(GlobalLock(hData));
            if (pText) {
                CString clipboardText = pText;
                GlobalUnlock(hData);
                
                // 检查最大长度限制
                int maxCharsToAdd = m_maxLength > 0 ? m_maxLength - m_text.GetLength() : clipboardText.GetLength();
                if (maxCharsToAdd <= 0 && m_selectionStart == m_selectionEnd)
                    return;
                
                // 如果有选中的文本，先删除
                if (m_selectionStart != m_selectionEnd) {
                    m_text.Delete(m_selectionStart, m_selectionEnd - m_selectionStart);
                    m_caretPosition = m_selectionStart;
                    m_selectionEnd = m_selectionStart;
                    
                    // 重新计算可以添加的字符数
                    maxCharsToAdd = m_maxLength > 0 ? m_maxLength - m_text.GetLength() : clipboardText.GetLength();
                    if (maxCharsToAdd <= 0)
                        return;
                }
                
                // 限制粘贴的文本长度
                if (maxCharsToAdd < clipboardText.GetLength())
                    clipboardText = clipboardText.Left(maxCharsToAdd);
                
                // 插入文本
                m_text.Insert(m_caretPosition, clipboardText);
                m_caretPosition += clipboardText.GetLength();
                
                UpdateTextLayout();
                EnsureCaretVisible();
            }
        }
        
        CloseClipboard();
    }
}

// 内部辅助方法
void CD2DEditUI::UpdateTextFormat()
{
    if (!m_pRender)
        return;
    m_textFormat.Release();
    // 创建新的文本格式，直接使用从CD2DTextFormat继承的方法
    m_textFormat = m_pRender->CreateTextFormat(
        GetFontName(), 
        GetFontSize(), 
        GetFontWeight(), 
        GetHorizontalAlignment(), 
        GetVerticalAlignment());
}

void CD2DEditUI::UpdateTextLayout()
{
    if (!m_pRender || !m_textFormat || !m_isVisible)
        return;
    
    // 获取内容矩形区域（考虑padding，不考虑margin和border）
    D2D1_RECT_F contentRect = GetContentRectangle();
    float maxWidth = contentRect.right - contentRect.left; // 内容区域宽度
    
    // 创建文本布局
    CString displayText = m_text;
    if (m_passwordMode) {
        // 替换为密码字符
        CString passwordText;
        for (int i = 0; i < m_text.GetLength(); i++) {
            passwordText += m_passwordChar;
        }
        displayText = passwordText;
    }
    
    if (maxWidth > 0 /*&& !displayText.IsEmpty()*/) {
        // 释放旧的布局对象
        m_textLayout.Release();
        // 创建新的文本布局对象
        m_textLayout = m_pRender->CreateTextLayout(
        displayText.GetString(),
        displayText.GetLength(),
        m_textFormat,
        maxWidth,
        FLT_MAX);// 垂直方向不限制高度
        
        // 设置文本布局的换行模式
        if (m_textLayout) {
            DWRITE_WORD_WRAPPING wordWrapping = m_wordWrap ? DWRITE_WORD_WRAPPING_WRAP : DWRITE_WORD_WRAPPING_NO_WRAP;
            m_textLayout->SetWordWrapping(wordWrapping);
            
            // 设置段落对齐方式（左对齐）
            m_textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            m_textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        }
        
    }
}

void CD2DEditUI::DrawCaret(CD2DRender* pRender)
{
    if (!pRender || m_caretPosition < 0 || m_caretPosition > m_text.GetLength())
        return;
    int countt = m_text.GetLength();
    // 获取光标位置的矩形
    D2D1_RECT_F caretRect = CharIndexToRect(m_caretPosition);
    
    // 获取字体大小，确保光标大小与字体大小成比例
    float fontSize = GetFontSize();
    
    // 根据字体大小计算光标宽度（确保最小宽度为1.0f）
    float caretWidth = max(1.0f, fontSize * 0.08f);
   
    
    // 计算光标高度，比字体高度稍大一些（增加10%）
    float fontHeight = caretRect.bottom - caretRect.top;
    float caretHeight = fontHeight * 1.1f;
    
    // 计算光标位置，使其垂直居中
    float caretTop = caretRect.top - (caretHeight - fontHeight) * 0.5f;
    
    // 绘制光标（一个竖线）
    D2D1_RECT_F rect = D2D1::RectF(
        caretRect.left, 
        caretTop, 
        caretRect.left + caretWidth, 
        caretTop + caretHeight
    );
    
    pRender->FillRectangle(rect, GetForegroundColor());
}
 
void CD2DEditUI::DrawSelection(CD2DRender* pRender)
{
    if (!pRender || !m_textLayout || m_selectionStart == m_selectionEnd)
        return;
    
	HRESULT hr;
    // 确保start和end顺序正确
    int start = min(m_selectionStart, m_selectionEnd);
    int end = max(m_selectionStart, m_selectionEnd);
	// 获取内容区域矩形
	D2D1_RECT_F contentRect = GetContentRectangle();
	float contentLeft = contentRect.left  - m_horizontalScrollOffset;
	float contentTop = contentRect.top  - m_verticalScrollOffset;

	UINT32                  maxHitTestMetricsCount = end - start;
	UINT					actualHitTestMetricsCount = 0;

	CAtlArray<DWRITE_HIT_TEST_METRICS> hitTestMetrics;
	hitTestMetrics.SetCount(maxHitTestMetricsCount);
	hr = m_textLayout->HitTestTextRange(
		start,
		end - start,
		contentLeft,
		contentTop,
		hitTestMetrics.GetData(),
		maxHitTestMetricsCount,
		&actualHitTestMetricsCount
		);

	if (SUCCEEDED(hr)) {
		for (size_t i = 0; i < actualHitTestMetricsCount; i++)
		{
			const auto& metrics = hitTestMetrics[i];
			D2D1_RECT_F rect = D2D1::RectF(metrics.left ,metrics.top, 
										  metrics.left + metrics.width, metrics.top + metrics.height);
			CString Dbg;
			Dbg.Format(L"Select{%f, %f, %f, %f}\n", rect.left, rect.top, rect.right, rect.bottom);
			OutputDebugString(Dbg);
			pRender->FillRectangle(rect, m_selectionColor);
		}
	}
}

void CD2DEditUI::UpdateCaretBlinkState()
{
    // 光标闪烁时间（毫秒）
    const DWORD BLINK_INTERVAL = 530;
    
    DWORD currentTime = GetTickCount();
    if (currentTime - m_lastCaretBlinkTime > BLINK_INTERVAL) {
        // 切换光标闪烁状态
        m_caretBlinkState = !m_caretBlinkState;
        m_lastCaretBlinkTime = currentTime;
    }
}

void CD2DEditUI::EnsureCaretVisible()
{
    // 简化实现：确保光标在可见区域内
    // 实际应用中应使用更精确的计算
    
    if (!m_pRender || !m_textFormat)
        return;
    return;
    // 获取光标位置的矩形
    D2D1_RECT_F caretRect = CharIndexToRect(m_caretPosition);
    
    // 获取控件可见区域
    D2D1_RECT_F controlRect = GetRectangle();
    
    // 检查是否需要水平滚动
    float borderPadding = 4.0f;
    if (caretRect.right > controlRect.right - borderPadding) {
        // 向右滚动
        m_horizontalScrollOffset = caretRect.right - (controlRect.right - borderPadding);
    } else if (caretRect.left < controlRect.left + borderPadding) {
        // 向左滚动
        m_horizontalScrollOffset = caretRect.left - (controlRect.left + borderPadding);
        if (m_horizontalScrollOffset < 0)
            m_horizontalScrollOffset = 0;
    }
    
    // 检查是否需要垂直滚动
    if (caretRect.bottom > controlRect.bottom - borderPadding) {
        // 向下滚动
        m_verticalScrollOffset = caretRect.bottom - (controlRect.bottom - borderPadding);
    } else if (caretRect.top < controlRect.top + borderPadding) {
        // 向上滚动
        m_verticalScrollOffset = caretRect.top - (controlRect.top + borderPadding);
        if (m_verticalScrollOffset < 0)
            m_verticalScrollOffset = 0;
    }
}

// 使用HitTestTextPosition从字符索引获取光标坐标
bool CD2DEditUI::GetCaretPositionFromCharIndex(int charIndex, D2D1_POINT_2F& caretPosition) const
{
    if (!m_textLayout || charIndex < 0 || charIndex > m_text.GetLength())
        return false;
    
    // 调用HitTestTextPosition获取光标位置
    BOOL isTrailingHit = FALSE;
    BOOL isInside = FALSE;
    DWRITE_HIT_TEST_METRICS hitTestMetrics = {0};
    
    isTrailingHit = FALSE;  // 对于插入点，使用false表示前导边缘
    HRESULT hr = m_textLayout->HitTestTextPosition(
        charIndex,
        isTrailingHit,  // 传递布尔值，不是引用
        &caretPosition.x,
        &caretPosition.y,
        &hitTestMetrics
    );
	DWRITE_TEXT_METRICS metrics;
	m_textLayout->GetMetrics(&metrics);
    if (SUCCEEDED(hr)) {
        // 调整坐标，考虑滚动偏移和内容区域位置
        D2D1_RECT_F contentRect = GetContentRectangle();
        caretPosition.x += contentRect.left;  // 使用内容区域的左边界
        caretPosition.y += contentRect.top;   // 使用内容区域的上边界
        
        // 应用滚动偏移
        caretPosition.x -= m_horizontalScrollOffset;
        caretPosition.y -= m_verticalScrollOffset;
        
        return true;
    }
    
    return false;
}


// 使用HitTestPoint从屏幕坐标获取字符索引
bool CD2DEditUI::GetCharIndexFromPoint(const D2D1_POINT_2F& point, int& charIndex) const
{
    if (!m_textLayout)
        return false;
    
    // 调整坐标，转换为文本布局坐标系
    D2D1_RECT_F contentRect = GetContentRectangle();
    D2D1_POINT_2F adjustedPoint;
    adjustedPoint.x = point.x - contentRect.left + m_horizontalScrollOffset;
    adjustedPoint.y = point.y - contentRect.top + m_verticalScrollOffset;
    
    // 调用HitTestPoint获取字符索引
    BOOL isTrailingHit = FALSE;
    BOOL isInside = FALSE;
    DWRITE_HIT_TEST_METRICS hitTestMetrics = {0};
    
    HRESULT hr = m_textLayout->HitTestPoint(
        adjustedPoint.x,
        adjustedPoint.y,
        &isTrailingHit,
        &isInside,
        &hitTestMetrics
    );
    
    // 从hitTestMetrics中提取字符索引
    if (SUCCEEDED(hr)) {
        charIndex = hitTestMetrics.textPosition;
    }
    
    return SUCCEEDED(hr);
}

// 使用HitTestTextRange获取选择区域的矩形
// 设置选择颜色
void CD2DEditUI::SetSelectionColor(const D2D1_COLOR_F& color)
{
    m_selectionColor = color;
}

// 获取选择颜色
D2D1_COLOR_F CD2DEditUI::GetSelectionColor() const
{
    return m_selectionColor;
}

