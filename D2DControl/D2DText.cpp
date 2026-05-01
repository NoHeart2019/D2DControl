#include "stdafx.h"
#include "D2DText.h"

/**
 * @brief CD2DTextFormat构造函数实现
 */
CD2DTextFormat::CD2DTextFormat()
    : m_text(L""),
      m_textColor(D2D1::ColorF(0.0f, 0.0f, 0.0f)), // 默认黑色文本
      m_maxWidth(FLT_MAX), // 默认较大的最大宽度
	  m_maxHeight(FLT_MAX), //默认较大的最大高度
	  m_fontName(_T("Microsoft YaHei")),   // 设置默认字体
      m_fontSize(8.0f),
	  m_lineSpacing(0.0f),
      m_horizontalAlign(DWRITE_TEXT_ALIGNMENT_LEADING),
      m_verticalAlign(DWRITE_PARAGRAPH_ALIGNMENT_NEAR),
      m_fontWeight(DWRITE_FONT_WEIGHT_NORMAL),
      m_fontStyle(DWRITE_FONT_STYLE_NORMAL),
      m_fontStretch(DWRITE_FONT_STRETCH_NORMAL)
{
   
}

/**
 * @brief CD2DTextFormat析构函数实现
 */
CD2DTextFormat::~CD2DTextFormat()
{
  
}

/**
 * @brief 设置文本内容实现
 */
void CD2DTextFormat::SetText(const wchar_t* text)
{
    if (text)
        m_text = text;
    else
        m_text.Empty();
}

/**
 * @brief 获取文本内容实现
 */
const CString& CD2DTextFormat::GetText() const
{
    return m_text;
}

/**
 * @brief 设置文本颜色实现
 */
void CD2DTextFormat::SetTextColor(const D2D1_COLOR_F& color)
{
    m_textColor = color;
}

/**
 * @brief 获取文本颜色实现
 */
D2D1_COLOR_F CD2DTextFormat::GetTextColor() const
{
    return m_textColor;
}

/**
 * @brief 设置字体实现
 */
bool CD2DTextFormat::SetTextFont(const wchar_t* fontFamilyName, float fontSize)
{
    if (!fontFamilyName || fontSize <= 0)
        return false;

    m_fontName = fontFamilyName;
    m_fontSize = fontSize;

	return TRUE;
}

bool CD2DTextFormat::SetFontSize(float fontSize)
{
	if (fontSize <= 0.0f)
		return false;

	m_fontSize = fontSize;

	return TRUE;
}


/**
 * @brief 获取字体名称实现
 */
const wchar_t* CD2DTextFormat::GetFontName() const
{
    return m_fontName.GetString();
}

/**
 * @brief 获取字体大小实现
 */
float CD2DTextFormat::GetFontSize() const
{
    return m_fontSize;
}

/**
 * @brief 设置字体粗细实现
 */
void CD2DTextFormat::SetFontWeight(DWRITE_FONT_WEIGHT weight)
{
    m_fontWeight = weight;
}

/**
 * @brief 获取字体粗细实现
 */
DWRITE_FONT_WEIGHT CD2DTextFormat::GetFontWeight() const
{
    return m_fontWeight;
}

/**
 * @brief 设置字体样式实现
 */
void CD2DTextFormat::SetFontStyle(DWRITE_FONT_STYLE style)
{
    m_fontStyle = style;
}

/**
 * @brief 获取字体样式实现
 */
DWRITE_FONT_STYLE CD2DTextFormat::GetFontStyle() const
{
    return m_fontStyle;
}

/**
 * @brief 设置字体拉伸实现
 */
void CD2DTextFormat::SetFontStretch(DWRITE_FONT_STRETCH stretch)
{
    m_fontStretch = stretch;
}

/**
 * @brief 获取字体拉伸实现
 */
DWRITE_FONT_STRETCH CD2DTextFormat::GetFontStretch() const
{
    return m_fontStretch;
}

/**
* @brief 设置和获取行高度
*/
void CD2DTextFormat::SetLineSpacing(float lineHeight)
{
	m_lineSpacing = lineHeight;
}


float CD2DTextFormat::GetLineSpacing() const
{
	return m_lineSpacing;
}

/**
 * @brief 设置文本对齐方式实现
 */
void CD2DTextFormat::SetTextAlignment(DWRITE_TEXT_ALIGNMENT horizontalAlign, DWRITE_PARAGRAPH_ALIGNMENT verticalAlign)
{
    m_horizontalAlign = horizontalAlign;
    m_verticalAlign = verticalAlign;
}

void CD2DTextFormat::SetTextAlignmentCenter()
{
	m_horizontalAlign = DWRITE_TEXT_ALIGNMENT_CENTER;
	m_verticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
}


/**
 * @brief 获取水平对齐方式实现
 */
DWRITE_TEXT_ALIGNMENT CD2DTextFormat::GetHorizontalAlignment() const
{
    return m_horizontalAlign;
}

/**
 * @brief 获取垂直对齐方式实现
 */
DWRITE_PARAGRAPH_ALIGNMENT CD2DTextFormat::GetVerticalAlignment() const
{
    return m_verticalAlign;
}

/**
 * @brief 设置文本最大宽度实现
 */
void CD2DTextFormat::SetMaxWidth(float maxWidth)
{
    m_maxWidth = maxWidth;
}

/**
 * @brief 获取文本最大宽度实现
 */
float CD2DTextFormat::GetMaxWidth() const
{
    return m_maxWidth;
}

/**
 * @brief 设置文本最大高度实现
 */
void CD2DTextFormat::SetMaxHeight(float maxHeight)
{
    m_maxHeight = maxHeight;
}

/**
 * @brief 获取文本最大高度实现
 */
float CD2DTextFormat::GetMaxHeight() const
{
    return m_maxHeight;
}


