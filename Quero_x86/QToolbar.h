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

// QToolbar.h : Declaration of the CQToolbar

#ifndef __QTOOLBAR_H_
#define __QTOOLBAR_H_

#define SUCCEEDED_OK(x) ((x)==S_OK)

#include <commctrl.h>
#include <mshtml.h>
#include <time.h>
#include <strsafe.h>
#include "QDebug.h"
#include "ComboQuero.h"
#include "ComboEngine.h"
#include "NavBar.h"
#include "LogoToolbar.h"
#include "ButtonBar.h"
#include "AutoComplete.h"
#include "Profiles.h"
#include "IDNA.h"
#include "IconAnimation.h"
#include "FavIcon.h"
#include "RedirectRequest.h"
#include "CertificateDownloader.h"
#ifdef COMPILE_FOR_WINDOWS_VISTA
#include "AeroReBar.h"
#endif

#include "..\QueroBroker\QueroBroker.h"

// Quero version

#define QUERO_VERSION 0x08000000 // 8.0.0.0

// Index

#define INDEX_UNDEFINED 0xFFFFFFFF
#define INDEX_UNDEFINED_INT -1

// Limits

#define HISTORYSIZE 512
#define URLHISTORYSIZE 2048
#define WHITELISTSIZE 512
#define BLOCKEDCONTENTSIZE 16
#define MAX_STRING_LENGTH 255

// Types

#define TYPE_UNKNOWN 0
#define TYPE_SEARCH 1
#define TYPE_ADDRESS 2

// Font sizes and colors

#define FONTSIZE_STANDARD 0
#define FONTSIZE_MEDIUM 2
#define FONTSIZE_LARGE 4

#define FONTCOLOR_BLACK 0
#define FONTCOLOR_GREENBLUE 1
#define FONTCOLOR_PINK 2
#define N_COLOR_SCHEMES 3

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
#define UPDATE_QUERO_CONTEXT_MENU 0x10000

// Dimensions

#define NAVBUTTONSIZE_DEFAULT 24
#define BUTTONSIZE_DEFAULT 20
#define ICONSIZE_DEFAULT 16
#define QUEROLOGOX 52
#define QUEROLOGOY 20
#define PADDINGY_UNKNOWN -1
#define MIN_ENGINEWIDTH 100
#define MAX_ENGINEWIDTH 256
#define MAX_CERTIFICATE_ORGANIZATION_EXTENT 500
#define MIN_EDIT_BOX_WIDTH 100

// High DPI
#define DPI_SCALEX(x) MulDiv(x,LogPixelsX,96)
#define DPI_SCALEY(y) MulDiv(y,LogPixelsY,96)

// Quero Settings Registry Values

#define SETTINGS_VALUES_COUNT 16
#define SETTINGS_VALUES_HIGHLIGHT 0
#define SETTINGS_VALUES_ZOOMFACTOR 1
#define SETTINGS_VALUES_SHOWURL 2
#define SETTINGS_VALUES_BLOCKADS 3
#define SETTINGS_VALUES_BLOCKPOPUPS 4
#define SETTINGS_VALUES_FONTSIZE 5
#define SETTINGS_VALUES_FONTCOLOR 6
#define SETTINGS_VALUES_IDNSUPPORT 7
#define SETTINGS_VALUES_BUTTONS 8
#define SETTINGS_VALUES_WARNINGS 9
#define SETTINGS_VALUES_OPTIONS1 10
#define SETTINGS_VALUES_OPTIONS2 11
#define SETTINGS_VALUES_RESTRICTIONS 12
#define SETTINGS_VALUES_THEME 13
#define SETTINGS_VALUES_VERSION 14
#define SETTINGS_VALUES_KEYS 15
const static TCHAR* SETTINGS_VALUES[SETTINGS_VALUES_COUNT]={L"Highlight",L"ZoomFactor",L"ShowURL",L"BlockAds",L"BlockPopUps",L"FontSize",L"FontColor",L"IDNSupport",L"Buttons",L"Warnings",L"Options1",L"Options2",L"Restrictions",L"Theme",L"Version",L"Keys"};

// Options

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
// removed QuickFind_Activation 0x10000000 | 0x20000000
#define OPTION_DisableAdBlockerForLocalWebSites 0x40000000
#define OPTION_DisableAdBlockerForHttpsWebSites 0x80000000

#define OPTION_NewWinTab_SearchBox(Options) ((Options&0x6000000)>>25) // 0x2000000 | 4000000 | reserved 8000000
#define OPTION_NewWinTab_ContextMenu(Options) ((Options&0x60000)>>17) // 0x20000 | 40000 | reserved 80000
#define SetOPTION_NewWinTab_SearchBox(Options,id) Options=(((Options)&~(3<<25)) | ((id&0x3)<<25))
#define SetOPTION_NewWinTab_ContextMenu(Options,id) Options=(((Options)&~(3<<17)) | ((id&0x3)<<17))

#define OPTION_QueroShortcutKey(Options) ((Options&0xC000)>>14) // 0x4000 | 8000 | reserved 10000
#define SetOPTION_QueroShortcutKey(Options,id) Options=(((Options)&~(3<<14)) | ((id&0x3)<<14))

//#define OPTION_QuickFind_Activation(Options) ((Options&0x30000000)>>28) // 0x10000000 | 0x20000000
//#define SetOPTION_QuickFind_Activation(Options,id) Options=(((Options)&~(3<<28)) | ((id&0x3)<<28))

#define DEFAULT_OPTIONS (0xFFFFFF - 0x100 /* SetFocusOnStartUp */ - OPTION_ShowSearchEngineComboBox - OPTION_SearchCountry - OPTION_WhiteQueroButton)

#define OPTION2_ShowSearchBox 0x01
#define OPTION2_EnableQueroContextMenu 0x02
#define OPTION2_EnableAeroTheme 0x04
#define OPTION2_ShowSearchTermsWhileSearching 0x08
#define OPTION2_DisplayCertificateInformation 0x10
#define OPTION2_HighlightInDifferentColors 0x20
#define OPTION2_HideFavoritesButton 0x1000000
#define OPTION2_PageLoadingAnimation 0x2000000
#define OPTION2_HideToolbarCloseButton 0x4000000
#define OPTION2_HideAddressBox 0x8000000 // IE9: hide address box and navigation buttons
#define OPTION2_HideFlashAds 0x10000000
#define OPTION2_ShowCaption 0x20000000  // IE9: display page title in title bar
#define OPTION2_ShowCaption_SysColor 0x100  // Title bar text color: 0 white 1 black
#define OPTION2_AutoMaximize 0x40000000
#define OPTION2_DPI_Scaling 0x200
#define OPTION2_AddressNavigation 0x400
#define OPTION2_SearchPrompt 0x800

#define DEFAULT_OPTIONS2 0xFFFFFF

// Restrictions

#define LOCK_QueroToolbarMenu 0x1
#define LOCK_OptionsDialog 0x2
#define LOCK_WhiteList 0x4
#define LOCK_ViewBlockedContent 0x8
#define LOCK_SetBlockAds 0x10
#define LOCK_SetBlockPopUps 0x20
#define LOCK_SetHighlight 0x40
#define LOCK_SetHideFlashAds 0x80

// Quero Options

#define QUERO_IMFEELINGLUCKY 0x01 // == FLAG_BROWSEBYNAME
#define QUERO_GO2HP 0x2
#define QUERO_REDIRECT 0x4
#define QUERO_CONTEXT_SEARCHENGINE_BOX 0x8
#define QUERO_SETTEXT 0x10

// Shortcut Options

#define SHORTCUT_OPTION_DEFAULT 0x0
#define SHORTCUT_OPTION_SEARCHBOX 0x1
#define SHORTCUT_OPTION_QUERO_TOOLBAR 0x2
#define SHORTCUT_OPTION_ENTERKEY 0x4
#define SHORTCUT_OPTION_MIDDLECLICK 0x8
#define SHORTCUT_OPTION_IGNORE_MODIFIERS 0x10

// Window/Tab Options

#define OPEN_UNDEFINED -1
#define OPEN_SameWindow 0
#define OPEN_NewWindow 1
#define OPEN_NewTab 2
#define OPEN_BackgroundTab 3

#define IE6_MapNewTabToNewWin(newWinTab) if(g_IE_MajorVersion<7 && newWinTab>OPEN_NewWindow) newWinTab=OPEN_NewWindow

// Quick Find Activated By

#define QFIND_ActivatedByQuickType 0
#define QFIND_ActivatedBySlashKey 1
#define QFIND_AlwaysActive 2
#define N_QFIND_ACTIVATIONS 3

// IE7 constants
#define navOpenInNewTab 0x0800
#define navOpenBackgroundTab 0x1000

// Pop-up Blocker Options

#define POPUPBLOCKER_Enable 0x01
#define POPUPBLOCKER_BlockAll 0x02
#define POPUPBLOCKER_PlaySound 0x100

#define DEFAULT_POPUPBLOCKEROPTIONS (POPUPBLOCKER_PlaySound)

// Special Characters

#define SPECIALCHARS_NON 0
#define SPECIALCHARS_ASCII 1
#define SPECIALCHARS_IDN 2

// Buttons

#define BUTTON_GO 0x1
#define BUTTON_BACK 0x2
#define BUTTON_FORWARD 0x4
#define BUTTON_STOP 0x8
#define BUTTON_REFRESH 0x10
#define BUTTON_HOME 0x20
#define BUTTON_QUERO 0x100

#define N_NAV_BUTTONS 5 // Update MAX_BUTTONS in CToolbar.h

#define DEFAULT_BUTTONS (0xFFFF-BUTTON_HOME-BUTTON_GO)

// Icons

#define ICON_SEARCH 0
#define ICON_SEARCH_NOTFOUND 1
#define ICON_URL 2
#define ICON_CONTENTBLOCKED 3
#define ICON_IDN 4
#define ICON_123 5
#define ICON_LOCK 6
#define NICONS 7

// Colors

#define COLOR_UNDEFINED -1
#define COLOR_Search 0
#define COLOR_Link 1
#define COLOR_Separator 2
#define COLOR_Highlight 3
#define COLOR_HighlightText 4
#define COLOR_Description 5
#define COLOR_NotFound 6
#define COLOR_Background 7
#define NCOLORS 8

#define HTTPSBCKGNDCOLOR 0xE0FFFF
//#define LINKCOLOR 0x057D05
#define LINKCOLOR 0x008000
#define SEARCHCOLOR 0x804200
#define PHRASENOTFOUNDCOLOR 0x0000cc
#define PARTIALSUPPORTCOLOR 0x404040
//#define PROFILEBCKGNDCLR 0xA3B0B6

// Warnings

#define WARNING_IDN 0x1
#define WARNING_MISSGLYPHS 0x2
#define WARNING_ASCIIRULES_VIOLATION 0x4

// Warning Dialog

#define WARNING_DIALOG_IDN 0
#define WARNING_DIALOG_ILLEGAL_URL 1
#define WARNING_DIALOG_MISSGLYPHS 2
#define WARNING_DIALOG_ASCIIRULES_VIOLATION 3

// Find

#define FIND_Next 0x1
#define FIND_Previous 0x2
#define FIND_First 0x4
#define FIND_Last 0x8
#define FIND_Focus 0x10

// InitWords

#define INITWORDS_Bigrams 0x1
#define INITWORDS_SearchBox 0x2
#define INITWORDS_History 0x4
#define INITWORDS_IncludeWholePhrase 0x8

// Highlight options

#define HIGHLIGHT_None 0
#define HIGHLIGHT_Highlight 0x1
#define HIGHLIGHT_ScrollToFirst 0x2
#define HIGHLIGHT_Focus 0x4

// Windows 9x switch

#ifdef COMPILE_FOR_WIN9X
	#define REGKEYLENGTH 255
#else
	#define REGKEYLENGTH 2048
#endif

#define IsWindows8OrLater() (g_WindowsVersion>=0x602)
#define IsWindowsVistaOrLater() (g_WindowsVersion>=0x600)
#define IsBelowWindows8() (g_WindowsVersion<0x602)
#define IsBelowWindowsVista() (g_WindowsVersion<0x600)
#define IsBelowWindowsXP() (g_WindowsVersion<0x501)
#define IsBelowWindows2000() (g_WindowsVersion<0x500)

#define MAXWORDS 30
#define MAXWORDLENGTH 256

// FindText
#define MAXFRAMEDEPTH 16
#define FRAME_UNDEFINED -2
#define FRAME_THIS -1
#define FRAME_LAST 0xFFFF

#define SEPARATOR_HEIGHT 4

// Timer
#define ID_HIGHLIGHT_TIMER 100
#define ID_COMBOENGINE_HOVER_TIMER 101
#define ID_COMBOQUERO_TOOLTIP_TIMER 102
#define HIGHLIGHT_REFRESH_INTERVAL 3000 // Highlight every 3 seconds if there was a progress
#define HOVER_INTERVAL 100
#define HOVER_INTERVALS_BEFORE_SELECTION 10
#define TOOLTIP_TIMEOUT 2000

// Mutex time-out intervals

#define QMUTEX_TIMEOUT 8000 // Shared data time-out interval in milliseconds
#define SEARCH_MUTEX_TIMEOUT 1000
#define DOWNLOAD_MUTEX_TIMEOUT 1000

// Spacing ( Left Margin | Edit Control | Spacing | Icon | Spacing | Icon | Spacing | Margin | Drop Down button )
#define EMBEDICONS_MARGIN 3 // Space between embedded icons and drop down button
#define EMBEDICONS_SPACING 8 // Space between embedded icons
#define EMBEDICONS_ICONWIDTH g_Scaled_IconSize
#define EMBEDICONS_SLOTWIDTH (EMBEDICONS_ICONWIDTH+EMBEDICONS_SPACING)

#define QEDITCTRL_LEFTMARGIN (g_Scaled_IconSize + 5)
#define SPACING_CERTIFICATE_ORGANIZATION_EDITCTRL 2 // Spcae between certificate information and edit box

#define CHARSETNAMESPACING 2 // Space between character set names

// Embedded Buttons

typedef struct EmbedButtonCondition {
	int Id;
	bool Condition;
} EmbedButtonCondition;

#define MAXEMBEDBUTTONS 4
#define EMBEDBUTTON_LOCK 0
#define EMBEDBUTTON_IDN 1
#define EMBEDBUTTON_CONTENTBLOCKED 2
#define EMBEDBUTTON_SEARCHENGINE 3

// Drop-down history settings

#define DESCRIPTION_RELATIVE_WIDTH 3/10 // 30%
#define DESCRIPTION_SPACING 5
#define DESCRIPTION_MIN_WIDTH 50

// Instances

#define MAX_QUERO_INSTANCES 128 // Maximum number of instances that can share the keyboard proc
#define UNASSIGNED_INSTANCE_ID -1

// Private Message Commands

#define WM_QUERO (WM_USER + 1024)	// Process query
#define WM_QUERO_REDIRECTBROWSER (WM_USER + 1025)
#define WM_QUERO_SHOWOPTIONS  (WM_USER + 1026)
#define WM_QUERO_SETFAVICON (WM_USER + 1027)
#define WM_QUERO_SHOWZOOMFACTOR (WM_USER + 1029)
#define WM_QUERO_SHOWRESIZEWINDOW  (WM_USER + 1030)
#define WM_QUERO_TOOLBAR_COMMAND (WM_USER + 1031)
#define WM_QUERO_UPDATE_DWMTOPMARGIN 0xBFFF // Max WM_APP
#define WM_QUERO_UNSUBCLASS_IEFRAME 0xBFFE // Max WM_APP - 1
#define WM_QUERO_KEYBOARD_HOOK_IEFRAME (WM_USER + 1028)

// Quero Toolbar Commands

#define QUERO_COMMAND_FINDONPAGE 1
#define QUERO_COMMAND_HIDEFLASHADS 2
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
#define QUERO_COMMAND_SETHIDEFLASHADS 19
#define QUERO_COMMAND_IEFRAME_CHANGED 20
#define QUERO_COMMAND_SETAUTOMAXIMIZE 21
#define QUERO_COMMAND_MAXIMIZE 22

// IE Commands

#define IE_COMMAND_HISTORY 0x1A233
#define IE_COMMAND_HISTORY_CENTER 0x1A238
#define IE_COMMAND_QUICKTABS 0x100002 // Send to IE Frame
#define IE_COMMAND_VIEWSOURCE 0xC5B

// Keys

#define KEY_None 0
#define KEY_Quero 1 // 0x2
#define KEY_QueroMenu 2 // 0x4
#define KEY_Slash 3 // 0x8
#define KEY_CtrlSlash 4 // 0x10
#define KEY_CtrlDel 5 // 0x20
#define KEY_Char 6 // 0x40 Character key that triggers "jump to search box" pressed (printable character)
#define KEY_F2 7 // 0x80
#define KEY_F3 8 // 0x100
#define KEY_CtrlPause 9 // 0x200
#define KEY_Highlight 10 // 0x400
#define KEY_ViewSource 11 // 0x800
#define KEY_QuickTabs 12 // 0x1000
#define KEY_ViewBlockedContent 13 // 0x2000
#define KEY_F4 14 // 0x4000
#define KEY_CtrlBack 15 // 0x8000
#define KEY_CtrlL 16 // 0x10000

#define KEY_Ctrl FCONTROL
#define KEY_Alt FALT
#define KEY_Shift FSHIFT

// Quero Shortcut Keys

#define QKEY_CharMap L"SDQQ"
#define QKEY_CtrlQ 0x3
#define QKEY_AltQ 0x2
#define QKEY_AltD 0x1
#define QKEY_AltS 0x0

// Zoom page methods

#define OLECMDID_OPTICAL_ZOOM ((enum OLECMDID)63) // IE7 constant
#define ZOOMPAGE_PUT_CSSTEXT 1
#define ZOOMPAGE_PUT_ZOOM 2
#define ZOOMPAGE_SET_OPTICAL_ZOOM 3
#define ZOOMFACTOR_MIN 10
#define ZOOMFACTOR_MAX 1000

#define UseOpticalZoom() (g_IE_MajorVersion>=7 && (g_Options&OPTION_UseOpticalZoom))

// Resize Window

#define RESIZEWINDOW_WIDTH_MIN 100
#define RESIZEWINDOW_HEIGHT_MIN 100
#define RESIZEWINDOW_MOVE 0x1
#define RESIZEWINDOW_FULLSCREEN 0x2

// Registry Value Ids

#define REG_VALUE_HKCU_NONAVBAR 0
#define REG_VALUE_HKLM_NONAVBAR 1
#define N_REG_VALUES 2

// History timestamp comparison

#define IsMoreRecent_Than(newer,older) ((newer.dwHighDateTime > older.dwHighDateTime) || ((newer.dwHighDateTime == older.dwHighDateTime) && (newer.dwLowDateTime >= older.dwLowDateTime)))

// Notification Message pointer

#define lpDIS  ((LPDRAWITEMSTRUCT)lParam)
#define lpnm   ((LPNMHDR)lParam)
#define lpnmTB ((LPNMTOOLBAR)lParam)
#define lpnmTBCustomDraw ((LPNMTBCUSTOMDRAW)lParam)
#define lpnmLV ((LPNMITEMACTIVATE)lParam)
#define lpnmLVKeyDown ((LPNMLVKEYDOWN)lParam)
#define lpdi   ((LPNMTTDISPINFO)lParam)
#define lpmis  ((LPMEASUREITEMSTRUCT)lParam)

// IID_IHTMLFrameBase2
//DEFINE_GUID(IID_IHTMLFrameBase2,0x3050f6db,0x98b5,0x11cf,0xbb,0x82,0x00,0xaa,0x00,0xbd,0xce,0x0b);

// Version Info

typedef struct VersionInfo {
	INT Version;
	BYTE Build;
	BYTE Language;
	WORD Reserved1;
	DWORD Reserved2;
	BYTE Reserved3;
	BYTE Flags; // Bit 0 First Run
} VersionInfo;

#define VERSION_FLAG_FIRSTRUN 0x1
#define VERSION_FLAG_SYNC_SETTINGS 0x80000000

// History data structure used in registry

typedef struct HQRegData {
	BYTE ProfileId;
	BYTE EngineId;
	BYTE Type;
	FILETIME Timestamp;
	BYTE Flags;
	DWORD reserved2;
} HQRegData;

typedef struct HURLRegData {
	FILETIME Timestamp;
	BYTE reserved1;
	DWORD reserved2;
} HURLRegData;

// Main history structure

typedef struct HistoryEntry {
	BSTR Query;
	BYTE ProfileId;
	BYTE EngineId;
	BYTE Type;
	FILETIME Timestamp;
	BYTE Flags; // Bit 0 Browse By Name
	HICON hIcon; // Cached icon for local copy CQToolbar::History, freed in FreeHistory
} HistoryEntry;

#define FLAG_BROWSEBYNAME 0x01

// WhiteList registry data structure

typedef struct HWLRegData {
	USHORT Permits;
	USHORT reserved;
} HWLRegData;

// WhiteList main structure

typedef struct WhiteListEntry {
	BSTR Pattern;
	int PatternLength;
	USHORT Permits;
	BYTE QueroInstanceId; // Reference to toolbar instance which added temporary whitelist entry
} WhiteListEntry;

#define WL_ALLOW_IDN 0x01
#define WL_ALLOW_MISSGLYPHS 0x02
#define WL_ALLOW_POPUPS 0x04
#define WL_ALLOW_FLASH 0x20
#define WL_ALLOW_BANNER 0x40
#define WL_ALLOW_ADSCRIPT 0x80
#define WL_ALLOW_DIV 0x100
#define WL_ALLOW_IFRAME 0x400
#define WL_ALLOW_ALL (WL_ALLOW_IDN|WL_ALLOW_MISSGLYPHS|WL_ALLOW_POPUPS|WL_ALLOW_FLASH|WL_ALLOW_BANNER|WL_ALLOW_ADSCRIPT|WL_ALLOW_DIV|WL_ALLOW_IFRAME)

// Blocked Content Item

typedef struct BlockedContentItem {
	BYTE Type;
	TCHAR URL[MAXURLLENGTH];
} BlockedContentItem;

#define BLOCKED_CONTENT_POPUP 0
#define BLOCKED_CONTENT_IFRAME 1
#define BLOCKED_CONTENT_FLASH 2
#define BLOCKED_CONTENT_BANNER 3
#define BLOCKED_CONTENT_ADSCRIPT 4
#define BLOCKED_CONTENT_DIV 5

#define BLOCKED_CONTENT_NTYPES 6

#define ADBLOCKER_Enable 0x01
#define ADBLOCKER_BLOCK_FLASH 0x02
#define ADBLOCKER_BLOCK_BANNER 0x04
#define ADBLOCKER_BLOCK_ADSCRIPT 0x08
#define ADBLOCKER_BLOCK_DIV 0x10
#define ADBLOCKER_BLOCK_IFRAME 0x40
#define ADBLOCKER_BLOCK_ALL (ADBLOCKER_BLOCK_FLASH|ADBLOCKER_BLOCK_BANNER|BLOCKED_CONTENT_ADSCRIPT|ADBLOCKER_BLOCK_DIV|ADBLOCKER_BLOCK_IFRAME)

#define DEFAULT_ADBLOCKEROPTIONS (0xFFFFFF - ADBLOCKER_Enable)

// IsAdURL Contexts

#define ISADURL_CONTEXT_EXTRACT_SIZE 0x80
#define ISADURL_CONTEXT_BANNER 1
#define ISADURL_CONTEXT_IFRAME 2
#define ISADURL_CONTEXT_SCRIPT 3

// Element Types

#define ELEMENT_TYPE_UNKNOWN 0x0
#define ELEMENT_TYPE_INPUT 0x1
#define ELEMENT_TYPE_TEXTAREA 0x2
#define ELEMENT_TYPE_SELECT 0x4
#define ELEMENT_TYPE_APPLET 0x8
#define ELEMENT_TYPE_OBJECT 0x10
#define ELEMENT_TYPE_EMBED 0x20
#define ELEMENT_TYPE_FRAME 0x40
#define ELEMENT_TYPE_IFRAME 0x80
#define N_ELEMENT_TYPES 8

// WindowProc for the edit control of the Combobox
LRESULT CALLBACK EditCtlWindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

// Shared variables among processes
extern LONG g_ToolbarBackgroundState;

// Shared data mutex
extern HANDLE g_hQSharedDataMutex;

// Shared variables among instances
extern FILETIME g_QueroStartTime;
extern int g_WindowsVersion;
extern int g_IE_MajorVersion;
extern HICON g_IE_Icon;
extern bool g_IgnoreAltKeyUpOnce;

// Shared settings
extern DWORD g_BlockPopUps;
extern DWORD g_BlockAds;
extern int g_FontSize; // -10 to +10; standard = 0
extern int g_FontColor;
extern UINT g_Options;
extern UINT g_Options2;
extern UINT g_Buttons;
extern UINT g_Warnings;
extern UINT g_Restrictions;
extern UINT g_Keys;
extern bool g_ShowURL;
extern bool g_IDNSupport;
extern BSTR g_QueroTheme_FileName;
extern HMODULE g_QueroTheme_DLL;
extern COLORREF g_ThemeColors[NCOLORS];
extern int g_Unscaled_NavButtonSize;
extern int g_Unscaled_ButtonSize;
extern int g_Unscaled_IconSize;
extern int g_Unscaled_PaddingY;
extern int g_Scaled_NavButtonSize;
extern int g_Scaled_ButtonSize;
extern int g_Scaled_IconSize;
extern int g_Scaled_QueroLogoX;
extern int g_Scaled_QueroLogoY;
extern int g_Scaled_PaddingY;
extern bool g_MiddleClick;

// Quero Thread Local Storage

typedef struct QThreadData {
	CQToolbar *pToolbar;
	HWND hIEWnd;
	DWORD ThreadId;
	DWORD ThreadId_IEWnd;
	HHOOK hKeyHookTab;
	bool bNewWindow;
} QThreadData;

extern QThreadData QThreadLocalStg[MAX_QUERO_INSTANCES];
extern int g_MaxUsedInstanceId;
extern UINT g_QueroInstanceCount;

extern QThreadData* GetCurrentQueroInstance();
extern QThreadData* GetFirstQueroInstance();
extern QThreadData* GetQueroInstanceActiveTab();

// Search History
extern HistoryEntry g_History[HISTORYSIZE];
extern UINT g_HistoryIndex;
extern UINT g_LTimeHistory;
	
// URL History
extern BSTR g_URLs[URLHISTORYSIZE];
extern UINT g_nURLs; // URL history index
extern UINT g_LTimeURLs;
	
// Whitelist
extern WhiteListEntry g_WhiteList[WHITELISTSIZE];
extern UINT g_WhiteListIndex;
extern UINT g_LTimeWhiteList;

// Temporary Whitelist
extern WhiteListEntry g_TemporaryWhiteList[MAX_QUERO_INSTANCES];
extern UINT g_TemporaryWhiteListIndex;

// Quero Shared Memory

typedef struct QSharedMemory {
	UINT LTimeHistory;
	UINT LTimeURLs;
	UINT LTimeWhiteList;
} QSharedMemory;

extern QSharedMemory* g_QSharedMemory;
extern HANDLE g_hQSharedMemoryMutex;
extern HANDLE g_hQSharedMemoryFileMapping;
extern HANDLE g_hQSharedListMutex;

// Safe string length macros

#define StrCbLen(psz,cbMax,cb); if(StringCbLength(psz,cbMax,&cb)!=S_OK) cb=0;
#define StrCbLenA(psz,cbMax,cb); if(StringCbLengthA(psz,cbMax,&cb!=S_OK) cb=0;
#define StrCchLen(psz,cchMax,cch); if(StringCchLength(psz,cchMax,&cch)!=S_OK) cch=0;
#define StrCchLenA(psz,cchMax,cch); if(StringCchLengthA(psz,cchMax,&cch)!=S_OK) cch=0;

// WM_SETICON: wParam to distinguish whether Quero wants to try to set a favicon or IE itself calls WM_SETICON
#define ICON_SMALL_FAVICON 666

// New and original window procedure of sublcassed IE Frame window

extern WNDPROC ORIG_IEFrame_WndProc;
extern LRESULT CALLBACK IEFrame_WndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

// References

class CQueroBand;
interface IQueroBroker;

/////////////////////////////////////////////////////////////////////////////
// CQToolbar
class CQToolbar : public CWindowImpl<CQToolbar>
{
public:

	DECLARE_WND_SUPERCLASS(TEXT("QTOOLBAR"), TOOLBARCLASSNAME)

	BEGIN_MSG_MAP(CQToolbar)
		//MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanging)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_MOVE, OnMove)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		MESSAGE_HANDLER(WM_SYSCOLORCHANGE, OnSysColorChange)
		MESSAGE_HANDLER(WM_MEASUREITEM, OnMeasureItem)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_MENUSELECT, OnMenuSelect)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_CTLCOLOREDIT, OnCtlColorEdit)
		MESSAGE_HANDLER(WM_CTLCOLORLISTBOX, OnCtlColorEdit)
		MESSAGE_HANDLER(WM_MBUTTONUP, OnMiddleClickUp)

		// Private Message Commands
		MESSAGE_HANDLER(WM_QUERO, OnGo)
		MESSAGE_HANDLER(WM_QUERO_SETFAVICON, OnSetFavIcon)
		MESSAGE_HANDLER(WM_QUERO_KEYBOARD_HOOK_IEFRAME, OnKeyboardHook_IEFrame)
		MESSAGE_HANDLER(WM_QUERO_REDIRECTBROWSER, OnRedirectBrowser)
		MESSAGE_HANDLER(WM_QUERO_SHOWOPTIONS, OnShowOptions)
		MESSAGE_HANDLER(WM_QUERO_SHOWZOOMFACTOR, OnShowZoomFactor)
		MESSAGE_HANDLER(WM_QUERO_SHOWRESIZEWINDOW, OnShowResizeWindow)
		MESSAGE_HANDLER(WM_QUERO_TOOLBAR_COMMAND, OnQueroToolbarCommand)

		//CHAIN_MSG_MAP_MEMBER(m_ComboQuero)
		//CHAIN_MSG_MAP_MEMBER(m_ComboEngine)
		//CHAIN_MSG_MAP_MEMBER(m_NavBar)
		//CHAIN_MSG_MAP_MEMBER(m_LogoToolbar)
		//CHAIN_MSG_MAP_MEMBER(m_ButtonBar)
	END_MSG_MAP()

	// Message handlers

	//LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	//LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	//LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSysColorChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMenuSelect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlColorEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMiddleClickUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// Private Message handlers
	LRESULT OnGo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFavIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRedirectBrowser(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnShowOptions(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnShowZoomFactor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnShowResizeWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnQueroToolbarCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyboardHook_IEFrame(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// Keyboard handler
	LRESULT OnKeyboardHook(int nCode, WPARAM wParam, LPARAM lParam,bool bIEFrame);
	bool IsPopUpMenuVisible();

	// Quero Instance related
	CQToolbar();
	~CQToolbar();
	void RemoveQueroInstance(int id);
	inline VersionInfo* GetVersionInfo() { return &QueroVersion; }
	inline bool IsFirstRun() { return bFirstRun; }
	static void UpdateQueroInstances(UINT update);
	void UpdateQueroInstance(UINT update);

	// Quero Settings
	static HKEY OpenQueroKey(HKEY hKeyRoot,TCHAR *pSubKey,bool bCreateKey);
	void SyncSettings();
	void LoadTheme(TCHAR *pQueroThemeFile);
	bool SaveSettingsValue(UINT ValueId,DWORD dwValue);
	bool SaveSettingsValueEx(UINT ValueId,DWORD dwType,LPBYTE pData,DWORD cbSize);
	bool DeleteSettingsValue(UINT ValueId);

	// Getter/Setter methods
	inline CComboQuero* GetComboQuero() {return &m_ComboQuero;};
	inline CComboEngine* GetComboEngine() {return &m_ComboEngine;};
	inline CNavBar* GetNavBar() {return &m_NavBar;};
	inline CButtonBar* GetButtonBar() {return &m_ButtonBar;};
	inline CLogoToolbar* GetLogoToolbar() {return &m_LogoToolbar;};
	inline CQueroBand* GetBand() { return m_pBand; }
	inline void SetBand(CQueroBand* pBand) {m_pBand=pBand;}
	inline IWebBrowser2* GetBrowser() { return m_pBrowser; }
	void SetBrowser(IWebBrowser2* pBrowser);
	void SetCurrentType(BYTE type,HICON hFavIcon,bool bRedraw=true);
	inline BYTE GetCurrentType() {return currentType;}
	void ToggleCurrentType();
	inline bool GetImFeelingLucky() {return ImFeelingLucky;}

	// IE Windows
	HWND GetIEFrameWindow();
	HWND GetIETabWindow();
	bool IsIEFrameWindow(HWND hwnd_IEFrame);
	void IEFrame_Changed();

	// IE Commands
	void PostCommandToIE(WPARAM wParam,bool bIEFrame);

	// New window/tab state
	UINT GetNewWinTabKeyState(UINT ShortcutOptions=SHORTCUT_OPTION_DEFAULT,UINT DefaultNewWinTab=OPEN_UNDEFINED);

	// Icons
	void SetIconOffset(int offset);
	inline int GetIconOffset() {return currentIconOffset;}
	void SetSearchIcon(int IconId);
	inline HICON GetFavIcon() {return currentFavIcon;}
	inline void SetFavIcon(HICON hIcon) {if(currentFavIcon) DestroyIcon(currentFavIcon);currentFavIcon=hIcon;}
	inline TCHAR* GetFavIconURL() { return (currentFavIcon)?m_FavIcon.GetFavIconURL():NULL; }
	void SetIcon_IEFrame(HICON hIcon);

	// Colors
	void SetColorScheme(int ColorSchemeId,bool bRedraw);
	inline COLORREF GetColor(int i) { return Colors[i]; }
	
	// Dimensions and Appearance
	void CreateDeferred();
	bool bToolbarCreated;
	void InitFontAndHeight();
	int GetToolbarHeight();
	int GetToolbarPadding();
	inline int GetItemHeight() {return ItemHeight;};
	int CalculatePxHeight(int fheight);
	int CalculateFontHeight(int pxheight);
	void MeasureDomainExtents(TCHAR *url,int DomainStartIndex,int DomainEndIndex,int *DomainStartExtent,int *DomainEndExtent);
	int MeasureTextExtent(TCHAR *pText,UINT uiTextLength);
	int MeasureEngineWidth();
	int GetToolbarMinWidth();
	void CenterDialog(HWND hwnd_Dialog);
	void ResizeWindow(int x,int y,int width,int height,UINT flags);
	void SizeComboEngineList();
	void ShowToolbarIfHidden();
	bool IsWindowMaximized();
	bool IsTheaterMode();
	void SetAutoMaximize(bool bAutoMaximize);

	// Drawing
	void DrawItemComboQuero(HDC hDC,DRAWITEMSTRUCT *pItem);
	void DrawItemComboEngine(HDC hDC,DRAWITEMSTRUCT *pItem);
	void DrawItemIcon(HDC hDC,RECT *pRect,HICON hIcon,HBRUSH hBrush,int Offset,bool bDrawLoadingAnimation);
	void DrawIDN(HDC hDC,RECT Rect);
	void DrawComboQueroIcon();
	HDC GetToolbarBckgrndMemDC();
	inline HBITMAP GetToolbarBckgrndBitmap() { return hToolbarBckgrndBitmap; }
	inline HBRUSH GetDefaultBackground() {return hDefaultBackground;}
	inline HBRUSH GetHighlightBrush() {return hHighlightBrush;}

	// Icon Animation
	inline bool IsLoadingAnimation() {return currentType==TYPE_ADDRESS && m_ComboQuero.bCurrentURLDisplayed && (m_IconAnimation.GetActiveAnimations()&ICON_ANIMATION_LOADING)!=0;}
	inline bool IsSearching() {return Searching;}
	inline void StartSearchAnimation() { m_IconAnimation.Start(ICON_ANIMATION_SEARCHING); }
	inline void StopSearchAnimation(bool wait=false) { m_IconAnimation.Stop(ICON_ANIMATION_SEARCHING,wait); }

	// History
	inline HistoryEntry* GetHistory() {return History;};
	inline UINT GetHistoryIndex() {return HistoryIndex;};
	void SyncHistory(bool Synchronize=true);
	bool SyncLocalHistory();
	void CopyLastHistoryEntry(HistoryEntry *pHistoryEntry,bool Synchronize=true);
	void FreeLastHistoryEntry();
	void SyncURLs(bool Synchronize=true);
	void FreeHistory(HistoryEntry *History,UINT* HistoryIndex);
	void FreeURLs();
	void AddToHistory(TCHAR* entry,BYTE type,BYTE flags,int engineid,int profileid);
	void AddToURLHistory(TCHAR* entry);
	void ClearHistory();
	void DeleteFromHistory(TCHAR *entry);
	void DeleteFromURLHistory(TCHAR *entry);
	HistoryEntry* GetLastHistoryEntry();

	// WhiteList
	void SyncWhiteList(bool Synchronize=true);
	void FreeWhiteList(WhiteListEntry *pWhiteList,UINT *pWhiteListIndex);
	int AddToWhiteList(TCHAR* entry,USHORT permits,bool or_permits);
	int DeleteFromWhiteList(TCHAR* entry);
	void ResetWhiteList();
	int GetWhiteListIndex(bool Synchronize=true);
	int GetWhiteListIndex(WhiteListEntry *pWhiteList,UINT *pWhiteListIndex,TCHAR *host,int hostlen,bool Synchronize=true);
	USHORT GetWhiteListPermits(TCHAR *url,TCHAR *host,int hostlen);
	UINT GetWhiteListBlockAds(TCHAR *url);
	bool GetWhiteListBlockPopUps(TCHAR *PopUpURL);
	void ShowWhiteList(HWND hwnd,bool MatchCurrentURL=false);
	void TemporarilyUnblock(bool bUnblock,bool bRemoveFromAllInstances,TCHAR* pattern,bool bSynchronize);
	void TemporarilyUnblockCurrentDomain(bool bUnblock,bool bRemoveFromAllInstances,bool bSynchronize);

	// Search profiles
	void SelectEngine(int EngineIndex,bool bForceUpdate=false,bool bSetCurSel=true,bool bRedraw=true);
	void SelectNextPrevEngine(bool bNext);
	void SelectProfile(int ProfileId,int EngineId=-1);
	inline int GetProfileId() { return CurrentProfileId; }
	inline BSTR GetProfileName() { return m_Profiles.CurrentProfile.Name; }
	inline SearchEngine* GetCurrentEngine() { return nengines?&m_Profiles.CurrentProfile.Engines[CurrentEngineIndex]:NULL; }
	inline BSTR GetEngineName() { return nengines?m_Profiles.CurrentProfile.Engines[CurrentEngineIndex].Name:NULL; }
	inline int GetEngineId() { return nengines?m_Profiles.CurrentProfile.Engines[CurrentEngineIndex].id:-1; }
	inline UINT GetEngineCount() {return nengines;}
	inline bool GetChooseProfile() { return chooseProfile;}
	inline bool SetChooseProfile(bool c) { return chooseProfile=c;}
	void SyncSearchProfiles();

	// Navigation related
	void Quero(TCHAR* pQuery=NULL,BYTE type=TYPE_UNKNOWN,BYTE options=0,UINT newWinTab=OPEN_UNDEFINED,int differentEngineId=-1,int differentProfileId=-1); // Search or navigate browser
	void OnBeforeNavigate(IDispatch* pDisp,VARIANT* vUrl,VARIANT* vFlags,VARIANT* vTarget,VARIANT* vPostData,VARIANT* vHeaders,SHORT* Cancel,bool first,bool toplevel);
	void OnNavigateBrowser(TCHAR *newurl,bool first);
	void OnNavigateError(TCHAR *url,long StatusCode,SHORT *Cancel);
	void OnSetSecureLockIcon(SecureLockIconConstants SecureLockIconStatus);
	inline bool IsNavigationPending() {return NavigationPending;}
	void OnSiteChange();
	void OnDocumentComplete();
	void OnDownloadBegin();
	void OnDownloadComplete();
	void OnProgressChange(int progress);
	void NavigateToAboutBlank();
	bool InterceptSearch(TCHAR *pURL,TCHAR *pAsciiURL,TCHAR *pPostDataUnicode);
	void FreeLastQueryURL();
	void NavigateUp(UINT newWinTab);
	bool NavigateUp_Available();

	// Pop-up Blocker
	void OnNewWindow3(IDispatch **ppDisp,VARIANT_BOOL *pCancel,DWORD dwFlags,BSTR bstrUrlContext,BSTR bstrUrl);
	void PopupBlocked();
	void GetDiffFileTime(FILETIME *time1,FILETIME *time2,FILETIME *diff);

	// URL related
	bool CheckIDN(TCHAR *url_decoded,int hoststartidx,int hostendidx,int domainstartidx,int idna_status);
	bool HasMissingGlyphs(HDC hDC,TCHAR *pHost,int len);
	void MakeAbsoluteURL(TCHAR *AbsoluteURL,TCHAR *URL,TCHAR *BaseURL);
	BYTE SetCurrentURL(TCHAR *url);
	int URLToAscii(TCHAR *url);
	int URLToUnicode(TCHAR *url,int *HostStartIndex,int *HostEndIndex,int *DomainStartIndex);
	inline CIDNA* GetIDNA() { return &m_IDNA; }
	bool IsStartPageURL(TCHAR *url);
	void CopyCurrentCoreDomain(TCHAR *pCoreDomain);
	void AppendCurrentAddress(TCHAR *pQueryWithAddress,TCHAR *pOriginalQuery,UINT iRequiresAddress);

	// Document Title related
	void OnTitleChange(BSTR bstrTitle);
	void SetTitle_IEFrame();
	void FreeCurrentDocumentTitle();

	// Security
	bool ShowSecurityWarning(int WarningDialog,TCHAR *pHost,int HostLen,int WL_HostStartIndex);
	inline bool IsSecureConnection() {return SecureLockIcon_Quero;}
	bool IsOperationAllowed(UINT LockFlags);

	// Auto Complete
	inline CAutoComplete* GetAutoComplete() { return m_AutoComplete; }
	inline void ResetAutoComplete() { if(m_AutoComplete) m_AutoComplete->ResetEnumerator(); }
	inline bool IsAutoCompleteVisible() { return (m_AutoComplete?m_AutoComplete->GetDropDownStatus():false); }

	// Find on page
	bool FindText(IHTMLDocument2* pHtmlDocument,TCHAR *pSearchText, long lFlags,BYTE findOptions,int depth=0);
	void FindOnPage(BYTE initiatedBy,BYTE findOptions);
	void QuickFind(TCHAR* pSearchText);
	inline void ClearLastFoundText() {LastFoundText[0]=_T('\0');}
	inline TCHAR* GetLastFoundText() {return LastFoundText;}
	void SetPhraseNotFound(bool notfound,bool noredraw=false);
	inline bool GetPhraseNotFound() {return PhraseNotFound;}
	bool IsFocusOnInput(IHTMLDocument2 *pHtmlDocument);

	// Content Blocking
	bool HideFlashAds(IHTMLDocument2* pHtmlDocument,bool bHide);
	HRESULT BlockObject(IHTMLDocument2* pHtmlDocument,TCHAR* pClassId,TCHAR *pType,bool bHide);
	HRESULT BlockEmbed(IHTMLDocument2* pHtmlDocument,TCHAR* pFileExt,TCHAR *pType,bool hbHde);
	//HRESULT RemoveHtmlElement(IHTMLDocument2* pHtmlDocument,IHTMLElement* pHtmlElement);
	HRESULT HideHtmlElement(IDispatch* pElementDisp,bool bHide);
	bool IsBannerAd(TCHAR *ContentURL,TCHAR *BaseURL,int AttrParsed,int ImgWidth,int ImgHeight);
	int IsAdImageSize(int ImgWidth,int ImgHeight);
	bool IsAdURL(TCHAR *ContentURL,TCHAR *BaseURL,BYTE context);
	bool IsVideoPlayerURL(TCHAR *ContentURL);
	void AddToBlockedContent(BYTE Type,TCHAR *ContentURL,TCHAR *BaseURL,bool Synchronize);
	void SetContentBlocked();
	void ResetBlockedContent();
	void OnContentBlockedButtonClick(POINT *point,RECT *rcExclude);
	void SetBlockAds(DWORD BlockAds);
	void SetBlockPopUps(DWORD BlockPopUps);
	void SetHideFlashAds(bool bHide);

	// DOM related
	bool GetHtmlDocument2(IHTMLDocument2** ppHtmlDocument);
	HRESULT GetElementCollection(BSTR pTagName,IHTMLDocument2* pHtmlDocument,IHTMLElementCollection** ppElementColl);
	int GetElementType(IHTMLElement* pHtmlElement);
	IHTMLDocument2* GetFrameDocument(int ElementType,IHTMLElement *pHtmlElement);
	IHTMLDocument2* GetFrameDocument(IHTMLFramesCollection2 *pFramesCollection,int index);

	// Highlighting
	bool HighlightWords(IHTMLDocument2* pHtmlDocument,TCHAR Words[MAXWORDS][MAXWORDLENGTH],int nWords,BYTE highlightOptions);
	bool HighlightWord(TCHAR* pWord);
	void SetHighlight(bool hl);

	// Zoom page
	bool ZoomPage(UINT zoom,BYTE method);
	void SetZoomFactor(UINT NewZoomFactor,bool bUpdateZoomFactor);

	// Status Text
	void PutStatusText(const TCHAR *pText);

	// Embedded buttons
	void UpdateEmbedButtons(bool bForceResizeEditCtrl,bool bRedraw);
	long GetEmbedButtonsTotalWidth();

	// Quero Toolbar menu
	void OnQueroButtonClick(UINT flags,POINT *point,RECT *rcExclude);

	// Focus
	HRESULT SetFocusOnParentWindow(IHTMLDocument2 *pHtmlDocument);
	HRESULT SetFocusOnIEServerWindow();

	// String methods
	static TCHAR* trim(TCHAR *pStr);
	static bool TrimNewlines(TCHAR *pStrDest,size_t cchDest,TCHAR *pStrSrc);
	UINT SplitIntoWords(TCHAR* str,TCHAR Words[MAXWORDS][MAXWORDLENGTH],BYTE options=INITWORDS_Bigrams,UINT MaxWords=MAXWORDS);
	void CopyWords(TCHAR WordsDest[MAXWORDS][MAXWORDLENGTH],UINT *nWordsDest,TCHAR WordsSrc[MAXWORDS][MAXWORDLENGTH],UINT nWordsSrc);
	void InitWords(TCHAR Words[MAXWORDS][MAXWORDLENGTH],UINT* nWords,BYTE options=INITWORDS_Bigrams|INITWORDS_SearchBox|INITWORDS_History,BSTR* pbstrQuery=NULL); // Retrieve the last searched keywords
	TCHAR* GetString(UINT id);

public:
	// Quero version information
	static VersionInfo QueroVersion;

	// Quero instance related
	int QueroInstanceId; // Index of the current IE Window in the g_hIEWnd/g_hToolbar array
	bool bFirstRun; // Quero Toolbar is loaded for the first time
	bool IsNewProcess; // Is this Quero instance the first in the IE process
	bool IsActive; // True if tab (toolbar) is active
	
	// Pointers to the QueroBand and the browser
	CQueroBand* m_pBand;
	IWebBrowser2* m_pBrowser;

	// Embedded controls
	CNavBar m_NavBar;
	CLogoToolbar m_LogoToolbar;
	CButtonBar m_ButtonBar;
	CComboQuero m_ComboQuero;
	CIconAnimation m_IconAnimation;
	CAutoComplete* m_AutoComplete;
	CComboEngine m_ComboEngine;

	// ReBar control
	#ifdef COMPILE_FOR_WINDOWS_VISTA
	CAeroReBar m_ReBar;
	#endif

	// Query Toolbar menu
	HMENU hPopupMenu;

	// FavIcon Downloader
	CFavIcon m_FavIcon;

	// Certificate Downloader
	CCertificateDownloader m_CertificateDownloader;

	// Quero Broker
	IQueroBroker *pQueroBroker;

	// Search History
	HistoryEntry History[HISTORYSIZE];
	UINT HistoryIndex;
	UINT LTimeHistory;

	// Last added history entry
	HistoryEntry LastHistoryEntry;
	
	// Blocked Content
	BlockedContentItem BlockedContent[BLOCKEDCONTENTSIZE];
	UINT BlockedContentIndex;
	bool ContentBlocked;
	bool bTemporarilyUnblock;
	
	// Last pop-up time
	FILETIME LastPopUpTime;

	// Dimensions and Scaling
	int ItemHeight; // Height of the inner combo box item
	int Padding_Top; // Padding between the top edge and the text in an item
	int Margin_Items; // Margin between items in the engine and search combo box
	int IdealEngineWidth; // Width of the engine's combobox
	int CurrentEngineWidth;
	int LogPixelsX;
	int LogPixelsY;

	// Fonts
	HFONT hFont; // Font in edit controls	
	HFONT hFontBold; // Same font bold

	// Colors and background
	COLORREF Colors[NCOLORS];
	HBRUSH hDefaultBackground; // Default background brush
	HBRUSH hHighlightBrush;
	HDC hToolbarBckgrndMemDC;
	HBITMAP hToolbarBckgrndBitmap;
	LONG ToolbarBackgroundState;

	// Icons
	static HICON g_Icons[NICONS];
	static bool g_bIconsLoaded;
	HICON currentIcon;
	HICON currentFavIcon;
	int currentIconOffset;

	// State variables
	BYTE currentType;
	int LastProgress;
	bool Searching;
	bool SecureLockIcon_IE; // As reported by DWebBrowserEvents2::SetSecureLockIcon (IE7+) or triggered by https protocol (IE6)
	bool SecureLockIcon_Quero; // Current state of Quero's lock icon
	bool NavigationFailed; // True if previous top-level navigation failed
	bool NavigationPending; // True if top-level navigation is pending, used for determining BlockAds settings by CQueroFilter
	bool InternalLink; // Quero.dll resource link
	bool ImFeelingLucky;
	bool PreviewIDN; // Preview IDN while Security Warning is displayed
	bool HistoryEntrySelected; // Dropdown entry selected, true to ignore CBN_SELCHANGE notification after CBN_SELENOK

	// Hover Timer related
	UINT LastHighlightedItemIndex; // Currently highlighted item in the SE drop-down list
	UINT Times_LastHighlightedItem_Identical;

	// Settings
	UINT ZoomFactor;

	// Redirect Request
	CRedirectRequest RedirectRequest;
	bool bAllowOnePopUp; // Ignore popup blocker once if new window is opened by Quero

	// Search Profiles
	CProfiles m_Profiles;
	int CurrentProfileId;
	int CurrentEngineIndex;
	bool chooseProfile; // If true display profiles in the search engine list
	UINT nengines; // Number of engines in the selected profile
	UINT nseparators; // Number of sepearators in the search engine's list

	// Current URL related
	TCHAR currentURL[MAXURLLENGTH];
	bool IsIPv6;
	int HostStartIndex;
	int HostEndIndex;
	int DomainStartIndex;
	int CoreDomainStartIndex;
	int CoreDomainEndIndex;
	int CoreDomainStartExtent; // Position where domain highlighting starts
	int CoreDomainEndExtent;
	TCHAR currentAsciiURL[MAXURLLENGTH];
	int HostStartIndexAscii;
	int HostEndIndexAscii;
	int DomainStartIndexAscii;
	BYTE SpecialCharsInURL;
	int Certificate_Organization_Extent;
	CIDNA m_IDNA;

	// Before navigation URL
	TCHAR beforeURL[MAXURLLENGTH];
	int BeforeHostStartIndex;
	int BeforeHostEndIndex;
	int BeforeDomainStartIndex;
	FILETIME URLNavigationTime;

	// Last Query URL
	BSTR LastQueryURL; // Used to prevent displaying the current URL in the search box

	// Current document title
	BSTR bstrCurrentDocumentTitle;

	// Embedded buttons
	int nEmbedIcons; // Number of embedded icons in the search box
	int EmbedButtons[MAXEMBEDBUTTONS];

	// Highlighting
	TCHAR HighlightedWords[MAXWORDS][MAXWORDLENGTH];
	UINT nHighlightedWords;
	bool Highlight;

	// Find on page
	TCHAR LastFoundText[MAXWORDLENGTH];
	long Find_LastFrameDocument[MAXFRAMEDEPTH]; // Used by FindText
	int Find_Occurence; // Used by FintText
	bool PhraseNotFound;

	// String buffer for LoadString
	TCHAR String[MAX_STRING_LENGTH];

	// Tooltip text
	TCHAR Tooltip[MAXURLLENGTH];
};

#endif //__QTOOLBAR_H_