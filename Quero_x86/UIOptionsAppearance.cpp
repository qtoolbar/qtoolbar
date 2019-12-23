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
// UIOptionsAppearance.cpp: implementation of the CUIOptionsAppearance class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIOptionsAppearance.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIOptionsAppearance::CUIOptionsAppearance(CQToolbar *pToolbar)
{
	m_pToolbar=pToolbar;
}

void CUIOptionsAppearance::SetOptions(UINT Options, UINT Options2)
{
	m_IE_Navigation_Bar=(g_Options&OPTION_HideNavigationBar)==0;
	m_IE_Navigation_Bar_InitialState=m_IE_Navigation_Bar;
	m_IE_Address_Box=(g_Options2&OPTION2_HideAddressBox)==0;
	m_IE_Show_Caption=(g_Options2&OPTION2_ShowCaption)!=0;
	m_IE_Caption_White_Text=(g_Options2&OPTION2_ShowCaption_SysColor)==0;
	m_ToolbarCloseButton=(g_Options2&OPTION2_HideToolbarCloseButton)==0;
	m_WhiteQueroButton=(g_Options&OPTION_WhiteQueroButton)!=0;
	m_ShowSearchBox=(g_Options2&OPTION2_ShowSearchBox)!=0;
	m_ShowSearchEngineComboBox=(g_Options&OPTION_ShowSearchEngineComboBox)!=0;
	m_EnableAeroTheme=(g_Options2&OPTION2_EnableAeroTheme)!=0;
}

void CUIOptionsAppearance::GetOptions(UINT *pOptions, UINT *pOptions2)
{
	UINT Options=*pOptions;
	UINT Options2=*pOptions2;

	if(m_IE_Navigation_Bar) Options&=~OPTION_HideNavigationBar;
	else Options|=OPTION_HideNavigationBar;
	if(m_ShowSearchEngineComboBox) Options|=OPTION_ShowSearchEngineComboBox;
	else Options&=~OPTION_ShowSearchEngineComboBox;
	if(m_ShowSearchBox) Options2|=OPTION2_ShowSearchBox;
	else Options2&=~OPTION2_ShowSearchBox;
	if(m_EnableAeroTheme) Options2|=OPTION2_EnableAeroTheme;
	else Options2&=~OPTION2_EnableAeroTheme;
	if(m_ToolbarCloseButton) Options2&=~OPTION2_HideToolbarCloseButton;
	else Options2|=OPTION2_HideToolbarCloseButton;
	if(m_IE_Address_Box) Options2&=~OPTION2_HideAddressBox;
	else Options2|=OPTION2_HideAddressBox;
	if(m_IE_Show_Caption) Options2|=OPTION2_ShowCaption;
	else Options2&=~OPTION2_ShowCaption;
	if(m_IE_Caption_White_Text) Options2&=~OPTION2_ShowCaption_SysColor;
	else Options2|=OPTION2_ShowCaption_SysColor;
	if(m_WhiteQueroButton) Options|=OPTION_WhiteQueroButton;
	else Options&=~OPTION_WhiteQueroButton;

	*pOptions=Options;
	*pOptions2=Options2;
}

LRESULT CUIOptionsAppearance::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	COMBOBOXINFO info;
	UINT i;

	if(m_IE_Navigation_Bar_Enabled==false) ::EnableWindow(GetDlgItem(IDC_IE_NAVIGATION_BAR),FALSE);

	#ifndef COMPILE_FOR_WINDOWS_VISTA
	::EnableWindow(GetDlgItem(IDC_ENABLEAEROTHEME),FALSE);
	#endif

	if(g_IE_MajorVersion<9)
	{
		::EnableWindow(GetDlgItem(IDC_IE_ADDRESS_BOX),FALSE);
		::EnableWindow(GetDlgItem(IDC_IE_SHOW_CAPTION),FALSE);
		::EnableWindow(GetDlgItem(IDC_IE_CAPTION_WHITE_TEXT),FALSE);
	}

	if(g_IE_MajorVersion<8) ::EnableWindow(GetDlgItem(IDC_TOOLBAR_CLOSE_BUTTON),FALSE);

	// Set Quero button text
	::SetWindowText(GetDlgItem(IDC_QUEROBUTTON),m_pToolbar->GetString(OPTION_QueroShortcutKey(g_Options)!=QKEY_AltQ?IDS_HINT_QUERO_ALT_Q:IDS_HINT_QUERO_ALT_SHIFT_Q));
	
	m_ComboFontSize = GetDlgItem(IDC_CBOFONTSIZE);
	m_ComboFontColor = GetDlgItem(IDC_CBOFONTCOLOR);
	
	info.cbSize=sizeof(COMBOBOXINFO); // Workaround: in Win2k combobox drop-down window is shrunken to 0 pixel
	GetComboBoxInfo(m_ComboFontSize.m_hWnd,&info);
	::MoveWindow(info.hwndList,0,0,100,100,FALSE);

	const UINT FontSizeOptions[3]={IDS_OPTION_STANDARD,IDS_OPTION_MEDIUM,IDS_OPTION_LARGE};

	for(i=0;i<3;i++) m_ComboFontSize.AddString(m_pToolbar->GetString(FontSizeOptions[i]));

	GetComboBoxInfo(m_ComboFontColor.m_hWnd,&info); // Workaround
	::MoveWindow(info.hwndList,0,0,100,100,FALSE);

	const UINT FontColorOptions[N_COLOR_SCHEMES]={IDS_OPTION_GREENBLUE,IDS_OPTION_BLACK,IDS_OPTION_PINK};

	for(i=0;i<N_COLOR_SCHEMES;i++) m_ComboFontColor.AddString(m_pToolbar->GetString(FontColorOptions[i]));

	m_Edit_QueroTheme_FileName=GetDlgItem(IDC_QUEROTHEME_FILENAME);

	return 1; // Let the system set the focus
}

BOOL CUIOptionsAppearance::ExchangeData(BOOL bSaveAndValidate)
{
	BOOL result;
	int id;

	result=DoDataExchange(bSaveAndValidate);
	if(bSaveAndValidate==FALSE)
	{
		UpdateEnableState();

		switch(m_FontSize)
		{
		case FONTSIZE_STANDARD:
			id=0;
			break;
		case FONTSIZE_MEDIUM:
			id=1;
			break;
		case FONTSIZE_LARGE:
			id=2;
			break;
		default:
			id=-1;
		}

		m_ComboFontSize.SetCurSel(id);

		switch(m_FontColor)
		{
		case FONTCOLOR_BLACK:
			id=1;
			break;
		case FONTCOLOR_GREENBLUE:
			id=0;
			break;
		case FONTCOLOR_PINK:
			id=2;
			break;
		default:
			id=-1;
		}

		m_ComboFontColor.SetCurSel(id);

		m_Edit_QueroTheme_FileName.SetWindowText(m_QueroTheme_FileName);
	}
	else
	{
		id=m_ComboFontSize.GetCurSel();
		switch(id)
		{
		case 0:
			m_FontSize=FONTSIZE_STANDARD;
			break;
		case 1:
			m_FontSize=FONTSIZE_MEDIUM;
			break;
		case 2:
			m_FontSize=FONTSIZE_LARGE;
			break;
		}

		id=m_ComboFontColor.GetCurSel();
		switch(id)
		{
		case 0:
			m_FontColor=FONTCOLOR_GREENBLUE;
			break;
		case 1:
			m_FontColor=FONTCOLOR_BLACK;
			break;
		case 2:
			m_FontColor=FONTCOLOR_PINK;
			break;
		}

		m_Edit_QueroTheme_FileName.GetWindowText(m_QueroTheme_FileName,MAX_PATH);
	}

	return result;
}

BOOL CUIOptionsAppearance::OnSetActive()
{
	ExchangeData(FALSE);
	return TRUE;
}

BOOL CUIOptionsAppearance::OnKillActive()
{
	ExchangeData(TRUE);
	return TRUE;
}

LRESULT CUIOptionsAppearance::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch(LOWORD(wParam))
	{
	#ifdef COMPILE_FOR_WINDOWS_VISTA
	case IDC_IE_NAVIGATION_BAR:
		ExchangeData(TRUE);
		Button_SetElevationRequiredState(::GetDlgItem(GetParent(),IDOK),m_IE_Navigation_Bar!=m_IE_Navigation_Bar_InitialState);
		break;
	#endif
	case IDC_IE_SHOW_CAPTION:
		if(HIWORD(wParam)==BN_CLICKED)
		{
			DoDataExchange(TRUE,IDC_IE_SHOW_CAPTION);
			UpdateEnableState();
		}
		bHandled=FALSE;
		break;
	case IDC_QUEROTHEME_BROWSE:
		CFileDialog* pFileDialog;
		
		pFileDialog=new CFileDialog(TRUE,NULL,m_QueroTheme_FileName,OFN_FILEMUSTEXIST|OFN_HIDEREADONLY,L"Quero Theme File (*.dll)\0*.dll\0",m_hWnd);

		if(pFileDialog)
		{
			if(pFileDialog->DoModal(m_hWnd)==IDOK)
			{
				StringCbCopy(m_QueroTheme_FileName,sizeof m_QueroTheme_FileName,pFileDialog->m_szFileName);
				m_Edit_QueroTheme_FileName.SetWindowText(m_QueroTheme_FileName);
			}
			delete pFileDialog;
		}
		break;
	default:
		bHandled=FALSE;
	}

	return 0;
}

void CUIOptionsAppearance::UpdateEnableState()
{
	::EnableWindow(GetDlgItem(IDC_IE_CAPTION_WHITE_TEXT),m_IE_Show_Caption);
}

void CUIOptionsAppearance::SetButtons(DWORD Buttons)
{
	m_Buttons=Buttons;
	m_GoButton=(Buttons&BUTTON_GO)!=0;
	m_BackButton=(Buttons&BUTTON_BACK)!=0;
	m_ForwardButton=(Buttons&BUTTON_FORWARD)!=0;
	m_RefreshButton=(Buttons&BUTTON_REFRESH)!=0;
	m_StopButton=(Buttons&BUTTON_STOP)!=0;
	m_HomeButton=(Buttons&BUTTON_HOME)!=0;
	m_QueroButton=(Buttons&BUTTON_QUERO)!=0;
}

DWORD CUIOptionsAppearance::GetButtons()
{
	if(m_GoButton) m_Buttons|=BUTTON_GO;
	else m_Buttons&=~BUTTON_GO;
	if(m_BackButton) m_Buttons|=BUTTON_BACK;
	else m_Buttons&=~BUTTON_BACK;
	if(m_ForwardButton) m_Buttons|=BUTTON_FORWARD;
	else m_Buttons&=~BUTTON_FORWARD;
	if(m_RefreshButton) m_Buttons|=BUTTON_REFRESH;
	else m_Buttons&=~BUTTON_REFRESH;
	if(m_StopButton) m_Buttons|=BUTTON_STOP;
	else m_Buttons&=~BUTTON_STOP;
	if(m_HomeButton) m_Buttons|=BUTTON_HOME;
	else m_Buttons&=~BUTTON_HOME;
	if(m_QueroButton) m_Buttons|=BUTTON_QUERO;
	else m_Buttons&=~BUTTON_QUERO;

	return m_Buttons;
}

void CUIOptionsAppearance::CopyQueroThemeFileName()
{
	if(WaitForSingleObject(g_hQSharedDataMutex,QMUTEX_TIMEOUT)==WAIT_OBJECT_0)
	{
		if(g_QueroTheme_FileName) StringCbCopy(m_QueroTheme_FileName,sizeof m_QueroTheme_FileName,g_QueroTheme_FileName);
		else m_QueroTheme_FileName[0]=0;

		ReleaseMutex(g_hQSharedDataMutex);
	}
	QDEBUG_CODE else QDEBUG_PRINT(L"SyncError",L"CopyQueroThemeFileName");
}