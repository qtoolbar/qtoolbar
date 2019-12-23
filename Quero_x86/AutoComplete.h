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
// AutoComplete.h: interface for the CAutoComplete class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTOCOMPLETE_H__31FF7BEB_34F9_41EC_8204_7BA59E2A3784__INCLUDED_)
#define AFX_AUTOCOMPLETE_H__31FF7BEB_34F9_41EC_8204_7BA59E2A3784__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAXWORDS 30
#define MAXWORDLENGTH 256

class CQToolbar;

class CAutoComplete : public IEnumString
{

public:

	// Constructors/destructors

	CAutoComplete();	
	~CAutoComplete();

	// Implementation

	void SetToolbar(CQToolbar *pToolbar);
	BOOL Bind(HWND hWndEdit, DWORD dwOptions = 0, LPCTSTR lpszFormatString = NULL);
	VOID Unbind();
	BOOL Enable(BOOL bEnable);

	void ResetEnumerator();
	bool GetDropDownStatus();

	//
	//	IUnknown implementation
	//
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject);

	//
	//	IEnumString implementation
	//
	STDMETHODIMP Next(ULONG celt, LPOLESTR* rgelt, ULONG* pceltFetched);
	STDMETHODIMP Skip(ULONG celt);
 
	STDMETHODIMP Reset(void); 
	STDMETHODIMP Clone(IEnumString** ppEnum);

private:
	CQToolbar *m_pToolbar;
	HKEY m_hKey;
	IAutoComplete *m_pAC;

	volatile ULONG m_nRefCount;
	UINT m_nCurrentElement;
	UINT nWords;
	TCHAR Words[MAXWORDS][MAXWORDLENGTH];
};

#endif // !defined(AFX_AUTOCOMPLETE_H__31FF7BEB_34F9_41EC_8204_7BA59E2A3784__INCLUDED_)
