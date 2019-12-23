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
// UISearchEngine.h: interface for the CUISearchEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UISEARCHENGINE_H__DDAF814A_196E_4645_9660_4961B0DBA905__INCLUDED_)
#define AFX_UISEARCHENGINE_H__DDAF814A_196E_4645_9660_4961B0DBA905__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "QToolbar.h"

#define EDIT_COUNT 6
#define EDIT_NAME 0
#define EDIT_QUERYURL 1
#define EDIT_POSTQUERY 2
#define EDIT_COUNTRYPARAM 3
#define EDIT_LINKURL 4
#define EDIT_ICON 5

class CQToolbar;

class CUISearchEngine  :
	public CDialogImpl<CUISearchEngine>,
	public CDialogResize<CUISearchEngine>
	//public CWinDataExchange<CUISearchEngine>
{
public:
	CUISearchEngine(SearchEngine *pEngine,UINT WindowTitleId,CQToolbar *pToolbar);
	~CUISearchEngine();

	enum { IDD = IDD_SEARCHENGINE };

	BEGIN_MSG_MAP(CUISearchEngine)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		CHAIN_MSG_MAP(CDialogResize<CUISearchEngine>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CUISearchEngine)
		DLGRESIZE_CONTROL(IDC_STATIC_FRAME,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_ENGINE_NAME,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_ENGINE_QUERYURL,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_ENGINE_LINKURL,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_ENGINE_POSTQUERY,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_ENGINE_COUNTRYPARAM,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_ENGINE_ICON,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_ENGINE_ICON_BROWSE,DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDCANCEL,DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDOK,DLSZ_MOVE_X)
	END_DLGRESIZE_MAP()

	/*BEGIN_DDX_MAP(CUISearchEngine)
	END_DDX_MAP()*/

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	bool SaveEngine();

	CEdit m_Edit[EDIT_COUNT];
	CComboBox m_Encoding;

	int OldEncoding;

	SearchEngine *m_pEngine;
	UINT WindowTitleId;
	CQToolbar *m_pToolbar;
};

#endif // !defined(AFX_UISEARCHENGINE_H__DDAF814A_196E_4645_9660_4961B0DBA905__INCLUDED_)
