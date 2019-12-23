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

// QueroBand.h : Declaration of the CQueroBand

#ifndef __QUEROBAND_H_
#define __QUEROBAND_H_

#include <mshtmhst.h>
#include "resource.h"       // main symbols
#include "ExDispID.h"
#include "QueroFilter.h"

// MangageBrowserConnection types
#define ADVISE 1
#define UNADVISE 0

// Version Functions
bool LoadVersion(HKEY hKeyQuero,VersionInfo *vi);
UINT UpdateVersion(HKEY hKeyQuero,VersionInfo *vi,bool bVersionLoaded,bool IsDLLRegistration);

// Update User Settings Function (currently used to deploy NoNavBar GPO during install/uninstall)
void UpdateAllUsers(bool bUninstall);

// Mark Toolbar Enhanced Protected Mode compatible (AppContainerCompatible)
DEFINE_GUID(CATID_AppContainerCompatible, 0x59fb2056,0xd625,0x48d0,0xa9,0x44,0x1a,0x85,0xb5,0xab,0x26,0x40);
HRESULT RegisterCLSIDInCategory(REFCLSID clsid, CATID catid, bool bRegister);

// Context Menu Constants
#define IDR_BROWSE_CONTEXT_MENU  24641
#define IDR_FORM_CONTEXT_MENU    24640
#define SHDVID_GETMIMECSETMENU   27
#define SHDVID_ADDMENUEXTENSIONS 53
#define ID_CTXMENU_OPENINNEWTAB 0x983
#define ID_CTXMENU_QUERO 0x1000
#define ID_CTXMENU_HIGHLIGHT 0x1100
#define ID_CTXMENU_GO 0x1101

// Get Quero Thread Data
QThreadData* GetCurrentQueroInstance();
QThreadData* GetFirstQueroInstance();
QThreadData* GetQueroInstanceActiveTab();

// Keyboard hook
LRESULT CALLBACK KeyboardHookTab(int nCode, WPARAM wp, LPARAM lp);

// HTMLWindow3.showModelessDialog Wrapper Function
typedef  HRESULT (STDMETHODCALLTYPE __RPC_FAR * FP_HTMLWINDOW3SHOWMODELESS)(IHTMLWindow3 __RPC_FAR* pWin,
																			BSTR,
																			VARIANT *,
																			VARIANT *,
																			IDispatch __RPC_FAR *__RPC_FAR *);

HRESULT STDMETHODCALLTYPE HTMLWindow3ShowModelessWrapper(IHTMLWindow3 __RPC_FAR* pWin,
														 BSTR url,
														 VARIANT *varArgIn,
														 VARIANT *varOptions,
														 IDispatch __RPC_FAR *__RPC_FAR * pomWindowResult);

// DocHostUIHandler.ShowContextMenu Wrapper Function
typedef HRESULT (STDMETHODCALLTYPE __RPC_FAR * FP_DOCHOSTUIHANDLER_SHOWCTXMENU)(IDocHostUIHandler __RPC_FAR* pUIHandler,
															DWORD dwID,POINT *ppt,
															IUnknown *pcmdtReserved,
															IDispatch *pdispReserved);

HRESULT STDMETHODCALLTYPE DocHostUIHandler_ShowContextMenu(IDocHostUIHandler __RPC_FAR* pUIHandler,
															DWORD dwID,POINT *ppt,
															IUnknown *pcmdtReserved,
															IDispatch *pdispReserved);


// HTMLDocument2.write(ln) Wrapper Functions
HRESULT STDMETHODCALLTYPE HTMLDocument2write(IHTMLDocument2 __RPC_FAR* pHtmlDocument,SAFEARRAY *psarray);
HRESULT STDMETHODCALLTYPE HTMLDocument2writeln(IHTMLDocument2 __RPC_FAR* pHtmlDocument,SAFEARRAY *psarray);
typedef  HRESULT (STDMETHODCALLTYPE __RPC_FAR * FP_HTMLDOCUMENT2WRITE)(IHTMLDocument2 __RPC_FAR* pHtmlDocument,SAFEARRAY *psarray);
typedef  HRESULT (STDMETHODCALLTYPE __RPC_FAR * FP_HTMLDOCUMENT2WRITELN)(IHTMLDocument2 __RPC_FAR* pHtmlDocument,SAFEARRAY *psarray);
void OrigDocumentWrite(IHTMLDocument2 *pHtmlDocument,BSTR bstr,bool IsWriteLn);

// IHTMLElement.putHTML
HRESULT STDMETHODCALLTYPE HTMLElement_put_HTML(IHTMLElement __RPC_FAR* pHtmlElement,BSTR v);
typedef  HRESULT (STDMETHODCALLTYPE __RPC_FAR * FP_HTMLELEMENT_PUT_HTML)(IHTMLElement __RPC_FAR* pHtmlElement,BSTR v);

// IHTMLElement.insertAdjacentHTML
HRESULT STDMETHODCALLTYPE HTMLElement_insertAdjacentHTML(IHTMLElement __RPC_FAR* pHtmlElement,BSTR where,BSTR html);
typedef  HRESULT (STDMETHODCALLTYPE __RPC_FAR * FP_HTMLELEMENT_INSERTADJACENTHTML)(IHTMLElement __RPC_FAR* pHtmlElement,BSTR where,BSTR html);

// HTMLDocument2.createElement
HRESULT STDMETHODCALLTYPE HTMLDocument2createElement(IHTMLDocument2 __RPC_FAR* pHtmlDocument,BSTR eTag,IHTMLElement **newElem);
typedef HRESULT (STDMETHODCALLTYPE __RPC_FAR * FP_HTMLDOCUMENT2CREATEELEMENT)(IHTMLDocument2 __RPC_FAR* pHtmlDocument,BSTR eTag,IHTMLElement **newElem);

// Window procedure for handling enabling/disabling/checking in our context menu.
LRESULT CALLBACK CtxMenuWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Command GUID for WebBrowser (invoke find on page dialog etc.)
DEFINE_GUID(CGID_IWebBrowser,0xED016940L,0xBD5B,0x11cf,0xBA,0x4E,0x00,0xC0,0x4F,0xD7,0x08,0x16);

// MapMiddleButton functions
UINT MapMiddleButton_Message(UINT uMsg);
void MapMiddleButton_ContextMenu(WPARAM wParam,LPARAM lParam);

class CQToolbar;

extern const TCHAR* AttrObject[];
extern const TCHAR* AttrParam[];
extern const TCHAR* AttrEmbed[];
extern const TCHAR* AttrBase[];
extern const TCHAR* AttrScript[];
extern const TCHAR* AttrImg[];
extern const TCHAR* AttrIFrame[];


/////////////////////////////////////////////////////////////////////////////
// CQueroBand
class ATL_NO_VTABLE CQueroBand : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CQueroBand, &CLSID_QueroBand>,
	public IDeskBand,
	public IObjectWithSite,
	public IInputObject,
	public IDocHostUIHandler,
	public IOleCommandTarget,
	public IDispatchImpl<IQueroBand, &IID_IQueroBand, &LIBID_QUEROLib>
{
public:
	CQueroBand();
	~CQueroBand();

DECLARE_REGISTRY_RESOURCEID(IDR_QUEROBAND)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_CATEGORY_MAP(CQueroBand)
//	IMPLEMENTED_CATEGORY(CATID_InfoBand)
//	IMPLEMENTED_CATEGORY(CATID_CommBand)
//	IMPLEMENTED_CATEGORY(CATID_DeskBand)
END_CATEGORY_MAP()

BEGIN_COM_MAP(CQueroBand)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IQueroBand)
	//COM_INTERFACE_ENTRY(IInputObject)
	COM_INTERFACE_ENTRY_IID(IID_IInputObject, IInputObject)
	COM_INTERFACE_ENTRY(IOleWindow)
	COM_INTERFACE_ENTRY_IID(IID_IDockingWindow, IDockingWindow)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY(IDocHostUIHandler)
	COM_INTERFACE_ENTRY(IOleCommandTarget)
	COM_INTERFACE_ENTRY_IID(IID_IDeskBand, IDeskBand)
END_COM_MAP()

// IDeskBand
public:
	STDMETHOD(GetBandInfo)(DWORD dwBandID, DWORD dwViewMode, DESKBANDINFO* pdbi);

// IObjectWithSite
public:
	STDMETHOD(SetSite)(IUnknown* pUnkSite);
	STDMETHOD(GetSite)(REFIID riid, void **ppvSite);

// IOleWindow
public:
	STDMETHOD(GetWindow)(HWND* phwnd);
	STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);

// IDockingWindow
public:
	STDMETHOD(CloseDW)(unsigned long dwReserved);
	STDMETHOD(ResizeBorderDW)(const RECT* prcBorder, IUnknown* punkToolbarSite, BOOL fReserved);
	STDMETHOD(ShowDW)(BOOL bShow);

// IInputObject
public:
	STDMETHOD(HasFocusIO)(void);
	STDMETHOD(TranslateAcceleratorIO)(LPMSG lpMsg);
	STDMETHOD(UIActivateIO)(BOOL fActivate, LPMSG lpMsg);

// IDocHostUIHandler
public:
	STDMETHOD(EnableModeless)(BOOL fEnable);
	STDMETHOD(FilterDataObject)(IDataObject *pDO, IDataObject **ppDORet);
	STDMETHOD(GetDropTarget)(IDropTarget *pDropTarget, IDropTarget **ppDropTarget);
	STDMETHOD(GetExternal)(IDispatch **ppDispatch);
	STDMETHOD(GetHostInfo)(DOCHOSTUIINFO *pInfo);
	STDMETHOD(GetOptionKeyPath)(LPOLESTR *pchKey,DWORD dw);
	STDMETHOD(HideUI)(VOID);
	STDMETHOD(OnDocWindowActivate)(BOOL fActivate);
	STDMETHOD(OnFrameWindowActivate)(BOOL fActivate);
	STDMETHOD(ResizeBorder)(LPCRECT prcBorder,IOleInPlaceUIWindow *pUIWindow,BOOL fFrameWindow);
	STDMETHOD(ShowContextMenu)(DWORD dwID,POINT *ppt,IUnknown *pcmdtReserved,IDispatch *pdispReserved);
	STDMETHOD(ShowUI)(DWORD dwID,IOleInPlaceActiveObject *pActiveObject,IOleCommandTarget *pCommandTarget,IOleInPlaceFrame *pFrame,IOleInPlaceUIWindow *pDoc);
	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg,const GUID *pguidCmdGroup,DWORD nCmdID);
	STDMETHOD(TranslateUrl)(DWORD dwTranslate,OLECHAR *pchURLIn,OLECHAR **ppchURLOut);
	STDMETHOD(UpdateUI)(VOID);

// IOleCommandTarget
public:
	STDMETHOD(QueryStatus)(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT *pCmdText);
	STDMETHOD(Exec)(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdExecOpt, VARIANT *pvaIn, VARIANT *pvaOut);

// IDispatch Methods
public:
	STDMETHOD(Invoke)(DISPID dispidMember,REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS * pdispparams, VARIANT * pvarResult,
		EXCEPINFO * pexcepinfo, UINT * puArgErr);

// IQueroBand
public:
	void FocusChange(BOOL bHaveFocus);
	inline IWebBrowser2 * GetWebBrowser() { return m_pWebBrowser2; }
	inline CQToolbar* GetToolbar() { return &m_Toolbar; }
	inline HWND GetParentWindow() { return m_hWndParent; }
	inline BOOL GetShowDW() { return m_bShow; }
	inline DWORD GetBandID() { return m_dwBandID; }
	inline DWORD GetViewMode() { return m_dwViewMode; }

	static bool ShowQueroToolbar(IWebBrowser2 *pWebBrowser,VARIANT_BOOL vbShow);

private:
	HRESULT RegisterAndCreateWindow();
	DWORD m_dwBandID;
	DWORD m_dwViewMode;
	BOOL m_bShow;
	BOOL m_bEnterHelpMode;
	HWND m_hWndParent;
	
	// InputObjectSite reference
	IInputObjectSite* m_pSite;
	
	// The Quero Toolbar
	CQToolbar m_Toolbar;

	// TRUE if blocking new windows
	BOOL m_bBlockNewWindow;

	// Connection Tokens - used for Advise and Unadvise
	DWORD m_dwBrowserCookie;   

	// Internet Explorer
	IWebBrowser2* m_pWebBrowser2;

	// Top-level web browser IDispatch*
	LPDISPATCH m_pTopLevelDisp;

	// IE DocHostUIHandler
	IDocHostUIHandler *m_pOrigDocHostUIHandler;
	bool m_ContextMenuInstalled;

	HRESULT ManageBrowserConnection(int eConnectType);

public:
	// Install the IHTMLWindow2.Open and IHTMLWindow3.showModelessDialog Wrapper to block pop-ups
	bool InstallPopUpBlocker(bool install);
	bool IsPopUpBlockerInstalled();

	// Install the IHTMLDocuemnt2.write(ln) Wrapper to block flash ads
	bool InstallAdBlocker(bool install);
	bool IsAdBlockerInstalled();

	// Install the DocHostUIHandler.ShowContextMenu Wrapper
	bool InstallContextMenu(bool install);
	bool IsContextMenuInstalled();
	
	bool bFirstNavigation; // First URL Navigation
	bool bDisplayBandOnNewLine;
};

#endif //__QUEROBAND_H_