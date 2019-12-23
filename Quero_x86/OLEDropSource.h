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
// OLEDropSource.h: interface for the COLEDropSource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLEDROPSOURCE_H__455445F7_D5DA_4720_9859_A0EFA93F8B50__INCLUDED_)
#define AFX_OLEDROPSOURCE_H__455445F7_D5DA_4720_9859_A0EFA93F8B50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

class COLEDropSource  : public IDropSource
{
public:

	//
  // IUnknown members
	//
  HRESULT __stdcall QueryInterface(REFIID iid, void ** ppvObject);
  ULONG   __stdcall AddRef(void);
  ULONG   __stdcall Release(void);
		
  //
	// IDropSource members
	//
  HRESULT __stdcall QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState);
	HRESULT __stdcall GiveFeedback(DWORD dwEffect);
	
	//
  // Constructor / Destructor
	//
   COLEDropSource();
   ~COLEDropSource();
	
private:

  //
	// private members and functions
	//
  LONG m_lRefCount;
};

#endif // !defined(AFX_OLEDROPSOURCE_H__455445F7_D5DA_4720_9859_A0EFA93F8B50__INCLUDED_)
