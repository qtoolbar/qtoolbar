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
// UIOptionsAdvanced.cpp: implementation of the CUIOptionsAdvanced class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIOptionsAdvanced.h"

#define  pnmv ((LPNMLISTVIEW)lParam)

const TCHAR* UA_Values[UA_NFIELDS]={NULL,L"Compatible",L"Version",L"Platform"};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIOptionsAdvanced::CUIOptionsAdvanced(CQToolbar *pToolbar)
{
	m_pToolbar=pToolbar;
}

void CUIOptionsAdvanced::SetOptions(UINT Options, UINT Options2)
{
	m_DispalyPrompt=(Options2&OPTION2_SearchPrompt)!=0;
	m_DownloadFavIcon=(Options&OPTION_DownloadFavIcon)!=0;
	m_SearchOnDNSFailure=(Options&OPTION_SearchOnDNSFailure)!=0;
	m_EnableQuickFind=(Options&OPTION_EnableQuickFind)!=0;
	m_NavigateOnACSelection=(Options&OPTION_NavigateOnACSelection)!=0;
	m_NavigateOnHistorySelection=(Options&OPTION_NavigateOnHistorySelection)!=0;
	m_ACUseTab=(Options&OPTION_ACUseTab)!=0;
	m_ShowFavIconsInTaskbar=(Options&OPTION_ShowFavIconsInTaskbar)!=0;
	m_QueroContextMenuExtension=(Options2&OPTION2_EnableQueroContextMenu)!=0;
	m_FavoritesButton=(g_Options2&OPTION2_HideFavoritesButton)==0;
	m_DPI_Scaling=(Options2&OPTION2_DPI_Scaling)!=0;
	m_QueroShortcutKey=0x3-OPTION_QueroShortcutKey(Options);
	m_NewWinTab_SearchBox=OPTION_NewWinTab_SearchBox(Options);
	m_NewWinTab_ContextMenu=OPTION_NewWinTab_ContextMenu(Options);
	IE6_MapNewTabToNewWin(m_NewWinTab_SearchBox);
	IE6_MapNewTabToNewWin(m_NewWinTab_ContextMenu);
}

void CUIOptionsAdvanced::GetOptions(UINT *pOptions, UINT *pOptions2)
{
	UINT Options=*pOptions;
	UINT Options2=*pOptions2;

	if(m_DispalyPrompt) Options2|=OPTION2_SearchPrompt;
	else Options2&=~OPTION2_SearchPrompt;
	if(m_DownloadFavIcon) Options|=OPTION_DownloadFavIcon;
	else Options&=~OPTION_DownloadFavIcon;
	if(m_SearchOnDNSFailure) Options|=OPTION_SearchOnDNSFailure;
	else Options&=~OPTION_SearchOnDNSFailure;
	if(m_EnableQuickFind) Options|=OPTION_EnableQuickFind;
	else Options&=~OPTION_EnableQuickFind;
	if(m_NavigateOnACSelection) Options|=OPTION_NavigateOnACSelection;
	else Options&=~OPTION_NavigateOnACSelection;
	if(m_NavigateOnHistorySelection) Options|=OPTION_NavigateOnHistorySelection;
	else Options&=~OPTION_NavigateOnHistorySelection;
	if(m_ACUseTab) Options|=OPTION_ACUseTab;
	else Options&=~OPTION_ACUseTab;
	if(m_ShowFavIconsInTaskbar) Options|=OPTION_ShowFavIconsInTaskbar;
	else Options&=~OPTION_ShowFavIconsInTaskbar;
	if(m_QueroContextMenuExtension) Options2|=OPTION2_EnableQueroContextMenu;
	else Options2&=~OPTION2_EnableQueroContextMenu;
	if(m_FavoritesButton) Options2&=~OPTION2_HideFavoritesButton;
	else Options2|=OPTION2_HideFavoritesButton;
	if(m_DPI_Scaling) Options2|=OPTION2_DPI_Scaling;
	else Options2&=~OPTION2_DPI_Scaling;

	SetOPTION_QueroShortcutKey(Options,0x3-m_QueroShortcutKey);

	SetOPTION_NewWinTab_SearchBox(Options,m_NewWinTab_SearchBox);
	SetOPTION_NewWinTab_ContextMenu(Options,m_NewWinTab_ContextMenu);

	*pOptions=Options;
	*pOptions2=Options2;
}

BOOL CUIOptionsAdvanced::ExchangeData(BOOL bSaveAndValidate)
{
	BOOL result;
	int i;

	result=DoDataExchange(bSaveAndValidate);
	if(bSaveAndValidate==FALSE)
	{
		UpdateEnableState();
		m_ComboQueroShortcutKey.SetCurSel(m_QueroShortcutKey);
		m_ComboNewWinTab_SearchBox.SetCurSel(m_NewWinTab_SearchBox);
		m_ComboNewWinTab_ContextMenu.SetCurSel(m_NewWinTab_ContextMenu);

		for(i=0;i<UA_NFIELDS;i++) m_Edit_CustomUserAgent[i].SetWindowText(m_CustomUserAgent[i]);
	}
	else
	{
		m_QueroShortcutKey=m_ComboQueroShortcutKey.GetCurSel();
		m_NewWinTab_SearchBox=m_ComboNewWinTab_SearchBox.GetCurSel();
		m_NewWinTab_ContextMenu=m_ComboNewWinTab_ContextMenu.GetCurSel();

		for(int i=0;i<UA_NFIELDS;i++) m_Edit_CustomUserAgent[i].GetWindowText(m_CustomUserAgent[i],MAX_UA_LEN);
	}

	return result;
}

void CUIOptionsAdvanced::UpdateEnableState()
{
	::EnableWindow(GetDlgItem(IDC_CHKSHOWFAVICONSINTASKBAR),m_DownloadFavIcon);
}

LRESULT CUIOptionsAdvanced::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int i,n;

	if(g_IE_MajorVersion!=7 && g_IE_MajorVersion!=8) ::EnableWindow(GetDlgItem(IDC_FAVORITES_BUTTON),FALSE);

	m_ComboQueroShortcutKey=GetDlgItem(IDC_QUEROSHORTCUTKEY);
	m_ComboNewWinTab_SearchBox=GetDlgItem(IDC_NEWWINTAB_SEARCHBOX);
	m_ComboNewWinTab_ContextMenu=GetDlgItem(IDC_NEWWINTAB_CONTEXTMENU);

	m_Edit_CurrentUserAgent=GetDlgItem(IDC_CURRENT_USER_AGENT);
	m_Edit_CurrentUserAgent.SetWindowText(m_CurrentUserAgent);

	const int IDC_Edit_CustomUserAgent[UA_NFIELDS]={IDC_UA_PREFIX,IDC_UA_COMPATIBLE,IDC_UA_VERSION,IDC_UA_PLATFORM};

	for(i=0;i<UA_NFIELDS;i++)
	{
		m_Edit_CustomUserAgent[i]=GetDlgItem(IDC_Edit_CustomUserAgent[i]);
		StringCbCopy(m_CustomUserAgent[i],sizeof m_CustomUserAgent[0],m_OldCustomUserAgent[i]);
	}

	COMBOBOXINFO info;
	CComboBox *ComboBoxes[3]={&m_ComboQueroShortcutKey,&m_ComboNewWinTab_SearchBox,&m_ComboNewWinTab_ContextMenu};

	info.cbSize=sizeof(COMBOBOXINFO); // Workaround: in Win2k combobox drop-down window is shrunken to 0 pixel
	for(i=0;i<3;i++)
	{
		GetComboBoxInfo(ComboBoxes[i]->m_hWnd,&info);
		::MoveWindow(info.hwndList,0,0,100,100,FALSE);
	}

	const UINT QueroShortcutKeyOptions[4]={IDS_OPTION_CTRL_Q,IDS_OPTION_ALT_Q,IDS_OPTION_ALT_D,IDS_OPTION_ALT_S};
	
	for(i=0;i<4;i++) m_ComboQueroShortcutKey.AddString(m_pToolbar->GetString(QueroShortcutKeyOptions[i]));

	const UINT OpenOptions[4]={IDS_OPTION_OPEN_SAMEWINDOW,IDS_OPTION_OPEN_NEWWINDOW,IDS_OPTION_OPEN_NEWTAB,IDS_OPTION_OPEN_BACKGROUNDTAB};

	if(g_IE_MajorVersion<7) n=2;
	else n=4;

	for(i=0;i<n;i++)
	{
		TCHAR *pString;

		pString=m_pToolbar->GetString(OpenOptions[i]);
		m_ComboNewWinTab_SearchBox.AddString(pString);
		m_ComboNewWinTab_ContextMenu.AddString(pString);
	}

	return 1; // Let the system set the focus
}

BOOL CUIOptionsAdvanced::OnSetActive()
{
	ExchangeData(FALSE);
	return TRUE;
}

LRESULT CUIOptionsAdvanced::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int i;

	switch(LOWORD(wParam))
	{
	case IDC_CHKDOWNLOADFAVICON:
		if(HIWORD(wParam)==BN_CLICKED)
		{
			DoDataExchange(TRUE,IDC_CHKDOWNLOADFAVICON);
			UpdateEnableState();
		}
		bHandled=FALSE;
		break;
	case IDC_CMDRESTORE:
		SetOptions(DEFAULT_OPTIONS,DEFAULT_OPTIONS2);
		for(i=0;i<UA_NFIELDS;i++) m_CustomUserAgent[i][0]=L'\0';
		ExchangeData(FALSE);
		break;
	default:
		bHandled=FALSE;
	}

	return 0;
}

BOOL CUIOptionsAdvanced::OnKillActive()
{
	ExchangeData(TRUE);

	if(g_IE_MajorVersion<7)
	{
		if(m_NewWinTab_SearchBox==OPEN_NewWindow) m_NewWinTab_SearchBox=OPEN_NewTab;
		if(m_NewWinTab_ContextMenu==OPEN_NewWindow) m_NewWinTab_ContextMenu=OPEN_BackgroundTab;
	}

	return TRUE;
}

void CUIOptionsAdvanced::ReadUserAgent()
{
	IServiceProvider* pServiceProvider;
	IWebBrowser2 *pWebBrowser;
	IHTMLWindow2 *pWindow;
	IOmNavigator *pNavigator;
	HRESULT hr;

	pWebBrowser=m_pToolbar->GetBrowser();

	if(pWebBrowser)
	{
		hr = pWebBrowser->QueryInterface(IID_IServiceProvider, (LPVOID*)&pServiceProvider);
		if(SUCCEEDED_OK(hr))
		{
			hr = pServiceProvider->QueryService(IID_IHTMLWindow2, IID_IHTMLWindow2, (LPVOID*)&pWindow);
			if(SUCCEEDED_OK(hr))
			{
				hr=pWindow->get_navigator(&pNavigator);

				if(SUCCEEDED_OK(hr))
				{
					BSTR bsUserAgent;
					
					hr=pNavigator->get_userAgent(&bsUserAgent);
					if(SUCCEEDED_OK(hr))
					{
						StringCbCopy(m_CurrentUserAgent,sizeof m_CurrentUserAgent,bsUserAgent);
						SysFreeString(bsUserAgent);
					}
				}
			}
			pServiceProvider->Release();
		}
	}

	HKEY hKeyUserAgent;
	DWORD size,type;

	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\5.0\\User Agent",0,NULL,REG_OPTION_NON_VOLATILE,KEY_READ,NULL,&hKeyUserAgent,NULL)==ERROR_SUCCESS)
	{
		for(int i=0;i<UA_NFIELDS;i++)
		{
			size=sizeof m_CustomUserAgent[0];
			hr=RegQueryValueEx(hKeyUserAgent,UA_Values[i],0,&type,(LPBYTE)m_CustomUserAgent[i],&size);
			if(hr!=ERROR_SUCCESS || type!=REG_SZ) m_CustomUserAgent[i][0]=0;

			StringCchCopy(m_OldCustomUserAgent[i],MAX_UA_LEN,m_CustomUserAgent[i]);
		}
				
		RegCloseKey(hKeyUserAgent);
	}
}

void CUIOptionsAdvanced::WriteUserAgent()
{
	HKEY hKeyUserAgent;
	bool bChanged[UA_NFIELDS];
	bool bUpdate;
	int i;

	bUpdate=false;

	for(i=0;i<UA_NFIELDS;i++)
	{
		bChanged[i]=StrCmp(m_OldCustomUserAgent[i],m_CustomUserAgent[i])!=0;
		if(bChanged[i]) bUpdate=true;
	}

	if(bUpdate)
	{
		if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\5.0\\User Agent",0,NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE,NULL,&hKeyUserAgent,NULL)==ERROR_SUCCESS)
		{
			for(i=0;i<UA_NFIELDS;i++)
			{
				if(bChanged[i])
				{
					if(m_CustomUserAgent[i][0]==0) RegDeleteValue(hKeyUserAgent,UA_Values[i]);
					else
					{
						size_t len;
						StrCbLen(m_CustomUserAgent[i],MAX_UA_LEN,len);
						if(len)
						{
							RegSetValueEx(hKeyUserAgent,UA_Values[i],0,REG_SZ,(LPBYTE)m_CustomUserAgent[i],(DWORD)len+sizeof TCHAR);
						}
					}
				}
			}

			RegCloseKey(hKeyUserAgent);
		}

	}

}