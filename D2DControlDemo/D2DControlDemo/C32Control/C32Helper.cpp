#include "stdafx.h"
#include "C32Helper.h"
#include <atlfile.h>
#include <Shlobj.h>
#include <ShellAPI.h>

//当前实例的运行目录
C32Environment theEnv;
C32SynEvent       theOnlyRun;

ShellExecuteDecl  ShellExecuteProcAddress;


PVOID  C32AllocateBuffer(int Length)
{
	PVOID p= NULL;
	int n = ((Length + 8)&-8);
	p = (PVOID)new UCHAR[n];
	if (p)
	{
		RtlZeroMemory(p, n);
	}
	return p;
}

VOID   C32FreeBuffer(PVOID Buffer)
{
	delete Buffer;
}

LPWSTR C32AllocateString(int Length)
{
	return (LPWSTR)C32AllocateBuffer(Length);
}


C32String C32TimeFormatSystemTimeString(ULONGLONG SystemTime)
{
	int Count = 0;
	_SYSTEMTIME st ={};
	C32String Time;
	Time.AssignStringW(64);

	FileTimeToSystemTime((FILETIME*)&SystemTime, &st);
	SystemTimeToTzSpecificLocalTime(NULL, &st, &st);
	Time.m_Count = swprintf_s(Time, 64,L"%4d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	return Time;   
}

C32String C32TimeGetSystemTimeString()
{
	C32String Time;
	_SYSTEMTIME St ={};
	GetSystemTime(&St);
	SystemTimeToTzSpecificLocalTime(NULL, &St, &St);

	Time.FormatString(L"%4d-%02d-%02d %02d:%02d:%02d",St.wYear, St.wMonth, St.wDay,  St.wHour, St.wMinute, St.wSecond);

	return Time;   
}

C32String C32TimeGetSystemTimeStringFormatPath()
{
	C32String Time;
	_SYSTEMTIME St ={};
	GetSystemTime(&St);
	SystemTimeToTzSpecificLocalTime(NULL, &St, &St);

	Time.FormatString(L"%4d_%02d_%02d_%02d_%02d_%02d",St.wYear, St.wMonth, St.wDay,  St.wHour, St.wMinute, St.wSecond);

	return Time;

}

ULONGLONG C32TimeStringToSystemTime(CString SystemTimeString)
{
	int xx = 0x12345678;
	SYSTEMTIME St = {};
	FILETIME   Ft = {0,0};

	DWORD wYear;
	DWORD wMonth;
	DWORD wDay;
	DWORD wHour;
	DWORD wMinute;
	DWORD wSecond;
	DWORD wMilliseconds;
	
	int Ret = _snwscanf_s(SystemTimeString, SystemTimeString.GetLength(), L"%4d-%02d-%02d %02d:%02d:%02d:%03d", 
		&wYear,&wMonth, &wDay, &wHour, &wMinute, &wSecond, &wMilliseconds);


	if (Ret != -1)
	{
		St.wYear = (WORD)wYear;
		St.wMonth = (WORD)wMonth;
		St.wDay = (WORD)wDay;
		St.wHour = (WORD)wHour;
		St.wMinute = (WORD)wMinute;
		St.wSecond = (WORD)wSecond;
		St.wMilliseconds = (WORD)wMilliseconds;
		SystemTimeToFileTime(&St, &Ft);
	}
		
	 return *(ULONGLONG*)&Ft;
}

BOOL C32LocateFile(LPCWSTR Path)
{
	if (Path == NULL)
		return FALSE;

	C32String Parameters = L"/select,";
	Parameters+= Path;
	SHELLEXECUTEINFO Si={};
	Si.cbSize = sizeof(SHELLEXECUTEINFO);
	Si.lpFile = L"Explorer";
	Si.lpParameters = Parameters;
	Si.lpVerb = L"Open";
	Si.nShow = SW_SHOWDEFAULT;
	Si.lpDirectory = NULL;

	return ShellExecuteEx(&Si);
}

BOOL C32SelectFolderPath(LPWSTR szTitle,CString& Path , HWND OwnerWindow)
{
  BOOL         retCode;
 
  BROWSEINFO   nBI;
  LPITEMIDLIST pidl;
  WCHAR szPath[MAX_PATH]={};
  memset(&nBI,0,sizeof(nBI));
  nBI.hwndOwner      = OwnerWindow;
  nBI.pszDisplayName = szPath;
  nBI.lpszTitle      = szTitle;
  nBI.ulFlags        = BIF_RETURNONLYFSDIRS | BIF_BROWSEFORCOMPUTER |BIF_EDITBOX|BIF_NEWDIALOGSTYLE;
  if (NULL == (pidl=SHBrowseForFolder(&nBI))) return FALSE;
  retCode = SHGetPathFromIDList(pidl,szPath);
  if(retCode == TRUE)
  {
	  Path = szPath;
  }
  return retCode;
}

BOOL C32RunExecute(LPCWSTR Path, LPCWSTR Parameter, LPCWSTR Directory)
{
	if (Path == NULL)
		return FALSE;

	SHELLEXECUTEINFO Si={};
	Si.cbSize = sizeof(SHELLEXECUTEINFO);
	Si.lpFile = Path;
	Si.lpParameters = Parameter;
	Si.lpDirectory = Directory;
	Si.nShow = SW_SHOWDEFAULT;

	return ShellExecuteEx(&Si);
}
 
CString C32GetSelfFolder()
{
	CString Directory;
	
	WCHAR   str[MAX_PATH];
	/// 本实例文件

	GetModuleFileName(NULL, str, _countof(str));

	Directory = str;

	return Directory;
}


CString C32GetCurrentFolder()
{
	CString Directory;
	WCHAR Buffer[260]={};
	DWORD Length = GetCurrentDirectory(260, Buffer);
	if (Length)
	{
		Directory = Buffer;
	}

	return Directory;
}

CString C32GetEnvironmentVariable(LPCWSTR Name)
{
	CString Var;
	if (Name == NULL)
		return Var;

	C32Buffer Buffer(4096);
	DWORD Len = GetEnvironmentVariable(Name, (LPWSTR)Buffer.GetBuffer(), 2048);
	if (Len < 2048)
	{
		Var = (LPWSTR)Buffer.GetBuffer();
	}
	return Var;
}

int C32StringFindString(CStringA& String, LPCSTR Start, LPCSTR End, CStringA& FindString,  int ContainPart, int iStart)
{
	if (iStart < 0)
		return iStart;

	int FileIndex = String.Find(Start, iStart);
	if (FileIndex != C32StringErrorIndex)
	{
		int Index = FileIndex + (int)strlen(Start);
		int FileIndex1 = String.Find(End,Index);
		if (FileIndex1 != C32StringErrorIndex)
		{
			if (ContainPart == STRING_CONATIN_LEFT)
			{
				FindString = String.Mid(FileIndex, FileIndex1-FileIndex);
			}else if (ContainPart == STRING_CONATIN_RIGHT)
			{
				FileIndex1 += (int)strlen(End);
				FindString = String.Mid(Index, FileIndex1-FileIndex);
			}else if (ContainPart == STRING_CONTAIN_LEFT_RIGHT)
			{
				FileIndex1 += (int)strlen(End);
				FindString = String.Mid(FileIndex, FileIndex1-FileIndex);
			}else
			{
				FindString = String.Mid(Index, FileIndex1-Index);
			}

			return FileIndex1;
		}
	}

	return -1;
}

int C32StringFindStringLeft(CStringA& String, LPCSTR TagString, CStringA& FindString, int iStart)
{
	int FileIndex = String.Find(TagString, iStart);
	if (FileIndex != C32StringErrorIndex)
	{
		FindString = String.Mid(iStart, FileIndex - iStart);
	}

	return FileIndex;
}

int C32StringFindStringRight(CStringA& String, LPCSTR TagString, CStringA& FindString, int iStart)
{
	int FileIndex = String.Find(TagString, iStart);
	if (FileIndex != C32StringErrorIndex)
	{
		int Size = String.GetLength() - FileIndex - (int)strlen(TagString);
		FindString = String.Right(Size);
	}

	return FileIndex;
}

int C32StringFindString(CString& String, LPCWSTR Start, LPCWSTR End, CString& FindString,  int ContainPart, int iStart)
{
	if (iStart < 0)
		return iStart;

	int FileIndex = String.Find(Start, iStart);
	if (FileIndex != C32StringErrorIndex)
	{
		int Index = FileIndex + (int)wcslen(Start);
		int FileIndex1 = String.Find(End,Index);
		if (FileIndex1 != C32StringErrorIndex)
		{
			if (ContainPart == STRING_CONATIN_LEFT)
			{
				FindString = String.Mid(FileIndex, FileIndex1-FileIndex);
			}else if (ContainPart == STRING_CONATIN_RIGHT)
			{
				FileIndex1 += (int)wcslen(End);
				FindString = String.Mid(Index, FileIndex1-FileIndex);
			}else if (ContainPart == STRING_CONTAIN_LEFT_RIGHT)
			{
				FileIndex1 += (int)wcslen(End);
				FindString = String.Mid(FileIndex, FileIndex1-FileIndex);
			}else
			{
				FindString = String.Mid(Index, FileIndex1-Index);
			}

			return FileIndex1;
		}
	}

	return -1;
}

int C32StringFindStringLeft(CString& String, LPCWSTR TagString, CString& FindString, int iStart)
{
	int FileIndex = String.Find(TagString, iStart);
	if (FileIndex != C32StringErrorIndex)
	{
		FindString = String.Left(FileIndex);
	}

	return FileIndex;
}

int C32StringFindStringRight(CString& String, LPCWSTR TagString, CString& FindString, int iStart)
{
	int FileIndex = String.Find(TagString, iStart);
	if (FileIndex != C32StringErrorIndex)
	{
		int Size = String.GetLength() - FileIndex - (int)wcslen(TagString);
		FindString = String.Right(Size);
	}

	return FileIndex;
}

CString C32StringReserveSplitString(LPCWSTR String, WCHAR Ch)
{
	CString Split;
	if (String == NULL)
		return Split;
	CString Str = String;

	int iFind = Str.ReverseFind(Ch);
	if (!C32StringIsErrorIndex(iFind))
	{
		Split = Str.Mid(++iFind);
	}

	return Split;
}

BOOL C32StringReserveSplitString(LPCWSTR String, WCHAR Ch, CString& LeftString, CString& RightString)
{
	if (String == NULL)
		return FALSE;
	CString Str = String;

	int iFind = Str.ReverseFind(Ch);
	if (!C32StringIsErrorIndex(iFind))
	{
		LeftString = Str.Left(iFind);
		RightString = Str.Mid(iFind + 1);

		return TRUE;
	}

	return FALSE;
}


ULONG C32StringSplitLineToList(CString& String, CAtlArray<CString>& List)
{
	ULONG Count = 0;
	int iStart = 0, Index = 0;
	do 
	{
		Index = String.Find(L"\r\n", iStart);
		if (Index != C32StringErrorIndex)
		{
			CString Line = String.Mid(iStart, Index - iStart);
			iStart = Index + (int)wcslen(L"\r\n");
			Line.Trim();
			if (!Line.IsEmpty())
			{
				List.Add(Line);
				Count++;
			}
		}

	} while (Index != -1);

	return Count;
}

CString C32TruncatString(CString String, int iIndex,BOOL IsEllipsis)
{
	String.Delete(iIndex);
	if (IsEllipsis)
		String+=L"...";

	return String;
}

int  C32StringToInt(CString NumberString, int Radix)
{
	int Number;
	if (NumberString.IsEmpty())
		return 0;
	Number= (int)wcstol(NumberString, NULL, Radix);
	return Number;
}

int  C32StringToInt(CStringA NumberString, int Radix)
{
	int Number;
	if (NumberString.IsEmpty())
		return 0;
	Number= (int)strtol(NumberString, NULL, Radix);
	return Number;
}

__int64  C32StringToInt64(CString NumberString, int Radix)
{
	int Number;
	if (NumberString.IsEmpty())
		return 0;
	Number= (int)_wcstoi64(NumberString, NULL, Radix);
	return Number;
}

__int64  C32StringToInt64(CStringA NumberString, int Radix)
{
	int Number;
	if (NumberString.IsEmpty())
		return 0;
	Number= (int)_strtoi64(NumberString, NULL, Radix);
	return Number;
}


ULONG_PTR  C32StringToPtr(CStringA NumberString)
{
	ULONG_PTR Number;
	if (NumberString.IsEmpty())
		return 0;
#if WIN64
	Number = _strtoui64(NumberString, NULL, 16);
#endif
	Number= strtoul(NumberString, NULL, 16);
	return Number;
}

VOID C32DebugStringArray(CAtlArray<CString>& Array)
{
	for(size_t i = 0; i < Array.GetCount(); i++)
	{
		ZxDebugEx(("%ws \n", Array.GetAt(i)));

	}
}

CStringA C32PathGetFileName(LPCSTR Path)
{
	CStringA Split;
	if (Path == NULL)
		return Split;
	CStringA Str = Path;

	int iFind = Str.ReverseFind('\\');
	if (!C32StringIsErrorIndex(iFind))
	{
		Split = Str.Mid(++iFind);
	}

	return Split;
}

CString C32PathGetFileName(LPCWSTR Path)
{
	CString Split;
	if (Path == NULL)
		return Split;
	CString Str = Path;

	int iFind = Str.ReverseFind(L'\\');
	if (!C32StringIsErrorIndex(iFind))
	{
		Split = Str.Mid(++iFind);
	}

	return Split;
}


CString C32PathGetFloder(LPCWSTR Path)
{
	CString Split;
	if (Path == NULL)
		return Split;
	CString Str = Path;

	int iFind = Str.ReverseFind(L'\\');
	if (!C32StringIsErrorIndex(iFind))
	{
		Split = Str.Left(iFind);
	}

	return Split;


}

BOOL   C32IsValidProcessPath(LPCWSTR Path)
{
	if (Path == NULL) return FALSE;
	int Count = (int)wcslen(Path);
	if (Count > MAX_PATH)
	  return FALSE;
	else
	  return TRUE;
}

BOOL   C32PathFileIsExist(LPCWSTR Path)
{
	C32File File;
	HANDLE hFile = NULL;

	hFile = File.Open(Path);
	if(hFile == NULL && File.m_ErrorCode !=5)
		return FALSE;

	return TRUE;
}

BOOL   C32PathFileIsExist(LPCWSTR Path, CString& ErrorString)
{
	C32File File;
	HANDLE hFile = NULL;
	if (Path == NULL)
	{
		ErrorString = L"文件路径为空";
		return FALSE;
	}
	hFile = File.Open(Path);
	if(hFile == NULL && File.m_ErrorCode !=5)
	{
		ErrorString.Format(L"文件不存在 : %ws \n", Path);
		return FALSE;
	}

	return TRUE;
}

BOOL   C32PathDirectoryIsExist(LPCWSTR Path, CString& ErrorString)
{
	assert(0);

	return FALSE;
}

BOOL   C32PathCreateDirectory(LPCWSTR Path, CString& ErrorString)
{
	if (Path == NULL)
	{
		ErrorString += L"目录不能为空";
		return FALSE;
	}
	BOOL Ret = CreateDirectory(Path, NULL);
	DWORD Code = GetLastError();
	if (Ret || Code == ERROR_ALREADY_EXISTS)
		return TRUE;

	ErrorString.AppendFormat(L"创建目录失败:错误码: %d \n", Code);

	return FALSE;

}

BOOL   C32PathCreateDirectory(LPCWSTR Path)
{
	BOOL Ret = CreateDirectory(Path, NULL);
	DWORD Code = GetLastError();
	if (Ret || Code == ERROR_ALREADY_EXISTS)
		return TRUE;

	return FALSE;
}

BOOL   C32IsPe(LPCWSTR Path)
{
	if (Path == NULL)
		return FALSE;

	C32File File;
	CHAR Pe[3]={};
	if (File.Read((PVOID)Pe, 2))
	{
		if (strcmp(Pe, "MZ") == 0)
			return TRUE;
	}

	return FALSE;
}


C32Buffer* C32GetFileContent(LPCWSTR FilePath, CString& ErrorString)
{
	C32File File;
	DWORD   Size;
	if (File.Open(FilePath))
	{
		Size = (DWORD)File.GetSize();
		if (Size)
		{
			C32Buffer* Buffer = new C32Buffer(Size);
			if (Buffer)
			{
				DWORD dwRead = File.Read(*Buffer, Size);
				if (dwRead != Size)
				{
					ErrorString.AppendFormat(L" 读取文件失败: %ws 错误代码: %d\n", FilePath, File.m_ErrorCode);
					delete Buffer;
					Buffer = NULL;
				}	
				return Buffer;
			}
		}else{
			ErrorString.AppendFormat(L" 文件中无内容:\n", FilePath);
		}
	}else
	{
		ErrorString.AppendFormat(L"打开文件失败: %ws 错误代码: %d\n", FilePath, File.m_ErrorCode);
	}


	return NULL;
}

CStringA C32FileGetContentStringA(LPCWSTR FilePath, CString& ErrorString)
{
	CStringA FileStringA;
	
	C32Buffer* Buffer = C32GetFileContent(FilePath, ErrorString);
	if (Buffer == NULL)
		return FileStringA;

	FileStringA.Append(*Buffer, Buffer->GetLength());

	delete Buffer;

	return FileStringA;
}

DWORD C32ReadFile(LPCWSTR FilePath, LONGLONG Offset, ULONG Length, PVOID Buffer,CString& ErrorString)
{
	CAtlFile File;

	HRESULT Hst = File.Create(FilePath, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
	if (Hst != S_OK)
	{
		ErrorString.Format(L"不能打开%ws文件", FilePath);
		return 0;
	}

	Hst = File.Seek(Offset, FILE_BEGIN);
	if (Hst != S_OK)
	{
		ErrorString.Format(L"不能读取%ws文件", FilePath);
		return 0;
	}
	DWORD nBytesRead = 0;
	Hst = File.Read(Buffer, Length, nBytesRead);
	if (Hst != S_OK)
	   return 0;

	return nBytesRead;
}





BOOL C32ArrayMoveValueToStart(int Array[], int Count, int Target)
{
	BOOL Move = FALSE;
	int iCurrent = -1;
	for (int i = Count - 1; i >= 0; i--)
	{
		if (Array[i] == Target)
		{
			iCurrent = i;
			Move = TRUE;
		}else if (iCurrent != -1)
		{
			Array[iCurrent] = Array[i];
			Array[i] = Target;
			iCurrent = i;
		}
	}

	return Move;
}

BOOL C32ArrayMoveValueToEnd(int Array[], int Count, int Target)
{
	BOOL Move = FALSE;
	int iCurrent = -1;
	for (int i = 0; i < Count; i++)
	{
		if (Array[i] == Target)
		{
			iCurrent = i;
			Move = TRUE;
		}else if (iCurrent != -1)
		{
			Array[iCurrent] = Array[i];
			Array[i] = Target;
			iCurrent = i;
		}
	}

	return Move;
}

BOOL C32ArrayMoveIndexToStart(int Array[], int Count, int Index)
{
	BOOL Move = FALSE;
	int iCurrent = -1;
	
	if (Index >= Count)
		return FALSE;

	for (int i = Count - 1; i >= 0; i--)
	{
		if (i == Index)
		{
			iCurrent = i;
			Move = TRUE;
		}else if (iCurrent != -1)
		{
			int Target = Array[iCurrent];
			Array[iCurrent] = Array[i];
			Array[i] = Target;
			iCurrent = i;
		}
	}

	return Move;
}

BOOL C32ArrayMoveIndexToEnd(int Array[], int Count, int Index)
{
	BOOL Move = FALSE;
	int iCurrent = -1;

	if (Index >= Count)
		return FALSE;

	for (int i = 0; i < Count; i++)
	{
		if (i == Index)
		{
			iCurrent = i;
			Move = TRUE;
		}else if (iCurrent != -1)
		{
			int Target = Array[iCurrent];
			Array[iCurrent] = Array[i];
			Array[i] = Target;
			iCurrent = i;
		}
	}

	return Move;
}

VOID C32ArrayMoveValueTest()
{
	int Array1[5]={2,3,4,1,5};
	C32ArrayMoveValueToStart(Array1, 5, 1);

	int Array2[5]={2,3,4,5,1};
	C32ArrayMoveValueToStart(Array2, 5, 1);

	int Array3[5]={1,2,5,3,4};
	C32ArrayMoveValueToEnd(Array3, 5, 5);

	int Array4[5]={2,3,4,1,5};
	C32ArrayMoveIndexToStart(Array4, 5, 3);

	int Array5[5]={1,2,5,3,4};
	C32ArrayMoveIndexToEnd(Array5, 5, 2);


	int xx = 0;
}

int C32ArrayGetIndex(int Array[], int Count, int Value)
{
	int Index = -1;

	for(int i = 0; i < Count; i++)
	{
		if (Array[i] == Value)
		{
			Index = i;
			break;
		}
	}

	return Index;
}


BOOL C32FileWrite(LPCWSTR Path, LPVOID Buffer, ULONG Length, EC32FileWriteType Type)
{
	C32File File;

	if (Type == EC32FileWriteAppend)
	{
		if (!File.Open(Path, OPEN_ALWAYS))
			return FALSE;
		File.SetFileOffset((ULONG)File.GetSize());
	}else
	{
		if (!File.Open(Path, CREATE_ALWAYS))
			return FALSE;
	}
	
	ULONG Count = File.Write(Buffer, Length);

	if (Count == Length)
		return TRUE;

	return FALSE;
}

BOOL C32FileAppend(LPCWSTR Path, LPVOID Buffer, ULONG Length)
{
	return C32FileWrite(Path, Buffer, Length, EC32FileWriteAppend);
}

BOOL C32FileOverWrite(LPCWSTR Path, LPVOID Buffer, ULONG Length)
{
	return C32FileWrite(Path, Buffer, Length, EC32FileWriteOverWrite);
}

BOOL C32FreeResourceToDisk(int RsrcId, LPCWSTR Type, LPCWSTR FilePath)
{
	C32ResorceFile File;
	DWORD Size;

	Size = File.FreeResourceDisk(RsrcId, Type, FilePath);

	return (Size != 0);
}

FARPROC C32GetDllFunction(LPCWSTR DllName, LPCSTR FunctionName)
{
	HMODULE  hModule;
	FARPROC Address = NULL;

	hModule = LoadLibrary(DllName);
	if (hModule)
	{
		Address = GetProcAddress(hModule, FunctionName);
		FreeLibrary(hModule);

	}
	return Address;
}

CString C32GetLastErrorString()
{
	DWORD ErrorCode;
	LPWSTR Buffer = NULL;
	ErrorCode = GetLastError();
	
	DWORD result = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, ErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&Buffer, 0, NULL);

	if (result == 0) {
		return L"Failed to get error message";
	}

	CString Message;
	Message.Append(Buffer);
	LocalFree(Buffer);

	return Message;
}


VOID	InitC32Helper()
{
	HMODULE hModule = LoadLibraryW(L"shell32.dll"); // 加载 shell32.dll 模块
	if(hModule)
	{
		ShellExecuteProcAddress = (ShellExecuteDecl)GetProcAddress(hModule, "ShellExecuteW");
	}
	theEnv.Initialize(NULL);
	
}