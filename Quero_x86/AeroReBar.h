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
// CAeroReBar.h: interface for the CAeroReBar class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CQToolbar;

class CAeroReBar : public CWindowImpl<CAeroReBar>
{
public:
	CAeroReBar(CQToolbar *pToolbar);
	//~CAeroReBar();

	BEGIN_MSG_MAP(CAeroReBar)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
	END_MSG_MAP()

	// Message handlers
	LRESULT OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	// Desktop Manager
	void ExtendFrameIntoClientArea(HWND hwnd_IEFrame,int NewDwmFrameTopMargin);
	void OnEnableAeroThemeChanged();

	// Current state of toolbar background
	bool bIsQueroBackgroundTransparent;

	// Current top margin
	int DwmFrameTopMargin;

	// Theater mode
	bool bTheaterMode;

	// Reference to QToolbar
	CQToolbar *m_pToolbar;
};
