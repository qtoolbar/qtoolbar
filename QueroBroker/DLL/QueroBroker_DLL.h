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

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the QUEROBROKER_DLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// QUEROBROKER_DLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef QUEROBROKER_DLL_EXPORTS
#define QUEROBROKER_DLL_API __declspec(dllexport)
#else
#define QUEROBROKER_DLL_API __declspec(dllimport)
#endif

// Limit
#define MAX_IE_WINDOWS 128

// Mutex timeout
#define QMUTEX_TIMEOUT 8000 // Shared data time-out interval in milliseconds

// Subclass states
#define SUBCLASS_STATE_NOT_SUBCLASSED 0
#define SUBCLASS_STATE_FAILED 1
#define SUBCLASS_STATE_SUCCEEDED 2
#define SUBCLASS_STATE_REMOVED 3

// Quero Options
#define OPTION_WhiteQueroButton 0x01
#define OPTION_DownloadFavIcon 0x02
#define OPTION_SearchOnDNSFailure 0x04
#define OPTION_EnableQuickType 0x08
#define OPTION_NavigateOnACSelection 0x10 // Navigate on Auto Complete selection
#define OPTION_NavigateOnHistorySelection 0x20
#define OPTION_SearchCountry 0x40
#define OPTION_ACUseTab 0x80 // Use tab key to auto complete
//#define OPTION_SetFocusOnStartUp 0x100 removed, replaced by Quick Type
#define OPTION_SaveSearchHistory 0x200
#define OPTION_HighlightDomain 0x400
#define OPTION_DigitAsciiIndicator 0x800
#define OPTION_ShowFavIconsInTaskbar 0x1000
#define OPTION_ShowSearchEngineComboBox 0x2000
// QueroShorcutKey 0x4000 | 0x8000 | 0x10000 reserved
// NewWinTab_ContextMenu 0x20000 | 0x40000 | 0x80000 reserved
#define OPTION_EnableQuickFind 0x100000
#define OPTION_UseOpticalZoom 0x200000 // Use IE7's optical zoom feature instead of css based zoom
#define OPTION_SaveAddressHistory 0x400000
#define OPTION_RememberLastSearch 0x800000
#define OPTION_HideNavigationBar 0x1000000
// NewWinTab_SearchBox 0x2000000 | 0x4000000 | 0x8000000 reserved
// QuickFind_Activation 0x10000000 | 0x20000000
#define OPTION_DisableAdBlockerForLocalWebSites 0x40000000
#define OPTION_DisableAdBlockerForHttpsWebSites 0x80000000
#define OPTION_NewWinTab_SearchBox(Options) ((Options&0x6000000)>>25) // 0x2000000 | 4000000 | reserved 8000000
#define OPTION_NewWinTab_ContextMenu(Options) ((Options&0x60000)>>17) // 0x20000 | 40000 | reserved 80000
#define SetOPTION_NewWinTab_SearchBox(Options,id) Options=(((Options)&~(3<<25)) | ((id&0x3)<<25))
#define SetOPTION_NewWinTab_ContextMenu(Options,id) Options=(((Options)&~(3<<17)) | ((id&0x3)<<17))
#define OPTION_QueroShortcutKey(Options) ((Options&0xC000)>>14) // 0x4000 | 8000 | reserved 10000
#define SetOPTION_QueroShortcutKey(Options,id) Options=(((Options)&~(3<<14)) | ((id&0x3)<<14))
#define OPTION_QuickFind_Activation(Options) ((Options&0x30000000)>>28) // 0x10000000 | 0x20000000
#define SetOPTION_QuickFind_Activation(Options,id) Options=(((Options)&~(3<<28)) | ((id&0x3)<<28))
#define DEFAULT_OPTIONS (0xFFFFFF - 0x100 /* SetFocusOnStartUp */ - OPTION_ShowSearchEngineComboBox - OPTION_SearchCountry - OPTION_WhiteQueroButton)
#define OPTION2_ShowSearchBox 0x01
#define OPTION2_EnableQueroContextMenu 0x02
#define OPTION2_EnableAeroTheme 0x04
#define OPTION2_ShowSearchTermsWhileSearching 0x08
#define OPTION2_DisplayCertificateInformation 0x10
#define OPTION2_HideFavoritesButton 0x1000000
#define OPTION2_PageLoadingAnimation 0x2000000
#define OPTION2_HideToolbarCloseButton 0x4000000
#define OPTION2_HideAddressBox 0x8000000 // IE9: hide address box and navigation buttons
#define OPTION2_HideFlash 0x10000000
#define OPTION2_ShowCaption 0x20000000  // IE9: display page title in title bar
#define OPTION2_ShowCaption_SysColor 0x100  // Use theme color for caption, otherwise draw white text
#define OPTION2_AutoMaximize 0x40000000
#define OPTION2_DPI_Scaling 0x200
#define OPTION2_AddressNavigation 0x400
#define OPTION2_SearchPrompt 0x800
#define DEFAULT_OPTIONS2 0xFFFFFF

// WM_SETICON: Favicon
#define ICON_SMALL_FAVICON 666

// WM_SETTEXT: Title
#define WPARAM_WM_SETTEXT_DOCUMENT_TITLE 666

// Private Message Commands
#define WM_QUERO_TOOLBAR_COMMAND (WM_USER + 1031)
#define WM_QUERO_UPDATE_DWMTOPMARGIN 0xBFFF // Max WM_APP
#define WM_QUERO_UNSUBCLASS_IEFRAME 0xBFFE // Max WM_APP - 1
#define WM_QUERO_KEYBOARD_HOOK_IEFRAME (WM_USER + 1028)

// Quero Toolbar Commands
#define QUERO_COMMAND_FINDONPAGE 1
#define QUERO_COMMAND_HIDEFLASH 2
#define QUERO_COMMAND_SETBLOCKADS 3
#define QUERO_COMMAND_SETHIGHLIGHT 4
#define QUERO_COMMAND_SHOWQUEROMENU 5
#define QUERO_COMMAND_SETFOCUS_QEDITCTRL 6
#define QUERO_COMMAND_SETFOCUS_QTOOLBAR 7
#define QUERO_COMMAND_SETFOCUS_IESERVERWINDOW 8
#define QUERO_COMMAND_SHOW_OPERATION_LOCKED_ERROR 9
#define QUERO_COMMAND_VIEW_BLOCKED_CONTENT 10
#define QUERO_COMMAND_SETFOCUS_COMBOENGINE 11
#define QUERO_COMMAND_SHOWTOOLBAR 12
#define QUERO_COMMAND_UPDATE_DWMTOPMARGIN_DELAYED 13
#define QUERO_COMMAND_UPDATE_DWMTOPMARGIN 14
#define QUERO_COMMAND_IEFRAME_ACTIVATED 15
#define QUERO_COMMAND_UPDATE_QUERO_INSTANCE 16
#define QUERO_COMMAND_TOGGLE_AD_BLOCKER 17
#define QUERO_COMMAND_CERTIFICATE_DOWNLOAD_COMPLETE 18
#define QUERO_COMMAND_SETHIDEFLASH 19
#define QUERO_COMMAND_IEFRAME_CHANGED 20
#define QUERO_COMMAND_SETAUTOMAXIMIZE 21

// UpdateQueroInstance constants
#define UPDATE_NONE 0x0
#define UPDATE_BUTTONS 0x1
#define UPDATE_FONTSIZE 0x2
#define UPDATE_FONTCOLOR 0x4
#define UPDATE_LAYOUT 0x8
#define UPDATE_SHOWURL 0x10
#define UPDATE_QUICKFIND 0x20
#define UPDATE_SEARCHPROFILES 0x40
#define UPDATE_FREELASTHISTORYENTRY 0x80
#define UPDATE_QUERO_LOGO 0x100
#define UPDATE_AEROTHEME 0x200
#define UPDATE_SYNC_SETTINGS 0x400
#define UPDATE_TEMP_UNBLOCK 0x800
#define UPDATE_CUSTOMIZE_IEFRAME 0x1000
#define UPDATE_DISPLAY_CERTIFICATE_INFORMATION 0x2000
#define UPDATE_SHOW_CAPTION 0x4000
#define UPDATE_AUTO_MAXIMIZE 0x8000

// Registry Value Ids
#define REG_VALUE_HKCU_NONAVBAR 0
#define REG_VALUE_HKLM_NONAVBAR 1
#define N_REG_VALUES 2

// Data associated with each IE Window
typedef struct QWindowData
{
	HWND hwnd_IEFrame;
	DWORD ThreadId_IEFrame;
	HHOOK hHook_CBT;
	HHOOK hHook_Keyboard;
	INT DwnFrameTopMargin;
	BYTE Subclass_State;
	HWND hwnd_Caption;
	SHORT RefCount;
} QWindowData;

// Exported Functions
QUEROBROKER_DLL_API HRESULT DLL_Hook_IEFrame(HWND hwnd_IEFrame);
QUEROBROKER_DLL_API HRESULT DLL_Unhook_IEFrame(HWND hwnd_IEFrame);
QUEROBROKER_DLL_API HRESULT DLL_Customize_IEFrame(HWND hwnd_IEFrame, UINT Options, UINT Options2);
QUEROBROKER_DLL_API HWND DLL_GetReBar(HWND hwnd_IEFrame, TCHAR *pClass, TCHAR *pWindow);

QUEROBROKER_DLL_API void DLL_SetOptions(UINT Options, UINT Options2, UINT UpdateQueroInstances);
QUEROBROKER_DLL_API HRESULT DLL_SetDwmFrameTopMargin(HWND hwnd_IEFrame, INT NewDwmTopMargin);
QUEROBROKER_DLL_API INT DLL_GetDwmFrameTopMargin(HWND hwnd_IEFrame);
QUEROBROKER_DLL_API void DLL_Set_IE_MajorVersion(INT IE_MajorVersion);
QUEROBROKER_DLL_API INT DLL_Get_IE_MajorVersion();
QUEROBROKER_DLL_API void DLL_SetWindowsVersion(INT WindowsVersion);
QUEROBROKER_DLL_API INT DLL_GetWindowsVersion();
QUEROBROKER_DLL_API void DLL_SetIcon(HWND hwnd_IEFrame, HICON hIcon);
QUEROBROKER_DLL_API bool DLL_Is_IEFrame(HWND hwnd_IEFrame);

// Internal Functions
void PostMessageToAllQueroInstances(UINT uMsg,WPARAM wParam,LPARAM lParam,bool bSendMessage);
LRESULT PostMessageToQueroToolbar(HWND hwnd_IEFrame,UINT uMsg,WPARAM wParam,LPARAM lParam,bool bSendMessage,bool bToAllInstances);
LRESULT CALLBACK CBTProc_IEFrame(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SubClassProc_IEFrame(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK SubClassProc_Caption(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK KeyboardProc_IEFrame(int nCode, WPARAM wParam, LPARAM lParam);
void DrawCaption_IE9(HWND hwnd_IEFrame, HWND hwnd_Caption);