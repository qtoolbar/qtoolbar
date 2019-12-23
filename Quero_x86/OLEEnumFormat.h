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
// OLEEnumFormat.h: interface for the COLEEnumFormat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLEENUMFORMAT_H__F419B7EC_882C_41B5_B2C7_4EBC9F5DDD1B__INCLUDED_)
#define AFX_OLEENUMFORMAT_H__F419B7EC_882C_41B5_B2C7_4EBC9F5DDD1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

class COLEEnumFormat : public IEnumFORMATETC
{
public:

	//
	// IUnknown members
	//
	HRESULT __stdcall  QueryInterface (REFIID iid, void ** ppvObject);
	ULONG	__stdcall  AddRef (void);
	ULONG	__stdcall  Release (void);

	//
	// IEnumFormatEtc members
	//
	HRESULT __stdcall  Next  (ULONG celt, FORMATETC * rgelt, ULONG * pceltFetched);
	HRESULT __stdcall  Skip  (ULONG celt); 
	HRESULT __stdcall  Reset (void);
	HRESULT __stdcall  Clone (IEnumFORMATETC ** ppEnumFormatEtc);

	//
	// Construction / Destruction
	//
	COLEEnumFormat(FORMATETC *pFormatEtc, int nNumFormats);
	~COLEEnumFormat();

	// helper function
	void DeepCopyFormatEtc(FORMATETC *dest, FORMATETC *source);

private:

	LONG		m_lRefCount;		// Reference count for this COM interface
	ULONG		m_nIndex;			// current enumerator index
	ULONG		m_nNumFormats;		// number of FORMATETC members
	FORMATETC * m_pFormatEtc;		// array of FORMATETC objects
};

#endif // !defined(AFX_OLEENUMFORMAT_H__F419B7EC_882C_41B5_B2C7_4EBC9F5DDD1B__INCLUDED_)
