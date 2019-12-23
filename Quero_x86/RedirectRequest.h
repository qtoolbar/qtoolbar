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

// RedirectRequest

#ifndef __REDIRECTREQUEST_H
#define __REDIRECTREQUEST_H

#include "IDNA.h"
#include <strsafe.h>

class CRedirectRequest  
{
public:
	CRedirectRequest();
	~CRedirectRequest();

	void Clear();
	void Navigate();

	void SetURL(TCHAR url[MAXURLLENGTH]);
	void SetBrowser(IWebBrowser2* pBrowser);
	void SetHeaders(BSTR bstrHeaders);
	void SetTarget(BSTR bstrTarget);
	void SetFlags(int iFlags);
	void SetPostData(ULONG ulPostDataLen,char *szPostDate,bool bCopy);

private:
	IWebBrowser2* m_pWebBrowser;
	BSTR m_bstrHeaders;
	int m_iFlags;
	BSTR m_bstrTarget;
	char* m_szPostData;
	ULONG m_ulPostDataLen;
	
	TCHAR m_URL[MAXURLLENGTH];
};

#endif
