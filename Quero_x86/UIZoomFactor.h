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
// UIZoomFactor.h: interface for the CUIZoomFactor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIZOOMFACTOR_H__INCLUDED_)
#define AFX_UIZOOMFACTOR_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "QToolbar.h"

class CUIZoomFactor :
	public CDialogImpl<CUIZoomFactor>,
	public CWinDataExchange<CUIZoomFactor>
{
public:
	CUIZoomFactor();
	~CUIZoomFactor();

	enum { IDD = IDD_ZOOMFACTOR };

	BEGIN_MSG_MAP(CUIZoomFactor)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CUIZoomFactor)
		DDX_UINT(IDC_EDIT_ZOOMFACTOR, m_ZoomFactor)
	END_DDX_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	UINT m_ZoomFactor;
	CUpDownCtrl m_SpinZoomFactor;

	CQToolbar *pToolbar;
};

#endif
