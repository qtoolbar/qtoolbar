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
// UIWarning.cpp: implementation of the CUIWarning class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIWarning.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIWarning::CUIWarning(int x,int y,TCHAR *title,TCHAR *description,HFONT font,HFONT fontbold,bool whitelist,bool proceed) : CDialogImpl<CUIWarning>()
{
	Title=title;
	Description=description;
	hFont=font;
	hFontBold=fontbold;
	PosX=x;
	PosY=y;
	EnableAddToWhiteList=whitelist;
	EnableProceed=proceed;
	hBackgroundBrush=CreateSolidBrush(0xFFFFFF);
}

CUIWarning::~CUIWarning()
{
	if(hBackgroundBrush) DeleteObject(hBackgroundBrush);
}

LRESULT CUIWarning::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Set Quero Theme Icons
	if(g_QueroTheme_DLL)
	{
		CStatic StaticBitmap;
		HBITMAP hBitmap;

		hBitmap=LoadBitmap(g_QueroTheme_DLL,MAKEINTRESOURCE(IDB_SHIELD));
		if(hBitmap)
		{
			StaticBitmap=GetDlgItem(IDC_BITMAP_SHIELD);
			StaticBitmap.SetBitmap(hBitmap);
		}
	}

	m_Title = GetDlgItem(IDC_TITLE);
	m_Description = GetDlgItem(IDC_DESCRIPTION);

	if(!EnableAddToWhiteList) ::EnableWindow(GetDlgItem(IDC_CHKADDTOWHITELIST),false);
	if(!EnableProceed) ::EnableWindow(GetDlgItem(IDOK),false);

	m_Title.SetFont(hFontBold);
	m_Description.SetFont(hFont);

	m_Title.SetWindowText(Title);
	m_Description.SetWindowText(Description);

	SetWindowPos(HWND_TOP,PosX,PosY,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOREDRAW);

	return 1; // Let the system set the focus
}

LRESULT CUIWarning::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CUIWarning::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return (LRESULT)hBackgroundBrush;
}