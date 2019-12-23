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
// UIOptionsAppearance.h: interface for the CUIOptionsAppearance class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIOPTIONSAPPEARANCE_H__D67EB236_C465_4483_844D_211D7A8CEE41__INCLUDED_)
#define AFX_UIOPTIONSAPPEARANCE_H__D67EB236_C465_4483_844D_211D7A8CEE41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "QToolbar.h"

class CUIOptionsAppearance :
	public CPropertyPageImpl<CUIOptionsAppearance>,
	public CWinDataExchange<CUIOptionsAppearance>
{
public:
	CUIOptionsAppearance(CQToolbar *pToolbar);

	enum { IDD = IDD_OPTIONS_APPEARANCE };

	BEGIN_MSG_MAP(CUIOptionsAppearance)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		CHAIN_MSG_MAP(CPropertyPageImpl<CUIOptionsAppearance>)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CUIOptionsAppearance)
		DDX_CHECK(IDC_CHKGOBUTTON, m_GoButton)
		DDX_CHECK(IDC_CHKBACKBUTTON, m_BackButton)
		DDX_CHECK(IDC_CHKFORWARDBUTTON, m_ForwardButton)
		DDX_CHECK(IDC_CHKREFRESHBUTTON, m_RefreshButton)
		DDX_CHECK(IDC_CHKSTOPBUTTON, m_StopButton)
		DDX_CHECK(IDC_CHKHOMEBUTTON, m_HomeButton)
		DDX_CHECK(IDC_QUEROBUTTON, m_QueroButton)
		DDX_CHECK(IDC_IE_NAVIGATION_BAR, m_IE_Navigation_Bar)
		DDX_CHECK(IDC_IE_ADDRESS_BOX, m_IE_Address_Box)
		DDX_CHECK(IDC_IE_SHOW_CAPTION, m_IE_Show_Caption)
		DDX_CHECK(IDC_IE_CAPTION_WHITE_TEXT, m_IE_Caption_White_Text)
		DDX_CHECK(IDC_TOOLBAR_CLOSE_BUTTON, m_ToolbarCloseButton)
		DDX_CHECK(IDC_WHITE_QUERO_BUTTON, m_WhiteQueroButton)
		DDX_CHECK(IDC_SHOWSEARCHBOX, m_ShowSearchBox)
		DDX_CHECK(IDC_SHOWSEARCHENGINECBOX, m_ShowSearchEngineComboBox)
		DDX_CHECK(IDC_ENABLEAEROTHEME, m_EnableAeroTheme)
	END_DDX_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	BOOL OnSetActive();
	BOOL OnKillActive();
	BOOL ExchangeData(BOOL bSaveAndValidate);

	void UpdateEnableState();

	CComboBox m_ComboFontSize;
	CComboBox m_ComboFontColor;

	bool m_GoButton;
	bool m_BackButton;
	bool m_ForwardButton;
	bool m_RefreshButton;
	bool m_StopButton;
	bool m_HomeButton;
	bool m_QueroButton;
	int m_FontSize;
	int m_FontColor;

	bool m_IE_Navigation_Bar;
	bool m_IE_Navigation_Bar_Enabled;
	bool m_IE_Navigation_Bar_InitialState;
	bool m_IE_Address_Box;
	bool m_IE_Show_Caption;
	bool m_IE_Caption_White_Text;
	bool m_ToolbarCloseButton;
	bool m_WhiteQueroButton;
	bool m_ShowSearchBox;
	bool m_ShowSearchEngineComboBox;
	bool m_EnableAeroTheme;

	void SetButtons(DWORD Buttons);
	DWORD GetButtons();
	void SetOptions(UINT Options, UINT Options2);
	void GetOptions(UINT *pOptions, UINT *pOptions2);

	DWORD m_Buttons;

	void CopyQueroThemeFileName();

	CEdit m_Edit_QueroTheme_FileName;
	TCHAR m_QueroTheme_FileName[MAX_PATH];

	CQToolbar *m_pToolbar;
};

#endif // !defined(AFX_UIOPTIONSAPPEARANCE_H__D67EB236_C465_4483_844D_211D7A8CEE41__INCLUDED_)
