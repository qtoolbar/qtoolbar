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
// QueroFilter.h: Definition of the CQueroFilter class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QueroFilter_H__F0C001AA_5F6A_45EC_A97A_D0F37DC68770__INCLUDED_)
#define AFX_QueroFilter_H__F0C001AA_5F6A_45EC_A97A_D0F37DC68770__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"       // main symbols
#include "Quero.h"
#include "QToolbar.h"

#define BUFFER_SIZE 16384
#define TAG_SIZE 2048

#define MAX_ATTR_LEN 255

#define STATE_NEUTRAL	0
#define STATE_BRACKET	1
#define STATE_TAG		2
#define STATE_QUOTE1	3
#define STATE_QUOTE2	4
#define STATE_REM		5

#define STATE_ATTRNAME	6
#define STATE_ATTRVALUE	7
#define STATE_SKIPVALUE	8

// Tag Ids
#define TAG_OBJECT		1
#define TAG_OBJECT_END	2
#define TAG_EMBED		3
#define TAG_EMBED_END	4
#define TAG_PARAM		5
#define TAG_PARAM_END	6
#define TAG_SCRIPT		7
#define TAG_SCRIPT_END	8
#define TAG_DIV			9
#define TAG_DIV_END		10
#define TAG_SPAN		11
#define TAG_SPAN_END	12
#define TAG_BASE		13
#define TAG_REM_END		14
#define TAG_IMG			15
#define TAG_IFRAME		16
#define TAG_IFRAME_END	17
#define TAG_OTHER		99

#define MAX_TAGNAME_LEN 6

#define N_TAGNAMES 8

typedef struct TagNameIds
{
	BYTE Length;
	TCHAR *pTagName;
	BYTE TagId_Start;
	BYTE TagId_End;
	int BlockAds;
	BYTE FilterScope;
} TagNameIds;

#define FILTER_SCOPE_STATIC 0x01
#define FILTER_SCOPE_DYNAMIC 0x02
#define FILTER_SCOPE_CREATEELEMENT 0x04

#define INSIDE_EMBED 0x1
#define INSIDE_OBJECT 0x2
#define INSIDE_DIV 0x4
#define INSIDE_SCRIPT 0x8
#define INSIDE_ADSCRIPT 0x10
#define INSIDE_EXTERNAL_SCRIPT 0x20
#define INSIDE_IFRAME 0x40

#define InsideAdContent() (InsideContent&(~INSIDE_SCRIPT|INSIDE_EXTERNAL_SCRIPT))!=0
#define NotInsideAdContent() (InsideContent&(~INSIDE_SCRIPT|INSIDE_EXTERNAL_SCRIPT))==0
#define InsideScript() (InsideContent&(INSIDE_SCRIPT|INSIDE_EXTERNAL_SCRIPT))!=0
#define NotInsideScript() (InsideContent&(INSIDE_SCRIPT|INSIDE_EXTERNAL_SCRIPT))==0

#define ATTR_NAME 0x1
#define ATTR_VALUE 0x2
#define ATTR_CLASSID 0x4
#define ATTR_TYPE 0x8
#define ATTR_DATA 0x10
#define ATTR_SRC 0x20
#define ATTR_WIDTH 0x40
#define ATTR_HEIGHT 0x80

class CQToolbar;

/////////////////////////////////////////////////////////////////////////////
// CQueroFilter

class ATL_NO_VTABLE CQueroFilter :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CQueroFilter,&CLSID_QueroFilter>,
	public IDispatchImpl<IQueroFilter, &IID_IQueroFilter, &LIBID_QUEROLib>, 
	public IInternetProtocol, // base class IInternetProtocolRoot
	public IInternetProtocolSink

{
public:
	CQueroFilter();
	~CQueroFilter();

	BEGIN_COM_MAP(CQueroFilter)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IQueroFilter)
		COM_INTERFACE_ENTRY_IID(IID_IInternetProtocol, IInternetProtocol)
		COM_INTERFACE_ENTRY_IID(IID_IInternetProtocolRoot, IInternetProtocol)
		COM_INTERFACE_ENTRY_IID(IID_IInternetProtocolSink, IInternetProtocolSink)
	END_COM_MAP()

	DECLARE_REGISTRY_RESOURCEID(IDR_QUEROFILTER)

	DECLARE_PROTECT_FINAL_CONSTRUCT()

// IInternetProtocolRoot interface
public:
	STDMETHOD(Start)(LPCWSTR szUrl,IInternetProtocolSink *pIProtSink,IInternetBindInfo *pIBindInfo,DWORD grfPI,HANDLE_PTR dwReserved);
	STDMETHOD(Continue)(PROTOCOLDATA *pStateInfo);
	STDMETHOD(Abort)(HRESULT hrReason,DWORD dwOptions);
	STDMETHOD(Terminate)(DWORD dwOptions);
	STDMETHOD(Suspend)();
	STDMETHOD(Resume)();
// IInternetProtocol interface
public:
	STDMETHOD(Read)(void *pv,ULONG cb,ULONG *pcbRead);
	STDMETHOD(Seek)(LARGE_INTEGER dlibMove,DWORD dwOrigin,ULARGE_INTEGER *plibNewPosition);
	STDMETHOD(LockRequest)(DWORD dwOptions);
	STDMETHOD(UnlockRequest)();

// IInternetProtocolSink interface
public:
	STDMETHOD(Switch)(PROTOCOLDATA __RPC_FAR *pProtocolData);       
	STDMETHOD(ReportProgress)(ULONG ulStatusCode, LPCWSTR szStatusText);      
	STDMETHOD(ReportData)(DWORD grfBSCF, ULONG ulProgress, ULONG ulProgressMax);        
	STDMETHOD(ReportResult)(HRESULT hrResult, DWORD dwError, LPCWSTR szResult);

// IQueroFilter
public:

	void ResetState();

	// Incoming and outgoing protocol handles
	IInternetProtocolSink* m_pOutgoingProtSink;
	IInternetProtocol* m_pIncomingProt;
	
	// PI flags handed to us
	DWORD m_grfPI;

	// Quero instance related
	CQToolbar *m_pToolbar;

	// Ad blocker related
	DWORD BlockAds;
	TCHAR BaseURL[MAXURLLENGTH];
	
	// Parsing state variables
	TCHAR ContentURL[MAXURLLENGTH];
	char buffer[BUFFER_SIZE];
	TCHAR attr_buffer[TAG_SIZE];
	BYTE prev1,prev2,prev3;
	UINT cbBuffer; // Bytes in buffer
	char* pTagStart; // Pointer to current (incomplete) tag in buffer
	int state;
	int oldstate;
	int tag;
	bool bSkipValue;
	bool bQuotesPossible;
	UINT InsideContent;
};

void OverwriteString(char *pDest,char *pSrc);
int ParseTagName(void *ppTagStart,int Len,DWORD BlockAds,BYTE FilterScope,bool IsUnicode);
int ParseAttribute(void **Tag,int *TagLen,const TCHAR **Attributes,TCHAR *Value,bool IsUnicode);
void DecodeHtmlEntities(TCHAR *pHtml);

#endif // !defined(AFX_QueroFilter_H__F0C001AA_5F6A_45EC_A97A_D0F37DC68770__INCLUDED_)
