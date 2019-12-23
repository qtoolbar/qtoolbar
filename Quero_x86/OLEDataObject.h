/*
   Quero Toolbar
   http://www.quero.at/
   Copyright 2013 Viktor Krammer

   This file is part of Quero Toolbar.

   Quero Toolbar is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Quero Toolbar is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Quero Toolbar.  If not, see <http://www.gnu.org/licenses/>.
*/
// OLEDataObject.h: interface for the COLEDataObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLEDATAOBJECT_H__B19A8E7E_EBBC_41B9_A66E_8DF1C0C163BB__INCLUDED_)
#define AFX_OLEDATAOBJECT_H__B19A8E7E_EBBC_41B9_A66E_8DF1C0C163BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

class COLEDataObject : public IDataObject
{
public:
	//
    // IUnknown members
	//
    HRESULT __stdcall QueryInterface (REFIID iid, void ** ppvObject);
    ULONG   __stdcall AddRef (void);
    ULONG   __stdcall Release (void);
		
    //
	// IDataObject members
	//
    HRESULT __stdcall GetData				(FORMATETC *pFormatEtc,  STGMEDIUM *pMedium);
    HRESULT __stdcall GetDataHere			(FORMATETC *pFormatEtc,  STGMEDIUM *pMedium);
    HRESULT __stdcall QueryGetData			(FORMATETC *pFormatEtc);
	HRESULT __stdcall GetCanonicalFormatEtc (FORMATETC *pFormatEct,  FORMATETC *pFormatEtcOut);
    HRESULT __stdcall SetData				(FORMATETC *pFormatEtc,  STGMEDIUM *pMedium,  BOOL fRelease);
	HRESULT __stdcall EnumFormatEtc			(DWORD      dwDirection, IEnumFORMATETC **ppEnumFormatEtc);
	HRESULT __stdcall DAdvise				(FORMATETC *pFormatEtc,  DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection);
	HRESULT __stdcall DUnadvise				(DWORD      dwConnection);
	HRESULT __stdcall EnumDAdvise			(IEnumSTATDATA **ppEnumAdvise);
	
	//
    // Constructor / Destructor
	//
    COLEDataObject(FORMATETC *fmt, STGMEDIUM *stgmed, int count);
    ~COLEDataObject();

	HGLOBAL DupMem(HGLOBAL hMem);
private:

	int LookupFormatEtc(FORMATETC *pFormatEtc);

    //
	// any private members and functions
	//
    LONG	   m_lRefCount;

	FORMATETC *m_pFormatEtc;
	STGMEDIUM *m_pStgMedium;
	LONG	   m_nNumFormats;
};

#endif // !defined(AFX_OLEDATAOBJECT_H__B19A8E7E_EBBC_41B9_A66E_8DF1C0C163BB__INCLUDED_)
