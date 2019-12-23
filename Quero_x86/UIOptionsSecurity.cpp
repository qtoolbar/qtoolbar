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
// UIOptionsSecurity.cpp: implementation of the UIOptionsSecurity class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIOptionsSecurity.h"
#include "QToolbar.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIOptionsSecurity::CUIOptionsSecurity(CQToolbar *pToolbar)
{
	m_pToolbar=pToolbar;
}

LRESULT CUIOptionsSecurity::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 1; // Let the system set the focus
}

BOOL CUIOptionsSecurity::OnSetActive()
{
	DoDataExchange(FALSE);
	return TRUE;
}

LRESULT CUIOptionsSecurity::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch(LOWORD(wParam))
	{
	//case IDC_CMDRESETWL:
	//	if(MessageBox(CONFIRM_RESETWL,L"Quero Toolbar",MB_ICONWARNING|MB_YESNO)==IDYES)
	//		m_pToolbar->ResetWhiteList();
		
	//	return 0;
	case IDC_CMDWHITELIST:
		m_pToolbar->ShowWhiteList(m_hWnd);
		break;
	}

	bHandled=false;
	return 0;
}

BOOL CUIOptionsSecurity::OnKillActive()
{
	DoDataExchange(TRUE);
	return TRUE;
}
