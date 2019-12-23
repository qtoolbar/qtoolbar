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

// CQueroBroker.cpp : Implementation of CQueroBroker

#include "stdafx.h"
#include "CQueroBroker.h"
#include <commctrl.h>
#define SECURITY_WIN32
#include <security.h>
#include <Sddl.h>

// Quero Debug Console
#define AFX_STDAFX_H__1E6B8A8B_FF2B_4335_9513_7BB74B9BDDD3__INCLUDED_
#include "..\Quero_x64\QDebug.h"
#include "..\Quero_x64\QDebug.cpp"

// CQueroBroker

const HKEY CQueroBroker::reg_keys[N_REG_VALUES]=
{
	HKEY_CURRENT_USER,
	HKEY_LOCAL_MACHINE
};
const TCHAR* CQueroBroker::reg_subkeys[N_REG_VALUES]=
{
	L"Software\\Policies\\Microsoft\\Internet Explorer\\Toolbars\\Restrictions",
	L"Software\\Policies\\Microsoft\\Internet Explorer\\Toolbars\\Restrictions"
};
const TCHAR* CQueroBroker::reg_values[N_REG_VALUES]=
{
	L"NoNavBar",
	L"NoNavBar"
};

CQueroBroker::CQueroBroker()
{
	// Query Windows version
	OSVERSIONINFO VersionInfo;
	VersionInfo.dwOSVersionInfoSize=sizeof VersionInfo;
	GetVersionEx(&VersionInfo);
	DLL_SetWindowsVersion(((VersionInfo.dwMajorVersion)<<8)+((VersionInfo.dwMinorVersion)&0xFF));

	QDEBUG_PRINT(L"QueroBroker",L"instance created.");
}

STDMETHODIMP CQueroBroker::Hook_IEFrame(LONG hwnd_IEFrame, LONG hwnd_QToolbar, UINT Options, UINT Options2, INT IE_MajorVersion)
{
	// Set IE version
	DLL_Set_IE_MajorVersion(IE_MajorVersion);
	// Customize UI
	if(Options2&(OPTION2_HideFavoritesButton|OPTION2_HideAddressBox)) DLL_Customize_IEFrame((HWND)LongToHandle(hwnd_IEFrame),Options,Options2);
	// Copy options
	DLL_SetOptions(Options,Options2,UPDATE_NONE);
	// Hook frame
	return DLL_Hook_IEFrame((HWND)LongToHandle(hwnd_IEFrame));
}

STDMETHODIMP CQueroBroker::Unhook_IEFrame(LONG hwnd_IEFrame)
{
	return DLL_Unhook_IEFrame((HWND)LongToHandle(hwnd_IEFrame));
}

STDMETHODIMP CQueroBroker::SetOptions(UINT Options, UINT Options2, UINT UpdateQueroInstances)
{
	DLL_SetOptions(Options,Options2,UpdateQueroInstances);
	return S_OK;
}

STDMETHODIMP CQueroBroker::SetDwmFrameTopMargin(LONG hwnd_IEFrame, INT NewDwmFrameTopMargin)
{
	return DLL_SetDwmFrameTopMargin((HWND)LongToHandle(hwnd_IEFrame),NewDwmFrameTopMargin);
}

STDMETHODIMP CQueroBroker::GetDwmFrameTopMargin(LONG hwnd_IEFrame)
{
	return DLL_GetDwmFrameTopMargin((HWND)LongToHandle(hwnd_IEFrame));
}

STDMETHODIMP CQueroBroker::SetIcon(LONG hwnd_IEFrame, LONG hIcon)
{
	DLL_SetIcon((HWND)LongToHandle(hwnd_IEFrame),(HICON)LongToHandle(hIcon));
	return S_OK;
}

STDMETHODIMP CQueroBroker::PostMessageToIE(LONG hwnd_IEFrame, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HRESULT result;
	
	// Restrict commands (QuickTabs, WM_QUERO_UPDATE_DWMTOPMARGIN)
	if((uMsg==WM_COMMAND && wParam==0x100002) || uMsg==WM_QUERO_UPDATE_DWMTOPMARGIN)
	{
		result=PostMessage((HWND)LongToHandle(hwnd_IEFrame),uMsg,wParam,lParam)?S_OK:E_FAIL;
	}
	else result=E_ACCESSDENIED;

	return result;
}

STDMETHODIMP CQueroBroker::SetWindowPos(LONG hwnd_IEFrame, LONG hwnd_InsertAfter, INT x, INT y, INT cx, INT cy, UINT uFlags)
{
	if(DLL_Is_IEFrame((HWND)LongToHandle(hwnd_IEFrame)))
	{
		ShowWindow((HWND)LongToHandle(hwnd_IEFrame),SW_RESTORE);
		::SetWindowPos((HWND)LongToHandle(hwnd_IEFrame),(HWND)LongToHandle(hwnd_InsertAfter),x,y,cx,cy,uFlags);
	}

	return S_OK;
}

STDMETHODIMP CQueroBroker::RegWrite_DWORD(LONG hwnd_IEFrame, UINT ValueId, DWORD dwValue, BOOL bDeleteValue)
{
	HRESULT result;
	SHELLEXECUTEINFO sei;
	LPTSTR user_string_sid;
	TCHAR keyname[MAX_PATH];
	TCHAR reg_parameters[MAX_PATH];

	// Try registry operation with current user credentials

	if(ValueId<N_REG_VALUES)
	{
		if(bDeleteValue)
		{
			result=SHDeleteValue(reg_keys[ValueId],reg_subkeys[ValueId],reg_values[ValueId]);
			SHDeleteEmptyKey(reg_keys[ValueId],reg_subkeys[ValueId]);
			QD(L"delete value");
		}
		else
		{
			result=SHSetValue(reg_keys[ValueId],reg_subkeys[ValueId],reg_values[ValueId],REG_DWORD,&dwValue,sizeof dwValue);
			QD(L"set value");
		}
	}
	else result=E_INVALIDARG;

	// Try to elevate privileges on access denied

	if(result==ERROR_ACCESS_DENIED && hwnd_IEFrame)
	{
		ZeroMemory(&sei,sizeof sei);
		user_string_sid=GetCurrentUserSid();
		if(user_string_sid)
		{
			StringCbPrintf(keyname,sizeof keyname,L"HKU\\%s",user_string_sid);
			if(bDeleteValue)
				StringCbPrintf(reg_parameters,sizeof reg_parameters,L"delete \"%s\\%s\" /v %s /f",keyname,reg_subkeys[ValueId],reg_values[ValueId]);
			else
				StringCbPrintf(reg_parameters,sizeof reg_parameters,L"add \"%s\\%s\" /v %s /t REG_DWORD /d %d /f",keyname,reg_subkeys[ValueId],reg_values[ValueId],dwValue);

			QD(reg_parameters);

			sei.cbSize=sizeof sei;
			sei.lpFile=L"reg.exe";
			sei.lpVerb=L"runas";
			sei.lpParameters=reg_parameters;
			sei.hwnd=(HWND)LongToHandle(hwnd_IEFrame);
			if(ShellExecuteEx(&sei))
			{
				result=S_OK;
				CloseHandle(sei.hProcess);
			}

			LocalFree(user_string_sid);
		}
	}

	return result;
}

STDMETHODIMP CQueroBroker::RegRead_DWORD(UINT ValueId, DWORD* pValue)
{
	HRESULT result;
	DWORD dwType;
	DWORD dwData;
	DWORD cbData;

	if(ValueId<N_REG_VALUES)
	{
		cbData=sizeof dwData;
		if(SHGetValue(reg_keys[ValueId],reg_subkeys[ValueId],reg_values[ValueId],&dwType,&dwData,&cbData)==ERROR_SUCCESS && dwType==REG_DWORD)
		{
			*pValue=dwData;
			result=S_OK;
		}
		else result=E_FAIL;
	}
	else result=E_INVALIDARG;

	return result;
}

LPTSTR CQueroBroker::GetCurrentUserSid()
{
	TCHAR username[MAX_PATH];
	ULONG username_length;
	BYTE user_sid[SECURITY_MAX_SID_SIZE];
	DWORD user_sid_size;
	SID_NAME_USE user_sid_name_used;
	TCHAR domainname[MAX_PATH];
	DWORD cch_domainname;
	LPTSTR user_string_sid;

	user_string_sid=NULL;

	username_length=MAX_PATH;
	if(GetUserNameEx(NameSamCompatible,username,&username_length))
	{
		QDEBUG_PRINT(L"UserName",username);
		user_sid_size=sizeof user_sid;
		cch_domainname=MAX_PATH;
		if(LookupAccountName(NULL,username,&user_sid,&user_sid_size,domainname,&cch_domainname,&user_sid_name_used))
		{
			if(ConvertSidToStringSid(&user_sid,&user_string_sid)==0) user_string_sid=NULL;
			QDEBUG_CODE else QDEBUG_PRINT(L"StringSid",user_string_sid);
		}
	}

	return user_string_sid;
}

STDMETHODIMP CQueroBroker::IsTabbedBrowsing(LONG hwnd_IEFrame)
{
	HRESULT result;

	HWND hwnd_ReBar;
	HWND hwnd_TabBand;

	if(DLL_Get_IE_MajorVersion()>=9)
	{
		hwnd_ReBar=DLL_GetReBar((HWND)LongToHandle(hwnd_IEFrame),L"WorkerW",NULL);
		if(hwnd_ReBar)
		{
			hwnd_ReBar=DLL_GetReBar((HWND)LongToHandle(hwnd_IEFrame),L"ReBarWindow32",NULL);
		}
	}
	else
	{
		hwnd_ReBar=DLL_GetReBar((HWND)LongToHandle(hwnd_IEFrame),L"CommandBarClass",NULL);
	}

	if(hwnd_ReBar)
	{
		hwnd_TabBand=FindWindowEx(hwnd_ReBar,NULL,L"TabBandClass",NULL);
		if(hwnd_TabBand) result=S_OK;
	}

	return result;
}

STDMETHODIMP CQueroBroker::SetTitle(LONG hwnd_IEFrame, BSTR bstrTitle)
{
	TCHAR title[255];

	title[0]=0;
	if(SysStringLen(bstrTitle))
	{
		StringCchCopyN(title,255,bstrTitle,128);
		StringCbCat(title,sizeof title,L" - ");
	}
	StringCbCat(title,sizeof title,L"Windows Internet Explorer");
	
	SendMessage((HWND)LongToHandle(hwnd_IEFrame),WM_SETTEXT,WPARAM_WM_SETTEXT_DOCUMENT_TITLE,(LPARAM)title);
	
	//QDEBUG_PRINT(L"SetTitle",bstrTitle);
	return S_OK;
}