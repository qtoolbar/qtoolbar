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

// LogoToolbar.h : Declaration of the CLogoToolbar

#ifndef __LOGOTOOLBAR_H_
#define __LOGOTOOLBAR_H_

#include <commctrl.h>
#include "Toolbar.h"
#include "ToolbarIcons.h"

#define LOGOGAP 2 // Gap between the Quero button and serach box

/////////////////////////////////////////////////////////////////////////////
// CLogoToolbar
class CLogoToolbar : public CWindowImpl<CLogoToolbar,CToolbar>
{
public:

	DECLARE_WND_SUPERCLASS(TEXT("LOGOTOOLBAR"), TOOLBARCLASSNAME)

	BEGIN_MSG_MAP(CLogoToolbar)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		//MESSAGE_HANDLER(WM_ERASEBKGND, OnBackground)
		//MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
	END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
//	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//	LRESULT OnBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//	LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	CLogoToolbar();
	void LoadToolbarIcons();
	void SetImageLists();
	void UpdatePosition();

	void UpdateLogoImage();
	static bool m_bWhiteLogo;
	static CToolbarIcons m_ToolbarIcons;
};

#endif //__LOGOTOOLBAR_H_
