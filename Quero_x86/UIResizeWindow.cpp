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
// UIResizeWindow.cpp: implementation of the CUIResizeWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIResizeWindow.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIResizeWindow::CUIResizeWindow() : CDialogImpl<CUIResizeWindow>()
{
}

CUIResizeWindow::~CUIResizeWindow()
{
}

LRESULT CUIResizeWindow::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	pToolbar->CenterDialog(m_hWnd);
	
	DoDataExchange(FALSE);

	return 1; // Let the system set the focus
}

LRESULT CUIResizeWindow::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch(LOWORD(wParam))
	{
	case IDOK:
		DoDataExchange(TRUE);
		EndDialog(IDOK);
		break;
	case IDCANCEL:
		EndDialog(IDCANCEL);
		break;
	}

	return 0;
}