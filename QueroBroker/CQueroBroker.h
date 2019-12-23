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

// CQueroBroker.h : Declaration of the CQueroBroker

#pragma once
#include "resource.h"       // main symbols

#include "QueroBroker.h"
#include "DLL\QueroBroker_DLL.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CQueroBroker

class ATL_NO_VTABLE CQueroBroker :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CQueroBroker, &CLSID_QueroBroker>,
	public IDispatchImpl<IQueroBroker, &IID_IQueroBroker, &LIBID_QueroBrokerLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CQueroBroker();

	DECLARE_REGISTRY_RESOURCEID(IDR_QUEROBROKER1)

	DECLARE_NOT_AGGREGATABLE(CQueroBroker)

	BEGIN_COM_MAP(CQueroBroker)
		COM_INTERFACE_ENTRY(IQueroBroker)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

public:
	STDMETHOD(Hook_IEFrame)(LONG hwnd_IEFrame, LONG hwnd_QToolbar, UINT Options, UINT Options2, INT IE_MajorVersion);
	STDMETHOD(Unhook_IEFrame)(LONG hwnd_IEFrame);
	STDMETHOD(SetOptions)(UINT Options, UINT Options2, UINT UpdateQueroInstances);
	STDMETHOD(SetDwmFrameTopMargin)(LONG hwnd_IEFrame, INT NewDwmFrameTopMargin);
	STDMETHOD(GetDwmFrameTopMargin)(LONG hwnd_IEFrame);
	STDMETHOD(SetIcon)(LONG hwnd_IEFrame, LONG hIcon);
	STDMETHOD(PostMessageToIE)(LONG hwnd_IEFrame, UINT uMsg, WPARAM wParam, LPARAM lParam);
	STDMETHOD(SetWindowPos)(LONG hwnd_IEFrame, LONG hwnd_InsertAfter, INT x, INT y, INT cx, INT cy, UINT uFlags);
	STDMETHOD(RegWrite_DWORD)(LONG hwnd_IEFrame, UINT ValueId, DWORD dwValue, BOOL bDeleteValue);
	STDMETHOD(RegRead_DWORD)(UINT ValueId, DWORD* pValue);
	STDMETHOD(IsTabbedBrowsing)(LONG hwnd_IEFrame);
	STDMETHOD(SetTitle)(LONG hwnd_IEFrame, BSTR bstrTitle);

	LPTSTR GetCurrentUserSid();

private:
	static const HKEY reg_keys[N_REG_VALUES];
	static const TCHAR* reg_subkeys[N_REG_VALUES];
	static const TCHAR* reg_values[N_REG_VALUES];
};

OBJECT_ENTRY_AUTO(__uuidof(QueroBroker), CQueroBroker)
