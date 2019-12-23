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
// FavIcon.cpp: implementation of the CFavIcon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FavIcon.h"
#include "QToolbar.h"
#include "resource.h"

#include <wininet.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFavIcon::CFavIcon(CQToolbar *pToolbar)
{
	m_nRefCount=1;

	m_pToolbar=pToolbar;
	FavIconURL[0]=0;
	LTime_FavIconURL=0;
}


STDMETHODIMP_(ULONG) CFavIcon::AddRef()
{
	return m_nRefCount;
}

STDMETHODIMP_(ULONG) CFavIcon::Release()
{
	return m_nRefCount;
}

STDMETHODIMP CFavIcon::QueryInterface(REFIID riid, void** ppvObject)
{
	HRESULT hr;
	
	if(ppvObject)
	{
		if(riid==IID_IUnknown || riid==IID_IBindStatusCallback)
		{
			*ppvObject=(LPVOID*)this;
			AddRef();
			hr = S_OK;
		}
		else
		{
			*ppvObject=NULL;
			hr = E_NOINTERFACE;
		}
	}
	else
	{
		hr = E_POINTER;
	}
	
	return hr;
}

STDMETHODIMP CFavIcon::OnProgress(ULONG ulProgress,ULONG ulProgressMax,ULONG ulStatusCode,LPCWSTR szStatusText)
{
	QDEBUG_CODE if(ulProgressMax>=0x10000) QDEBUG_PRINTF(L"CFavIcon::OnProgress",L"%d/%d/%d/%s",ulProgress,ulProgressMax,ulStatusCode,szStatusText);

	QDEBUG_PRINTF(L"CFavIcon::OnProgress",L"%d/%d/%d/%s",ulProgress,ulProgressMax,ulStatusCode,szStatusText);

	// Abort download if icon file size is larger than 64kb or when bAbortThreads signalled
	return (ulProgressMax<0x10000 && !bAbortThreads)?S_OK:E_ABORT;
}

STDMETHODIMP CFavIcon::GetBindInfo(DWORD *grfBINDF,BINDINFO *pbindinfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CFavIcon::GetPriority(LONG *pnPriority)
{
	return E_NOTIMPL;
}

STDMETHODIMP CFavIcon::OnDataAvailable(DWORD grfBSCF,DWORD dwSize,FORMATETC *pformatetc,STGMEDIUM *pstgmed)
{
	return S_OK;
}

STDMETHODIMP CFavIcon::OnObjectAvailable(REFIID riid,IUnknown *punk)
{
	return S_OK;
}

STDMETHODIMP CFavIcon::OnStartBinding(DWORD dwReserved,IBinding *pib)
{
	return S_OK;
}

STDMETHODIMP CFavIcon::OnStopBinding(HRESULT hresult,LPCWSTR szError)
{
	return S_OK;
}

STDMETHODIMP CFavIcon::OnLowResource(DWORD dwReserved)
{
	return E_NOTIMPL;
}


DWORD WINAPI CFavIcon::FavIconDownloadThread(LPVOID lpParameter)
{
	CFavIcon *pFavIcon=(CFavIcon*)lpParameter;
	HANDLE hMutex;
	HWND hWnd;
	UINT LTime_FavIconURL;
	HICON hIcon;
	HRESULT hr;
	
	TCHAR DownloadFavIconURL[MAXURLLENGTH];
	TCHAR FavIconFileName[MAX_PATH];

	// Save mutex and hwnd handle in the case that pFavIcon object terminates while this thread is still running

	hMutex=pFavIcon->hDownloadMutex;
	hWnd=pFavIcon->m_pToolbar->m_hWnd;

	if(WaitForSingleObject(hMutex,DOWNLOAD_MUTEX_TIMEOUT)==WAIT_OBJECT_0)
	{
		StringCbCopy(DownloadFavIconURL,sizeof DownloadFavIconURL,pFavIcon->FavIconURL);
		LTime_FavIconURL=pFavIcon->LTime_FavIconURL;
		
		ReleaseMutex(hMutex);

		// Download FavIcon
	
		//hr=URLDownloadToCacheFile(NULL,DownloadFavIconURL,FavIconFileName,MAX_PATH,0,pFavIcon); with callback
		hr=URLDownloadToCacheFile(NULL,DownloadFavIconURL,FavIconFileName,MAX_PATH,0,NULL);
		if(SUCCEEDED_OK(hr))
		{
			if(WaitForSingleObject(hMutex,DOWNLOAD_MUTEX_TIMEOUT)==WAIT_OBJECT_0)
			{
				if(IsWindow(hWnd))
				{
					if(LTime_FavIconURL==pFavIcon->LTime_FavIconURL && pFavIcon->bAbortThreads==false)
					{
						hIcon=(HICON)LoadImage(_Module.m_hInst,FavIconFileName,IMAGE_ICON,g_Scaled_IconSize,g_Scaled_IconSize,LR_DEFAULTCOLOR|LR_LOADFROMFILE);
						if(hIcon)
						{
							::PostMessage(hWnd,WM_QUERO_SETFAVICON,0,(LPARAM)hIcon);
						}
					}
				}

				ReleaseMutex(hMutex);
			}
			QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"FavIconDownloadThread 2");
		}

	}
	QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"FavIconDownloadThread 1");

	ExitThread(0);
}

void CFavIcon::Download(TCHAR *pURL)
{
	if(WaitForSingleObject(hDownloadMutex,DOWNLOAD_MUTEX_TIMEOUT)==WAIT_OBJECT_0)
	{
		StringCbCopy(FavIconURL,sizeof FavIconURL,pURL);
		LTime_FavIconURL++;

		CreateDownloadThread(FavIconDownloadThread);
	
		ReleaseMutex(hDownloadMutex);
	}
	QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"CFavIcon::Download");
}

HICON CFavIcon::LoadFavIconFromCache(TCHAR *pURL,bool bAppendFaviconIco)
{
	HICON hIcon;
	LPINTERNET_CACHE_ENTRY_INFO pCacheEntry;
	DWORD dwCacheEntrySize;
	dwCacheEntrySize=0;

	TCHAR *pFavIconURL;
	TCHAR FavIconURL[MAXURLLENGTH];

	hIcon=NULL;
	pFavIconURL=NULL;

	if(pURL)
	{
		if(bAppendFaviconIco)
		{
			if(AppendFaviconIco(pURL,FavIconURL)) pFavIconURL=FavIconURL;
		}
		else pFavIconURL=pURL;

		if(pFavIconURL && GetUrlCacheEntryInfo(pFavIconURL,NULL,&dwCacheEntrySize)==FALSE)
		{
			if(GetLastError()==ERROR_INSUFFICIENT_BUFFER)
			{
				pCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO)new BYTE[dwCacheEntrySize];
				if(GetUrlCacheEntryInfo(pFavIconURL,pCacheEntry,&dwCacheEntrySize)==TRUE)
				{
					hIcon=(HICON)LoadImage(_Module.m_hInst,pCacheEntry->lpszLocalFileName,IMAGE_ICON,g_Scaled_IconSize,g_Scaled_IconSize,LR_DEFAULTCOLOR|LR_LOADFROMFILE);
				}
				delete [] pCacheEntry;
			}
		}
	}

	return hIcon;
}

bool CFavIcon::AppendFaviconIco(TCHAR *pURL,TCHAR FavIconURL[MAXURLLENGTH])
{
	bool result;
	int HostEndIndex;
	size_t url_len;
	CIDNA idna;
	TCHAR FavIconURL_WithScheme[MAXURLLENGTH];

	result=false;
	
	StringCchCopy(FavIconURL,MAXURLLENGTH,pURL);
	url_len=MAXURLLENGTH;
	if((idna.URLToAscii(FavIconURL,&url_len,NULL,&HostEndIndex,NULL)&IDNA_ILLEGAL)==0)
	{
		if(idna.HasScheme())
		{
			StringCchCopy(FavIconURL+HostEndIndex,MAXURLLENGTH-HostEndIndex,L"/favicon.ico");
		}
		else
		{
			StringCchCopy(FavIconURL_WithScheme,MAXURLLENGTH,L"http://");
			StringCchCatN(FavIconURL_WithScheme,MAXURLLENGTH,FavIconURL,HostEndIndex);
			StringCchCat(FavIconURL_WithScheme,MAXURLLENGTH,L"/favicon.ico");
			StringCchCopy(FavIconURL,MAXURLLENGTH,FavIconURL_WithScheme);
		}
		result=true;
	}

	return result;
}