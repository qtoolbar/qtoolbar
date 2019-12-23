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
// UISearchEngine.cpp: implementation of the CUISearchEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UISearchEngine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUISearchEngine::CUISearchEngine(SearchEngine *pEngine,UINT WindowTitleId,CQToolbar *pToolbar)
{
	m_pEngine=pEngine;
	m_pToolbar=pToolbar;
	this->WindowTitleId=WindowTitleId;
}

CUISearchEngine::~CUISearchEngine()
{
}

LRESULT CUISearchEngine::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_pToolbar->CenterDialog(m_hWnd);

	if(WindowTitleId) SetWindowText(m_pToolbar->GetString(WindowTitleId));

	//DoDataExchange(FALSE);

	const int EditCtrlIds[EDIT_COUNT]={IDC_ENGINE_NAME,IDC_ENGINE_QUERYURL,IDC_ENGINE_POSTQUERY,IDC_ENGINE_COUNTRYPARAM,IDC_ENGINE_LINKURL,IDC_ENGINE_ICON};
	UINT i;

	for(i=0;i<EDIT_COUNT;i++)
	{
		m_Edit[i]=GetDlgItem(EditCtrlIds[i]);
		m_Edit[i].LimitText(i==EDIT_NAME?MAX_SEARCHENGINE_NAME_LEN-1:i==EDIT_ICON?MAX_PATH:MAXURLLENGTH-1);
	}
	m_Encoding=GetDlgItem(IDC_ENGINE_ENCODING);

	if(m_pEngine->Name) m_Edit[EDIT_NAME].SetWindowText(m_pEngine->Name);
	if(m_pEngine->QueryURL) m_Edit[EDIT_QUERYURL].SetWindowText(m_pEngine->QueryURL);
	if(m_pEngine->PostQuery) m_Edit[EDIT_POSTQUERY].SetWindowText(m_pEngine->PostQuery);
	if(m_pEngine->CountryParam) m_Edit[EDIT_COUNTRYPARAM].SetWindowText(m_pEngine->CountryParam);
	if(m_pEngine->LinkURL) m_Edit[EDIT_LINKURL].SetWindowText(m_pEngine->LinkURL);
	if(m_pEngine->IconFile) m_Edit[EDIT_ICON].SetWindowText(m_pEngine->IconFile);

	OldEncoding=-1;
	for(i=0;i<N_ENCODINGS;i++)
	{
		m_Encoding.AddString(m_pToolbar->GetString(Encoding_Names[i]));
		if(LOWORD(m_pEngine->Flags)==Encoding_CodePages[i]) OldEncoding=i;
	}
	m_Encoding.SetCurSel(OldEncoding);

	 // Workaround: in Win2k combobox drop-down window is shrunken to 0 pixel
	COMBOBOXINFO info;
	info.cbSize=sizeof(COMBOBOXINFO);
	GetComboBoxInfo(m_Encoding.m_hWnd,&info);
	::MoveWindow(info.hwndList,0,0,100,100,FALSE);

	// Init DialogResize
	DlgResize_Init();

	return 1; // Let the system set the focus
}

LRESULT CUISearchEngine::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch(LOWORD(wParam))
	{
	case IDOK:
		//DoDataExchange(TRUE);
		if(SaveEngine()) EndDialog(IDOK);
		break;
	case IDCANCEL:
		EndDialog(IDCANCEL);
		break;
	case IDC_ENGINE_ICON_BROWSE:
		CFileDialog* pFileDialog;
		BSTR bstrText;

		bstrText=NULL;
		if(m_Edit[EDIT_ICON].GetWindowText(bstrText) && bstrText)
		{
			pFileDialog=new CFileDialog(TRUE,NULL,bstrText,OFN_FILEMUSTEXIST|OFN_HIDEREADONLY,L"Icon File (*.ico)\0*.ico\0",m_hWnd);

			if(pFileDialog)
			{
				if(pFileDialog->DoModal(m_hWnd)==IDOK)
				{
					m_Edit[EDIT_ICON].SetWindowText(pFileDialog->m_szFileName);
				}
				delete pFileDialog;
			}
			SysFreeString(bstrText);
		}
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

bool CUISearchEngine::SaveEngine()
{
	bool result=false;
	BSTR bstrEdit[EDIT_COUNT];
	BSTR bstrText;
	UINT i;
	int NewEncoding;
	bool bModified;

	BSTR *pEngineBSTRs[EDIT_COUNT]={&m_pEngine->Name,&m_pEngine->QueryURL,&m_pEngine->PostQuery,&m_pEngine->CountryParam,&m_pEngine->LinkURL,&m_pEngine->IconFile};

	bModified=false;

	for(i=0;i<EDIT_COUNT;i++)
	{
		bstrEdit[i]=NULL;
		bstrText=NULL;
		if(m_Edit[i].GetWindowText(bstrText) && bstrText)
		{
			CQToolbar::trim(bstrText);
			if(bstrText[0]) bstrEdit[i]=SysAllocString(bstrText);
			if(*pEngineBSTRs[i])
			{
				if(StrCmp(*pEngineBSTRs[i],bstrText)) bModified=true;
			}
			else if(bstrText[0]) bModified=true;

			SysFreeString(bstrText);
		}
		else if(*pEngineBSTRs[i] && **pEngineBSTRs[i]) bModified=true;
	}

	if(bstrEdit[EDIT_NAME] && bstrEdit[EDIT_QUERYURL])
	{
		for(i=0;i<EDIT_COUNT;i++)
		{
			if(*pEngineBSTRs[i]) SysFreeString(*pEngineBSTRs[i]);
			*pEngineBSTRs[i]=bstrEdit[i];
			bstrEdit[i]=NULL;
		}

		NewEncoding=m_Encoding.GetCurSel();
		if(NewEncoding!=CB_ERR && NewEncoding<N_ENCODINGS)
		{
			if(OldEncoding!=NewEncoding)
			{
				m_pEngine->Flags=(m_pEngine->Flags&0xFFFF0000)|Encoding_CodePages[NewEncoding];
				bModified=true;
			}
		}

		if(bModified) m_pEngine->Flags|=SEARCHENGINE_CUSTOM;
		result=true;
	}
	else MessageBox(m_pToolbar->GetString(IDS_ERR_REQUIRED_FIELDS),L"Quero Toolbar",MB_ICONWARNING|MB_OK);

	for(i=0;i<EDIT_COUNT;i++) if(bstrEdit[i]) SysFreeString(bstrEdit[i]);

	return result;
}