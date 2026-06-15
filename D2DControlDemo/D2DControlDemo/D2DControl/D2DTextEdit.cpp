// CD2DTextEdit.cpp
#include "stdafx.h"
#include "D2DTextEdit.h"



// 辅助函数：获取实际行高像素值（定义在 CD2DTextStyle.h 或此处）
extern float CalculateActualLineHeight(const CD2DTextStyle& style);

CD2DTextEdit::CD2DTextEdit()
	: CD2DControlUI()
	, m_mode(MultiLine)
	, m_wordWrap(true)
	, m_readOnly(false)
	, m_caretPos(0)
	, m_selectionStart(0)
	, m_selectionEnd(0)
	, m_mouseCaptured(false)
	, m_scrollOffsetY(0.0f)
	, m_scrollBarVisible(false)
	, m_textContentHeight(0.0f)
	, m_caretVisible(true)
	, m_lastCaretBlink(GetTickCount())
	, m_placeholderText()
	, m_maxChars(-1)  
{
	// 初始化滚动条
	m_VScroll.SetOrientation(CD2DScrollUI::Vertical);
	m_VScroll.SetVisible(false);
	
}

CD2DTextEdit::~CD2DTextEdit()
{
}

void CD2DTextEdit::SetMode(Mode mode)
{
	if (m_mode != mode)
	{
		m_mode = mode;
		if (mode == SingleLine)
		{
			// 将多行文本中的换行替换为空格或忽略？
			// 这里简单处理：移除换行符
			m_text.Replace(L"\r\n", L" ");
			m_text.Replace(L"\n", L" ");
			m_scrollBarVisible = false;
			m_scrollOffsetY = 0;
		}
		m_caretPos = 0;
		m_selectionStart = 0;
		m_selectionEnd = 0;
		InvalidateContent();
	}
}

void CD2DTextEdit::SetWordWrap(bool wrap)
{
	m_wordWrap = wrap;
	InvalidateContent();
}

void CD2DTextEdit::SetReadOnly(bool readOnly)
{
	m_readOnly = readOnly;
}

void CD2DTextEdit::SetText(const CAtlString& text)
{
	CAtlString limitedText = text;
	if (m_maxChars >= 0 && limitedText.GetLength() > m_maxChars)
		limitedText = limitedText.Left(m_maxChars);

	if (m_text != limitedText)
	{
		m_text = limitedText;
		m_caretPos = 0;
		m_selectionStart = 0;
		m_selectionEnd = 0;
		m_scrollOffsetY = 0;
		InvalidateContent();
	}
}

void CD2DTextEdit::SetPlaceholder(const CAtlString& text)
{
	m_placeholderText = text;
}

const CAtlString& CD2DTextEdit::GetPlaceholder() const
{
	return m_placeholderText;
}

void CD2DTextEdit::SetMaxChars(int maxChars)
{
	m_maxChars = maxChars;
}

int CD2DTextEdit::GetMaxChars() const
{
	return m_maxChars;
}

void CD2DTextEdit::SetSelection(int start, int end)
{
	int len = m_text.GetLength();
	m_selectionStart = max(0, min(start, len));
	m_selectionEnd = max(0, min(end, len));
	m_caretPos = m_selectionEnd;
}

void CD2DTextEdit::GetSelection(int& start, int& end) const
{
	start = m_selectionStart;
	end = m_selectionEnd;
}

void CD2DTextEdit::ReplaceSelection(const CAtlString& text)
{
	if (m_readOnly) return;

	int start = min(m_selectionStart, m_selectionEnd);
	int end = max(m_selectionStart, m_selectionEnd);

	// 计算替换后的文本长度，若超过限制则截断新文本
	int newLen = m_text.GetLength() - (end - start) + text.GetLength();
	CAtlString newText = text;
	if (m_maxChars >= 0 && newLen > m_maxChars)
	{
		int allowed = m_maxChars - (m_text.GetLength() - (end - start));
		if (allowed < 0) allowed = 0;
		newText = text.Left(allowed);   // 仅保留能容纳的部分
	}

	// 删除选中文本
	if (start < end)
		m_text.Delete(start, end - start);

	// 插入新文本
	if (!newText.IsEmpty())
		m_text.Insert(start, newText);

	// 移动光标到插入末尾
	int newPos = start + newText.GetLength();
	m_caretPos = newPos;
	m_selectionStart = newPos;
	m_selectionEnd = newPos;

	m_caretVisible = true;
	m_lastCaretBlink = GetTickCount();
	InvalidateContent();
}

void CD2DTextEdit::SetCaretPos(int pos, bool extendSelection)
{
	int len = m_text.GetLength();
	pos = max(0, min(pos, len));
	if (!extendSelection)
	{
		m_selectionStart = pos;
		m_selectionEnd = pos;
	}
	else
	{
		m_selectionEnd = pos;
	}
	m_caretPos = pos;
	m_caretVisible = true;
	m_lastCaretBlink = GetTickCount();
	EnsureCaretVisible();
}

void CD2DTextEdit::SetScrollOffsetY(float offset)
{
	float maxOffset = max(0.0f, m_textContentHeight - GetContentRectangle().bottom + GetContentRectangle().top);
	m_scrollOffsetY = max(0.0f, min(offset, maxOffset));
	if (m_VScroll.GetValue() != static_cast<int>(m_scrollOffsetY))
		m_VScroll.SetValue(static_cast<int>(m_scrollOffsetY));
}

void CD2DTextEdit::Copy()
{
	if (m_selectionStart == m_selectionEnd) return;

	int start = min(m_selectionStart, m_selectionEnd);
	int end = max(m_selectionStart, m_selectionEnd);
	CAtlString selected = m_text.Mid(start, end - start);

	if (OpenClipboard(nullptr))
	{
		EmptyClipboard();
		size_t len = (selected.GetLength() + 1) * sizeof(WCHAR);
		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
		if (hMem)
		{
			memcpy(GlobalLock(hMem), selected.GetString(), len);
			GlobalUnlock(hMem);
			SetClipboardData(CF_UNICODETEXT, hMem);
		}
		CloseClipboard();
	}
}

void CD2DTextEdit::Cut()
{
	if (m_readOnly) return;
	Copy();
	ReplaceSelection(L"");
}

void CD2DTextEdit::Paste()
{
	if (m_readOnly) return;
	if (!OpenClipboard(nullptr)) return;

	HANDLE hData = GetClipboardData(CF_UNICODETEXT);
	if (hData)
	{
		WCHAR* pText = static_cast<WCHAR*>(GlobalLock(hData));
		if (pText)
		{
			ReplaceSelection(pText);
			GlobalUnlock(hData);
		}
	}
	CloseClipboard();
}

// ==================== 绘制 ====================
void CD2DTextEdit::DrawControl(CD2DRender* pRender)
{
	if (!IsVisible() || !pRender) return;
	ID2D1RenderTarget* pRT = pRender->GetRenderTarget();
	if (!pRT) return;

	CD2DStyle style = CalculateEffectiveStyle();
	D2D1_RECT_F borderRect = GetBorderRectangle();
	D2D1_RECT_F contentRect = GetContentRectangle();

	// 绘制背景和边框
	DrawBackground(style, borderRect, pRender);
	DrawBorder(style, borderRect, pRender);

	// 更新滚动条状态（测量文本高度）
	UpdateScrollState(pRender);

	// 文本绘制区域（扣除滚动条）
	float scrollBarW = m_scrollBarVisible ? max(10.0f, m_VScroll.GetDesiredSize(pRender).width) : 0.0f;
	D2D1_RECT_F textRect = contentRect;
	textRect.right -= scrollBarW;

	// 判断是否显示占位符
	bool showPlaceholder = !m_placeholderText.IsEmpty() && !GetStatus().IsFocused() && m_text.IsEmpty();
	if (!m_text.IsEmpty() || showPlaceholder)
	{
		CD2DTextStyle textStyle = style.text;
		textStyle.wordWrap = (m_mode == MultiLine) ? m_wordWrap : false;

		CComPtr<IDWriteTextFormat> spFormat = pRender->CreateTextFormat(textStyle);
		if (spFormat)
		{
			// 根据垂直对齐决定布局高度
			float layoutHeight = (m_mode == SingleLine) 
				? (textRect.bottom - textRect.top) 
				: ((style.text.verticalAlign != DWRITE_PARAGRAPH_ALIGNMENT_NEAR) 
				? (textRect.bottom - textRect.top) 
				: FLT_MAX);
			IDWriteFactory* pDW = pRender->GetWriteFactory();
			CComPtr<IDWriteTextLayout> spLayout;
			pDW->CreateTextLayout(
				showPlaceholder ? m_placeholderText.GetString() : m_text.GetString(),
				(UINT32)(showPlaceholder ? m_placeholderText.GetLength() : m_text.GetLength()),
				spFormat,
				textRect.right - textRect.left,
				layoutHeight,
				&spLayout
				);
			if (spLayout)
			{
				 //设置行高
				float lineH = CalculateActualLineHeight(textStyle);
				// 原结构体方式已替换为三参数调用
				spLayout->SetLineSpacing(
					DWRITE_LINE_SPACING_METHOD_UNIFORM,
					lineH,                // 行高（像素）
					lineH * 0.8f          // 基线
					);

				// 裁剪区域
				pRT->PushAxisAlignedClip(textRect, D2D1_ANTIALIAS_MODE_ALIASED);

				// 应用滚动偏移
				D2D1_MATRIX_3X2_F oldTransform;
				pRT->GetTransform(&oldTransform);
				pRT->SetTransform(oldTransform*D2D1::Matrix3x2F::Translation(0.0f, -m_scrollOffsetY));

				// 绘制选择高亮
				DrawSelection(pRender, spLayout, textRect);

				// 绘制文本
				ID2D1SolidColorBrush* pBrush = nullptr;
				D2D1_COLOR_F textcolor = showPlaceholder ?  
											D2D1::ColorF(0.7f, 0.7f, 0.7f, 0.8f) : textStyle.color;   // 半透明灰色
				 
					
				pRT->CreateSolidColorBrush(textcolor, &pBrush);
				if (pBrush)
				{
					pRT->DrawTextLayout(
						D2D1::Point2F(textRect.left, contentRect.top),
						spLayout,
						pBrush,
						D2D1_DRAW_TEXT_OPTIONS_CLIP
						);
					pBrush->Release();
				}

				// 绘制光标
				if (m_caretVisible && !m_readOnly)
				{
					// 更新光标闪烁
					DWORD now = GetTickCount();
					if (now - m_lastCaretBlink >= CARET_BLINK_INTERVAL)
					{
						m_caretVisible = !m_caretVisible;
						m_lastCaretBlink = now;
					}
					DrawCaret(pRender, textRect);
				}

				pRT->SetTransform(oldTransform);
				pRT->PopAxisAlignedClip();
			}
		}
		
	}else
	{
		// 绘制光标（空文本时也会显示）
		if (!m_readOnly && GetStatus().IsFocused())
		{
			// 光标闪烁控制
			DWORD now = GetTickCount();
			if (now - m_lastCaretBlink >= CARET_BLINK_INTERVAL)
			{
				m_caretVisible = !m_caretVisible;
				m_lastCaretBlink = now;
			}
			if (m_caretVisible)
			{
				DrawCaret(pRender, textRect);
			}
		}
		else
		{
			m_caretVisible = false;   // 失去焦点或只读时隐藏光标
		}

	}
	
	// 绘制滚动条
	if (m_scrollBarVisible)
		m_VScroll.DrawControl(pRender);
}

void CD2DTextEdit::DrawSelection(CD2DRender* pRender, IDWriteTextLayout* pLayout, const D2D1_RECT_F& textRect)
{
	if (m_selectionStart == m_selectionEnd)
		return;

	int start = min(m_selectionStart, m_selectionEnd);
	int end = max(m_selectionStart, m_selectionEnd);
	if (start >= end)
		return;

	ID2D1RenderTarget* pRT = pRender->GetRenderTarget();
	if (!pRT)
		return;

	// 第一次调用获取矩形数量
	UINT32 actualHit = 0;
	pLayout->HitTestTextRange(
		start,
		static_cast<UINT32>(end - start),
		textRect.left,         // 原点 X
		textRect.top,          // 原点 Y
		nullptr,               // 不传矩形数组，仅获取数量
		0,
		&actualHit
		);

	if (actualHit == 0)
		return;

	// 用 CAtlArray 存放矩形，并第二次调用获取实际矩形
	CAtlArray<DWRITE_HIT_TEST_METRICS> metrics;
	metrics.SetCount(actualHit);
	pLayout->HitTestTextRange(
		start,
		static_cast<UINT32>(end - start),
		textRect.left,
		textRect.top,
		metrics.GetData(),
		actualHit,
		&actualHit
		);

	// 绘制选区矩形（半透明蓝色）
	CComPtr<ID2D1SolidColorBrush> spBrush;
	pRT->CreateSolidColorBrush(D2D1::ColorF(0.2f, 0.5f, 1.0f, 0.3f), &spBrush);
	if (spBrush)
	{
		for (size_t i = 0; i < metrics.GetCount(); ++i)
		{
			const auto& metric = metrics[i];
			D2D1_RECT_F rect = D2D1::RectF(metric.left ,metric.top, 
				metric.left + metric.width, metric.top + metric.height);

			pRT->FillRectangle(rect, spBrush);
		}
	}
}

void CD2DTextEdit::DrawCaret(CD2DRender* pRender, const D2D1_RECT_F& textRect)
{
	if (m_caretPos < 0 || m_caretPos > m_text.GetLength()) return;

	ID2D1RenderTarget* pRT = pRender->GetRenderTarget();
	IDWriteFactory* pDW = pRender->GetWriteFactory();
	if (!pRT || !pDW) return;

	CD2DStyle style = CalculateEffectiveStyle();
	CD2DTextStyle textStyle = style.text;
	CComPtr<IDWriteTextFormat> spFormat = pRender->CreateTextFormat(textStyle);
	if (!spFormat) return;
	float layoutHeight = (m_mode == SingleLine) 
		? (textRect.bottom - textRect.top) 
		: ((style.text.verticalAlign != DWRITE_PARAGRAPH_ALIGNMENT_NEAR) 
		? (textRect.bottom - textRect.top) 
		: FLT_MAX);
	CComPtr<IDWriteTextLayout> spLayout;
	pDW->CreateTextLayout(
		m_text.GetString(),
		(UINT32)m_text.GetLength(),
		spFormat,
		textRect.right - textRect.left,
		layoutHeight,
		&spLayout
		);
	if (!spLayout) return;

	// 设置行高（与绘制文本一致）
	float lineH = CalculateActualLineHeight(textStyle);
	// 原结构体方式已替换为三参数调用
	spLayout->SetLineSpacing(
		DWRITE_LINE_SPACING_METHOD_UNIFORM,
		lineH,                // 行高（像素）
		lineH * 0.8f          // 基线
		);

	// 获取光标位置矩形
	DWRITE_HIT_TEST_METRICS metrics;
	float caretX, caretY;
	spLayout->HitTestTextPosition(m_caretPos, FALSE, &caretX, &caretY, &metrics);

	// 绘制光标竖线
	float x = textRect.left + caretX;
	float y0 = textRect.top + caretY;
	float y1 = y0 + metrics.height;

	CComPtr<ID2D1SolidColorBrush> spBrush;
	pRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f), &spBrush);
	if (spBrush)
	{
		pRT->DrawLine(D2D1::Point2F(x, y0), D2D1::Point2F(x, y1), spBrush, 1.0f);
	}
}

// ==================== 鼠标事件 ====================
bool CD2DTextEdit::OnMouseDownL(const D2D1_POINT_2F& point)
{
	if (m_scrollBarVisible && m_VScroll.IsHitTest(point))
	{
		bool handled = m_VScroll.OnMouseDownL(point);
		m_scrollOffsetY = static_cast<float>(m_VScroll.GetValue());
		return handled;
	}

	SetFocused(true);
	m_caretVisible = true;
	D2D1_POINT_2F contentPoint = { point.x, point.y + m_scrollOffsetY };
	int pos = HitTestText(contentPoint);
	SetCaretPos(pos, false);
	m_mouseCaptured = true;
	m_ptMouseDown = point;
	return true;
}

bool CD2DTextEdit::OnMouseMove(const D2D1_POINT_2F& point)
{
	if (m_scrollBarVisible && m_VScroll.isThumbDragging())
	{
		bool handled = m_VScroll.OnMouseMove(point);
		m_scrollOffsetY = static_cast<float>(m_VScroll.GetValue());
		return handled;
	}

	if (m_mouseCaptured)
	{
		D2D1_POINT_2F contentPoint = { point.x, point.y + m_scrollOffsetY };
		int pos = HitTestText(contentPoint);
		SetCaretPos(pos, true); // 扩展选择
		return true;
	}
	return false;
}

bool CD2DTextEdit::OnMouseUpL(const D2D1_POINT_2F& point)
{
	if (m_scrollBarVisible && m_VScroll.IsPressed())
	{
		bool handled = m_VScroll.OnMouseUpL(point);
		m_scrollOffsetY = static_cast<float>(m_VScroll.GetValue());
		return handled;
	}

	m_mouseCaptured = false;
	return true;
}

bool CD2DTextEdit::OnMouseLeave(const D2D1_POINT_2F& point)
{
	if (m_scrollBarVisible)
		m_VScroll.OnMouseLeave(point);
	m_mouseCaptured = false;
	m_caretVisible = false;

	SetFocused(false);
	return CD2DControlUI::OnMouseLeave(point);
}

bool CD2DTextEdit::OnMouseWheel(float delta, const D2D1_POINT_2F& /*point*/)
{
	if (m_scrollBarVisible && m_mode == MultiLine)
	{
		float step = 20.0f;
		float newOffset = m_scrollOffsetY - (delta > 0 ? step : -step);
		SetScrollOffsetY(newOffset);
		return true;
	}
	return false;
}

// ==================== 键盘事件 ====================
bool CD2DTextEdit::OnKeyDown(DWORD keyCode)
{
	bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
	bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
	// 允许 IME 处理组合键
	if (keyCode == VK_PROCESSKEY)
		return false;
	switch (keyCode)
	{
	case VK_LEFT:
		MoveCaretBy(-1, shift);
		break;
	case VK_RIGHT:
		MoveCaretBy(1, shift);
		break;
	case VK_UP:
		// 实现垂直移动（需要通过布局计算）
		// 简单处理：暂时不变
		break;
	case VK_DOWN:
		break;
	case VK_HOME:
		SetCaretPos(0, shift);
		break;
	case VK_END:
		SetCaretPos(m_text.GetLength(), shift);
		break;
	case VK_DELETE:
		if (!m_readOnly)
		{
			if (m_selectionStart != m_selectionEnd)
				ReplaceSelection(L"");
			else if (m_caretPos < m_text.GetLength())
			{
				m_text.Delete(m_caretPos, 1);
				InvalidateContent();
			}
		}
		break;
	case VK_BACK:
		if (!m_readOnly)
		{
			if (m_selectionStart != m_selectionEnd)
				ReplaceSelection(L"");
			else if (m_caretPos > 0)
			{
				m_text.Delete(m_caretPos - 1, 1);
				m_caretPos--;
				m_selectionStart = m_caretPos;
				m_selectionEnd = m_caretPos;
				InvalidateContent();
			}
		}
		break;
	case 'A':
		if (ctrl)
		{
			SetSelection(0, m_text.GetLength());
		}
		break;
	case 'C':
		if (ctrl)
			Copy();
		break;
	case 'X':
		if (ctrl && !m_readOnly)
			Cut();
		break;
	case 'V':
		if (ctrl && !m_readOnly)
			Paste();
		break;
	case VK_RETURN:
		if (!m_readOnly)
		{
			if (m_mode == SingleLine)
			{
				// 单行模式，回车可能触发默认行为（如确认），这里不处理
			}
			else
			{
				ReplaceSelection(L"\r\n");
			}
		}
		break;
	default:
		return CD2DControlUI::OnKeyDown(keyCode);
	}
	return true;
}

bool CD2DTextEdit::OnKeyUp(DWORD keyCode)
{
	// 可在此处理按键释放逻辑（如输入法状态），当前无需额外动作
	return CD2DControlUI::OnKeyUp(keyCode);
}

bool CD2DTextEdit::OnChar(DWORD ch)
{
	if (m_readOnly || ch < 32) return false; // 控制字符忽略（除了特定处理）

	// 过滤掉不合适的字符
	WCHAR wch = static_cast<WCHAR>(ch);
	if (m_mode == SingleLine && (wch == L'\r' || wch == L'\n'))
		return false;

	ReplaceSelection(CAtlString(&wch, 1));
	return true;
}

// ==================== 测量与布局 ====================
void CD2DTextEdit::Layout()
{
	
}

D2D1_SIZE_F CD2DTextEdit::MeasureContent(CD2DRender* pRender, float maxWidth, float maxHeight) const
{
	return D2D1::SizeF(GetWidth(), GetHeight());
}

void CD2DTextEdit::UpdateScrollState(CD2DRender* pRender)
{
	if (!pRender || m_mode == SingleLine)
	{
		m_scrollBarVisible = false;
		m_scrollOffsetY = 0;
		return;
	}

	D2D1_RECT_F contentRect = GetContentRectangle();
	float viewportHeight = contentRect.bottom - contentRect.top;
	float textWidth = contentRect.right - contentRect.left;

	// 减去滚动条宽度（如果可能显示）
	float scrollBarW = m_scrollBarVisible ? max(10.0f, m_VScroll.GetDesiredSize(pRender).width) : 0.0f;
	float availWidth = textWidth - scrollBarW;
	if (availWidth < 0) availWidth = 0;

	m_textContentHeight = GetTextContentHeight(pRender, availWidth);

	bool needScroll = m_textContentHeight > viewportHeight;
	if (needScroll != m_scrollBarVisible)
	{
		m_scrollBarVisible = needScroll;
		if (!needScroll) m_scrollOffsetY = 0;
	}

	if (m_scrollBarVisible)
	{
		scrollBarW = max(10.0f, m_VScroll.GetDesiredSize(nullptr).width);
		D2D1_RECT_F scrollRect = D2D1::RectF(
			contentRect.right - scrollBarW,
			contentRect.top,
			contentRect.right,
			contentRect.bottom
			);
		m_VScroll.SetRectangle(scrollRect);
		int totalH = static_cast<int>(m_textContentHeight);
		int viewH = static_cast<int>(viewportHeight);
		m_VScroll.SetRange(0, totalH);
		m_VScroll.SetPageSize(viewH);
		m_VScroll.SetValue(static_cast<int>(m_scrollOffsetY));
		m_VScroll.SetVisible(true);

	}
	else
	{
		m_VScroll.SetVisible(false);
	}
}

float CD2DTextEdit::GetTextContentHeight(CD2DRender* pRender, float contentWidth) const
{
	if (m_text.IsEmpty() || !pRender) return 0.0f;
	IDWriteFactory* pDW = pRender->GetWriteFactory();
	if (!pDW) return 0.0f;

	CD2DStyle style = CalculateEffectiveStyle();
	CD2DTextStyle textStyle = style.text;
	textStyle.wordWrap = (m_mode == MultiLine) ? m_wordWrap : false;

	CComPtr<IDWriteTextFormat> spFormat = pRender->CreateTextFormat(textStyle);
	if (!spFormat) return 0.0f;

	CComPtr<IDWriteTextLayout> spLayout;
	pDW->CreateTextLayout(
		m_text.GetString(),
		(UINT32)m_text.GetLength(),
		spFormat,
		contentWidth,
		FLT_MAX,
		&spLayout
		);
	if (!spLayout) return 0.0f;

	// 设置行高
	float lineH = CalculateActualLineHeight(textStyle);
	// 原结构体方式已替换为三参数调用
	spLayout->SetLineSpacing(
		DWRITE_LINE_SPACING_METHOD_UNIFORM,
		lineH,                // 行高（像素）
		lineH * 0.8f          // 基线
		);

	DWRITE_TEXT_METRICS metrics;
	spLayout->GetMetrics(&metrics);
	return metrics.height;
}

int CD2DTextEdit::HitTestText(const D2D1_POINT_2F& contentPoint) 
{
	if (!m_pCachedRender || m_text.IsEmpty())
		return 0;

	IDWriteFactory* pDW = m_pCachedRender->GetWriteFactory();
	ID2D1RenderTarget* pRT = m_pCachedRender->GetRenderTarget();
	if (!pDW || !pRT) return 0;

	CD2DStyle style = CalculateEffectiveStyle();
	CD2DTextStyle textStyle = style.text;
	textStyle.wordWrap = (m_mode == MultiLine) ? m_wordWrap : false;

	CComPtr<IDWriteTextFormat> spFormat = m_pCachedRender->CreateTextFormat(textStyle);
	if (!spFormat) return 0;

	D2D1_RECT_F contentRect = GetContentRectangle();
	float scrollBarW = m_scrollBarVisible ? max(10.0f, (m_VScroll.GetDesiredSize(m_pCachedRender).width)) : 0.0f;
	float textWidth = (contentRect.right - contentRect.left) - scrollBarW;
	float textHeight = (m_mode == SingleLine) ? (contentRect.bottom - contentRect.top) : FLT_MAX;

	CComPtr<IDWriteTextLayout> spLayout;
	HRESULT hr = pDW->CreateTextLayout(
		m_text.GetString(),
		(UINT32)m_text.GetLength(),
		spFormat,
		textWidth,
		textHeight,
		&spLayout
		);
	if (FAILED(hr) || !spLayout) return 0;

	// 设置行高
	float lineH = CalculateActualLineHeight(textStyle);
	// 原结构体方式已替换为三参数调用
	spLayout->SetLineSpacing(
		DWRITE_LINE_SPACING_METHOD_UNIFORM,
		lineH,                // 行高（像素）
		lineH * 0.8f          // 基线
		);

	// 命中测试：将 contentPoint 转换为相对于文本布局原点的坐标
	float originX = contentRect.left;
	float originY = contentRect.top; // 注意绘制时使用了 contentRect.top 作为基线，文本布局的原点是 (textRect.left, contentRect.top)
	float localX = contentPoint.x - originX;
	float localY = contentPoint.y - originY;

	BOOL isTrailingHit = FALSE;
	BOOL isInside = FALSE;

	DWRITE_HIT_TEST_METRICS hitMetrics;
	hr = spLayout->HitTestPoint(localX, localY, &isTrailingHit, &isInside,&hitMetrics);
	if (SUCCEEDED(hr)) {
		int pos = (isTrailingHit == TRUE ? hitMetrics.textPosition + 1 : hitMetrics.textPosition);
		return pos;
	}
	return 0;
}
// 占位辅助
void CD2DTextEdit::EnsureCaretVisible()
{
	// TODO: 根据光标位置调整滚动偏移
}

void CD2DTextEdit::MoveCaretBy(int delta, bool extendSelection)
{
	int newPos = m_caretPos + delta;
	if (newPos < 0) newPos = 0;
	if (newPos > m_text.GetLength()) newPos = m_text.GetLength();
	SetCaretPos(newPos, extendSelection);
}

void CD2DTextEdit::DeleteSelection()
{
	ReplaceSelection(L"");
}