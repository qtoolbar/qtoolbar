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
// UIWhiteList.h: interface for the CUIWhiteList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIWHITELIST_H__06465A06_3E1B_422B_8856_8FEA66616AA6__INCLUDED_)
#define AFX_UIWHITELIST_H__06465A06_3E1B_422B_8856_8FEA66616AA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "QToolbar.h"

#define N_WHITELIST_OPTIONS 8

class CUIWhiteList :
	public CDialogImpl<CUIWhiteList>,
	public CDialogResize<CUIWhiteList>,
	public CWinDataExchange<CUIWhiteList>
{
public:
	CUIWhiteList();
	~CUIWhiteList();

	enum { IDD = IDD_WHITELIST };

	BEGIN_MSG_MAP(CUIWhiteList)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		CHAIN_MSG_MAP(CDialogResize<CUIWhiteList>)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CUIWhiteList)
		DDX_CHECK(IDC_ALLOW_IDN, m_Allow_IDN)
		DDX_CHECK(IDC_ALLOW_MISSGLYPHS, m_Allow_MissGlyphs)
		DDX_CHECK(IDC_ALLOW_POPUPS, m_Allow_PopUps)
		DDX_CHECK(IDC_ALLOW_FLASH, m_Allow_Flash)
		DDX_CHECK(IDC_ALLOW_BANNER, m_Allow_Banner)
		DDX_CHECK(IDC_ALLOW_ADSCRIPT, m_Allow_AdScript)
		DDX_CHECK(IDC_ALLOW_DIV, m_Allow_DIV)
		DDX_CHECK(IDC_ALLOW_IFRAME, m_Allow_IFRAME)
		DDX_CHECK(IDC_SELECT_ALL, m_SelectAll)
	END_DDX_MAP()

	BEGIN_DLGRESIZE_MAP(CUIWhiteList)
		DLGRESIZE_CONTROL(IDC_RESET,DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDCANCEL,DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_WHITELIST,DLSZ_SIZE_X|DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()


	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void SetAllowSubItems(int index,SHORT permits);
	void SetAllowCheckboxes(SHORT permits);
	SHORT GetSelectedPermits();
	bool GetPattern(TCHAR pattern[MAXURLLENGTH]);
	SHORT GetWhiteListPermits(TCHAR pattern[MAXURLLENGTH]);

	CEdit m_Domain;
	CListViewCtrl m_WhiteList;

	bool m_Allow_IDN;
	bool m_Allow_MissGlyphs;
	bool m_Allow_PopUps;
	bool m_Allow_Flash;
	bool m_Allow_Banner;
	bool m_Allow_AdScript;
	bool m_Allow_DIV;
	bool m_Allow_IFRAME;

	bool m_SelectAll;

	bool MatchCurrentURL;

	bool bProcessNotifications;

	CQToolbar *m_pToolbar;
};

#endif // !defined(AFX_UIWHITELIST_H__06465A06_3E1B_422B_8856_8FEA66616AA6__INCLUDED_)
