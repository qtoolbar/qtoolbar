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
// UIOptionsSecurity.h: interface for the UIOptionsSecurity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIOPTIONSSECURITY_H__14101308_F51E_48B1_B207_A3333D1F5F7D__INCLUDED_)
#define AFX_UIOPTIONSSECURITY_H__14101308_F51E_48B1_B207_A3333D1F5F7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "QToolbar.h"

class CUIOptionsSecurity :
	public CPropertyPageImpl<CUIOptionsSecurity>,
	public CWinDataExchange<CUIOptionsSecurity>
{
public:
	CUIOptionsSecurity(CQToolbar *pToolbar);

	enum { IDD = IDD_OPTIONS_SECURITY };

	BEGIN_MSG_MAP(CUIOptionsSecurity)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		CHAIN_MSG_MAP(CPropertyPageImpl<CUIOptionsSecurity>)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CUIOptionsSecurity)
		DDX_CHECK(IDC_CHKHIGHLIGHTDOMAIN, m_HighlightDomain)
		DDX_CHECK(ID_DISPLAY_CERTIFICATE_INFORMATION, m_DisplayCertificateInformation)
		DDX_CHECK(IDC_CHKIDNWARNING, m_IDNWarning)
		DDX_CHECK(IDC_CHKMISSGLYPHSWARNING, m_MissGlyphsWarning)
		DDX_CHECK(IDC_CHKASCIIRULESWARNING, m_ASCIIRulesViolationWarning)
		DDX_CHECK(IDC_CHKDIGITASCIIINDICATOR, m_DigitAsciiIndicator)
	END_DDX_MAP()


// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	BOOL OnSetActive();
	BOOL OnKillActive();

	bool m_HighlightDomain;
	bool m_DisplayCertificateInformation;
	bool m_IDNWarning;
	bool m_MissGlyphsWarning;
	bool m_ASCIIRulesViolationWarning;
	bool m_DigitAsciiIndicator;

	CQToolbar *m_pToolbar;

};

#endif // !defined(AFX_UIOPTIONSSECURITY_H__14101308_F51E_48B1_B207_A3333D1F5F7D__INCLUDED_)
