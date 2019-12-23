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
// UIWarning.h: interface for the CUIWarning class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIWARNING_H__76E5A116_59F4_4579_9DC3_B9A637A9BECA__INCLUDED_)
#define AFX_UIWARNING_H__76E5A116_59F4_4579_9DC3_B9A637A9BECA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "QToolbar.h"

class CUIWarning :
	public CDialogImpl<CUIWarning>,
	public CWinDataExchange<CUIWarning>
{
public:
	CUIWarning(int x,int y,TCHAR *title,TCHAR *description,HFONT font,HFONT fontbold,bool whitelist,bool proceed);
	~CUIWarning();

	enum { IDD = IDD_WARNING };

	BEGIN_MSG_MAP(CUIWarning)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CTLCOLORDLG, OnCtlColor)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CUIWarning)
		DDX_CHECK(IDC_CHKADDTOWHITELIST, m_AddToWhiteList)
	END_DDX_MAP()


	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	inline bool IsAddToWhiteList() {return m_AddToWhiteList;}

private:

	HBRUSH hBackgroundBrush;

	CStatic m_Title;
	CEdit m_Description;
	bool m_AddToWhiteList;

	TCHAR *Title;
	TCHAR *Description;

	HFONT hFont;
	HFONT hFontBold;

	int PosX,PosY; // Dialog position
	bool EnableAddToWhiteList;
	bool EnableProceed;
};

#endif // !defined(AFX_UIWARNING_H__76E5A116_59F4_4579_9DC3_B9A637A9BECA__INCLUDED_)
