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

// ComboQuero.cpp : Implementation of CComboQuero

**********************************************************************//

#include "stdafx.h"
#include "ComboQuero.h"
#include "Quero.h"
#include "QueroBand.h"

#include <urlhist.h>
#include <wininet.h>

// Undocumented wininet functions

INTERNETAPI_(BOOL) InternetShowSecurityInfoByURLW(LPCWSTR url,HWND hwnd);
INTERNETAPI_(BOOL) InternetShowSecurityInfoByURLA(LPCSTR url,HWND hwnd);

#ifdef UNICODE
#define InternetShowSecurityInfoByURL InternetShowSecurityInfoByURLW
#else
#define InternetShowSecurityInfoByURL InternetShowSecurityInfoByURLA
#endif


/////////////////////////////////////////////////////////////////////////////
// CComboQuero

CComboQuero::CComboQuero()
{
	m_pBand=NULL;
	m_pToolbar=NULL;

	hEditBckgrndBrush=NULL;
	hEditBckgrndBitmap=NULL;

	bIsEmptySearch=false;
	bIgnoreChange=false;
	bHighlightIDN=false;
	Hover=HOVER_NONE;
	bCurrentURLDisplayed=false;
	bURLChanged=false;
	bTextChanged=false;
	EditCtrlWidth=0;
	bToolTipActivated=false;
}

CComboQuero::~CComboQuero()
{
	if(hEditBckgrndBrush) DeleteObject(hEditBckgrndBrush);
	if(hEditBckgrndBitmap) DeleteObject(hEditBckgrndBitmap);
	if(hToolTipControl && ::IsWindow(hToolTipControl)) ::DestroyWindow(hToolTipControl);
}

WNDPROC CComboQuero::ORIG_ListWndProc=NULL;

LRESULT CALLBACK CComboQuero::ListWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	uMsg=MapMiddleButton_Message(uMsg);

	return CallWindowProc(ORIG_ListWndProc,hwnd,uMsg,wParam,lParam);
}

void CComboQuero::SubclassListWnd()
{
	if(ORIG_ListWndProc==NULL) ORIG_ListWndProc=(WNDPROC)::GetWindowLongPtr(m_hWndList,GWLP_WNDPROC);
	::SetWindowLongPtr(m_hWndList,GWLP_WNDPROC,(LONG_PTR)ListWndProc);
}

HWND CComboQuero::Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName, DWORD dwStyle, DWORD dwExStyle, UINT nID)
{
	HWND hWnd;
	int i;

	hWnd=CWindowImpl<CComboQuero>::Create(hWndParent,rcPos,szWindowName,dwStyle,dwExStyle,nID);

	UpdateComboBoxInfo();

	m_Edit.SetComboQuero(this);
	m_Edit.SetToolbar(m_pToolbar);
	m_hWndEdit=m_Edit.Create(hWnd, rcPos, NULL, WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|ES_LEFT, 0, IDC_EDITBOX);

	// Create the tooltip control for the embedded buttons
	TOOLINFO ti;
	
    hToolTipControl=CreateWindowEx(WS_EX_TOPMOST,
        TOOLTIPS_CLASS,
        NULL,
        WS_POPUP | TTS_NOPREFIX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        hWnd,
        NULL,
        NULL,
        NULL
        );

	if(hToolTipControl)
	{
		ti.cbSize=sizeof(TOOLINFO);
		ti.uFlags=TTF_SUBCLASS;
		ti.hwnd=hWnd;
		ti.hinst=NULL;
		ti.lpszText=LPSTR_TEXTCALLBACK;
		ti.rect=m_rcItem;
		ti.rect.right=QEDITCTRL_LEFTMARGIN+2;
		ti.rect.left=0;

		// Add embedded buttons and URL/search button
		for(i=0;i<MAXEMBEDBUTTONS+1;i++)
		{
			ti.uId=i;
			::SendMessage(hToolTipControl,TTM_ADDTOOL,0,(LPARAM)&ti);
		}

		// Add certificate information button
		ti.uId=MAXEMBEDBUTTONS+1;
		ti.rect.left=QEDITCTRL_LEFTMARGIN+2;
		ti.rect.right=QEDITCTRL_LEFTMARGIN+2+MAX_CERTIFICATE_ORGANIZATION_EXTENT;
		ti.rect.top=m_pToolbar->Padding_Top;
		ti.rect.bottom--;
		::SendMessage(hToolTipControl,TTM_ADDTOOL,0,(LPARAM)&ti);
	}

	return hWnd;
}

void CComboQuero::OnHeightChange(int height)
{
	// Set the height of the combo box and list items
	::SendMessage(m_hWnd,CB_SETITEMHEIGHT,(WPARAM)-1,height);
	::SendMessage(m_hWnd,CB_SETITEMHEIGHT,0,height+m_pToolbar->Margin_Items);

	// Update the dimensions of the combo box
	UpdateComboBoxInfo();
}

void CComboQuero::UpdateComboBoxInfo()
{
	COMBOBOXINFO info;

	info.cbSize=sizeof(COMBOBOXINFO);
	GetComboBoxInfo(m_hWnd,&info);
	m_hWndList=info.hwndList;
	m_hWndItem=info.hwndItem;
	m_rcItem=info.rcItem;
}

TCHAR* CComboQuero::GetFindText(BSTR& bstrQuery)
{
	TCHAR* pFindText;

	if(!bIsEmptySearch)
	{	
		if(GetText(bstrQuery))
		{
			m_pToolbar->trim(bstrQuery);
			if(bstrQuery[0]==L'/')
			{
				m_pToolbar->trim(bstrQuery+1);
				pFindText=bstrQuery+1;
			}
			else pFindText=bstrQuery;
		}
		else pFindText=NULL;
	}
	else pFindText=bstrQuery=SysAllocString(L"");

	return pFindText;
}

STDMETHODIMP CComboQuero::TranslateAcceleratorIO(LPMSG lpMsg)
{
	int nVirtKey = (int)(lpMsg->wParam);
	LRESULT droppedstate;
	HWND focus;
	bool bCtrlKeyDown=::GetKeyState(VK_CONTROL)<0; // Control key pressed?
	bool bShiftKeyDown=::GetKeyState(VK_SHIFT)<0; // Shift key pressed?
	BSTR bstrQuery;
	bool bAutoCompleteVisible;
	bool bHideDropDown;
	bool bTranslateDispatchMsg;

	focus=GetFocus();
	droppedstate=::SendMessage(m_hWnd,CB_GETDROPPEDSTATE,0,0);
	bTranslateDispatchMsg=true;

	// (ALT+)Enter pressed
	if(nVirtKey==VK_RETURN && (lpMsg->message==WM_KEYDOWN || lpMsg->message==WM_SYSKEYDOWN))
	{
		if(droppedstate)
		{
			UINT i=(UINT)::SendMessage(m_hWnd,CB_GETCURSEL,0,0);	
			if(i!=CB_ERR && i<m_pToolbar->GetHistoryIndex())
			{
				UINT oldOptions=g_Options;

				// Call CQToolbar::OnCommand (CBN_SELENDOK)
				g_Options|=OPTION_NavigateOnHistorySelection;
				SendMessage(WM_KEYDOWN,VK_RETURN,0);
				g_Options=oldOptions;
				return S_OK;
			}
			else ::SendMessage(m_hWnd,CB_SHOWDROPDOWN,FALSE,0);
		}
		if(focus==m_pToolbar->GetComboEngine()->m_hWnd && ::SendMessage(m_pToolbar->GetComboEngine()->m_hWnd,CB_GETDROPPEDSTATE,0,0))
		{
			// Rewrite WM_SYSKEYDOWN message
			lpMsg->message=WM_KEYDOWN;
			DispatchMessage(lpMsg);
			//if(!m_pToolbar->GetChooseProfile()) m_pBand->FocusChange(FALSE);
		}
		else
		{		
			if(m_Edit.GetText(bstrQuery))
			{
				m_pBand->FocusChange(FALSE);
				m_pToolbar->trim(bstrQuery);
				// Find on Page
				if(bstrQuery[0]==L'/' && m_pToolbar->GetCurrentType()==TYPE_SEARCH && (g_Options&OPTION_EnableQuickFind)==0)
				{
					m_pToolbar->FindOnPage(FIND_INITIATED_BY_Enter,(bstrQuery[1]?FIND_First:FIND_Next)|FIND_Focus);
				}
				else // Navigate
				{
					BYTE options;

					if(bstrQuery[0]==L'\0')
						options=QUERO_GO2HP; // Go2Hp if Query text is empty
					else if(bCtrlKeyDown && focus==m_hWndEdit)
						options=QUERO_IMFEELINGLUCKY;
					else options=0;

					::PostMessage(GetParent(), WM_QUERO, (WPARAM)options, SHORTCUT_OPTION_SEARCHBOX|SHORTCUT_OPTION_ENTERKEY); 
				}

				SysFreeString(bstrQuery);
			}
		}
		
		bTranslateDispatchMsg=false;
	}
	
	// WM_KEYDOWN
	else if(WM_KEYDOWN == lpMsg->message)
	{
		bHideDropDown=true;
		switch(nVirtKey)
		{
		case VK_ESCAPE:
			if(focus==m_hWndEdit)
			{
				if(!IsBelowWindows2000()) bAutoCompleteVisible=m_pToolbar->IsAutoCompleteVisible();

				DispatchMessage(lpMsg);

				if(droppedstate) ::SendMessage(m_hWnd,CB_SHOWDROPDOWN,FALSE,0);
				else
				{
					// Workaround: AC state unkown: first select whole text and then reset content
					#ifndef COMPILE_FOR_WINDOWS_VISTA					
					if(IsBelowWindows2000())
					{
						DWORD len,start,end;

						len=SendMessage(m_hWndEdit,WM_GETTEXTLENGTH,0,0);
						::SendMessage(m_hWndEdit,EM_GETSEL,(WPARAM)&start,(LPARAM)&end);
						if(len && start && end!=len-1) // Whole text is not selected?
						{
							::SendMessage(m_hWndEdit,EM_SETSEL,0,-1); // Select all
							bAutoCompleteVisible=true;
						}
						else bAutoCompleteVisible=false;
					}
					#endif

					if(!bAutoCompleteVisible) // Reset content of search box
					{
						if(g_ShowURL && m_pToolbar->currentURL[0])
						{
							SetTextCurrentURL();
							::SendMessage(m_hWndEdit,EM_SETSEL,0,-1);
						}
						else SetText(L"",TYPE_SEARCH,NULL,false);
					}
				}

				bTranslateDispatchMsg=false;
			}
			bHideDropDown=false;
			break;
		case VK_UP:
		case VK_DOWN:
		case VK_PRIOR: // page up
		case VK_NEXT: // page down
			if(focus==m_hWndEdit)
			{
				if(bCtrlKeyDown && (nVirtKey==VK_UP || nVirtKey==VK_DOWN))
				{
					m_pToolbar->SelectNextPrevEngine(nVirtKey==VK_DOWN);
					bTranslateDispatchMsg=false;
				}
				else if(droppedstate)
				{
					lpMsg->hwnd=m_hWnd; // Pass message to the drop-down list
				}
				else
				{				
					// Test if AutoComplete is active
					// To do: Use IsAutoCompleteVisible on Windows 2000+

					bTextChanged=false;
					DispatchMessage(lpMsg); // Pass Cursor-Keys to AutoComplete
					if(!bTextChanged) // AutoComplete is not active
					{
						::SendMessage(m_hWnd,CB_SHOWDROPDOWN,TRUE,0);
					}

					bTranslateDispatchMsg=false;
				}
			}
			bHideDropDown=false;
			break;
		case VK_SHIFT:
			bHideDropDown=false;
			break;
		case VK_DELETE:
			if(bShiftKeyDown && focus==m_hWndEdit)
			{
				if(m_Edit.GetText(bstrQuery))
				{
					// Delete entry from Quero history
					m_pToolbar->DeleteFromHistory(bstrQuery);
					m_pToolbar->DeleteFromURLHistory(bstrQuery);

					// Delete entry from IE URL history
					if(m_pToolbar->GetIDNA()->IsAddress(bstrQuery))
					{
						IUrlHistoryStg* pUrlHistoryStg;
						HRESULT hr;

						pUrlHistoryStg=NULL;
						hr = CoCreateInstance(CLSID_CUrlHistory,NULL,CLSCTX_INPROC_SERVER, IID_IUrlHistoryStg,(LPVOID*)&pUrlHistoryStg);
						if(SUCCEEDED_OK(hr))
						{
							pUrlHistoryStg->DeleteUrl(bstrQuery,0);
							pUrlHistoryStg->Release();
						}
					}

					SysFreeString(bstrQuery);
				}
				SetText(L"",TYPE_SEARCH,NULL,false);
			}
			break;
		case 'V':
			if(bCtrlKeyDown && bShiftKeyDown)
			{
				m_Edit.PostMessage(WM_PASTE_AND_GO,0,SHORTCUT_OPTION_IGNORE_MODIFIERS);
				bTranslateDispatchMsg=false;
			}
			break;
		case 'A':
			if(bCtrlKeyDown)
			{
				::SendMessage(m_hWndEdit,EM_SETSEL,0,-1);
				bTranslateDispatchMsg=false;
			}
			break;
		case 'L':
			if(bCtrlKeyDown)
			{
				m_pToolbar->ToggleCurrentType();
				bTranslateDispatchMsg=false;
			}
			break;
		}
		if(bHideDropDown && droppedstate && focus==m_hWndEdit) ::SendMessage(m_hWnd,CB_SHOWDROPDOWN,FALSE,0);
	}
	
	// WM_KEYUP
	else if(WM_KEYUP == lpMsg->message)
	{
		if(VK_F4 == nVirtKey && focus==m_hWndEdit)
		{
			::SendMessage(m_hWnd,CB_SHOWDROPDOWN,droppedstate?FALSE:TRUE,0);
			bTranslateDispatchMsg=false;
		}
	}

	if(bTranslateDispatchMsg)
	{
		TranslateMessage(lpMsg);
		DispatchMessage(lpMsg);
	}
	return S_OK;
}

void CComboQuero::SetBand(CQueroBand* pBand)
{
	m_pBand = pBand;
	m_Edit.SetBand(pBand);
}

LRESULT CComboQuero::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_Edit.SetFocus();

	bHandled = FALSE;
	return 0;
}

LRESULT CComboQuero::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// if(GetFocus()==m_hWndEdit) return FALSE; // do not change the cursor if control has not the focus
	return FALSE;
}

int CComboQuero::HitTest(LPARAM lParam,RECT *rect,int *ButtonIndex)
{
	int x=LOWORD(lParam);
	int result=HOVER_NONE;

	*ButtonIndex=-1;
	
	m_Edit.GetClientRect(rect);
	m_Edit.MapWindowPoints(m_hWnd,rect);

	if(x<rect->left)
	{
		if(x<QEDITCTRL_LEFTMARGIN+2)
		{
			result=(m_pToolbar->GetCurrentType()==TYPE_SEARCH)?HOVER_SEARCHICON:HOVER_URLICON;
		}
		else if(m_pToolbar->Certificate_Organization_Extent) result=HOVER_CERTIFICATE_ORGANIZATION;
	}
	else if(x>=rect->right)
	{
		int nbuttons=m_pToolbar->nEmbedIcons;

		if(nbuttons && x<=m_rcItem.right)
		{
			x--;
			if(x>=(m_rcItem.right-EMBEDICONS_MARGIN)) *ButtonIndex=0;
			else
			{
				*ButtonIndex=((m_rcItem.right-EMBEDICONS_MARGIN)-x)/EMBEDICONS_SLOTWIDTH;
				if(*ButtonIndex>=nbuttons) *ButtonIndex=nbuttons-1;
			}

			switch(m_pToolbar->EmbedButtons[*ButtonIndex])
			{
			case EMBEDBUTTON_CONTENTBLOCKED:
				result=HOVER_CONTENTBLOCKEDBUTTON;
				break;
			case EMBEDBUTTON_SEARCHENGINE:
				result=HOVER_SEARCHENGINEBUTTON;
				break;
			case EMBEDBUTTON_IDN:
				result=HOVER_IDNBUTTON;
				break;
			case EMBEDBUTTON_LOCK:
				result=HOVER_LOCKBUTTON;
				break;
			}
		}
		else result=HOVER_DROPDOWN;
	}

	return result;
}

LRESULT CComboQuero::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RECT rect;
	int NewHover;
	int ButtonIndex;

	NewHover=HitTest(lParam,&rect,&ButtonIndex);

	//READYSTATE state;

	//m_pBand->GetWebBrowser()->get_ReadyState(&state);
	//if(state==READYSTATE_LOADING) SetCursor(LoadCursor(NULL,IDC_APPSTARTING));
	//else SetCursor(LoadCursor(NULL,IDC_ARROW));
	SetCursor(LoadCursor(NULL,IDC_ARROW));

	// Disable hover effect of drop-down button
	#ifndef COMPILE_FOR_WINDOWS_VISTA
	if(IsBelowWindowsVista() && Hover==HOVER_DROPDOWN && NewHover!=HOVER_DROPDOWN) SendMessage(WM_MOUSELEAVE,0);
	#endif

	switch(NewHover)
	{
	case HOVER_DROPDOWN:
		if(::SendMessage(m_hWnd,CB_GETDROPPEDSTATE,0,0)) bHandled=TRUE;
		else bHandled=FALSE;
		break;
	case HOVER_IDNBUTTON:
		if(Hover!=HOVER_IDNBUTTON) 
		{
			// Highlight IDN if focus is not in the edit box
			if(GetFocus()!=m_hWndEdit && bHighlightIDN==false)
			{
				bHighlightIDN=true;
				SetTextCurrentURL();
				m_Edit.ShowWindow(SW_HIDE);
			}
		}
		break;
	case HOVER_URLICON:
		if(wParam==MK_LBUTTON) m_Edit.StartDragOperation(true,0);
		break;
	}

	if(IsWindowsVistaOrLater()) bHandled=FALSE;

	// Update status text
	//if(NewHover!=Hover && NewHover!=HOVER_SEARCHICON && (NewHover>HOVER_DROPDOWN || Hover>HOVER_DROPDOWN)) m_pToolbar->PutStatusText(pStatusText);
	
	// Turn IDN highlighting off?
	if(bHighlightIDN && NewHover!=HOVER_IDNBUTTON)
	{
		bHighlightIDN=false;
		m_Edit.ShowWindow(SW_SHOW);
	}

	// Track when mouse leaves the window
	if(NewHover!=Hover)
	{
		TRACKMOUSEEVENT me;
		
		me.cbSize=sizeof(TRACKMOUSEEVENT);
		me.hwndTrack=m_hWnd;
		me.dwFlags=TME_LEAVE;
		TrackMouseEvent(&me);
	}

	Hover=NewHover;
	
	return 0;
}

LRESULT CComboQuero::OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT result;
	UINT id;
	UINT HintId=0;

	switch(lpnm->code)
	{
	case TTN_NEEDTEXT:
		id=(UINT)lpdi->hdr.idFrom;

		if(id<(UINT)m_pToolbar->nEmbedIcons)
		{
			switch(m_pToolbar->EmbedButtons[id])
			{
				case EMBEDBUTTON_LOCK:
					HintId=IDS_HINT_LOCK;
					break;
				case EMBEDBUTTON_IDN:
					HintId=(m_pToolbar->SpecialCharsInURL==SPECIALCHARS_IDN)?IDS_HINT_IDN:IDS_HINT_123;
					break;
				case EMBEDBUTTON_CONTENTBLOCKED:
					HintId=IDS_HINT_CONTENTBLOCKED;
					break;
				case EMBEDBUTTON_SEARCHENGINE:
					lpdi->lpszText=(TCHAR*)m_pToolbar->GetEngineName();
					break;
				default:
					lpdi->lpszText=NULL;
			}
		}
		else if(id==MAXEMBEDBUTTONS && m_pToolbar->GetCurrentType()==TYPE_SEARCH && m_pToolbar->GetPhraseNotFound()==false) HintId=IDS_HINT_FIND;
		else if(id==(MAXEMBEDBUTTONS+1) && m_pToolbar->Certificate_Organization_Extent) HintId=IDS_HINT_LOCK;
		else lpdi->lpszText=NULL;

		if(HintId) lpdi->lpszText=m_pToolbar->GetString(HintId);
		lpdi->hinst=NULL;
		result=0;
		break;
	// End TTN_NEEDTEXT
	
	default:
		bHandled=FALSE;
		result=-1;
	}

	return result;
}

LRESULT CComboQuero::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(Hover==HOVER_SEARCHICON && m_pToolbar->GetCurrentType()==TYPE_SEARCH)
	{
		if(m_pToolbar->GetPhraseNotFound()==false) m_pToolbar->SetSearchIcon(ICON_SEARCH);
	}

	if(bHighlightIDN)
	{
		bHighlightIDN=false;
		m_Edit.ShowWindow(SW_SHOW);
	}

	Hover=HOVER_NONE;

	bHandled=FALSE;

	return 0;
}


LRESULT CComboQuero::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(SendMessage(CB_GETDROPPEDSTATE,0,0)==FALSE)
	{
		bHandled=TRUE;
		return 0;
	}
	else
	{
		bHandled=FALSE;
		return -1;
	}
}

LRESULT CComboQuero::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(LOWORD(wParam)==IDC_EDITBOX && HIWORD(wParam)==EN_CHANGE)
	{
		if(m_Edit.bDoubleClick)
		{
			m_Edit.bDoubleClick=false;
			ReleaseCapture();
		}

		// Analyze current text
		BSTR bstrQuero;
		TCHAR* pPhrase;
	
		pPhrase=GetFindText(bstrQuero);
		if(pPhrase)
		{
			// Clear LastFoundText?
			if(*pPhrase && StrCmpI(pPhrase,m_pToolbar->GetLastFoundText())) m_pToolbar->ClearLastFoundText();

			// Reset PhraseNotFound if no search is performed or phrase is empty
			if(((g_Options&OPTION_EnableQuickFind)==0 && bstrQuero[0]!=L'/') || bstrQuero[0]==0 || (bstrQuero[0]==L'/' && bstrQuero[1]==0)) m_pToolbar->SetPhraseNotFound(false);

			if(!bIgnoreChange)
			{
				// Update state
				bCurrentURLDisplayed=false;
				bTextChanged=true;
				// Set type
				m_pToolbar->SetCurrentType(((g_Options2&OPTION2_AddressNavigation) && m_pToolbar->GetIDNA()->IsAddress(bstrQuero))?TYPE_ADDRESS:TYPE_SEARCH,NULL);
				// Perform QuickSearch
				if((g_Options&OPTION_EnableQuickFind) || bstrQuero[0]==L'/')
				{
					m_pToolbar->QuickFind(pPhrase);
				}
			}
			else bIgnoreChange=false;

			SysFreeString(bstrQuero);
		}
	}
	else bHandled=FALSE;

	return 0;
}

void CComboQuero::SetText(LPCTSTR text,BYTE type,HICON hFavIcon,bool IsCurrentURL)
{
	bIgnoreChange=true;
	bIsEmptySearch=false;
	bCurrentURLDisplayed=IsCurrentURL;
	bURLChanged=true;
	
	// Update type and icon
	m_pToolbar->SetPhraseNotFound(false,true);
	m_pToolbar->SetCurrentType(type,hFavIcon,false);

	// Update URL
	m_Edit.SetRedraw(FALSE);
	SendMessage(m_hWndEdit,WM_SETTEXT,0,(LPARAM)text);
	// Make sure that the core domain is visible
	PositionText(false,false,false);

	// Redraw
	m_Edit.SetRedraw(TRUE);
	RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_NOERASE|RDW_UPDATENOW);
	m_Edit.RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_NOERASE|RDW_UPDATENOW);
}

void CComboQuero::SetTextPrompt()
{
	SetText((g_Options2&OPTION2_SearchPrompt)?m_pToolbar->GetString(IDS_PROMPT):L"",TYPE_SEARCH,NULL,false);
	bIsEmptySearch=true;
}

void CComboQuero::SetTextCurrentURL()
{
	SetText(m_pToolbar->currentURL,TYPE_ADDRESS,m_pToolbar->currentFavIcon,true);
}

void CComboQuero::PositionText(bool bMoveIfPos0,bool bMoveIfFocused,bool bRedraw)
{
	bool bHasFocus;
	int pos;

	bHasFocus=(GetFocus()==m_hWndEdit);

  // Make core domain visible
	if(bHasFocus==false || bMoveIfFocused)
	{
		// Determine caret position of core domain
		if(bCurrentURLDisplayed && (m_pToolbar->CoreDomainEndExtent+3)>EditCtrlWidth && m_pToolbar->IsIPv6==false)
			pos=m_pToolbar->HostEndIndex;
		else
			pos=0;

		if(pos || bMoveIfPos0)
		{
			if(bRedraw) m_Edit.SetRedraw(FALSE);
			// Make visible range consistent by selecting Pos 0 first
			if(pos)	SendMessage(m_hWndEdit,EM_SETSEL,0,0);
			// Position caret
			SendMessage(m_hWndEdit,EM_SETSEL,pos,pos);
			if(bRedraw)
			{
				m_Edit.SetRedraw(TRUE);
				// Workaround: gdi bug: on EM_SETSEL background is not erased with the background brush
				m_Edit.RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_NOERASE|RDW_UPDATENOW);
			}
		}
	}
}

LRESULT CComboQuero::OnClickDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RECT rect;
	int Hover;
	int ButtonIndex;

	Hover=HitTest(lParam,&rect,&ButtonIndex);

	if(Hover==HOVER_SEARCHICON && bIsEmptySearch) Hover=HOVER_URLICON;

	switch(Hover)
	{
	case HOVER_SEARCHICON:
		m_pToolbar->FindOnPage(FIND_INITIATED_BY_SearchIcon,((uMsg==WM_LBUTTONDOWN || uMsg==WM_LBUTTONDBLCLK)?FIND_Next:FIND_Previous)|FIND_Focus);
		break;

	case HOVER_URLICON:
		if(GetFocus()==m_hWndEdit) ::SendMessage(m_hWndEdit,EM_SETSEL,0,-1);
		else m_Edit.SetFocus();
		break;

	case HOVER_IDNBUTTON:
		if(uMsg==WM_LBUTTONDBLCLK && m_pToolbar->SpecialCharsInURL==SPECIALCHARS_IDN)
		{
			SetText(m_pToolbar->currentAsciiURL,TYPE_ADDRESS,m_pToolbar->GetFavIcon(),false);
			if(bHighlightIDN)
			{
				bHighlightIDN=false;
				m_Edit.ShowWindow(SW_SHOW);
			}
		}
		else if(bHighlightIDN==false)
		{
			TRACKMOUSEEVENT me;
			
			bHighlightIDN=true;
			
			me.cbSize=sizeof(TRACKMOUSEEVENT);
			me.hwndTrack=m_hWnd;
			me.dwFlags=TME_LEAVE;
			TrackMouseEvent(&me);

			m_pBand->FocusChange(FALSE);
			SetText(m_pToolbar->currentURL,TYPE_ADDRESS,m_pToolbar->GetFavIcon(),true);
			m_Edit.ShowWindow(SW_HIDE);
		}
		break;

	case HOVER_LOCKBUTTON:
	case HOVER_CERTIFICATE_ORGANIZATION:
		// Show security certificate does not work
		/*#define SHDVID_SSLSTATUS 33
		IOleCommandTarget *pct;

		if(SUCCEEDED_OK(m_pBand->GetWebBrowser()->QueryInterface(IID_IOleCommandTarget,(LPVOID*)&pct)))
		{
			pct->Exec(&CGID_ShellDocView, SHDVID_SSLSTATUS, 0, NULL, NULL);
			pct->Release();
		}*/

		// Above code from msdn does nothing, so using undocumented function from wininet.dll instead
		SetTextCurrentURL();
		InternetShowSecurityInfoByURL(m_pToolbar->currentAsciiURL,m_hWndEdit);
		break;

	case HOVER_CONTENTBLOCKEDBUTTON:
		m_pToolbar->SendMessage(WM_QUERO_TOOLBAR_COMMAND,QUERO_COMMAND_VIEW_BLOCKED_CONTENT,0);
		break;

	case HOVER_SEARCHENGINEBUTTON:
		m_pToolbar->GetComboEngine()->SetFocus();
		m_pToolbar->GetComboEngine()->PostMessage(CB_SHOWDROPDOWN,TRUE,0);
		break;

	case HOVER_DROPDOWN:
		bHandled=FALSE;
		break;
	}

	return 0;
}


LRESULT CComboQuero::OnClickUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(uMsg==WM_LBUTTONUP && ::SendMessage(m_hWnd,CB_GETDROPPEDSTATE,0,0)) bHandled=FALSE;
	else bHandled=TRUE;
	
	return 0;
}

void CComboQuero::PrepareEditBackground()
{
	HDC hEditDC;
	HDC memDC;
	RECT rect;
	LONG BmpWidth,BmpHeight;

	hEditDC=m_Edit.GetDC();
	if(hEditDC)
	{
		memDC=CreateCompatibleDC(hEditDC);
		if(memDC)
		{
			m_Edit.GetWindowRect(&rect);
			BmpWidth=GetSystemMetrics(SM_CXVIRTUALSCREEN); // Build a larger pattern brush in the case the edit control is expanded
			BmpHeight=rect.bottom-rect.top;
			rect.left=0;
			rect.right=BmpWidth;
			rect.top=0;
			rect.bottom=BmpHeight;

			if(hEditBckgrndBitmap) DeleteObject(hEditBckgrndBitmap);
			hEditBckgrndBitmap=CreateCompatibleBitmap(hEditDC,BmpWidth,BmpHeight);
			SelectObject(memDC,hEditBckgrndBitmap);

			FillRect(memDC,&rect,m_pToolbar->GetDefaultBackground());

			FirstCharOffset=(short)m_Edit.SendMessage(EM_POSFROMCHAR,0,0);

#ifdef COMPILE_FOR_WIN9X
			if(GetVersion()&0x80000000) FirstCharOffset+=25; // Win9X
#endif

			rect.left=m_pToolbar->CoreDomainStartExtent+FirstCharOffset;
			rect.right=m_pToolbar->CoreDomainEndExtent+FirstCharOffset;
			rect.bottom--;

			FillRect(memDC,&rect,m_pToolbar->GetHighlightBrush());

			//m_Edit.GetWindowRect(&rect);
			//BitBlt(hEditDC,rect.left,rect.top,BmpWidth,BmpHeight,memDC,0,0,SRCCOPY);

			if(hEditBckgrndBrush) DeleteObject(hEditBckgrndBrush);

			hEditBckgrndBrush=CreatePatternBrush(hEditBckgrndBitmap);

			DeleteDC(memDC);
		}
		m_Edit.ReleaseDC(hEditDC);
	}
}

LRESULT CComboQuero::OnCtlColorEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT result;
	short NewFirstCharOffset;
	int color;
	
	if(m_pToolbar->GetCurrentType()==TYPE_ADDRESS) color=COLOR_Link;
	else if(m_pToolbar->GetPhraseNotFound()) color=COLOR_NotFound;
	else color=COLOR_Search;

	SetTextColor((HDC)wParam,m_pToolbar->GetColor(color));

	bHandled=TRUE;

	if((g_Options&OPTION_HighlightDomain) && bCurrentURLDisplayed && GetFocus()!=m_hWndEdit && m_pToolbar->CoreDomainStartExtent!=m_pToolbar->CoreDomainEndExtent)
	{
		NewFirstCharOffset=(short)m_Edit.SendMessage(EM_POSFROMCHAR,0,0);

		if(bURLChanged || NewFirstCharOffset!=FirstCharOffset)
		{
			PrepareEditBackground();
			bURLChanged=false;
		}

		SetBkMode((HDC)wParam,TRANSPARENT);
		SelectObject((HDC)wParam,hEditBckgrndBrush);
		SetBrushOrgEx((HDC)wParam,0,0,NULL);

		result=(LRESULT)hEditBckgrndBrush;
	}
	else
	{
		SetBkMode((HDC)wParam,OPAQUE);
		SetBkColor((HDC)wParam,m_pToolbar->GetColor(COLOR_Background));
		result=(LRESULT)m_pToolbar->GetDefaultBackground();
	}

	return result;
}

LRESULT CComboQuero::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RECT rect;

	// Workaround for Windows Vista: when switching between tabs or when clicking on the recycle bin button the area of the embedded edit control is sometimes getting corrupted
	if(IsWindowsVistaOrLater())
	{
		// Validate rectangle of embedded edit control
		if(bHighlightIDN==false)
		{
			m_Edit.GetWindowRect(&rect);
			ScreenToClient(&rect);
			ValidateRect(&rect);
		}
		// Validate animating icon since it is drawn directly in CQToolbar::SetIconOffset
		if(m_pToolbar->m_IconAnimation.IsAnimating())
		{
			CopyRect(&rect,&m_rcItem);
			rect.right=rect.left+21;
			ValidateRect(&rect);
		}
	}

	bHandled=FALSE;
	return 0;
}

void CComboQuero::Redraw(bool bUpdateEditBackground,UINT RedrawFlagsComboBox,UINT RedrawFlagsEditCtrl)
{
	if(bUpdateEditBackground) bURLChanged=true;
	if(RedrawFlagsComboBox) RedrawWindow(NULL,NULL,RedrawFlagsComboBox);
	if(RedrawFlagsEditCtrl) m_Edit.RedrawWindow(NULL,NULL,RedrawFlagsEditCtrl);
}

void CComboQuero::ShowToolTip(int id,bool bShow)
{
	if(!(bShow==false && bToolTipActivated==false) && hToolTipControl)
	{
		TOOLINFO ti;

		ti.cbSize=sizeof ti;
		ti.hwnd=m_hWnd;
		ti.uId=id;

		if(::SendMessage(hToolTipControl,TTM_GETTOOLINFO,0,(LPARAM)&ti))
		{
			if(bShow)
			{
				if(bToolTipActivated==false)
				{
					ti.uFlags|=TTF_CENTERTIP;
					::SendMessage(hToolTipControl,TTM_SETTOOLINFO,0,(LPARAM)&ti);
					::SendMessage(hToolTipControl,TTM_TRACKACTIVATE,TRUE,(LPARAM)&ti);
				}
				else ::SendMessage(hToolTipControl,TTM_UPDATE,0,0);
				
			}
			else
			{
				if(bToolTipActivated)
				{
					::SendMessage(hToolTipControl,TTM_POP,0,0);
					::SendMessage(hToolTipControl,TTM_TRACKACTIVATE,FALSE,(LPARAM)&ti);
					ti.uFlags&=~TTF_CENTERTIP;
					::SendMessage(hToolTipControl,TTM_SETTOOLINFO,0,(LPARAM)&ti);
				}
			}

			bToolTipActivated=bShow;
		}
	}
}

int CComboQuero::GetEditCtrlMargin()
{
	return LOWORD(m_Edit.SendMessage(EM_GETMARGINS,0,0));
}