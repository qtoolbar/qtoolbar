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
// RedirectRequest.cpp: implementation of the CRedirectRequest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RedirectRequest.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRedirectRequest::CRedirectRequest()
{
	m_pWebBrowser=NULL;
	m_bstrHeaders=NULL;
	m_iFlags=0;
	m_bstrTarget=NULL;
	m_szPostData=NULL;
	m_ulPostDataLen=0;
	m_URL[0]=0;
}

CRedirectRequest::~CRedirectRequest()
{
	Clear();
}

void CRedirectRequest::SetBrowser(IWebBrowser2* pBrowser)
{
	m_pWebBrowser=pBrowser;
	m_pWebBrowser->AddRef();
}

void CRedirectRequest::SetURL(TCHAR url[MAXURLLENGTH])
{
	StringCbCopy(m_URL,sizeof m_URL,url);
}

void CRedirectRequest::SetHeaders(BSTR bstrHeaders)
{
	m_bstrHeaders=SysAllocString(bstrHeaders);
}

void CRedirectRequest::SetTarget(BSTR bstrTarget)
{
	m_bstrTarget=SysAllocString(bstrTarget);
}

void CRedirectRequest::SetFlags(int iFlags)
{
	m_iFlags=iFlags;
}

void CRedirectRequest::SetPostData(ULONG ulPostDataLen,char *szPostData,bool bCopy)
{
	m_ulPostDataLen=ulPostDataLen;
	if(ulPostDataLen)
	{
		if(bCopy)
		{
			m_szPostData=new char[m_ulPostDataLen+1];
			if(m_szPostData)
			{
				StringCbCopyA(m_szPostData,m_ulPostDataLen+1,szPostData);
			}
			else m_ulPostDataLen=0;
		}
		else m_szPostData=szPostData;
	}
}

void CRedirectRequest::Clear()
{
	if(m_pWebBrowser)
	{
		m_pWebBrowser->Release();
		m_pWebBrowser=NULL;
	}	
	if(m_bstrTarget)
	{
		SysFreeString(m_bstrTarget);
		m_bstrTarget=NULL;
	}
	if(m_bstrHeaders)
	{
		SysFreeString(m_bstrHeaders);
		m_bstrHeaders=NULL;
	}
	if(m_szPostData)
	{
		delete m_szPostData;
		m_szPostData=NULL;
	}

	m_ulPostDataLen=0;
	m_iFlags=0;
	m_URL[0]=L'\0';
}

void CRedirectRequest::Navigate()
{
	if(m_pWebBrowser)
	{
		VARIANT vFlags,vTarget,vPostData,vHeaders;
		SAFEARRAY sArray;

		VariantInit(&vFlags);
		VariantInit(&vTarget);
		VariantInit(&vPostData);
		VariantInit(&vHeaders);

		if(m_iFlags)
		{
			vFlags.vt=VT_I4;
			vFlags.intVal=m_iFlags;
		}
		if(m_bstrTarget)
		{
			vTarget.vt=VT_BSTR;
			vTarget.bstrVal=m_bstrTarget;
		}
		if(m_bstrHeaders)
		{
			vHeaders.vt=VT_BSTR;
			vHeaders.bstrVal=m_bstrHeaders;
		}

		if(m_ulPostDataLen)
		{
			sArray.cDims=1;
			sArray.fFeatures=FADF_FIXEDSIZE;
			sArray.cbElements=1;
			sArray.cLocks=0;
			sArray.pvData=(char*)m_szPostData;
			sArray.rgsabound[0].cElements=m_ulPostDataLen;
			sArray.rgsabound[0].lLbound=0;
			vPostData.vt=VT_ARRAY|VT_UI1;
			vPostData.parray=&sArray;
		}

		m_pWebBrowser->Navigate(CComBSTR(m_URL),&vFlags,&vTarget,&vPostData,&vHeaders);

		Clear();
	}
}