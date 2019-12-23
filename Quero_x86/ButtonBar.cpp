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

// ButtonBar.cpp : Implementation of CButtonBar

#include "stdafx.h"
#include "ButtonBar.h"
#include "QToolbar.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CButtonBar

CToolbarIcons CButtonBar::m_ToolbarIcons;

CButtonBar::CButtonBar()
{
	pButtonSizeX=&g_Scaled_ButtonSize;
	pButtonSizeY=&g_Scaled_ButtonSize;
	ButtonMargin=8;

	ButtonCount=1;
	CommandIds[0]=IDM_QUERO;
	ButtonIds[0]=BUTTON_GO;
	ButtonStyles[0]=(BTNS_BUTTON | BTNS_AUTOSIZE);
	ButtonMask=BUTTON_GO;
	
	bSetTooltipNoPrefixStyle=true;
}

void CButtonBar::LoadToolbarIcons()
{
	WORD Icon=IDI_BUTTON_GO,IconHot=IDI_BUTTON_GO_HOT;

	m_ToolbarIcons.Create(g_Scaled_ButtonSize,g_Scaled_ButtonSize,ILC_COLOR32|ILC_MASK,1,CREATE_TB_IMAGELIST|CREATE_TB_HOTIMAGELIST);
	m_ToolbarIcons.LoadIcons(TB_IMAGELIST,&Icon,1);
	m_ToolbarIcons.LoadIcons(TB_HOTIMAGELIST,&IconHot,1);
}

void CButtonBar::SetImageLists()
{
	m_ToolbarIcons.SetImageList(m_hWnd,TB_SETIMAGELIST,TB_IMAGELIST);
	m_ToolbarIcons.SetImageList(m_hWnd,TB_SETHOTIMAGELIST,TB_HOTIMAGELIST);
	if(IsWindowsVistaOrLater()) m_ToolbarIcons.SetImageList(m_hWnd,TB_SETPRESSEDIMAGELIST,TB_HOTIMAGELIST);
}

void CButtonBar::UpdatePosition()
{
	// Position is updated in CQToolbar::OnSize
}

LRESULT CButtonBar::OnMiddleClickUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT p;
	int CommandId;

	p.x=GET_X_LPARAM(lParam); 
	p.y= GET_Y_LPARAM(lParam);

	CommandId=PointToCommandId(&p);

	// First button pressed?
	if(CommandId==IDM_QUERO)
	{
		m_pToolbar->PostMessage(WM_QUERO,0,SHORTCUT_OPTION_MIDDLECLICK);
	}
		
	return 0;
}