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
// FavIcon.h

#ifndef __FAVICON_H
#define __FAVICON_H

#include "Downloader.h"
#include "IDNA.h"

class CQToolbar;

class CFavIcon : public CDownloader //: public IBindStatusCallback
{
public:
	CFavIcon(CQToolbar *pToolbar);

	//	IUnknown
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject);

	// IBindStatusCallback
	STDMETHODIMP OnProgress(ULONG ulProgress,ULONG ulProgressMax,ULONG ulStatusCode,LPCWSTR szStatusText);
	STDMETHODIMP GetBindInfo(DWORD *grfBINDF,BINDINFO *pbindinfo);
	STDMETHODIMP GetPriority(LONG *pnPriority);
	STDMETHODIMP OnDataAvailable(DWORD grfBSCF,DWORD dwSize,FORMATETC *pformatetc,STGMEDIUM *pstgmed);
	STDMETHODIMP OnObjectAvailable(REFIID riid,IUnknown *punk);
	STDMETHODIMP OnStartBinding(DWORD dwReserved,IBinding *pib);
	STDMETHODIMP OnStopBinding(HRESULT hresult,LPCWSTR szError);
	STDMETHODIMP OnLowResource(DWORD dwReserved);

	// CFavIcon
	static DWORD WINAPI FavIconDownloadThread(LPVOID lpParameter);

	void Download(TCHAR *pURL);
	inline TCHAR* GetFavIconURL() { return FavIconURL; }

	static HICON LoadFavIconFromCache(TCHAR *pURL,bool bAppendFaviconIco);
	static bool AppendFaviconIco(TCHAR *pURL,TCHAR FavIconURL[MAXURLLENGTH]);

private:
	CQToolbar* m_pToolbar;

	volatile ULONG m_nRefCount;

	TCHAR FavIconURL[MAXURLLENGTH];
	UINT LTime_FavIconURL;
};

#endif