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
// OLEEnumFormat.cpp: implementation of the COLEEnumFormat class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OLEEnumFormat.h"
#include "QDebug.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//
//	Constructor 
//
COLEEnumFormat::COLEEnumFormat(FORMATETC *pFormatEtc, int nNumFormats)
{
	int i;

	m_lRefCount   = 1;
	m_nIndex      = 0;
	m_nNumFormats = nNumFormats;
	m_pFormatEtc  = new FORMATETC[nNumFormats];

	// Copy the FORMATETC structures
	if(m_pFormatEtc)
	{
		for(i = 0; i < nNumFormats; i++)
		{	
			DeepCopyFormatEtc(&m_pFormatEtc[i], &pFormatEtc[i]);
		}
	}
}

//
//	Destructor
//
COLEEnumFormat::~COLEEnumFormat()
{
	ULONG i;

	if(m_pFormatEtc)
	{
		for(i = 0; i < m_nNumFormats; i++)
		{
			if(m_pFormatEtc[i].ptd)	CoTaskMemFree(m_pFormatEtc[i].ptd);
		}

		delete[] m_pFormatEtc;
	}
}

//
//	Helper function to perform a "deep" copy of a FORMATETC
//
void COLEEnumFormat::DeepCopyFormatEtc(FORMATETC *dest, FORMATETC *source)
{
	// Copy the source FORMATETC into dest
	*dest = *source;
	
	if(source->ptd)
	{
		// Allocate memory for the DVTARGETDEVICE if necessary
		dest->ptd = (DVTARGETDEVICE*)CoTaskMemAlloc(sizeof(DVTARGETDEVICE));

		// Copy the contents of the source DVTARGETDEVICE into dest->ptd
		if(dest->ptd) *(dest->ptd) = *(source->ptd);
	}
}

//
//	IUnknown::AddRef
//
ULONG __stdcall COLEEnumFormat::AddRef(void)
{
    // Increment object reference count
    return InterlockedIncrement(&m_lRefCount);
}

//
//	IUnknown::Release
//
ULONG __stdcall COLEEnumFormat::Release(void)
{
    // Decrement object reference count
	LONG count = InterlockedDecrement(&m_lRefCount);
		
	if(count == 0)
	{
		delete this;
		return 0;
	}
	else
	{
		return count;
	}
}

//
//	IUnknown::QueryInterface
//
HRESULT __stdcall COLEEnumFormat::QueryInterface(REFIID iid, void **ppvObject)
{
    // Check to see what interface has been requested
    if(iid == IID_IEnumFORMATETC || iid == IID_IUnknown)
    {
        AddRef();
        *ppvObject = this;
        return S_OK;
    }
    else
    {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
}

//
//	IEnumFORMATETC::Next
//
//	If the returned FORMATETC structure contains a non-null "ptd" member, then
//  the caller must free this using CoTaskMemFree (stated in the COM documentation)
//
HRESULT __stdcall COLEEnumFormat::Next(ULONG celt, FORMATETC *pFormatEtc, ULONG * pceltFetched)
{
	ULONG copied  = 0;
	HRESULT hr;

	if(celt && pFormatEtc)
	{
		// Copy FORMATETC structures into caller's buffer
		while(m_nIndex < m_nNumFormats && copied < celt)
		{
			DeepCopyFormatEtc(&pFormatEtc[copied], &m_pFormatEtc[m_nIndex]);
			copied++;
			m_nIndex++;
		}

		// Store result
		if(pceltFetched) *pceltFetched = copied;

		// Did we copy all that was requested?
		hr=(copied == celt) ? S_OK : S_FALSE;
	}
	else hr=E_INVALIDARG;

	return hr;
}


//
//	IEnumFORMATETC::Skip
//
HRESULT __stdcall COLEEnumFormat::Skip(ULONG celt)
{
	m_nIndex += celt;
	return (m_nIndex <= m_nNumFormats) ? S_OK : S_FALSE;
}

//
//	IEnumFORMATETC::Reset
//
HRESULT __stdcall COLEEnumFormat::Reset(void)
{
	m_nIndex = 0;
	return S_OK;
}

//
//	IEnumFORMATETC::Clone
//
HRESULT __stdcall COLEEnumFormat::Clone(IEnumFORMATETC ** ppEnumFormatEtc)
{
	HRESULT hr;

	if(m_nNumFormats && m_pFormatEtc && ppEnumFormatEtc)
	{
		// Make a duplicate enumerator
		*ppEnumFormatEtc = new COLEEnumFormat(m_pFormatEtc, m_nNumFormats);
		if(ppEnumFormatEtc)
		{
			// Manually set the index state
			((COLEEnumFormat *) *ppEnumFormatEtc)->m_nIndex = m_nIndex;

			hr=S_OK;
		}
		else hr=E_OUTOFMEMORY;
	}
	else hr=E_INVALIDARG;

	return hr;
}







