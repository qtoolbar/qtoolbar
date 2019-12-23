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

// QueroBroker.cpp : Implementation of WinMain

#include "stdafx.h"
#include "resource.h"
#include "QueroBroker.h"
#include "dlldatax.h"

#ifdef COMPILE_FOR_WINDOWS_VISTA
#include <iepmapi.h>
#endif

class CQueroBrokerModule : public CAtlExeModuleT< CQueroBrokerModule >
{
public :
	DECLARE_LIBID(LIBID_QueroBrokerLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_QUEROBROKER, "{FD314553-3565-45C0-8E80-4096EFF4E3D9}")

	HRESULT RegisterServer(BOOL bRegTypeLib = FALSE, const CLSID* pCLSID = NULL) throw( );
	HRESULT UnregisterServer(BOOL bUnRegTypeLib, const CLSID* pCLSID = NULL) throw( );
};

CQueroBrokerModule _AtlModule;

HINSTANCE g_hInstance;


extern "C" int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	g_hInstance=hInstance;
	return _AtlModule.WinMain(nShowCmd);
}

HRESULT CQueroBrokerModule::RegisterServer(BOOL bRegTypeLib, const CLSID* pCLSID) throw( )
{
	HKEY hKey;
	DWORD Policy;
	DWORD AppPath_Lengh;
	TCHAR AppPath[MAX_PATH];

	// Add the Quero Broker to the global elevation policy
	AppPath_Lengh=GetModuleFileName(g_hInstance,AppPath,MAX_PATH);
	if(AppPath_Lengh)
	{
		if(PathRemoveFileSpec(AppPath))
		{
			#ifdef _WIN64
				if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Internet Explorer\\Low Rights\\ElevationPolicy\\{C8540297-E35E-446C-8A85-B6025C677BFA}", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL)==ERROR_SUCCESS)
			#else
				if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Internet Explorer\\Low Rights\\ElevationPolicy\\{C75EEBE3-352A-4c64-B950-BADE38DDB732}", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL)==ERROR_SUCCESS)
			#endif
			{
				RegSetValueEx(hKey,L"AppName",0,REG_SZ,(BYTE*)L"QueroBroker.exe",32);
				RegSetValueEx(hKey,L"AppPath",0,REG_SZ,(BYTE*)AppPath,(AppPath_Lengh+1)*sizeof TCHAR);
				Policy=3;
				RegSetValueEx(hKey,L"Policy",0,REG_DWORD,(BYTE*)&Policy,sizeof Policy);
				RegCloseKey(hKey);

				#ifdef COMPILE_FOR_WINDOWS_VISTA
				IERefreshElevationPolicy();
				#endif
			}
		}
	}

	return CAtlExeModuleT::RegisterServer(bRegTypeLib,pCLSID);
}

HRESULT CQueroBrokerModule::UnregisterServer(BOOL bUnRegTypeLib, const CLSID* pCLSID) throw( )
{
	// Remove the Quero Broker from the elevation policy
	#ifdef _WIN64
		SHDeleteKey(HKEY_LOCAL_MACHINE,L"SOFTWARE\\Microsoft\\Internet Explorer\\Low Rights\\ElevationPolicy\\{C8540297-E35E-446C-8A85-B6025C677BFA}");
	#else
		SHDeleteKey(HKEY_LOCAL_MACHINE,L"SOFTWARE\\Microsoft\\Internet Explorer\\Low Rights\\ElevationPolicy\\{C75EEBE3-352A-4c64-B950-BADE38DDB732}");
	#endif
	
	#ifdef COMPILE_FOR_WINDOWS_VISTA
	IERefreshElevationPolicy();
	#endif

	return CAtlExeModuleT::UnregisterServer(bUnRegTypeLib,pCLSID);
}

