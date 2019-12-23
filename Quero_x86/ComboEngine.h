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

// ComboEngine.h : Declaration of the CComboEngine

#ifndef __COMBOENGINE_H_
#define __COMBOENGINE_H_

#include <commctrl.h>

class CQueroBand;
class CQToolbar;

/////////////////////////////////////////////////////////////////////////////
// CComboEngine
class CComboEngine : public CWindowImpl<CComboEngine>
{
public:

	DECLARE_WND_SUPERCLASS(TEXT("QENGINE"), TEXT("COMBOBOX"))

	BEGIN_MSG_MAP(CComboEngine)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
	END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	CComboEngine();
	~CComboEngine();
	HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL, DWORD dwStyle = WS_CHILD | WS_VISIBLE, DWORD dwExStyle = 0, UINT nID = 0);
	void SetBand(CQueroBand* pBand);
	void SetToolbar(CQToolbar* pToolbar);	
	inline CQToolbar* GetToolbar() {return m_pToolbar;}
	
	HWND m_hWndItem; // hwnd of the edit control
	HWND m_hWndList; // hwnd of the edit control's list
	RECT m_rcButton; // Drop-down button dimensions

	void OnHeightChange(int height);
	void UpdateComboBoxInfo();
	void SubclassListWnd();

private:
	CQueroBand *m_pBand;
	CQToolbar *m_pToolbar;

	static WNDPROC ORIG_ListWndProc;
	static LRESULT CALLBACK ListWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

public:
	bool bHighlightSelection;
};

#endif //__COMBOENGINE_H_