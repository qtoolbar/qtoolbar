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

// QueroFilter.cpp : Implementation of CQueroApp and DLL registration.

#include "stdafx.h"
#include "QueroFilter.h"

TagNameIds TagNames[N_TAGNAMES]={
	{3,L"img",TAG_IMG,0,ADBLOCKER_BLOCK_BANNER,FILTER_SCOPE_STATIC|FILTER_SCOPE_DYNAMIC|FILTER_SCOPE_CREATEELEMENT},
	{3,L"div",TAG_DIV,TAG_DIV_END,ADBLOCKER_BLOCK_DIV,FILTER_SCOPE_DYNAMIC},
	{4,L"base",TAG_BASE,0,0,FILTER_SCOPE_STATIC},
	{5,L"embed",TAG_EMBED,TAG_EMBED_END,ADBLOCKER_BLOCK_FLASH,FILTER_SCOPE_STATIC|FILTER_SCOPE_DYNAMIC|FILTER_SCOPE_CREATEELEMENT},
	{5,L"param",TAG_PARAM,0,ADBLOCKER_BLOCK_FLASH,FILTER_SCOPE_STATIC|FILTER_SCOPE_DYNAMIC},
	{6,L"script",TAG_SCRIPT,TAG_SCRIPT_END,0,FILTER_SCOPE_STATIC|FILTER_SCOPE_DYNAMIC},
	{6,L"object",TAG_OBJECT,TAG_OBJECT_END,ADBLOCKER_BLOCK_FLASH,FILTER_SCOPE_STATIC|FILTER_SCOPE_DYNAMIC|FILTER_SCOPE_CREATEELEMENT},
	{6,L"iframe",TAG_IFRAME,TAG_IFRAME_END,ADBLOCKER_BLOCK_IFRAME,FILTER_SCOPE_DYNAMIC|FILTER_SCOPE_CREATEELEMENT}
}; // (update MAX_TAGNAME_LEN if necessary)

const TCHAR* AttrObject[]={L"classid",L"type",L"data",NULL};
const TCHAR* AttrParam[]={L"name",L"value",NULL};
const TCHAR* AttrEmbed[]={L"type",L"src",NULL};
const TCHAR* AttrBase[]={L"href",NULL};
const TCHAR* AttrScript[]={L"src",NULL};
const TCHAR* AttrImg[]={L"src",L"width",L"height",NULL};
const TCHAR* AttrIFrame[]={L"src",NULL};

/////////////////////////////////////////////////////////////////////////////
//

CQueroFilter::CQueroFilter()
{
	m_pOutgoingProtSink=NULL;
	m_pIncomingProt=NULL;
	m_grfPI=0;

	ResetState();
}


CQueroFilter::~CQueroFilter()
{
	if(m_pOutgoingProtSink) m_pOutgoingProtSink->Release();
	if(m_pIncomingProt) m_pIncomingProt->Release();
}

void CQueroFilter::ResetState()
{
	m_pToolbar=NULL;

	BlockAds=g_BlockAds;

	state=oldstate=STATE_NEUTRAL;
	tag=0;
	InsideContent=0;
	cbBuffer=0;
	pTagStart=NULL;
	prev1=0;
	prev2=0;
	prev3=0;
	bSkipValue=false;
	bQuotesPossible=false;
}

STDMETHODIMP CQueroFilter::Start(LPCWSTR szUrl,IInternetProtocolSink *pIProtSink,IInternetBindInfo *pIBindInfo,DWORD grfPI,HANDLE_PTR dwReserved)
{
	HRESULT hr;
	PROTOCOLFILTERDATA* ProtocolFilterData;
	TCHAR *pBindURL;
	ULONG cElements;
	QThreadData* pQueroInstance;

	// QDEBUG_PRINTF(L"QueroFilter",L"thread [%x]",GetCurrentThreadId());

	// If ad blocker is turned off consider using INET_E_USE_DEFAULT_PROTOCOLHANDLER?

	if((grfPI&PI_FILTER_MODE) && dwReserved)
    {
		// Get the protocol pointer from reserved pointer
        ProtocolFilterData = (PROTOCOLFILTERDATA*) dwReserved;

		if(ProtocolFilterData->pProtocol) // We can't do anything without an interface to read from!
		{
			ATLASSERT(NULL == m_pIncomingProt);
			ATLASSERT(NULL == m_pOutgoingProtSink);

			// Reset parsing state, BlockAds, m_pQueroInstance, m_InitialThreadId, m_pToolbar
			ResetState();

			// Hold incoming and outgoing handles
			m_pIncomingProt=ProtocolFilterData->pProtocol;
			m_pIncomingProt->AddRef();
			m_pOutgoingProtSink=pIProtSink;
			m_pOutgoingProtSink->AddRef();

			if(BlockAds&ADBLOCKER_Enable)
			{
				// Get the bind URL

				cElements=0;
				if(SUCCEEDED_OK(pIBindInfo->GetBindString(BINDSTRING_URL,&pBindURL,1,&cElements)) && cElements)
				{
					StringCbCopy(BaseURL,sizeof BaseURL,pBindURL);
					CoTaskMemFree(pBindURL);
				}
				else
				{
					QDEBUG_PRINT(L"pIBindInfo->GetBindString",L"failed");
					BaseURL[0]=L'\0';
				}

				// Get the associated Quero Toolbar

				QDEBUG_CODE IServiceProvider *pServiceProvider;
				QDEBUG_CODE IWindowForBindingUI *pWindowForBindingUI;
				QDEBUG_CODE HWND hIEWnd;

				QDEBUG_CODE	if(SUCCEEDED_OK(pIProtSink->QueryInterface(IID_IServiceProvider,(LPVOID*)&pServiceProvider)))
				QDEBUG_CODE	{
				QDEBUG_CODE		if(SUCCEEDED_OK(pServiceProvider->QueryService(IID_IWindowForBindingUI,IID_IWindowForBindingUI,(LPVOID*)&pWindowForBindingUI)))
				QDEBUG_CODE		{
				QDEBUG_CODE			if(SUCCEEDED_OK(pWindowForBindingUI->GetWindow(IID_IHttpSecurity,&hIEWnd)))
				QDEBUG_CODE			{
				QDEBUG_CODE				//DWORD ThreadId;
							
				QDEBUG_CODE				//ThreadId=GetWindowThreadProcessId(hIEWnd,NULL);

				QDEBUG_CODE				//QDEBUG_PRINTF(L"GetWindowThreadProcessId",L"thread [%x]",ThreadId);
				QDEBUG_CODE			}
				QDEBUG_CODE			else QD(L"GetWindow failed");
				QDEBUG_CODE			pWindowForBindingUI->Release();

				QDEBUG_CODE		}
				QDEBUG_CODE		else QD(L"QueryService failed");
				QDEBUG_CODE		pServiceProvider->Release();
				QDEBUG_CODE	} else QD(L"QueryInterface failed");

				if(WaitForSingleObject(g_hQSharedDataMutex,QMUTEX_TIMEOUT)==WAIT_OBJECT_0)
				{
					pQueroInstance=GetCurrentQueroInstance();

					// Determine effective BlockAds settings for current URL

					if(pQueroInstance)
					{
						m_pToolbar=pQueroInstance->pToolbar;
						BlockAds=m_pToolbar->GetWhiteListBlockAds(m_pToolbar->IsNavigationPending()?BaseURL:NULL);
					}
					else
					{
						// Workaround: get first Quero instance
						QDEBUG_PRINT(L"CQueroFilter::Start",L"GetFirstQueroInstance");
						pQueroInstance=GetFirstQueroInstance();
						if(pQueroInstance)
						{
							BlockAds=pQueroInstance->pToolbar->GetWhiteListBlockAds(BaseURL);
						}
					}

					ReleaseMutex(g_hQSharedDataMutex);
				}
				QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"CQueroFilter::Start");
			}

			m_grfPI=grfPI;
		
			hr = S_OK;
		}
		else hr = E_INVALIDARG;
    }
	else  hr = E_INVALIDARG;

	return hr;
}

STDMETHODIMP CQueroFilter::Continue(PROTOCOLDATA *pStateInfo)
{
	return m_pIncomingProt?m_pIncomingProt->Continue(pStateInfo):E_FAIL;
}

STDMETHODIMP CQueroFilter::Abort(HRESULT hrReason,DWORD dwOptions)
{
	return m_pIncomingProt?m_pIncomingProt->Abort(hrReason, dwOptions):E_FAIL;
}

STDMETHODIMP CQueroFilter::Terminate(DWORD dwOptions)
{
	HRESULT hr;

	if(m_pOutgoingProtSink)
	{
		m_pOutgoingProtSink->Release();
		m_pOutgoingProtSink=NULL;
	}

	if(m_pIncomingProt) hr=m_pIncomingProt->Terminate(dwOptions);
	else hr=E_FAIL;

	// m_pIncomingProt is released in destructor because after Terminate Read can be called

	return hr;
}
STDMETHODIMP CQueroFilter::Suspend()
{
	return m_pIncomingProt?m_pIncomingProt->Suspend():E_FAIL;
}

STDMETHODIMP CQueroFilter::Resume()
{
	return m_pIncomingProt?m_pIncomingProt->Resume():E_FAIL;
}

STDMETHODIMP CQueroFilter::Read(void *pv, ULONG cb, ULONG *pcbRead)
{
	HRESULT hr;
	bool ContentBlocked;

	if(m_pIncomingProt && pv && pcbRead)
	{
		if(BlockAds&ADBLOCKER_Enable)
		{
			//hr=m_pIncomingProt->Read(buffer+cbNextRead,(cb+cbNextRead+TAG_SIZE>BUFFER_SIZE)?BUFFER_SIZE-cbNextRead:cb+TAG_SIZE-cbNextRead, pcbRead);
			//hr=m_pIncomingProt->Read(buffer+cbNextRead,(cb+cbNextRead+TAG_SIZE>BUFFER_SIZE)?BUFFER_SIZE-cbNextRead:cb+TAG_SIZE, pcbRead);
			*pcbRead=0;
			hr=m_pIncomingProt->Read(buffer+cbBuffer,(cb+cbBuffer+TAG_SIZE>BUFFER_SIZE)?BUFFER_SIZE-cbBuffer:cb+TAG_SIZE, pcbRead);
	
			//QDEBUG_PRINTF(L"Read",L"cb=%d, cbBuffer=%d, pcb=%d",cb,cbBuffer,*pcbRead);

			if(hr==INET_E_DATA_NOT_AVAILABLE && cbBuffer)
			{
				QDEBUG_PRINTF(L"CQueroFilter",L"INET_E_DATA_NOT_AVAILABLE pcb=%d",*pcbRead);
				*pcbRead=0;
				hr=S_OK;
			}

			/*if(m_pToolbar==NULL)
			{
				TCHAR text[20000];
				MultiByteToWideChar(CP_ACP,0,(char*)buffer,-1,text,*pcbRead);
				text[*pcbRead]=0;
				QDEBUG_PRINT(L"HTMLFilter:",text);
			}*/

			if(SUCCEEDED(hr) || hr==E_PENDING) // hr==S_OK || hr==S_FALSE
			{
				signed long i; // i can become negative
				char *pHtmlData;
				char *pAttrStart;
				int attr_index;
				int AttrParsed;
				int ImgWidth,ImgHeight;

				BYTE c;
				int len;
				bool bTagTooLarge;

				pHtmlData=buffer+cbBuffer;
				cbBuffer+=*pcbRead;

				//if(pTagStart) QDEBUG_PRINTA(L"QueroFilter pTagStart",pTagStart);
					
				i=0;
				ContentBlocked=false;

				do // Resume processing if tag is too large
				{					
					while(i<(LONG)*pcbRead)
					{
						c=*pHtmlData;

						if(c==L'>' && prev1==L'-' && prev2==L'-') // Comment end
						{
							// Mask comment end to not interfere with content blocking
							if(InsideAdContent()) *(pHtmlData)=L' ';
							else if(state==STATE_TAG) *(pHtmlData-1)=L' ';
							if(state==STATE_REM) tag=TAG_REM_END;
						}

						switch(state)
						{
						case STATE_NEUTRAL:
							if(c=='<')
							{
								pTagStart=pHtmlData;
								oldstate=state;
								state=STATE_BRACKET;
							}
							else if(c==L'-' && prev1==L'-' && prev2==L'!' && prev3==L'<')
							{
								tag=0;
								oldstate=state;
								state=STATE_REM;
							}
							break;
						case STATE_BRACKET:
							if(isalpha(c) || c==L'/')
							{
								tag=0;
								bSkipValue=false;
								state=STATE_TAG;
							}
							else
							{
								pTagStart=NULL;
								state=STATE_NEUTRAL;
							}		
							break;			
						case STATE_TAG:
							if(tag==0)
							{
								if(c=='>' || isspace(c))
								{
									len=(int)(pHtmlData-pTagStart);
									tag=ParseTagName((void*)pTagStart,len,BlockAds,FILTER_SCOPE_STATIC,false);
									switch(tag)
									{
									case TAG_OTHER:
										pTagStart=NULL;
										oldstate=state;
										state=STATE_NEUTRAL;
										break;
									case TAG_SCRIPT:
										if(InsideScript()) tag=TAG_OTHER;
										InsideContent|=INSIDE_SCRIPT;
										break;
									case TAG_SCRIPT_END:
										InsideContent&=~INSIDE_SCRIPT;
										break;
									}
								}
							}
							else if(bSkipValue)
							{
								if(bQuotesPossible)
								{
									if(c=='\'')
									{
										oldstate=state;
										state=STATE_QUOTE1;
									}
									else if(c=='"')
									{
										oldstate=state;
										state=STATE_QUOTE2;
									}
									if(!isspace(c)) bQuotesPossible=false;
								}
								else if(isspace(c)) bSkipValue=false;
							}
							else if(c=='=')
							{
								bSkipValue=true;
								bQuotesPossible=true;
							}

							if(c=='>')
							{
								switch(tag)
								{
								case TAG_IMG:
									if(InsideContent==0)
									{
										len=(int)(pHtmlData-pTagStart)-3;
										pAttrStart=pTagStart+4;
										ImgWidth=ImgHeight=0;
										AttrParsed=0;
										do
										{
											attr_index=ParseAttribute((void**)&pAttrStart,&len,AttrImg,attr_buffer,false);
											switch(attr_index)
											{
											case 0: // src
												if((AttrParsed&ATTR_SRC)==0)
												{
													StringCbCopy(ContentURL,sizeof ContentURL,attr_buffer);
													AttrParsed|=ATTR_SRC;
												}
												break;
											case 1: // width
												if((AttrParsed&ATTR_WIDTH)==0)
												{
													if(StrToIntEx(attr_buffer,STIF_DEFAULT,&ImgWidth)) AttrParsed|=ATTR_WIDTH;
												}
												break;
											case 2: // height
												if((AttrParsed&ATTR_HEIGHT)==0)
													if(StrToIntEx(attr_buffer,STIF_DEFAULT,&ImgHeight)) AttrParsed|=ATTR_HEIGHT;
												break;
											}
										} while(attr_index!=-1 && AttrParsed!=(ATTR_SRC|ATTR_WIDTH|ATTR_HEIGHT));

										if(AttrParsed&ATTR_SRC)
										{
											if(m_pToolbar)
											{
												if(m_pToolbar->IsBannerAd(ContentURL,BaseURL,AttrParsed,ImgWidth,ImgHeight))
												{
													if(InsideContent==0)
													{
														OverwriteString(pTagStart,"<!--  ");
														OverwriteString(pHtmlData-5,"   -->");
													}
													QD(L"AddToBlockedContent 6");
													m_pToolbar->AddToBlockedContent(BLOCKED_CONTENT_BANNER,ContentURL,BaseURL,true);
												}
											}
										}
									}
									break;
								case TAG_EMBED:
									if(InsideContent==0 && m_pToolbar)
									{
										len=(int)(pHtmlData-pTagStart)-5;
										pAttrStart=pTagStart+6;
										AttrParsed=0;
										do
										{
											attr_index=ParseAttribute((void**)&pAttrStart,&len,AttrEmbed,attr_buffer,false);
											switch(attr_index)
											{
											case 0: // type
												if((AttrParsed&ATTR_TYPE)==0)
												{
													CharLower(attr_buffer);
													if(StrStr(attr_buffer,L"x-shockwave-flash")) AttrParsed|=ATTR_TYPE;
													else attr_index=-1;
												}
												break;
											case 1: // src
												if((AttrParsed&ATTR_SRC)==0)
												{
													StringCbCopy(ContentURL,sizeof ContentURL,attr_buffer);
													CharLower(attr_buffer);
													if(StrStr(attr_buffer,L".swf")) AttrParsed|=ATTR_TYPE; // Mark as flash type
													AttrParsed|=ATTR_SRC;
												}
												break;
											}
										} while(attr_index!=-1 && AttrParsed!=(ATTR_TYPE|ATTR_SRC));

										if(AttrParsed==(ATTR_TYPE|ATTR_SRC))
										{
											if(!m_pToolbar->IsVideoPlayerURL(ContentURL))
											{
												if(InsideContent==0)
												{
													OverwriteString(pTagStart,"<!--  ");
													OverwriteString(pHtmlData-5,"   -->");
												}
												QD(L"AddToBlockedContent 7");
												m_pToolbar->AddToBlockedContent(BLOCKED_CONTENT_FLASH,ContentURL,BaseURL,true);
											}
										}
									}
									break;
								/*
								case TAG_EMBED_END:
									if(InsideContent==0)
									{
										OverwriteString(pTagStart,"<!--");
										OverwriteString(pHtmlData-2,"-->");
									}
									break;
								*/
								case TAG_OBJECT:
									if(InsideContent==0)
									{
										len=(int)(pHtmlData-pTagStart)-6;
										pAttrStart=pTagStart+7;
										AttrParsed=0;
										do
										{
											attr_index=ParseAttribute((void**)&pAttrStart,&len,AttrObject,attr_buffer,false);
											switch(attr_index)
											{
											case 0: // classid
												if((AttrParsed&ATTR_CLASSID)==0)
												{
													CharUpper(attr_buffer);
													if(StrStr(attr_buffer,L"D27CDB6E-AE6D-11CF-96B8-444553540000")) AttrParsed|=ATTR_CLASSID;
												}
												break;
											case 1: // type
												if((AttrParsed&ATTR_TYPE)==0)
												{
													CharLower(attr_buffer);
													if(StrStr(attr_buffer,L"shockwave")) AttrParsed|=ATTR_TYPE;
												}
												break;
											case 2: // data
												if((AttrParsed&ATTR_DATA)==0)
												{
													StringCbCopy(ContentURL,sizeof ContentURL,attr_buffer);
													AttrParsed|=ATTR_DATA;
												}
												break;
											}
										} while(attr_index!=-1 && AttrParsed==0);

										if(AttrParsed&(ATTR_CLASSID|ATTR_TYPE))
										{
											if(InsideContent==0)
											{
												InsideContent=INSIDE_OBJECT;
												OverwriteString(pTagStart,"<!--   ");
												ContentBlocked=true;
											}
										}
									}
									break;
								case TAG_OBJECT_END:
									if(InsideContent==INSIDE_OBJECT)
									{
										OverwriteString(pHtmlData-2,"-->");
										InsideContent=0;
									}
									break;
								case TAG_PARAM:
									if(InsideContent==INSIDE_OBJECT && m_pToolbar)
									{
										len=(int)(pHtmlData-pTagStart)-5;
										pAttrStart=pTagStart+6;
										AttrParsed=0;									
										do
										{
											attr_index=ParseAttribute((void**)&pAttrStart,&len,AttrParam,attr_buffer,false);
											switch(attr_index)
											{
											case 0: // name
												if((AttrParsed&ATTR_NAME)==0)
												{
													if(StrCmpI(attr_buffer,L"movie") && StrCmpI(attr_buffer,L"src")) attr_index=-1;
													else AttrParsed|=ATTR_NAME;
												}
												break;
											case 1: // value
												if((AttrParsed&ATTR_VALUE)==0)
												{
													StringCbCopy(ContentURL,sizeof ContentURL,attr_buffer);
													AttrParsed|=ATTR_VALUE;
												}
												break;
											}
										} while(attr_index!=-1 && AttrParsed!=(ATTR_NAME|ATTR_VALUE));

										if(AttrParsed==(ATTR_NAME|ATTR_VALUE))
										{
											QD(L"AddToBlockedContent 8");
											m_pToolbar->AddToBlockedContent(BLOCKED_CONTENT_FLASH,ContentURL,BaseURL,true);
										}
									}
									break;
								case TAG_SCRIPT:
									if((BlockAds&ADBLOCKER_BLOCK_ADSCRIPT) && m_pToolbar)
									{
										len=(int)(pHtmlData-pTagStart)-6;
										pAttrStart=pTagStart+7;

										if(ParseAttribute((void**)&pAttrStart,&len,AttrScript,attr_buffer,false)==0)
										{
											CharLower(attr_buffer);
											if(m_pToolbar->IsAdURL(attr_buffer,BaseURL,ISADURL_CONTEXT_SCRIPT))
											{
												if(InsideContent==INSIDE_SCRIPT)
												{
													OverwriteString(pTagStart,"<!--");
													InsideContent|=INSIDE_ADSCRIPT;
												}
												QD(L"AddToBlockedContent 9");
												m_pToolbar->AddToBlockedContent(BLOCKED_CONTENT_ADSCRIPT,attr_buffer,NULL,true);
											}
										}
									}
									break;
								case TAG_SCRIPT_END:
									if(InsideContent&INSIDE_ADSCRIPT)
									{
										OverwriteString(pHtmlData-2,"-->");
										InsideContent=0;
									}
									break;
								case TAG_BASE:
									if(InsideContent==0)
									{
										len=(int)(pHtmlData-pTagStart)-4;
										pAttrStart=pTagStart+5;
										AttrParsed=0;

										if(ParseAttribute((void**)&pAttrStart,&len,AttrBase,attr_buffer,false)==0)
										{
											StringCbCopy(BaseURL,sizeof BaseURL,attr_buffer);
											CQToolbar::trim(BaseURL);
										}
									}
									break;
								}

								pTagStart=NULL;
								oldstate=state;
								state=STATE_NEUTRAL;
							}
							break;
						case STATE_QUOTE1:
							if(c=='\'')	state=oldstate;
							break;
						case STATE_QUOTE2:
							if(c=='"')	state=oldstate;
							break;
						case STATE_REM:
							if(tag==TAG_REM_END) state=oldstate;
							break;
						}

						prev3=prev2;
						prev2=prev1;
						prev1=c;

						pHtmlData++;
						i++;
					}
				
					// Resume processing if last tag is too large
					bTagTooLarge=false;
					if(pTagStart)
					{
						len=(int)(pHtmlData-pTagStart);
						if(len>=TAG_SIZE)
						{
							QDEBUG_PRINTA(L"CQueroFilter: tag too large",pTagStart);

							if(NotInsideScript())
							{
								OverwriteString(pTagStart,"<xxx ");
								ContentBlocked=true;
							}
							pHtmlData=pTagStart+5;
							pTagStart=NULL;

							i-=len-5; // i can become negative if tag start is part of a previous read operation

							state=STATE_NEUTRAL;
							prev1=0;
							prev2=0;
							prev3=0;
							bTagTooLarge=true;
						}
					}
				
				} while(bTagTooLarge);

				 // No more data available?

				if(hr==S_FALSE)
				{
					pTagStart=NULL;
					state=STATE_NEUTRAL;
				}

				// Prepare for next read call

				if(state==STATE_NEUTRAL || pTagStart==NULL)
				{
					if(cbBuffer>cb)
					{						
						if(pTagStart)
						{					
							QDEBUG_PRINTA(L"CQueroFilter: state=neutral, pTagStart!=0",pTagStart);
							QDEBUG_PRINTF(L"CQueroFilter",L"hr=%x; oldstate=%d, cbBuffer=%d",hr,oldstate,cbBuffer);
						}
						*pcbRead=cb;
						hr=S_OK;
					}
					else
					{
						pTagStart=NULL;
						*pcbRead=cbBuffer;
					}
				}
				else // Last tag incomplete
				{
					len=(int)(pTagStart-buffer); // Length of data preceding current tag

					if(len>=(int)cb)
					{
						*pcbRead=cb;
						hr=S_OK;
					}
					else
					{
						*pcbRead=len;
						hr=E_PENDING;
					}
				}
			
				// Copy buffer to caller buffer and move remaining data to start of buffer
				if(*pcbRead)
				{
					CopyMemory(pv,buffer,*pcbRead);

					MoveMemory(buffer,buffer+*pcbRead,cbBuffer-*pcbRead);

					cbBuffer-=*pcbRead;

					if(pTagStart) pTagStart-=*pcbRead;
				}

				if(ContentBlocked && m_pToolbar) m_pToolbar->SetContentBlocked();

				//*((char*)pv+*pcbRead)=0;

				//MultiByteToWideChar(CP_ACP,0,(char*)pv,-1,text,10000);
				//StringCbPrintf(text,sizeof text,L"%d %d",cb,*pcbRead);
				//MessageBox(NULL,text,L"buffer",MB_OK);

			} // End read succeeded

		} // End Ad Blocker enabled
		else hr=m_pIncomingProt->Read(pv,cb,pcbRead); // Do not modify content if Ad Blocker is not enabled

	} // End m_pIncomingProt valid
	else hr=E_FAIL;

	return hr;
}

STDMETHODIMP CQueroFilter::Seek(LARGE_INTEGER dlibMove,DWORD dwOrigin,ULARGE_INTEGER *plibNewPosition)
{
	return E_NOTIMPL;
}

STDMETHODIMP CQueroFilter::LockRequest(DWORD dwOptions)
{
	return S_OK;
}

STDMETHODIMP CQueroFilter::UnlockRequest()
{
	return S_OK;
}

/*
// pInbuff points to a buffer of bytes, not characters, and should always have 1 extra character's worth
HRESULT CQueroBand::ParseHTML(BYTE* pInBuff, ULONG cbBufferSize)
{
	HRESULT hr = E_FAIL;

	if (NULL == pInBuff)
	{
		_ASSERTE(false);
		return E_INVALIDARG;
	}
	
	TCHAR* pbuff = (TCHAR*)pInBuff;
	ULONG cchBuffer = cbBufferSize;

	// Conversion from whatever is in buffer (we don't know if it is uni or not) to TCHAR

	// TODO: Obviously our conversion of data here is very simplistic - coding an improvement
	// over A2T and W2T conversion routines of the data would be a nice exercise for the 
	// exuberant MIME filter developer

	if (m_IProt_bIncomingIsUnicode)
	{
		// Prot Handler split last unicode word in middle
		if (cbBufferSize & 0x1)
		{
			m_IProt_bSplitUniWord = true;
			m_IProt_byteSplit = pInBuff[cbBufferSize-1];
			cchBuffer--;
			pInBuff[cbBufferSize-1] = 0;
			pInBuff[cbBufferSize] = 0;
		}
		else
		{
			pInBuff[cbBufferSize] = 0;
			pInBuff[cbBufferSize+1] = 0;
		}

		cchBuffer = cchBuffer / sizeof(TCHAR);
		pbuff = W2T((LPWSTR)pbuff);

	}
	else
	{
		pbuff=new TCHAR[2000];
		pInBuff[cbBufferSize] = 0;
		MessageBeep(-1);
		MultiByteToWideChar(CP_ACP,0,(char*)pInBuff,-1,pbuff,2000);
		//pbuff = A2T((LPSTR)pbuff);
	}

	// TODO: We should also do some MIME sniffing tests to verify that what we
	//		 actually have is text data.. or look for an XML DTD PI or something like that

	bool bDone = false;
	TCHAR* pCur = pbuff;
	TCHAR* pNewCur = NULL;


	TCHAR* strCharSet = _T("<>&\r");

	bDone = false;
	TCHAR* pEnd = pbuff + cchBuffer;
	while (!bDone && (pCur < pEnd))
	{
		pNewCur = _tcspbrk(pCur, strCharSet);

		if (NULL == pNewCur)
		{
			m_pHTMLBuffer->WriteString(pCur, pEnd - pCur);
			bDone = true;
		}
		else
		{
			// Write out part up to <>&..
			if (pNewCur > pCur)
				m_pHTMLBuffer->WriteString(pCur, pNewCur - pCur); // no Null Terminator
			switch (*pNewCur)
			{
			case _T('<'):
				*m_pHTMLBuffer << _T("&lt;");
				break;

			case _T('>'):
				*m_pHTMLBuffer << _T("&gt;");
				break;

			case _T('&'):
				*m_pHTMLBuffer << _T("&amp;");
				break;

			case _T('\r'):
				*m_pHTMLBuffer << _T("\r<BR>");
				break;

			default:
				_ASSERTE(false); // should never hit this!
				break;
			}

			if (0 == *pCur)
				bDone = true;

			pCur = pNewCur + 1;
		}		
	}

	return S_OK;
}
*/

/////////////////////////////////////////////////////////////////////////////
// IInternetProtocolSink interface
STDMETHODIMP CQueroFilter::Switch(PROTOCOLDATA __RPC_FAR *pProtocolData)
{
	return m_pOutgoingProtSink?m_pOutgoingProtSink->Switch(pProtocolData):E_FAIL;
}

STDMETHODIMP CQueroFilter::ReportProgress(ULONG ulStatusCode, LPCWSTR szStatusText)
{
	return m_pOutgoingProtSink?m_pOutgoingProtSink->ReportProgress(ulStatusCode, szStatusText):E_FAIL;
}

STDMETHODIMP CQueroFilter::ReportData(DWORD grfBSCF, ULONG ulProgress, ULONG ulProgressMax)
{
	return m_pOutgoingProtSink?m_pOutgoingProtSink->ReportData(grfBSCF, ulProgress, ulProgressMax):E_FAIL;
}

STDMETHODIMP CQueroFilter::ReportResult(HRESULT hrResult, DWORD dwError, LPCWSTR szResult)
{
	return m_pOutgoingProtSink?m_pOutgoingProtSink->ReportResult(hrResult, dwError, szResult):E_FAIL;
}

void OverwriteString(char *pDest,char *pSrc)
{
	char ch;

	ch=*pSrc;
	while(ch)
	{
		*pDest=ch;
		pDest++;
		pSrc++;
		ch=*pSrc;
	}
}

int ParseTagName(void *pTagStart,int Len,DWORD BlockAds,BYTE FilterScope,bool IsUnicode)
{
	TCHAR ch;
	BYTE *pTagA;
	TCHAR *pTagW;
	int TagNameLen;
	bool IsEndTag;
	int i,j;
	int result;

	result=TAG_OTHER;
	IsEndTag=false;
	
	if(IsUnicode)
	{
		pTagW=(TCHAR *)pTagStart;
		pTagW++; // Skip <
		if(*pTagW==L'/')
		{
			IsEndTag=true;
			pTagW++;
		}
	}
	else
	{
		pTagA=(BYTE*)pTagStart;
		pTagA++; // Skip <
		if(*pTagA=='/')
		{
			IsEndTag=true;
			pTagA++;
		}
	}

	TagNameLen=Len-1;
	if(IsEndTag) TagNameLen--;
		
	i=0;
	while(i<N_TAGNAMES && TagNameLen>=TagNames[i].Length)
	{
		if(TagNames[i].Length==TagNameLen && (FilterScope&TagNames[i].FilterScope))
		{
			// Compare tag name
			j=0;
			while(j<TagNameLen)
			{
				ch=IsUnicode?pTagW[j]:pTagA[j];
				if(ch<=L'Z' && ch>=L'A') ch+=L'a'-L'A'; // Convert to lower case
				if(ch!=TagNames[i].pTagName[j]) break;

				j++;
			}
			// Tag name found?
			if(j==TagNameLen)
			{
				if(TagNames[i].BlockAds==0 || (TagNames[i].BlockAds&BlockAds))
				{
					if(IsEndTag)
					{
						if(TagNames[i].TagId_End!=0) result=TagNames[i].TagId_End;
					}
					else result=TagNames[i].TagId_Start;
				}
				break;
			}
		}

		i++;
	} // End iterating through TagNames

	return result;
}

int ParseAttribute(void **Tag,int *TagLen,const TCHAR **Attributes,TCHAR *Value,bool IsUnicode)
{
	int state;
	bool bNameRead;
	TCHAR ch;
	TCHAR delimiter;

	TCHAR AttrName[MAX_ATTR_LEN];

	BYTE *pTagA;
	TCHAR *pTagW;

	int iAttrName,iValue;
	int iParsed;
	state=STATE_NEUTRAL;
	
	if(IsUnicode) pTagW=(TCHAR *)*Tag;
	else pTagA=(BYTE *)*Tag;

	// Parse attributes
	while(*TagLen>0)
	{
		ch=IsUnicode?*pTagW:*pTagA;

		//TCHAR text[2];text[0]=ch;text[1]=0;MessageBox(NULL,text,L"ch",MB_OK);

		if(ch==L'>' || _istspace(ch)) ch=L' ';

		switch(state)
		{
		case STATE_NEUTRAL:
			if(ch==L'=') state=STATE_SKIPVALUE;
			else if(ch!=L' ')
			{
				AttrName[0]=ch;
				iAttrName=1;
				bNameRead=false;
				state=STATE_ATTRNAME;
			}
			break;
		case STATE_ATTRNAME:
			if(ch==L'=')
			{
				AttrName[iAttrName]=0;
				iValue=0;
				delimiter=0;
				iParsed=0;
				while(Attributes[iParsed])
				{
					if(!StrCmpI(Attributes[iParsed],AttrName)) break;
					iParsed++;
				}
				if(Attributes[iParsed]) state=STATE_ATTRVALUE;
				else state=STATE_SKIPVALUE;
			}
			else if(ch==L' ') bNameRead=true;
			else
			{
				if(bNameRead) // New attribute
				{
					AttrName[0]=ch;
					iAttrName=1;
					bNameRead=false;
				}
				else
				{
					if(iAttrName<MAX_ATTR_LEN-1) AttrName[iAttrName++]=ch;
				}
			}
			break;
		case STATE_SKIPVALUE:
			if(iValue || delimiter)
			{
				if(ch==delimiter || ch==L' ')
				{
					state=STATE_NEUTRAL;
				}
			}
			else
			{
				if(ch==L'"' || ch==L'\'') delimiter=ch;
				else if(ch!=L' ') iValue=1;
			}
			break;
		case STATE_ATTRVALUE:
			if(iValue || delimiter)
			{
				if(ch==delimiter || ch==L' ')
				{
					Value[iValue]=0;
					DecodeHtmlEntities(Value);
					*Tag=IsUnicode?(void*)(pTagW+1):(void*)(pTagA+1);
					(*TagLen)--;
					return iParsed;
				}
				else
				{
					if(iValue<TAG_SIZE-1) Value[iValue++]=ch;
				}
			}
			else
			{
				if(ch==L'"' || ch==L'\'') delimiter=ch;
				else if(ch!=L' ') Value[iValue++]=ch; // First character, no buffer checks necessary
			}
			break;
		}

		if(IsUnicode) pTagW++;
		else pTagA++;
		(*TagLen)--;
	}

	*Tag=IsUnicode?(void*)pTagW:(void*)pTagA;

	return -1;
}

#define MAX_HTML_ENTITY_LENGTH 9

void DecodeHtmlEntities(TCHAR *pHtml)
{
	TCHAR ch,next,digitch;
	DWORD cp;
	TCHAR *pWriteBack;
	TCHAR Entity[MAX_HTML_ENTITY_LENGTH+1];
	int EntityLen;
	bool bInsideEntity;
	bool IsHexadecimal;
	bool bEntitiyDecoded;
	int i,j;
	int digit,base;

	bInsideEntity=false;
	pWriteBack=pHtml;
	ch=*pHtml;
	while(ch)
	{
		pHtml++;
		next=*pHtml;

		if(bInsideEntity)
		{
			Entity[EntityLen++]=ch;

			if(EntityLen<MAX_HTML_ENTITY_LENGTH)
			{
				if(_istpunct(next) || _istspace(next) || next==L'\0')
				{
					bEntitiyDecoded=false;
					Entity[EntityLen]=0;
					if(Entity[1]==L'#')
					{
						// Parse numeric entity
						if(Entity[2]==L'x' || Entity[2]==L'X')
						{
							IsHexadecimal=true;
							i=3;
							base=16;
						}
						else
						{
							IsHexadecimal=false;
							i=2;
							base=10;
						}
						cp=0;
						while(i<EntityLen)
						{
							digitch=Entity[i];
						
							if(digitch>=L'0' && digitch<=L'9') digit=digitch-L'0';
							else
							{
								if(IsHexadecimal)
								{	
									if(digitch>=L'a' && digitch<=L'f') digit=digitch-L'a'+10;
									else if(digitch>=L'A' && digitch<=L'F') digit=digitch-L'A'+10;
									else break;
								}
								else break;
							}

							cp=(cp*base)+digit;
							i++;
						}

						if(cp)
						{
							TCHAR *pEntity;

							// Write codepoint
							pEntity=Entity;
							CIDNA::EncodeUTF16(cp,&pEntity);
							// Append unprocessed part of the entity
							j=IsUTF16BMP(cp)?1:2;
							while(i<EntityLen) Entity[j++]=Entity[i++];
							EntityLen=j;
							bEntitiyDecoded=true;
						}
					}
					else
					{
						// Parse named entity
						if(!StrCmpI((TCHAR*)Entity+1,L"amp"))
						{
							EntityLen=1;
							bEntitiyDecoded=true;
						}
					}					

					// Skip delimiter
					if(next==L';' && bEntitiyDecoded)
					{
						pHtml++;
						next=*pHtml;
					}
					
					bInsideEntity=false;
				} // End _istpunct(next) || next==L'\0'
			} // End EntityLen<=MAX_HTML_ENTITY_LENGTH
			else
			{
				bInsideEntity=false;
			}

			// Write processed entity back
			if(bInsideEntity==false) for(i=0;i<EntityLen;i++,pWriteBack++) *pWriteBack=Entity[i];
		}
		else // bInsideEntity == false
		{
			if(ch==L'&' && next)
			{
				bInsideEntity=true;
				Entity[0]=L'&';
				EntityLen=1;
			}
			else
			{
				*pWriteBack=ch;
				pWriteBack++;
			}
		}

		ch=next;
	}

	*pWriteBack=L'\0';
}