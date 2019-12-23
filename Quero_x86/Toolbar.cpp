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

// Toolbar.cpp: implementation of the CToolbar class.

#include "stdafx.h"
#include "Toolbar.h"
#include "QToolbar.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CToolbar

CToolbar::CToolbar()
{
	ButtonCount=0;
	pButtonSizeX=NULL;
	pButtonSizeY=NULL;
}

CToolbar::~CToolbar()
{
	if(IsWindow()) DestroyWindow();
}

LRESULT CToolbar::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT i;
	HWND hWndTooltips;

	// Buttons with images and text
	SendMessage(m_hWnd, TB_SETEXTENDEDSTYLE, 0, (LPARAM)TBSTYLE_EX_MIXEDBUTTONS);
	// Sets the size of the TBBUTTON structure.
	SendMessage(m_hWnd, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
	// Set the maximum number of text rows and bitmap size.
	SendMessage(m_hWnd, TB_SETMAXTEXTROWS, 1, 0L);
	// Set Padding
	SendMessage(m_hWnd, TB_SETPADDING, 0, CalculatePadding());

	// Set the toolbar's image list
	SetImageLists();

	// Add the button for the toolbar to the window
	TBBUTTON Buttons[MAX_BUTTONS];
	
	ZeroMemory((void*)&Buttons, sizeof(Buttons));

	for(i=0;i<ButtonCount;i++)
	{
		Buttons[i].idCommand = CommandIds[i];
		Buttons[i].fsState = (VisibleButtons&ButtonIds[i])?TBSTATE_ENABLED:TBSTATE_ENABLED|TBSTATE_HIDDEN;
		Buttons[i].fsStyle = ButtonStyles[i];
		Buttons[i].iBitmap = i;
	}
	
	::SendMessage(m_hWnd, TB_ADDBUTTONS, ButtonCount, (LPARAM)&Buttons);

	// Retrieve toolbar size
	::SendMessage(m_hWnd, TB_GETMAXSIZE, 0, (LPARAM)&BarSize);
	if(IsBelowWindowsXP()) CalculateBarSize();

	// Position the toolbar
	UpdatePosition();

	// Add TTS_NOPREFIX to the tooltips style
	if(bSetTooltipNoPrefixStyle)
	{
		hWndTooltips=(HWND)SendMessage(m_hWnd, TB_GETTOOLTIPS , 0, 0);
		if(hWndTooltips) ::SetWindowLong(hWndTooltips,GWL_STYLE,::GetWindowLong(hWndTooltips,GWL_STYLE)|TTS_NOPREFIX);
	}

	return 0;
}

LRESULT CToolbar::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RECT tbRect;

	::SendMessage(m_hWnd, TB_SETROWS, MAKEWPARAM(1, FALSE), (LPARAM)&tbRect);
	::SendMessage(m_hWnd, TB_AUTOSIZE, 0,0);

	return 0;
}


LPARAM CToolbar::CalculatePadding()
{
	int padding;

	if(pButtonSizeY)
	{
		padding=m_pToolbar->GetToolbarHeight()-*pButtonSizeY;
	}
	else padding=0;
	
	if(padding<0) padding=0;
	else if(padding>=8) padding-=4;

	return MAKELPARAM(0,padding);
}

void CToolbar::OnHeightChange(int height)
{
	LPARAM lParamButtonSize;

	SendMessage(m_hWnd, TB_SETPADDING, 0, CalculatePadding());
	
	if(pButtonSizeX && pButtonSizeY)
	{
		lParamButtonSize=MAKELPARAM(*pButtonSizeX,*pButtonSizeY);
		SendMessage(m_hWnd,  TB_SETBUTTONSIZE, 0, lParamButtonSize);
	}
	
	::SendMessage(m_hWnd, TB_GETMAXSIZE, 0, (LPARAM)&BarSize);
	if(IsBelowWindowsXP()) CalculateBarSize();

	UpdatePosition();
}

int CToolbar::GetButtonIndex(WORD ButtonId)
{
	UINT i;

	i=0;
	while(i<ButtonCount && ButtonIds[i]!=ButtonId) i++;
	if(i==ButtonCount) i=-1;

	return i;
}

void CToolbar::EnableButton(UINT CommandId,BOOL bEnable)
{
	::SendMessage(m_hWnd,TB_ENABLEBUTTON,CommandId,MAKELONG(bEnable,0));
	if(IsWindowsVistaOrLater()) RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_NOERASE|RDW_UPDATENOW);
}

void CToolbar::ShowButtons(UINT Buttons)
{
	UINT i;

	VisibleButtons=Buttons;

	for(i=0;i<ButtonCount;i++) ::SendMessage(m_hWnd, TB_HIDEBUTTON, CommandIds[i], MAKELONG((VisibleButtons&ButtonIds[i])==0,0));

	// Adjust toolbar size
	::SendMessage(m_hWnd, TB_GETMAXSIZE, 0, (LPARAM)&BarSize);
	if(IsBelowWindowsXP()) CalculateBarSize();
	::MoveWindow(m_hWnd,0,4,BarSize.cx,BarSize.cy,FALSE);
}

bool CToolbar::IsButtonPressed(UINT CommandId)
{
	return (SendMessage(m_hWnd,TB_GETSTATE,CommandId,0)&TBSTATE_PRESSED)!=0;
}

void CToolbar::PressButton(UINT CommandId,BOOL bPressButton)
{
	SendMessage(m_hWnd,TB_PRESSBUTTON,CommandId,MAKELONG(bPressButton,0));
}

void CToolbar::PostPressButton(UINT CommandId,BOOL bPressButton)
{
	::PostMessage(m_hWnd,TB_PRESSBUTTON,CommandId,MAKELONG(bPressButton,0));
}

int CToolbar::PointToCommandId(POINT *pPoint)
{
	int iButtonHit;
	
	iButtonHit=(int)SendMessage(m_hWnd,TB_HITTEST,0,(LPARAM)pPoint);
	return (iButtonHit>=0 && (UINT)iButtonHit<ButtonCount)?CommandIds[iButtonHit]:-1;
}

void CToolbar::SetButtonImage(UINT CommandId,UINT ImageId)
{
	TBBUTTONINFO bi;

	bi.cbSize=sizeof bi;
	bi.dwMask=TBIF_IMAGE;
	bi.iImage=ImageId;
	SendMessage(m_hWnd,TB_SETBUTTONINFO,CommandId,(LPARAM)&bi);
}

// Workaround for Windows 2000 and below
void CToolbar::CalculateBarSize()
{
	UINT n;
	UINT i;

	n=0;
	i=0;
	while(i<ButtonCount)
	{
		if(VisibleButtons&ButtonIds[i]) n++;
		i++;
	}
	
	if(pButtonSizeX && pButtonSizeY)
	{
		BarSize.cx=n*(*pButtonSizeX+ButtonMargin);
		BarSize.cy=*pButtonSizeY+HIWORD(CalculatePadding());
	}
}