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
// UIOptionsSettings.h: interface for the CUIOptionsSettings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIOPTIONSSETTINGS_H__84F74EE8_461A_4A54_88A1_53CF85374172__INCLUDED_)
#define AFX_UIOPTIONSSETTINGS_H__84F74EE8_461A_4A54_88A1_53CF85374172__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "QToolbar.h"

class CUIOptionsSettings :
	public CPropertyPageImpl<CUIOptionsSettings>,
	public CWinDataExchange<CUIOptionsSettings>
{

public:
	CUIOptionsSettings(CQToolbar *pToolbar);

	enum { IDD = IDD_OPTIONS_SETTINGS };

	BEGIN_MSG_MAP(CUIOptionsSettings)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		CHAIN_MSG_MAP(CPropertyPageImpl<CUIOptionsSettings>)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CUIOptionsSettings)
		DDX_CHECK(IDC_CHKSHOWURL, m_ShowURL)
		DDX_CHECK(IDC_SHOW_SEARCH_TERMS_WHILE_SEARCHING, m_ShowSearchTermWhileSearching)
		DDX_CHECK(IDC_ADDRESS_NAVIGATION, m_AddressNavigation)
		DDX_CHECK(IDC_CHKIDNSUPPORT, m_IDNSupport)
		DDX_CHECK(IDC_PAGE_LOADING_ANIMATION, m_PageLoadingAnimation)
		DDX_CHECK(IDC_CHKHIGHLIGHT, m_Highlight)
		DDX_CHECK(IDC_HIGHLIGHT_IN_DIFFERENT_COLORS, m_HighlightInDifferentColors)
		DDX_CHECK(IDC_CHKSEARCHCOUNTRY, m_SearchCountry)
		DDX_CHECK(IDC_SAVE_ADDRESS_HISTORY, m_SaveAddressHistory)
		DDX_CHECK(IDC_SAVE_SEARCH_HISTORY, m_SaveSearchHistory)
		DDX_CHECK(IDC_REMEMBER_LAST_SEARCH, m_RememberLastSearch)
		DDX_CHECK(IDC_QUICK_TYPE, m_QuickType)
		DDX_CHECK(IDC_QUICK_FIND, m_QuickFind)
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

	void EnableHighlightInDifferentColors(bool bEnable);

	bool m_ShowURL;
	bool m_ShowSearchTermWhileSearching;
	bool m_AddressNavigation;
	bool m_IDNSupport;
	bool m_PageLoadingAnimation;
	bool m_Highlight;
	bool m_HighlightInDifferentColors;
	bool m_SearchCountry;
	bool m_SaveAddressHistory;
	bool m_SaveSearchHistory;
	bool m_RememberLastSearch;

	bool m_QuickType;
	bool m_QuickFind;

	CEdit m_Edit_QueroVersion;
	
	void SetOptions(UINT Options, UINT Options2);
	void GetOptions(UINT *pOptions, UINT *pOptions2);

	CQToolbar *m_pToolbar;
};

#endif // !defined(AFX_UIOPTIONSSETTINGS_H__84F74EE8_461A_4A54_88A1_53CF85374172__INCLUDED_)