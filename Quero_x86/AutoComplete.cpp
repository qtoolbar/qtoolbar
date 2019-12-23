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
// AutoComplete.cpp: implementation of the CAutoComplete class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AutoComplete.h"
#include "QToolbar.h"

DEFINE_GUID(CLSID_AutoComplete, 0x00bb2763, 0x6a77, 0x11d0, 0xa5, 0x35, 0x00, 0xc0, 0x4f, 0xd7, 0xd0, 0x62);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoComplete::CAutoComplete()
{
	m_pAC=NULL;
	m_nCurrentElement=0;
	m_nRefCount=1;
	m_hKey=NULL;
	m_pToolbar=NULL;
	nWords=0;
}

CAutoComplete::~CAutoComplete()
{
	Unbind();
}

// Implementation

void CAutoComplete::SetToolbar(CQToolbar *pToolbar)
{
	m_pToolbar=pToolbar;
}

BOOL CAutoComplete::Bind(HWND hWndEdit,DWORD dwOptions,LPCTSTR lpszFormatString)
{
	HRESULT hr;
	
	if(m_pAC==NULL)
	{
		hr=CoCreateInstance(CLSID_AutoComplete,NULL,CLSCTX_INPROC_SERVER,IID_IAutoComplete,(LPVOID*)&m_pAC);
		if(SUCCEEDED_OK(hr))
		{
			hr=m_pAC->Init(hWndEdit,this,NULL,lpszFormatString);
			if(SUCCEEDED_OK(hr))
			{
				if(dwOptions)
				{
					IAutoComplete2 *pAC2;

					hr=m_pAC->QueryInterface(IID_IAutoComplete2,(LPVOID*)&pAC2);
					if(SUCCEEDED_OK(hr))
					{
						pAC2->SetOptions(dwOptions);
						pAC2->Release();
					}
				}
			}
			else
			{
				m_pAC->Release();
				m_pAC=NULL;
			}
		}
	}
	else hr=FALSE;

	return hr;
}

void CAutoComplete::Unbind()
{
	if(m_pAC)
	{
		m_pAC->Release();
		m_pAC=NULL;
	}
}

BOOL CAutoComplete::Enable(BOOL bEnable)
{
	return m_pAC?SUCCEEDED_OK(m_pAC->Enable(bEnable)):FALSE;
}

void CAutoComplete::ResetEnumerator()
{
	if(m_pAC)
	{
		IAutoCompleteDropDown *pACDropDown;

		if(SUCCEEDED_OK(m_pAC->QueryInterface(IID_IAutoCompleteDropDown,(LPVOID*)&pACDropDown)))
		{
			pACDropDown->ResetEnumerator();

			pACDropDown->Release();
		}
	}
}

bool CAutoComplete::GetDropDownStatus()
{
	bool result=false;
	DWORD flags;

	if(m_pAC)
	{
		IAutoCompleteDropDown *pACDropDown;

		if(SUCCEEDED_OK(m_pAC->QueryInterface(IID_IAutoCompleteDropDown,(LPVOID*)&pACDropDown)))
		{
			if(SUCCEEDED_OK(pACDropDown->GetDropDownStatus(&flags,NULL)))
			{
				result=(flags==ACDD_VISIBLE);
			}

			pACDropDown->Release();
		}
	}

	return result;
}

//
//	IUnknown implementation
//
STDMETHODIMP_(ULONG) CAutoComplete::AddRef()
{
	return ::InterlockedIncrement(reinterpret_cast<volatile LONG*>(&m_nRefCount));
}

STDMETHODIMP_(ULONG) CAutoComplete::Release()
{
	ULONG nCount;
	
	nCount = (ULONG) ::InterlockedDecrement(reinterpret_cast<volatile LONG*>(&m_nRefCount));

	if (nCount == 0) delete this;

	return nCount;
}

STDMETHODIMP CAutoComplete::QueryInterface(REFIID riid, void** ppvObject)
{
	HRESULT hr;
	
	if(ppvObject)
	{
		if(riid==IID_IUnknown || riid==IID_IEnumString)
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

//
//	IEnumString implementation
//
STDMETHODIMP CAutoComplete::Next(ULONG celt, LPOLESTR* rgelt, ULONG* pceltFetched)
{
	ULONG i;
	UINT n;
	size_t len;

	i=0;

	if(WaitForSingleObject(g_hQSharedListMutex,QMUTEX_TIMEOUT)==WAIT_OBJECT_0)
	{
		n=g_HistoryIndex+g_nURLs+nWords;
		
		if(!celt) celt = 1;
		if(pceltFetched) *pceltFetched=0;
		if(nWords)
		{
			if(g_Options&OPTION_EnableQuickFind) n+=nWords;
			n++;
		}

		while(i<celt && m_nCurrentElement<n)
		{
			TCHAR* entry;

			if(m_nCurrentElement<g_HistoryIndex)
			{
				HistoryEntry *h;

				h=&g_History[m_nCurrentElement];
				if(h->Type==TYPE_SEARCH) entry=h->Query;
				else 
				{
					m_nCurrentElement++;
					continue;
				}
			}
			else 
				if(m_nCurrentElement<g_HistoryIndex+g_nURLs) entry=g_URLs[m_nCurrentElement-g_HistoryIndex];
			else 
				if(m_nCurrentElement<n-1) entry=Words[(m_nCurrentElement-g_HistoryIndex-g_nURLs)%nWords];
			else
			{
				HistoryEntry* lastSearch;

				lastSearch=m_pToolbar->GetLastHistoryEntry();

				if(lastSearch && lastSearch->Type==TYPE_SEARCH) entry=lastSearch->Query;
				else entry=NULL;
			}

			if(entry)
			{
				StrCbLen(entry,MAXURLLENGTH*sizeof(TCHAR),len);
				if(len)
				{
					if(m_nCurrentElement>=g_HistoryIndex+g_nURLs+(g_Options&OPTION_EnableQuickFind?nWords:0))
					{
						len+=2*sizeof(TCHAR);
						rgelt[i] = (LPWSTR)::CoTaskMemAlloc(len);
						if(rgelt[i])
						{
							StringCbCopy(rgelt[i],len,L"/");
							StringCbCat(rgelt[i],len,entry);
						}
						else break;
					}
					else
					{
						len+=sizeof(TCHAR);
						rgelt[i] = (LPWSTR)::CoTaskMemAlloc(len);
						if(rgelt[i]) StringCbCopy(rgelt[i],len,entry);
						else break;
					}

					if(pceltFetched) *pceltFetched++;
				}
			}
			else break;
			
			m_nCurrentElement++;			
			i++;
		}

		ReleaseMutex(g_hQSharedListMutex);
	}
	QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"Next");

	return (i==celt)?S_OK:S_FALSE;
}

STDMETHODIMP CAutoComplete::Skip(ULONG celt)
{
	m_nCurrentElement += celt;

	if (m_nCurrentElement >= g_HistoryIndex+g_nURLs) 
	{
		m_nCurrentElement = 0;
		nWords=0;
		return S_FALSE;
	}
	else return S_OK;
}

STDMETHODIMP CAutoComplete::Reset(void)
{
	m_nCurrentElement = 0;

	QD(L"reset");
	
	m_pToolbar->SyncHistory();
	m_pToolbar->SyncURLs();
	
	m_pToolbar->InitWords(Words,&nWords,INITWORDS_Bigrams|INITWORDS_History);

	return S_OK;
}

STDMETHODIMP CAutoComplete::Clone(IEnumString** ppEnum)
{
	HRESULT hr;

	if(ppEnum)
	{
		CAutoComplete* pNew = new CAutoComplete();

		if(pNew)
		{
			pNew->SetToolbar(m_pToolbar);
			pNew->m_nCurrentElement=m_nCurrentElement;
			m_pToolbar->CopyWords(pNew->Words,&pNew->nWords,Words,nWords);

			*ppEnum = pNew;
			hr=S_OK;
		}
		else hr=E_OUTOFMEMORY;
	}
	else hr=E_POINTER;
	
	return hr;
}