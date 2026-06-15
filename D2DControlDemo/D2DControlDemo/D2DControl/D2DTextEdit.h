// CD2DTextEdit.h
#pragma once
#include "D2DControlUI.h"
#include "D2DScrollUI.h"

class CD2DTextEdit : public CD2DControlUI
{
public:
	enum Mode { SingleLine, MultiLine };

	CD2DTextEdit();
	virtual ~CD2DTextEdit();

	// 模式与折行
	void SetMode(Mode mode);
	Mode GetMode() const { return m_mode; }
	void SetWordWrap(bool wrap);
	bool IsWordWrap() const { return m_wordWrap; }
	void SetReadOnly(bool readOnly);
	bool IsReadOnly() const { return m_readOnly; }

	// 占位符
	void SetPlaceholder(const CAtlString& text);
	const CAtlString& GetPlaceholder() const;

	// 最大字符数
	void SetMaxChars(int maxChars);
	int GetMaxChars() const;
	// 文本操作
	void SetText(const CAtlString& text);
	const CAtlString& GetText() const { return m_text; }

	// 选择与光标
	void SetSelection(int start, int end);          // 字符索引，-1 表示文本末尾
	void GetSelection(int& start, int& end) const;
	void ReplaceSelection(const CAtlString& text);  // 用新文本替换当前选中，若无选中则在光标处插入
	int  GetCaretPos() const { return m_caretPos; }
	void SetCaretPos(int pos, bool extendSelection = false);

	// 滚动（多行时有效）
	void SetScrollOffsetY(float offset);
	float GetScrollOffsetY() const { return m_scrollOffsetY; }
	CD2DScrollUI* GetScrollBar() { return &m_VScroll; }

	// 剪贴板
	void Copy();
	void Cut();
	void Paste();


	void SetRender(CD2DRender* pRender){m_pCachedRender = pRender;};   // 设置当前渲染器，供内部测量使用

	// 重写基类
	virtual void DrawControl(CD2DRender* pRender) override;
	virtual bool OnMouseDownL(const D2D1_POINT_2F& point) override;
	virtual bool OnMouseMove(const D2D1_POINT_2F& point) override;
	virtual bool OnMouseUpL(const D2D1_POINT_2F& point) override;
	virtual bool OnMouseLeave(const D2D1_POINT_2F& point) override;
	virtual bool OnKeyDown(DWORD keyCode) override;
	virtual bool OnKeyUp(DWORD keyCode) override;
	virtual bool OnChar(DWORD ch) override;
	virtual bool OnMouseWheel(float delta, const D2D1_POINT_2F& point) override;
	virtual void Layout() ;
	virtual D2D1_SIZE_F MeasureContent(CD2DRender* pRender, float maxWidth = FLT_MAX, float maxHeight = FLT_MAX) const override;

	// 输入法编辑（暂为预留接口）
	virtual bool OnImeComposition(DWORD dwCommand, const void* pData) { return false; }

protected:
	// 内部辅助
	void UpdateScrollState(CD2DRender* pRender);
	float GetTextContentHeight(CD2DRender* pRender, float contentWidth) const;
	int  HitTestText(const D2D1_POINT_2F& contentPoint) ; // 内容坐标 → 字符索引
	void EnsureCaretVisible();
	void DeleteSelection();
	void MoveCaretBy(int delta, bool extendSelection);

	void DrawTextContent(CD2DRender* pRender, const D2D1_RECT_F& textRect);
	void DrawSelection(CD2DRender* pRender, IDWriteTextLayout* pLayout, const D2D1_RECT_F& textRect);
	void DrawCaret(CD2DRender* pRender, const D2D1_RECT_F& textRect);

private:
	// 渲染缓存（用于命中测试）
	CD2DRender* m_pCachedRender;

	// 模式与折行
	Mode m_mode;                // 编辑模式：多行或单行
	bool m_wordWrap;            // 是否自动折行（仅多行模式有效）
	bool m_readOnly;            // 是否只读

	// 光标与选择
	int m_caretPos;             // 光标位置（字符索引）
	int m_selectionStart;       // 选择起点（字符索引）
	int m_selectionEnd;         // 选择终点（字符索引）

	// 鼠标交互状态
	bool m_mouseCaptured;       // 鼠标左键是否按下（用于拖拽选择）
	D2D1_POINT_2F m_ptMouseDown;// 鼠标按下时的坐标

	// 垂直滚动
	float m_scrollOffsetY;      // 当前垂直滚动偏移（像素）
	bool m_scrollBarVisible;    // 滚动条是否可见
	CD2DScrollUI m_VScroll;     // 内置垂直滚动条
	float m_textContentHeight;  // 文本总高度（用于滚动范围计算）

	// 光标闪烁
	bool m_caretVisible;        // 光标是否可见（用于闪烁）
	DWORD m_lastCaretBlink;     // 上次闪烁切换的时间戳（毫秒）

	// 占位符
	CAtlString m_placeholderText;

	// 最大字符数限制（-1 表示无限制）
	int m_maxChars;

	static const DWORD CARET_BLINK_INTERVAL = 500; // 光标闪烁间隔（毫秒）
};