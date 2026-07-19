#pragma once


class CObject
{
public:

	// Object model (types, destruction, allocation)
	//virtual CRuntimeClass* GetRuntimeClass() const;
	virtual ~CObject() = 0;  // virtual destructors are necessary

	// Diagnostic allocations
	void* PASCAL operator new(size_t nSize);
	void* PASCAL operator new(size_t, void* p);
	void PASCAL operator delete(void* p);
	void PASCAL operator delete(void* p, void* pPlace);

#if defined(_DEBUG) //&& !defined(_AFX_NO_DEBUG_CRT)
	// for file name/line number tracking using DEBUG_NEW
	//void* PASCAL operator new(size_t nSize, LPCSTR lpszFileName, int nLine);
	//void PASCAL operator delete(void *p, LPCSTR lpszFileName, int nLine);
#endif

	// Disable the copy constructor and assignment by default so you will get
	//   compiler errors instead of unexpected behaviour if you pass objects
	//   by value or assign objects.
protected:
	CObject();
private:
	CObject(const CObject& objectSrc);              // no implementation
	void operator=(const CObject& objectSrc);       // no implementation

	// Attributes
public:
	//BOOL IsSerializable() const;
	//BOOL IsKindOf(const CRuntimeClass* pClass) const;

	// Overridables
	//virtual void Serialize(CArchive& ar);

#ifdef _DEBUG
	CString m_strName;
#endif
};

class CSyncObject : public CObject
{
public:
	CSyncObject()
	{

	}
	
	~CSyncObject()
	{

	}

	explicit CSyncObject(LPCTSTR pstrName);

	operator HANDLE() const;
	HANDLE  m_hObject;
	// Operations
	virtual BOOL Lock() = 0;

	virtual BOOL Unlock() = 0;
	

	friend class CSingleLock;
	
};

class CSingleLock
{
	// Constructors
public:
	explicit CSingleLock(CSyncObject* pObject, BOOL bInitialLock = FALSE);

	// Operations
public:
	BOOL Lock(DWORD dwTimeOut = INFINITE);
	BOOL Unlock();
	BOOL Unlock(LONG lCount, LPLONG lPrevCount = NULL);
	BOOL IsLocked();

	// Implementation
public:
	~CSingleLock();

protected:
	CSyncObject* m_pObject;
	HANDLE  m_hObject;
	BOOL    m_bAcquired;
};


// COM Sync Classes
class CCriticalSection : public CSyncObject
{
public:
	CCriticalSection() 
	{
		memset(&m_sect, 0, sizeof(CRITICAL_SECTION));
		InitializeCriticalSectionAndSpinCount(&m_sect, 0);
	}

	~CCriticalSection()
	{
		::DeleteCriticalSection(&m_sect);
	}

	BOOL Lock() 
	{
		EnterCriticalSection(&m_sect);
		return TRUE;
	}

	BOOL Unlock() 
	{
		LeaveCriticalSection(&m_sect);
		return TRUE;
	}
	
public:
	operator CRITICAL_SECTION*()
	{
		return (CRITICAL_SECTION*) &m_sect; 
	}


private:
	CRITICAL_SECTION m_sect;
};

class CAutoLock
{
	// Constructors
public:
	explicit CAutoLock(CSyncObject* pObject);

	// Operations
public:
	BOOL Lock();
	
	BOOL Unlock();

	BOOL IsLocked();

	// Implementation
public:
	~CAutoLock();

protected:
	CSyncObject* m_pObject;
	
};

// CEvent
class CEvent : public CSyncObject
{
private:
	using CSyncObject::Unlock;

	// Constructor
public:
	/* explicit */ CEvent(BOOL bInitiallyOwn = FALSE, BOOL bManualReset = FALSE,
		LPCTSTR lpszNAme = NULL, LPSECURITY_ATTRIBUTES lpsaAttribute = NULL);

	// Operations
public:
	BOOL SetEvent();
	BOOL PulseEvent();
	BOOL ResetEvent();
	BOOL Unlock();

	// Implementation
public:
	virtual ~CEvent();
};

// 读写锁
class CReadWriteLock
{
public:
	CReadWriteLock()
	{
		InitializeSRWLock(&m_lock);
	}

	void LockRead()
	{
		AcquireSRWLockShared(&m_lock);
	}

	void UnlockRead()
	{
		ReleaseSRWLockShared(&m_lock);
	}

	void LockWrite()
	{
		AcquireSRWLockExclusive(&m_lock);
	}

	void UnlockWrite()
	{
		ReleaseSRWLockExclusive(&m_lock);
	}

private:
	SRWLOCK m_lock;
};

//引用计数锁
class CLockedCounter
{
public:
	CLockedCounter()
	{
		m_Count = 0ULL;
		
	}

	~CLockedCounter()
	{
	}

public:
	VOID Init(ULONGLONG Value)
	{
		m_Count = Value;
	}

	VOID Init(ULONG Value)
	{
		m_Count = (ULONGLONG)Value;
	}

	VOID Increment(ULONG  Value = 1)
	{
		CAutoLock Locker(&m_Locker);
		m_Count += (ULONGLONG)Value;
	}

	VOID Decrement(ULONG  Value = 1)
	{
		CAutoLock Locker(&m_Locker);
		m_Count -= (ULONGLONG)Value;
	}


	LONG GetLong()
	{
		CAutoLock Locker(&m_Locker);
		return (LONG)m_Count;
	}

public:
	operator DWORD()
	{
		return (DWORD)m_Count;
	}

	operator int()
	{
		return (int)m_Count;
	}

public:
	ULONGLONG  m_Count;
	CCriticalSection m_Locker;
};