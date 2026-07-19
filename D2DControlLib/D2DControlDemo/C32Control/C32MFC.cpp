#include "stdafx.h"
#include "C32MFC.h"

// CObject
CObject::CObject()
{ }
CObject::~CObject()
{ }
//void CObject::Serialize(CArchive&)
//{ /* CObject does not serialize anything by default */ }
void* PASCAL CObject::operator new(size_t, void* p)
{ return p; }
//#ifndef _DEBUG
// _DEBUG versions in afxmem.cpp
void PASCAL CObject::operator delete(void* p)
{ ::operator delete(p); }
void PASCAL CObject::operator delete(void* p, void*)
{ ::operator delete(p); }
void* PASCAL CObject::operator new(size_t nSize)
{ return ::operator new(nSize); }
//#endif
// Basic synchronization object

CSyncObject::CSyncObject(LPCTSTR pstrName)
{
	(pstrName);   // unused in release builds

#ifdef _DEBUG
	m_strName = pstrName;
#endif
}


/////////////////////////////////////////////////////////////////////////////
// CSingleLock

CSingleLock::CSingleLock(CSyncObject* pObject, BOOL bInitialLock)
{
	m_pObject = pObject;
	m_hObject = pObject->m_hObject;
	m_bAcquired = FALSE;

	if (bInitialLock)
		Lock();
}

CSingleLock::~CSingleLock()
{
	Unlock();
}

BOOL CSingleLock::Lock(DWORD dwTimeOut /* = INFINITE */)
{
	m_bAcquired = m_pObject->Lock();
	return m_bAcquired;
}

BOOL CSingleLock::Unlock()
{
	ASSERT(m_pObject != NULL);
	if (m_bAcquired)
		m_bAcquired = !m_pObject->Unlock();

	// successfully unlocking means it isn't acquired
	return !m_bAcquired;
}

BOOL CSingleLock::Unlock(LONG lCount, LPLONG lpPrevCount /* = NULL */)
{
	ASSERT(m_pObject != NULL);
	if (m_bAcquired)
		m_bAcquired = !m_pObject->Unlock();

	// successfully unlocking means it isn't acquired
	return !m_bAcquired;
}



CAutoLock::CAutoLock(CSyncObject* pObject)
{
	m_pObject = pObject;
	
	Lock();
}

BOOL CAutoLock::Lock()
{
	return m_pObject->Lock();
}

BOOL CAutoLock::Unlock()
{
	return m_pObject->Unlock();
}

CAutoLock::~CAutoLock()
{
	m_pObject->Unlock();
}

CEvent::CEvent(BOOL bInitiallyOwn, BOOL bManualReset, LPCTSTR pstrName,
	LPSECURITY_ATTRIBUTES lpsaAttribute)
	: CSyncObject(pstrName)
{
	m_hObject = ::CreateEvent(lpsaAttribute, bManualReset,
		bInitiallyOwn, pstrName);
}

CEvent::~CEvent()
{
}

BOOL CEvent::Unlock()
{
	return TRUE;
}


