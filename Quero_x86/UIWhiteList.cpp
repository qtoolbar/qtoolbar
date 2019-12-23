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
// UIWhiteList.cpp: implementation of the CUIWhiteList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIWhiteList.h"

const int WhiteListOptions[N_WHITELIST_OPTIONS]={WL_ALLOW_FLASH,WL_ALLOW_BANNER,WL_ALLOW_ADSCRIPT,WL_ALLOW_DIV,WL_ALLOW_IFRAME,WL_ALLOW_POPUPS,WL_ALLOW_IDN,WL_ALLOW_MISSGLYPHS};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIWhiteList::CUIWhiteList()
{
	bProcessNotifications=true;
}

CUIWhiteList::~CUIWhiteList()
{
}

LRESULT CUIWhiteList::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT i;

	m_Domain = GetDlgItem(IDC_DOMAIN);
	m_WhiteList = GetDlgItem(IDC_WHITELIST);

	m_WhiteList.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	
	#ifdef COMPILE_FOR_WINDOWS_VISTA
	SetWindowTheme(m_WhiteList.m_hWnd,L"explorer",NULL);
	#endif

	const UINT ColumnNames[1+N_WHITELIST_OPTIONS]={IDS_WL_SITE,IDS_WL_FLASH,IDS_WL_BANNER,IDS_WL_ADSCRIPT,IDS_WL_DIV,IDS_WL_IFRAME,IDS_WL_POPUPS,IDS_WL_IDN,IDS_WL_GLYPHS};

	for(i=0;i<(1+N_WHITELIST_OPTIONS);i++) m_WhiteList.InsertColumn(i,m_pToolbar->GetString(ColumnNames[i]),LVCFMT_LEFT,i?60:120,-1);

	m_pToolbar->SyncWhiteList();

	if(WaitForSingleObject(g_hQSharedListMutex,QMUTEX_TIMEOUT)==WAIT_OBJECT_0)
	{
		int index;

		i=0;
		while(i<g_WhiteListIndex)
		{
			index=m_WhiteList.InsertItem(i,g_WhiteList[i].Pattern);
			SetAllowSubItems(index,g_WhiteList[i].Permits);

			i++;
		}

		if(MatchCurrentURL)
		{
			index=m_pToolbar->GetWhiteListIndex(false);
			if(index==-1)
			{
				TCHAR CoreDomain[MAXURLLENGTH];

				m_pToolbar->CopyCurrentCoreDomain(CoreDomain);
				m_Domain.SetWindowText(CoreDomain);
			}
			else
			{
				m_Domain.SetWindowText(g_WhiteList[index].Pattern);
				SetAllowCheckboxes(g_WhiteList[index].Permits);
			}
		}

		ReleaseMutex(g_hQSharedListMutex);
	}
	QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"OnInitDialog");

	// Init DialogResize
	DlgResize_Init();

	return 1; // Let the system set the focus
}

void CUIWhiteList::SetAllowSubItems(int index,SHORT permits)
{
	UINT ids;
	int i;

	i=0;
	while(i<N_WHITELIST_OPTIONS)
	{
		ids=(permits&WhiteListOptions[i])?IDS_WL_ALLOW:IDS_WL_DEFAULT;
		i++;
		m_WhiteList.SetItemText(index,i,m_pToolbar->GetString(ids));
	}
}

void CUIWhiteList::SetAllowCheckboxes(SHORT permits)
{
	m_Allow_IDN=(permits&WL_ALLOW_IDN)!=0;
	m_Allow_MissGlyphs=(permits&WL_ALLOW_MISSGLYPHS)!=0;
	m_Allow_PopUps=(permits&WL_ALLOW_POPUPS)!=0;
	m_Allow_Flash=(permits&WL_ALLOW_FLASH)!=0;
	m_Allow_Banner=(permits&WL_ALLOW_BANNER)!=0;
	m_Allow_AdScript=(permits&WL_ALLOW_ADSCRIPT)!=0;
	m_Allow_DIV=(permits&WL_ALLOW_DIV)!=0;
	m_Allow_IFRAME=(permits&WL_ALLOW_IFRAME)!=0;

	m_SelectAll=(permits&WL_ALLOW_ALL)==WL_ALLOW_ALL;

	DoDataExchange(FALSE);
}

SHORT CUIWhiteList::GetSelectedPermits()
{
	SHORT permits;

	DoDataExchange(TRUE);

	permits=0;
	if(m_Allow_IDN) permits|=WL_ALLOW_IDN;
	if(m_Allow_MissGlyphs) permits|=WL_ALLOW_MISSGLYPHS;
	if(m_Allow_PopUps) permits|=WL_ALLOW_POPUPS;
	if(m_Allow_Flash) permits|=WL_ALLOW_FLASH;
	if(m_Allow_Banner) permits|=WL_ALLOW_BANNER;
	if(m_Allow_AdScript) permits|=WL_ALLOW_ADSCRIPT;
	if(m_Allow_DIV) permits|=WL_ALLOW_DIV;
	if(m_Allow_IFRAME) permits|=WL_ALLOW_IFRAME;

	return permits;
}

bool CUIWhiteList::GetPattern(TCHAR pattern[MAXURLLENGTH])
{
	CIDNA idna;
	int status;
	size_t url_len;
	int hoststartidx,hostendidx;

	m_Domain.GetWindowText(pattern,MAXURLLENGTH);
	m_pToolbar->trim(pattern);
	CharLower(pattern);
	url_len=MAXURLLENGTH;
	status=idna.URLToAscii(pattern,&url_len,NULL,NULL,NULL);
	if((status&IDNA_ILLEGAL)==0)
	{
		url_len=MAXURLLENGTH;
		status=idna.URLToUnicode(pattern,&url_len,&hoststartidx,&hostendidx,NULL);
		if(hoststartidx || hostendidx!=(int)(url_len)) return false;
	}

	return (status&IDNA_ILLEGAL)==0;
}

SHORT CUIWhiteList::GetWhiteListPermits(TCHAR pattern[MAXURLLENGTH])
{
	SHORT Permits=0;

	if(WaitForSingleObject(g_hQSharedListMutex,QMUTEX_TIMEOUT)==WAIT_OBJECT_0)
	{
		UINT i;

		i=0;
		while(i<g_WhiteListIndex)
		{
			if(g_WhiteList[i].Pattern && !StrCmp(g_WhiteList[i].Pattern,pattern))
			{
				Permits=g_WhiteList[i].Permits;
				break;
			}
			
			i++;
		}

		ReleaseMutex(g_hQSharedListMutex);
	}
	QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"GetWhieListPermits");

	return Permits;
}

LRESULT CUIWhiteList::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TCHAR Pattern[MAXURLLENGTH];
	int lvIndex;
	SHORT permits;
	UINT ErrorMsgId=0;

	switch(LOWORD(wParam))
	{
	case IDC_ADD:
		if(GetPattern(Pattern))
		{
			permits=GetSelectedPermits();
			lvIndex=m_pToolbar->AddToWhiteList(Pattern,permits,false);
			
			if(lvIndex!=-1)
			{
				LVFINDINFO fi;

				fi.flags=LVFI_STRING;
				fi.psz=Pattern;

				lvIndex=m_WhiteList.FindItem(&fi,-1);

				if(lvIndex==-1)	lvIndex=m_WhiteList.InsertItem(m_WhiteList.GetItemCount(),Pattern);

				m_Domain.SetWindowText(Pattern);
				SetAllowSubItems(lvIndex,permits);

				// Select the entry in the list view
				bProcessNotifications=false;
				m_WhiteList.SetItemState(lvIndex,LVNI_SELECTED|LVNI_FOCUSED,LVNI_SELECTED|LVNI_FOCUSED);
				m_WhiteList.EnsureVisible(lvIndex,FALSE);
				m_WhiteList.SetFocus();
				bProcessNotifications=true;
			}
		}
		else ErrorMsgId=IDS_ERR_WL_PATTERNINVALID;
		break;
	case IDC_DELETE:
		if(GetPattern(Pattern))
		{
			lvIndex=m_pToolbar->DeleteFromWhiteList(Pattern);
			
			if(lvIndex!=-1)
			{
				LVFINDINFO fi;

				fi.flags=LVFI_STRING;
				fi.psz=Pattern;

				lvIndex=m_WhiteList.FindItem(&fi,-1);
				if(lvIndex!=-1) m_WhiteList.DeleteItem(lvIndex);

				m_Domain.SetWindowText(L"");
				SetAllowCheckboxes(0);
			}
			else ErrorMsgId=IDS_ERR_WL_PATTERNNOTFOUND;
		}
		else ErrorMsgId=IDS_ERR_WL_PATTERNINVALID;
		break;
	case IDC_SELECT_ALL:
		if(HIWORD(wParam)==BN_CLICKED)
		{
			DoDataExchange(TRUE);
			SetAllowCheckboxes(m_SelectAll?~0:0);
		}
		break;
	case IDC_RESET:
		if(MessageBox(m_pToolbar->GetString(IDS_WL_CONFIRM_RESET),L"Quero Toolbar",MB_ICONWARNING|MB_YESNO)==IDYES)
		{
			m_pToolbar->ResetWhiteList();
			m_WhiteList.DeleteAllItems();
		}
		break;
	case IDCANCEL:
		EndDialog(IDCANCEL);
		break;
	default:
		if(HIWORD(wParam)==BN_CLICKED)
		{
			DoDataExchange(TRUE);
			m_SelectAll=false;
			DoDataExchange(FALSE);
		}
		break;
	}

	if(ErrorMsgId)
	{
		MessageBox(m_pToolbar->GetString(ErrorMsgId),L"Quero Toolbar",MB_OK|MB_ICONWARNING);
		m_Domain.SetSel(0,-1);
		m_Domain.SetFocus();
	}

	return 0;
}

LRESULT CUIWhiteList::OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TCHAR Pattern[MAXURLLENGTH];

	if(bProcessNotifications && lpnm->hwndFrom==m_WhiteList.m_hWnd)
	{
		if((lpnm->code)==NM_SETFOCUS)
		{
			int lvIndex;

			lvIndex=m_WhiteList.GetNextItem(-1,LVNI_SELECTED);
			if(lvIndex!=-1)
			{
				m_WhiteList.GetItemText(lvIndex,0,Pattern,sizeof Pattern);
				m_Domain.SetWindowText(Pattern);

				SetAllowCheckboxes(GetWhiteListPermits(Pattern));
			}
			else
			{
				if(m_WhiteList.GetItemCount())
					m_WhiteList.SetItemState(0,LVNI_SELECTED|LVNI_FOCUSED,LVNI_SELECTED|LVNI_FOCUSED);
			}
		}
		else if((lpnmLV->hdr.code)==LVN_ITEMCHANGED)
		{
			if((lpnmLV->uNewState)&LVIS_SELECTED)
			{
				m_WhiteList.GetItemText(lpnmLV->iItem,0,Pattern,sizeof Pattern);
				m_Domain.SetWindowText(Pattern);
				SetAllowCheckboxes(GetWhiteListPermits(Pattern));
			}
		}
		if((lpnmLV->hdr.code)==LVN_KEYDOWN )
		{
			if(lpnmLVKeyDown->wVKey==VK_DELETE)
			{
				if(m_WhiteList.GetNextItem(-1,LVNI_SELECTED)!=-1) PostMessage(WM_COMMAND,IDC_DELETE);
			}
		}
	}
	
	return 0;
}