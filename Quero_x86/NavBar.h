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

// NavBar.h : Declaration of the CNavBar

#ifndef __NAVBAR_H_
#define __NAVBAR_H_

#include <commctrl.h>
#include <tlogstg.h>
#include "Toolbar.h"
#include "ToolbarIcons.h"

#define MAX_TRAVEL_LOG_ENTRIES 32

#define NAVBUTTON_TOTAL_WIDTH (8+g_Scaled_NavButtonSize)

/////////////////////////////////////////////////////////////////////////////
// CNavBar
class CNavBar : public CWindowImpl<CNavBar,CToolbar>
{
public:

	DECLARE_WND_SUPERCLASS(TEXT("NAVBAR"), TOOLBARCLASSNAME)

	BEGIN_MSG_MAP(CNavBar)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		//MESSAGE_HANDLER(WM_ERASEBKGND, OnBackground)
		//MESSAGE_HANDLER(WM_NOTIFY, OnNotify)

		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRightClick) // Needed to completely intercept the click
		MESSAGE_HANDLER(WM_RBUTTONUP, OnRightClick)
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
	LRESULT OnRightClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMiddleClickUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	CNavBar();
	void LoadToolbarIcons();
	void SetImageLists();
	void UpdatePosition();

	UINT GetTravelLogEntries(bool bRelativeBack,UINT MaxTravelLogEntries,ITravelLogEntry *pTravelLogEntries[MAX_TRAVEL_LOG_ENTRIES],ITravelLogStg **ppTravelLog);
	void ShowTravelLog(bool bRelativeBack);
	TCHAR* GetTravelLogTooltip(TCHAR *pTooltip,size_t cbTooltip,bool bRelativeBack);
	void GoBackForward(bool bRelativeBack,UINT ShortcutOptions);

	static CToolbarIcons m_ToolbarIcons;
};

#endif //__NAVBAR_H_