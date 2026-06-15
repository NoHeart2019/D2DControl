#pragma once
#include <atlcoll.h>
#include <atlbase.h>  // 包含 ATL 基础头文件
#include <atlstr.h>   // 包含 CString 支持
#include <windows.h>  // 包含 Windows API 头文件

#define C32A2W(String) (LPCSTR)String ? CA2W(String, CP_ACP) : L""
#define C32A2WUTF8(String) (LPCSTR)String ? CA2W(String, CP_UTF8) : L""
#define C32W2A(String) (LPCWSTR)String ? CW2A(String, CP_ACP) : ""
#define C32W2AUTF8(String) (LPCWSTR)String ? CW2A(String, CP_UTF8) : ""


#define C32StringErrorIndex  -1
#define C32StringIsErrorIndex(x) (x == -1)

#define C32Bit(n)  (ULONG)(1 << n)
#define C32SetBit(x, n, v)  ((x) =((((ULONG)x) &~C32Bit(n)) | (v << n)))
#define C32GetBit(x, n) ((BOOL)(((ULONG)x & C32Bit(n)) >>n))
#define C32AndValue(x, y) (x & y)
#define C32TestFlag(x, v)(x & v)

PVOID  C32AllocateBuffer(int Length);

VOID   C32FreeBuffer(PVOID Buffer);

LPWSTR C32AllocateString(int Length);

template<typename T>
__inline  int C32QsortIntAsc(const void * Elem1, const void * Elem2)
{
	T n1 = *(T*)(Elem1);
	T n2 = *(T*)(Elem2);

	return n1 > n2;
}


class C32Buffer
{
public:
	C32Buffer()
	{
		Init();
		
	}

	C32Buffer(int Length)
	{
		Init();
		Assign(Length);
	}

	~C32Buffer()
	{
		if (!m_Attach)
		{
			if (m_Buffer)
			{
				C32FreeBuffer(m_Buffer);
				m_Buffer = NULL;
			}
			
		}
	}

public:
	operator LPVOID()
	{
		return m_Buffer;
	}

	operator LPCSTR()
	{
		return (LPCSTR)m_Buffer;
	}
	
	operator LPSTR()
	{
		return (LPSTR)m_Buffer;
	}
	
	operator LPCWSTR()
	{
		return (LPCWSTR)m_Buffer;
	}
	
	operator LPWSTR()
	{
		return (LPWSTR)m_Buffer;
	}

	BOOL Assign(int Length)
	{
		FreeBuffer();
		m_Buffer = C32AllocateBuffer(Length);
		m_Length = Length;

		return (m_Buffer ? TRUE : FALSE);
	}

	VOID Attach(PVOID Buffer, int Length)
	{
		
		m_Buffer = Buffer;
		m_Length = Length;
		m_Attach = TRUE;

	}

	VOID Detach()
	{
		m_Buffer = NULL;
		m_Length = NULL;
	}

	VOID FreeBuffer()
	{
		if (m_Attach  == FALSE && m_Buffer)
		{
			C32FreeBuffer(m_Buffer);
			m_Buffer = NULL;
			m_Length = 0;
		}
	}

	VOID Init()
	{
		m_Length = 0;
		m_Buffer = NULL;
		m_Attach = FALSE;
	}

	int GetLength()
	{
		return m_Length;
	}

	BOOL CopyBuffer(PVOID Buffer, int Length)
	{
		if (Length > m_Length)
		{
			Assign(m_Length);
			
		}
		if (m_Buffer)
			RtlCopyMemory(m_Buffer, Buffer, Length);
		return (m_Buffer!= NULL);
	}

	PVOID GetBuffer()
	{
		return m_Buffer;
	}

	BOOL IsEmptry()
	{
		return (m_Buffer ? FALSE : TRUE);
	}

	C32Buffer(const C32Buffer& Right)
	{
		C32AllocateBuffer(Right.m_Length);
		if (m_Buffer)
		{
			m_Length = Right.m_Length;
			RtlCopyMemory(m_Buffer, Right.m_Buffer, m_Length);
		}
	}

	C32Buffer& operator=(const C32Buffer& Right)
	{
		//C32Buffer(Right);
		return *this;
	}

	
	

	PVOID m_Buffer;
	int   m_Length;
	BOOL  m_Attach;
};

class C32String : public C32Buffer
{

public:
	C32String()
	{
		Init();
	}

	~C32String()
	{

	}

	operator LPWSTR() const
	{
		return (LPWSTR)(m_Buffer? m_Buffer : L"");
	}

	operator LPCWSTR() const
	{
		return (LPCWSTR)(m_Buffer? m_Buffer : L"");

	}

	operator CString() const
	{
		return (LPCWSTR)(m_Buffer? m_Buffer : L"");
	}

	/*
		String 操作

	*/
	BOOL AssignStringW(int CharCount)
	{
		//多分配1个字符
		CharCount++;

		if(Assign(CharCount*2) == TRUE)
		{
			m_TotalCount = CharCount;
		}else
		{
			m_TotalCount = 0;
		}
		m_DbgString = (LPWSTR)m_Buffer;
		return (m_Buffer ? TRUE : FALSE);
	}

	/*
		默认是WCHAR
	*/
	LPWSTR GetString()
	{
		return GetStringW();
	}

	int GetStringLength()
	{
		return GetStringLengthW();
	}

	LPWSTR GetStringW()
	{
		return (LPWSTR)m_Buffer;
	}

	LPSTR GetStringA()
	{
		return (LPSTR)m_Buffer;
	}

	int GetStringLengthW()
	{
		int Count = 0;
		if (m_Buffer)
		{
			Count = (int)wcslen((const wchar_t*)m_Buffer);
		}
		return Count;
	}

	int GetStringLengthA()
	{
		int Count = 0;
		if (m_Buffer)
		{
			Count = (int)strlen((const char*)m_Buffer);
		}
		return Count;
	}

	int GetTotalCount()
	{
		int TotalSize = GetLength();

		return (TotalSize? TotalSize/2 -1 : 0) ;
	}

	int GetCount()
	{
		return GetStringLengthW();
	}

	int GetByte()
	{
		return m_Count*2;
	}

	BOOL IsEmptry()
	{
		return GetCount() == 0;
	}

	VOID SetCount()
	{
		if (m_Buffer)
		{
			m_Count = (int)wcslen((LPCWSTR)m_Buffer);
		}
	}

	VOID SetCount(int Count)
	{
		m_Count = Count;
	}

	int GetFormatLength(__in __format_string LPCWSTR Format, va_list argList)
	{
		wchar_t Buffer[2048]={};

		return wvsprintfW(Buffer, Format, argList);
	}

	C32String& FormatString(__in __format_string LPCWSTR Format, ...)
	{
		int Count = 0;
		va_list argList;
		va_start(argList, Format);

		m_Count = GetFormatLength(Format, argList);

		if (m_Count)
		{
			AssignStringW(m_Count);
			m_Count = wvsprintfW(GetString(), Format, argList);
		}
		va_end( argList );

		return *this;
	}

	C32String& AppendString(LPCWSTR Str)
	{
		if (Str== NULL)
			return *this;

		int AppendCount = (int)wcslen(Str);
		if (m_Count+ AppendCount >= m_TotalCount )
		{
			LPWSTR OldString = (LPWSTR)m_Buffer;
			Detach();
			if(AssignStringW(m_Count+ AppendCount))
			{
				CopyString(OldString, m_Count);
				delete OldString;
				
			}else
			{
				//分配内存失败
				m_Buffer = OldString;
				m_Length = m_Count*2;

				return *this;
			}
			
		}

		wcscpy_s((LPWSTR)m_Buffer + m_Count, m_TotalCount -m_Count, Str);
		m_Count += (int)wcslen(Str);
		

		return *this;

	}

	C32String& CopyString(LPWSTR Str)
	{
		if (Str== NULL)
			return *this;

		int Count = (int)wcslen(Str);
		if (Count == 0)
			return *this;

		if(AssignStringW(Count))
		{
			CopyString(Str, Count);
		}

		return *this;

	}

	C32String& CopyString(LPCWSTR Str)
	{
		CopyString((LPWSTR)Str);

		return *this;
	}

	C32String& AppendString(LPWSTR Str)
	{
		AppendString((LPCWSTR)Str);
		return *this;
	}

	BOOL CopyString(LPWSTR Str, int Count)
	{
		if (Str)
		{
			wcscpy_s((LPWSTR)m_Buffer, m_TotalCount,Str);
			m_Count = Count;
		}
		return TRUE;
	}

	
	VOID Init()
	{
		m_Count = 0;
		m_TotalCount = 0;

		m_DbgString = NULL;
	}

public:
	int m_Count;
	int m_TotalCount;
	LPWSTR m_DbgString;

	const C32String& operator = (LPWSTR Str)
	{
		*this = (LPCWSTR)Str;

		return *this;
	}

	const C32String& operator = (LPCWSTR Str)
	{
		CopyString(Str);

		return *this;
	}

	const C32String& operator += (LPCWSTR Str)
	{
		AppendString(Str);

		return *this;
	}

	const C32String& operator += (LPWSTR Str)
	{
		*this +=(LPCWSTR)Str;

		return *this;
	}
	
	const C32String& operator += (const C32String& Str)
	{
		*this +=(LPCWSTR)Str;

		return *this;
	}


	C32String(LPCWSTR Str)
	{
		*this = Str;
	}

	C32String(LPWSTR Str)
	{
		operator= (Str);
	}

	C32String(const C32String& Right)
	{
		*this = Right;
	}

	C32String& operator=(const C32String& Right)
	{
		//C32Buffer(Right);
		AssignStringW(Right.m_Length);
		if (m_Buffer)
		{
			m_Count = Right.m_Count;
			m_Length = Right.m_Length;
			m_TotalCount = Right.m_TotalCount;
			RtlCopyMemory(m_Buffer, Right.m_Buffer, m_Count*2);
			
		}
		return *this;
	}

};

/*
    线程同步
*/

class C32SynThread
{
public:
    C32SynThread (){
		m_Parameter = NULL;
        m_Handle = NULL;
        m_bTerminate = FALSE;
        m_ThreadId = 0;
    }

    ~C32SynThread(){

        Close();
    }

public:
    BOOL Create(LPTHREAD_START_ROUTINE StartRoutine, LPVOID Parameter = NULL)
    {
        if (StartRoutine == NULL)
            return FALSE;
        LPVOID lpParameter = (Parameter ? Parameter : (PVOID)this);

        HANDLE Handle = CreateThread(NULL, 0, StartRoutine, lpParameter, 0, &m_ThreadId);

        m_Handle = Handle;

        return (Handle ? TRUE :FALSE);
    }

    BOOL Wait(DWORD dwMilliseconds = INFINITE)
    {
        BOOL bWait = FALSE;
        DWORD Ret;

        if (m_Handle)
        {
            Ret = WaitForSingleObject(m_Handle, dwMilliseconds);
            if (Ret >= 0)
                bWait = TRUE;
        }


        return bWait;
    }
    // Thread Stop Running
    VOID SetTerminate(BOOL Terminate = TRUE)
    {
        m_bTerminate = Terminate;
    }

    BOOL IsTerminate()
    {
        return m_bTerminate;
    }

    BOOL Suspend()
    {
        BOOL bSuspend = FALSE;
        if (m_Handle)
        {
            DWORD Count;
            Count = SuspendThread(m_Handle);

            bSuspend = (Count == -1 ? FALSE : TRUE);
        }

        return bSuspend;
    }

    BOOL Resume()
    {
        BOOL bResume = FALSE;
        if (m_Handle)
        {
            DWORD Count;
            Count = ResumeThread(m_Handle);

            bResume = (Count == -1 ? FALSE : TRUE);
        }

        return bResume;

    }

    BOOL Close()
    {
        BOOL bClose = FALSE;
        if (m_Handle)
        {
            bClose = CloseHandle(m_Handle);
            m_Handle = NULL;
        }

        return !!bClose;
    }

public:
	VOID SetParameter(LPVOID lpParameter)
	{
		m_Parameter = lpParameter;
	}
	
	PVOID GetParameter()
	{
		return m_Parameter ;
	}

	PVOID  m_Parameter;
    BOOL   m_bTerminate;
    HANDLE m_Handle;
    DWORD  m_ThreadId;
};

class C32SynEvent
{
public:
    C32SynEvent()
    {
        m_Event = NULL;
		m_LastEror = 0;
    }
    ~C32SynEvent()
    {
		Close();
    }

public:

    BOOL Create(BOOL  SignalStatie = TRUE, LPCWSTR EventName = NULL)
    {
        m_Event = CreateEvent(NULL, TRUE, SignalStatie, EventName);
		m_LastEror = GetLastError();
        return (m_Event ? TRUE : FALSE);
    }

	BOOL IsOpen()
	{
		if (m_LastEror == ERROR_ALREADY_EXISTS)
			return TRUE;

		return FALSE;
	}


    VOID Close()
    {
        if (m_Event)
        {
            CloseHandle(m_Event);
            m_Event = NULL;
        }
    }

    BOOL Set()
    {
        BOOL bSet = FALSE;
        if (m_Event)
            bSet = SetEvent(m_Event);

        return bSet;
    }

	HANDLE GetEvent()
	{
		return m_Event;
	}

    BOOL Reset()
    {
        BOOL bSet = FALSE;
        if (m_Event)
            bSet = ResetEvent(m_Event);

        return bSet;
    }

    DWORD WaitFor(DWORD  dwMilliseconds = INFINITE)
    {
        DWORD Ret = WAIT_FAILED;
		if (m_Event)
			Ret = WaitForSingleObject(m_Event, dwMilliseconds);
        

        return Ret;
    }

private:
    HANDLE m_Event;
	DWORD  m_LastEror;
};

/*
	封装CreateFile 相关的API
	std 看不懂所以少用
*/
class C32File
{
public:
	C32File()
	{
		m_ErrorCode = 0;
		m_hFile = NULL;
	}

	~C32File()
	{
		m_ErrorCode = 0;
		Close();
	}

public:
	HANDLE Create(LPCWSTR FilePath, DWORD dwCreateDisposition = OPEN_ALWAYS)
	{
		DWORD  dwDesiredAccess = FILE_GENERIC_READ | FILE_GENERIC_WRITE;
		DWORD  dwShareMode = FILE_SHARE_READ;
		//DWORD  dwCreateDisposition = OPEN_ALWAYS;
		DWORD  dwAttribute = FILE_ATTRIBUTE_NORMAL;
		HANDLE hFile = CreateFile(FilePath, dwDesiredAccess, dwShareMode, NULL, dwCreateDisposition,dwAttribute, NULL);
		m_hFile = hFile;
		m_ErrorCode = GetLastError();

		return (hFile ==  INVALID_HANDLE_VALUE ? NULL : hFile);
	}
	
	HANDLE Open(LPCWSTR FilePath, DWORD dwCreateDisposition = OPEN_EXISTING)
	{
		return Create(FilePath, dwCreateDisposition);
	}

	VOID   Close()
	{
		if (m_hFile)
		{
			CloseHandle(m_hFile);
			m_hFile = NULL;
		}
	}

	ULONGLONG Seek(ULONG Method, LONGLONG Distance)
	{
		LARGE_INTEGER li;

		li.QuadPart = Distance;
		if (m_hFile == INVALID_HANDLE_VALUE)
			return -1;

		li.LowPart = ::SetFilePointer(m_hFile, li.LowPart, &li.HighPart, Method);
		if (li.LowPart == INVALID_SET_FILE_POINTER)
			return -1;

		return li.QuadPart;
	}



	ULONG  Write(PVOID Buffer, ULONG Length)
	{
		if (m_hFile == NULL || Buffer == NULL || Length == 0)
			return 0;
		
		DWORD Count = 0;
		BOOL bWrite = WriteFile(m_hFile, Buffer, Length, &Count, NULL);
		m_ErrorCode = GetLastError();
		if (bWrite == 0)
			return 0;

		return Count;
	}

	ULONG  Read(PVOID Buffer, ULONG Length)
	{
		if (m_hFile == NULL || Buffer == NULL || Length == 0)
			return 0;

		DWORD Count = 0;
		BOOL bRet = ReadFile(m_hFile, Buffer, Length, &Count, NULL);
		m_ErrorCode = GetLastError();
		if (bRet == 0)
			return 0;

		return Count;
	}

	ULONG   SetFileOffset(ULONG Offset, DWORD MoveMethod = FILE_BEGIN)
	{
		if (m_hFile == NULL)
			return 0;

		return SetFilePointer(m_hFile, MoveMethod, NULL, MoveMethod);
	}

	ULONGLONG GetSize()
	{
		ULONGLONG FileSize = 0;
		if (m_hFile)
		{
			DWORD FileLow = 0, High = 0;

			FileLow = GetFileSize(m_hFile, &High);
			
			FileSize = (((ULONGLONG)High << 32) | FileLow);
		}

		return FileSize;
	}

	BOOL SetFileSize(ULONGLONG Size)
	{
		LARGE_INTEGER li;
		DWORD FileSize;
		if (m_hFile == NULL)
			return FALSE;
		
		li.QuadPart = (LONGLONG)Size;
		FileSize = SetFilePointer(m_hFile, li.LowPart, &li.HighPart, FILE_BEGIN);
		if (FileSize)
		{
			SetEndOfFile(m_hFile);
			return TRUE;
		}

		return FALSE;
	}

public:
	DWORD m_ErrorCode;
	HANDLE m_hFile;

};

class C32ResorceFile
{
public:
	C32ResorceFile()
	{
		Init();
	}
	
	~C32ResorceFile()
	{

	}
	
	
	//将资源释放到磁盘.
	DWORD   FreeResourceDisk(int RsrcId, LPCWSTR Type, LPCWSTR FilePath)
	{
		DWORD Size = 0;
		if(LoadResource(RsrcId, (LPWSTR)Type))
		{
			C32File File;
			File.Create(FilePath);
			Size = File.Write(m_Address, m_Size);
		}

		return Size;
	}
private:
	LPVOID LoadResource(int RsrcId, LPWSTR Type)
	{
		m_hRsrc = FindResource(NULL, MAKEINTRESOURCE(RsrcId), Type);
		if (m_hRsrc)
		{
			m_Size = SizeofResource(NULL,m_hRsrc);
			m_hGlobal = ::LoadResource(NULL, m_hRsrc);
			if (m_hGlobal)
			{
				m_Address = LockResource(m_hGlobal);
			}

		}

		return m_Address;
	}


public:
	VOID Init()
	{
		m_Address = NULL;
		m_Size = 0;
		m_hGlobal = NULL;
		m_hRsrc = NULL;
	}
	LPVOID  m_Address;
	DWORD   m_Size;
	HGLOBAL m_hGlobal;
	HRSRC  m_hRsrc;
};

/*
	计算执行时间的帮助类
*/

class C32RunTime
{
#define C32_ONE_SECOND  1000 //ms
#define C32_ONE_MINUTE  60*1000
public:
	C32RunTime(LPCSTR Line)
	{
		m_Tick = GetTickCount64();
		m_Name = Line;
	}
	
	virtual ~C32RunTime()
	{
		ULONGLONG Time = GetElapseTime();
		ULONGLONG Second = Time /C32_ONE_SECOND;
		ZxDebugEx((__FUNCTION__"():: %s Ms:%lld Second: %lld\n",m_Name.GetString(),Time, Second));
	}

	VOID Init()
	{
		m_Tick = 0ULL;
		m_Tick1 = 0ULL;
	}

	ULONGLONG GetElapseTime()
	{
		ULONGLONG Time;
		m_Tick1 = GetTickCount64();

		Time = m_Tick1 - m_Tick;

		return Time;
	}

public:
	
	ULONGLONG m_Tick;
	ULONGLONG m_Tick1;
	CStringA  m_Name;

};

/*
	ShellExecute 帮助类
*/

typedef  HINSTANCE  (_cdecl *ShellExecuteDecl)(__in_opt HWND hwnd, __in_opt LPCWSTR lpOperation, __in LPCWSTR lpFile, __in_opt LPCWSTR lpParameters,
	__in_opt LPCWSTR lpDirectory, __in INT nShowCmd);

extern ShellExecuteDecl  ShellExecuteProcAddress;

class C32Shell {
public:
	// 打开文件或文件夹
	static bool Open(const CString& filePath) {
		HINSTANCE result = ShellExecuteProcAddress(
			NULL,                // 父窗口句柄
			_T("open"),          // 操作类型
			filePath,            // 文件路径
			NULL,                // 参数
			NULL,                // 默认目录
			SW_SHOWNORMAL        // 显示方式
			);

		if ((int)result <= 32) {
			return false; // 返回失败
		}
		return true; // 返回成功
	}

	// 执行命令（例如运行程序）
	static bool Execute(const CString& command) {
		HINSTANCE result = ShellExecuteProcAddress(
			NULL,                // 父窗口句柄
			_T("open"),          // 操作类型
			command,             // 命令
			NULL,                // 参数
			NULL,                // 默认目录
			SW_SHOWNORMAL        // 显示方式
			);

		if ((int)result <= 32) {
			return false; // 返回失败
		}
		return true; // 返回成功
	}

	// 打印文件（仅支持打印功能的文件类型）
	static bool Print(const CString& filePath) {
		HINSTANCE result = ShellExecuteProcAddress(
			NULL,                // 父窗口句柄
			_T("print"),         // 操作类型
			filePath,            // 文件路径
			NULL,                // 参数
			NULL,                // 默认目录
			SW_HIDE              // 隐藏窗口
			);

		if ((int)result <= 32) {
			return false; // 返回失败
		}
		return true; // 返回成功
	}

	// 打开 URL（浏览器打开网页）
	static bool OpenURL(const CString& url) {
		HINSTANCE result = ShellExecuteProcAddress(
			NULL,                // 父窗口句柄
			_T("open"),          // 操作类型
			url,                 // URL 地址
			NULL,                // 参数
			NULL,                // 默认目录
			SW_SHOWNORMAL        // 显示方式
			);

		if ((int)result <= 32) {
			return false; // 返回失败
		}
		return true; // 返回成功
	}
};

#define C32MacroJoin(x, y) x##y

#define C32MacroToString1(x) #x

#define C32MacroToString(x) C32MacroToString1(x)

#define C32MacroRunTime(x, y) C32MacroJoin(x, y)

#define C32MacroLine C32MacroJoin(":", C32MacroToString(__LINE__))
 
#define C32MarcroRunTimeFunction C32MacroRunTime(__FILE__, __FUNCTION__)

#define C32MarcroRunTimeName() C32MacroRunTime(C32MarcroRunTimeFunction, __FILE__)


#define C32MeasureTime()\
	const C32RunTime  C32MacroRunTime(RunTime, __COUNTER__)(C32MacroRunTime(__FUNCTION__, C32MacroLine))
	

//以下是逻辑处理帮助函数
C32String C32TimeFormatSystemTimeString(ULONGLONG SystemTime);

C32String C32TimeGetSystemTimeString();

C32String C32TimeGetSystemTimeStringFormatPath();

ULONGLONG C32TimeStringToSystemTime(CString SystemTimeString);

CString C32GetSelfFolder();

CString C32GetCurrentFolder();

/*
	返回最大字符为2048,超过2048返回""
*/
CString C32GetEnvironmentVariable(LPCWSTR Name);

#define STRING_CONATIN_NO 0
#define STRING_CONATIN_LEFT 1
#define STRING_CONATIN_RIGHT 2
#define STRING_CONTAIN_LEFT_RIGHT 3

#define C32FINDE_STRING_CONATIN_NO 0
#define C32FINDE_STRING_CONATIN_LEFT 1
#define C32FINDE_STRING_CONATIN_RIGHT 2
#define C32FINDE_STRING_CONTAIN_LEFT_RIGHT 3


int C32StringFindString(CStringA& String, LPCSTR Start, LPCSTR End, CStringA& FindString,  int ContainPart = STRING_CONATIN_NO, int iStart = 0);

int C32StringFindStringLeft(CStringA& String, LPCSTR TagString, CStringA& FindString, int iStart = 0);

int C32StringFindStringRight(CStringA& String, LPCSTR TagString, CStringA& FindString, int iStart = 0);

int C32StringFindString(CString& String, LPCWSTR Start, LPCWSTR End, CString& FindString,  int ContainPart = STRING_CONATIN_NO, int iStart = 0);

int C32StringFindStringLeft(CString& String, LPCWSTR TagString, CString& FindString, int iStart = 0);

int C32StringFindStringRight(CString& String, LPCWSTR TagString, CString& FindString, int iStart = 0);

CString C32StringReserveSplitString(LPCWSTR String, WCHAR Ch);

BOOL C32StringReserveSplitString(LPCWSTR String, WCHAR Ch, CString& LeftString, CString& RightString);



/*
	配置文件经常使用
	分割字符串到数组.
	Line = \r\n
*/
ULONG C32StringSplitLineToList(CString& String, CAtlArray<CString>& List);

CString C32TruncatString(CString String, int iIndex,BOOL IsEllipsis = TRUE);

int  C32DumpCStringToBuffer(CString& String, PCHAR Buffer, int Length);


//转换成数字
int  C32StringToInt(CString NumberString, int Radix = 10);
int  C32StringToInt(CStringA NumberString, int Radix = 10);

__int64  C32StringToInt64(CString NumberString, int Radix = 10);
__int64  C32StringToInt64(CStringA NumberString, int Radix = 10);


/*
	16进制转换
*/
ULONG_PTR  C32StringToPtr(CStringA NumberString);

CStringA C32PathGetFileName(LPCSTR Path);

CString C32PathGetFileName(LPCWSTR Path);

CString C32PathGetFloder(LPCWSTR Path);

VOID C32DebugStringArray(CAtlArray<CString>& Array);

/*
	可运行的应用程序其路径小于MAX_PATH
*/
BOOL   C32IsValidProcessPath(LPCWSTR Path);

BOOL   C32IsPe(LPCWSTR Path);

BOOL   C32PathFileIsExist(LPCWSTR Path);

BOOL   C32PathFileIsExist(LPCWSTR Path, CString& ErrorString);

BOOL   C32PathDirectoryIsExist(LPCWSTR Path, CString& ErrorString);

BOOL   C32PathCreateDirectory(LPCWSTR Path);

BOOL   C32PathCreateDirectory(LPCWSTR Path, CString& ErrorString);


/*
	文件操作
*/
C32Buffer* C32GetFileContent(LPCWSTR FilePath, CString& ErrorString);

CStringA C32FileGetContentStringA(LPCWSTR FilePath, CString& ErrorString);

DWORD C32ReadFile(LPCWSTR FilePath, LONGLONG Offset, ULONG Length, PVOID Buffer,CString& ErrorString);


//系统对话框
BOOL C32LocateFile(LPCWSTR Path);

BOOL C32SelectFolderPath(LPWSTR szTitle,CString& Path , HWND OwnerWindow= NULL);

BOOL C32RunExecute(LPCWSTR Path, LPCWSTR Parameter, LPCWSTR Directory = NULL);

//找到数组中的值然后移动到数组开始，适用于列视图动态显示或隐藏列
BOOL C32ArrayMoveValueToStart(int Array[], int Count, int Target);

//找到数组中的值然后移动到数组末尾，适用于列视图动态显示或隐藏列
BOOL C32ArrayMoveValueToEnd(int Array[], int Count, int Target);

//将数组序号 index移动到数组开始
BOOL C32ArrayMoveIndexToStart(int Array[], int Count, int Index);

//将数组序号 index移动到数组结尾
BOOL C32ArrayMoveIndexToEnd(int Array[], int Count, int Index);

VOID C32ArrayMoveValueTest();


int C32ArrayGetIndex(int Array[], int Count, int Value);

typedef enum _EC32FileWriteType
{
	EC32FileWriteAppend, //打开文件追加写
	EC32FileWriteOverWrite,    //新建或打开文件重新写

}EC32FileWriteType, *PEC32FileWriteType;



BOOL C32FileWrite(LPCWSTR Path, LPVOID Buffer, ULONG Length, EC32FileWriteType Type);

BOOL C32FileAppend(LPCWSTR Path, LPVOID Buffer, ULONG Length);

BOOL C32FileOverWrite(LPCWSTR Path, LPVOID Buffer, ULONG Length);

//释放资源文件
BOOL C32FreeResourceToDisk(int RsrcId, LPCWSTR Type, LPCWSTR FilePath);

FARPROC C32GetDllFunction(LPCWSTR DllName, LPCSTR FunctionName);


/*
	将错误码转换成可读的字符串
*/
CString C32GetLastErrorString();


class C32Environment
{

public:
	VOID Initialize(HINSTANCE hInstance = NULL)
	{
		WCHAR Str[MAX_PATH];

		GetModuleFileName(NULL, Str, MAX_PATH);
		m_SelfPath = Str;

		C32StringReserveSplitString(m_SelfPath, L'\\',m_SelfFolder, m_SelfName);
		m_SelfFolder += L"\\";

	}

public:
    VOID Debug()
	{
		ZxDebugEx((__FUNCTION__"():: SelfFolder: %ws \n", m_SelfFolder.GetString()));
		ZxDebugEx((__FUNCTION__"():: SelfName: %ws \n", m_SelfName.GetString()));
	}

public:

	CString  m_SelfPath;   //自身实例的完整路径
	CString  m_SelfFolder; //自身实例运行目录。以 \\结尾
	CString  m_SelfName;   //自身实例名
};


extern C32Environment theEnv;

// 只允许运行一个应用程序
extern C32SynEvent       theOnlyRun;
/*
	初始化C32Helper.cpp中的全局变量
*/
VOID	InitC32Helper();