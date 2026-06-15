// StyleManager.h
#pragma once

#include <atlcoll.h>
#include <atlstr.h>
#include "D2DControl/D2D.h"
#include "D2DControl/D2DScrollUI.h"
#include "D2DControl/D2DScrollUI.h"
#include "D2DControl/D2DTextArea.h"
#include "D2DControl/D2DTextEdit.h"

class StyleManager
{
public:	
	static void Reigster();
	static BOOL bRegistered ;
public:
	struct ButtonStyleSet
	{
		CD2DStyle normal;
		CD2DStyle hover;
		CD2DStyle pressed;
		CD2DStyle disabled;
	};

	struct ListItemStyleSet
	{
		CD2DStyle normal;
		CD2DStyle hover;
		CD2DStyle checked;
		CD2DStyle dragOver;
		CD2DStyle Dragging;
	};

	struct ScrollStyleSet
	{
		CD2DScrollStyle normal;
		CD2DScrollStyle hover;
		CD2DScrollStyle pressed;
		CD2DScrollStyle disabled;
	};

	static StyleManager& Get()
	{
		static StyleManager instance;
		return instance;
	}

	
	// 注册按钮样式类
	void RegisterButtonStyle(const CAtlString& name, const ButtonStyleSet& styles)
	{
		m_buttonStyles.SetAt(name, styles);
	}

	// 应用按钮样式类
	void ApplyButtonStyle(CD2DControlUI& btn, const CAtlString& name)
	{
		ButtonStyleSet* pStyles = &m_buttonStyles.Lookup(name)->m_value;
		
		if (pStyles)
		{
			btn.SetStyle(ED2DStatus::Normal, pStyles->normal);
			btn.SetStyle(ED2DStatus::Hover,  pStyles->hover);
			btn.SetStyle(ED2DStatus::Pressed, pStyles->pressed);
			btn.SetStyle(ED2DStatus::Disabled, pStyles->disabled);
			btn.SetDefaultStyle(pStyles->normal);
		}
	}

	// 注册列表项样式类
	void RegisterListItemStyle(const CAtlString& name, const ListItemStyleSet& styles)
	{
		m_listItemStyles.SetAt(name, styles);
	}

	// 应用到列表项
	void ApplyListItemStyle(CD2DControlUI& item, const CAtlString& name)
	{
		ListItemStyleSet* pStyles = &m_listItemStyles.Lookup(name)->m_value;
		if (pStyles)
		{
			item.SetStyle(ED2DStatus::Normal,   pStyles->normal);
			item.SetStyle(ED2DStatus::Hover,    pStyles->hover);
			item.SetStyle(ED2DStatus::Checked,  pStyles->checked);
			item.SetStyle(ED2DStatus::DragOver, pStyles->dragOver);
			item.SetStyle(ED2DStatus::Dragging, pStyles->Dragging);

			item.SetDefaultStyle(pStyles->normal);
		}
	}

	// 注册滚动条样式类
	void RegisterScrollStyle(const CAtlString& name, const ScrollStyleSet& styles)
	{
		m_scrollStyles.SetAt(name, styles);
	}

	void ApplyScrollStyle(CD2DScrollUI& scroll, const CAtlString& name)
	{
		ScrollStyleSet* pStyles = &m_scrollStyles.Lookup(name)->m_value;
		if (pStyles)
		{
			scroll.SetScrollStyle(ED2DStatus::Normal,   pStyles->normal);
			scroll.SetScrollStyle(ED2DStatus::Hover,    pStyles->hover);
			scroll.SetScrollStyle(ED2DStatus::Pressed,  pStyles->pressed);
			scroll.SetScrollStyle(ED2DStatus::Disabled, pStyles->disabled);
		}
	}

public:
	CAtlMap<CAtlString, ButtonStyleSet>   m_buttonStyles;
	CAtlMap<CAtlString, ListItemStyleSet> m_listItemStyles;
	CAtlMap<CAtlString, ScrollStyleSet>   m_scrollStyles;
};

