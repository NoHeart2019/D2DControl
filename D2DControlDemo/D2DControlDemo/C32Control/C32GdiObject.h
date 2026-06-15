#pragma once

#define ASSERT assert
class C32GdiObject 
{
	
public:

	// Attributes
	HGDIOBJ m_hObject;                  // must be first data member
	inline operator HGDIOBJ() const
	{ 
		return this == NULL ? NULL : m_hObject; 
	}
	HGDIOBJ GetSafeHandle() const
	{ 
		return this == NULL ? NULL : m_hObject; 
	}

	BOOL Attach(HGDIOBJ hObject)
	{
		if (hObject == NULL)
		{
			return FALSE;
		}
		// remember early to avoid leak
		m_hObject = hObject;
		return TRUE;
	}

	HGDIOBJ Detach()
	{
		HGDIOBJ hObject = m_hObject;
		m_hObject = NULL;
		return hObject;
	}

	inline C32GdiObject() // must Create a derived class object
	{ 
		m_hObject = NULL; 
	}
	
	BOOL DeleteObject()
	{
		if (m_hObject == NULL)
			return FALSE;
		return ::DeleteObject(Detach());
	}

	inline int GetObject(int nCount, LPVOID lpObject) const
	{ 
		ASSERT(m_hObject != NULL); 
		return NULL;//::GetObject(m_hObject, nCount, lpObject); 
	}

	inline BOOL CreateStockObject(int nIndex)
	{ 
		return (m_hObject = ::GetStockObject(nIndex)) != NULL; 
	}
	inline BOOL UnrealizeObject()
	{ 
		ASSERT(m_hObject != NULL); 
		return ::UnrealizeObject(m_hObject);
	}
	inline UINT GetObjectType() const
	{ 
		return (UINT)::GetObjectType(m_hObject); 
	}
	
	inline BOOL operator==(const C32GdiObject& obj) const
	{ 
		return ((HGDIOBJ) obj) == m_hObject; 
	}
	inline BOOL operator!=(const C32GdiObject& obj) const
	{ 
		return ((HGDIOBJ) obj) != m_hObject; 
	}


	// Implementation
public:
	
	 ~C32GdiObject()
	{
		DeleteObject(); 
	}

};

template <class T>
class C32ObjectPtr 
{
public:
	C32ObjectPtr() 
	{

	}

	C32ObjectPtr(_Inout_opt_ T lp) : m_p(lp)
	{
	}

	~C32ObjectPtr()
	{
		Release();
	}

	T operator=(_Inout_opt_ T lp) 
	{
		if(*this!=lp)
		{
			m_p = lp;
		}
		return *this;
	}

	 operator T() const
	{
		return m_p;
	}
	//The assert on operator& usually indicates a bug.  If this is really
	//what is needed, however, take the address of the p member explicitly.
	T* operator&() throw()
	{
		
		return &m_p;
	}
	
	bool operator!() const
	{	
		return (m_p == NULL);
	}
	bool operator<(_In_opt_ T pT) const 
	{
		return m_p < pT;
	}


	bool operator==(_In_opt_ T pT) 
	{
		return m_p == pT;
	}

	void Release() 
	{
		T pTemp = m_p;
		if (pTemp)
		{
			DeleteObject((HGDIOBJ)pTemp);
		}
	}
	
	// Attach to an existing interface (does not AddRef)
	void Attach(_In_opt_ T p2) 
	{
		Release();
		m_p = p2;
	}

	// Detach the interface (does not Release)
	T* Detach() 
	{
		T* pt = m_p;
		m_p = NULL;
		return pt;
	}
	
	
public:

	T m_p;
};

class C32Brush : public C32GdiObject
{
public:
	// CBrush
	inline operator HBRUSH() const
	{
		return (HBRUSH)(this == NULL ? NULL : m_hObject); 
	} 
	inline C32Brush* PASCAL FromHandle(HBRUSH hBrush)
	{
		return NULL;//(CBrush*) CGdiObject::FromHandle(hBrush); 
	} 
	
	inline C32Brush()
	{

	} 
	inline ~C32Brush()
	{

	} 
	inline BOOL CreateSolidBrush(COLORREF crColor)
	{
		return Attach(::CreateSolidBrush(crColor)); 
	} 
	inline BOOL CreateHatchBrush(int nIndex, COLORREF crColor)
	{
		return Attach(::CreateHatchBrush(nIndex, crColor)); 
	} 
	inline BOOL CreateBrushIndirect(const LOGBRUSH* lpLogBrush)
	{
		return Attach(::CreateBrushIndirect(lpLogBrush)); 
	} 
	/*inline BOOL CreatePatternBrush(CBitmap* pBitmap)
	{
		return Attach(::CreatePatternBrush((HBITMAP)pBitmap->GetSafeHandle())); 
	} */
	inline BOOL CreateDIBPatternBrush(const void* lpPackedDIB, UINT nUsage)
	{
		return Attach(::CreateDIBPatternBrushPt(lpPackedDIB, nUsage)); 
	} 
	inline BOOL CreateSysColorBrush(int nIndex)
	{
		return Attach(::GetSysColorBrush(nIndex)); 
	} 
	inline int GetLogBrush(LOGBRUSH* pLogBrush)
	{
		ASSERT(m_hObject != NULL);
		return ::GetObject(m_hObject, sizeof(LOGBRUSH), pLogBrush); 
	} 

};

class C32Font:  public C32GdiObject
{
public:
	// CFont
	operator HFONT() const
	{ return (HFONT)(this == NULL ? NULL : m_hObject); }
	/*CFont* PASCAL CFont::FromHandle(HFONT hFont)
	{ return (CFont*) CGdiObject::FromHandle(hFont); }*/
	C32Font()
	{ }
	~C32Font()
	{ }
	 BOOL CreateFontIndirect(const LOGFONT* lpLogFont)
	{ 
		return Attach(::CreateFontIndirect(lpLogFont)); 
	}
	 BOOL CreateFont(int nHeight, int nWidth, int nEscapement,
		int nOrientation, int nWeight, BYTE bItalic, BYTE bUnderline,
		BYTE cStrikeOut, BYTE nCharSet, BYTE nOutPrecision,
		BYTE nClipPrecision, BYTE nQuality, BYTE nPitchAndFamily,
		LPCTSTR lpszFacename)
	{ 
		return Attach(::CreateFont(nHeight, nWidth, nEscapement,
	nOrientation, nWeight, bItalic, bUnderline, cStrikeOut,
	nCharSet, nOutPrecision, nClipPrecision, nQuality,
	nPitchAndFamily, lpszFacename)); }
	int GetLogFont(LOGFONT* pLogFont)
	{ 
		ASSERT(m_hObject != NULL);
		return ::GetObject(m_hObject, sizeof(LOGFONT), pLogFont); 
	}




};

class C32SelectObject
{
public:
	C32SelectObject(HDC hDc, HGDIOBJ hGdiObject)
	{
		Init();
		if (hDc && hGdiObject)
		{
			m_Hdc = hDc;
			m_DbgObject = hGdiObject;
			m_GdiObject = SelectObject(m_Hdc, hGdiObject);
		}
		
	}

	C32SelectObject(HDC hDc, HBRUSH hBrush)
	{
		Init();
		if (hDc && hBrush)
		{
			m_Hdc = hDc;
			m_DbgObject = (HGDIOBJ)hBrush;
			m_GdiObject = SelectObject(m_Hdc, (HGDIOBJ)hBrush);
		}
		
	}


	
	~C32SelectObject()
	{
		if (m_Hdc && m_GdiObject)
		{
			HGDIOBJ hGdiObject = SelectObject(m_Hdc, m_GdiObject);
			ASSERT(hGdiObject == NULL || hGdiObject == m_DbgObject);
		}
	}

	VOID Init()
	{
		m_DbgObject = NULL;
		m_GdiObject = NULL;
		m_Hdc = NULL;
	}

	
	HGDIOBJ  m_DbgObject;
	HGDIOBJ  m_GdiObject;

	HDC m_Hdc;

};

class C32Dc
{
public:
	C32Dc()
	{
		Initialize();
	}

	C32Dc(HWND hwnd)
	{
		m_Hwnd = hwnd;
		m_Hdc = GetDC(hwnd);
	}

	~C32Dc()
	{
		if (m_Hdc)
		{
			ReleaseDC(m_Hwnd, m_Hdc);
			Initialize();
		}
	}

 
	VOID Initialize()
	{
		m_Hwnd = NULL;
		m_Hdc = NULL;
	}

public:
	operator HDC()const
	{
		return m_Hdc;
	}
public:
	HWND m_Hwnd;
	HDC m_Hdc;

};

#undef ASSERT