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
// UIOptionsAdBlocker.cpp: implementation of the CUIOptionsAdBlocker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIOptionsAdBlocker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIOptionsAdBlocker::CUIOptionsAdBlocker(CQToolbar *pToolbar)
{
	m_pToolbar=pToolbar;
}

LRESULT CUIOptionsAdBlocker::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_RdoPopUpBlocker_BlockUnwanted=GetDlgItem(IDC_RDOPOPUPBLOCKER_BLOCKUNWANTED);	
	m_RdoPopUpBlocker_BlockAll=GetDlgItem(IDC_RDOPOPUPBLOCKER_BLOCKALL);
	m_ChkPopUpBlocker_PlaySound=GetDlgItem(IDC_CHKPOPUPBLOCKER_PLAYSOUND);

	m_EnablePopUpBlocker=m_BlockPopUps&POPUPBLOCKER_Enable;

	const int ResIds[N_ADBLOCKER_OPTIONS]={IDC_BLOCK_FLASH,IDC_BLOCK_BANNER,IDC_BLOCK_ADSCRIPT,IDC_BLOCK_DIV,IDC_BLOCK_IFRAME};

	for(int i=0;i<N_ADBLOCKER_OPTIONS;i++) m_ChkAdBlocker[i]=GetDlgItem(ResIds[i]);

	if(g_IE_MajorVersion<9)
	{
		m_EnableAdBlocker=(m_BlockAds&ADBLOCKER_Enable);
		::ShowWindow(GetDlgItem(IDC_STATIC_AD_BLOCKER_NOTE),SW_HIDE);
	}
	else
	{
		m_EnableAdBlocker=false;
		::ShowWindow(GetDlgItem(IDC_BLOCK_ADS),SW_HIDE);
		for(int i=0;i<N_ADBLOCKER_OPTIONS;i++) m_ChkAdBlocker[i].ShowWindow(SW_HIDE);
	}

	return 1; // Let the system set the focus
}

BOOL CUIOptionsAdBlocker::ExchangeData(BOOL bSaveAndValidate)
{
	BOOL result;
	int i;
	const int AdBlockerOptions[N_ADBLOCKER_OPTIONS]={ADBLOCKER_BLOCK_FLASH,ADBLOCKER_BLOCK_BANNER,ADBLOCKER_BLOCK_ADSCRIPT,ADBLOCKER_BLOCK_DIV,ADBLOCKER_BLOCK_IFRAME};

	result=DoDataExchange(bSaveAndValidate);
	if(bSaveAndValidate==FALSE)
	{
		if(m_BlockPopUps&POPUPBLOCKER_BlockAll)
			m_RdoPopUpBlocker_BlockAll.SetCheck(1);
		else
			m_RdoPopUpBlocker_BlockUnwanted.SetCheck(1);

		if(m_BlockPopUps&POPUPBLOCKER_PlaySound) m_ChkPopUpBlocker_PlaySound.SetCheck(1);
		
		EnablePopUpBlocker(m_EnablePopUpBlocker);

		for(i=0;i<N_ADBLOCKER_OPTIONS;i++) if(m_BlockAds&AdBlockerOptions[i]) m_ChkAdBlocker[i].SetCheck(1);

		EnableAdBlocker(m_EnableAdBlocker);
	}
	else
	{
		m_BlockPopUps=m_EnablePopUpBlocker?POPUPBLOCKER_Enable:0;
		if(m_RdoPopUpBlocker_BlockUnwanted.GetCheck()) m_BlockPopUps&=~(POPUPBLOCKER_BlockAll);
		else m_BlockPopUps|=POPUPBLOCKER_BlockAll;
		if(m_ChkPopUpBlocker_PlaySound.GetCheck()) m_BlockPopUps|=POPUPBLOCKER_PlaySound;

		if(m_EnableAdBlocker) m_BlockAds|=ADBLOCKER_Enable;
		else m_BlockAds&=~ADBLOCKER_Enable;

		for(i=0;i<N_ADBLOCKER_OPTIONS;i++)
		{
			if(m_ChkAdBlocker[i].GetCheck()) m_BlockAds|=AdBlockerOptions[i];
			else m_BlockAds&=~AdBlockerOptions[i];
		}
	}

	return result;
}

void CUIOptionsAdBlocker::EnablePopUpBlocker(bool bEnable)
{
	m_RdoPopUpBlocker_BlockUnwanted.EnableWindow(bEnable);
	m_RdoPopUpBlocker_BlockAll.EnableWindow(bEnable);
	m_ChkPopUpBlocker_PlaySound.EnableWindow(bEnable);
}

void CUIOptionsAdBlocker::EnableAdBlocker(bool bEnable)
{
	for(int i=0;i<N_ADBLOCKER_OPTIONS;i++) m_ChkAdBlocker[i].EnableWindow(bEnable);
}

LRESULT CUIOptionsAdBlocker::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(LOWORD(wParam)==IDC_BLOCK_POPUPS && HIWORD(wParam)==BN_CLICKED)
	{
		DoDataExchange(TRUE);
		EnablePopUpBlocker(m_EnablePopUpBlocker);
	}
	else if(LOWORD(wParam)==IDC_BLOCK_ADS && HIWORD(wParam)==BN_CLICKED)
	{
		DoDataExchange(TRUE);
		EnableAdBlocker(m_EnableAdBlocker);
	}
	else if(LOWORD(wParam)==IDC_CMDWHITELIST)
	{
		m_pToolbar->ShowWhiteList(m_hWnd);
	}

	return 0;
}

BOOL CUIOptionsAdBlocker::OnSetActive()
{
	ExchangeData(FALSE);
	return TRUE;
}

BOOL CUIOptionsAdBlocker::OnKillActive()
{
	ExchangeData(TRUE);
	return TRUE;
}

void CUIOptionsAdBlocker::SetOptions(UINT Options, UINT Options2)
{
	m_DisableAdBlockerForLocalWebSites=(Options&OPTION_DisableAdBlockerForLocalWebSites)!=0;
	m_DisableAdBlockerForHttpsWebSites=(Options&OPTION_DisableAdBlockerForHttpsWebSites)!=0;
}

void CUIOptionsAdBlocker::GetOptions(UINT *pOptions, UINT *pOptions2)
{
	UINT Options=*pOptions;
	UINT Options2=*pOptions2;

	if(m_DisableAdBlockerForLocalWebSites) Options|=OPTION_DisableAdBlockerForLocalWebSites;
	else Options&=~OPTION_DisableAdBlockerForLocalWebSites;
	if(m_DisableAdBlockerForHttpsWebSites) Options|=OPTION_DisableAdBlockerForHttpsWebSites;
	else Options&=~OPTION_DisableAdBlockerForHttpsWebSites;

	if(m_HideFlashAds) Options2|=OPTION2_HideFlashAds;
	else Options2&=~OPTION2_HideFlashAds;

	*pOptions=Options;
	*pOptions2=Options2;
}