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
// OLEDataObject.cpp: implementation of the COLEDataObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OLEDataObject.h"
#include "OLEEnumFormat.h"
#include "QDebug.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COLEDataObject::COLEDataObject(FORMATETC *fmtetc, STGMEDIUM *stgmed, int count) 
{
	int i;

	m_lRefCount  = 1;
	m_nNumFormats = count;
	
	m_pFormatEtc  = new FORMATETC[count];
	m_pStgMedium  = new STGMEDIUM[count];

	if(m_pFormatEtc && m_pStgMedium)
	{
		for(i = 0; i < count; i++)
		{
			m_pFormatEtc[i] = fmtetc[i];
			m_pStgMedium[i] = stgmed[i];
		}
	}
}

COLEDataObject::~COLEDataObject()
{
	int i;

	// Cleanup

	if(m_pStgMedium)
	{
		for(i=0;i<m_nNumFormats;i++) ReleaseStgMedium(&m_pStgMedium[i]);
		delete[] m_pStgMedium;
	}

	if(m_pFormatEtc) delete[] m_pFormatEtc;
}

//
//	IUnknown::AddRef
//
ULONG __stdcall COLEDataObject::AddRef(void)
{
    // increment object reference count
    return InterlockedIncrement(&m_lRefCount);
}

//
//	IUnknown::Release
//
ULONG __stdcall COLEDataObject::Release(void)
{
    // decrement object reference count
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
HRESULT __stdcall COLEDataObject::QueryInterface(REFIID iid, void **ppvObject)
{
	HRESULT hr;

    // Check to see what interface has been requested
    if(iid == IID_IDataObject || iid == IID_IUnknown)
    {
        AddRef();
        *ppvObject = this;
        hr=S_OK;
    }
    else
    {
        *ppvObject = NULL;
		hr=E_NOINTERFACE;
    }

	return hr;
}

HGLOBAL COLEDataObject::DupMem(HGLOBAL hMem)
{
	SIZE_T len;
	PVOID pSource,pDest;

	// Lock the source memory object
	len = GlobalSize(hMem);
	pSource = GlobalLock(hMem);
	if(pSource)
	{
		// Create a fixed "global" block - i.e. just a regular lump of our process heap
		pDest = GlobalAlloc(GMEM_FIXED, len);

		if(pDest) CopyMemory(pDest, pSource, len);

		GlobalUnlock(hMem);
	}
	else pDest=NULL;
	
	return pDest;
}

int COLEDataObject::LookupFormatEtc(FORMATETC *pFormatEtc)
{
	for(int i = 0; i < m_nNumFormats; i++)
	{
		if((pFormatEtc->tymed & m_pFormatEtc[i].tymed)   &&
			pFormatEtc->cfFormat == m_pFormatEtc[i].cfFormat && 
			pFormatEtc->dwAspect == m_pFormatEtc[i].dwAspect)
		{
			return i;
		}
	}
	return -1;
}

//
//	IDataObject::GetData
//
HRESULT __stdcall COLEDataObject::GetData(FORMATETC *pFormatEtc, STGMEDIUM *pMedium)
{
	HRESULT hr=S_OK;
	int idx;

	// Try to match the requested FORMATETC with one of our supported formats
	idx = LookupFormatEtc(pFormatEtc);

	if(idx>=0)
	{
		// Transfer the data into the supplied storage-medium
		pMedium->tymed = m_pFormatEtc[idx].tymed;
		pMedium->pUnkForRelease = NULL;
	
		switch(m_pFormatEtc[idx].tymed)
		{
		case TYMED_HGLOBAL:
			pMedium->hGlobal = DupMem(m_pStgMedium[idx].hGlobal);
			break;
		default:
			hr=DV_E_FORMATETC;
		}
	}
	else hr=DV_E_FORMATETC;

	return hr;
}

//
//	IDataObject::GetDataHere
//
HRESULT __stdcall COLEDataObject::GetDataHere (FORMATETC *pFormatEtc, STGMEDIUM *pMedium)
{
	// GetDataHere is only required for IStream and IStorage mediums
	// It is an error to call GetDataHere for things like HGLOBAL and other clipboard formats
	//
	//	OleFlushClipboard 
	//
	return DATA_E_FORMATETC;
}

//
//	IDataObject::QueryGetData
//
//	Called to see if the IDataObject supports the specified format of data
//
HRESULT __stdcall COLEDataObject::QueryGetData (FORMATETC *pFormatEtc)
{
	return (LookupFormatEtc(pFormatEtc) == -1) ? DV_E_FORMATETC : S_OK;
}

//
//	IDataObject::GetCanonicalFormatEtc
//
HRESULT __stdcall COLEDataObject::GetCanonicalFormatEtc (FORMATETC *pFormatEct, FORMATETC *pFormatEtcOut)
{
	// Apparently we have to set this field to NULL even though we don't do anything else
	pFormatEtcOut->ptd = NULL;
	return E_NOTIMPL;
}

//
//	IDataObject::SetData
//
HRESULT __stdcall COLEDataObject::SetData (FORMATETC *pFormatEtc, STGMEDIUM *pMedium,  BOOL fRelease)
{
	return E_NOTIMPL;
}

//
//	IDataObject::EnumFormatEtc
//
HRESULT __stdcall COLEDataObject::EnumFormatEtc (DWORD dwDirection, IEnumFORMATETC **ppEnumFormatEtc)
{
	HRESULT hr;

	if(dwDirection == DATADIR_GET)
	{
		// for Win2k+ you can use the SHCreateStdEnumFmtEtc API call, however
		// to support all Windows platforms we need to implement IEnumFormatEtc ourselves.

		if(m_nNumFormats && m_pFormatEtc && ppEnumFormatEtc)
		{
			*ppEnumFormatEtc = new COLEEnumFormat(m_pFormatEtc,m_nNumFormats);

			hr=(*ppEnumFormatEtc) ? S_OK : E_OUTOFMEMORY;
		}
		else hr=E_INVALIDARG;
	}
	else
	{
		// the direction specified is not support for drag&drop
		hr=E_NOTIMPL;
	}

	return hr;
}

//
//	IDataObject::DAdvise
//
HRESULT __stdcall COLEDataObject::DAdvise (FORMATETC *pFormatEtc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection)
{
	return OLE_E_ADVISENOTSUPPORTED;
}

//
//	IDataObject::DUnadvise
//
HRESULT __stdcall COLEDataObject::DUnadvise (DWORD dwConnection)
{
	return OLE_E_ADVISENOTSUPPORTED;
}

//
//	IDataObject::EnumDAdvise
//
HRESULT __stdcall COLEDataObject::EnumDAdvise (IEnumSTATDATA **ppEnumAdvise)
{
	return OLE_E_ADVISENOTSUPPORTED;
}















