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

#pragma once

typedef DWORD ARGB;

class CMenuIcon
{
public:
	CMenuIcon(void);
	~CMenuIcon(void);

	HRESULT AddIconToMenuItem(HICON hIcon_Unchecked,HICON hIcon_Checked, HMENU hMenu, UINT uItem, UINT uFlags);

	HBITMAP ConvertIconToBitmap(HICON hIcon);
	void InitBitmapInfo(__out_bcount(cbInfo) BITMAPINFO *pbmi, ULONG cbInfo, LONG cx, LONG cy, WORD bpp);
	HBITMAP Create32BitHBITMAP(HDC hdc, LONG cx, LONG cy, __deref_opt_out void **ppvBits);

	HBITMAP m_hBitmap_Unchecked;
	HBITMAP m_hBitmap_Checked;
};
