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
// UIOptionsAdvanced.h: interface for the CUIOptionsAdvanced class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIOPTIONSADVANCED_H__90C868E9_EC3E_4B0D_BD11_49DA6EF5E01C__INCLUDED_)
#define AFX_UIOPTIONSADVANCED_H__90C868E9_EC3E_4B0D_BD11_49DA6EF5E01C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "QToolbar.h"

#define MAX_UA_LEN 2048
#define UA_PREFIX 0
#define UA_COMPATIBLE 1
#define UA_VERSION 2
#define UA_PLATFORM 3
#define UA_NFIELDS 4

class CUIOptionsAdvanced :
	public CPropertyPageImpl<CUIOptionsAdvanced>,
	public CWinDataExchange<CUIOptionsAdvanced>
{

public:
	CUIOptionsAdvanced(CQToolbar *pToolbar);

	enum { IDD = IDD_OPTIONS_ADVANCED };

	BEGIN_MSG_MAP(CUIOptionsAdvanced)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		CHAIN_MSG_MAP(CPropertyPageImpl<CUIOptionsAdvanced>)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CUIOptionsAdvanced)
		DDX_CHECK(IDC_DISPLAY_PROMPT, m_DispalyPrompt)
		DDX_CHECK(IDC_CHKDOWNLOADFAVICON, m_DownloadFavIcon)
		DDX_CHECK(IDC_CHKSEARCHONDNSFAILURE, m_SearchOnDNSFailure)
		DDX_CHECK(IDC_CHKNAVIGATEONACSEL, m_NavigateOnACSelection)
		DDX_CHECK(IDC_CHKNAVIGATEONHISTSEL, m_NavigateOnHistorySelection)
		DDX_CHECK(IDC_CHKACUSETAB, m_ACUseTab)
		DDX_CHECK(IDC_CHKSHOWFAVICONSINTASKBAR, m_ShowFavIconsInTaskbar)
		DDX_CHECK(IDC_QUERO_CONTEXT_MENU_EXTENSION, m_QueroContextMenuExtension)
		DDX_CHECK(IDC_FAVORITES_BUTTON, m_FavoritesButton)
		DDX_CHECK(IDC_DPI_SCALING, m_DPI_Scaling)
	END_DDX_MAP()


// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	BOOL OnSetActive();
	BOOL OnKillActive();
	BOOL ExchangeData(BOOL bSaveAndValidate);

	void UpdateEnableState();

	void ReadUserAgent();
	void WriteUserAgent();

	bool m_DispalyPrompt;
	bool m_DownloadFavIcon;
	bool m_SearchOnDNSFailure;
	bool m_EnableQuickFind;
	bool m_NavigateOnACSelection;
	bool m_NavigateOnHistorySelection;
	bool m_ACUseTab;
	bool m_ShowFavIconsInTaskbar;
	bool m_QueroContextMenuExtension;
	bool m_FavoritesButton;
	bool m_DPI_Scaling;

	int m_QueroShortcutKey;
	
	int m_NewWinTab_SearchBox;
	int m_NewWinTab_ContextMenu;

	CComboBox m_ComboQueroShortcutKey;
	CComboBox m_ComboNewWinTab_SearchBox;
	CComboBox m_ComboNewWinTab_ContextMenu;
	
	CEdit m_Edit_CurrentUserAgent;
	CEdit m_Edit_CustomUserAgent[UA_NFIELDS];

	TCHAR m_CurrentUserAgent[MAX_UA_LEN];
	TCHAR m_CustomUserAgent[UA_NFIELDS][MAX_UA_LEN];
	TCHAR m_OldCustomUserAgent[UA_NFIELDS][MAX_UA_LEN];

	void SetOptions(UINT Options, UINT Options2);
	void GetOptions(UINT *pOptions, UINT *pOptions2);

	CQToolbar *m_pToolbar;
};

#endif // !defined(AFX_UIOPTIONSADVANCED_H__90C868E9_EC3E_4B0D_BD11_49DA6EF5E01C__INCLUDED_)