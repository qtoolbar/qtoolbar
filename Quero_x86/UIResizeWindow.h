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
// UIResizeWindow.h: interface for the CUIResizeWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIRESIZEWINDOW_H__INCLUDED_)
#define AFX_UIRESIZEWINDOW_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "QToolbar.h"

class CUIResizeWindow :
	public CDialogImpl<CUIResizeWindow>,
	public CWinDataExchange<CUIResizeWindow>
{
public:
	CUIResizeWindow();
	~CUIResizeWindow();

	enum { IDD = IDD_RESIZEWINDOW };

	BEGIN_MSG_MAP(CUIResizeWindow)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CUIResizeWindow)
		DDX_INT(IDC_EDIT_WIDTH, m_Width)
		DDX_INT(IDC_EDIT_HEIGHT, m_Height)
	END_DDX_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	int m_Width;
	int m_Height;

	CQToolbar *pToolbar;
};

#endif
