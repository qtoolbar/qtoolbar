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
// UISearchProfile.h: interface for the CUISearchProfile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UISEARCHPROFILE_H__D766E307_A5B3_433F_BB7D_9A68441474F4__INCLUDED_)
#define AFX_UISEARCHPROFILE_H__D766E307_A5B3_433F_BB7D_9A68441474F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "QToolbar.h"

class CQToolbar;

class CUISearchProfile  :
	public CDialogImpl<CUISearchProfile>,
	public CDialogResize<CUISearchProfile>
//	public CWinDataExchange<CUISearchProfile>
{
public:
	CUISearchProfile(Profile *pProfile,UINT WindowTitleId,CQToolbar *pToolbar);
	~CUISearchProfile();

	enum { IDD = IDD_SEARCHPROFILE };

	BEGIN_MSG_MAP(CUISearchProfile)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		CHAIN_MSG_MAP(CDialogResize<CUISearchProfile>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CUISearchProfile)
		DLGRESIZE_CONTROL(IDC_STATIC_FRAME,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_PROFILE_NAME,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_PROFILE_LUCKYSEARCH,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDCANCEL,DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDOK,DLSZ_MOVE_X)
	END_DLGRESIZE_MAP()

/*	BEGIN_DDX_MAP(CUISearchProfile)
	END_DDX_MAP()*/

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	bool SaveProfile();

	CEdit m_Edit_Name;
	CEdit m_Edit_LuckyURL;

	Profile *m_pProfile;
	UINT WindowTitleId;
	CQToolbar *m_pToolbar;
};

#endif // !defined(AFX_UISEARCHPROFILE_H__D766E307_A5B3_433F_BB7D_9A68441474F4__INCLUDED_)