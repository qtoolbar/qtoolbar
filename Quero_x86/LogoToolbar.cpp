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

// LogoToolbar.cpp : Implementation of CLogoToolbar

#include "stdafx.h"
#include "LogoToolbar.h"
#include "QToolbar.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CLogoToolbar

bool CLogoToolbar::m_bWhiteLogo=false;
CToolbarIcons CLogoToolbar::m_ToolbarIcons;

CLogoToolbar::CLogoToolbar()
{
	pButtonSizeX=&g_Scaled_QueroLogoX;
	pButtonSizeY=&g_Scaled_QueroLogoY;
	ButtonMargin=17;

	ButtonCount=1;
	CommandIds[0]=IDM_LOGO;
	ButtonIds[0]=BUTTON_QUERO;
	ButtonStyles[0]=(BTNS_AUTOSIZE|BTNS_DROPDOWN);
	ButtonMask=BUTTON_QUERO;
	
	bSetTooltipNoPrefixStyle=false;
}

void CLogoToolbar::LoadToolbarIcons()
{
	WORD Icon;

	m_bWhiteLogo=(g_Options&OPTION_WhiteQueroButton);
	Icon=m_bWhiteLogo?IDI_QUERO_WHITE:IDI_QUERO;

	m_ToolbarIcons.Create(g_Scaled_QueroLogoX,g_Scaled_QueroLogoY,(ILC_COLOR32|ILC_MASK),1,CREATE_TB_IMAGELIST);
	m_ToolbarIcons.LoadIcons(TB_IMAGELIST,&Icon,1);
}

void CLogoToolbar::SetImageLists()
{
	m_ToolbarIcons.SetImageList(m_hWnd,TB_SETIMAGELIST,TB_IMAGELIST);
}

void CLogoToolbar::UpdatePosition()
{
	int toolbarheight;
	int space;
	int posy;

	toolbarheight=m_pToolbar->GetToolbarHeight();
	if(toolbarheight)
	{
		space=toolbarheight-BarSize.cy;
		posy=space/2; // Default: 4 pixel offset

		if(IsBelowWindows8() && g_IE_MajorVersion>=9 && (g_Options2&OPTION2_EnableAeroTheme) && posy>0) posy--;

		::MoveWindow(m_hWnd,m_pToolbar->GetNavBar()->GetSize().cx,posy,BarSize.cx,BarSize.cy,FALSE);
	}
}

void CLogoToolbar::UpdateLogoImage()
{
	bool bSetWhiteLogo;
	WORD Icon;

	bSetWhiteLogo=(g_Options&OPTION_WhiteQueroButton);
	if(bSetWhiteLogo != m_bWhiteLogo)
	{
		m_bWhiteLogo=bSetWhiteLogo;
		Icon=m_bWhiteLogo?IDI_QUERO_WHITE:IDI_QUERO;
		m_ToolbarIcons.RemoveAll();
		m_ToolbarIcons.LoadIcons(TB_IMAGELIST,&Icon,1);
	}
}
