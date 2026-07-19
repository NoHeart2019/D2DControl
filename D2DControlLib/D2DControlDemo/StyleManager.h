#pragma once

#include <atlcoll.h>
#include <atlstr.h>
#include "D2DControl/D2DControlUI.h"   // 包含 CD2DStyle, ED2DStatus 等

// 类样式集：每个类名对应一套完整的样式（默认 + 各状态）
struct ClassStyleSet
{
	CD2DStyle                       defaultStyle;
	CAtlMap<ED2DStatus, CD2DStyle>  stateStyles;

	ClassStyleSet() {}

	// 拷贝构造函数
	ClassStyleSet(const ClassStyleSet& other)
		: defaultStyle(other.defaultStyle)
	{
		POSITION pos = other.stateStyles.GetStartPosition();
		while (pos)
		{
			ED2DStatus key;
			CD2DStyle val;
			other.stateStyles.GetNextAssoc(pos, key, val);
			stateStyles[key] = val;
		}
	}

	// 赋值运算符
	ClassStyleSet& operator=(const ClassStyleSet& other)
	{
		if (this != &other)
		{
			defaultStyle = other.defaultStyle;
			stateStyles.RemoveAll();
			POSITION pos = other.stateStyles.GetStartPosition();
			while (pos)
			{
				ED2DStatus key;
				CD2DStyle val;
				other.stateStyles.GetNextAssoc(pos, key, val);
				stateStyles[key] = val;
			}
		}
		return *this;
	}
};

class StyleManager
{
public:
	static StyleManager& Get();
	static void SetControlStyle(CD2DControlUI* pControl, const CAtlString& classNames);


	// 注册类样式
	void RegisterClassStyle(const CAtlString& className, const ClassStyleSet& styleSet);
	// 注销类样式
	void UnregisterClassStyle(const CAtlString& className);

	// 设置控件类名（空格分隔多个类名，如 L"btn btn-cancel"）
	void SetControlClass(CD2DControlUI* pControl, const CAtlString& classNames);


	// 添加单个类
	void AddControlClass(CD2DControlUI* pControl, const CAtlString& className);
	// 移除单个类
	void RemoveControlClass(CD2DControlUI* pControl, const CAtlString& className);

	// 控件销毁前调用，清理映射
	void RemoveControl(CD2DControlUI* pControl);

	// 重新应用类样式（当类样式被重新注册后，刷新所有相关控件）
	void RefreshClassStyle(const CAtlString& className);


private:
	StyleManager() {}
	~StyleManager() {}
	StyleManager(const StyleManager&);
	StyleManager& operator=(const StyleManager&);

	// 解析空格分隔的类名字符串，返回类名数组
	void ParseClassNames(const CAtlString& classNames, CAtlArray<CAtlString>& outNames);

	// 核心：根据控件当前类名列表，将类样式合并到控件上
	void ApplyClassStyles(CD2DControlUI* pControl);

	// 直接根据类名数组应用样式
	void ApplyClassStyles(CD2DControlUI* pControl, const CAtlArray<CAtlString>& names);

	// 全局样式注册表：类名 -> 样式集
	CAtlMap<CAtlString, ClassStyleSet> m_classStyles;
	// 控件 -> 类名列表的映射
	CAtlMap<CD2DControlUI*, CSimpleArray<CAtlString>> m_controlClasses; 
};


void RegisterStickyNoteStyles();

// 在合适的头文件或 cpp 中
void RegisterAINavigatorStyles();