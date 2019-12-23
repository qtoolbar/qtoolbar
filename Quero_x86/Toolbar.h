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

// Toolbar.h: interface for the CToolbar class.

#ifndef __Toolbar_H_
#define __Toolbar_H_

#include <commctrl.h>
#include "ToolbarIcons.h"

#define MAX_BUTTONS 5

class CQToolbar;

/////////////////////////////////////////////////////////////////////////////
// CToolbar
class CToolbar : public CWindow
{
public:

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//	LRESULT OnBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//	LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	CToolbar();
	~CToolbar();
	
	inline void SetToolbar(CQToolbar* p) { m_pToolbar=p; }
	virtual void LoadToolbarIcons()=0;
	virtual void SetImageLists()=0;
	inline UINT GetVisibleButtons() { return VisibleButtons; }
	inline void SetVisibleButtons(UINT Buttons) { VisibleButtons=Buttons; }
	inline bool HasVisibleButtons() { return (VisibleButtons&ButtonMask)!=0; }
	int GetButtonIndex(WORD ButtonId);

	void OnHeightChange(int height);
	virtual void UpdatePosition()=0;
	
	void ShowButtons(UINT Buttons);
	void EnableButton(UINT CommandId,BOOL bEnable);
	bool IsButtonPressed(UINT CommandId);
	void PressButton(UINT CommandId,BOOL bPressButton);
	void PostPressButton(UINT CommandId,BOOL bPressButton);
	int PointToCommandId(POINT *pPoint);
	void SetButtonImage(UINT CommandId,UINT ImageId);

	inline SIZE GetSize() { return BarSize; }
	virtual LPARAM CalculatePadding();
	void CalculateBarSize(); // Workaround for Windows 2000 and below
	
protected:
	CQToolbar* m_pToolbar;
	SIZE BarSize; // Current dimensions of the toolbar including all visible buttons

	int* pButtonSizeX; // Pointer to g_Scaled_ButtonSize
	int* pButtonSizeY; // Pointer to g_Scaled_ButtonSize
	int ButtonMargin; // Used for CalculateBarSize
	bool bSetTooltipNoPrefixStyle;
	UINT VisibleButtons; // Bit array of visible buttons
	UINT ButtonMask; // Bit array of buttons defined in ButtonIds
	UINT ButtonCount;
	UINT CommandIds[MAX_BUTTONS];
	UINT ButtonIds[MAX_BUTTONS];
	BYTE ButtonStyles[MAX_BUTTONS];
};

#endif // !defined(__Toolbar_H_)