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
// ContentFilter.cpp: implementation of the CContentFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ContentFilter.h"
#include "QToolbar.h"
#include "QueroBand.h"

extern CContentFilter g_ContentFilter;
extern FP_HTMLDOCUMENT2WRITE ORIG_HTMLDOCUMENT2WRITE;
extern FP_HTMLDOCUMENT2WRITELN ORIG_HTMLDOCUMENT2WRITELN;
extern TagNameIds TagNames[N_TAGNAMES];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CContentFilter::CContentFilter()
{
	FilterStateIndex=0;

	for(int i=0;i<MAX_CONCURRENT_WRITES;i++)
	{
		FilterStates[i].pUnknown=NULL;
		FilterStates[i].BaseURL=NULL;
	}
}

CContentFilter::~CContentFilter()
{
	FreeFilterStates(NULL);
}

void CContentFilter::FreeFilterStates(CQToolbar *pToolbar)
{
	FilterState *pFilterState=FilterStates;

	for(int i=0;i<MAX_CONCURRENT_WRITES;i++)
	{
		if(pToolbar==NULL || (pFilterState->pUnknown && pFilterState->pToolbar==pToolbar))
		{
			pFilterState->pUnknown=NULL;
			if(pFilterState->BaseURL)
			{
				SysFreeString(pFilterState->BaseURL);
				pFilterState->BaseURL=NULL;
			}
		}
		pFilterState++;
	}

	if(pToolbar==NULL) FilterStateIndex=0;
}

bool CContentFilter::FilterContent(FilterState *pState,BSTR pText)
{
	TCHAR c;
	TCHAR prev1,prev2,prev3;
	TCHAR attr_buffer[TAG_SIZE];
	TCHAR ContentURL[MAXURLLENGTH];

	IHTMLDocument2 *pHtmlDocument;
	TCHAR *pHtmlData;
	TCHAR *pWriteBack;
	TCHAR *pTagStart;
	TCHAR *pDivStart;
	TCHAR *pBlockStart;
	TCHAR *pElement;
	TCHAR *pAttrStart;
	int attr_index;
	int AttrParsed;
	int ImgWidth,ImgHeight;

	int len,attr_len;
	int tag;
	bool bSkipValue;
	bool bQuotesPossible;
	int state,oldstate;
	UINT InsideContent;
	UINT BlockAds;
	bool ContentBlocked=false;
	bool WriteToBuffer;
	int DivLevel;
	int cchBuffer;
	bool finished;
	bool PassInput=true;
	bool bContainsScriptEnd=false;

	// Load State
	pHtmlDocument=pState->pHtmlDocument;
	BlockAds=pState->BlockAds;
	prev1=pState->prev1;
	prev2=pState->prev2;
	prev3=pState->prev3;
	state=pState->State;
	oldstate=pState->OldState;
	tag=pState->Tag;
	bSkipValue=pState->bSkipValue;
	bQuotesPossible=pState->bQuotesPossible;
	InsideContent=pState->InsideContent;
	pTagStart=pState->pTagStart;
	pBlockStart=pState->pBlockStart;
	pDivStart=pState->pDivStart;
	DivLevel=pState->DivLevel;
	cchBuffer=pState->cchBuffer;
	if(cchBuffer && pState->WriteToBuffer)
	{
		pWriteBack=(pState->WriteBuffer)+cchBuffer;
		WriteToBuffer=true;
	}
	else
	{
		pWriteBack=NULL;
		if(InsideContent&(INSIDE_OBJECT|INSIDE_ADSCRIPT|INSIDE_IFRAME)) pBlockStart=pText;
		WriteToBuffer=false;
	}

	pHtmlData=pText;

	c=*pHtmlData;

	if(pState->IsWriteln)
	{
		finished=false;
		if(c==0) c=L'\n';
	}
	else finished=(c==0);

	while(finished==false)
	{
		if(pWriteBack)
		{
			*pWriteBack=c;
			pWriteBack++;
		}

		// Buffer overflow handling

		if(WriteToBuffer)
		{
			cchBuffer++;			

			if(cchBuffer>=TAG_SIZE-1) // Taking terminating null and newline for writeln into account
			{
				CComBSTR *ResumeText;
				TCHAR *pStart;

				if(InsideContent&INSIDE_DIV)
				{
					if(pTagStart)
					{
						// Tag inside suspicious div too large: output div to tag start, resume processing incomplete tag

						QDEBUG_PRINT(L"tag too large: pDivStart && pTagStart",pDivStart);

						ResumeText=new CComBSTR(SysStringLen(pText)+TAG_SIZE);
						if(ResumeText)
						{
							ResumeText->Empty();

							*pWriteBack=0;

							if(pBlockStart && pBlockStart<pTagStart) pStart=pBlockStart;
							else pStart=pTagStart;
					
							ResumeText->Append(pStart);
							if(*pHtmlData) ResumeText->Append(pHtmlData+1);

							*pStart=0;

							pState->prev1=0;
							pState->prev2=0;
							pState->prev3=0;
							pState->State=STATE_NEUTRAL;
							pState->OldState=STATE_NEUTRAL;
							pState->Tag=0;
							pState->InsideContent=InsideContent&(~INSIDE_DIV);
							pState->pTagStart=NULL;
							pState->pBlockStart=NULL;
							pState->pDivStart=NULL;
							pState->DivLevel=0;
							pState->cchBuffer=(UINT)(pStart-pState->WriteBuffer);
							pState->WriteToBuffer=false;
						
							if(FilterContent(pState,ResumeText->m_str))
								OrigDocumentWrite(pHtmlDocument,ResumeText->m_str,pState->IsWriteln);

							delete ResumeText;
						}
						return false;
					}
					else
					{
						// Suspicious div too large: ignore div

						QDEBUG_PRINT(L"tag too large: pDivStart && !pTagStart",pDivStart);

						pDivStart=NULL;
						DivLevel=0;
						InsideContent&=~INSIDE_DIV;

						if(pBlockStart)
						{
							cchBuffer-=(int)(pWriteBack-pBlockStart);
							pWriteBack=pBlockStart;
							pBlockStart=pText;
						}
						*pWriteBack=0;
						pWriteBack=pText;
						WriteToBuffer=false;						
					}
				}
				else if(pTagStart) // Suspicious tag too large: Overwrite tag and resume processing
				{
					OverwriteString(pTagStart,L"<xxx ");
					ContentBlocked=true;

					ResumeText=new CComBSTR(SysStringLen(pText)+TAG_SIZE);
					if(ResumeText)
					{
						ResumeText->Empty();

						*pWriteBack=0;						
				
						ResumeText->Append(pTagStart);
						if(*pHtmlData) ResumeText->Append(pHtmlData+1);

						pState->prev1=0;
						pState->prev2=0;
						pState->prev3=0;
						pState->State=STATE_NEUTRAL;
						pState->OldState=STATE_NEUTRAL;
						pState->Tag=0;
						pState->InsideContent=InsideContent;
						pState->pTagStart=NULL;
						pState->pBlockStart=NULL;
						pState->pDivStart=pDivStart;
						pState->DivLevel=DivLevel;
						pState->cchBuffer=0;
						pState->WriteToBuffer=false;

						QDEBUG_PRINT(L"tag too large: pTagStart",pTagStart);
						
						if(FilterContent(pState,ResumeText->m_str))
							OrigDocumentWrite(pHtmlDocument,ResumeText->m_str,pState->IsWriteln);

						delete ResumeText;						
					}
					return false;
				}
			}
		}

		// Main processing loop

		switch(state)
		{
		case STATE_NEUTRAL:
			if(c==L'<')
			{
				pTagStart=pWriteBack?pWriteBack-1:pHtmlData;
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
			if(_istalpha(c) || c==L'/')
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
				if(c==L'>' || _istspace(c))
				{
					if(pWriteBack) len=(int)(pWriteBack-pTagStart)-1;
					else len=(int)(pHtmlData-pTagStart);

					tag=ParseTagName((void*)pTagStart,len,BlockAds,FILTER_SCOPE_DYNAMIC,true);
					switch(tag)
					{
					case TAG_OTHER:
						if(c!=L'>')
						{
							pTagStart=NULL;
							oldstate=state;
							state=STATE_NEUTRAL;
							if(WriteToBuffer && pDivStart==NULL)
							{
								if(pBlockStart)
								{
									cchBuffer-=(int)(pWriteBack-pBlockStart);					
									pWriteBack=pBlockStart;
									pBlockStart=pText;
								}
								*pWriteBack=0;
								pWriteBack=pText;
								WriteToBuffer=false;
							}
						}
						break;
					case TAG_DIV:
						if(InsideContent&INSIDE_DIV) DivLevel++;
						break;
					case TAG_DIV_END:
						// Test if DIV element contains HTML tags (allow gmail "Loading ..." messages)
						if(InsideContent==INSIDE_DIV)
						{
							DivLevel--;
							if(pDivStart && DivLevel==0)
							{
								pElement=pDivStart+4; // Skip "<DIV"
								while(pElement<pTagStart)
								{
									if(*pElement==L'<')
									{
										break;
										//pElement++;
										//if(!StrCmpN(pElement,L"xxx",3) || !StrCmpNI(pElement,L"IMG",3)) break; // Block DIV only if it contains blocked content or images
									}
									else pElement++;
								}
								if(pElement>=pTagStart)
								{
									InsideContent=0;
									pDivStart=NULL;
								}
							}
						}
						break;
					case TAG_SCRIPT:
						if(InsideScript()) tag=TAG_OTHER;
						InsideContent|=INSIDE_SCRIPT;
						break;
					case TAG_SCRIPT_END:
						InsideContent&=~INSIDE_SCRIPT;
						bContainsScriptEnd=true;
						break;
					}
				}
			}
			else if(bSkipValue)
			{
				if(bQuotesPossible)
				{
					if(c==L'\'')
					{
						oldstate=state;
						state=STATE_QUOTE1;
					}
					else if(c==L'"')
					{
						oldstate=state;
						state=STATE_QUOTE2;
					}
					if(!_istspace(c)) bQuotesPossible=false;
				}
				else if(_istspace(c)) bSkipValue=false;
			}
			else if(c==L'=')
			{
				bSkipValue=true;
				bQuotesPossible=true;
			}

			if(c==L'>')
			{
				if(pWriteBack) len=(int)(pWriteBack-pTagStart)-1;
				else len=(int)(pHtmlData-pTagStart);
				if(len>TAG_SIZE-1) len=TAG_SIZE-1;

				switch(tag)
				{
				case TAG_IMG:
					if((InsideContent&(~INSIDE_DIV))==0)
					{
						attr_len=len-3;
						pAttrStart=pTagStart+4;
						ImgWidth=ImgHeight=0;
						AttrParsed=0;
						do
						{
							attr_index=ParseAttribute((void**)&pAttrStart,&attr_len,AttrImg,attr_buffer,true);
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
									if(StrToIntEx(attr_buffer,STIF_DEFAULT,&ImgWidth)) AttrParsed|=ATTR_WIDTH;
								break;
							case 2: // height
								if((AttrParsed&ATTR_HEIGHT)==0)
									if(StrToIntEx(attr_buffer,STIF_DEFAULT,&ImgHeight)) AttrParsed|=ATTR_HEIGHT;
								break;
							}
						} while(attr_index!=-1 && AttrParsed!=(ATTR_SRC|ATTR_WIDTH|ATTR_HEIGHT));

						if(AttrParsed&ATTR_SRC)
						{
							if(pState->pToolbar)
							{
								if(pState->pToolbar->IsBannerAd(ContentURL,pState->BaseURL,AttrParsed,ImgWidth,ImgHeight))
								{
									pWriteBack=pTagStart;
									if(WriteToBuffer) cchBuffer-=len+1;

									QD(L"AddToBlockedContent 1");
									pState->pToolbar->AddToBlockedContent(BLOCKED_CONTENT_BANNER,ContentURL,pState->BaseURL,false);
								}
							}
						}
					}
					break;
				case TAG_DIV:
					if(InsideContent==0)
					{
						if(len>TAG_SIZE-1) len=TAG_SIZE-1;
						StringCchCopyN(attr_buffer,TAG_SIZE,pTagStart,len);
						CharLower(attr_buffer);
						if(StrStr(attr_buffer,L"position") && (StrStr(attr_buffer,L"absolute") || StrStr(attr_buffer,L"fixed")))
						{
							//MessageBox(NULL,pDivStart,L"inside div",MB_OK);
							InsideContent=INSIDE_DIV;
							pDivStart=pTagStart;
							DivLevel=1;
						}
					}
					break;
				case TAG_DIV_END:
					if(InsideContent==INSIDE_DIV && DivLevel==0)
					{
						if(pState->pToolbar)
						{
							if(pWriteBack) len=(int)(pWriteBack-pDivStart);
							else len=(int)(pHtmlData-pDivStart)+1;
							StringCchCopyN(ContentURL,MAXURLLENGTH,pDivStart,len);
							QD(L"AddToBlockedContent 2");
							pState->pToolbar->AddToBlockedContent(BLOCKED_CONTENT_DIV,ContentURL,pState->BaseURL,false);
							QDEBUG_PRINT(L"DIV blocked",ContentURL);
							
							if(WriteToBuffer) cchBuffer-=(int)(pWriteBack-pDivStart);
							pWriteBack=pDivStart;
						}
						QDEBUG_CODE else QDEBUG_PRINT(L"FilterContent",L"DIV not blocked (pToolbar==NULL)!");
						InsideContent=0;
						pDivStart=NULL;
					}
					break;
				case TAG_IFRAME:
					if((InsideContent&(~INSIDE_DIV))==0)
					{
						attr_len=len-6;
						pAttrStart=pTagStart+7;
						if(ParseAttribute((void**)&pAttrStart,&attr_len,AttrIFrame,attr_buffer,true)==0)
						{
							StringCbCopy(ContentURL,sizeof ContentURL,attr_buffer);
							
							if(pState->pToolbar)
							{
								if(pState->pToolbar->IsAdURL(ContentURL,NULL,ISADURL_CONTEXT_IFRAME))
								{
									if(pBlockStart==NULL) pBlockStart=pTagStart;
									pWriteBack=pTagStart;
									if(WriteToBuffer) cchBuffer-=len+1;
									InsideContent|=INSIDE_IFRAME;
									ContentBlocked=true;

									QD(L"AddToBlockedContent 3");
									pState->pToolbar->AddToBlockedContent(BLOCKED_CONTENT_IFRAME,ContentURL,pState->BaseURL,false);
									QDEBUG_PRINT(L"IFRAME blocked",ContentURL);
								}
							}

							QDEBUG_PRINT(L"IFRAME URL",ContentURL);
							QDEBUG_PRINT(L"IFRAME Tag",pTagStart);
						}
					}
					break;
				case TAG_IFRAME_END:
					if(InsideContent&INSIDE_IFRAME)
					{
						if(WriteToBuffer) cchBuffer-=(int)(pWriteBack-pBlockStart);
						pWriteBack=pBlockStart;
						pBlockStart=NULL;
						InsideContent&=~INSIDE_IFRAME;
					}
					break;
				case TAG_EMBED:
					if((InsideContent&(~(INSIDE_DIV|INSIDE_OBJECT)))==0 && pState->pToolbar)
					{
						attr_len=len-5;
						pAttrStart=pTagStart+6;
						AttrParsed=0;
						do
						{
							attr_index=ParseAttribute((void**)&pAttrStart,&attr_len,AttrEmbed,attr_buffer,true);
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
							// Do not filter Flash-based video player
							if(!pState->pToolbar->IsVideoPlayerURL(ContentURL))
							{
								// Do not filter if user has explicitly open a flash movie (i.e entered the URL, followed a link or clicked on a blocked content item)
								if(StrCmp(pState->pToolbar->currentURL,ContentURL))
								{
									pWriteBack=pTagStart;
									if(WriteToBuffer) cchBuffer-=len+1;
									//OverwriteString(pTagStart,L"<!--");
									//if(InsideContent==0) OverwriteString(pWriteBack?pWriteBack-3:pHtmlData-2,L"-->",;
									QD(L"AddToBlockedContent 4");
									pState->pToolbar->AddToBlockedContent(BLOCKED_CONTENT_FLASH,ContentURL,pState->BaseURL,false);				
									//MessageBox(NULL,ContentURL,L"embed src FilterFlash",MB_OK);
								}
							}
						}
					}
					break;
				/*
				case TAG_EMBED_END:
					if(InsideContent==0)
					{
						pWriteBack=pTagStart;
						if(WriteToBuffer) cchBuffer-=len+1;
						//OverwriteString(pTagStart,L"<!--");
						//OverwriteString(pWriteBack?pWriteBack-3:pHtmlData-2,L"-->");
					}
					break;
				*/
				case TAG_OBJECT:
					if((InsideContent&(~INSIDE_DIV))==0 && pState->pToolbar)
					{
						attr_len=len-6;
						pAttrStart=pTagStart+7;
						AttrParsed=0;
						do
						{
							attr_index=ParseAttribute((void**)&pAttrStart,&attr_len,AttrObject,attr_buffer,true);
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

						QDEBUG_PRINT(L"TAG_OBJECT",pTagStart);

						if(AttrParsed&(ATTR_CLASSID|ATTR_TYPE))
						{
							if((AttrParsed&ATTR_DATA)==0 || !pState->pToolbar->IsVideoPlayerURL(ContentURL))
							{
								QDEBUG_PRINT(L"inside object",pTagStart);
								if(pBlockStart==NULL) pBlockStart=pTagStart;
								//pWriteBack=pTagStart;
								//if(WriteToBuffer) cchBuffer-=len+1;
								InsideContent|=INSIDE_OBJECT;
								//OverwriteString(pTagStart,L"<!--   ");
								ContentBlocked=true;
							}
						}
					}
					else QDEBUG_PRINT(L"TAG_OBJECT inside div",pTagStart);
					break;
				case TAG_OBJECT_END:
					if(InsideContent&INSIDE_OBJECT)
					{
						if(WriteToBuffer) cchBuffer-=(int)(pWriteBack-pBlockStart);
						pWriteBack=pBlockStart;
						pBlockStart=NULL;
						//OverwriteString(pWriteBack?pWriteBack-3:pHtmlData-2,L"-->");
						InsideContent&=~INSIDE_OBJECT;
					}
					break;
				case TAG_PARAM:
					if((InsideContent&INSIDE_OBJECT) && pState->pToolbar)
					{
						attr_len=len-5;
						pAttrStart=pTagStart+6;
						AttrParsed=0;
						do
						{
							attr_index=ParseAttribute((void**)&pAttrStart,&attr_len,AttrParam,attr_buffer,true);
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
							if(pState->pToolbar->IsVideoPlayerURL(ContentURL))
							{
								pBlockStart=NULL;
								InsideContent&=~INSIDE_OBJECT;
							}
							else
							{
								QD(L"AddToBlockedContent 5");
								pState->pToolbar->AddToBlockedContent(BLOCKED_CONTENT_FLASH,ContentURL,pState->BaseURL,false);
								//MessageBox(NULL,ContentURL,L"param FilterFlash",MB_OK);
							}
						}
					}
					break;
				case TAG_SCRIPT:
					if((BlockAds&ADBLOCKER_BLOCK_ADSCRIPT) && pState->pToolbar)
					{
						attr_len=len-6;
						pAttrStart=pTagStart+7;

						if(ParseAttribute((void**)&pAttrStart,&attr_len,AttrScript,attr_buffer,true)==0)
						{
							CharLower(attr_buffer);
							if(pState->pToolbar->IsAdURL(attr_buffer,pState->BaseURL,ISADURL_CONTEXT_SCRIPT))
							{
								if((InsideContent&(INSIDE_OBJECT|INSIDE_IFRAME))==0)
								{
									if(pBlockStart==NULL) pBlockStart=pTagStart;
									InsideContent|=INSIDE_ADSCRIPT;
								}
								QD(L"AddToBlockedContent 5");
								pState->pToolbar->AddToBlockedContent(BLOCKED_CONTENT_ADSCRIPT,attr_buffer,pState->BaseURL,false);
							}

							InsideContent|=INSIDE_EXTERNAL_SCRIPT;
						}
					}
					break;
				case TAG_SCRIPT_END:
					if(InsideContent&INSIDE_ADSCRIPT)
					{
						if(WriteToBuffer) cchBuffer-=(int)(pWriteBack-pBlockStart);
						pWriteBack=pBlockStart;
						pBlockStart=NULL;
					}
					InsideContent&=~(INSIDE_ADSCRIPT|INSIDE_EXTERNAL_SCRIPT);
					break;
				}

				pTagStart=NULL;
				oldstate=state;
				state=STATE_NEUTRAL;
				if(WriteToBuffer && pDivStart==NULL)
				{
					if(pBlockStart)
					{
						cchBuffer-=(int)(pWriteBack-pBlockStart);
						pWriteBack=pBlockStart;
						pBlockStart=pText;
					}
					*pWriteBack=0;
					pWriteBack=pText;
					WriteToBuffer=false;
				}
			}
			break;
		case STATE_QUOTE1:
			if(c==L'\'') state=oldstate;
			break;
		case STATE_QUOTE2:
			if(c==L'"')	state=oldstate;
			break;
		case STATE_REM:
			if(c==L'>' && prev1==L'-' && prev2==L'-') state=oldstate;
			break;
		}

		prev3=prev2;
		prev2=prev1;
		prev1=c;

		pHtmlData++;

		// If writeln is called insert one loop iteration with c==L'\n' at the end of the string

		if(pState->IsWriteln)
		{
			if(*(pHtmlData-1))
			{
				c=*pHtmlData;
				if(c==0) c=L'\n';
			}
			else c=0;
		}
		else c=*pHtmlData;

		finished=(c==0);

		// Check if incomplete tag/div exceeds the buffer size

		if(finished && WriteToBuffer==false && pState->IsPutHTML==false)
		{
			if(InsideContent&INSIDE_DIV)
			{
				if(pWriteBack) len=(int)(pWriteBack-pDivStart);
				else len=(int)(pHtmlData-pDivStart);
				
				if(len>=TAG_SIZE)
				{
					// Ignore div
					pDivStart=NULL;
					DivLevel=0;
					InsideContent&=~INSIDE_DIV;
				}
			}
			
			if(state!=STATE_NEUTRAL && pTagStart)
			{
				if(pWriteBack) len=(int)(pWriteBack-pTagStart);
				else len=(int)(pHtmlData-pTagStart);

				if(len>=TAG_SIZE)
				{
					// Overwrite tag and resume processing after tag name
					OverwriteString(pTagStart,L"<xxx ");
					ContentBlocked=true;

					if(pWriteBack)
					{
						TerminateBSTR(pText,pWriteBack);
						pWriteBack=NULL;
					}
					pHtmlData=pTagStart+5;
					c=*pHtmlData;

					prev1=prev2=prev3=0;
					state=STATE_NEUTRAL;
					oldstate=STATE_NEUTRAL;
					tag=0;
					pTagStart=NULL;
					
					finished=false;
				}
			}
		}
	}

	// Adjust State for the next call

	CComBSTR OutputText;

	// Workaround: quit SCRIPT to workaround the following case where XXX would be output in the wrong context
	// <script language="JavaScript"><!--document.write( XXX );//--></script><script language="JavaScript">
	if(InsideScript() && NotInsideAdContent() && bContainsScriptEnd)
	{
		InsideContent&=~(INSIDE_SCRIPT|INSIDE_EXTERNAL_SCRIPT);
		QDEBUG_PRINTF(L"FilterContent",L"assuming SCRIPT end");
	}

	// Workaround: quit HTML remark to workaround the case where the remark end tag is outside the script
	// <script language="JavaScript"><!--document.write('<'+'!-- ');//--></script> XXX <!--/TopList COUNTER--> ZZZ
	if(state==STATE_REM && InsideContent==0)
	{
		state=STATE_NEUTRAL;
		QDEBUG_PRINTF(L"FilterContent",L"assuming HTML remark end");
	}

	if(pBlockStart && state==STATE_NEUTRAL)
	{
		if(WriteToBuffer) cchBuffer-=(int)(pWriteBack-pBlockStart);
		pWriteBack=pBlockStart;
	}

	// Defer output
	if(WriteToBuffer)
	{
		// Terminate string
		if(pWriteBack) *pWriteBack=0;
		PassInput=false; // Output nothing
	}
	else
	{
		// Terminate BSTR
		if(pWriteBack) TerminateBSTR(pText,pWriteBack);

		// Copy buffer to OutputText
		if(cchBuffer)
		{
			OutputText.Append(pState->WriteBuffer);
			cchBuffer=0;
		}

		// Defer output of incomplete tag/div
		if(((state!=STATE_NEUTRAL && pTagStart) || pDivStart) && pState->IsPutHTML==false)
		{
			TCHAR *pResume;

			pResume=pDivStart?pDivStart:pTagStart;
			if(pWriteBack) len=(int)(pWriteBack-pResume);
			else len=(int)(pHtmlData-pResume);
			StringCchCopyN(pState->WriteBuffer,TAG_SIZE,pResume,len);		
			
			// Add trailing newline if writeln is called and write back is turned off
			if(pState->IsWriteln && !pWriteBack) StringCchCat(pState->WriteBuffer,TAG_SIZE,L"\n");

			WriteToBuffer=true;
			cchBuffer=len;
			TerminateBSTR(pText,pResume);
			
			if(pDivStart)
			{
				if(pTagStart) pTagStart=pState->WriteBuffer+(pTagStart-pDivStart);
				if(pBlockStart) pBlockStart=pState->WriteBuffer+(pBlockStart-pDivStart);
				pDivStart=pState->WriteBuffer;
			}
			else
			{
				pTagStart=pState->WriteBuffer;
				if(pBlockStart)
				{
					TerminateBSTR(pText,pBlockStart);
					pBlockStart=pTagStart;
				}
			}

			if(*pText)
			{
				pState->IsWriteln=false;
			}
			else PassInput=false;

			//MessageBox(NULL,pState->WriteBuffer,L"incomplete",MB_OK);
		}

		// Output buffer and new text with one call to OrigDocumentWrite only
		if(OutputText.Length())
		{
			OutputText.Append(pText);
			QDEBUG_PRINT(L"OutputText",OutputText.m_str);
			OrigDocumentWrite(pHtmlDocument,OutputText.m_str,pState->IsWriteln);
			PassInput=false;
		}

	} // End WriteToBufffer==false

	// Set ContentBlocked
	if(ContentBlocked && pState->pToolbar) pState->pToolbar->SetContentBlocked();

	// Save State
	pState->prev1=prev1;
	pState->prev2=prev2;
	pState->prev3=prev3;
	pState->State=state;
	pState->OldState=oldstate;
	pState->Tag=tag;
	pState->bSkipValue=bSkipValue;
	pState->bQuotesPossible=bQuotesPossible;
	pState->InsideContent=InsideContent;
	pState->pTagStart=pTagStart;
	pState->pBlockStart=pBlockStart;
	pState->pDivStart=pDivStart;
	pState->DivLevel=DivLevel;
	pState->cchBuffer=cchBuffer;
	pState->WriteToBuffer=WriteToBuffer;

	//Sleep(100);
	//MessageBox(NULL,pText,L"write",MB_OK);
	QDEBUG_PRINTF(L"FilterContent",L"%s %d",pText,SysStringLen(pText));
	return PassInput;
}

void CContentFilter::OrigDocumentWrite(IHTMLDocument2 *pHtmlDocument,BSTR bstr,bool IsWriteln)
{
	SAFEARRAY *sfArray;
	VARIANT *vData;

	// Creates a new one-dimensional array
	sfArray=SafeArrayCreateVector(VT_VARIANT,0,1);
	if(sfArray)
	{
		if(SUCCEEDED_OK(SafeArrayAccessData(sfArray,(LPVOID*) &vData)))
		{
			vData->vt=VT_BSTR;
			vData->bstrVal=bstr;
			
			SafeArrayUnaccessData(sfArray);

			//MessageBox(NULL,vData->bstrVal,L"write",MB_OK);
			if(IsWriteln)
				ORIG_HTMLDOCUMENT2WRITELN(pHtmlDocument,sfArray);
			else
				ORIG_HTMLDOCUMENT2WRITE(pHtmlDocument,sfArray);

			// Assure that bstr is not freed when freeing the SafeArrray; orignially calling SafeArrayDestroyDescriptor unexpectedly freed the bstr
			if(SUCCEEDED_OK(SafeArrayAccessData(sfArray,(LPVOID*) &vData)))
			{
				vData->vt=VT_EMPTY;
				SafeArrayUnaccessData(sfArray);
			}
		}

		SafeArrayDestroy(sfArray); // Originally used SafeArrayDestroyDescriptor
	}
}

IUnknown* CContentFilter::GetDocumentRoot(IHTMLDocument2 __RPC_FAR* pHtmlDocument)
{
	IHTMLDocument3 *pHtmlDocument3=NULL;
	IHTMLElement *pRootElement=NULL;
	IUnknown *pUnknown=NULL;
	HRESULT hr;
	
	hr=pHtmlDocument->QueryInterface(IID_IHTMLDocument3,(LPVOID*)&pHtmlDocument3);
	if(SUCCEEDED_OK(hr) && pHtmlDocument3)
	{
		hr=pHtmlDocument3->get_documentElement(&pRootElement);
		if(SUCCEEDED_OK(hr) && pRootElement)
		{
			hr=pRootElement->QueryInterface(IID_IUnknown,(LPVOID*)&pUnknown);
			if(SUCCEEDED_OK(hr) && pUnknown) pUnknown->Release();

			pRootElement->Release();
		}

		pHtmlDocument3->Release();
	}

	return pUnknown;
}

BSTR CContentFilter::GetDocumentURL(IUnknown *pUnknown,BYTE Type)
{
	BSTR bstrURL;
	IDispatch *pDispatch;
	IHTMLDocument2 *pHtmlDocument2;
	IHTMLElement *pHtmlElement;
	HRESULT hr;

	bstrURL=NULL;

	switch(Type)
	{
	case GETDOCUMENTURL_IHTMLDOCUMENT2:
		pHtmlDocument2=NULL;
		hr=pUnknown->QueryInterface(IID_IHTMLDocument2,(LPVOID*)&pHtmlDocument2);
		if(SUCCEEDED_OK(hr) && pHtmlDocument2)
		{
			if(pHtmlDocument2->get_URL(&bstrURL)!=S_OK) bstrURL=NULL;
			pHtmlDocument2->Release();
		}
		// WIN32 if(((IHTMLDocument2*)pUnknown)->get_URL(&bstrURL)!=S_OK) bstrURL=NULL;
		break;

	case GETDOCUMENTURL_IHTMLELEMENT:
		pDispatch=NULL;
		pHtmlDocument2=NULL;

		pHtmlElement=NULL;
		hr=pUnknown->QueryInterface(IID_IHTMLElement,(LPVOID*)&pHtmlElement);
		if(SUCCEEDED_OK(hr) && pHtmlElement)
		{
			hr=pHtmlElement->get_document(&pDispatch);
			pHtmlElement->Release();
		}
		// WIN32 hr=((IHTMLElement*)(pUnknown))->get_document(&pDispatch);

		if(SUCCEEDED_OK(hr) && pDispatch)
		{
			hr=pDispatch->QueryInterface(IID_IHTMLDocument2,(LPVOID*)&pHtmlDocument2);
			if(SUCCEEDED_OK(hr) && pHtmlDocument2)
			{
				if(pHtmlDocument2->get_URL(&bstrURL)!=S_OK) bstrURL=NULL;
				pHtmlDocument2->Release();
			}
			pDispatch->Release();
		}
		break;
	}

	//if(bstrURL) QDEBUG_PRINT(L"GetDocumentURL",bstrURL);
	return bstrURL;
}

bool CContentFilter::FilterDocumentWrite(IHTMLDocument2 __RPC_FAR* pHtmlDocument,SAFEARRAY *psarray,bool IsWriteln)
{
	IUnknown *pDocumentRoot=NULL;
	IUnknown *pUnknown=NULL;

	// QDEBUG_PRINTF(L"FilterDocumentWrite",L"thread [%x]",GetCurrentThreadId());

	bool PassInput=true;
	VARIANT FAR *rgvar;

	if(SUCCEEDED_OK(SafeArrayAccessData(psarray, (void HUGEP **) &rgvar)))
	{
		UINT n=psarray->rgsabound->cElements;

		if(rgvar && n)
		{
			FilterState *pFilterState;
			int i;
			UINT k;

			// Obtain the IUnknown interface pointer of the document's root element to identify the document

			pDocumentRoot=GetDocumentRoot(pHtmlDocument);

			//TCHAR text[255];
			//StringCbPrintf(text,255,L"pUnkown %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",(LONG)pUnknown,*((LONG*)pUnknown),*((LONG*)pUnknown+1),*((LONG*)pUnknown+2),*((LONG*)pUnknown+3),*((LONG*)pUnknown+4),*((LONG*)pUnknown+5),*((LONG*)pUnknown+6),*((LONG*)pUnknown+7),*((LONG*)pUnknown+8),*((LONG*)pUnknown+9),*((LONG*)pUnknown+10),*((LONG*)pUnknown+11),*((LONG*)pUnknown+12),*((LONG*)pUnknown+13));
			//MessageBox(NULL,text,rgvar[0].bstrVal,MB_OK);

			//pUnknown=(IUnknown *)*((LONG*)pHtmlDocument+3); // Use third pointer to identfiy document

			if(WaitForSingleObject(g_hQSharedDataMutex,QMUTEX_TIMEOUT)==WAIT_OBJECT_0)
			{
				if(pDocumentRoot) pUnknown=pDocumentRoot;
				else
				{
					// If the root element is undefined use the IUnkown pointer of the HtmlDocument to identify the document
					if(SUCCEEDED_OK(pHtmlDocument->QueryInterface(IID_IUnknown,(LPVOID*)&pUnknown))) pUnknown->Release();					
				}

				if(pUnknown)
				{
					i=0;
					pFilterState=g_ContentFilter.FilterStates;
					while(i<MAX_CONCURRENT_WRITES)
					{
						if(pFilterState->pUnknown==pUnknown) break;
						pFilterState++;
						i++;
					}
				}
				else i=MAX_CONCURRENT_WRITES;

				//i=MAX_CONCURRENT_WRITES;
				if(i>=MAX_CONCURRENT_WRITES)
				{
					pFilterState=&g_ContentFilter.FilterStates[g_ContentFilter.FilterStateIndex];
					g_ContentFilter.FilterStateIndex++;
					if(g_ContentFilter.FilterStateIndex>=MAX_CONCURRENT_WRITES) g_ContentFilter.FilterStateIndex=0;

					//pUnknown->AddRef(); // Keep reference, without reference recognition of previous document is not reliable

					pFilterState->BaseURL=GetDocumentURL(pHtmlDocument,GETDOCUMENTURL_IHTMLDOCUMENT2);
					
					pFilterState->pUnknown=pDocumentRoot;
					pFilterState->prev1=0;
					pFilterState->prev2=0;
					pFilterState->prev3=0;
					pFilterState->State=STATE_NEUTRAL;
					pFilterState->OldState=STATE_NEUTRAL;
					pFilterState->pTagStart=0;
					pFilterState->pBlockStart=0;
					pFilterState->pDivStart=0;
					pFilterState->DivLevel=0;
					pFilterState->IsPutHTML=false;
					
					pFilterState->cchBuffer=0;
					pFilterState->WriteToBuffer=false;
					pFilterState->Tag=0;
					pFilterState->InsideContent=0;

					// Find associated Toolbar instance

					QThreadData* QueroInstance;

					pFilterState->pToolbar=NULL;
					QueroInstance=GetCurrentQueroInstance();
					if(QueroInstance) pFilterState->pToolbar=QueroInstance->pToolbar;
					QDEBUG_CODE else QDEBUG_PRINT(L"FilterDocumentWrite",L"GetCurrentQueroInstance failed!");

					// Determine effective BlockAds settings for current URL

					if(pFilterState->pToolbar) pFilterState->BlockAds=pFilterState->pToolbar->GetWhiteListBlockAds(NULL);
					else pFilterState->BlockAds=g_BlockAds;

					QDEBUG_PRINT(L"new document",rgvar[0].vt==VT_BSTR?rgvar[0].bstrVal:L"not VT_BSTR!");
					//QDEBUG_PRINTF(rgvar[0].bstrVal,L"pUnkown %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",(LONG)pUnknown,*((LONG*)pUnknown),*((LONG*)pUnknown+1),*((LONG*)pUnknown+2),*((LONG*)pUnknown+3),*((LONG*)pUnknown+4),*((LONG*)pUnknown+5),*((LONG*)pUnknown+6),*((LONG*)pUnknown+7),*((LONG*)pUnknown+8),*((LONG*)pUnknown+9),*((LONG*)pUnknown+10),*((LONG*)pUnknown+11),*((LONG*)pUnknown+12),*((LONG*)pUnknown+13));
				}

				pFilterState->pHtmlDocument=pHtmlDocument;
				pFilterState->IsWriteln=IsWriteln;

				if(pFilterState->BlockAds&ADBLOCKER_Enable)
				{			
					if(n>1)
					{
						CComBSTR *CombinedInput;
						UINT CombinedLen;

						CombinedLen=0;
						k=0;

						// Convert variant type to BSTR and count lenghts
						while(k<n)
						{
							if(rgvar[k].vt!=VT_BSTR)
								if(VariantChangeTypeEx(&rgvar[k],&rgvar[k],0x409,VARIANT_NOUSEROVERRIDE|VARIANT_ALPHABOOL,VT_BSTR)!=S_OK)
									continue;

							CombinedLen+=SysStringLen(rgvar[k].bstrVal);
							k++;
						}

						// Combine array into one BSTR
						CombinedInput=new CComBSTR(CombinedLen+1);
						if(CombinedInput)
						{
							CombinedInput->Empty();
							k=0;
							while(k<n)
							{
								if(rgvar[k].vt==VT_BSTR) CombinedInput->AppendBSTR(rgvar[k].bstrVal);
								k++;
							}
							QDEBUG_PRINT(L"combined write",CombinedInput->m_str);
							PassInput=FilterContent(pFilterState,CombinedInput->m_str);
					
							// Output Content
							if(PassInput)
							{
								OrigDocumentWrite(pHtmlDocument,CombinedInput->m_str,pFilterState->IsWriteln);							
								PassInput=false;
							}
							delete CombinedInput;
						}
					}
					else
					{
						// Convert variant type to BSTR (locale: 0x409 EN-US)
						if(rgvar[0].vt==VT_BSTR)
							PassInput=FilterContent(pFilterState,rgvar[0].bstrVal);
						else
						{
							if(VariantChangeTypeEx(&rgvar[0],&rgvar[0],0x409,VARIANT_NOUSEROVERRIDE|VARIANT_ALPHABOOL,VT_BSTR)==S_OK)
								PassInput=FilterContent(pFilterState,rgvar[0].bstrVal);
						}

						// Output Content if IsWriteln changed from true to false or root document undefined
						if(PassInput)
						{
							if((IsWriteln==true && pFilterState->IsWriteln==false) || pDocumentRoot==NULL)
							{
								//MessageBox(NULL,rgvar[0].bstrVal,L"output writeln via write",MB_OK);
								OrigDocumentWrite(pHtmlDocument,rgvar[0].bstrVal,pFilterState->IsWriteln);
								PassInput=false;
							}

						}
						//MessageBox(NULL,rgvar[0].bstrVal,L"single output",MB_OK);
					}

					// Try to get the document's root element after the content is written
					if(pDocumentRoot==NULL)
					{
						pFilterState->pUnknown=GetDocumentRoot(pHtmlDocument);
						if(pFilterState->pUnknown==NULL)
						{
							QDEBUG_PRINT(L"Failed acquiring root element",rgvar[0].bstrVal);
							// Use the IUnkown pointer of the HtmlDocument to identify the document if the root element is still not available
							pFilterState->pUnknown=pUnknown;
						}
					}
				}

				ReleaseMutex(g_hQSharedDataMutex);
			}
			QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"FilterDocumentWrite");
			//if(PassInput) MessageBox(NULL,rgvar[0].bstrVal,pFilterState->WriteBuffer,MB_OK);
		}

		SafeArrayUnaccessData(psarray);
	}

	return PassInput;
}

bool CContentFilter::FilterTag(IUnknown *pUnknown,BSTR pHtml,bool IsCreateElement)
{
	bool PassInput=true;
	UINT i;
	UINT length;

	// QDEBUG_PRINTF(L"FilterTag",L"thread [%x]",GetCurrentThreadId());

	if(WaitForSingleObject(g_hQSharedDataMutex,QMUTEX_TIMEOUT)==WAIT_OBJECT_0)
	{
		FilterState State;

		ZeroMemory(&State,sizeof State);
		State.IsPutHTML=true;

		// Get base URL

		State.BaseURL=GetDocumentURL(pUnknown,IsCreateElement?GETDOCUMENTURL_IHTMLDOCUMENT2:GETDOCUMENTURL_IHTMLELEMENT);

		// Find associated Toolbar instance

		QThreadData* QueroInstance;

		QueroInstance=GetCurrentQueroInstance();
		if(QueroInstance) State.pToolbar=QueroInstance->pToolbar;
		QDEBUG_CODE else QDEBUG_PRINT(L"FilterTag",L"GetCurrentQueroInstance failed!");

		// Determine actual BlockAds settings for current URL

		if(State.pToolbar) State.BlockAds=State.pToolbar->GetWhiteListBlockAds(NULL);
		else State.BlockAds=g_BlockAds;

		if(State.BlockAds&ADBLOCKER_Enable)
		{
			if(IsCreateElement)
			{
				length=SysStringLen(pHtml);
				i=0;
				while(i<N_TAGNAMES)
				{
					if(	(TagNames[i].FilterScope&FILTER_SCOPE_CREATEELEMENT) &&
						(TagNames[i].BlockAds&State.BlockAds) &&
						(TagNames[i].Length==length) && !StrCmpI(pHtml,TagNames[i].pTagName)	)
					{
						PassInput=false;
						break;
					}
					i++;
				}
			}
			
			if(PassInput) PassInput=FilterContent(&State,pHtml);
			else
			{
				TerminateBSTR(pHtml,pHtml);
				if(State.pToolbar) State.pToolbar->SetContentBlocked();
			}
		}
		ReleaseMutex(g_hQSharedDataMutex);
	}
	QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"FilterTag");

	return PassInput;
}

void CContentFilter::OverwriteString(TCHAR *pDest,TCHAR *pSrc)
{
	TCHAR ch;

	ch=*pSrc;
	while(ch)
	{
		*pDest=ch;
		pDest++;
		pSrc++;
		ch=*pSrc;
	}
}

void CContentFilter::TerminateBSTR(BSTR bstr, TCHAR *pTerminatePosition)
{
	UINT oldByteLength;
	SIZE_T newByteLength;

	if(bstr<=pTerminatePosition)
	{
		newByteLength=((BYTE*)pTerminatePosition-(BYTE*)bstr);
		oldByteLength=(*(((UINT*)bstr)-1)); // Get SysStringByteLen

		if(newByteLength<oldByteLength)
		{
			// Set new BSTR length in bytes
			*(((UINT*)bstr)-1)=(UINT)newByteLength;
			*pTerminatePosition=0;
		}
	}
}