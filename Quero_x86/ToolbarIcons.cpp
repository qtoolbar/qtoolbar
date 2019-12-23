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
// ToolbarIcons.cpp: implementation of the CToolbarIcons class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToolbarIcons.h"
#include "QToolbar.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolbarIcons::CToolbarIcons()
{
	int i;

	for(i=0;i<N_IMAGELISTS;i++) m_hImageList[i]=NULL;
}

CToolbarIcons::~CToolbarIcons()
{
	Destroy();
}

void CToolbarIcons::Destroy()
{
	int i;

	for(i=0;i<N_IMAGELISTS;i++)
	{
		if(m_hImageList[i])
		{
			ImageList_Destroy(m_hImageList[i]);
			m_hImageList[i]=NULL;
		}
	}
}

void CToolbarIcons::Create(int cx,int cy,UINT flags,int n,BYTE TBImageLists)
{
	BYTE i;

	i=0;
	while(i<N_IMAGELISTS)
	{
		if(TBImageLists&(1<<i)) m_hImageList[i]=ImageList_Create(cx,cy,flags,n,0);
		i++;
	}
	m_cx=cx;
	m_cy=cy;
}

void CToolbarIcons::LoadIcons(BYTE TBImageList,const WORD *IconResIds,int n)
{
	HICON hIcon;
	int i;

	if(m_hImageList[TBImageList])
	{
		i=0;
		while(i<n)
		{
			hIcon=LoadThemeIcon(IconResIds[i],m_cx,m_cy);
			if(hIcon)
			{
				ImageList_AddIcon(m_hImageList[TBImageList],hIcon);
				DestroyIcon(hIcon);
			}

			i++;
		}
	}
}

void CToolbarIcons::SetImageList(HWND hwnd,int TBSetImageList,BYTE TBImageList)
{
	if(m_hImageList[TBImageList]) SendMessage(hwnd,TBSetImageList,0,(LPARAM)m_hImageList[TBImageList]);
}

HICON CToolbarIcons::LoadThemeIcon(WORD IconResId,int cx, int cy)
{
	HICON hIcon;
	int j;

	j=g_QueroTheme_DLL?0:1;
	do
	{
		hIcon=(HICON)LoadImage(j?_Module.GetResourceInstance():g_QueroTheme_DLL,MAKEINTRESOURCE(IconResId),IMAGE_ICON,cx,cy,LR_DEFAULTCOLOR);
		j++;
	} while(j<2 && hIcon==NULL);

	return hIcon;
}

void CToolbarIcons::RemoveAll()
{
	int i;

	for(i=0;i<N_IMAGELISTS;i++) if(m_hImageList[i]) ImageList_RemoveAll(m_hImageList[i]);
}