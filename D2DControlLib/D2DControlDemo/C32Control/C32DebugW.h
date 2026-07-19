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


// 日志记录器类（Unicode 版本）
class CDebugLogger {
private:
	LogLevel   m_CurrentLevel;      // 当前日志级别
	OutputMode m_CurrentMode;       // 当前输出模式
	CStringW   m_LogFilePath;       // 日志文件路径
	bool       m_IsLoggingEnabled;  // 控制日志是否输出的标志
	ULONG      m_IndexLog;

public:
	// 构造函数
	CDebugLogger(LogLevel level = LogLevelDebug, OutputMode mode = OutputModeDebugger,
		const CStringW& filePath = L"debug.log", bool isLoggingEnabled = false)
		: m_CurrentLevel(level)
		, m_CurrentMode(mode)
		, m_LogFilePath(filePath)
		, m_IsLoggingEnabled(isLoggingEnabled)
	{
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
	// 输出信息到调试器（宽字符版本）
	void OutputToDebugger(const CStringW& message) {
		if (m_IsLoggingEnabled) {
			OutputDebugStringW(message);
		}
	}

	// 输出信息到文件（写入 UTF-16 LE，无 BOM）
	void OutputToFile(const CStringW& message) {
		if (!m_IsLoggingEnabled)
			return;

		HANDLE hFile = CreateFileW(m_LogFilePath, GENERIC_WRITE, 0, NULL,
			OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return;

		// 将文件指针移动到末尾
		SetFilePointer(hFile, 0, NULL, FILE_END);

		// 直接写入宽字符字节序列
		DWORD bytesWritten;
		const wchar_t* buffer = message.GetString();
		DWORD bytesToWrite = static_cast<DWORD>(message.GetLength() * sizeof(wchar_t));
		WriteFile(hFile, buffer, bytesToWrite, &bytesWritten, NULL);

		CloseHandle(hFile);
	}

	// 根据模式输出信息
	void OutputMessage(const CStringW& message) {
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

	// 重载：可变参数版本（宽字符格式串）
	void OutputMessage(const wchar_t* format, ...) {
		if (m_IsLoggingEnabled) {
			va_list args;
			va_start(args, format);
			CStringW message;
			message.FormatV(format, args);
			va_end(args);
			OutputMessage(message);
		}
	}

	// 重载：带日志级别的可变参数版本
	void OutputMessage(LogLevel logLevel, const wchar_t* format, ...) {
		if (!m_IsLoggingEnabled)
			return;

		// 根据当前日志级别决定是否输出
		if ((logLevel == LogLevelDebug && m_CurrentLevel != LogLevelDebug) ||
			(logLevel == LogLevelInfo && m_CurrentLevel == LogLevelError)) {
				return;
		}

		va_list args;
		va_start(args, format);
		CStringW message;
		message.FormatV(format, args);
		va_end(args);

		CStringW logMessage;
		switch (logLevel) {
		case LogLevelDebug:
			logMessage.Format(L"[DEBUG] %s", message);
			break;
		case LogLevelInfo:
			logMessage.Format(L"[INFO] %s", message);
			break;
		case LogLevelError:
			logMessage.Format(L"[ERROR] %s", message);
			break;
		}
		OutputMessage(logMessage);
	}

public:
	// 输出调试信息
	void Debug(const CStringW& message) {
		if (m_IsLoggingEnabled && m_CurrentLevel == LogLevelDebug) {
			CStringW logMessage;
			logMessage.Format(L"[DEBUG] %s", message);
			OutputMessage(logMessage);
		}
	}

	// 输出一般信息（带自增序号）
	void Info(const CStringW& message) {
		if (m_IsLoggingEnabled && (m_CurrentLevel == LogLevelDebug || m_CurrentLevel == LogLevelInfo)) {
			CStringW logMessage;
			logMessage.Format(L"[INFO] %d %s", m_IndexLog++, message);
			OutputMessage(logMessage);
		}
	}

	// 输出错误信息
	void Error(const CStringW& message) {
		if (m_IsLoggingEnabled) {
			CStringW logMessage;
			logMessage.Format(L"[ERROR] %s", message);
			OutputMessage(logMessage);
		}
	}

	// 辅助格式化函数（返回 CStringW）
	CStringW Format(const wchar_t* format, ...) {
		ULONG length = 4096;
		CStringW result;
		wchar_t* buffer = new wchar_t[length];
		if (buffer == nullptr)
			return result;

		ZeroMemory(buffer, length * sizeof(wchar_t));

		va_list arglist;
		va_start(arglist, format);
		int n = vswprintf_s(buffer, length, format, arglist);
		va_end(arglist);

		if (n > 0)
			result = buffer;

		delete[] buffer;
		return result;
	}
};
extern CDebugLogger theLog;

#define ZxDebug(x) \
{\
	CString Str000;\
	Str000.Format x;\
	theLog.Debug(Str000);\
}
#define ZxDebugEx(x) \
{\
	CString Str000;\
	Str000.Format x;\
	theLog.Info(Str000);\
}


int CDebugLoggerTest();
