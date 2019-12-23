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
// OLEDropSource.cpp: implementation of the COLEDropSource class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OLEDropSource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COLEDropSource::COLEDropSource() 
{
	m_lRefCount = 1;
}

COLEDropSource::~COLEDropSource()
{
}

//
//	IUnknown::AddRef
//
ULONG __stdcall COLEDropSource::AddRef(void)
{
    // increment object reference count
    return InterlockedIncrement(&m_lRefCount);
}

//
//	IUnknown::Release
//
ULONG __stdcall COLEDropSource::Release(void)
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
HRESULT __stdcall COLEDropSource::QueryInterface(REFIID iid, void **ppvObject)
{
	HRESULT hr;

    // Check to see what interface has been requested
    if(iid == IID_IDropSource || iid == IID_IUnknown)
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

//
//	COLEDropSource::QueryContinueDrag
//
//	Called by OLE whenever Escape/Control/Shift/Mouse buttons have changed
//
HRESULT __stdcall COLEDropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
{
	HRESULT hr;

	// If the <Escape> key has been pressed since the last call, cancel the drop
	if(fEscapePressed == TRUE) hr=DRAGDROP_S_CANCEL;	
	// If the <LeftMouse> button has been released, then do the drop!
	else if((grfKeyState & MK_LBUTTON) == 0) hr=DRAGDROP_S_DROP;
	// Otherwise continue with the drag-drop
	else hr=S_OK;
	
	return hr;
}


//
//	COLEDropSource::GiveFeedback
//
//	Return either S_OK, or DRAGDROP_S_USEDEFAULTCURSORS to instruct OLE to use the
//  default mouse cursor images
//
HRESULT __stdcall COLEDropSource::GiveFeedback(DWORD dwEffect)
{
	return DRAGDROP_S_USEDEFAULTCURSORS;
}