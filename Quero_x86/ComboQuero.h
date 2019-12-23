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

// ComboQuero.h : Declaration of the CComboQuero

#ifndef __COMBOQUERO_H_
#define __COMBOQUERO_H_

#include <commctrl.h>
#include "QEditCtrl.h"

class CQueroBand;
class CQToolbar;

#define FIND_INITIATED_BY_QueroMenu 1
#define FIND_INITIATED_BY_SearchIcon 2
#define FIND_INITIATED_BY_FKey 3
#define FIND_INITIATED_BY_Enter 4

#define HOVER_NONE 0
#define HOVER_DROPDOWN 1
#define HOVER_URLICON 2
#define HOVER_SEARCHICON 3
#define HOVER_IDNBUTTON 4
#define HOVER_LOCKBUTTON 5
#define HOVER_CONTENTBLOCKEDBUTTON 6
#define HOVER_SEARCHENGINEBUTTON 7
#define HOVER_CERTIFICATE_ORGANIZATION 8

/////////////////////////////////////////////////////////////////////////////
// CComboQuero
class CComboQuero : public CWindowImpl<CComboQuero>
{
public:
	
	DECLARE_WND_SUPERCLASS(TEXT("COMBOQUERO"), TEXT("COMBOBOX"))

	BEGIN_MSG_MAP(CComboQuero)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnClickDown)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnClickDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnClickUp)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnClickDown)
		MESSAGE_HANDLER(WM_RBUTTONDBLCLK, OnClickDown)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnClickUp)
		MESSAGE_HANDLER(WM_CTLCOLOREDIT , OnCtlColorEdit)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
	END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlColorEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	CComboQuero();
	~CComboQuero();
	HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL, DWORD dwStyle = WS_CHILD | WS_VISIBLE, DWORD dwExStyle = 0, UINT nID = 0);
	STDMETHOD(TranslateAcceleratorIO)(LPMSG lpMsg);
	void SetBand(CQueroBand* pBand);
	inline void SetToolbar(CQToolbar *bar) { m_pToolbar=bar;}
	inline CQToolbar* GetToolbar() { return m_pToolbar;}
	inline CQEditCtrl* GetEditCtrl() { return &m_Edit;}
	int GetEditCtrlMargin();
	void SetText(LPCTSTR text,BYTE type,HICON hFavIcon,bool IsCurrentURL);
	void SetTextPrompt();
	void SetTextCurrentURL();
	void PositionText(bool bMoveIfPos0,bool bMoveIfFocused=false,bool bRedraw=true);
	inline BOOL SetWindowText(LPCTSTR lpszString) { return m_Edit.SetWindowText(lpszString); }
	inline BOOL GetWindowText(BSTR& bstrText) { return m_Edit.GetWindowText(bstrText); }
	inline int GetWindowText(LPTSTR lpszStringBuf, int nMaxCount ) { return m_Edit.GetWindowText(lpszStringBuf,nMaxCount); }
	inline bool GetText(BSTR &bstrText) { return m_Edit.GetText(bstrText); };
	TCHAR* GetFindText(BSTR& bstrQuery);
	void PrepareEditBackground(); // Prepares the background brush for the edit control	
	void OnHeightChange(int height);
	void UpdateComboBoxInfo();
	int HitTest(LPARAM lParam,RECT *rect,int *ButtonIndex);

	void Redraw(bool bUpdateEditBackground=false,UINT RedrawFlagsComboBox=RDW_INVALIDATE|RDW_NOERASE,UINT RedrawFlagsEditCtrl=RDW_INVALIDATE|RDW_NOERASE);

	void SubclassListWnd();

	void ShowToolTip(int id,bool bShow);

private:
	CQEditCtrl m_Edit; // The overlayed edit control
	HBRUSH hEditBckgrndBrush; // Pattern brush for the edit control
	HBITMAP hEditBckgrndBitmap; // Background bitmap for the pattern brush
	CQueroBand *m_pBand;
	CQToolbar *m_pToolbar;

	static WNDPROC ORIG_ListWndProc;
	static LRESULT CALLBACK ListWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

public:
	bool bIsEmptySearch; // Display help text: enter keywords or address 
	bool bIgnoreChange; // Ignore the next EN_CHANGE notification
	bool bHighlightIDN;
	int Hover;
	bool bCurrentURLDisplayed;
	bool bURLChanged;
	bool bTextChanged;
	short FirstCharOffset;
	int EditCtrlWidth; // Set by QToolbar::UpdateEmbedButtons
	HWND hToolTipControl;
	bool bToolTipActivated;

	HWND m_hWndItem; // hwnd of the selection field
	HWND m_hWndList; // hwnd of the drop-down list
	HWND m_hWndEdit; // hwnd of the embedded edit control
	RECT m_rcItem; // item rectangle
};

#endif //__COMBOQUERO_H_