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
#include "MenuIcon.h"

#include <wincodec.h>

CMenuIcon::CMenuIcon(void)
{
	m_hBitmap_Unchecked=NULL;
	m_hBitmap_Checked=NULL;
}

CMenuIcon::~CMenuIcon(void)
{
	if(m_hBitmap_Unchecked) DeleteObject(m_hBitmap_Unchecked);
	if(m_hBitmap_Checked) DeleteObject(m_hBitmap_Checked);
}

HRESULT CMenuIcon::AddIconToMenuItem(HICON hIcon_Unchecked, HICON hIcon_Checked, HMENU hMenu, UINT uItem, UINT uFlags)
{
	HRESULT hr;

	// Convert icons to bitmap
	if(hIcon_Unchecked) m_hBitmap_Unchecked=ConvertIconToBitmap(hIcon_Unchecked);
	if(hIcon_Checked) m_hBitmap_Checked=ConvertIconToBitmap(hIcon_Unchecked);

	if(m_hBitmap_Unchecked)
	{
		SetMenuItemBitmaps(hMenu,uItem,uFlags,m_hBitmap_Unchecked,m_hBitmap_Checked);
		hr=S_OK;
	}
	else hr=E_FAIL;

	return hr;
}

HBITMAP CMenuIcon::ConvertIconToBitmap(HICON hIcon)
{
	HBITMAP hBitmap;
	HRESULT hr;
	IWICImagingFactory *pFactory;
	IWICBitmap *pBitmap;
	UINT cx, cy;
	BYTE *pbBuffer;

	hBitmap=NULL;

	hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFactory));
	if(SUCCEEDED(hr))
	{
		hr = pFactory->CreateBitmapFromHICON(hIcon, &pBitmap);
		if(SUCCEEDED(hr))
		{
			hr = pBitmap->GetSize(&cx, &cy);
			if(SUCCEEDED(hr))
			{
				hBitmap = Create32BitHBITMAP(NULL, (LONG)cx, -(LONG)cy, reinterpret_cast<void **>(&pbBuffer));
				if(hBitmap)
				{
					const UINT cbStride = cx * sizeof(ARGB);
					const UINT cbBuffer = cy * cbStride;
					hr = pBitmap->CopyPixels(NULL, cbStride, cbBuffer, pbBuffer);
					if(FAILED(hr))
					{
						DeleteObject(hBitmap);
						hBitmap=NULL;
					}						
				}
			}
			pBitmap->Release();
		}
		pFactory->Release();
	}

	return hBitmap;
}

void CMenuIcon::InitBitmapInfo(__out_bcount(cbInfo) BITMAPINFO *pbmi, ULONG cbInfo, LONG cx, LONG cy, WORD bpp)
{
    ZeroMemory(pbmi, cbInfo);
    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biPlanes = 1;
    pbmi->bmiHeader.biCompression = BI_RGB;

    pbmi->bmiHeader.biWidth = cx;
    pbmi->bmiHeader.biHeight = cy;
    pbmi->bmiHeader.biBitCount = bpp;
}

HBITMAP CMenuIcon::Create32BitHBITMAP(HDC hdc, LONG cx, LONG cy, __deref_opt_out void **ppvBits)
{
	HBITMAP hBitmap;
	BITMAPINFO bmi;
	HDC hdcUsed;

    hBitmap = NULL;

    hdcUsed = hdc ? hdc : GetDC(NULL);
    if(hdcUsed)
    {
		InitBitmapInfo(&bmi, sizeof(bmi), cx, cy, 32);
        hBitmap = CreateDIBSection(hdcUsed, &bmi, DIB_RGB_COLORS, ppvBits, NULL, 0);
        
		if(hdc==NULL) ReleaseDC(NULL, hdcUsed);
    }

	return hBitmap;
}
