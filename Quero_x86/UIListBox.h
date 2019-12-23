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
// UIListBox.h: interface for the CUIListBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UILISTBOX_H__EA87B436_5E66_4BBD_AAB0_B8DE88E0292B__INCLUDED_)
#define AFX_UILISTBOX_H__EA87B436_5E66_4BBD_AAB0_B8DE88E0292B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Private Message Commands

#define WM_LISTBOX_KEYDOWN (WM_USER)

class CUIListBox  : public CWindowImpl<CUIListBox, CListBox>
{
public:
	//CUIListBox();
	//~CUIListBox();

	BEGIN_MSG_MAP(CUIListBox)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
	END_MSG_MAP()

	// Message handlers
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

};

#endif // !defined(AFX_UILISTBOX_H__EA87B436_5E66_4BBD_AAB0_B8DE88E0292B__INCLUDED_)
