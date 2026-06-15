#pragma once
#include "C32Env.h"     //包含的头文件和项目链接的基本设置
#include "C32Control.h" // 封装Win32的基本控件类和自定义控件
#include "C32DebugMsg.h" // 输出控件消息和通知相关的结构或参数信息
#include "C32Debug.h"
#include "C32Util.h"     // 与界面相关的帮助函数
#include "C32Helper.h"   // 写逻辑相关的类和函数
#include "C32D2D1.h"
#include "C32MFC.h"

/*
	初始化C32 Lib
*/
VOID InitC32(HINSTANCE hInstance);