#pragma once

#include <atlbase.h>
#include <atlstr.h>
#include <windows.h>
#include <iostream>

// 定义日志级别枚举
enum LogLevel {
	LogLevelDebug,
	LogLevelInfo,
	LogLevelError
};

// 定义输出模式枚举
enum OutputMode {
	OutputModeDebugger,
	OutputModeFile
};

// 日志记录器类
class CDebugLogger {
private:
	// 成员变量，采用 m_X 格式，_ 后首字母大写
	LogLevel m_CurrentLevel;  // 当前日志级别
	OutputMode m_CurrentMode; // 当前输出模式
	CString m_LogFilePath;   // 日志文件路径
	bool m_IsLoggingEnabled;  // 控制日志是否输出的标志
	ULONG m_IndexLog;
public:
	// 构造函数，初始化日志级别、输出模式、日志文件路径和日志启用状态
	CDebugLogger(LogLevel level = LogLevelDebug, OutputMode mode = OutputModeDebugger, 
		const CStringA& filePath = "debug.log", bool isLoggingEnabled = false)
		: m_CurrentLevel(level), 
		m_CurrentMode(mode), 
		m_LogFilePath(filePath), 
		m_IsLoggingEnabled(isLoggingEnabled) {
		m_IndexLog = 0;
		}

	// 设置日志级别
	void SetLogLevel(LogLevel level) {
		m_CurrentLevel = level;
	}

	// 设置输出模式
	void SetOutputMode(OutputMode mode) {
		m_CurrentMode = mode;
	}

	// 设置日志是否输出
	void SetLoggingEnabled(bool enabled) {
		m_IsLoggingEnabled = enabled;
	}


public:
	// 输出信息到调试器
	void OutputToDebugger(const CStringA& message) {
		if (m_IsLoggingEnabled) {
			OutputDebugStringA(message);
		}
	}

	// 输出信息到文件
	void OutputToFile(const CStringA& message) {
		if (m_IsLoggingEnabled) {
			HANDLE hFile = CreateFile(m_LogFilePath, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE) {
				return;
			}
			// 将文件指针移动到文件末尾
			SetFilePointer(hFile, 0, NULL, FILE_END);
			DWORD bytesWritten;

			CStringA nonConstMessage = message; // 创建一个非 const 的副本
			const char* buffer = nonConstMessage.GetBuffer();
			int length = nonConstMessage.GetLength();

			// 写入日志信息
			WriteFile(hFile, buffer, length, &bytesWritten, NULL);

			nonConstMessage.ReleaseBuffer(); // 释放缓冲区
			CloseHandle(hFile);
		}
	}

	// 根据模式输出信息
	void OutputMessage(const CStringA& message) {
		if (m_IsLoggingEnabled) {
			switch (m_CurrentMode) {
			case OutputModeDebugger:
				OutputToDebugger(message);
				break;
			case OutputModeFile:
				OutputToFile(message);
				break;
			}
		}
	}

	// 重载 OutputMessage 函数，处理可变参数
	void OutputMessage(const char* format, ...) {
		if (m_IsLoggingEnabled) {
			va_list args;
			va_start(args, format);
			CStringA message;
			message.FormatV(format, args);
			va_end(args);
			OutputMessage(message);
		}
	}

	// 重载 OutputMessage 函数，接受 LogLevel 和可变参数
	void OutputMessage(LogLevel logLevel, const char* format, ...) {
		if (m_IsLoggingEnabled) {
			if ((logLevel == LogLevelDebug && m_CurrentLevel != LogLevelDebug) ||
				(logLevel == LogLevelInfo && m_CurrentLevel == LogLevelError)) {
					return;
			}

			va_list args;
			va_start(args, format);
			CStringA message;
			message.FormatV(format, args);
			va_end(args);

			CStringA logMessage;
			switch (logLevel) {
			case LogLevelDebug:
				logMessage.Format("[DEBUG] %s", message);
				break;
			case LogLevelInfo:
				logMessage.Format("[INFO] %s", message);
				break;
			case LogLevelError:
				logMessage.Format("[ERROR] %s", message);
				break;
			}

			OutputMessage(logMessage);
		}
	}


public:
	// 输出调试信息
	void Debug(const CStringA& message) {
		if (m_IsLoggingEnabled && m_CurrentLevel == LogLevelDebug) {
			CStringA logMessage;
			logMessage.Format("[DEBUG] %s", message);
			OutputMessage(logMessage);
		}
	}

	// 输出信息
	void Info(const CStringA& message) {
		if (m_IsLoggingEnabled && (m_CurrentLevel == LogLevelDebug || m_CurrentLevel == LogLevelInfo)) {
			CStringA logMessage;
			logMessage.Format("[INFO] %d %s", m_IndexLog++, message);
			OutputMessage(logMessage);
		}
	}

	// 输出错误信息
	void Error(const CStringA& message) {
		if (m_IsLoggingEnabled) {
			CStringA logMessage;
			logMessage.Format("[ERROR] %s", message);
			OutputMessage(logMessage);
		}
	}

	CStringA Format(const char* formats, ...)
	{
		ULONG n = 0;
		ULONG length = 4096;
		CStringA String = "";
		char* Ptr = new char[length];
		if (Ptr == NULL)
			return String;
		RtlZeroMemory(Ptr, length);
		va_list arglist = NULL;
		va_start(arglist, formats);
		n = vsprintf_s(Ptr, length, formats, arglist);
		va_end(arglist);

		String = Ptr;

		return String;
	}


};

extern CDebugLogger theLog;

#define ZxDebug(x) \
{\
	CStringA Str000;\
	Str000.Format x;\
	theLog.Debug(Str000);\
}
#define ZxDebugEx(x) \
{\
	CStringA Str000;\
	Str000.Format x;\
	theLog.Info(Str000);\
}


int CDebugLoggerTest();
