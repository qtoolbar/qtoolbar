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
// UIOptionsAdBlocker.h: interface for the CUIOptionsAdBlocker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIOPTIONSADBLOCKER_H__590F9371_9DE0_4292_B5B2_A99C8B6CFB6F__INCLUDED_)
#define AFX_UIOPTIONSADBLOCKER_H__590F9371_9DE0_4292_B5B2_A99C8B6CFB6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "QToolbar.h"

#define N_ADBLOCKER_OPTIONS 5

class CUIOptionsAdBlocker :
	public CPropertyPageImpl<CUIOptionsAdBlocker>,
	public CWinDataExchange<CUIOptionsAdBlocker>
{

public:
	CUIOptionsAdBlocker(CQToolbar *pToolbar);

	enum { IDD = IDD_OPTIONS_ADBLOCKER };

	BEGIN_MSG_MAP(CUIOptionsAdBlocker)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		CHAIN_MSG_MAP(CPropertyPageImpl<CUIOptionsAdBlocker>)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CUIOptionsAdBlocker)
		DDX_CHECK(IDC_HIDE_FLASH, m_HideFlashAds)
		DDX_CHECK(IDC_BLOCK_POPUPS, m_EnablePopUpBlocker)
		DDX_CHECK(IDC_BLOCK_ADS, m_EnableAdBlocker)
		DDX_CHECK(IDC_DISABLE_AD_BLOCKER_FOR_LOCAL_WEB_SITES, m_DisableAdBlockerForLocalWebSites)
		DDX_CHECK(IDC_DISABLE_AD_BLOCKER_FOR_HTTPS_WEB_SITES, m_DisableAdBlockerForHttpsWebSites)
	END_DDX_MAP()


// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	BOOL OnSetActive();
	BOOL OnKillActive();
	BOOL ExchangeData(BOOL bSaveAndValidate);

	bool m_HideFlashAds;

	void EnablePopUpBlocker(bool bEnable);
	void EnableAdBlocker(bool bEnable);

	CButton m_RdoPopUpBlocker_BlockUnwanted;
	CButton m_RdoPopUpBlocker_BlockAll;
	CButton m_ChkPopUpBlocker_PlaySound;

	bool m_EnablePopUpBlocker;
	DWORD m_BlockPopUps;

	bool m_EnableAdBlocker;
	CButton m_ChkAdBlocker[N_ADBLOCKER_OPTIONS];
	DWORD m_BlockAds;

	bool m_DisableAdBlockerForLocalWebSites;
	bool m_DisableAdBlockerForHttpsWebSites;

	void SetOptions(UINT Options, UINT Options2);
	void GetOptions(UINT *pOptions, UINT *pOptions2);

	CQToolbar *m_pToolbar;
};

#endif // !defined(AFX_UIOPTIONSADBLOCKER_H__590F9371_9DE0_4292_B5B2_A99C8B6CFB6F__INCLUDED_)
