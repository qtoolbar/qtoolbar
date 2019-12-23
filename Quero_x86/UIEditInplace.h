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
// UIEditInplace.h: interface for the CUIEditInplace class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIEDITINPLACE_H__9F315292_3642_4CAC_AA9D_EAA9D5BD556C__INCLUDED_)
#define AFX_UIEDITINPLACE_H__9F315292_3642_4CAC_AA9D_EAA9D5BD556C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CUIEditInplace : public CWindowImpl<CUIEditInplace, CEdit>
{
public:
	//CUIEditInplace();
	//~CUIEditInplace();

	BEGIN_MSG_MAP(CUIEditInplace)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
		MESSAGE_HANDLER(WM_GETDLGCODE, OnGetDlgCode)
	END_MSG_MAP()

	// Message handlers
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetDlgCode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};

#endif // !defined(AFX_UIEDITINPLACE_H__9F315292_3642_4CAC_AA9D_EAA9D5BD556C__INCLUDED_)
