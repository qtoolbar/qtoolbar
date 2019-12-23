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

// QueroBand.cpp : Implementation of CQueroBand

#include "stdafx.h"
#include <wchar.h>
#include <mmsystem.h>
#include "Quero.h"
#include "QueroBand.h"
#include "LogoToolbar.h"
#include "ContentFilter.h"

// Window Vista specific

#ifdef COMPILE_FOR_WINDOWS_VISTA
#include "MenuIcon.h"
#endif

// Shared data among all Quero Toolbar instances

#pragma data_seg (".QueroToolbarData")
LONG g_ToolbarBackgroundState=1; // Increased by 1 when the background image should be updated
#pragma data_seg ()
#pragma comment(linker,"/SECTION:.QueroToolbarData,RWS") // Tell linker to make it shared

// Shared data among all instances of the current process

// Shared data mutex
HANDLE g_hQSharedDataMutex=NULL;

FILETIME g_QueroStartTime;
int g_WindowsVersion=0;
int g_IE_MajorVersion=0;

WNDPROC	g_pOrigCtxMenuWndProc=NULL; // Previous context menu windows procedure
HMENU g_QueroCtxMenu=NULL; // Quero Context Menu
bool g_EnableCtxMenuGo;

bool g_AdBlockerInstalled=false;
bool g_PopUpBlockerInstalled=false;

#ifndef COMPILE_FOR_WINDOWS_VISTA
HICON g_IE_Icon=NULL; // Default IE frame window icon
#endif

// Dimensions and Quero Theme
BSTR g_QueroTheme_FileName=NULL;
HMODULE g_QueroTheme_DLL=NULL;
COLORREF g_ThemeColors[NCOLORS];
int g_Unscaled_NavButtonSize=NAVBUTTONSIZE_DEFAULT;
int g_Unscaled_ButtonSize=BUTTONSIZE_DEFAULT;
int g_Unscaled_IconSize=ICONSIZE_DEFAULT;
int g_Unscaled_PaddingY=PADDINGY_UNKNOWN;
int g_Scaled_NavButtonSize=NAVBUTTONSIZE_DEFAULT;
int g_Scaled_ButtonSize=BUTTONSIZE_DEFAULT;
int g_Scaled_IconSize=ICONSIZE_DEFAULT;
int g_Scaled_QueroLogoX=QUEROLOGOX;
int g_Scaled_QueroLogoY=QUEROLOGOY;
int g_Scaled_PaddingY=PADDINGY_UNKNOWN;

// Ignore ALT Key Up once
bool g_IgnoreAltKeyUpOnce=false; // Prevents that the menu bar appears if the user opens a search with Alt+Click in a new tab

// Shared settings
DWORD g_BlockPopUps=DEFAULT_POPUPBLOCKEROPTIONS;
DWORD g_BlockAds=DEFAULT_ADBLOCKEROPTIONS;
int g_FontSize=FONTSIZE_STANDARD;
int g_FontColor=FONTCOLOR_GREENBLUE;
UINT g_Options=DEFAULT_OPTIONS;
UINT g_Options2=DEFAULT_OPTIONS2;
UINT g_Buttons=DEFAULT_BUTTONS;
UINT g_Warnings=~0;
UINT g_Restrictions=0;
UINT g_Keys=~0;
bool g_ShowURL=true;
bool g_IDNSupport=true;
bool g_MiddleClick;

// Quero Thread Local Storage
QThreadData QThreadLocalStg[MAX_QUERO_INSTANCES];
int g_MaxUsedInstanceId=0;
UINT g_QueroInstanceCount;

// Search History
HistoryEntry g_History[HISTORYSIZE];
UINT g_HistoryIndex=0;
UINT g_LTimeHistory=0;
	
// URL History
BSTR g_URLs[URLHISTORYSIZE];
UINT g_nURLs=0; // URL history index
UINT g_LTimeURLs=0;
	
// Whitelist
WhiteListEntry g_WhiteList[WHITELISTSIZE];
UINT g_WhiteListIndex=0;
UINT g_LTimeWhiteList=0;

// Temporary Whitelist
WhiteListEntry g_TemporaryWhiteList[MAX_QUERO_INSTANCES];
UINT g_TemporaryWhiteListIndex=0;

// Quero Shared Memory

QSharedMemory* g_QSharedMemory=NULL;
HANDLE g_hQSharedMemoryMutex=NULL;
HANDLE g_hQSharedMemoryFileMapping=NULL;
HANDLE g_hQSharedListMutex=NULL;

// Content Filter
CContentFilter g_ContentFilter;

// The original showModelessDialog method
FP_HTMLWINDOW3SHOWMODELESS ORIG_HTMLWINDOW3SHOWMODELESS=NULL;

// The original write methods
FP_HTMLDOCUMENT2WRITE ORIG_HTMLDOCUMENT2WRITE=NULL;
FP_HTMLDOCUMENT2WRITELN ORIG_HTMLDOCUMENT2WRITELN=NULL;

// The original put_inner/outerHTML and insertAdjacentHTML methods
FP_HTMLELEMENT_PUT_HTML ORIG_HTMLELEMENT_PUT_INNERHTML=NULL;
FP_HTMLELEMENT_PUT_HTML ORIG_HTMLELEMENT_PUT_OUTERHTML=NULL;
FP_HTMLELEMENT_INSERTADJACENTHTML ORIG_HTMLELEMENT_INSERTADJACENTHTML=NULL;

// The original createElement method
FP_HTMLDOCUMENT2CREATEELEMENT ORIG_HTMLDOCUMENT2CREATEELEMENT=NULL;

// The original DocHostUIHandler.ShowContextMenu method
FP_DOCHOSTUIHANDLER_SHOWCTXMENU ORIG_DOCHOSTUIHANDLER_SHOWCTXMENU=NULL;

// COM pointers to distinguish COM object types
ULONG_PTR COM_IHTMLDocument2_PTR;
ULONG_PTR COM_IHTMLElement_PTR;

/////////////////////////////////////////////////////////////////////////////
// CQueroBand

CQueroBand::CQueroBand():
	m_pSite(NULL),
	m_pWebBrowser2(NULL),
	m_pTopLevelDisp(NULL),
	m_dwBandID(0),
	m_dwViewMode(0),
	m_bShow(FALSE),
	m_bEnterHelpMode(FALSE),
	m_hWndParent(NULL),
	m_bBlockNewWindow(TRUE),
	m_dwBrowserCookie(0),
	m_pOrigDocHostUIHandler(NULL),
	m_ContextMenuInstalled(false)
{
	m_Toolbar.GetComboQuero()->SetBand(this);
	m_Toolbar.GetComboEngine()->SetBand(this);
	m_Toolbar.SetBand(this);
	bFirstNavigation=true;
	bDisplayBandOnNewLine=m_Toolbar.IsFirstRun();
}

CQueroBand::~CQueroBand()
{
	if(m_pOrigDocHostUIHandler) m_pOrigDocHostUIHandler->Release();
	if(m_pWebBrowser2) m_pWebBrowser2->Release();
	if(m_pSite) m_pSite->Release();
}

HRESULT CQueroBand::RegisterAndCreateWindow()
{
	RECT rect = {0,0,0,0};

	const DWORD DEFAULT_TOOLBAR_STYLE = 
      /*Window styles:*/ WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | WS_TABSTOP |
      /*Toolbar styles:*/ TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_TRANSPARENT | TBSTYLE_LIST |
                         /* TBSTYLE_WRAPABLE | */
      /*Common Control styles:*/ CCS_TOP | CCS_NODIVIDER | CCS_NOPARENTALIGN | CCS_NORESIZE ;

	return (m_Toolbar.Create(m_hWndParent, rect, NULL, DEFAULT_TOOLBAR_STYLE))?S_OK:E_FAIL;
}

// IDeskBand
STDMETHODIMP CQueroBand::GetBandInfo(DWORD dwBandID, DWORD dwViewMode, DESKBANDINFO* pdbi)
{
	m_dwBandID = dwBandID;
	m_dwViewMode = dwViewMode;

	if(pdbi)
	{
		int toolbarminwidth=m_Toolbar.GetToolbarMinWidth();
		int toolbarheight=m_Toolbar.GetToolbarHeight();

		if(pdbi->dwMask & DBIM_MINSIZE)
		{
			pdbi->ptMinSize.x = toolbarminwidth;
			pdbi->ptMinSize.y = toolbarheight;
		}
		if(pdbi->dwMask & DBIM_MAXSIZE)
		{
			pdbi->ptMaxSize.x = 0; // Ignored
			pdbi->ptMaxSize.y = toolbarheight;
		}
		if(pdbi->dwMask & DBIM_INTEGRAL)
		{
			pdbi->ptIntegral.x = 0; // Ignored
			pdbi->ptIntegral.y = 0; // Not sizeable
		}
		if(pdbi->dwMask & DBIM_ACTUAL) // Ideal size
		{
			pdbi->ptActual.x = toolbarminwidth;
			if(g_Options2&OPTION2_ShowSearchBox) pdbi->ptActual.x<<=1;
			pdbi->ptActual.y = toolbarheight;
		}
		if(pdbi->dwMask & DBIM_TITLE)
		{
			pdbi->wszTitle[0]=0; // No title
		}
		/*if(pdbi->dwMask & DBIM_BKCOLOR)
		{
			pdbi->crBkgnd=0;
		}*/
		if(pdbi->dwMask & DBIM_MODEFLAGS)
		{
			pdbi->dwModeFlags = DBIMF_NORMAL;
		}
	}

	return S_OK;
}

// IOleWindow
STDMETHODIMP CQueroBand::GetWindow(HWND* phwnd)
{
	HRESULT hr;

	if(phwnd)
	{
		*phwnd = m_Toolbar.m_hWnd;
		hr = S_OK;
	}
	else hr = E_INVALIDARG;

	return hr;
}

STDMETHODIMP CQueroBand::ContextSensitiveHelp(BOOL fEnterMode)
{
	m_bEnterHelpMode = fEnterMode;
	return S_OK;
}

// IDockingWindow
STDMETHODIMP CQueroBand::CloseDW(unsigned long dwReserved)
{
	// Workaround: Prevents that the toolbar disappears when an IE window closes where only one tab is opened
	if(m_hWndParent)
	{
		if(g_IE_MajorVersion>=8)
		{
			// Freeze complete toolbar area ("Menu Bar", WorkerW)
			SendMessage(GetParent(m_hWndParent),WM_SETREDRAW,FALSE,0);
		}
		else if(g_IE_MajorVersion==7)
		{
			LockWindowUpdate(m_hWndParent);
		}
	}
	//ShowDW(FALSE);
	return S_OK;
}

STDMETHODIMP CQueroBand::ResizeBorderDW(const RECT* prcBorder, IUnknown* punkToolbarSite, BOOL fReserved)
{
	// Not used by any band object.
	return E_NOTIMPL;
}

STDMETHODIMP CQueroBand::ShowDW(BOOL bShow)
{
	if(bDisplayBandOnNewLine && bShow && m_hWndParent)
	{
		int nCount = (int)::SendMessage(m_hWndParent, RB_GETBANDCOUNT, 0, 0L);

		for(int i = 0; i < nCount; i++)
		{
			REBARBANDINFO rbbi = { sizeof(REBARBANDINFO), RBBIM_CHILD | RBBIM_STYLE};
			BOOL bRet = (BOOL)::SendMessage(m_hWndParent, RB_GETBANDINFO, i, (LPARAM)&rbbi);
			if(bRet && rbbi.hwndChild == m_Toolbar.m_hWnd)
			{
				rbbi.fMask = RBBIM_STYLE ;
				rbbi.fStyle |= RBBS_BREAK;
			
				::SendMessage(m_hWndParent, RB_SETBANDINFO, i, (LPARAM)&rbbi);
			}
		}
	}

	// Create toolbar UI, load profiles, history and icons
	if(bShow && m_Toolbar.bToolbarCreated==false) m_Toolbar.CreateDeferred();

	m_bShow = bShow;

	m_Toolbar.ShowWindow(m_bShow ? SW_SHOW : SW_HIDE);

	return S_OK;
}

// IObjectWithSite
STDMETHODIMP CQueroBand::SetSite(IUnknown* pUnkSite)
{
	HRESULT hr;
	bool bSiteChanged;

	// Release old pWebBrowser2
	if(m_pWebBrowser2)
	{
		ManageBrowserConnection(UNADVISE);
		m_pTopLevelDisp = NULL;
		m_Toolbar.SetBrowser(NULL);
		m_hWndParent = NULL;
		m_pWebBrowser2->Release();
		m_pWebBrowser2 = NULL;
	}

	// If a site is being held, release it
	if(m_pSite)
	{
		m_pSite->Release();
		m_pSite = NULL;
		bSiteChanged=true;
	}
	else bSiteChanged=false;

	// Release m_pOrigDocHostUIHandler
	if(m_pOrigDocHostUIHandler)
	{
		m_pOrigDocHostUIHandler->Release();
		m_pOrigDocHostUIHandler=NULL;
	}

	if(pUnkSite)
	{
		IOleWindow *pOleWindow = NULL;
		IOleCommandTarget* pCmdTarget = NULL;

		// Get the parent window
		hr=pUnkSite->QueryInterface(IID_IOleWindow, (LPVOID*)&pOleWindow);
		if(SUCCEEDED_OK(hr))
		{
			hr=pOleWindow->GetWindow(&m_hWndParent);
			pOleWindow->Release();
		}

		if(SUCCEEDED_OK(hr))
		{
			// Get and keep the IInputObjectSite pointer
			hr=pUnkSite->QueryInterface(IID_IInputObjectSite,(LPVOID*)&m_pSite);
			if(SUCCEEDED_OK(hr))
			{
				// Get the IWebBrowser2 pointer				
				hr = pUnkSite->QueryInterface(IID_IOleCommandTarget,(LPVOID*)&pCmdTarget);
				if(SUCCEEDED_OK(hr))
				{
					IServiceProvider* pServiceProvider;

					hr = pCmdTarget->QueryInterface(IID_IServiceProvider, (LPVOID*)&pServiceProvider);
					if(SUCCEEDED_OK(hr))
					{
						hr = pServiceProvider->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, (LPVOID*)&m_pWebBrowser2);
						if(SUCCEEDED_OK(hr))
						{
							m_Toolbar.SetBrowser(m_pWebBrowser2);

							// Connect to the browser in order to handle events
							ManageBrowserConnection(ADVISE);

							// Cretae the main toolbar window
							if(bSiteChanged) m_Toolbar.OnSiteChange();
							else hr=RegisterAndCreateWindow();
						}
						pServiceProvider->Release();
					}
					pCmdTarget->Release();
				}
			}
		}
	}
	else hr=S_OK;

	return hr;
}

STDMETHODIMP CQueroBand::GetSite(REFIID riid, void **ppvSite)
{
	HRESULT hr;

	*ppvSite = NULL;

	if(m_pSite) hr=m_pSite->QueryInterface(riid, ppvSite);
	else hr=E_FAIL;

	return hr;
}

void CQueroBand::FocusChange(BOOL bHaveFocus)
{
	if(m_pSite)
	{
		IUnknown* pUnkown=NULL;
		if(SUCCEEDED_OK(QueryInterface(IID_IUnknown,(LPVOID*)&pUnkown)) && pUnkown)
		{
			// Remove the focus from the Quero Toolbar in IE7
			if(bHaveFocus==false && g_IE_MajorVersion>=7 && m_hWndParent) if(m_Toolbar.SetFocusOnIEServerWindow()!=S_OK) ::SetFocus(m_hWndParent);

			m_pSite->OnFocusChangeIS(pUnkown, bHaveFocus);
			pUnkown->Release();
		}
	}
}

STDMETHODIMP CQueroBand::HasFocusIO(void)
{
	// If any of the windows in our toolbar has focus then return S_OK else S_FALSE.
	return IsChild(m_Toolbar.m_hWnd,::GetFocus())?S_OK:S_FALSE;
}

STDMETHODIMP CQueroBand::TranslateAcceleratorIO(LPMSG lpMsg)
{
	bool bCtrlKeyDown=::GetKeyState(VK_CONTROL)<0; // Control key pressed?
	int nVirtKey = (int)(lpMsg->wParam);
	HRESULT result=-1;

	if(lpMsg->message == WM_KEYDOWN)
	{
		// Handle Tab key
		if(nVirtKey == VK_TAB && !bCtrlKeyDown)
		{
			bool bShiftKeyDown=::GetKeyState(VK_SHIFT)<0;
			HWND focus=::GetFocus();

			if(m_Toolbar.GetComboQuero()->m_hWndEdit == focus)
			{
				// Test if AutoComplete is active
				if(g_Options&OPTION_ACUseTab)
				{
					m_Toolbar.GetComboQuero()->bTextChanged=false;
					DispatchMessage(lpMsg); // Pass TAB to AutoComplete
					if(m_Toolbar.GetComboQuero()->bTextChanged) result=S_OK; // AutoComplete is active
				}

				// Handle Tab if AutoCompelte is not active
				if(result==-1)
				{
					if(bShiftKeyDown)
					{
						//FocusChange(FALSE);
						result=S_FALSE; // Jump to previous toolbar
					}
					else
					{			
						m_Toolbar.GetComboEngine()->SetFocus();
						if((g_Options&OPTION_ShowSearchEngineComboBox)==0) m_Toolbar.GetComboEngine()->SendMessage(CB_SHOWDROPDOWN,TRUE,0);
						result=S_OK;
					}
				}
			}
			else if(m_Toolbar.GetComboEngine()->m_hWnd == focus)
			{
				if(bShiftKeyDown && (g_Options2&OPTION2_ShowSearchBox))
				{
					m_Toolbar.GetComboQuero()->GetEditCtrl()->SetFocus();
					result=S_OK;
				}
				else
				{
					//FocusChange(FALSE);
					result=S_FALSE; // Jump to next toolbar
				}
			}
		}
		
		// Handle IE keyboard shortcuts
		else if(nVirtKey == VK_F5 || (bCtrlKeyDown && nVirtKey == 'R')) // Refresh
		{
			FocusChange(FALSE);
			if(g_ShowURL) m_Toolbar.GetComboQuero()->SetTextCurrentURL();
			result=S_FALSE;
		}
		else if(nVirtKey >= VK_F1 && nVirtKey <= VK_F12) result=S_FALSE;
		else if(bCtrlKeyDown)
		{
			if(nVirtKey == 'F')
			{
				IDispatch *pDispatch=NULL;
				HRESULT hr;
			
				// Invoke "Find on page" dialog
				if(m_pWebBrowser2)
				{
					hr=m_pWebBrowser2->get_Document(&pDispatch);
					if(SUCCEEDED_OK(hr) && pDispatch)
					{
						IOleCommandTarget *pOleCommandTarget=NULL;

						hr=pDispatch->QueryInterface(IID_IOleCommandTarget, (LPVOID*)&pOleCommandTarget);
						pDispatch->Release();
						if(SUCCEEDED_OK(hr) && pOleCommandTarget)
						{
							pOleCommandTarget->Exec(&CGID_IWebBrowser, /* CmdId for Find dialog */ 1, 0, NULL, NULL);
							pOleCommandTarget->Release();
						}
					}
				}
				result=S_OK;
			}
			else if(nVirtKey == 'W')
			{
				// Close the tab
				if(m_pWebBrowser2) m_pWebBrowser2->Quit();
				result=S_OK;
			}
			else
			{
				#define N_EDITCTRLVIRTKEYS 12
				const int EditCtrlVirtKeys[N_EDITCTRLVIRTKEYS]={VK_RETURN,'A','X','C','V','Z',VK_LEFT,VK_RIGHT,VK_UP,VK_DOWN,L'L',VK_INSERT};
				int i;

				i=0;
				while(i<N_EDITCTRLVIRTKEYS && nVirtKey!=EditCtrlVirtKeys[i]) i++;

				if(i>=N_EDITCTRLVIRTKEYS) result=S_FALSE;
			}
		}
	}
	else if(lpMsg->message == WM_SYSKEYDOWN)
	{
		if(nVirtKey == VK_HOME || nVirtKey == VK_LEFT || nVirtKey == VK_RIGHT)
		{
			result=S_FALSE;
		}
	}

	// The only window that needs to translate messages is our edit box so forward them
	if(result==-1) result=m_Toolbar.GetComboQuero()->TranslateAcceleratorIO(lpMsg);

	return result;
}

STDMETHODIMP CQueroBand::UIActivateIO(BOOL fActivate, LPMSG lpMsg)
{
	// If our deskband is being activated then set focus in the edit box.
	if(fActivate && lpMsg!=NULL && lpMsg->message!=WM_LBUTTONDOWN)
	{
		if(g_Options2&OPTION2_ShowSearchBox)
			m_Toolbar.GetComboQuero()->GetEditCtrl()->SetFocus();
		else if(g_Options&OPTION_ShowSearchEngineComboBox)
			m_Toolbar.GetComboEngine()->SetFocus();
		else return E_FAIL;
	}
	return S_OK;
}

bool CQueroBand::ShowQueroToolbar(IWebBrowser2 *pWebBrowser,VARIANT_BOOL vbShow)
{
	VARIANT vCLSID,vShow,vSize;

	vCLSID.vt=VT_BSTR;
	vCLSID.bstrVal=CComBSTR(L"{A411D7F4-8D11-43EF-BDE4-AA921666388A}");
	vShow.vt=VT_BOOL;
	vShow.boolVal=vbShow;
	vSize.vt=VT_EMPTY;

	return SUCCEEDED_OK(pWebBrowser->ShowBrowserBar(&vCLSID,&vShow,&vSize));
}

//
// Funnel web browser events through this class
//
HRESULT CQueroBand::ManageBrowserConnection(int eConnectType)
{
	HRESULT hr;

	if(m_pWebBrowser2)
	{
		if(eConnectType==ADVISE)
		{
			if(m_dwBrowserCookie==0) hr=AtlAdvise(m_pWebBrowser2, (IDispatch*)this, __uuidof(DWebBrowserEvents2), &m_dwBrowserCookie);
			else hr=S_OK; // Already advised
		}
		else
		{
			if(m_dwBrowserCookie)
			{
				hr=AtlUnadvise(m_pWebBrowser2, __uuidof(DWebBrowserEvents2), m_dwBrowserCookie);
				m_dwBrowserCookie=0;
			}
			else hr=S_OK; // Not advised, nothing to do
		}
	}
	else hr=S_OK;

	return hr;
}

//
// IDispatch Methods
//

#define DISPID_WINDOWSTATECHANGED 283
#define OLECMDIDF_WINDOWSTATE_USERVISIBLE 0x00000001
#define OLECMDIDF_WINDOWSTATE_ENABLED 0x00000002
#define OLECMDIDF_WINDOWSTATE_USERVISIBLE_VALID 0x00010000
#define OLECMDIDF_WINDOWSTATE_ENABLED_VALID 0x00020000

STDMETHODIMP CQueroBand::Invoke(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
							 DISPPARAMS* pDispParams, VARIANT* pvarResult,
							 EXCEPINFO*  pExcepInfo,  UINT* puArgErr)
{
	if(!pDispParams) return E_INVALIDARG;

	switch(dispidMember)
	{

	case DISPID_COMMANDSTATECHANGE:
		// The parameters for this DISPID:
		// [0]: Enabled state - VT_BOOL
		// [1]: Command identifier - VT_I4
		switch(pDispParams->rgvarg[1].intVal)
		{
		case CSC_NAVIGATEBACK:
			m_Toolbar.GetNavBar()->EnableButton(IDM_BACK,pDispParams->rgvarg[0].boolVal);
			break;
		case CSC_NAVIGATEFORWARD:
			m_Toolbar.GetNavBar()->EnableButton(IDM_FORWARD,pDispParams->rgvarg[0].boolVal);
			break;
		}
		break;

	case DISPID_WINDOWSTATECHANGED:
		// The parameters for this DISPID:
		// [0]: dwValidFlagsMask - VT_I4
		// [1]: dwFlags - VT_I4		
		bool NewActiveState;

		// Update IsActive

		NewActiveState=(pDispParams->rgvarg[1].intVal&OLECMDIDF_WINDOWSTATE_USERVISIBLE);
		if(NewActiveState==false && m_Toolbar.IsActive) // Retain active state if window is minimized
		{
			WINDOWPLACEMENT wp;
			GetWindowPlacement(m_Toolbar.GetIEFrameWindow(),&wp);

			NewActiveState=(wp.showCmd==SW_SHOWMINIMIZED);
		}

		if(m_Toolbar.IsActive!=NewActiveState)
		{
			m_Toolbar.IsActive=NewActiveState;

			if(m_Toolbar.IsActive)
			{
				// Update icon
				if(g_Options&OPTION_ShowFavIconsInTaskbar)
				{
					HICON hIcon;

					hIcon=m_Toolbar.GetFavIcon();
					m_Toolbar.SetIcon_IEFrame(hIcon);
				}
				// Update window title
				m_Toolbar.SetTitle_IEFrame();
			}
		}
		break;
	
	case DISPID_STATUSTEXTCHANGE:
		// The parameters for this DISPID:
		// [0]: New status bar text - VT_BSTR
		break;

	case DISPID_PROGRESSCHANGE:
		// The parameters for this DISPID:
		// [0]: Maximum progress - VT_I4
		// [1]: Amount of total progress - VT_I4
		int progress;

		progress=pDispParams->rgvarg[1].intVal;

		// Call OnProgressChange if there is real progress and document is not completed
		if(progress && (progress%1000)==0) m_Toolbar.OnProgressChange(progress);

		break;

	case DISPID_SETSECURELOCKICON:
		// [0]: SecureLockIconConstants VT_I4
		m_Toolbar.OnSetSecureLockIcon((SecureLockIconConstants)pDispParams->rgvarg[0].intVal);
		break;

	case DISPID_BEFORENAVIGATE2:
		//
		// The parameters for this DISPID are as follows:
		// [0]: Cancel flag  - VT_BYREF|VT_BOOL
		// [1]: HTTP headers - VT_BYREF|VT_VARIANT
		// [2]: Address of HTTP POST data  - VT_BYREF|VT_VARIANT 
		// [3]: Target frame name - VT_BYREF|VT_VARIANT 
		// [4]: Option flags - VT_BYREF|VT_VARIANT
		// [5]: URL to navigate to - VT_BYREF|VT_VARIANT
		// [6]: An object that evaluates to the top-level or frame
		//      WebBrowser object corresponding to the event. 
		// 
		// User clicked a link or launched the browser.
		//
		//m_ReflectWnd.GetToolBar().OnBeforeNavigate(pDispParams->rgvarg[5].pvarVal->bstrVal);
		bool toplevel;

		if(m_pTopLevelDisp==NULL) toplevel=true;
		else toplevel = (m_pTopLevelDisp==pDispParams->rgvarg[6].pdispVal);
		
		m_Toolbar.OnBeforeNavigate(
			pDispParams->rgvarg[6].pdispVal,
			pDispParams->rgvarg[5].pvarVal,
			pDispParams->rgvarg[4].pvarVal,
			pDispParams->rgvarg[3].pvarVal,
			pDispParams->rgvarg[2].pvarVal,
			pDispParams->rgvarg[1].pvarVal,  // header pDispParams->rgvarg[1].pvarVal->ppbstrVal;
			pDispParams->rgvarg[0].pboolVal,
			bFirstNavigation,toplevel);

		break;
		
	case DISPID_NAVIGATECOMPLETE2:
		//
		// The parameters for this DISPID:
		// [0]: URL navigated to - VT_BYREF|VT_VARIANT
		// [1]: An object that evaluates to the top-level or frame
		//      WebBrowser object corresponding to the event. 
		//
		// Fires after a navigation to a link is completed on either 
		// a window or frameSet element.
		//
		if(m_pTopLevelDisp==NULL)
		{
			// This is the IDispatch* of the top-level browser
			m_pTopLevelDisp = pDispParams->rgvarg[1].pdispVal;

			// Install pop-up blocker
			if(g_BlockPopUps&POPUPBLOCKER_Enable) InstallPopUpBlocker(true);

			// Install ad blocker
			if(g_BlockAds&ADBLOCKER_Enable) InstallAdBlocker(true);

			if(bFirstNavigation)
			{
				// Install DocHostUIHandler
				if(g_Options2&OPTION2_EnableQueroContextMenu) InstallContextMenu(true);

				bFirstNavigation=false;
				m_Toolbar.OnNavigateBrowser(pDispParams->rgvarg[0].pvarVal->bstrVal,true);
			}
			else
			{
				m_Toolbar.OnNavigateBrowser(pDispParams->rgvarg[0].pvarVal->bstrVal,false);
			}
		}
		else if(m_pTopLevelDisp==pDispParams->rgvarg[1].pdispVal) // pass URL only for top-level 
			m_Toolbar.OnNavigateBrowser(pDispParams->rgvarg[0].pvarVal->bstrVal,false);
	
		// Any new windows that might pop up after this (due to script) 
		// should be blocked.
		//m_bBlockNewWindow = TRUE;	// Reset
		break;

	case DISPID_NAVIGATEERROR:
		// The parameters for this DISPID are as follows:
		// [0]: cancel flag
		// [1]: status code: pDispParams->rgvarg[1].pvarVal->lVal;
		// [2]: 
		// [3]: URL
		// [4]: IDispatch: pDispParams->rgvarg[4].pdispVal;
		// [5]: 
		// [6]:
		if (m_pTopLevelDisp && m_pTopLevelDisp==pDispParams->rgvarg[4].pdispVal)
		{
			m_Toolbar.OnNavigateError(pDispParams->rgvarg[3].pvarVal->bstrVal,pDispParams->rgvarg[1].pvarVal->lVal,pDispParams->rgvarg[0].pboolVal);
		}
		
		break;

	case DISPID_DOCUMENTCOMPLETE:
		//
		// The parameters for this DISPID:
		// [0]: URL navigated to - VT_BYREF|VT_VARIANT
		// [1]: An object that evaluates to the top-level or frame
		//      WebBrowser object corresponding to the event. 
		//
		// Fires when a document has been completely loaded and initialized.
		// Unreliable -- currently, the DWebBrowserEvents2::DocumentComplete 
		// does not fire when the IWebBrowser2::Visible property of the 
		// WebBrowser Control is set to false (see Q259935).  Also, multiple 
		// DISPID_DOCUMENTCOMPLETE events can be fired before the final 
		// READYSTATE_COMPLETE (see Q180366).
		//
		READYSTATE ready;
		// Call onDocumentComplete for every completed frame
		if(m_pWebBrowser2)
		{
			m_pWebBrowser2->get_ReadyState(&ready);
			if(ready==READYSTATE_COMPLETE) m_Toolbar.OnDocumentComplete();
		}
		break;

	case DISPID_DOWNLOADBEGIN:
		// No parameters
		// Fires when a navigation operation is beginning.
		m_Toolbar.OnDownloadBegin();
		break;
	
	case DISPID_DOWNLOADCOMPLETE:
		// No parameters
		// Fires when a navigation operation finishes, is halted, or fails.
		m_Toolbar.OnDownloadComplete();
		break;
		
	case DISPID_TITLECHANGE:
		//
		// The parameters for this DISPID:
		// [0]: Document title - VT_BSTR
		// [1]: An object that evaluates to the top-level or frame
		//      WebBrowser object corresponding to the event.
		//
		m_Toolbar.OnTitleChange(pDispParams->rgvarg[0].bstrVal);
		break;

/*	case DISPID_NEWWINDOW2:
		// [0]: Cancel flag  - VT_BYREF|VT_BOOL
		// [1]: IDispatch* - Pointer to an IDispatch interface. 
		//MessageBox(NULL,L"newwindow2",L"",MB_OK);
		if (m_bBlockNewWindow)
		{
			// Play a sound when a popup is blocked.
			// Some links open a new window and will be blocked.
			// The sound gives some indication to the user as to
			// whats happening; otherwise, the link will appear to
			// be broken.

			::MessageBeep(MB_ICONEXCLAMATION);

			// Set the cancel flag
			pDispParams->rgvarg[0].pvarVal->vt = VT_BOOL;
			pDispParams->rgvarg[0].pvarVal->boolVal = VARIANT_TRUE;

			*pDispParams->rgvarg[0].pboolVal=VARIANT_TRUE;;
		}
		m_bBlockNewWindow = TRUE;	// Reset
		break;
*/

	case 273 /* DISPID_NEWWINDOW3 */:
		// [0]: bstrUrl
		// [1]: bstrUrlContext
		// [2]: NWMF dwFlags
		// [3]: VARIANT_BOOL Cancel
		// [4]: IDispatch **ppDisp
		
		if(g_BlockPopUps&POPUPBLOCKER_Enable)
			m_Toolbar.OnNewWindow3(
				pDispParams->rgvarg[4].ppdispVal,
				pDispParams->rgvarg[3].pboolVal,
				pDispParams->rgvarg[2].intVal,
				pDispParams->rgvarg[1].bstrVal,
				pDispParams->rgvarg[0].bstrVal);
		break;

/*
		//
		// The parameters for this DISPID:
		// [0]: Name of property that changed - VT_BSTR
		//
	case DISPID_PROPERTYCHANGE:
		break;
		
		//
		// The parameters for this DISPID:
		// [0]: Address of cancel flag - VT_BYREF|VT_BOOL
		//
*/
	case DISPID_ONTHEATERMODE:
	case DISPID_ONFULLSCREEN:
		QD(L"fullscreen");
		break;

	case DISPID_QUIT:
		ManageBrowserConnection(UNADVISE);
		break;
   }
   
   return S_OK;
}

// IDocHostUIHandler methods
STDMETHODIMP CQueroBand::EnableModeless(BOOL fEnable)
{
	return (m_pOrigDocHostUIHandler)?m_pOrigDocHostUIHandler->EnableModeless(fEnable):E_NOTIMPL;
}

STDMETHODIMP CQueroBand::FilterDataObject(IDataObject *pDO, IDataObject **ppDORet)
{
	return (m_pOrigDocHostUIHandler)?m_pOrigDocHostUIHandler->FilterDataObject(pDO,ppDORet):S_FALSE;
}

STDMETHODIMP CQueroBand::GetDropTarget(IDropTarget *pDropTarget, IDropTarget **ppDropTarget)
{
	return (m_pOrigDocHostUIHandler)?m_pOrigDocHostUIHandler->GetDropTarget(pDropTarget,ppDropTarget):E_NOTIMPL;
}

STDMETHODIMP CQueroBand::GetExternal(IDispatch **ppDispatch)
{
	if(ppDispatch) *ppDispatch=NULL;
	return (m_pOrigDocHostUIHandler)?m_pOrigDocHostUIHandler->GetExternal(ppDispatch):E_NOTIMPL;
}

STDMETHODIMP CQueroBand::GetHostInfo(DOCHOSTUIINFO *pInfo)
{
	return (m_pOrigDocHostUIHandler)?m_pOrigDocHostUIHandler->GetHostInfo(pInfo):S_FALSE;
}

STDMETHODIMP CQueroBand::GetOptionKeyPath(LPOLESTR *pchKey,DWORD dw)
{
	return (m_pOrigDocHostUIHandler)?m_pOrigDocHostUIHandler->GetOptionKeyPath(pchKey,dw):E_NOTIMPL;
}

STDMETHODIMP CQueroBand::HideUI(VOID)
{
	return (m_pOrigDocHostUIHandler)?m_pOrigDocHostUIHandler->HideUI():S_OK;
}

STDMETHODIMP CQueroBand::OnDocWindowActivate(BOOL fActivate)
{
	return (m_pOrigDocHostUIHandler)?m_pOrigDocHostUIHandler->OnDocWindowActivate(fActivate):S_OK;
}

STDMETHODIMP CQueroBand::OnFrameWindowActivate(BOOL fActivate)
{
	return (m_pOrigDocHostUIHandler)?m_pOrigDocHostUIHandler->OnFrameWindowActivate(fActivate):S_OK;
}

STDMETHODIMP CQueroBand::ResizeBorder(LPCRECT prcBorder,IOleInPlaceUIWindow *pUIWindow,BOOL fFrameWindow)
{
	return (m_pOrigDocHostUIHandler)?m_pOrigDocHostUIHandler->ResizeBorder(prcBorder,pUIWindow,fFrameWindow):S_OK;
}

STDMETHODIMP CQueroBand::ShowContextMenu(DWORD dwID,POINT *ppt,IUnknown *pcmdtReserved,IDispatch *pdispReserved)
{
	HRESULT hr;
	
	hr=DocHostUIHandler_ShowContextMenu(this,dwID,ppt,pcmdtReserved,pdispReserved);
	if(hr!=S_OK && m_pOrigDocHostUIHandler) hr=m_pOrigDocHostUIHandler->ShowContextMenu(dwID,ppt,pcmdtReserved,pdispReserved);

	return hr;
}

STDMETHODIMP CQueroBand::ShowUI(DWORD dwID,IOleInPlaceActiveObject *pActiveObject,IOleCommandTarget *pCommandTarget,IOleInPlaceFrame *pFrame,IOleInPlaceUIWindow *pDoc)
{
	return (m_pOrigDocHostUIHandler)?m_pOrigDocHostUIHandler->ShowUI(dwID,pActiveObject,pCommandTarget,pFrame,pDoc):S_FALSE;
}

STDMETHODIMP CQueroBand::TranslateAccelerator(LPMSG lpMsg,const GUID *pguidCmdGroup,DWORD nCmdID)
{
	return (m_pOrigDocHostUIHandler)?m_pOrigDocHostUIHandler->TranslateAccelerator(lpMsg,pguidCmdGroup,nCmdID):S_FALSE;
}

STDMETHODIMP CQueroBand::TranslateUrl(DWORD dwTranslate,OLECHAR *pchURLIn,OLECHAR **ppchURLOut)
{
	return (m_pOrigDocHostUIHandler)?m_pOrigDocHostUIHandler->TranslateUrl(dwTranslate,pchURLIn,ppchURLOut):S_FALSE;
}

STDMETHODIMP CQueroBand::UpdateUI(VOID)
{
	return (m_pOrigDocHostUIHandler)?m_pOrigDocHostUIHandler->UpdateUI():S_OK;
}

// IOleCommandTarget methods
STDMETHODIMP CQueroBand::QueryStatus(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT *pCmdText)
{
	return E_NOTIMPL;
}

STDMETHODIMP CQueroBand::Exec(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdExecOpt, VARIANT *pvaIn, VARIANT *pvaOut)
{
	return nCmdID==OLECMDID_SHOWSCRIPTERROR?S_OK:E_NOTIMPL;
}

// Global Functions

QThreadData* GetCurrentQueroInstance()
{
	int i;
	DWORD ThreadId;
	QThreadData *QueroInstance;

	QueroInstance=NULL;
	ThreadId=GetCurrentThreadId();
	if(ThreadId)
	{
		for(i=0;i<=g_MaxUsedInstanceId;i++)
			if(QThreadLocalStg[i].ThreadId==ThreadId)
			{
				QueroInstance=&QThreadLocalStg[i];
				break;
			}
	}

	QDEBUG_CODE if(QueroInstance==NULL) QDEBUG_PRINTF(L"GetCurrentQueroInstance",L"thread [%x] not found",ThreadId);

	return QueroInstance;
}

QThreadData* GetFirstQueroInstance()
{
	int i;
	QThreadData *QueroInstance;

	QueroInstance=NULL;
	for(i=0;i<=g_MaxUsedInstanceId;i++)
		if(QThreadLocalStg[i].hIEWnd)
		{
			QueroInstance=&QThreadLocalStg[i];
			break;
		}

	QDEBUG_CODE if(QueroInstance==NULL) QDEBUG_PRINTF(L"GetFirstQueroInstance",L"no instance found");

	return QueroInstance;
}

QThreadData* GetQueroInstanceActiveTab()
{
	int i;
	DWORD ThreadId;
	QThreadData *QueroInstance;

	QueroInstance=NULL;
	ThreadId=GetCurrentThreadId();
	if(ThreadId)
	{
		for(i=0;i<=g_MaxUsedInstanceId;i++)
			if(QThreadLocalStg[i].ThreadId_IEWnd==ThreadId && IsWindowVisible(QThreadLocalStg[i].pToolbar->m_hWnd))
			{
				QueroInstance=&QThreadLocalStg[i];
				break;
			}
	}

	QDEBUG_CODE if(QueroInstance==NULL) QDEBUG_PRINT(L"GetQueroInstanceActiveTab",L"not found");

	return QueroInstance;
}

#ifndef COMPILE_FOR_WINDOWS_VISTA
WNDPROC ORIG_IEFrame_WndProc=NULL;
LRESULT CALLBACK IEFrame_WndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_SETICON:
		if(wParam==ICON_SMALL_FAVICON)
		{
			wParam=ICON_SMALL;
		}
		else if(wParam==ICON_SMALL && (g_Options&OPTION_ShowFavIconsInTaskbar))
		{
			QThreadData *QueroInstance;
			bool HasFavIcon=false;

			// Get the Quero instance of the active tab
			if(WaitForSingleObject(g_hQSharedDataMutex,QMUTEX_TIMEOUT)==WAIT_OBJECT_0)
			{
				QueroInstance=GetQueroInstanceActiveTab();
				if(QueroInstance &&	QueroInstance->pToolbar->GetFavIcon()) HasFavIcon=true;
				ReleaseMutex(g_hQSharedDataMutex);
			}
			QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"IEFrame_WndProc 1");

			if(HasFavIcon) return NULL;
		}
		break;
	}

	return CallWindowProc(ORIG_IEFrame_WndProc,hwnd,uMsg,wParam,lParam);
}
#endif

HRESULT STDMETHODCALLTYPE HTMLWindow3ShowModelessWrapper(IHTMLWindow3 __RPC_FAR* pWin,
														 BSTR url,
														 VARIANT *varArgIn,
														 VARIANT *varOptions,
														 IDispatch __RPC_FAR *__RPC_FAR * pomWindowResult)
{
	bool AllowPopUp=false;
	HRESULT result;

	bool bControlKeyDown=::GetAsyncKeyState(VK_CONTROL)<0;
	bool bAltKeyDown=::GetAsyncKeyState(VK_MENU)<0;
			
	if(bControlKeyDown && (g_IE_MajorVersion<7 || bAltKeyDown))
	{
		AllowPopUp=true;
	}

	if(AllowPopUp==false)
	{
		// Check if current domain is in whitelist
		if(WaitForSingleObject(g_hQSharedDataMutex,QMUTEX_TIMEOUT)==WAIT_OBJECT_0)
		{
			QThreadData* QueroInstance;

			QueroInstance=GetCurrentQueroInstance();
			if(QueroInstance)
			{
				if(QueroInstance->pToolbar->GetWhiteListBlockPopUps(NULL))
				{
					QDEBUG_PRINT(L"HTMLWindow3ShowModelessWrapper",L"AddToBlockedContent");
					if(url) QueroInstance->pToolbar->AddToBlockedContent(BLOCKED_CONTENT_POPUP,url,NULL,false);
					QueroInstance->pToolbar->PopupBlocked();
				}
				else AllowPopUp=true;
			}

			ReleaseMutex(g_hQSharedDataMutex);
		}
		QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"HtmlWindow3");
	}

	if(AllowPopUp)
	{
		result=ORIG_HTMLWINDOW3SHOWMODELESS(pWin,url,varArgIn,varOptions,pomWindowResult);
	}
	else
	{
		// Block pop-up, but return S_OK to pretend operation was successful
		*pomWindowResult=NULL;		
		result=S_OK;
	}

	return result;
}

HRESULT STDMETHODCALLTYPE HTMLDocument2write(IHTMLDocument2 __RPC_FAR* pHtmlDocument,SAFEARRAY *psarray)
{
	HRESULT hr;

	//QD(L"HTMLDocument2write");

	if(CContentFilter::FilterDocumentWrite(pHtmlDocument,psarray,false))
	{
		hr=ORIG_HTMLDOCUMENT2WRITE(pHtmlDocument,psarray);
	}
	else hr=S_OK;
		
	return hr;

	/* WIN32
		// Check type of object to ensure that document.write is called and not blur()

		if(*((ULONG_PTR*)pHtmlDocument+4)==COM_IHTMLDocument2_PTR)
		{
			if(CContentFilter::FilterDocumentWrite(pHtmlDocument,psarray,false)==false)
				return S_OK;
		}

		// Restore caller stack frame
		__asm
		{
			mov esp,ebp;	// Restore esp
			add esp,4;		// Free ebp pointer
			mov ebp,dword ptr [ebp];

			jmp ORIG_HTMLDOCUMENT2WRITE; // Invoke original handler
		}
	   #pragma warning(disable : 4035) // Disable warning "no return value"
	*/
}

HRESULT STDMETHODCALLTYPE HTMLDocument2writeln(IHTMLDocument2 __RPC_FAR* pHtmlDocument,SAFEARRAY *psarray)
{
	HRESULT hr;

	//QD(L"HTMLDocument2writeln");

	if(CContentFilter::FilterDocumentWrite(pHtmlDocument,psarray,true))
	{
		hr=ORIG_HTMLDOCUMENT2WRITELN(pHtmlDocument,psarray);
	}
	else hr=S_OK;
		
	return hr;

	/* WIN32
		// Check type of object to ensure that document.writeln

		if(*((ULONG_PTR*)pHtmlDocument+4)==COM_IHTMLDocument2_PTR)
		{
			if(CContentFilter::FilterDocumentWrite(pHtmlDocument,psarray,true)==false)
				return S_OK;
		}

		// Restore caller stack frame
		__asm
		{
			mov esp,ebp;	// Restore esp
			add esp,4;		// Free ebp pointer
			mov ebp,dword ptr [ebp];

			jmp ORIG_HTMLDOCUMENT2WRITELN; // Invoke original handler
		}
		#pragma warning(disable : 4035) // Disable warning "no return value"
	*/
}

HRESULT STDMETHODCALLTYPE HTMLDocument2createElement(IHTMLDocument2 __RPC_FAR* pHtmlDocument,BSTR eTag,IHTMLElement **newElem)
{
	//QD(L"HTMLDocument2createElement");

	if(eTag && *eTag)
	{
		QDEBUG_PRINT(L"createElement",eTag);
		CContentFilter::FilterTag(pHtmlDocument,eTag,true);
	}
	return ORIG_HTMLDOCUMENT2CREATEELEMENT(pHtmlDocument,eTag,newElem);

	/* WIN32
		// Check object type to ensure that IHtmlDocument2::createElement is invoked

		//QDEBUG_PRINTF(L"setAttribute COM pointer",L"%x %x %x %x %x %x %x %x %x %x %x",*((DWORD*)pHtmlElement),*((DWORD*)pHtmlElement+1),*((DWORD*)pHtmlElement+2),*((DWORD*)pHtmlElement+3),*((DWORD*)pHtmlElement+4),*((DWORD*)pHtmlElement+5),*((DWORD*)pHtmlElement+6),*((DWORD*)pHtmlElement+7),*((DWORD*)pHtmlElement+8),*((DWORD*)pHtmlElement+9),*((DWORD*)pHtmlElement+10));

		if(*((ULONG_PTR*)pHtmlDocument+4)==COM_IHTMLDocument2_PTR)
		{		
			if(eTag && *eTag)
			{
				QDEBUG_PRINT(L"createElement",eTag);
				CContentFilter::FilterTag(pHtmlDocument,eTag,true);
			}
		}

		// Restore caller stack frame
		__asm
		{
			mov esp,ebp;	// Restore esp
			add esp,4;		// Free ebp pointer
			mov ebp,dword ptr [ebp];

			jmp ORIG_HTMLDOCUMENT2CREATEELEMENT; // Invoke original handler
		}
		#pragma warning(disable : 4035) // Disable warning "no return value"
	*/
}

HRESULT STDMETHODCALLTYPE HTMLElement_put_innerHTML(IHTMLElement __RPC_FAR* pHtmlElement,BSTR v)
{
	HRESULT hr;
	bool bPassInput;

	//QD(L"HTMLElement_put_innerHTML");

	if(v)
	{
		bPassInput=CContentFilter::FilterTag(pHtmlElement,v,false);
	}
	else bPassInput=true;

	if(bPassInput) hr=ORIG_HTMLELEMENT_PUT_INNERHTML(pHtmlElement,v);
	else hr=S_OK;

	return hr;

	/* WIN32
		// Check object type to ensure that IHtmlElement::put_innerHTML is invoked

		/*TCHAR text[100];
		StringCbPrintf(text,100,L"%x",*((DWORD*)pHtmlElement+4));
		MessageBox(NULL,text,L"com",MB_OK);

		if(*((ULONG_PTR*)pHtmlElement+4)==COM_IHTMLElement_PTR)
		{
			if(v)
			{
				if(CContentFilter::FilterTag(pHtmlElement,v,false)==false)
					return S_OK;
			}
		}

		// Restore caller stack frame
		__asm
		{
			mov esp,ebp;	// Restore esp
			add esp,4;		// Free ebp pointer
			mov ebp,dword ptr [ebp];

			jmp ORIG_HTMLELEMENT_PUT_INNERHTML; // Invoke original handler
		}
		#pragma warning(disable : 4035) // Disable warning "no return value"
	*/
}

HRESULT STDMETHODCALLTYPE HTMLElement_put_outerHTML(IHTMLElement __RPC_FAR* pHtmlElement,BSTR v)
{
	HRESULT hr;
	bool bPassInput;

	//QD(L"HTMLElement_put_outerHTML");

	if(v)
	{
		bPassInput=CContentFilter::FilterTag(pHtmlElement,v,false);
	}
	else bPassInput=true;

	if(bPassInput) hr=ORIG_HTMLELEMENT_PUT_OUTERHTML(pHtmlElement,v);
	else hr=S_OK;

	return hr;

	/* WIN32
		// Check type of object to ensure that element.put_outerHTML

		if(*((ULONG_PTR*)pHtmlElement+4)==COM_IHTMLElement_PTR)
		{
			if(v)
			{
				if(CContentFilter::FilterTag(pHtmlElement,v,false)==false)
					return S_OK;
			}
		}

		// Restore caller stack frame
		__asm
		{
			mov esp,ebp;	// Restore esp
			add esp,4;		// Free ebp pointer
			mov ebp,dword ptr [ebp];

			jmp ORIG_HTMLELEMENT_PUT_OUTERHTML; // Invoke original handler
		}
		#pragma warning(disable : 4035) // Disable warning "no return value"
	*/
}

HRESULT STDMETHODCALLTYPE HTMLElement_insertAdjacentHTML(IHTMLElement __RPC_FAR* pHtmlElement,BSTR where,BSTR html)
{
	HRESULT hr;
	bool bPassInput;

	//QD(L"HTMLElement_insertAdjacentHTML");

	if(html)
	{
		QDEBUG_PRINT(L"insertAdjacentHTML",html);
		bPassInput=CContentFilter::FilterTag(pHtmlElement,html,false);
	}
	else bPassInput=true;

	if(bPassInput) hr=ORIG_HTMLELEMENT_INSERTADJACENTHTML(pHtmlElement,where,html);
	else hr=S_OK;

	return hr;

	/* WIN32
		// Check type of object to ensure that element.insertAdjacentHTML

		if(*((ULONG_PTR*)pHtmlElement+4)==COM_IHTMLElement_PTR)
		{
			if(html)
			{
				QDEBUG_PRINT(L"insertAdjacentHTML",html);
				if(CContentFilter::FilterTag(pHtmlElement,html,false)==false)
					return S_OK;
			}
		}

		// Restore caller stack frame
		__asm
		{
			mov esp,ebp;	// Restore esp
			add esp,4;		// Free ebp pointer
			mov ebp,dword ptr [ebp];

			jmp ORIG_HTMLELEMENT_INSERTADJACENTHTML; // Invoke original handler
		}
		#pragma warning(disable : 4035) // Disable warning "no return value"
	*/
}

bool CQueroBand::InstallPopUpBlocker(bool install)
{
	IHTMLDocument2 *pHtmlDocument;
	IHTMLWindow2 *pWin;
	IHTMLWindow3 *pWin3=NULL;
	HRESULT hr;
	bool result=false;

	if(WaitForSingleObject(g_hQSharedDataMutex,QMUTEX_TIMEOUT)==WAIT_OBJECT_0)
	{
		if(install!=g_PopUpBlockerInstalled)
		{
			if(m_Toolbar.GetHtmlDocument2(&pHtmlDocument))
			{
				if(SUCCEEDED_OK(pHtmlDocument->get_parentWindow(&pWin)) && pWin)
				{
					hr=pWin->QueryInterface(IID_IHTMLWindow3,(LPVOID*)&pWin3);
					if(SUCCEEDED_OK(hr) && pWin3)
					{
						// Modifying the vtbl of IHTMLWindow3
						ULONG_PTR *pVtbl;
						DWORD op, op2;
						ULONG_PTR *pShowModeless;

						pVtbl=(ULONG_PTR*)*(ULONG_PTR*)pWin3; // The first dword/qword is the address of the vtbl
						pShowModeless=pVtbl+19; // Offset to showModalDialog method, open is the 20th entry in the vtbl
						if(VirtualProtect(pShowModeless, sizeof(ULONG_PTR), PAGE_READWRITE, &op))
						{
							if(ORIG_HTMLWINDOW3SHOWMODELESS==NULL) ORIG_HTMLWINDOW3SHOWMODELESS=(FP_HTMLWINDOW3SHOWMODELESS)*pShowModeless;
							if(install) *pShowModeless=(ULONG_PTR)HTMLWindow3ShowModelessWrapper;
							else if(*pShowModeless==(ULONG_PTR)HTMLWindow3ShowModelessWrapper) *pShowModeless=(ULONG_PTR)ORIG_HTMLWINDOW3SHOWMODELESS;

							VirtualProtect(pShowModeless, sizeof(ULONG_PTR), op, &op2);
						}
						g_PopUpBlockerInstalled=install;
						result=true;

						pWin3->Release();
					}
					pWin->Release();				
				}
				pHtmlDocument->Release();
			}
		}
		else result=true;
		ReleaseMutex(g_hQSharedDataMutex);
	}
	QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"InstallPopUpBlocker");

	return result;
}

bool CQueroBand::InstallAdBlocker(bool install)
{
	IHTMLDocument2 *pHtmlDocument2;
	IHTMLElement *pHtmlElement;
	bool result=false;

	if(g_IE_MajorVersion<9)
	{
		if(WaitForSingleObject(g_hQSharedDataMutex,QMUTEX_TIMEOUT)==WAIT_OBJECT_0)
		{
			if(install!=g_AdBlockerInstalled)
			{
				if(m_Toolbar.GetHtmlDocument2(&pHtmlDocument2))
				{
					if(SUCCEEDED_OK(pHtmlDocument2->createElement(CComBSTR(L""),&pHtmlElement)) && pHtmlElement)
					{
						ULONG_PTR *pVtbl;
						ULONG_PTR *pWrite;
						ULONG_PTR *pPutInnerHtml;
						DWORD OldProtectHtmlDocument2, OldProtectHtmlElement, op;

						// Save pointers to vtables

						COM_IHTMLElement_PTR=*((ULONG_PTR*)pHtmlElement+4);
						COM_IHTMLDocument2_PTR=*((ULONG_PTR*)pHtmlDocument2+4);

						// Modifying the vtable of IHTMLDocument2 and IHTMLElement
						
						pVtbl=(ULONG_PTR*)COM_IHTMLDocument2_PTR;
						pWrite=pVtbl+59; // Offset to the write method
						pVtbl=(ULONG_PTR*)COM_IHTMLElement_PTR;
						pPutInnerHtml=pVtbl+57; // Offset to the put_innerHtml method

						if(VirtualProtect(pWrite, sizeof(ULONG_PTR)*16, PAGE_READWRITE, &OldProtectHtmlDocument2))
						{
							if(VirtualProtect(pPutInnerHtml, sizeof(ULONG_PTR)*16, PAGE_READWRITE, &OldProtectHtmlElement))
							{
								if(ORIG_HTMLDOCUMENT2WRITE==NULL)
								{
									ORIG_HTMLDOCUMENT2WRITE=(FP_HTMLDOCUMENT2WRITE)*(pWrite);
									ORIG_HTMLDOCUMENT2WRITELN=(FP_HTMLDOCUMENT2WRITELN)*(pWrite+1);
									ORIG_HTMLDOCUMENT2CREATEELEMENT=(FP_HTMLDOCUMENT2CREATEELEMENT)*(pWrite+13);

									ORIG_HTMLELEMENT_PUT_INNERHTML=(FP_HTMLELEMENT_PUT_HTML)*(pPutInnerHtml);
									ORIG_HTMLELEMENT_PUT_OUTERHTML=(FP_HTMLELEMENT_PUT_HTML)*(pPutInnerHtml+4);
									ORIG_HTMLELEMENT_INSERTADJACENTHTML=(FP_HTMLELEMENT_INSERTADJACENTHTML)*(pPutInnerHtml+8);
								}
								if(install)
								{
									*(pWrite)=(ULONG_PTR)HTMLDocument2write;
									*(pWrite+1)=(ULONG_PTR)HTMLDocument2writeln;
									*(pWrite+13)=(ULONG_PTR)HTMLDocument2createElement;

									*(pPutInnerHtml)=(ULONG_PTR)HTMLElement_put_innerHTML;
									*(pPutInnerHtml+4)=(ULONG_PTR)HTMLElement_put_outerHTML;
									*(pPutInnerHtml+8)=(ULONG_PTR)HTMLElement_insertAdjacentHTML;
								}
								else
								{
									if(*(pWrite)==(ULONG_PTR)HTMLDocument2write) *(pWrite)=(ULONG_PTR)ORIG_HTMLDOCUMENT2WRITE;
									if(*(pWrite+1)==(ULONG_PTR)HTMLDocument2writeln) *(pWrite+1)=(ULONG_PTR)ORIG_HTMLDOCUMENT2WRITELN;
									if(*(pWrite+13)==(ULONG_PTR)HTMLDocument2createElement) *(pWrite+13)=(ULONG_PTR)ORIG_HTMLDOCUMENT2CREATEELEMENT;

									if(*(pPutInnerHtml)==(ULONG_PTR)HTMLElement_put_innerHTML) *(pPutInnerHtml)=(ULONG_PTR)ORIG_HTMLELEMENT_PUT_INNERHTML;
									if(*(pPutInnerHtml+4)==(ULONG_PTR)HTMLElement_put_outerHTML) *(pPutInnerHtml+4)=(ULONG_PTR)ORIG_HTMLELEMENT_PUT_OUTERHTML;
									if(*(pPutInnerHtml+8)==(ULONG_PTR)HTMLElement_insertAdjacentHTML) *(pPutInnerHtml+8)=(ULONG_PTR)ORIG_HTMLELEMENT_INSERTADJACENTHTML;

									// Reset FilterStates
									g_ContentFilter.FreeFilterStates(NULL);
								}

								g_AdBlockerInstalled=install;
								result=true;
								
								VirtualProtect(pPutInnerHtml, sizeof(ULONG_PTR)*16, OldProtectHtmlElement, &op);
							}
							VirtualProtect(pWrite, sizeof(ULONG_PTR)*16, OldProtectHtmlDocument2, &op);
						}

						/* WIN32
							ULONG_PTR *pVtbl;
							ULONG_PTR *pWrite;
							DWORD op, op2;

							// Save pointers to distinguish object type

							COM_IHTMLElement_PTR=*((ULONG_PTR*)pHtmlElement+4);
							COM_IHTMLDocument2_PTR=*((ULONG_PTR*)pHtmlDocument2+4);

							// Modifying the vtbl of IHTMLDocument2 and IHTMLElement
							
							pVtbl=(ULONG_PTR*)*(ULONG_PTR*)pHtmlDocument2; // The first dword is the address of the vtbl
							pWrite=pVtbl+57; // Offset to put_innerHTML method
							if(VirtualProtect(pWrite, sizeof(ULONG_PTR)*16, PAGE_READWRITE, &op))
							{	
								if(ORIG_HTMLDOCUMENT2WRITE==NULL)
								{
									ORIG_HTMLELEMENT_PUT_INNERHTML=(FP_HTMLELEMENT_PUT_HTML)*(pWrite);
									ORIG_HTMLDOCUMENT2WRITE=(FP_HTMLDOCUMENT2WRITE)*(pWrite+2);
									ORIG_HTMLDOCUMENT2WRITELN=(FP_HTMLDOCUMENT2WRITELN)*(pWrite+3);
									ORIG_HTMLELEMENT_PUT_OUTERHTML=(FP_HTMLELEMENT_PUT_HTML)*(pWrite+4);
									ORIG_HTMLELEMENT_INSERTADJACENTHTML=(FP_HTMLELEMENT_INSERTADJACENTHTML)*(pWrite+8);
									ORIG_HTMLDOCUMENT2CREATEELEMENT=(FP_HTMLDOCUMENT2CREATEELEMENT)*(pWrite+15);
								}
								if(install)
								{
									*(pWrite)=(ULONG_PTR)HTMLElement_put_innerHTML;
									*(pWrite+2)=(ULONG_PTR)HTMLDocument2write;
									*(pWrite+3)=(ULONG_PTR)HTMLDocument2writeln;
									*(pWrite+4)=(ULONG_PTR)HTMLElement_put_outerHTML;
									*(pWrite+8)=(ULONG_PTR)HTMLElement_insertAdjacentHTML;
									*(pWrite+15)=(ULONG_PTR)HTMLDocument2createElement;
								}
								else
								{
									if(*(pWrite)==(ULONG_PTR)HTMLElement_put_innerHTML) *(pWrite)=(ULONG_PTR)ORIG_HTMLELEMENT_PUT_INNERHTML;
									if(*(pWrite+2)==(ULONG_PTR)HTMLDocument2write) *(pWrite+2)=(ULONG_PTR)ORIG_HTMLDOCUMENT2WRITE;
									if(*(pWrite+3)==(ULONG_PTR)HTMLDocument2writeln) *(pWrite+3)=(ULONG_PTR)ORIG_HTMLDOCUMENT2WRITELN;
									if(*(pWrite+4)==(ULONG_PTR)HTMLElement_put_outerHTML) *(pWrite+4)=(ULONG_PTR)ORIG_HTMLELEMENT_PUT_OUTERHTML;
									if(*(pWrite+8)==(ULONG_PTR)HTMLElement_insertAdjacentHTML) *(pWrite+8)=(ULONG_PTR)ORIG_HTMLELEMENT_INSERTADJACENTHTML;
									if(*(pWrite+15)==(ULONG_PTR)HTMLDocument2createElement) *(pWrite+15)=(ULONG_PTR)ORIG_HTMLDOCUMENT2CREATEELEMENT;

									// Reset FilterStates
									g_ContentFilter.FreeFilterStates(NULL);
								}
								g_AdBlockerInstalled=install;
								VirtualProtect(pWrite, sizeof(ULONG_PTR)*16, op, &op2);
								result=true;						
							}
						*/
						pHtmlElement->Release();
					}
					pHtmlDocument2->Release();
				}		
			}
			else result=true;
			
			ReleaseMutex(g_hQSharedDataMutex);
		}
		QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"InstallAdBlocker");
	} // End g_IE_MajorVersion < 9

	return result;
}

bool CQueroBand::InstallContextMenu(bool install)
{
	IDocHostUIHandler *pUIHandler=NULL;
	IDispatch *pDispatch=NULL;
	HRESULT hr;
	bool result=false;

	// Installing a custom ui handler has some unwanted side effects:
	// save as web archive and webpage complete options disaapear in save as dialog
	// returning S_FALSE in ShowContextMenu does not show IE accelerators

	ICustomDoc *pCustomDoc=NULL;

	if(m_pWebBrowser2)
	{
		hr=m_pWebBrowser2->get_Document(&pDispatch);
		if(SUCCEEDED_OK(hr) && pDispatch)
		{
			// If this is not an HTML document (e.g., it's a Word doc or a PDF), don't sink.
			IHTMLDocument2 *pHtmlDocument=NULL;
			hr=pDispatch->QueryInterface(IID_IHTMLDocument2,(LPVOID*)&pHtmlDocument);
			if(SUCCEEDED_OK(hr) && pHtmlDocument)
			{
				// Get pointer to default interface
				IOleObject *pOleObject=NULL;
				hr=pDispatch->QueryInterface(IID_IOleObject,(LPVOID*)&pOleObject);
				if(SUCCEEDED_OK(hr) && pOleObject)
				{
					IOleClientSite *pClientSite=NULL;
					hr = pOleObject->GetClientSite(&pClientSite);
					if(SUCCEEDED_OK(hr) && pClientSite)
					{
						if(m_pOrigDocHostUIHandler==NULL)
						{
							hr=pClientSite->QueryInterface(IID_IDocHostUIHandler,(LPVOID*)&m_pOrigDocHostUIHandler);
						}
						if(SUCCEEDED_OK(hr) && m_pOrigDocHostUIHandler)
						{
							hr=pDispatch->QueryInterface(IID_ICustomDoc,(LPVOID*)&pCustomDoc);
							if(SUCCEEDED_OK(hr) && pCustomDoc)
							{
								if(install)
								{
									pCustomDoc->SetUIHandler(this);
									m_ContextMenuInstalled=true;
								}
								else
								{
									pCustomDoc->SetUIHandler(m_pOrigDocHostUIHandler);
									m_pOrigDocHostUIHandler=NULL;
									m_ContextMenuInstalled=false;
								}
								result=true;
								pCustomDoc->Release();
							}
						}						
						pClientSite->Release();
					}
					pOleObject->Release();
				}		
				pHtmlDocument->Release();
			}
			pDispatch->Release();
		}
	}

/*
	// Overriding ShowContextMenu
	if(WaitForSingleObject(g_hQSharedDataMutex,QMUTEX_TIMEOUT)==WAIT_OBJECT_0)
	{
		if(install!=g_ContextMenuInstalled)
		{
			if(m_pWebBrowser2)
			{
				hr=m_pWebBrowser2->get_Document(&pDispatch);
				if(SUCCEEDED_OK(hr) && pDispatch)
				{
					// If this is not an HTML document (e.g., it's a Word doc or a PDF), don't sink.
					IHTMLDocument2 *pHtmlDocument=NULL;
					hr=pDispatch->QueryInterface(IID_IHTMLDocument2,(LPVOID*)&pHtmlDocument);
					if(SUCCEEDED_OK(hr) && pHtmlDocument)
					{
						// Get pointer to default interface
						IOleObject *pOleObject=NULL;
						hr=pDispatch->QueryInterface(IID_IOleObject,(LPVOID*)&pOleObject);
						if(SUCCEEDED_OK(hr) && pOleObject)
						{
							IOleClientSite *pClientSite=NULL;
							hr = pOleObject->GetClientSite(&pClientSite);
							if(SUCCEEDED_OK(hr) && pClientSite)
							{
								hr=pClientSite->QueryInterface(IID_IDocHostUIHandler,(LPVOID*)&pUIHandler);
								
								pClientSite->Release();
							}
							pOleObject->Release();
						}		
						pHtmlDocument->Release();
					}
					pDispatch->Release();
				}
			}
			else hr=E_FAIL;

			if(SUCCEEDED_OK(hr) && pUIHandler)
			{
				// Modifying the vtbl of IDocHostUIHandler

				ULONG_PTR *pVtbl;
				ULONG_PTR *pShow;
				DWORD op, op2;

				pVtbl=(ULONG_PTR*)*(ULONG_PTR*)pUIHandler; // The first dword/qword is the address of the vtbl
				pShow=pVtbl+3; // Offset to ShowContextMenu method
				
				if(VirtualProtect(pShow, sizeof(ULONG_PTR), PAGE_READWRITE, &op))
				{
					if(ORIG_DOCHOSTUIHANDLER_SHOWCTXMENU==NULL) ORIG_DOCHOSTUIHANDLER_SHOWCTXMENU=(FP_DOCHOSTUIHANDLER_SHOWCTXMENU)*pShow;
					if(install) *pShow=(ULONG_PTR)DocHostUIHandler_ShowContextMenu;
					else if(*pShow==(ULONG_PTR)DocHostUIHandler_ShowContextMenu) *pShow=(ULONG_PTR)ORIG_DOCHOSTUIHANDLER_SHOWCTXMENU;

					VirtualProtect(pShow, sizeof(ULONG_PTR), op, &op2);
					g_ContextMenuInstalled=install;
					result=true;
				}

				pUIHandler->Release();
			}
		}
		else result=true;

		ReleaseMutex(g_hQSharedDataMutex);
	}
	QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"InstallContextMenu");
*/

	return result;
}

bool CQueroBand::IsPopUpBlockerInstalled()
{
	return g_PopUpBlockerInstalled;
}

bool CQueroBand::IsAdBlockerInstalled()
{
	return g_AdBlockerInstalled;
}

bool CQueroBand::IsContextMenuInstalled()
{
	return m_ContextMenuInstalled;
}

// Low-level keyboard hook

LRESULT CALLBACK KeyboardHookTab(int nCode, WPARAM wp, LPARAM lp)
{
	QThreadData *QueroInstance;
	LRESULT result=0;

	// Get the current Quero instance
	if(WaitForSingleObject(g_hQSharedDataMutex,QMUTEX_TIMEOUT)==WAIT_OBJECT_0)
	{
		QueroInstance=GetCurrentQueroInstance();
		if(QueroInstance)
		{
			result=QueroInstance->pToolbar->OnKeyboardHook(nCode,wp,lp,false);
	
			if(result==0 && QueroInstance->hKeyHookTab) result=CallNextHookEx(QueroInstance->hKeyHookTab, nCode, wp, lp);

		} // End QueroInstance

		ReleaseMutex(g_hQSharedDataMutex);
	}
	QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"KeyboardHookTab");

	return result;
}

HRESULT STDMETHODCALLTYPE DocHostUIHandler_ShowContextMenu(IDocHostUIHandler __RPC_FAR* pUIHandler,
															DWORD dwID,POINT *ppt,
															IUnknown *pcmdTarget,
															IDispatch *pdispObject)
{
	HRESULT hr;

	if(dwID==CONTEXT_MENU_TEXTSELECT || dwID==CONTEXT_MENU_ANCHOR || dwID==CONTEXT_MENU_CONTROL)
	{
		IOleCommandTarget *pOleCommandTarget;
		IOleWindow *pOleWindow;
		HWND hwnd;

		hr = pcmdTarget->QueryInterface(IID_IOleCommandTarget,(LPVOID*)&pOleCommandTarget);
		if(SUCCEEDED_OK(hr))
		{
			#ifdef COMPILE_FOR_WIN9X
			HMODULE hModule = LoadLibrary(L"shdoclc.dll");
			#else
			HMODULE hModule;
			TCHAR *pIEFrameDLL_Filename;

			if(IsBelowWindowsVista()) pIEFrameDLL_Filename=g_IE_MajorVersion>=7?L"ieframe.dll.mui":L"shdoclc.dll";
			else pIEFrameDLL_Filename=L"ieframe.dll";
			
			hModule=LoadLibraryEx(pIEFrameDLL_Filename,NULL,LOAD_LIBRARY_AS_DATAFILE);
			#endif			
			
			if(hModule)
			{			
				HMENU hCtxMenu = LoadMenu(hModule,MAKEINTRESOURCE(IDR_BROWSE_CONTEXT_MENU));
				if(hCtxMenu)
				{
					HMENU hSubMenu = GetSubMenu(hCtxMenu, dwID);
					if(hSubMenu)
					{
						/*
						// Get the language submenu
						CComVariant var;
						hr = spCT->Exec(&CGID_ShellDocView, SHDVID_GETMIMECSETMENU, 0, NULL, &var);
						if(SUCCEEDED_OK(hr))
						{
							MENUITEMINFO mii = {0};
							mii.cbSize = sizeof(mii);
							mii.fMask  = MIIM_SUBMENU;
							mii.hSubMenu = (HMENU) var.byref;

							// Add language submenu to Encoding context item
							SetMenuItemInfo(hSubMenu, IDM_LANGUAGE, FALSE, &mii);
						*/
						// Insert Shortcut Menu Extensions from registry
						CComVariant var1;
						V_VT(&var1) = VT_INT_PTR;
						V_BYREF(&var1) = hSubMenu;

						CComVariant var2;
						V_VT(&var2) = VT_I4;
						V_I4(&var2) = dwID;

						hr = pOleCommandTarget->Exec(&CGID_ShellDocView, SHDVID_ADDMENUEXTENSIONS, 0, &var1, &var2);
						if(SUCCEEDED_OK(hr))
						{
							IHTMLElement *pHtmlElement=NULL;
							IHTMLDocument2 *pHtmlDocument2=NULL;
							IHTMLSelectionObject *pHtmlSelectionObject=NULL;
							BSTR selection=NULL;

							hr=pdispObject->QueryInterface(IID_IHTMLElement,(LPVOID*)&pHtmlElement);
							if(SUCCEEDED_OK(hr) && pHtmlElement)
							{
								hr=pHtmlElement->get_document((IDispatch**)&pHtmlDocument2);
								if(SUCCEEDED_OK(hr) && pHtmlDocument2)
								{
									hr=pHtmlDocument2->get_selection(&pHtmlSelectionObject);
									if(SUCCEEDED_OK(hr) && pHtmlSelectionObject)
									{
										IHTMLTxtRange *pHtmlTextRange=NULL;

										hr=pHtmlSelectionObject->createRange((IDispatch**)&pHtmlTextRange);
										if(SUCCEEDED_OK(hr) && pHtmlTextRange)
										{	
											hr=pHtmlTextRange->get_text(&selection);
											pHtmlTextRange->Release();
										}
										pHtmlSelectionObject->Release();
									}
									pHtmlDocument2->Release();
								}
								pHtmlElement->Release();
							}

							if(SUCCEEDED_OK(hr) && selection)
							{
								QThreadData *QueroInstance;
								CQToolbar *pToolbar;
								size_t len;
								
								// Identify IE window and get pointer to QToolbar

								if(WaitForSingleObject(g_hQSharedDataMutex,QMUTEX_TIMEOUT)==WAIT_OBJECT_0)
								{
									bool bReleaseMutex=true;

									QueroInstance=GetCurrentQueroInstance();
									if(QueroInstance)
									{
										pToolbar=QueroInstance->pToolbar;

										hr = pcmdTarget->QueryInterface(IID_IOleWindow,(LPVOID*)&pOleWindow);
										if(SUCCEEDED_OK(hr) && pOleWindow)
										{
											hr=pOleWindow->GetWindow(&hwnd);
											if(SUCCEEDED_OK(hr))
											{
												CQToolbar::trim(selection);
												StrCchLen(selection,MAXURLLENGTH,len);
												if(len)
												{
													UINT i;
													#ifdef COMPILE_FOR_WINDOWS_VISTA
													CMenuIcon MenuIcon;
													CMenuIcon EngineIcons[MAX_ENGINES];
													#endif

													if(pToolbar->GetIDNA()->IsAddress(selection)) g_EnableCtxMenuGo=true;
													else g_EnableCtxMenuGo=false;

													g_QueroCtxMenu=CreateMenu();
													for(i=0;i<pToolbar->nengines;i++)
													{
														AppendMenu(g_QueroCtxMenu,MF_STRING|MF_ENABLED,ID_CTXMENU_QUERO+i,pToolbar->m_Profiles.CurrentProfile.Engines[i].Name);
														#ifdef COMPILE_FOR_WINDOWS_VISTA
														HICON hIcon;
														hIcon=pToolbar->m_Profiles.GetEngineIcon(&pToolbar->m_Profiles.CurrentProfile.Engines[i],NULL);
														if(hIcon) EngineIcons[i].AddIconToMenuItem(hIcon,NULL,g_QueroCtxMenu,ID_CTXMENU_QUERO+i,MF_BYCOMMAND);
														#endif
													}
													if(i==0) AppendMenu(g_QueroCtxMenu,MF_STRING|MF_DISABLED,ID_CTXMENU_QUERO,pToolbar->GetString(IDS_EMPTY));

													// Customize context menu
													if(dwID==CONTEXT_MENU_ANCHOR) // 0x1c ... insert our item before "Properties"
													{
														// Remove IE8 Activites separator
														if(g_IE_MajorVersion>=8) RemoveMenu(hSubMenu,10,MF_BYPOSITION);
														// Remove "Open In New Tab" if tabbed browsing is disabled
														if(pToolbar->pQueroBroker && pToolbar->pQueroBroker->IsTabbedBrowsing(HandleToLong(pToolbar->GetIEFrameWindow()))==S_FALSE)
														{
															::RemoveMenu(hSubMenu,ID_CTXMENU_OPENINNEWTAB,MF_BYCOMMAND);
														}
														// Insert Quero menu items
														::InsertMenu(hSubMenu,0x1c,MF_POPUP|MF_BYCOMMAND|MF_ENABLED,(UINT_PTR)g_QueroCtxMenu,L"Quero");
														#ifdef COMPILE_FOR_WINDOWS_VISTA
														//MenuIcon.AddIconToMenuItem(CQToolbar::g_Icons[ICON_SEARCH],NULL,hSubMenu,GetMenuPosFromID(hSubMenu,0x1c)-1,MF_BYPOSITION);
														#endif
														::InsertMenu(hSubMenu,0x1c,MF_STRING|MF_ENABLED,ID_CTXMENU_HIGHLIGHT,L"Highlight");
														::InsertMenu(hSubMenu,0x1c,MF_SEPARATOR|MF_BYCOMMAND,0,0);
													}
													else
													{
														// Remove IE8 Activites separator
														if(g_IE_MajorVersion>=8 && dwID==CONTEXT_MENU_TEXTSELECT) RemoveMenu(hSubMenu,6,MF_BYPOSITION);
														// Append Quero menu items
														::AppendMenu(hSubMenu,MF_SEPARATOR,0,0);
														::AppendMenu(hSubMenu,MF_POPUP|MF_ENABLED,(UINT_PTR)g_QueroCtxMenu,L"Quero");
														#ifdef COMPILE_FOR_WINDOWS_VISTA
														//MenuIcon.AddIconToMenuItem(CQToolbar::g_Icons[ICON_SEARCH],NULL,hSubMenu,GetMenuItemCount(hSubMenu)-1,MF_BYPOSITION);
														#endif
														::AppendMenu(hSubMenu,MF_STRING|MF_ENABLED,ID_CTXMENU_HIGHLIGHT,pToolbar->GetString(IDS_CTXMENU_HIGHLIGHT));
														::AppendMenu(hSubMenu,MF_STRING,ID_CTXMENU_GO,pToolbar->GetString(IDS_CTXMENU_GOTOURL));
													}
										
													// Subclass IE window
													// This is required in order to enable our menu. Otherwise, IE disregards menu items it doesn't recognize.

													g_pOrigCtxMenuWndProc=(WNDPROC)SetWindowLongPtr(hwnd,GWLP_WNDPROC,(LONG_PTR)CtxMenuWndProc);

													// Show shortcut menu
													g_MiddleClick=false;
													UINT nCmd = ::TrackPopupMenu(hSubMenu,
														TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD,
														ppt->x,
														ppt->y,
														0,
														hwnd,
														(RECT*)NULL);

													// Get newWinTab state
													UINT newWinTab=pToolbar->GetNewWinTabKeyState(g_MiddleClick?SHORTCUT_OPTION_MIDDLECLICK:SHORTCUT_OPTION_DEFAULT);

													// Unsubclass IE window
													SetWindowLongPtr(hwnd,GWLP_WNDPROC,(LONG_PTR)g_pOrigCtxMenuWndProc);
													
													// Execute command
													if(nCmd==ID_CTXMENU_GO)
													{
														pToolbar->Quero(selection,TYPE_ADDRESS,(newWinTab==OPEN_SameWindow && g_ShowURL)?QUERO_REDIRECT|QUERO_SETTEXT:QUERO_REDIRECT,newWinTab);
													}
													else if(nCmd==ID_CTXMENU_HIGHLIGHT)
													{
														pToolbar->HighlightWord(selection);
													}
													else if(nCmd>=ID_CTXMENU_QUERO && nCmd<(ID_CTXMENU_QUERO+pToolbar->nengines))
													{
														if(newWinTab==OPEN_SameWindow)
														{
															pToolbar->GetComboQuero()->SetText(selection,TYPE_SEARCH,NULL,false);
															pToolbar->SelectEngine(nCmd-ID_CTXMENU_QUERO);
														}

														pToolbar->Quero(selection,TYPE_SEARCH,QUERO_REDIRECT,newWinTab,pToolbar->m_Profiles.CurrentProfile.Engines[nCmd-ID_CTXMENU_QUERO].id);
													}
													else
													{
														// Release mutex before sending the command
														ReleaseMutex(g_hQSharedDataMutex);																						
														bReleaseMutex=false;

														// Send selected shortcut menu item command to IE frame
														// (cannot use PostMessage because "Open in new Window/Tab" is then regarded as as an unsolicited pop-up window by IE)
														::SendMessage(hwnd,WM_COMMAND,nCmd,NULL);
													}

												} // Selection len ok?
												else hr=E_FAIL;
											} // hwnd ok
										} // IOLEWindow ok
									} // QToolbar found
									else hr=E_FAIL; // QToolbar not found
									
									if(bReleaseMutex) ReleaseMutex(g_hQSharedDataMutex);
								} // Lock on thread data structure
								QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"DocHostUIHandler");

								SysFreeString(selection);
							} // Selection available
							else hr=E_FAIL;
						}
					} // GetSubmMnu
					if(g_QueroCtxMenu)
					{
						DestroyMenu(g_QueroCtxMenu);
						g_QueroCtxMenu=NULL;
					}					
					DestroyMenu(hCtxMenu);
				} // LoadMenu
				else hr=E_FAIL;
				
				FreeLibrary(hModule);
			} // LoadLibrary
			else hr=E_FAIL;

			pOleCommandTarget->Release();
		} // End OleCommandTarget
	
	} // End handled context menu types
	else hr=E_FAIL;

	return hr;
	//return SUCCEEDED_OK(hr) ? S_OK : ORIG_DOCHOSTUIHANDLER_SHOWCTXMENU(pUIHandler,dwID,ppt,pcmdTarget,pdispObject);
}

// Window procedure for handling enabling/disabling/checking in our context menu.
LRESULT CALLBACK CtxMenuWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lr=0;

	if(uMsg==WM_INITMENUPOPUP)
	{
		if(wParam!=(WPARAM)g_QueroCtxMenu)
		{
			lr=CallWindowProc(g_pOrigCtxMenuWndProc, hwnd, uMsg, wParam, lParam);
			::EnableMenuItem((HMENU)wParam,ID_CTXMENU_HIGHLIGHT,MF_BYCOMMAND|MF_ENABLED);
			if(g_EnableCtxMenuGo) ::EnableMenuItem((HMENU)wParam,ID_CTXMENU_GO,MF_BYCOMMAND|MF_ENABLED);
		}
	}
	else if(uMsg==WM_MBUTTONUP)
	{
		MapMiddleButton_ContextMenu(wParam,lParam);
	}
	else
	{
		// Map quero menu items to id==0 to prevent display of wrong help text in the status bar
		if(uMsg==WM_MENUSELECT)
		{
			WORD id=LOWORD(wParam);

			if(id==ID_CTXMENU_HIGHLIGHT || id==ID_CTXMENU_GO || (HMENU)lParam==g_QueroCtxMenu)
			{
				wParam=HIWORD(wParam);
			}
		}

		// Call original window procedure
		lr=CallWindowProc(g_pOrigCtxMenuWndProc, hwnd, uMsg, wParam, lParam);
	}

	return lr;
}

UINT MapMiddleButton_Message(UINT uMsg)
{
	switch(uMsg)
	{
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_KEYDOWN:
		g_MiddleClick=false;
		break;
	case WM_MBUTTONDOWN:
		uMsg=WM_LBUTTONDOWN;
		break;
	case WM_MBUTTONUP:
		uMsg=WM_LBUTTONUP;
		g_MiddleClick=true;
		break;
	}

	return uMsg;
}

void MapMiddleButton_ContextMenu(WPARAM wParam,LPARAM lParam)
{
	// Find the latest context menu the application created
	HWND hPopUpWnd=::FindWindow(MAKEINTATOM(0x8000),NULL);

	if(hPopUpWnd)
	{
		g_MiddleClick=true;
		PostMessage(hPopUpWnd ,WM_LBUTTONDOWN,wParam,lParam);
		PostMessage(hPopUpWnd ,WM_LBUTTONUP,wParam,lParam);
	}
}