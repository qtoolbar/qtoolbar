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

// QueroBroker_DLL.cpp : Defines the entry point for the DLL application.

#include "stdafx.h"
#include "QueroBroker_DLL.h"
#include <commctrl.h>
#include <winuser.h>
#include <shlwapi.h>

#ifdef COMPILE_FOR_WINDOWS_VISTA
#include <dwmapi.h>
#include <uxtheme.h>
#include <vssym32.h>
#endif

#ifdef _MANAGED
#pragma managed(push, off)
#endif

// Quero Debug Console
#define AFX_STDAFX_H__1E6B8A8B_FF2B_4335_9513_7BB74B9BDDD3__INCLUDED_
#include "..\..\Quero_x64\QDebug.h"
#include "..\..\Quero_x64\QDebug.cpp"

// Handle to the DLL module
HMODULE g_hModule_QueroBroker_DLL;

// Shared data mutex
HANDLE g_hQueroBrokerMutex;

// Shared data among all instances of the QueroBroker DLL
#pragma data_seg (".QueroBrokerData")
INT g_IE_MajorVersion=0;
INT g_WindowsVersion=0;
HICON g_IE_Icon=NULL;
UINT g_Options=DEFAULT_OPTIONS;
UINT g_Options2=DEFAULT_OPTIONS2;
int g_MaxUsedWindowDataIndex=-1;
QWindowData g_IE_WindowData[MAX_IE_WINDOWS]={};
#pragma data_seg ()
#pragma comment(linker,"/SECTION:.QueroBrokerData,RWS") // Tell linker to make it shared

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_hModule_QueroBroker_DLL=hModule;
		DisableThreadLibraryCalls(hModule);
		g_hQueroBrokerMutex=CreateMutex(NULL,FALSE,L"QueroBrokerMutex");
		// Workaround: prevent module from unloading when hook is removed from iexplore.exe
		// prevents random crashes when unsubclassing the IE frame window and removing the hooks afterwards
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_PIN|GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,(LPCTSTR)&DllMain,&hModule);		
		break;

	case DLL_PROCESS_DETACH:
		if(g_hQueroBrokerMutex) CloseHandle(g_hQueroBrokerMutex);
		break;
	}

    return TRUE;
}

QUEROBROKER_DLL_API bool DLL_Is_IEFrame(HWND hwnd_IEFrame)
{
	bool result;
	TCHAR className[32];

	if(GetClassName(hwnd_IEFrame,className,32))
	{
		result = (StrCmp(className,L"IEFrame")==0);
	}
	else result=false;

	return result;
}

QUEROBROKER_DLL_API void DLL_SetOptions(UINT Options, UINT Options2, UINT UpdateQueroInstances)
{
	int i;

	QD(L"DLL_SetOptions");

	g_Options=Options;
	g_Options2=Options2;

	if(UpdateQueroInstances&(UPDATE_CUSTOMIZE_IEFRAME|UPDATE_SHOW_CAPTION|UPDATE_AUTO_MAXIMIZE))
	{
		i=0;
		while(i<=g_MaxUsedWindowDataIndex)
		{
			if(g_IE_WindowData[i].hwnd_IEFrame)
			{
				if(UpdateQueroInstances&UPDATE_CUSTOMIZE_IEFRAME) DLL_Customize_IEFrame(g_IE_WindowData[i].hwnd_IEFrame,Options,Options2);
				if(UpdateQueroInstances&UPDATE_SHOW_CAPTION) if(g_IE_WindowData[i].hwnd_Caption) InvalidateRect(g_IE_WindowData[i].hwnd_Caption,NULL,TRUE);
				if((UpdateQueroInstances&UPDATE_AUTO_MAXIMIZE) && (g_Options2&OPTION2_AutoMaximize)) ShowWindow(g_IE_WindowData[i].hwnd_IEFrame,SW_MAXIMIZE);
			}
			i++;
		}
		UpdateQueroInstances&=~(UPDATE_CUSTOMIZE_IEFRAME|UPDATE_SHOW_CAPTION|UPDATE_AUTO_MAXIMIZE);
	}

	if(UpdateQueroInstances) PostMessageToAllQueroInstances(WM_QUERO_TOOLBAR_COMMAND,QUERO_COMMAND_UPDATE_QUERO_INSTANCE,(LPARAM)UpdateQueroInstances,false);
}

QUEROBROKER_DLL_API void DLL_Set_IE_MajorVersion(INT IE_MajorVersion)
{
	g_IE_MajorVersion=IE_MajorVersion;
}

QUEROBROKER_DLL_API INT DLL_Get_IE_MajorVersion()
{
	return g_IE_MajorVersion;
}

QUEROBROKER_DLL_API void DLL_SetWindowsVersion(INT WindowsVersion)
{
	g_WindowsVersion=WindowsVersion;
}

QUEROBROKER_DLL_API INT DLL_GetWindowsVersion()
{
	return g_WindowsVersion;
}

QWindowData* GetFreeWindowData()
{
	QWindowData *pWindowData;
	int i;

	i=0;
	pWindowData=g_IE_WindowData;
	while(i<=g_MaxUsedWindowDataIndex && pWindowData->hwnd_IEFrame!=NULL && i<MAX_IE_WINDOWS)
	{
		i++;
		pWindowData++;
	}
	if(i<MAX_IE_WINDOWS)
	{
		if(i>g_MaxUsedWindowDataIndex) g_MaxUsedWindowDataIndex=i;
	}
	else pWindowData=NULL;

	return pWindowData;
}

void RemoveWindowData(QWindowData* pWindowData)
{
	SIZE_T i;

	i=pWindowData-g_IE_WindowData;
	if(i<MAX_IE_WINDOWS)
	{
		pWindowData->hwnd_IEFrame=NULL;
		pWindowData->ThreadId_IEFrame=0;
		if((int)i==g_MaxUsedWindowDataIndex) g_MaxUsedWindowDataIndex--;
	}
}

QWindowData* GetWindowDataByHwnd(HWND hwnd_IEFrame)
{
	int i;
	QWindowData *pWindowData;

	i=0;
	pWindowData=g_IE_WindowData;
	while(i<=g_MaxUsedWindowDataIndex && pWindowData->hwnd_IEFrame!=hwnd_IEFrame)
	{
		i++;
		pWindowData++;
	}
	if(i>g_MaxUsedWindowDataIndex) pWindowData=NULL;

	return pWindowData;
}

QWindowData* GetWindowDataByCurrentThread()
{
	int i;
	QWindowData *pWindowData;
	DWORD ThreadId;

	ThreadId=GetCurrentThreadId();

	i=0;
	pWindowData=g_IE_WindowData;
	while(i<=g_MaxUsedWindowDataIndex && pWindowData->ThreadId_IEFrame!=ThreadId)
	{
		i++;
		pWindowData++;
	}
	if(i>g_MaxUsedWindowDataIndex) pWindowData=NULL;

	return pWindowData;
}

void PostMessageToAllQueroInstances(UINT uMsg,WPARAM wParam,LPARAM lParam,bool bSendMessage)
{
	int i=0;

	while(i<=g_MaxUsedWindowDataIndex)
	{
		if(g_IE_WindowData[i].hwnd_IEFrame)
		{
			PostMessageToQueroToolbar(g_IE_WindowData[i].hwnd_IEFrame,uMsg,wParam,lParam,bSendMessage,true);
		}
		i++;
	}
}

LRESULT PostMessageToQueroToolbar(HWND hwnd_IEFrame,UINT uMsg,WPARAM wParam,LPARAM lParam,bool bSendMessage,bool bToAllInstances)
{
	LRESULT result;
	HWND hwnd_FrameTab;
	HWND hwnd_InternetToolbarHost;
	HWND hwnd_MenuBar;
	HWND hwnd_ReBar;
	HWND hwnd_QToolbar;

	result=E_FAIL;
	hwnd_FrameTab=NULL;
	hwnd_InternetToolbarHost=NULL;
	do
	{
		if(g_IE_MajorVersion>=8)
		{
			hwnd_FrameTab=FindWindowEx(hwnd_IEFrame,hwnd_FrameTab,L"Frame Tab",NULL);
			hwnd_InternetToolbarHost=NULL;
		}
		else hwnd_FrameTab=hwnd_IEFrame;
		if(hwnd_FrameTab)
		{
			hwnd_InternetToolbarHost=FindWindowEx(hwnd_FrameTab,hwnd_InternetToolbarHost,L"InternetToolbarHost",NULL);
			if(hwnd_InternetToolbarHost)
			{
				hwnd_MenuBar=FindWindowEx(hwnd_InternetToolbarHost,NULL,L"WorkerW",NULL);
				if(hwnd_MenuBar)
				{
					hwnd_ReBar=FindWindowEx(hwnd_MenuBar,NULL,L"ReBarWindow32",NULL);
					if(hwnd_ReBar)
					{
						hwnd_QToolbar=FindWindowEx(hwnd_ReBar,NULL,L"QTOOLBAR",NULL);
						if(hwnd_QToolbar)
						{
							// Post message to all instances or to active instance only
							if(bToAllInstances || IsWindowVisible(hwnd_QToolbar))
							{
								if(bSendMessage)
									result=SendMessage(hwnd_QToolbar,uMsg,wParam,lParam);
								else
									if(PostMessage(hwnd_QToolbar,uMsg,wParam,lParam)) result=S_OK;

								if(bToAllInstances==false) break;
							}
						} // End hwnd_QToolbar found
					} // End hwnd_ReBar found
				} // End hwnd_MenuBar found
			} // End hwnd_InternetToolbarHost found
		} // End hwnd_FrameTab found
	} while(hwnd_InternetToolbarHost);

	return result;
}

QUEROBROKER_DLL_API HRESULT DLL_Hook_IEFrame(HWND hwnd_IEFrame)
{
	HRESULT result;
	QWindowData *pWindowData;

	result=E_FAIL;

	if(WaitForSingleObject(g_hQueroBrokerMutex,QMUTEX_TIMEOUT)==WAIT_OBJECT_0)
	{
		// Check if hwnd is already hooked
		pWindowData=GetWindowDataByHwnd(hwnd_IEFrame);
		if(pWindowData==NULL)
		{
			pWindowData=GetFreeWindowData();
			if(pWindowData)
			{
				pWindowData->hwnd_IEFrame=hwnd_IEFrame;
				pWindowData->ThreadId_IEFrame=GetWindowThreadProcessId(hwnd_IEFrame,NULL);
				pWindowData->Subclass_State=SUBCLASS_STATE_NOT_SUBCLASSED;
				pWindowData->DwnFrameTopMargin=0;
				if(g_IE_MajorVersion>=9)
				{
					pWindowData->hwnd_Caption=FindWindowEx(pWindowData->hwnd_IEFrame,NULL,L"Client Caption",NULL);
				}
				else pWindowData->hwnd_Caption=NULL;
				pWindowData->RefCount=1;

				if(g_Options2&OPTION2_AutoMaximize)
				{
					ShowWindow(hwnd_IEFrame,SW_MAXIMIZE);
				}

				QDEBUG_PRINTF(L"QueroBroker_DLL",L"threadid %x",pWindowData->ThreadId_IEFrame);

				pWindowData->hHook_CBT=SetWindowsHookEx(WH_CBT,CBTProc_IEFrame,g_hModule_QueroBroker_DLL,pWindowData->ThreadId_IEFrame);
				pWindowData->hHook_Keyboard=SetWindowsHookEx(WH_KEYBOARD,KeyboardProc_IEFrame,g_hModule_QueroBroker_DLL,pWindowData->ThreadId_IEFrame);

				if(pWindowData->hHook_CBT)
				{
					result=S_OK;
					QDEBUG_PRINT(L"QueroBroker_DLL",L"Hook successful");
				}
			}
		}
		else pWindowData->RefCount++;
		ReleaseMutex(g_hQueroBrokerMutex);
	}
	QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"DLL_Hook_IEFrame");

	return result;
}

QUEROBROKER_DLL_API HRESULT DLL_Unhook_IEFrame(HWND hwnd_IEFrame)
{
	HRESULT result;
	QWindowData *pWindowData;

	result=E_FAIL;

	if(WaitForSingleObject(g_hQueroBrokerMutex,QMUTEX_TIMEOUT)==WAIT_OBJECT_0)
	{
		pWindowData=GetWindowDataByHwnd(hwnd_IEFrame);
		if(pWindowData)
		{
			pWindowData->RefCount--;
			if(pWindowData->RefCount==0)
			{
				if(pWindowData->Subclass_State==SUBCLASS_STATE_SUCCEEDED) SendMessage(hwnd_IEFrame,WM_QUERO_UNSUBCLASS_IEFRAME,0xFFFFFFFF,0xFFFFFFFF);
				if(pWindowData->hHook_Keyboard) UnhookWindowsHookEx(pWindowData->hHook_Keyboard);
				if(pWindowData->hHook_CBT) UnhookWindowsHookEx(pWindowData->hHook_CBT);
				RemoveWindowData(pWindowData);

				// All IE windows closed?
				if(g_MaxUsedWindowDataIndex==-1)
				{
					// Free default IE frame window icon
					if(g_IE_Icon)
					{
						DestroyIcon(g_IE_Icon);
						g_IE_Icon=NULL;
					}
				}
			}
			result=S_OK;
		}
		ReleaseMutex(g_hQueroBrokerMutex);
	}
	QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"DLL_Unhook_IEFrame");

	return result;
}

QUEROBROKER_DLL_API HRESULT DLL_Customize_IEFrame(HWND hwnd_IEFrame, UINT Options, UINT Options2)
{
	HWND hwnd_ReBar;
	BOOL bShow;

	// Navigation Bar
	hwnd_ReBar=DLL_GetReBar(hwnd_IEFrame,L"WorkerW",NULL);
	if(hwnd_ReBar)
	{
		bShow = (Options2&OPTION2_HideAddressBox)==0;
		SendMessage(hwnd_ReBar,RB_SHOWBAND,0,bShow);
		// IE11: ExtendFrameIntoClientArea to prevent black line
		#ifdef COMPILE_FOR_WINDOWS_VISTA
		if(g_IE_MajorVersion>=11 && DLL_GetDwmFrameTopMargin(hwnd_IEFrame)==0 && (g_Options2&OPTION2_ShowSearchBox))
		{
			RECT rect_ReBar;
			MARGINS margins;

			GetWindowRect(hwnd_ReBar,&rect_ReBar);
			MapWindowPoints(hwnd_ReBar,hwnd_IEFrame,(LPPOINT)&rect_ReBar,2);
			margins.cxLeftWidth=0;
			margins.cxRightWidth=0;
			margins.cyBottomHeight=0;
			margins.cyTopHeight=rect_ReBar.bottom;
			DwmExtendFrameIntoClientArea(hwnd_IEFrame,&margins);
		}
		#endif
		SendMessage(hwnd_ReBar,RB_SHOWBAND,1,bShow);
	}

	// Command Bar
	if(g_IE_MajorVersion==8) // IE8: OPTION2_HideFavoritesButton
	{
		hwnd_ReBar=DLL_GetReBar(hwnd_IEFrame,L"CommandBarClass",NULL);
		if(hwnd_ReBar)
		{
			SendMessage(hwnd_ReBar,RB_SHOWBAND,0,(Options2&OPTION2_HideFavoritesButton)==0);
		}
	}

	return S_OK;
}

QUEROBROKER_DLL_API HWND DLL_GetReBar(HWND hwnd_IEFrame, TCHAR *pClass, TCHAR *pWindow)
{
	HWND hwnd_Parent;
	HWND hwnd_ReBar;

	hwnd_ReBar=NULL;

	hwnd_Parent=FindWindowEx(hwnd_IEFrame,NULL,pClass,pWindow);
	if(hwnd_Parent)
	{
		hwnd_ReBar=FindWindowEx(hwnd_Parent,NULL,L"ReBarWindow32",NULL);
	}
	else // Theater mode?
	{
		hwnd_IEFrame=FindWindowEx(hwnd_IEFrame,NULL,L"WorkerW",NULL);
		if(hwnd_IEFrame) hwnd_ReBar=DLL_GetReBar(hwnd_IEFrame,pClass,pWindow);
	}

	return hwnd_ReBar;
}

QUEROBROKER_DLL_API HRESULT DLL_SetDwmFrameTopMargin(HWND hwnd_IEFrame,INT NewDwmTopMargin)
{
	HRESULT result;
	QWindowData *pWindowData;

	pWindowData=GetWindowDataByHwnd(hwnd_IEFrame);
	if(pWindowData)
	{
		if(NewDwmTopMargin!=pWindowData->DwnFrameTopMargin)
		{
			result=S_OK;
			pWindowData->DwnFrameTopMargin=NewDwmTopMargin;
		}
		else result=S_FALSE;
	}
	else result=E_FAIL;

	return result;
}

QUEROBROKER_DLL_API INT DLL_GetDwmFrameTopMargin(HWND hwnd_IEFrame)
{
	QWindowData *pWindowData;

	pWindowData=GetWindowDataByHwnd(hwnd_IEFrame);
	return pWindowData?pWindowData->DwnFrameTopMargin:0;
}

QUEROBROKER_DLL_API void DLL_SetIcon(HWND hwnd_IEFrame, HICON hIcon)
{
	HICON hIconCopy;
	HICON hPrevIcon;

	if(WaitForSingleObject(g_hQueroBrokerMutex,QMUTEX_TIMEOUT)==WAIT_OBJECT_0)
	{
		if(hIcon==NULL) hIcon=g_IE_Icon;

		hIconCopy=CopyIcon(hIcon);
		if(hIconCopy)
		{
			hPrevIcon=(HICON)SendMessage(hwnd_IEFrame,WM_SETICON,ICON_SMALL_FAVICON,(LPARAM)hIconCopy);
			if(hPrevIcon)
			{
				if(g_IE_Icon==NULL) g_IE_Icon=CopyIcon(hPrevIcon);
				DestroyIcon(hPrevIcon);
			}
		}

		ReleaseMutex(g_hQueroBrokerMutex);
	}
	QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"DLL_SetIcon");
}

LRESULT CALLBACK CBTProc_IEFrame(int nCode, WPARAM wParam, LPARAM lParam)
{
	QWindowData *pWindowData;

	pWindowData=GetWindowDataByCurrentThread();
	if(pWindowData)
	{
		if(pWindowData->Subclass_State==SUBCLASS_STATE_NOT_SUBCLASSED)
		{
			if(SetWindowSubclass(pWindowData->hwnd_IEFrame,SubClassProc_IEFrame,0,(DWORD_PTR)pWindowData))
			{
				pWindowData->Subclass_State=SUBCLASS_STATE_SUCCEEDED;
				QDEBUG_PRINT(L"QueroBroker_DLL",L"subclass succeeded");
				// Subclass IE9 caption window
				if(pWindowData->hwnd_Caption)
				{
					SetWindowSubclass(pWindowData->hwnd_Caption,SubClassProc_Caption,0,(DWORD_PTR)pWindowData);
					// Update caption
					InvalidateRect(pWindowData->hwnd_Caption,NULL,FALSE);
				}
			}
			else pWindowData->Subclass_State=SUBCLASS_STATE_FAILED;
		}
	}

	return CallNextHookEx(NULL,nCode,wParam,lParam);
}

LRESULT CALLBACK SubClassProc_IEFrame(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	LRESULT result;
	bool bCallDefSubclassProc;
	
	result=0;
	bCallDefSubclassProc=true;

	//QDEBUG_PRINTF(L"SubClassProc_IEFrame",L"%x %x %x",uMsg,wParam,lParam);

	switch(uMsg)
	{
	case WM_SETICON:
		if(wParam==ICON_SMALL_FAVICON)
		{
			wParam=ICON_SMALL;
		}
		else if(wParam==ICON_SMALL && (g_Options&OPTION_ShowFavIconsInTaskbar))
		{
			bCallDefSubclassProc=false;
		}
		break;

	case WM_SETTEXT:
		bCallDefSubclassProc=((g_Options&OPTION_HideNavigationBar)==0 || wParam==WPARAM_WM_SETTEXT_DOCUMENT_TITLE);
		// Update caption
		if((g_Options2&OPTION2_ShowCaption)!=0 && bCallDefSubclassProc && ((QWindowData*)dwRefData)->hwnd_Caption) InvalidateRect(((QWindowData*)dwRefData)->hwnd_Caption,NULL,FALSE);
		break;

	case WM_ACTIVATE:
		PostMessageToQueroToolbar(hWnd,WM_QUERO_TOOLBAR_COMMAND,QUERO_COMMAND_IEFRAME_ACTIVATED,0,false,false);
		// Update caption
		if((g_Options2&OPTION2_ShowCaption)!=0 && ((QWindowData*)dwRefData)->hwnd_Caption) InvalidateRect(((QWindowData*)dwRefData)->hwnd_Caption,NULL,FALSE);
		break;

	case 0x031E: // WM_DWMCOMPOSITIONCHANGED:
		// Delay reaction to ensure that Quero sets frame margins after IE does
		result=DefSubclassProc(hWnd,uMsg,wParam,lParam);
		PostMessage(hWnd,WM_QUERO_UPDATE_DWMTOPMARGIN,0xFFFFFFFF,0xFFFFFFFF);
		bCallDefSubclassProc=false;
		break;

	case WM_QUERO_UPDATE_DWMTOPMARGIN:
		if(wParam==0xFFFFFFFF && lParam==0xFFFFFFFF)
		{
			PostMessageToQueroToolbar(hWnd,WM_QUERO_TOOLBAR_COMMAND,QUERO_COMMAND_UPDATE_DWMTOPMARGIN,(LPARAM)hWnd,false,false);
			bCallDefSubclassProc=false;
		}
		break;

	case WM_QUERO_UNSUBCLASS_IEFRAME:
		if(wParam==0xFFFFFFFF && lParam==0xFFFFFFFF)
		{
			QDEBUG_PRINT(L"QueroBroker_DLL",L"WM_QUERO_UNSUBCLASS_IEFRAME");
			if(((QWindowData*)dwRefData)->hwnd_Caption) RemoveWindowSubclass(((QWindowData*)dwRefData)->hwnd_Caption,SubClassProc_Caption,0);
			result=(LRESULT)RemoveWindowSubclass(hWnd,SubClassProc_IEFrame,0);
			bCallDefSubclassProc=false;
		}
		break;

	// IE11, IE10 on Windows 8: exit video full screen mode
	case 0x8066:
		QDEBUG_PRINT(L"QueroBroker_DLL",L"exit full screen");
		if(g_Options2&OPTION2_HideAddressBox) DLL_Customize_IEFrame(hWnd,g_Options,g_Options2);
		PostMessage(hWnd,WM_QUERO_UPDATE_DWMTOPMARGIN,0xFFFFFFFF,0xFFFFFFFF);
		break;
	}

	// Extend frame into client area on certain events (Favorite center pinned, exit full screen, ...)
	#ifdef COMPILE_FOR_WINDOWS_VISTA
	if(uMsg>=0xC000 && (g_Options2&OPTION2_EnableAeroTheme)) // "ISO_MSG_WM_MSG" (RegisterWindowMessage does not return correct id
	{
		//QDEBUG_PRINTF(L"0xC000",L"%x %x %x",uMsg,wParam,lParam);
		if(wParam==0xBDA || wParam==0xC0A) // IE8 Beta, RC1 0x478
		{
			QDEBUG_PRINT(L"ISO_MSG_WM_MSG",L"DwmExtendFrameIntoClientArea");
			MARGINS margins;
			margins.cxLeftWidth=0;
			margins.cxRightWidth=0;
			margins.cyBottomHeight=0;
			margins.cyTopHeight=DLL_GetDwmFrameTopMargin(hWnd);
			if(margins.cyTopHeight!=0) DwmExtendFrameIntoClientArea(hWnd,&margins);
		}
	}
	#endif

	if(bCallDefSubclassProc) result=DefSubclassProc(hWnd,uMsg,wParam,lParam);

	return result;
}

LRESULT CALLBACK SubClassProc_Caption(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	LRESULT result;
	bool bCallDefSubclassProc;
	
	result=0;
	bCallDefSubclassProc=true;

	#ifdef COMPILE_FOR_WINDOWS_VISTA
	if((g_Options2&OPTION2_ShowCaption)!=0)
	{
		switch(uMsg)
		{
		case WM_PAINT:
			DrawCaption_IE9(((QWindowData*)dwRefData)->hwnd_IEFrame,hWnd);
			bCallDefSubclassProc=(IsCompositionActive()==FALSE);
			break;
		case WM_ERASEBKGND:
			bCallDefSubclassProc=false;
			break;
		}
	}
	#endif
	
	if(bCallDefSubclassProc) result=DefSubclassProc(hWnd,uMsg,wParam,lParam);

	return result;
}

void DrawCaption_IE9(HWND hwnd_IEFrame, HWND hwnd_Caption)
{
	// included uxtheme.lib
	#ifdef COMPILE_FOR_WINDOWS_VISTA
	HDC hDC;
	TCHAR title[255];
	int title_length;
	HTHEME hTheme;
	DTTOPTS dttopts;
	NONCLIENTMETRICS nc_metrics;
	HFONT hFont_Caption;
	int padding_top;
	int padding_right;
	int logpixelsy;
	RECT rect;
	RECT rect_text;
	bool bActiveWindow;
	bool bCompositionActive;

	if(g_IE_MajorVersion>=9)
	{
		if(IsWindowVisible(hwnd_Caption))
		{
			bCompositionActive=IsCompositionActive()==TRUE;
			bActiveWindow=GetForegroundWindow()==hwnd_IEFrame;
			title_length=GetWindowText(hwnd_IEFrame,title,255);
			if(title_length>0)
			{
				hDC=GetDC(hwnd_Caption);
				if(hDC)
				{
					QD(L"DrawCaption");
					GetClientRect(hwnd_Caption,&rect);
					//QDEBUG_PRINTF(L"DrawCaption rect",L"%d %d %d %d",rect.left,rect.top,rect.right,rect.bottom);
					rect_text=rect;
					nc_metrics.cbSize=sizeof nc_metrics;
					SystemParametersInfo(SPI_GETNONCLIENTMETRICS,sizeof nc_metrics,&nc_metrics,0);

					if(IsZoomed(hwnd_IEFrame))
					{
						padding_top=(g_WindowsVersion>=0x0602)?11:12;
						rect_text.left+=4;
					}
					else
					{
						if(g_WindowsVersion>=0x0602) padding_top=bCompositionActive?3:11;
						else padding_top=bCompositionActive?4:12;
						rect_text.left+=2;
					}
					if((g_Options2&OPTION2_HideAddressBox)==0) padding_top--;
					nc_metrics.lfCaptionFont.lfHeight+=(g_WindowsVersion>=0x0602)?2:1;

					// Reserve space for window buttons (minimize, maximize, close)
					padding_right=102;

					// Scale padding
					logpixelsy=GetDeviceCaps(hDC,LOGPIXELSY);
					if(logpixelsy!=96)
					{
						if(logpixelsy>=192)
						{
							if(padding_top>8) padding_top=MulDiv(padding_top-4,logpixelsy,96)+4;
							else padding_top=MulDiv(padding_top,logpixelsy,96)-1;
						}
						padding_right=MulDiv(padding_right,logpixelsy,96);
					}

					rect_text.top+=padding_top;
					rect_text.right-=padding_right;
					hFont_Caption=CreateFontIndirect(&nc_metrics.lfCaptionFont);
					if(hFont_Caption)
					{
						HDC hMemDC;
						HBITMAP hBitmap,hBitmapOld;
						BITMAPINFO dib = {0};
						hMemDC=CreateCompatibleDC(hDC);
						if(hMemDC)
						{
							dib.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
							dib.bmiHeader.biWidth = rect.right;
							dib.bmiHeader.biHeight = -rect.bottom;
							dib.bmiHeader.biPlanes = 1;
							dib.bmiHeader.biBitCount = 32;
							dib.bmiHeader.biCompression = BI_RGB;

							hBitmap=CreateDIBSection(hDC,&dib,DIB_RGB_COLORS,NULL,NULL,0);
							hBitmapOld=(HBITMAP)SelectObject(hMemDC,hBitmap);

							SelectObject(hMemDC,hFont_Caption);

							// Center on Windows 8
							if(g_WindowsVersion>=0x0602)
							{
								SIZE size_title;
								LONG caption_width;

								GetTextExtentPoint32(hMemDC,title,title_length,&size_title);
								//QDEBUG_PRINTF(L"extent",L"%d",size_title.cx);
								caption_width=rect.right-rect.left;

								if(caption_width-size_title.cx>200)
								{
									rect_text.left=(caption_width-size_title.cx)/2;
								}
								else if(caption_width-size_title.cx>100)
								{
									rect_text.left=caption_width-size_title.cx-100;
								}
							}

							hTheme=OpenThemeData(hwnd_Caption,L"WINDOW");
							if(hTheme)
							{
								if(bCompositionActive)
								{
									FillRect(hMemDC,&rect,(HBRUSH)GetStockObject(BLACK_BRUSH));
								}
								else
								{
									rect.left-=8;
									DrawThemeBackground(hTheme,hMemDC,WP_CAPTION,bActiveWindow?CS_ACTIVE:CS_INACTIVE,&rect,NULL);
									rect.left+=8;
								}

								dttopts.dwSize=sizeof dttopts;
								dttopts.dwFlags = DTT_COMPOSITED|DTT_GLOWSIZE|DTT_TEXTCOLOR;
								// Add text glow on Windows 7
								dttopts.iGlowSize = (g_WindowsVersion>=0x0602)?0:16;
								if(g_Options2&OPTION2_ShowCaption_SysColor)
								{
									dttopts.crText=GetThemeSysColor(hTheme,bActiveWindow?COLOR_CAPTIONTEXT:COLOR_INACTIVECAPTIONTEXT);
								}
								else
								{
									dttopts.crText=0xFFFFFF;
								}

								DrawThemeTextEx(hTheme,hMemDC,WP_CAPTION,bActiveWindow?CS_ACTIVE:CS_INACTIVE,title,title_length,DT_LEFT|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS,&rect_text,&dttopts);

								if(bCompositionActive==false) rect.right-=100;

								BitBlt(hDC,0,0,rect.right,rect.bottom,hMemDC,0,0,SRCCOPY);

								ValidateRect(hwnd_Caption,&rect);

								CloseThemeData(hTheme);
							}
							SelectObject(hMemDC,hBitmapOld);
							DeleteObject(hBitmap);
							DeleteDC(hMemDC);
						} // End CreateCompatibleDC
						DeleteObject(hFont_Caption);
					} // End CreateFontIndirect
					ReleaseDC(hwnd_Caption,hDC);
				} // End GetDC
			} // End GetWindowText
		} // End caption window visible
	} // End >= IE9
	#endif
}

LRESULT CALLBACK KeyboardProc_IEFrame(int nCode, WPARAM wParam, LPARAM lParam)
{
	QWindowData *pWindowData;
	bool bPostMessage;
	HWND hwnd;
	TCHAR className[32];

	pWindowData=GetWindowDataByCurrentThread();
	if(pWindowData)
	{
		if(nCode==HC_ACTION)
		{
			if((lParam&0x80000000)==0) // Key pressed?
			{
				// Post keyboard msg if focus is in the New Tab Window or the IE address bar has focus and is hidden (IE10)
				hwnd=GetParent(GetFocus());
				if(GetClassName(hwnd,className,32))
				{
					bPostMessage = ((StrCmp(className,L"NewTabWnd")==0) ||
						(StrCmp(className,L"Address Band Root")==0 && IsWindowVisible(hwnd)==0));
				}

				// Post keyboard msg if CTRL, ALT or F3/F2 key is pressed regardless of focus
				if(bPostMessage==false)
				{
					bPostMessage=(GetKeyState(VK_CONTROL)<0 || GetKeyState(VK_MENU)<0 || wParam==VK_F3 || wParam==VK_F2);
				}
				
				// Post keyboard msg to Quero Toolbar
				if(bPostMessage)
				{
					if(PostMessageToQueroToolbar(pWindowData->hwnd_IEFrame,WM_QUERO_KEYBOARD_HOOK_IEFRAME,wParam,lParam,true,false))
						return 1;
				}
			}
		}
	}

	return CallNextHookEx(NULL,nCode,wParam,lParam);
}