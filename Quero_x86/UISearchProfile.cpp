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
// UISearchProfile.cpp: implementation of the CUISearchProfile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UISearchProfile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUISearchProfile::CUISearchProfile(Profile *pProfile,UINT WindowTitleId,CQToolbar *pToolbar)
{
	m_pProfile=pProfile;
	m_pToolbar=pToolbar;
	
	this->WindowTitleId=WindowTitleId;
}

CUISearchProfile::~CUISearchProfile()
{
}

LRESULT CUISearchProfile::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_pToolbar->CenterDialog(m_hWnd);

	if(WindowTitleId) SetWindowText(m_pToolbar->GetString(WindowTitleId));
	
	//DoDataExchange(FALSE);

	m_Edit_Name=GetDlgItem(IDC_PROFILE_NAME);
	m_Edit_LuckyURL=GetDlgItem(IDC_PROFILE_LUCKYSEARCH);

	m_Edit_Name.LimitText(MAX_PROFILE_NAME_LEN-1);
	m_Edit_LuckyURL.LimitText(MAXURLLENGTH-1);

	if(m_pProfile)
	{
		if(m_pProfile->Name) m_Edit_Name.SetWindowText(m_pProfile->Name);
		if(m_pProfile->LuckySearch.QueryURL) m_Edit_LuckyURL.SetWindowText(m_pProfile->LuckySearch.QueryURL);
		else m_Edit_LuckyURL.SetWindowText(PROFILE_DEFAULT_LUCKYURL);
	}

	// Init DialogResize
	DlgResize_Init();

	return 1; // Let the system set the focus
}

LRESULT CUISearchProfile::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch(LOWORD(wParam))
	{
	case IDOK:
		//DoDataExchange(TRUE);
		if(SaveProfile()) EndDialog(IDOK);
		break;
	
	case IDCANCEL:
		EndDialog(IDCANCEL);
		break;
	}

	switch(HIWORD(wParam))
	{
	case EN_KILLFOCUS:
		::SendMessage((HWND)lParam,EM_SETSEL,0,0);
		break;
	}

	return 0;
}

bool CUISearchProfile::SaveProfile()
{
	BSTR bstrText;
	BSTR bstrName;
	BSTR bstrLuckyURL;

	bool result=false;

	bstrName=NULL;
	bstrLuckyURL=NULL;

	bstrText=NULL;
	if(m_Edit_Name.GetWindowText(bstrText) && bstrText)
	{
		CQToolbar::trim(bstrText);
		if(bstrText[0]) bstrName=SysAllocString(bstrText);
		SysFreeString(bstrText);
	}

	bstrText=NULL;
	if(m_Edit_LuckyURL.GetWindowText(bstrText) && bstrText)
	{
		CQToolbar::trim(bstrText);
		if(bstrText[0]) bstrLuckyURL=SysAllocString(bstrText);
		SysFreeString(bstrText);
	}

	if(bstrName)
	{
		if((m_pProfile->Flags&PROFILE_CUSTOM)==0)
		{
			if(m_pProfile->Name && StrCmp(m_pProfile->Name,bstrName))
			{
				m_pProfile->Flags|=PROFILE_NAME_MODIFIED;
			}
			
			if(bstrLuckyURL)
			{
				if(m_pProfile->LuckySearch.QueryURL && StrCmp(m_pProfile->LuckySearch.QueryURL,bstrLuckyURL))
				{
					m_pProfile->Flags|=PROFILE_LUCKYURL_MODIFIED;
				}
			}
			else if(m_pProfile->LuckySearch.QueryURL && StrCmp(m_pProfile->LuckySearch.QueryURL,PROFILE_DEFAULT_LUCKYURL))
			{
				m_pProfile->Flags|=PROFILE_LUCKYURL_MODIFIED;
			}
		}

		if(m_pProfile->Name) SysFreeString(m_pProfile->Name);
		m_pProfile->Name=bstrName;
		bstrName=NULL;

		if(m_pProfile->LuckySearch.QueryURL) SysFreeString(m_pProfile->LuckySearch.QueryURL);
		m_pProfile->LuckySearch.QueryURL=bstrLuckyURL;
		bstrLuckyURL=NULL;

		result=true;
	}
	else MessageBox(m_pToolbar->GetString(IDS_ERR_REQUIRED_FIELDS),L"Quero Toolbar",MB_ICONWARNING|MB_OK);

	if(bstrName) SysFreeString(bstrName);
	if(bstrLuckyURL) SysFreeString(bstrLuckyURL);

	return result;
}