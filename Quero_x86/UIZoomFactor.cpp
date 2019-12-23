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
// UIZoomFactor.cpp: implementation of the CUIZoomFactor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIZoomFactor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIZoomFactor::CUIZoomFactor() : CDialogImpl<CUIZoomFactor>()
{
}

CUIZoomFactor::~CUIZoomFactor()
{
}

LRESULT CUIZoomFactor::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	pToolbar->CenterDialog(m_hWnd);
	
	m_SpinZoomFactor=GetDlgItem(IDC_SPIN_ZOOMFACTOR);
	m_SpinZoomFactor.SetRange32(ZOOMFACTOR_MIN,ZOOMFACTOR_MAX);

	DoDataExchange(FALSE);

	return 1; // Let the system set the focus
}

LRESULT CUIZoomFactor::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT OldZoomFactor=m_ZoomFactor;
	bool bUpdateZoomFactor=false;

	switch(LOWORD(wParam))
	{
	case IDOK:
		DoDataExchange(TRUE);
		EndDialog(IDOK);
		bUpdateZoomFactor=true;
		break;
	case IDCANCEL:
		EndDialog(IDCANCEL);
		m_ZoomFactor=pToolbar->ZoomFactor;
		break;
	case IDC_EDIT_ZOOMFACTOR:
		if(HIWORD(wParam)==EN_CHANGE && IsWindowVisible())
		{
			DoDataExchange(TRUE);
		}
		break;
	}

	if(m_ZoomFactor!=OldZoomFactor || bUpdateZoomFactor) pToolbar->SetZoomFactor(m_ZoomFactor,bUpdateZoomFactor);

	return 0;
}