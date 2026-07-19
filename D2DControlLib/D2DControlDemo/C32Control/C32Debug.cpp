#include "stdafx.h"
#include "C32Debug.h"


CDebugLogger theLog;


int CDebugLoggerTest() {
	setlocale(LC_ALL, "Chinese");

	CDebugLogger logger;
	// 设置日志级别为 INFO
	logger.SetLogLevel(LogLevelInfo);
	// 设置输出模式为文件
	logger.SetOutputMode(OutputModeFile);

	logger.Format("%ws", L"中国人民万岁");

	// 使用重载的 OutputMessage 函数
	logger.OutputMessage(LogLevelInfo, "Info message with string %s", "test");
	logger.OutputMessage(LogLevelError, "Error message with float %.2f", 3.14);

	// 禁用日志输出
	logger.SetLoggingEnabled(false);

	// 这些日志将不会输出
	logger.OutputMessage(LogLevelDebug, "This debug message won't be logged", 0);
	logger.OutputMessage(LogLevelInfo, "This info message won't be logged", 0);
	logger.OutputMessage(LogLevelError, "This error message won't be logged", 0);

	return 0;
}