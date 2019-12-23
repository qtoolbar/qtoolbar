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

// QEditCtrl.cpp : Implementation of CQEditCtrl

#include "stdafx.h"
#include "QEditCtrl.h"
#include "ComboQuero.h"
#include "Quero.h"
#include "QueroBand.h"
#include "OLEDropSource.h"
#include "OLEDataObject.h"

/////////////////////////////////////////////////////////////////////////////
// CQEditCtrl

CQEditCtrl* CQEditCtrl::Hook_pQEditCtrl=NULL;

CQEditCtrl::CQEditCtrl()
{
	m_lRefCount=1;

	m_pBand=NULL;
	m_pComboQuero=NULL;
	m_pToolbar=NULL;
	lastDoubleClickTime.dwHighDateTime=0;
	lastDoubleClickTime.dwLowDateTime=0;
	lastDoubleClicklParam=0;
	bSelectText=true;
	bFirstClick=false;
	bFirstDoubleClick=false;
	bDoubleClick=false;
	startSel=0;
	endSel=0;
	bStartDragDrop=false;
	bDidDragDrop=false;
	bAllowDrop=false;
	bDidDrop=false;
	dropPos=0;
	uiSetRedrawFalseCount=0;
}

CQEditCtrl::~CQEditCtrl()
{
}

//
//	IUnknown::AddRef
//
ULONG __stdcall CQEditCtrl::AddRef(void)
{
    // increment object reference count
    return InterlockedIncrement(&m_lRefCount);
}

//
//	IUnknown::Release
//
ULONG __stdcall CQEditCtrl::Release(void)
{
    // decrement object reference count
	LONG count = InterlockedDecrement(&m_lRefCount);
		
	if(count == 0)
	{
		delete this;
		return 0;
	}
	else
	{
		return count;
	}
}

//
//	IUnknown::QueryInterface
//
HRESULT __stdcall CQEditCtrl::QueryInterface(REFIID iid, void **ppvObject)
{
    // check to see what interface has been requested
    if(iid == IID_IDropTarget || iid == IID_IUnknown)
    {
        AddRef();
        *ppvObject = this;
        return S_OK;
    }
    else
    {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
}

void CQEditCtrl::SetBand(CQueroBand* pBand)
{
	m_pBand = pBand;
}

LRESULT CQEditCtrl::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RegisterDragDrop(m_hWnd,this);
	bHandled=FALSE;
	return 0;
}

LRESULT CQEditCtrl::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RevokeDragDrop(m_hWnd);
	bHandled=FALSE;
	return 0;
}

LRESULT CQEditCtrl::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(g_Options2&OPTION2_ShowSearchBox)
	{
		if(m_pBand)
		{
			m_pBand->FocusChange(TRUE); // Notify host that our band has the focus so TranslateAcceleratorIO messages are directed towards our band.
			if(m_pComboQuero->bIsEmptySearch)
			{
				m_pComboQuero->bIgnoreChange=true;
				SetWindowText(L"");
				m_pComboQuero->bIsEmptySearch=false;
				bSelectText=true;
			}
			else 
			{
				if(bSelectText) SendMessage(EM_SETSEL,0,-1);
				else bSelectText=true;
			}
		}

		// Reset auto complete list (workaround: check if window is visible to avoid crash of ResetEnumeration if AC list is active when window is closed)
		if(IsWindowVisible()) m_pToolbar->ResetAutoComplete();

		if(m_pComboQuero->bHighlightIDN)
		{
			m_pComboQuero->bHighlightIDN=false;
			ShowWindow(SW_SHOW);
		}
	}
	else
	{
		m_pToolbar->SetFocus();
	}

	#ifdef COMPILE_FOR_WINDOWS_VISTA
		OnEvent_RedrawOpaque(uMsg,wParam,lParam,bHandled);
	#else
		bHandled = FALSE;
	#endif

	return 0;
}

LRESULT CQEditCtrl::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BSTR bstrQuery;
	HWND focus;
	TCHAR *pLastFoundText;
	TCHAR phrase[MAXWORDLENGTH+1];

	if(::SendMessage(m_pComboQuero->m_hWnd,CB_GETDROPPEDSTATE,0,0))
		::SendMessage(m_pComboQuero->m_hWnd,CB_SHOWDROPDOWN,FALSE,0);

	if((g_Options&OPTION_ShowSearchEngineComboBox)==0) m_pComboQuero->ShowToolTip(0,false);

	focus=GetFocus();

	if(focus!=m_pComboQuero->m_hWndItem)
	{
		if(GetText(bstrQuery))
		{
			m_pToolbar->trim(bstrQuery);
			if(bstrQuery[0]==L'\0' || (m_pToolbar->GetCurrentType()==TYPE_SEARCH) && !StrCmp(bstrQuery,L"/"))
			{
				pLastFoundText=m_pToolbar->GetLastFoundText();

				if(g_ShowURL && m_pToolbar->currentURL[0] && (bstrQuery[0]==L'\0' || pLastFoundText[0]==L'\0'))
				{
					m_pComboQuero->SetTextCurrentURL();
				}
				else if(pLastFoundText[0])
				{
					StringCbCopy(phrase,sizeof phrase,L"/");
					StringCbCat(phrase,sizeof phrase,pLastFoundText);
					SetWindowText(phrase);
				}
				else
				{
					m_pComboQuero->SetTextPrompt();
				}
			}
			else m_pComboQuero->PositionText(true);

			SysFreeString(bstrQuery);
		}
	}

	bFirstClick=false;
	bSelectText=true;
	bDoubleClick=false;

	bHandled = FALSE;
	return 0;
}

/*LRESULT CQEditCtrl::OnNCCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(wParam) // indicate valid part of client area
	{
		LPNCCALCSIZE_PARAMS params=(LPNCCALCSIZE_PARAMS)lParam;
		params->rgrc[0].left+=3;
		params->rgrc[0].right-=30;
		return WVR_VALIDRECTS;
	}
	else // provide client area
	{
		((LPRECT)lParam)->left+=3;
		((LPRECT)lParam)->right-=30;
		bHandled=TRUE;
		return 0;
	}

	bHandled=FALSE;
	return 0;
}*/

LRESULT CQEditCtrl::OnDoubleClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG start,end,len;
	int index,width;
	BSTR bstrQuery;

	// Scroll to the original position so that the text under the cursor before the first click is selected
	if(bFirstDoubleClick)
	{
		m_pComboQuero->PositionText(true,true);
		bFirstDoubleClick=false;
	}
	
	::SendMessage(m_hWnd,EM_GETSEL,(WPARAM)&start,(LPARAM)&end);
	len=(LONG)SendMessage(m_hWnd,EM_LINELENGTH,0,0);

	if(len>1) width=(LONG)SendMessage(m_hWnd,EM_POSFROMCHAR,len-1,0)+6;
	else width=0;

	index=(int)SendMessage(m_hWnd,EM_CHARFROMPOS,0,lParam);
	start=end=index;

	if(LOWORD(lParam)>width)
	{
		start=end=len;
	}
	else
	{
		if(GetText(bstrQuery))
		{
			if(_istpunct(bstrQuery[start]))
			{
				while(start>0 && _istpunct(bstrQuery[start-1])) start--;
				while(end<len && _istpunct(bstrQuery[end])) end++;
			}
			else if(_istspace(bstrQuery[start]))
			{
				while(start>0 && _istspace(bstrQuery[start-1])) start--;
				while(end<len && _istspace(bstrQuery[end])) end++;
			}
			else
			{
				while(start>0 && !_istspace(bstrQuery[start-1]) && !_istpunct(bstrQuery[start-1])) start--;
				while(end<len && !_istspace(bstrQuery[end]) && !_istpunct(bstrQuery[end])) end++;
			}

			SysFreeString(bstrQuery);
		}
	}

	::SendMessage(m_hWnd,EM_SETSEL,start,end);
	
	startSel=start;
	endSel=end;
	bDoubleClick=true;

	if(end-start < len)
	{
		CoFileTimeNow(&lastDoubleClickTime);
		lastDoubleClicklParam=lParam;
	}
	SetCapture();
	bHandled=TRUE;
	return 0;
}

LRESULT CQEditCtrl::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG start,end,len,index,newstart,newend;
	BSTR bstrQuery;
	bool left,right;

	if(wParam==MK_LBUTTON)
	{
		if(bDoubleClick)
		{
			RECT rect;
			
			GetClientRect(&rect);

			len=(LONG)SendMessage(m_hWnd,EM_LINELENGTH,0,0);

			index=(signed short)LOWORD(lParam);
			if(index<=rect.left) index=0;
			else if(index>=rect.right) index=len;
			else index=(LONG)SendMessage(m_hWnd,EM_CHARFROMPOS,0,MAKELPARAM(index,1));

			left=(index < startSel);
			right=(index >= endSel);

			::SendMessage(m_hWnd,EM_GETSEL,(WPARAM)&start,(LPARAM)&end);

			newstart=startSel;
			newend=endSel;

			if(GetText(bstrQuery))
			{
				if(left)
				{
					if(_istpunct(bstrQuery[index]))
					{
						while(index>0 && _istpunct(bstrQuery[index-1])) index--;
					}
					else if(_istspace(bstrQuery[index]))
					{
						while(index>0 && _istspace(bstrQuery[index-1])) index--;
					}
					else
					{
						while(index>0 && !_istspace(bstrQuery[index-1]) && !_istpunct(bstrQuery[index-1])) index--;
					}
					newstart=index;
					newend=endSel;
				}
				else if(right)
				{
					if(_istpunct(bstrQuery[index]))
					{
						while(index<len && _istpunct(bstrQuery[index])) index++;
					}
					else if(_istspace(bstrQuery[index]))
					{
						while(index<len && _istspace(bstrQuery[index])) index++;
					}
					else
					{
						while(index<len && !_istspace(bstrQuery[index]) && !_istpunct(bstrQuery[index])) index++;
					}
					newstart=startSel;
					newend=index;
				}				

				SysFreeString(bstrQuery);
			}
			if(newstart!=start || newend!=end) ::SendMessage(m_hWnd,EM_SETSEL,newstart,newend);
		}
		else if(bStartDragDrop) // If the mouse is held down then start drag-drop operation
		{
			StartDragOperation(false,lParam);
		}
	}

	#ifdef COMPILE_FOR_WINDOWS_VISTA
		// Call default handler
		SetRedraw(FALSE);
		DefWindowProc();
		SetRedraw(TRUE);
		// Remove WM_MOUSEMOVE message
		MSG msg;
		PeekMessage(&msg,m_hWnd,WM_MOUSEMOVE,WM_MOUSEMOVE,PM_REMOVE);
		// Redraw
		Invalidate(FALSE);
	#else
		bHandled=FALSE;
	#endif

	return 0;
}

LRESULT CQEditCtrl::OnClickDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	FILETIME current;
	bool bTribleClick;

	CoFileTimeNow(&current);

	bTribleClick=false;
	bDoubleClick=false;
	if(lParam==lastDoubleClicklParam)
	{	
		if(current.dwHighDateTime==lastDoubleClickTime.dwHighDateTime)
		{
			if(current.dwLowDateTime-lastDoubleClickTime.dwLowDateTime < 5000000) bTribleClick=true;
		}
		else if(current.dwHighDateTime-1==lastDoubleClickTime.dwHighDateTime &&
			~lastDoubleClickTime.dwLowDateTime+current.dwLowDateTime < 5000000) bTribleClick=true;
	}

	if(bTribleClick)
	{
		PostMessage(EM_SETSEL,0,-1);
		lastDoubleClickTime.dwHighDateTime=0;
		lastDoubleClicklParam=0;
	}
	else
	{
		if(GetFocus()!=m_hWnd)
		{
			bFirstClick=true;
			bFirstDoubleClick=true;
			bSelectText=false;
			bHandled=FALSE;
		}
		else
		{
			bFirstClick=false;
			bFirstDoubleClick=false;
			
			// If the mouse is pressed when the cursor is over a selection
			// start a drag-drop operation as soon as the mouse is moved again
			if(MouseInSelection(lParam))
			{
				bStartDragDrop=true;
				bDidDragDrop=false;
				SetCapture();
			}
			else
			{
				#ifdef COMPILE_FOR_WINDOWS_VISTA
					OnEvent_RedrawOpaque(uMsg,wParam,lParam,bHandled);
				#else
					bHandled=FALSE;
				#endif
			}
		}
	}
	return 0;
}

LRESULT CQEditCtrl::OnClickUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(bFirstClick)
	{
		if(GetFocus()==m_hWnd)
		{
			LONG start,end,len;

			::SendMessage(m_hWnd,EM_GETSEL,(WPARAM)&start,(LPARAM)&end);
			len=(LONG)SendMessage(m_hWnd,EM_LINELENGTH,0,0);

			if(start==end && len)
			{
				SendMessage(m_hWnd,EM_SETSEL,0,-1);
			}
		}
		bFirstClick=false;
	}

	if(bDoubleClick)
	{
		ReleaseCapture();
		bDoubleClick=false;
	}

	// Stop drag-drop operation when the mouse is released
	if(bStartDragDrop)
	{
		ReleaseCapture();
		bStartDragDrop=false;

		if(!bDidDragDrop) // Remove selection
		{
			LRESULT nCurPos;

			nCurPos = SendMessage(m_hWnd, EM_CHARFROMPOS, 0, lParam);
			SendMessage(m_hWnd,EM_SETSEL,nCurPos,nCurPos);
		}
	}

	bHandled=FALSE;
	return 0;
}

LRESULT CQEditCtrl::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_pComboQuero->ShowToolTip(0,false);

	// Redirect VK_RETURN (sent on AC item selection directly to the embedded edit control) to the toolbar
	if(wParam==VK_RETURN && (g_Options&OPTION_NavigateOnACSelection))
	{
		m_pToolbar->PostMessage(WM_KEYDOWN,VK_RETURN);
	}
	else if(g_IE_MajorVersion>=7)
	{
		// Workaround: IE7 Disappearing Caret Bug
		POINT CaretPos;
		if(GetCaretPos(&CaretPos))
		{
			if(CaretPos.x==0 && CaretPos.y==0)
			{
				CreateDefaultCaret(true);
			}
		}
	}

	#ifdef COMPILE_FOR_WINDOWS_VISTA
		OnEvent_RedrawOpaque(uMsg,wParam,lParam,bHandled);
	#else
		bHandled=FALSE;
	#endif

	return 0;
}

bool CQEditCtrl::GetText(BSTR &bstrText)
{
	bstrText=NULL;
	if(GetWindowText(bstrText)==TRUE)
	{
		// ATL8 return TRUE if string is empty but does not initialize BSTR
		if(bstrText==NULL) bstrText=SysAllocString(L"");
	}

	return (bstrText!=NULL);
}

void CQEditCtrl::StartDragOperation(bool bDragIcon,LPARAM lParamCursorPos)
{
	COLEDataObject *pDataObject;
	COLEDropSource *pDropSource;
	DWORD dwEffect;
	DWORD dwResult;
	LONG start,end,len,index;
	BSTR bstrQuery;
	POINT CaretPos;

	FORMATETC fmtetc[MAX_DRAGDROP_FORMATS];	
	STGMEDIUM stgmed[MAX_DRAGDROP_FORMATS];
	int nDragDropFormats;

	bStartDragDrop=true;
	bDidDragDrop=false;

	// Transfer the current selection into the IDataObject
	nDragDropFormats=InitDragDropData(fmtetc,stgmed);
	if(nDragDropFormats)
	{		
		// Create IDataObject and IDropSource COM objects
		pDataObject=new COLEDataObject(fmtetc,stgmed,nDragDropFormats);
		if(pDataObject)
		{									
			pDropSource=new COLEDropSource();
			if(pDropSource)
			{
				// Get current caret pos
				GetCaretPos(&CaretPos);

				bDidDrop=false;
				dwResult = DoDragDrop((IDataObject*)pDataObject,(IDropSource*)pDropSource,DROPEFFECT_COPY|DROPEFFECT_MOVE,&dwEffect);
				
				index = (LONG)SendMessage(m_hWnd,EM_CHARFROMPOS,0,lParamCursorPos);

				// Success?
				if(dwResult == DRAGDROP_S_DROP)
				{
					if(bDidDrop) // Reflexive drag & drop
					{
						SendMessage(m_hWnd,EM_GETSEL,(WPARAM)&start,(LPARAM)&end);
						
						len=(LONG)SendMessage(m_hWnd,EM_LINELENGTH,0,0);
						if(GetText(bstrQuery))
						{
							if(dwEffect & DROPEFFECT_MOVE)
							{
								if(dropPos<start || dropPos>end)
								{
									BSTR bstrCopy;
									int i,j,k;

									bstrCopy=SysAllocString(bstrQuery);
									if(bstrCopy)
									{
										i=0;j=0;k=start;
										while(j<len)
										{
											if(i==dropPos && k<end)	bstrQuery[j++]=bstrCopy[k++];
											else if(i>=start && i<end) i++;
											else bstrQuery[j++]=bstrCopy[i++];
										}
										SysFreeString(bstrCopy);
										SetWindowText(bstrQuery);
										if(dropPos>end) SendMessage(m_hWnd,EM_SETSEL,dropPos-end+start,dropPos);
										else SendMessage(m_hWnd,EM_SETSEL,dropPos,dropPos+end-start);
									}
								}
								else SendMessage(m_hWnd,EM_SETSEL,dropPos,dropPos);
							}
							else // Copy
							{
								if(dropPos<=start || dropPos>=end)
								{
									BSTR bstrCopy;
									
									int i,j,k;

									bstrCopy=new TCHAR[len+(end-start)];								
									if(bstrCopy)
									{
										i=0;j=0;k=start;
										while(i<=len)
										{
											if(i==dropPos && k<end)	bstrCopy[j++]=bstrQuery[k++];
											else bstrCopy[j++]=bstrQuery[i++];
										}
										SetWindowText(bstrCopy);
										SendMessage(m_hWnd,EM_SETSEL,dropPos,dropPos+end-start);
										delete bstrCopy;
									}
								} else SendMessage(m_hWnd,EM_SETSEL,dropPos,dropPos);
							}
							SysFreeString(bstrQuery);
						}						
					} // End reflexive drop
					else
					{
						if(dwEffect!=DROPEFFECT_NONE) m_pBand->FocusChange(FALSE);
					}
				} // End DRAGDROP_S_DROP

				// Reset caret
				if(dwResult != DRAGDROP_S_DROP || dwEffect==DROPEFFECT_NONE)
				{
					CreateDefaultCaret(false);
					SetCaretPos(CaretPos.x,CaretPos.y);
					ShowCaret();
				}
			}
			
			pDropSource->Release();
		}
		pDataObject->Release();
	} // End InitDragDropData succeeded

	ReleaseCapture();
	bStartDragDrop=false;
	bDidDragDrop=true;
}

int CQEditCtrl::InitDragDropData(FORMATETC fmtetc[MAX_DRAGDROP_FORMATS],STGMEDIUM stgmed[MAX_DRAGDROP_FORMATS])
{
	DWORD SelStart, SelEnd;
	DWORD SelLength;
	BSTR bstrTemp;
	int nDragDropFormats;
	int i;

	UINT CF_URL = RegisterClipboardFormat(L"UniformResourceLocator");
	UINT CF_URLW = RegisterClipboardFormat(L"UniformResourceLocatorW");
	UINT CF_FILEGROUPDESCRIPTOR = RegisterClipboardFormat(L"FileGroupDescriptor");
	UINT CF_FILEGROUPDESCRIPTORW = RegisterClipboardFormat(L"FileGroupDescriptorW");
	UINT CF_FILECONTENTS = RegisterClipboardFormat(CFSTR_FILECONTENTS);

	const CLIPFORMAT ClipbardFromats[MAX_DRAGDROP_FORMATS]={CF_UNICODETEXT,CF_TEXT,CF_URLW,CF_URL,CF_FILEGROUPDESCRIPTORW,CF_FILEGROUPDESCRIPTOR,CF_FILECONTENTS};

	nDragDropFormats=0;

	SendMessage(m_hWnd, EM_GETSEL, (WPARAM)&SelStart, (LPARAM)&SelEnd);
	SelLength = SelEnd - SelStart;
	if(SelLength>0)
	{
		if(GetText(bstrTemp))
		{
			TCHAR selection[MAXURLLENGTH];
			TCHAR AsciiURL[MAXURLLENGTH];
			FILEGROUPDESCRIPTOR filegroupdescriptor;
			FILEGROUPDESCRIPTORA filegroupdescriptorA;
			TCHAR data[MAXURLLENGTH];
			LPVOID pData; // Pointer to the data that is to be stored in the Global storage medium
			size_t size;
			UINT AddressType;
			bool bConvertToAscii; // Convert the data to Ascii before writing it to the Global storage medium
			bool bIsURL;
		
			HGLOBAL hGlobal;
			LPVOID pGlobalData;

			StringCchCopyN(selection,MAXURLLENGTH,bstrTemp+SelStart,SelLength);
			SysFreeString(bstrTemp);

			// Determine which clipboard formats to initialize

			AddressType=m_pToolbar->GetIDNA()->IsAddress(selection);
			if(AddressType)
			{
				if(AddressType&ADDRESS_URL_WITHOUT_SCHEME) AddressType=ADDRESS_NONE;
				else if(AddressType&(ADDRESS_PATH|ADDRESS_DRIVE))
				{
					DWORD len=MAXURLLENGTH;
					if(FAILED(UrlCreateFromPath(selection,AsciiURL,&len,NULL))) AddressType=ADDRESS_NONE;
					else StringCbCopy(selection,sizeof selection,AsciiURL);
				}
				else if(m_pToolbar->GetIDNA()->IsInternetURL(selection))
				{
					CIDNA idna;
					size_t url_len;
					int status;

					StringCbCopy(AsciiURL,sizeof AsciiURL,selection);
					url_len=MAXURLLENGTH;
					status=idna.URLToAscii(AsciiURL,&url_len,NULL,NULL,NULL);
					if(status&IDNA_ILLEGAL) AddressType=ADDRESS_NONE;
				}
				else StringCbCopy(AsciiURL,sizeof AsciiURL,selection);
			}
			if(AddressType)
			{			
				bIsURL=true;
				nDragDropFormats=7;
			}
			else
			{
				bIsURL=false;
				nDragDropFormats=2;
			}

			// Initilize fmtetc and storage media

			i=0;
			while(i<nDragDropFormats)
			{
				fmtetc[i].cfFormat=ClipbardFromats[i];
				fmtetc[i].ptd=NULL;
				fmtetc[i].dwAspect=DVASPECT_CONTENT;
				fmtetc[i].lindex=-1;
				fmtetc[i].tymed=TYMED_HGLOBAL;

				if(ClipbardFromats[i]==CF_UNICODETEXT || ClipbardFromats[i]==CF_URLW)
				{
					pData=selection;
					StrCbLen((TCHAR*)pData,MAXURLLENGTH,size);
					if(size) size+=sizeof TCHAR;
					bConvertToAscii=false;
				}
				else if(ClipbardFromats[i]==CF_TEXT || ClipbardFromats[i]==CF_URL)
				{
					pData=bIsURL?(void*)AsciiURL:selection;
					StrCchLen((TCHAR*)pData,MAXURLLENGTH,size);
					if(size) size++;
					bConvertToAscii=true;
				}
				else if(ClipbardFromats[i]==CF_FILEGROUPDESCRIPTORW || ClipbardFromats[i]==CF_FILEGROUPDESCRIPTOR)
				{
					// Generate the filename for the link

					data[0]=L'\0';

					// Use title of the document as the filename

					if(m_pComboQuero->bCurrentURLDisplayed)
					{
						IHTMLDocument2 *pHtmlDocument;
						BSTR bsTitle=NULL;

						if(m_pToolbar->GetHtmlDocument2(&pHtmlDocument))
						{
							if(pHtmlDocument->get_title(&bsTitle)==S_OK && bsTitle)
							{
								StringCbCopy(data,sizeof data,bsTitle);
								MakeValidFileName(data,MAX_FILENAME_CCH-4);
								SysFreeString(bsTitle);
							}
							pHtmlDocument->Release();
						}
					}

					// Use url as the filename if no suitable title is available

					if(data[0]==L'\0')
					{
						StringCbCopy(data,sizeof data,selection);
						MakeValidFileName(data,MAX_FILENAME_CCH-4);
					}
			
					// Append extension
				
					StringCbCat(data,sizeof data,L".url");

					if(ClipbardFromats[i]==CF_FILEGROUPDESCRIPTORW)
					{
						filegroupdescriptor.cItems=1;
						filegroupdescriptor.fgd[0].dwFlags=0;
						StringCbCopy(filegroupdescriptor.fgd[0].cFileName,sizeof filegroupdescriptor.fgd[0].cFileName,data);

						pData=(void*)&filegroupdescriptor;
						size=sizeof filegroupdescriptor;
					}
					else
					{
						filegroupdescriptorA.cItems=1;
						filegroupdescriptorA.fgd[0].dwFlags=0;

						if(WideCharToMultiByte(CP_ACP,0,data,-1,filegroupdescriptorA.fgd[0].cFileName,sizeof filegroupdescriptorA.fgd[0].cFileName,"_",0)==0) filegroupdescriptorA.fgd[0].cFileName[0]=0;

						pData=(void*)&filegroupdescriptorA;
						size=sizeof filegroupdescriptorA;
					}
	
					fmtetc[i].lindex=0;
					bConvertToAscii=false;
				}
				else if(fmtetc[i].cfFormat==CF_FILECONTENTS)
				{
					// Generate the internet shortcut file

					StringCbCopy(data,sizeof data,L"[InternetShortcut]\r\nURL=");
					StringCbCat(data,sizeof data,AsciiURL);
					if(m_pComboQuero->bCurrentURLDisplayed)
					{
						TCHAR *FavIconURL;

						FavIconURL=m_pToolbar->GetFavIconURL();
						if(FavIconURL)
						{
							StringCbCat(data,sizeof data,L"\r\nIconFile=");
							StringCbCat(data,sizeof data,FavIconURL);
							StringCbCat(data,sizeof data,L"\r\nIconIndex=1");
						}
					}

					pData=(void*)data;
					StrCchLen((TCHAR*)pData,MAXURLLENGTH,size);
					if(size) size++;
					
					fmtetc[i].lindex=0;
					bConvertToAscii=true;
				}
				else pData=NULL;

				// Create global storage medium

				if(pData)
				{
					hGlobal=GlobalAlloc(GHND,size);
					if(hGlobal)
					{
						pGlobalData=GlobalLock(hGlobal);
						if(pGlobalData)
						{
							if(bConvertToAscii)
							{
								if(WideCharToMultiByte(CP_ACP,0,(TCHAR *)pData,-1,(char*)pGlobalData,(int)size,0,0)==0) *(char *)pGlobalData=0;
							}
							else
							{
								CopyMemory(pGlobalData,pData,size);
							}
							GlobalUnlock(hGlobal);
						}
						else
						{
							GlobalFree(hGlobal);
							hGlobal=NULL;
						}
					}
				}
				else hGlobal=NULL;

				stgmed[i].tymed=hGlobal?TYMED_HGLOBAL:TYMED_NULL;
				stgmed[i].pUnkForRelease=NULL;
				stgmed[i].hGlobal=hGlobal;
			
				i++;
			} // End enumerating formats
		} // End GetText
	} // End SelLength>0

	return nDragDropFormats;
}

#define N_INVALID_CHARS 10

void CQEditCtrl::MakeValidFileName(TCHAR *pName,size_t cchName)
{
	TCHAR *pStr;
	TCHAR ch;
	size_t i,j;
	const TCHAR InvalidCharacters[N_INVALID_CHARS+1]=L".\\<>:\"/|*?";

	pStr=pName;
	ch=*pStr;
	i=1;
	while(ch && i<cchName)
	{
		j=0;
		while(j<N_INVALID_CHARS && ch!=InvalidCharacters[j]) j++;
		if(j<N_INVALID_CHARS || ch<0x20) *pStr=L' ';

		pStr++;
		ch=*pStr;
		i++;
	}

	// Trim file name 
	if(cchName)
	{
		*pStr=L'\0';
		m_pToolbar->trim(pName);
	}
}

bool CQEditCtrl::MouseInSelection(LPARAM MouseMsgParam)
{
	DWORD nSelStart;
	DWORD nSelEnd;

	// Get the selection inside the edit control
	SendMessage(m_hWnd, EM_GETSEL, (WPARAM)&nSelStart, (LPARAM)&nSelEnd);

	if(nSelStart != nSelEnd)
	{
		DWORD nCurPos;

		// Get the cursor position the mouse has clicked on
		nCurPos = (DWORD)SendMessage(m_hWnd, EM_CHARFROMPOS, 0, MouseMsgParam);

		// Did the mouse click inside the active selection?
		return (nCurPos >= nSelStart && nCurPos < nSelEnd) ? true : false;
	}

	return false;
}

void CQEditCtrl::PositionCaret(POINTL pt)
{
	int curpos;
	int pos,len;
	RECT rect;
		
	// Get the character position of the mouse cursor
	ScreenToClient((POINT *)&pt);
	curpos = (int)SendMessage(m_hWnd, EM_CHARFROMPOS, 0, MAKELPARAM(pt.x, 1));
	if(curpos<=0) curpos=0;
	len = (int)SendMessage(m_hWnd, EM_LINELENGTH,0,0);
	GetClientRect(&rect);

	if(curpos>=len && len>0)
	{
		HDC hDC;
		SIZE sz;
		BSTR bstrQuero;

		pos = LOWORD(SendMessage(m_hWnd, EM_POSFROMCHAR, curpos-1, 0));

		hDC=GetDC();
		if(hDC)
		{
			SelectObject(hDC,GetFont());

			if(GetText(bstrQuero))
			{				
				GetTextExtentPoint32(hDC,bstrQuero+len-1,1,&sz);
				pos+=sz.cx;
				SysFreeString(bstrQuero);
			}
			ReleaseDC(hDC);
		}
	}
	else pos = LOWORD(SendMessage(m_hWnd, EM_POSFROMCHAR, curpos, 0));

	if(pos>=rect.left+3 && pos<rect.right) SetCaretPos(pos,0);
}

void CQEditCtrl::CreateDefaultCaret(bool bShowCaret)
{
	RECT rect;

	::SendMessage(m_hWnd,EM_GETRECT,0,(LPARAM)&rect);
	::CreateCaret(m_hWnd,NULL,0,rect.bottom-rect.top);
	if(bShowCaret) ShowCaret();
}

HRESULT __stdcall CQEditCtrl::DragEnter(IDataObject *pDataObject, DWORD grfKeyState,POINTL pt, DWORD *pdwEffect)
{
    // Does the dataobject contain data we want?
    bAllowDrop=QueryDataObject(pDataObject)!=0;

    if(bAllowDrop)
    {
        // Get the dropeffect based on keyboard state
        *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);

		if(*pdwEffect!=DROPEFFECT_NONE)
		{
			// Create caret in case focus has not changed but caret was destroyed due to a drag operation inside the Web page
			if(GetFocus()==m_hWnd)
			{
				CreateDefaultCaret(false);
			}
			else
			{
				bSelectText=true;
				SetFocus();
			}
			PositionCaret(pt);
			ShowCaret();
		}
    }
    else
    {
        *pdwEffect = DROPEFFECT_NONE;
    }

    return S_OK;
}

#define N_CLIPFORMATS 4

UINT CQEditCtrl::QueryDataObject(IDataObject *pDataObject)
{
    FORMATETC fmtetc = { CF_UNICODETEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

	CLIPFORMAT clipformats[N_CLIPFORMATS] = {
		CF_UNICODETEXT,
		RegisterClipboardFormat(L"UniformResourceLocatorW"),
		CF_TEXT,
		RegisterClipboardFormat(L"UniformResourceLocator")
		};

	int i;

	i=0;
	while(i<N_CLIPFORMATS)
	{
		fmtetc.cfFormat=clipformats[i];
		if(pDataObject->QueryGetData(&fmtetc)==S_OK) break;

		i++;
	}

	return (i<N_CLIPFORMATS)?fmtetc.cfFormat:0;
}

DWORD CQEditCtrl::DropEffect(DWORD grfKeyState, POINTL pt, DWORD dwAllowed)
{
	DWORD dwEffect = 0;

	// 1. check "pt" -> do we allow a drop at the specified coordinates?
	
	// 2. work out that the drop-effect should be based on grfKeyState
	if(grfKeyState & MK_CONTROL)
	{
		dwEffect = dwAllowed & DROPEFFECT_COPY;
	}
	else if(grfKeyState & MK_SHIFT)
	{
		dwEffect = dwAllowed & DROPEFFECT_MOVE;
	}
	
	// 3. no key-modifiers were specified (or drop effect not allowed), so
	//    base the effect on those allowed by the dropsource
	if(dwEffect == 0)
	{
		if(dwAllowed & DROPEFFECT_MOVE) dwEffect = DROPEFFECT_MOVE;
		else if(dwAllowed & DROPEFFECT_COPY) dwEffect = DROPEFFECT_COPY;
		else if(dwAllowed & DROPEFFECT_LINK) dwEffect = DROPEFFECT_LINK;
	}
	
	return dwEffect;
}

HRESULT __stdcall CQEditCtrl::DragOver(DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
    if(bAllowDrop)
    {
        *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
        PositionCaret(pt);
    }
    else
    {
        *pdwEffect = DROPEFFECT_NONE;
    }

    return S_OK;
}

HRESULT __stdcall CQEditCtrl::DragLeave(void)
{
	if(!bStartDragDrop) m_pBand->FocusChange(FALSE);
    return S_OK;
}

HRESULT __stdcall CQEditCtrl::Drop(IDataObject *pDataObject, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
    PositionCaret(pt);

    if(bAllowDrop)
    {
		*pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);

		if(*pdwEffect!=DROPEFFECT_NONE) DropData(pDataObject,pt);
    }
    else
    {
        *pdwEffect = DROPEFFECT_NONE;
    }
    
    return S_OK;
}

void CQEditCtrl::DropData(IDataObject *pDataObject,POINTL pt)
{
	// UINT CF_URL = RegisterClipboardFormat(_T("UniformResourceLocator"));
	UINT CF_URLW = RegisterClipboardFormat(_T("UniformResourceLocatorW"));

	bool bUnicode;
	CLIPFORMAT cfFormat;

	// Construct a FORMATETC object
	FORMATETC fmtetc = { CF_TEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stgmed;
	TCHAR UnicodeText[MAXURLLENGTH];

	// See if the dataobject contains any TEXT stored as a HGLOBAL
	cfFormat=QueryDataObject(pDataObject);
	if(cfFormat)
	{
		bUnicode=(cfFormat==CF_UNICODETEXT || cfFormat==CF_URLW);

		fmtetc.cfFormat=cfFormat;
	
		if(pDataObject->QueryGetData(&fmtetc) == S_OK)
		{
			// Get the data
			if(pDataObject->GetData(&fmtetc, &stgmed) == S_OK)
			{
				int curpos,start,end;
			
				ScreenToClient((POINT *)&pt);
				curpos = (int)SendMessage(m_hWnd, EM_CHARFROMPOS, 0, MAKELPARAM(pt.x, 0));
				SendMessage(m_hWnd, EM_GETSEL,(WPARAM)&start,(LPARAM)&end);

				// We asked for the data as a HGLOBAL, so access it appropriately
				PVOID data = GlobalLock(stgmed.hGlobal);
				if(data)
				{
					if(bStartDragDrop) // Do "reflexive" drag & drop
					{
						bDidDrop=true;
						dropPos=curpos;
					}
					else
					{
						if(bUnicode) StringCbCopy(UnicodeText,sizeof UnicodeText,(TCHAR*)data);
						else
						{
							if(MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,(char*)data,-1,UnicodeText,MAXURLLENGTH)==0) UnicodeText[0]=0;
						}
						 // Remove newslines to support pasting multi-line URLs
						CQToolbar::TrimNewlines(UnicodeText,MAXURLLENGTH,UnicodeText);
						SetWindowText(UnicodeText);
						SendMessage(m_hWnd,EM_SETSEL,0,-1);
					}

					GlobalUnlock(stgmed.hGlobal);
				}

				ReleaseStgMedium(&stgmed);
			}			
		}
	}
}

LRESULT CQEditCtrl::OnPaste(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(OpenClipboard())
	{
		HANDLE hClipboardText;
		TCHAR *pClipboardText;
		TCHAR ClipboardText[MAXURLLENGTH];

		hClipboardText=GetClipboardData(CF_UNICODETEXT);
		if(hClipboardText)
		{
			pClipboardText=(TCHAR*)GlobalLock(hClipboardText);
			if(pClipboardText)
			{
				// Remove newslines to support pasting multi-line URLs
				CQToolbar::TrimNewlines(ClipboardText,MAXURLLENGTH,pClipboardText);
				SendMessage(EM_REPLACESEL,(WPARAM)TRUE,(LPARAM)ClipboardText);
				GlobalUnlock(hClipboardText);
			} // End GlobalLock
		} // End GetClipboardData
		CloseClipboard();
	} // End OpenClipboard

	// OnEvent_RedrawOpaque? EM_REPLACESEL -> call OnEvent_RedrawOpaque
	return 0;
}

LRESULT CQEditCtrl::OnContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Initialize hook members
	Hook_pQEditCtrl=this;
	Hook_bPasteAndGoAdded=false;
	Hook_bUpOneLevelAdded=false;
	Hook_bstrPasteAndGo=SysAllocString(m_pToolbar->GetString(IDS_PASTE_AND_GO));
	Hook_bstrUpOneLevel=SysAllocString(m_pToolbar->GetString(IDS_UP_ONE_LEVEL));
	Hook_bUpOneLevelEnabled=m_pToolbar->NavigateUp_Available();

	// Set focus
	if(GetFocus()!=m_hWnd) SetFocus();

	// Install hook
	Hook_hCallWndProcHook=::SetWindowsHookEx(WH_CALLWNDPROC,CallWndProcHook,NULL,GetCurrentThreadId());

	// Invoke default context menu
	g_MiddleClick=false;
	DefWindowProc();

	// Uninstall hook and free memory
	if(Hook_hCallWndProcHook) UnhookWindowsHookEx(Hook_hCallWndProcHook);
	if(Hook_bstrPasteAndGo) SysFreeString(Hook_bstrPasteAndGo);
	if(Hook_bstrUpOneLevel) SysFreeString(Hook_bstrUpOneLevel);

	return 0;
}

LRESULT CQEditCtrl::OnPasteAndGo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SendMessage(WM_PASTE);
	m_pToolbar->SetFocusOnIEServerWindow();
	if(g_MiddleClick) lParam|=SHORTCUT_OPTION_MIDDLECLICK;
	m_pToolbar->PostMessage(WM_QUERO,wParam,lParam);
	return 0;
}

LRESULT CQEditCtrl::OnUpOneLevel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_pToolbar->SetFocusOnIEServerWindow();
	m_pToolbar->NavigateUp(OPEN_UNDEFINED);
	return 0;
}

LRESULT CALLBACK CQEditCtrl::CallWndProcHook(int nCode,WPARAM wParam,LPARAM lParam)
{
	LRESULT result;
	CWPSTRUCT *pCWP;
	HMENU hMenu;
	UINT flags;
	DWORD style;

	if(Hook_pQEditCtrl)
	{
		if(nCode==HC_ACTION && Hook_pQEditCtrl->Hook_bPasteAndGoAdded==false)
		{
		pCWP=(CWPSTRUCT*)lParam;

			// Ignore MN_GETHMENU
			if(pCWP->message!=MN_GETHMENU)
			{
				// Is pop-up window?
				style=::GetWindowLong(pCWP->hwnd,GWL_STYLE);
				if((style&WS_POPUPWINDOW))
				{
					// Get menu (GetMenu always returns NULL)
					hMenu=(HMENU)::SendMessage(pCWP->hwnd,MN_GETHMENU,0,0);
					if(hMenu && Hook_pQEditCtrl->Hook_bstrPasteAndGo && Hook_pQEditCtrl->Hook_bstrUpOneLevel) // Msg  0x1e2 == 482
					{
						// Insert Up One Level
						Hook_pQEditCtrl->Hook_bUpOneLevelAdded=true;						
						flags=MF_BYPOSITION|MF_STRING;
						if(Hook_pQEditCtrl->Hook_bUpOneLevelEnabled==false) flags|=MF_DISABLED|MF_GRAYED;
						InsertMenu(hMenu,0,flags,WM_UP_ONE_LEVEL,Hook_pQEditCtrl->Hook_bstrUpOneLevel);
						InsertMenu(hMenu,1,MF_BYPOSITION|MF_SEPARATOR,0,NULL);

						// Insert Paste and Go command
						Hook_pQEditCtrl->Hook_bPasteAndGoAdded=true;
						flags=MF_BYCOMMAND|MF_STRING;
						if(IsClipboardFormatAvailable(CF_TEXT)==FALSE) flags|=MF_DISABLED|MF_GRAYED;
						InsertMenu(hMenu,0x303,flags,WM_PASTE_AND_GO,Hook_pQEditCtrl->Hook_bstrPasteAndGo);
					}
				}
			}
		}
		
		result=CallNextHookEx(Hook_pQEditCtrl->Hook_hCallWndProcHook,nCode,wParam,lParam);
	}
	else result=0;
	
	return result;
}

LRESULT CQEditCtrl::OnMiddleClickUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MapMiddleButton_ContextMenu(wParam,lParam);
	return 0;
}

bool CQEditCtrl::SetRedraw(BOOL bRedraw)
{
	bool bSetRedraw;

	if(bRedraw)
	{
		if(uiSetRedrawFalseCount) uiSetRedrawFalseCount--;
		bSetRedraw=(uiSetRedrawFalseCount==0);
	}
	else
	{
		bSetRedraw=(uiSetRedrawFalseCount==0);
		uiSetRedrawFalseCount++;
	}

	if(bSetRedraw) CWindow::SetRedraw(bRedraw);

	//QDEBUG_PRINTF(L"CQEditCtrl::SetRedraw",L"%d %d",SetRedrawFalseCount,bRedraw);

	return bSetRedraw;
}

#ifdef COMPILE_FOR_WINDOWS_VISTA

LRESULT CQEditCtrl::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPaintDC targetDC(m_hWnd);
	HDC hBufferedDC;
	CBufferedPaint BufferedPaint;
	
	if(BufferedPaint.Begin(targetDC,&targetDC.m_ps.rcPaint,BPBF_TOPDOWNDIB,NULL,&hBufferedDC))
	{
		SendMessage(WM_PRINTCLIENT,	reinterpret_cast<WPARAM>(hBufferedDC),PRF_CLIENT);
		BufferedPaint.MakeOpaque();
		BufferedPaint.End(TRUE);
	}

	return 0;
}

LRESULT CQEditCtrl::OnErase(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 1;
}

LRESULT CQEditCtrl::OnEvent_RedrawOpaque(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetRedraw(FALSE);
	DefWindowProc();
	SetRedraw(TRUE);
	Invalidate(FALSE);
	return 0;
}

#endif