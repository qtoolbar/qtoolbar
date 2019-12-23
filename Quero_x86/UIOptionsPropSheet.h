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
// UIOptionsPropSheet.h: interface for the CUIOptionsPropSheet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIOPTIONSPROPSHEET_H__5C635CEF_58AE_45F9_87D8_0B06F7E3DF17__INCLUDED_)
#define AFX_UIOPTIONSPROPSHEET_H__5C635CEF_58AE_45F9_87D8_0B06F7E3DF17__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CUIOptionsPropSheet :
	public CPropertySheetImpl<CUIOptionsPropSheet>
{
public:
	CUIOptionsPropSheet(UINT nTitleID = NULL, UINT uStartPage = 0, HWND hWndParent = NULL);

	BEGIN_MSG_MAP(CUIOptionsPropSheet)
		CHAIN_MSG_MAP(CPropertySheetImpl<CUIOptionsPropSheet>)
	END_MSG_MAP()
};

#endif // !defined(AFX_UIOPTIONSPROPSHEET_H__5C635CEF_58AE_45F9_87D8_0B06F7E3DF17__INCLUDED_)