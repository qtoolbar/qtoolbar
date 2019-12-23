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

// ButtonBar.h : Declaration of the CButtonBar

#ifndef __ButtonBar_H_
#define __ButtonBar_H_

#include <commctrl.h>
#include "Toolbar.h"
#include "ToolbarIcons.h"

/////////////////////////////////////////////////////////////////////////////
// CButtonBar
class CButtonBar : public CWindowImpl<CButtonBar,CToolbar>
{
public:

	DECLARE_WND_SUPERCLASS(TEXT("BUTTONBAR"), TOOLBARCLASSNAME)

	BEGIN_MSG_MAP(CButtonBar)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		//MESSAGE_HANDLER(WM_ERASEBKGND, OnBackground)
		//MESSAGE_HANDLER(WM_NOTIFY, OnNotify)		
		MESSAGE_HANDLER(WM_MBUTTONUP , OnMiddleClickUp)
	END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
//	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//	LRESULT OnBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//	LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMiddleClickUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	CButtonBar();

	void LoadToolbarIcons();
	void SetImageLists();
	void UpdatePosition();

	static CToolbarIcons m_ToolbarIcons;
};

#endif //__ButtonBar_H_