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

// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__1E6B8A8B_FF2B_4335_9513_7BB74B9BDDD3__INCLUDED_)
#define AFX_STDAFX_H__1E6B8A8B_FF2B_4335_9513_7BB74B9BDDD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Build

#include "Build.h"

// Strict type checking

#define STRICT

// ATL

#define _ATL_APARTMENT_THREADED

// Work-around for 'ATL' : ambiguous symbol error message
#if _MSC_VER == 1200
	#define ATL ATLFIX
	#include <atlbase.h>
	#undef ATL
	namespace ATL = ::ATLFIX;
#else
	#include <atlbase.h>
#endif

// Module reference
extern CComModule _Module;

#include <atlcom.h>
#include <atlwin.h>

// Shell includes (IDeskBand etc.)

#include <shlguid.h>
#include <shlobj.h>

// WTL

#define _WTL_NO_CSTRING

#include <atlapp.h>
#include <atlmisc.h>
#include <atlctl.h>
#include <atlframe.h>
#include <atlddx.h>
#include <atldlgs.h>
#include <atlctrls.h>
#include <atlctrlw.h>
#include <atlctrlx.h>
#include <atlscrl.h>
#ifdef COMPILE_FOR_WINDOWS_VISTA
	#include <atltheme.h>
#endif


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__1E6B8A8B_FF2B_4335_9513_7BB74B9BDDD3__INCLUDED)
