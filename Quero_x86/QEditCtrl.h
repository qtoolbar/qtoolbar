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

// QEditCtrl.h : Declaration of the CEditCtrl

#ifndef __QEDITCTRL_H_
#define __QEDITCTRL_H_

#include <commctrl.h>
#include "resource.h"

class CQueroBand;
class CComboQuero;
class CQToolbar;

#define MAX_DRAGDROP_FORMATS 7
#define MAX_FILENAME_CCH 127

// Private Message Commands

#define WM_PASTE_AND_GO (WM_USER + 1024)
#define WM_UP_ONE_LEVEL (WM_USER + 1025)


/////////////////////////////////////////////////////////////////////////////
// CQEditCtrl
class CQEditCtrl : public CWindowImpl<CQEditCtrl> , public IDropTarget
{
public:

	DECLARE_WND_SUPERCLASS(TEXT("CQEDITCTRL"), TEXT("EDIT"))

	BEGIN_MSG_MAP(CQEditCtrl)
		#ifdef COMPILE_FOR_WINDOWS_VISTA
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnErase)
		MESSAGE_HANDLER(WM_CHAR, OnEvent_RedrawOpaque)
		MESSAGE_HANDLER(EM_SETSEL, OnEvent_RedrawOpaque)
		#endif
		//MESSAGE_HANDLER(WM_NCCALCSIZE, OnNCCalcSize)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnDoubleClick)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnClickDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnClickUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
		MESSAGE_HANDLER(WM_PASTE, OnPaste)
		MESSAGE_HANDLER(WM_PASTE_AND_GO, OnPasteAndGo)
		MESSAGE_HANDLER(WM_MBUTTONUP, OnMiddleClickUp)
		MESSAGE_HANDLER(WM_UP_ONE_LEVEL, OnUpOneLevel)
	END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
//	LRESULT OnNCCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDoubleClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaste(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPasteAndGo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMiddleClickUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnUpOneLevel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	#ifdef COMPILE_FOR_WINDOWS_VISTA
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnErase(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEvent_RedrawOpaque(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	#endif

    // IUnknown implementation
    HRESULT __stdcall QueryInterface (REFIID iid, void ** ppvObject);
    ULONG   __stdcall AddRef (void);
    ULONG   __stdcall Release (void);

    // IDropTarget implementation
    HRESULT __stdcall DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
    HRESULT __stdcall DragOver(DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
    HRESULT __stdcall DragLeave(void);
    HRESULT __stdcall Drop(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);

	CQEditCtrl();
	virtual ~CQEditCtrl();
	void SetBand(CQueroBand* pBand);
	inline void SetComboQuero(CComboQuero *pComboQuero) {m_pComboQuero=pComboQuero;}
	inline void SetToolbar(CQToolbar *pToolbar) {m_pToolbar=pToolbar;}
	void StartDragOperation(bool bDragIcon,LPARAM lParamCursorPos);
	int InitDragDropData(FORMATETC fmtetc[MAX_DRAGDROP_FORMATS],STGMEDIUM stgmed[MAX_DRAGDROP_FORMATS]);
	void MakeValidFileName(TCHAR *pName,size_t cchName);
	bool MouseInSelection(LPARAM MouseMsgParam);
	void PositionCaret(POINTL pt);
	void CreateDefaultCaret(bool bShowCaret);
	void DropData(IDataObject *pDataObject,POINTL pt);

	bool GetText(BSTR &bstrText);

	bool SetRedraw(BOOL bRedraw = TRUE);

private:
	// Internal helper function
	DWORD DropEffect(DWORD grfKeyState, POINTL pt, DWORD dwAllowed);
	UINT QueryDataObject(IDataObject *pDataObject);
	long   m_lRefCount;

	CQueroBand *m_pBand;
	CComboQuero *m_pComboQuero;
	CQToolbar *m_pToolbar;
	FILETIME lastDoubleClickTime;
	LPARAM lastDoubleClicklParam;
	bool bFirstClick;
	bool bFirstDoubleClick;
	bool bStartDragDrop;
	bool bDidDragDrop;
	bool bAllowDrop;
	bool bDidDrop;
	int dropPos;
	UINT uiSetRedrawFalseCount;
public:
	bool bSelectText;
	bool bDoubleClick;
	int startSel;
	int endSel;

	// CallWndProcHook members

	static LRESULT CALLBACK CallWndProcHook(int nCode,WPARAM wParam,LPARAM lParam);
	static CQEditCtrl *Hook_pQEditCtrl;
	HHOOK Hook_hCallWndProcHook;
	BSTR Hook_bstrPasteAndGo;
	BSTR Hook_bstrUpOneLevel;
	bool Hook_bPasteAndGoAdded;
	bool Hook_bUpOneLevelAdded;
	bool Hook_bUpOneLevelEnabled;
};

#endif // __EDITCONTROL_H_