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

// Quero.cpp : Implementation of DLL Exports.

// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f Querops.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "Quero.h"

#include "Quero_i.c"
#include "QueroBand.h"
#include "QueroFilter.h"

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_QueroBand, CQueroBand)
OBJECT_ENTRY(CLSID_QueroFilter, CQueroFilter)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		QDEBUG_PRINTF(L"Quero Toolbar",L"version %x build %d launched.",QUERO_VERSION,QUERO_BUILD);
		_Module.Init(ObjectMap, hInstance, &LIBID_QUEROLib);
        DisableThreadLibraryCalls(hInstance);
		break;

	case DLL_PROCESS_DETACH:
		_Module.Term();
		break;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	IWebBrowser2 *pIE = NULL;
	HRESULT result;
	HKEY hKey;

    // Registers object, typelib and all interfaces in typelib

	result=_Module.RegisterServer(TRUE);

	if(SUCCEEDED_OK(result))
	{
		if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Internet Explorer\\Toolbar", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL)==ERROR_SUCCESS)
		{
			RegSetValueEx(hKey, L"{A411D7F4-8D11-43EF-BDE4-AA921666388A}", 0, REG_BINARY, NULL, 0);
			RegCloseKey(hKey);
		}

		// Initialize g_WindowsVersion
		
		OSVERSIONINFO OSVersionInfo;

		OSVersionInfo.dwOSVersionInfoSize=sizeof OSVersionInfo;
		GetVersionEx(&OSVersionInfo);
		g_WindowsVersion=((OSVersionInfo.dwMajorVersion)<<8)+((OSVersionInfo.dwMinorVersion)&0xFF);

		// Mark add-on as Enhanced Protected Mode compatible

		RegisterCLSIDInCategory(CLSID_QueroBand,CATID_AppContainerCompatible,true);
	
		// Write version information to the registry and clean up deprecated keys from previous Quero versions

		VersionInfo vi;

		hKey=CQToolbar::OpenQueroKey(HKEY_CURRENT_USER,NULL,true);
		if(hKey)
		{
			UpdateVersion(hKey,&vi,false,true);
			RegCloseKey(hKey);
		}

		// Set GPO NoNavBar

		UpdateAllUsers(false);

		// Start a new instance of Internet Explorer and make our bar visible
		
		HRESULT hr = CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_LOCAL_SERVER, IID_IWebBrowser2, (LPVOID*)&pIE);
		if(SUCCEEDED_OK(hr))
		{
			pIE->put_Visible(VARIANT_FALSE);
			CQueroBand::ShowQueroToolbar(pIE,VARIANT_TRUE);
			pIE->Quit();
		}
	}

    return result;
}

HRESULT RegisterCLSIDInCategory(REFCLSID clsid, CATID catid, bool bRegister)
{
#ifdef COMPILE_FOR_WINDOWS_VISTA
	ICatRegister* pCatRegister = NULL;
	HRESULT hr;

	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (LPVOID*)&pCatRegister);
	if (SUCCEEDED_OK(hr))
	{
		// Register/Unregister the category as being "implemented" by the class
		CATID rgcatid[1];
		
		rgcatid[0] = catid;
		if(bRegister)
		{
			hr = pCatRegister->RegisterClassImplCategories(clsid, 1, rgcatid);
		}
		else
		{
			hr = pCatRegister->UnRegisterClassImplCategories(clsid, 1, rgcatid);
		}

		pCatRegister->Release();
	}

	return hr;
#else
	return E_NOTIMPL;
#endif
}

bool LoadVersion(HKEY hKeyQuero,VersionInfo *vi)
{
	DWORD size;
	
	size=sizeof(VersionInfo);
	return (RegQueryValueEx(hKeyQuero,SETTINGS_VALUES[SETTINGS_VALUES_VERSION],0,NULL,(LPBYTE)vi,&size)==ERROR_SUCCESS && size==sizeof(VersionInfo));
}

UINT UpdateVersion(HKEY hKeyQuero,VersionInfo *vi,bool bVersionLoaded,bool IsDLLRegistration)
{
	bool bSetVersion;
	bool bUpdateProfiles;
	bool bSyncSettings;
	UINT VersionFlags;

	bSyncSettings=false;

	// Load version if not already loaded

	if(IsDLLRegistration && bVersionLoaded==false)
	{
		bVersionLoaded=LoadVersion(hKeyQuero,vi);
	}
	
	// Migrate "Quero Toolbar" registry key from older versions
	// If LoadVersion failed, try to locate the "Quero Toolbar" key under InternetRegistry or AppDataLow
	// On Windows 7/Vista: 	Move "Quero Toolbar" registry key from InternetRegistry to AppDataLow
	// On Windows 8: Move "Quero Toolbar" registry key from AppDataLow to InternetRegistry

	#ifdef COMPILE_FOR_WINDOWS_VISTA
	if(bVersionLoaded==false)
	{
		HKEY hKeyInternetRegistry;
		HKEY hKeyHKCU;
		TCHAR InternetRegistrySID[255];
		DWORD cchSID;

		hKeyHKCU=NULL;

		// Open AppDataLow
		if(IsWindows8OrLater())
		{
			if(RegOpenKeyEx(HKEY_CURRENT_USER,L"Software\\AppDataLow",0,KEY_ALL_ACCESS,&hKeyHKCU)!=ERROR_SUCCESS)
				hKeyHKCU=NULL;
		}

		// Open InternetRegistry
		if(hKeyHKCU==NULL)
		{
			if(RegOpenKeyEx(HKEY_CURRENT_USER,L"Software\\Microsoft\\Internet Explorer\\InternetRegistry\\REGISTRY\\USER",0,KEY_READ,&hKeyInternetRegistry)==ERROR_SUCCESS)
			{
				cchSID=255;
				if(RegEnumKeyEx(hKeyInternetRegistry,0,InternetRegistrySID,&cchSID,NULL,NULL,NULL,NULL)==ERROR_SUCCESS)
				{
					if(RegOpenKeyEx(hKeyInternetRegistry,InternetRegistrySID,0,KEY_ALL_ACCESS,&hKeyHKCU)!=ERROR_SUCCESS)
						hKeyHKCU=NULL; 
				}
				RegCloseKey(hKeyInternetRegistry);
			}
		}

		// If open InternetRegistry failed, try moving HKCU\Software\Quero Toolbar instead
		if(hKeyHKCU==NULL)
		{
			if(RegOpenKeyEx(HKEY_CURRENT_USER,NULL,0,KEY_ALL_ACCESS,&hKeyHKCU)!=ERROR_SUCCESS)
				hKeyHKCU=NULL;
		}

		// Move "Quero Toolbar" registry key to new destination
		if(hKeyHKCU)
		{
			// SHCopyKey does not copy the security attributes.
			// For Enhanced Protected Mode compatibility security attributes need to be intact
			if(SHCopyKey(hKeyHKCU,L"Software\\Quero Toolbar",hKeyQuero,0)==ERROR_SUCCESS)
				//RegCopyTree(hKeyHKCU,L"Software\\Quero Toolbar",hKeyQuero)==ERROR_SUCCESS)
			{
				SHDeleteKey(hKeyHKCU,L"Software\\Quero Toolbar");
			}
			RegCloseKey(hKeyHKCU);
		}

		// Try loading Quero version again
		bSyncSettings=bVersionLoaded=LoadVersion(hKeyQuero,vi);
	}
	#endif

	if(bVersionLoaded)
	{
		bSetVersion=false;
		bUpdateProfiles=false;

		// Update Version and Brand

		if(vi->Version!=QUERO_VERSION || vi->Build!=QUERO_BUILD || vi->Language!=QUERO_LANGUAGE)
		{
			// Upgrading from Quero Toolbar 2
			#ifndef COMPILE_FOR_WINDOWS_VISTA
			if(vi->Version>=0x00020000 && vi->Version<0x03000000)
			{
				if(IsDLLRegistration) SHDeleteKey(HKEY_CLASSES_ROOT,L"PROTOCOLS\\Filter\\text/html"); // Unregister global MIME filter
				RegDeleteValue(hKeyQuero,L"BlockFlashAds");
				RegDeleteValue(hKeyQuero,L"Buttons");
			}
			#endif

			// Upgrading from version prior Quero Toolbar 4
			if(vi->Version<0x04000000)
			{
				RegDeleteValue(hKeyQuero,L"DefaultEngines");
				RegDeleteValue(hKeyQuero,L"Profile");
			}

			// Update per-user search profiles if newer version is installed or language or build changed
			#ifdef COMPILE_FOR_WINDOWS_VISTA
				if(vi->Version<QUERO_VERSION || vi->Language!=QUERO_LANGUAGE) bUpdateProfiles=true;
			#else
				if(vi->Version<=QUERO_VERSION) bUpdateProfiles=true;
			#endif

			vi->Version=QUERO_VERSION;
			vi->Build=QUERO_BUILD;
			vi->Language=QUERO_LANGUAGE;
			bSetVersion=true;
		}

		// Update Flags

		VersionFlags=vi->Flags; // Return old flags

		if(IsDLLRegistration==false && (vi->Flags&VERSION_FLAG_FIRSTRUN))
		{
			vi->Flags&=~VERSION_FLAG_FIRSTRUN;
			bSetVersion=true;
		}
	}
	else
	{
		vi->Version=QUERO_VERSION;
		vi->Build=QUERO_BUILD;
		vi->Language=QUERO_LANGUAGE;
		vi->Flags=IsDLLRegistration?VERSION_FLAG_FIRSTRUN:0;
		VersionFlags=VERSION_FLAG_FIRSTRUN;
		bSetVersion=true;
		bUpdateProfiles=true;
	}

	if(bSetVersion)
	{
		vi->Reserved1=0;
		vi->Reserved2=0;
		vi->Reserved3=0;
		RegSetValueEx(hKeyQuero,SETTINGS_VALUES[SETTINGS_VALUES_VERSION],0,REG_BINARY,(LPBYTE)vi,sizeof(VersionInfo));
	}

	if(bUpdateProfiles)
	{
		CProfiles profiles;
		profiles.UpdateUserProfiles();
	}

	if(bSyncSettings) VersionFlags|=VERSION_FLAG_SYNC_SETTINGS;

	return VersionFlags;
}

void UpdateAllUsers(bool bUninstall)
{
	LONG result;
	HKEY hKeyUser;
	HKEY hKeyQuero;
	DWORD dwIndex;
	DWORD QueroOptions1;
	DWORD cbUserKeyName;
	TCHAR UserKeyName[MAX_PATH];
	
	dwIndex=0;
	do
	{
		cbUserKeyName=sizeof UserKeyName;
		result=RegEnumKeyEx(HKEY_USERS,dwIndex,UserKeyName,&cbUserKeyName,NULL,NULL,NULL,NULL);
		if(result==ERROR_SUCCESS)
		{
			//QDEBUG_PRINT(L"UpdateAllUsers",UserKeyName);
			if(RegOpenKeyEx(HKEY_USERS,UserKeyName,NULL,KEY_ALL_ACCESS,&hKeyUser)==ERROR_SUCCESS)
			{
				hKeyQuero=CQToolbar::OpenQueroKey(hKeyUser,NULL,false);
				if(hKeyQuero)
				{
					DWORD type;
					DWORD size;

					size=sizeof DWORD;
					if(RegQueryValueEx(hKeyQuero,SETTINGS_VALUES[SETTINGS_VALUES_OPTIONS1],NULL,&type,(LPBYTE)&QueroOptions1,&size)!=ERROR_SUCCESS || type!=REG_DWORD)
						QueroOptions1=DEFAULT_OPTIONS;

					if(bUninstall)
					{
						SHDeleteValue(hKeyUser,L"Software\\Policies\\Microsoft\\Internet Explorer\\Toolbars\\Restrictions",L"NoNavBar");
						SHDeleteEmptyKey(hKeyUser,L"Software\\Policies\\Microsoft\\Internet Explorer\\Toolbars\\Restrictions");
						SHDeleteEmptyKey(hKeyUser,L"Software\\Policies\\Microsoft\\Internet Explorer\\Toolbars");
						SHDeleteEmptyKey(hKeyUser,L"Software\\Policies\\Microsoft\\Internet Explorer");
					}
					else if(QueroOptions1&OPTION_HideNavigationBar)
					{
						DWORD dwValue=1;
						SHSetValue(hKeyUser,L"Software\\Policies\\Microsoft\\Internet Explorer\\Toolbars\\Restrictions",L"NoNavBar",REG_DWORD,&dwValue,sizeof dwValue);
					}

					RegCloseKey(hKeyQuero);
				}
				RegCloseKey(hKeyUser);
			} // End open sub key
		} // End RegEnumKeyEx
		dwIndex++;
	} while(result==ERROR_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	HKEY hKey;
	UINT i;
	const TCHAR* QueroToolbarKeysToDelete[4]={L"Software",L"Software\\AppDataLow\\Software",L"Software\\Microsoft\\Internet Explorer\\InternetRegistry\\Software",L".DEFAULT\\Software"};

	// Unregister IE add-on

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Internet Explorer\\Toolbar", 0, KEY_WRITE, &hKey)==ERROR_SUCCESS)
	{
		RegDeleteValue(hKey, L"{A411D7F4-8D11-43EF-BDE4-AA921666388A}");
		RegCloseKey(hKey);
	}

	// Unregister Enhanced Protected Mode / AppContainerCompatible category

	RegisterCLSIDInCategory(CLSID_QueroBand,CATID_AppContainerCompatible,false);

	// Unset GPO NoNavBar

	UpdateAllUsers(true);

	// Delete "Quero Toolbar" Registry keys

	for(i=0;i<4;i++)
	{
		if(RegOpenKeyEx(i<3?HKEY_CURRENT_USER:HKEY_USERS, QueroToolbarKeysToDelete[i], 0, KEY_WRITE, &hKey)==ERROR_SUCCESS)
		{
			SHDeleteKey(hKey,L"Quero Toolbar");
			RegCloseKey(hKey);
		}
	}

	return _Module.UnregisterServer(TRUE);
}
