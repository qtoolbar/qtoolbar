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
// ToolbarIcons.h: interface for the CToolbarIcons class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLBARICONS_H__398EDF17_3268_4D47_B04D_B8CBD8DA2390__INCLUDED_)
#define AFX_TOOLBARICONS_H__398EDF17_3268_4D47_B04D_B8CBD8DA2390__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define CREATE_TB_IMAGELIST 0x01
#define CREATE_TB_HOTIMAGELIST 0x02
#define CREATE_TB_PRESSEDIMAGELIST 0x04
#define CREATE_TB_DISABLEDIMAGELIST 0x08

#define TB_IMAGELIST 0
#define TB_HOTIMAGELIST 1
#define TB_PRESSEDIMAGELIST 2
#define TB_DISABLEDIMAGELIST 3

#define N_IMAGELISTS 4

#define TB_SETPRESSEDIMAGELIST (WM_USER + 104)

class CToolbarIcons  
{
public:
	CToolbarIcons();
	~CToolbarIcons();

	void Create(int cx,int cy,UINT flags,int n,BYTE TBImageLists);
	void LoadIcons(BYTE TBImageList,const WORD *IconResIds,int n);
	void SetImageList(HWND hwnd,int TBSetImageList,BYTE TBImageList);
	void RemoveAll();
	void Destroy();

	static HICON LoadThemeIcon(WORD IconResId,int cx, int cy);
	
private:
	HIMAGELIST m_hImageList[N_IMAGELISTS];
	int m_cx;
	int m_cy;
};

#endif // !defined(AFX_TOOLBARICONS_H__398EDF17_3268_4D47_B04D_B8CBD8DA2390__INCLUDED_)
