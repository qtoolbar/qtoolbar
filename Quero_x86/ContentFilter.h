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
// ContentFilter.h

#ifndef __CONTENTFILTER_H
#define __CONTENTFILTER_H

#include "QueroFilter.h"

class CQToolbar;

// FlashFilterState
typedef struct FilterState
{
	IHTMLDocument2 *pHtmlDocument;
	IUnknown *pUnknown;
	BSTR BaseURL;
	UINT BlockAds;
	CQToolbar *pToolbar;
	TCHAR WriteBuffer[TAG_SIZE];
	TCHAR prev1,prev2,prev3;
	int State;
	int OldState;
	int Tag;
	bool bSkipValue;
	bool bQuotesPossible;
	UINT InsideContent;
	bool IsWriteln;
	bool IsPutHTML;
	UINT cchBuffer;
	bool WriteToBuffer;
	TCHAR* pTagStart;
	TCHAR* pBlockStart;
	TCHAR* pDivStart;
	int DivLevel;
} FilterState;

 // Maximum number of cuncurrent dcoument.write/ln invocations
#define MAX_CONCURRENT_WRITES 64

#define GETDOCUMENTURL_IHTMLDOCUMENT2 0
#define GETDOCUMENTURL_IHTMLELEMENT 1

class CContentFilter  
{
public:
	CContentFilter();
	~CContentFilter();

	void FreeFilterStates(CQToolbar *pToolbar);
	
	static bool FilterContent(FilterState *pState,BSTR pText);
	static void OrigDocumentWrite(IHTMLDocument2 *pHtmlDocument,BSTR bstr,bool IsWriteln);
	static IUnknown* GetDocumentRoot(IHTMLDocument2 __RPC_FAR* pHtmlDocument);
	static BSTR GetDocumentURL(IUnknown *pUnknown,BYTE Type);
	static bool FilterDocumentWrite(IHTMLDocument2 __RPC_FAR* pHtmlDocument,SAFEARRAY *psarray,bool IsWriteln);
	static bool FilterTag(IUnknown * pUnknown,BSTR pHtml,bool IsCreateElement);

	static void OverwriteString(TCHAR *pDest,TCHAR *pSrc);
	static void TerminateBSTR(BSTR bstr, TCHAR *pTerminatePosition);
	
	FilterState FilterStates[MAX_CONCURRENT_WRITES];
	int FilterStateIndex;

};

#endif