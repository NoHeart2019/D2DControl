#include "stdafx.h"
#include "StyleManager.h"

// ------------------------------------------------------------------
// 单例
StyleManager& StyleManager::Get()
{
	static StyleManager instance;
	return instance;
}

void StyleManager::SetControlStyle(CD2DControlUI* pControl, const CAtlString& classNames)
{
	Get().SetControlClass(pControl, classNames);
}
// ------------------------------------------------------------------
// 解析类名字符串 "btn btn-cancel" -> ["btn", "btn-cancel"]
void StyleManager::ParseClassNames(const CAtlString& classNames, CAtlArray<CAtlString>& outNames)
{
	outNames.RemoveAll();
	if (classNames.IsEmpty())
		return;

	int start = 0;
	int length = classNames.GetLength();
	for (int i = 0; i <= length; i++)
	{
		if (i == length || classNames[i] == L' ')
		{
			if (i > start)
			{
				CAtlString name = classNames.Mid(start, i - start);
				name.Trim();
				if (!name.IsEmpty())
					outNames.Add(name);
			}
			start = i + 1;
		}
	}
}

// ------------------------------------------------------------------
// 注册类样式
void StyleManager::RegisterClassStyle(const CAtlString& className, const ClassStyleSet& styleSet)
{
	// 直接覆盖（支持更新）
	m_classStyles[className] = styleSet;
}

// 注销类样式
void StyleManager::UnregisterClassStyle(const CAtlString& className)
{
	m_classStyles.RemoveKey(className);
}

// ------------------------------------------------------------------
// 设置控件类名
void StyleManager::SetControlClass(CD2DControlUI* pControl, const CAtlString& classNames)
{
	if (!pControl)
		return;

	CAtlArray<CAtlString> names;
	ParseClassNames(classNames, names);
	//m_controlClasses[pControl] = names;   // 覆盖
	ApplyClassStyles(pControl, names);
}

// 添加单个类
void StyleManager::AddControlClass(CD2DControlUI* pControl, const CAtlString& className)
{
	if (!pControl || className.IsEmpty())
		return;

	ATL::CAtlMap<CD2DControlUI*, CSimpleArray<CAtlString>>::CPair* pPair = m_controlClasses.Lookup(pControl);
	CSimpleArray<CAtlString>* pNames = NULL;
	if (pPair)
		pNames = &pPair->m_value;
	if (pNames)
	{
		// 检查是否已存在
		bool exists = false;
		for (int i = 0; i < pNames->GetSize(); i++)
		{
			if ((*pNames)[i] == className)
			{
				exists = true;
				break;
			}
		}
		if (!exists)
			pNames->Add(className);
	}
	else
	{
		CSimpleArray<CAtlString> newList;
		newList.Add(className);
		//m_controlClasses[pControl] = newList;
	}
	ApplyClassStyles(pControl);
}

// 移除单个类
void StyleManager::RemoveControlClass(CD2DControlUI* pControl, const CAtlString& className)
{
	if (!pControl)
		return;
	CAtlMap<CD2DControlUI*, CSimpleArray<CAtlString>>::CPair* pPair = m_controlClasses.Lookup(pControl);
	CSimpleArray<CAtlString>* pNames = NULL;
	if (pPair)
		pNames = &pPair->m_value;
	if (!pNames)
		return;
	for (int i = 0; i < pNames->GetSize(); i++)
	{
		if ((*pNames)[i] == className)
		{
			pNames->RemoveAt(i);
			break;
		}
	}
	if (pNames->GetSize() == 0)
		m_controlClasses.RemoveKey(pControl);
	ApplyClassStyles(pControl);
}

// ------------------------------------------------------------------
// 控件销毁时清理
void StyleManager::RemoveControl(CD2DControlUI* pControl)
{
	if (!pControl)
		return;
	m_controlClasses.RemoveKey(pControl);
}

// ------------------------------------------------------------------
// 刷新某个类的所有控件（当该类样式更新时）
void StyleManager::RefreshClassStyle(const CAtlString& className)
{
	// 遍历映射，找到所有使用该类的控件，重新应用
	POSITION pos = m_controlClasses.GetStartPosition();
	while (pos)
	{
		CD2DControlUI* pControl = NULL;
		CSimpleArray<CAtlString> names;
		m_controlClasses.GetNextAssoc(pos, pControl, names);
		bool usesClass = false;
		for (int i = 0; i < names.GetSize(); i++)
		{
			if (names[i] == className)
			{
				usesClass = true;
				break;
			}
		}
		if (usesClass)
			ApplyClassStyles(pControl);
	}
}

// ------------------------------------------------------------------
// 核心：应用类样式到控件

void StyleManager::ApplyClassStyles(CD2DControlUI* pControl, const CAtlArray<CAtlString>& names)
{
	if (!pControl)
		return;

	// 1. 备份控件当前内联样式（默认 + 各状态）
	CD2DStyle inlineDefault = pControl->GetDefaultStyle();
	CAtlMap<ED2DStatus, CD2DStyle> inlineStates;
	for (int s = ED2DStatus::Normal; s < ED2DStatus::ED2DStatusMax; s++)
	{
		ED2DStatus status = (ED2DStatus)s;
		if (pControl->HasStyle(status))
			inlineStates[status] = pControl->GetStyle(status);
	}

	
	// 2. 若没有类名，则恢复到纯内联样式
	if (names.GetCount() == 0)
	{
		pControl->SetDefaultStyle(inlineDefault);
		// 清除可能存在的旧类状态样式（用空样式覆盖）
		for (int s = ED2DStatus::Normal; s < ED2DStatus::ED2DStatusMax; s++)
		{
			ED2DStatus status = (ED2DStatus)s;
			if (!inlineStates.Lookup(status)) // 如果内联没有该状态样式，则移除控件上的
				pControl->SetStyle(status, CD2DStyle());
		}
		// 重新设置内联状态样式
		POSITION pos = inlineStates.GetStartPosition();
		while (pos)
		{
			ED2DStatus status;
			CD2DStyle style;
			inlineStates.GetNextAssoc(pos, status, style);
			pControl->SetStyle(status, style);
		}
		return;
	}

	// 3. 构建累积的类样式集
	ClassStyleSet accumulated;
	for (size_t i = 0; i < names.GetCount(); i++)
	{
		const CAtlString& className = names[i];
		ATL::CAtlMap<CAtlString, ClassStyleSet>::CPair* pPair = m_classStyles.Lookup(className);
		if (!pPair)
			continue;
		ClassStyleSet* pClassStyle = &pPair->m_value;

		// 合并默认样式
		ApplyStyle(accumulated.defaultStyle, pClassStyle->defaultStyle);

		// 合并各状态样式
		POSITION statePos = pClassStyle->stateStyles.GetStartPosition();
		while (statePos)
		{
			ED2DStatus status;
			CD2DStyle style;
			pClassStyle->stateStyles.GetNextAssoc(statePos, status, style);

			ATL::CAtlMap<ED2DStatus, CD2DStyle>::CPair* pExistPair = accumulated.stateStyles.Lookup(status);
			CD2DStyle* pExisting = NULL;
			if (pExistPair)
				pExisting = &pExistPair->m_value;

			if (pExisting)
				ApplyStyle(*pExisting, style);
			else
				accumulated.stateStyles[status] = style;
		}
	}

	// 4. 将类样式写入控件（作为基础层）
	pControl->SetDefaultStyle(accumulated.defaultStyle);
	// 4.1 先完全清除控件上已有的状态样式（避免旧类残留）
	pControl->ClearAllStyle();


	POSITION accPos = accumulated.stateStyles.GetStartPosition();
	while (accPos)
	{
		ED2DStatus status;
		CD2DStyle style;
		accumulated.stateStyles.GetNextAssoc(accPos, status, style);
		pControl->SetStyle(status, style);
	}

	////// 5. 将内联样式重新叠加，保证其优先级最高
	//{
	//	CD2DStyle curDefault = pControl->GetDefaultStyle();
	//	ApplyStyle(curDefault, inlineDefault);
	//	//pControl->SetDefaultStyle(curDefault);
	//}
	POSITION inlinePos = inlineStates.GetStartPosition();
	while (inlinePos)
	{
		ED2DStatus status;
		CD2DStyle style;
		inlineStates.GetNextAssoc(inlinePos, status, style);
		CD2DStyle curState = pControl->GetStyle(status);
		ApplyStyle(curState, style);
		pControl->SetStyle(status, curState);
	}
}

// 原有单参数版本改为调用新函数
void StyleManager::ApplyClassStyles(CD2DControlUI* pControl)
{
	if (!pControl)
		return;
	CSimpleArray<CAtlString> names;
	ATL::CAtlMap<CD2DControlUI*, CSimpleArray<CAtlString>>::CPair* pPair = m_controlClasses.Lookup(pControl);
	if (pPair)
		names = pPair->m_value;
	//ApplyClassStyles(pControl, names);
}

