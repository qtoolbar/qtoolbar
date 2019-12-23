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

#include "stdafx.h"
#include "AeroReBar.h"
#include "QToolbar.h"
#include <dwmapi.h>

#include "..\QueroBroker\QueroBroker.h"


CAeroReBar::CAeroReBar(CQToolbar *pToolbar)
{
	m_pToolbar=pToolbar;
	bIsQueroBackgroundTransparent=false;
	bTheaterMode=false;
	DwmFrameTopMargin=0;
}

/*
CAeroReBar::~CAeroReBar()
{
}
*/

LRESULT CAeroReBar::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bool bCurrentTheaterMode;
	HWND hwnd_IEFrame;

	hwnd_IEFrame=m_pToolbar->GetIEFrameWindow();

	// Update top margin
	if(m_pToolbar->pQueroBroker) DwmFrameTopMargin=m_pToolbar->pQueroBroker->GetDwmFrameTopMargin(HandleToLong(hwnd_IEFrame));

	// Query theater mode
	bCurrentTheaterMode=m_pToolbar->IsTheaterMode();

	// If exiting theater mode, set the dwm top margin
	if(bCurrentTheaterMode!=bTheaterMode)
	{
		if(bTheaterMode==true)
		{
			m_pToolbar->PostMessage(WM_QUERO_TOOLBAR_COMMAND,QUERO_COMMAND_UPDATE_DWMTOPMARGIN_DELAYED,(LPARAM)hwnd_IEFrame);
		}
		bTheaterMode=bCurrentTheaterMode;
	}

	bHandled=FALSE;
	return 0;
}

LRESULT CAeroReBar::OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT result=0;
	HWND hwnd_QToolbar;
	HWND hwnd_IEFrame;
	HWND hwnd_Root;
	RECT rect_ReBar;
	RECT rect_QToolbar;
	RECT rect_Band;
	HBRUSH hBrush;
	bool bDrawTransparentBackground;
	int NewDwmFrameTopMargin;
	int index;
	int BandCount;
	HTHEME hTheme;

	// Get IE Frame HWND
	hwnd_IEFrame=m_pToolbar->GetIEFrameWindow();

	// IE9: Check if tab was dragged to a new window
	if(g_IE_MajorVersion>=9)
	{
		hwnd_Root=GetAncestor(m_hWnd,GA_ROOT);
		if(hwnd_IEFrame!=hwnd_Root)
		{
			hwnd_IEFrame=hwnd_Root;
			m_pToolbar->PostMessage(WM_QUERO_TOOLBAR_COMMAND,QUERO_COMMAND_IEFRAME_CHANGED,0);
		}
	}

	if(g_Options2&OPTION2_EnableAeroTheme)
	{
		if(IsCompositionActive())
		{
			NewDwmFrameTopMargin=0;

			// Draw default toolbar background
			DefWindowProc();

			// Get ReBar position
			GetClientRect(&rect_ReBar);

			// Find Quero Toolbar
			hwnd_QToolbar=m_pToolbar->m_hWnd;
			if(hwnd_QToolbar && ::IsWindowVisible(hwnd_QToolbar))
			{
				::GetClientRect(hwnd_QToolbar,&rect_QToolbar);
				::MapWindowPoints(hwnd_QToolbar,m_hWnd,(LPPOINT)&rect_QToolbar,2);

				// Include toolbar gripper and toolbar close button
				if(rect_QToolbar.left<=50) rect_QToolbar.left=0;

				// Draw transparent background under Quero Toolbar if toolbar is on top and no other toolbar is on the same line
				if(rect_QToolbar.left==rect_ReBar.left && rect_QToolbar.top==rect_ReBar.top)
				{
					// Test if Quero Toolbar spawns the whole line
					// (necessary, because on resize events rect_ReBar is already updated whereas rect_QToolbar is not)
					if(rect_QToolbar.right!=rect_ReBar.right)
					{
						index=0;
						BandCount=(int)SendMessage(RB_GETBANDCOUNT,0,0);
						while(index<BandCount)
						{
							SendMessage(RB_GETRECT,(WPARAM)index,(LPARAM)&rect_Band);
							if(rect_Band.right>rect_QToolbar.right) break;
							index++;
						}
						bDrawTransparentBackground=(index==BandCount);
					}
					else bDrawTransparentBackground=true;

					if(bDrawTransparentBackground)
					{
						hBrush=(HBRUSH)GetStockObject(BLACK_BRUSH);
						if(hBrush) FillRect((HDC)wParam,&rect_QToolbar,hBrush);

						// Calculate the top frame margin
						::MapWindowPoints(hwnd_QToolbar,hwnd_IEFrame,(LPPOINT)&rect_QToolbar,2);
						NewDwmFrameTopMargin=rect_QToolbar.bottom;
					}
				}
			}

			// Do not extend frame if theater mode is active
			if(bTheaterMode) NewDwmFrameTopMargin=0;
			// If top margin was not set, extend frame above ReBar
			else if(NewDwmFrameTopMargin==0)
			{
				MapWindowPoints(hwnd_IEFrame,(LPPOINT)&rect_ReBar,2);
				NewDwmFrameTopMargin=rect_ReBar.top;
				bDrawTransparentBackground=false;
			}

			// Extend frame into client area
			ExtendFrameIntoClientArea(hwnd_IEFrame,NewDwmFrameTopMargin);

			result=1;
		} // End DwmCompositionEnabled
		else if(IsThemeActive())
		{
			// Get ReBar position
			GetClientRect(&rect_ReBar);

			// DrawThemeBackground
			hTheme=OpenThemeData(m_hWnd,L"WINDOW");
			if(hTheme)
			{
				InflateRect(&rect_ReBar,8,8);
				DrawThemeBackground(hTheme,(HDC)wParam,WP_FRAMEBOTTOM,GetForegroundWindow()==hwnd_IEFrame?FS_ACTIVE:FS_INACTIVE,&rect_ReBar,NULL);
				CloseThemeData(hTheme);
				result=1;
			}

			bDrawTransparentBackground=false;
		}
	} // End OPTION2_EnableAeroTheme
	else
	{
		bDrawTransparentBackground=false;
		
		// Workaround in IE11: Reset DwmFrameTopMargin if OPTION2_HideAddressBox is enabled and OPTION2_EnableAeroTheme is disabled
		if(g_IE_MajorVersion>=11 && (g_Options2&OPTION2_HideAddressBox) && IsCompositionActive() && DwmFrameTopMargin==0)
		{
			MARGINS margins;

			// Get ReBar position
			GetClientRect(&rect_ReBar);
			MapWindowPoints(hwnd_IEFrame,(LPPOINT)&rect_ReBar,2);

			// Extend frame above ReBar
			margins.cxLeftWidth=0;
			margins.cxRightWidth=0;
			margins.cyBottomHeight=0;
			margins.cyTopHeight=rect_ReBar.top;
			DwmExtendFrameIntoClientArea(hwnd_IEFrame,&margins);

			// Update top margin
			DwmFrameTopMargin=margins.cyTopHeight;
		}
	}

	if(bDrawTransparentBackground!=bIsQueroBackgroundTransparent)
	{
		bIsQueroBackgroundTransparent=bDrawTransparentBackground;
	}

	if(result==0) bHandled=FALSE;

	return result;
}

void CAeroReBar::ExtendFrameIntoClientArea(HWND hwnd_IEFrame,int NewDwmFrameTopMargin)
{
	MARGINS margins;
	int OldDwmFrameTopMargin;

	QD(L"ExtendFrameIntoClientArea");

	margins.cxLeftWidth=0;
	margins.cxRightWidth=0;
	margins.cyBottomHeight=0;
	margins.cyTopHeight=NewDwmFrameTopMargin;

	if(DwmFrameTopMargin!=NewDwmFrameTopMargin && m_pToolbar->pQueroBroker)
	{
		// Update top margin
		m_pToolbar->pQueroBroker->SetDwmFrameTopMargin(HandleToLong(hwnd_IEFrame),NewDwmFrameTopMargin);
		OldDwmFrameTopMargin=DwmFrameTopMargin;
		DwmFrameTopMargin=NewDwmFrameTopMargin;

		// Update Toolbar Background Bitmap
		InterlockedIncrement(&g_ToolbarBackgroundState);

		// Make background opaque if the toolbar looses Aero Glass
		if(NewDwmFrameTopMargin<OldDwmFrameTopMargin)
			::RedrawWindow(m_hWnd,NULL,NULL,RDW_INVALIDATE|RDW_NOERASE|RDW_UPDATENOW|RDW_ALLCHILDREN);

		// Extend frame into client area
		DwmExtendFrameIntoClientArea(hwnd_IEFrame,&margins);
		//QDEBUG_PRINTF(L"FrameTopHeight",L"%d",margins.cyTopHeight);

		// Make background transparent if the toolbar gains Aero Glass
		if(NewDwmFrameTopMargin>OldDwmFrameTopMargin)
			::RedrawWindow(m_hWnd,NULL,NULL,RDW_INVALIDATE|RDW_NOERASE|RDW_UPDATENOW|RDW_ALLCHILDREN);
	}
	else
	{
		DwmExtendFrameIntoClientArea(hwnd_IEFrame,&margins);
	}
}

void CAeroReBar::OnEnableAeroThemeChanged()
{
	HWND hwnd_IEFrame;
	RECT rect_ReBar;

	// Update Toolbar Background Bitmap
	InterlockedIncrement(&g_ToolbarBackgroundState);
	::RedrawWindow(GetParent(),NULL,NULL,RDW_INVALIDATE|RDW_NOERASE|RDW_ALLCHILDREN);

	if((g_Options2&OPTION2_EnableAeroTheme)==0 && IsCompositionActive())
	{
		// Get IE Frame HWND
		hwnd_IEFrame=m_pToolbar->GetIEFrameWindow();

		// Get ReBar position
		GetClientRect(&rect_ReBar);

		// Extend frame above ReBar
		MapWindowPoints(hwnd_IEFrame,(LPPOINT)&rect_ReBar,2);
		ExtendFrameIntoClientArea(hwnd_IEFrame,rect_ReBar.top);
	}
}
