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
// UIOptionsSettings.cpp: implementation of the CUIOptionsSettings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIOptionsSettings.h"
#include "QToolbar.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIOptionsSettings::CUIOptionsSettings(CQToolbar *pToolbar)
{
	m_pToolbar=pToolbar;
}

LRESULT CUIOptionsSettings::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TCHAR QueroVersion[255];
	TCHAR QueroVersionNumber[32];
	const TCHAR *QueroBuild[4]={L"xp",L"9x",L"x86",L"x64"};

	if(g_IE_MajorVersion<7) ::EnableWindow(GetDlgItem(IDC_PAGE_LOADING_ANIMATION),FALSE);

	// Print Quero version information
	if((QUERO_VERSION&0x00FFFFFF)==0) StringCbPrintf(QueroVersionNumber,sizeof QueroVersionNumber,L"%d",QUERO_VERSION>>24,(QUERO_VERSION&0xFF0000)>>16);
	else if((QUERO_VERSION&0x0000FFFF)==0) StringCbPrintf(QueroVersionNumber,sizeof QueroVersionNumber,L"%d.%d",QUERO_VERSION>>24,(QUERO_VERSION&0xFF0000)>>16);
	else if((QUERO_VERSION&0x000000FF)==0) StringCbPrintf(QueroVersionNumber,sizeof QueroVersionNumber,L"%d.%d.%d",QUERO_VERSION>>24,(QUERO_VERSION&0xFF0000)>>16,(QUERO_VERSION&0xFF00)>>8);
	else StringCbPrintf(QueroVersionNumber,sizeof QueroVersionNumber,L"%d.%d.%d.%d",QUERO_VERSION>>24,(QUERO_VERSION&0xFF0000)>>16,(QUERO_VERSION&0xFF00)>>8,(QUERO_VERSION&0xFF));
	StringCbPrintf(QueroVersion,sizeof QueroVersion,L"Quero Toolbar version %s %s IE%d Windows 0x%04x",QueroVersionNumber,(QUERO_BUILD<4?QueroBuild[QUERO_BUILD]:L"?"),g_IE_MajorVersion,g_WindowsVersion);
	m_Edit_QueroVersion=GetDlgItem(IDC_QUERO_VERSION);
	m_Edit_QueroVersion.SetWindowText(QueroVersion);

	return 1; // Let the system set the focus
}

BOOL CUIOptionsSettings::ExchangeData(BOOL bSaveAndValidate)
{
	BOOL result;

	result=DoDataExchange(bSaveAndValidate);
	if(bSaveAndValidate==FALSE)
	{
		EnableHighlightInDifferentColors(m_Highlight);
	}

	return result;
}

LRESULT CUIOptionsSettings::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(HIWORD(wParam)==BN_CLICKED)
	{
		DoDataExchange(TRUE);

		switch(LOWORD(wParam))
		{
		case IDC_CHKHIGHLIGHT:
			EnableHighlightInDifferentColors(m_Highlight);
			break;
		}
	}

	return 0;
}

void CUIOptionsSettings::EnableHighlightInDifferentColors(bool bEnable)
{
	::EnableWindow(GetDlgItem(IDC_HIGHLIGHT_IN_DIFFERENT_COLORS),bEnable);
}

BOOL CUIOptionsSettings::OnSetActive()
{
	ExchangeData(FALSE);
	return TRUE;
}

BOOL CUIOptionsSettings::OnKillActive()
{
	ExchangeData(TRUE);
	return TRUE;
}

void CUIOptionsSettings::SetOptions(UINT Options, UINT Options2)
{
	m_SearchCountry=(Options&OPTION_SearchCountry)!=0;
	m_SaveAddressHistory=(Options&OPTION_SaveAddressHistory)!=0;
	m_SaveSearchHistory=(Options&OPTION_SaveSearchHistory)!=0;
	m_RememberLastSearch=(Options&OPTION_RememberLastSearch)!=0;
	m_QuickType=(Options&OPTION_EnableQuickType)!=0;
	m_QuickFind=(Options&OPTION_EnableQuickFind)!=0;

	m_ShowSearchTermWhileSearching=(Options2&OPTION2_ShowSearchTermsWhileSearching)!=0;
	m_PageLoadingAnimation=(Options2&OPTION2_PageLoadingAnimation)!=0;
	m_HighlightInDifferentColors=(Options2&OPTION2_HighlightInDifferentColors)!=0;
	m_AddressNavigation=(Options2&OPTION2_AddressNavigation)!=0;
}

void CUIOptionsSettings::GetOptions(UINT *pOptions, UINT *pOptions2)
{
	UINT Options=*pOptions;
	UINT Options2=*pOptions2;

	if(m_SearchCountry) Options|=OPTION_SearchCountry;
	else Options&=~OPTION_SearchCountry;
	if(m_SaveAddressHistory) Options|=OPTION_SaveAddressHistory;
	else Options&=~OPTION_SaveAddressHistory;
	if(m_SaveSearchHistory) Options|=OPTION_SaveSearchHistory;
	else Options&=~OPTION_SaveSearchHistory;
	if(m_RememberLastSearch) Options|=OPTION_RememberLastSearch;
	else Options&=~OPTION_RememberLastSearch;
	if(m_QuickType) Options|=OPTION_EnableQuickType;
	else Options&=~OPTION_EnableQuickType;
	if(m_QuickFind) Options|=OPTION_EnableQuickFind;
	else Options&=~OPTION_EnableQuickFind;

	if(m_ShowSearchTermWhileSearching) Options2|=OPTION2_ShowSearchTermsWhileSearching;
	else Options2&=~OPTION2_ShowSearchTermsWhileSearching;
	if(m_PageLoadingAnimation) Options2|=OPTION2_PageLoadingAnimation;
	else Options2&=~OPTION2_PageLoadingAnimation;
	if(m_HighlightInDifferentColors) Options2|=OPTION2_HighlightInDifferentColors;
	else Options2&=~OPTION2_HighlightInDifferentColors;
	if(m_AddressNavigation) Options2|=OPTION2_AddressNavigation;
	else Options2&=~OPTION2_AddressNavigation;

	*pOptions=Options;
	*pOptions2=Options2;
}