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

// ComboEngine.cpp : Implementation of CComboEngine

#include "stdafx.h"
#include "ComboEngine.h"
#include "Quero.h"
#include "QueroBand.h"
#include "QToolbar.h"

/////////////////////////////////////////////////////////////////////////////
// CComboEngine

CComboEngine::CComboEngine()
{
	m_pBand=NULL;
	m_pToolbar=NULL;
	bHighlightSelection=true;
}

CComboEngine::~CComboEngine()
{
}

WNDPROC CComboEngine::ORIG_ListWndProc=NULL;

LRESULT CALLBACK CComboEngine::ListWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	uMsg=MapMiddleButton_Message(uMsg);

	return CallWindowProc(ORIG_ListWndProc,hwnd,uMsg,wParam,lParam);
}

void CComboEngine::SubclassListWnd()
{
	if(ORIG_ListWndProc==NULL) ORIG_ListWndProc=(WNDPROC)::GetWindowLongPtr(m_hWndList,GWLP_WNDPROC);
	::SetWindowLongPtr(m_hWndList,GWLP_WNDPROC,(LONG_PTR)ListWndProc);
}

HWND CComboEngine::Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName, DWORD dwStyle, DWORD dwExStyle, UINT nID)
{
	HWND hWnd;

	hWnd=CWindowImpl<CComboEngine>::Create(hWndParent,rcPos,szWindowName,dwStyle,dwExStyle,nID);

	UpdateComboBoxInfo();

	return hWnd;
}

void CComboEngine::UpdateComboBoxInfo()
{
	COMBOBOXINFO info;
	info.cbSize=sizeof(COMBOBOXINFO);

	GetComboBoxInfo(m_hWnd,&info);
	m_hWndItem=info.hwndItem;
	m_hWndList=info.hwndList;
	m_rcButton=info.rcButton;
}

void CComboEngine::OnHeightChange(int height)
{
	::SendMessage(m_hWnd,CB_SETITEMHEIGHT,(WPARAM)-1,height);

	UpdateComboBoxInfo();
}

void CComboEngine::SetBand(CQueroBand* pBand)
{
	m_pBand = pBand;
}

void CComboEngine::SetToolbar(CQToolbar* pToolbar)
{
	m_pToolbar = pToolbar;
}

LRESULT CComboEngine::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Notify host that our band has the focus so TranslateAcceleratorIO 
	// messages are directed towards our band.
	if(m_pBand)	m_pBand->FocusChange(TRUE);
	bHandled = FALSE;
	return 0;
}

LRESULT CComboEngine::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT result;
	int nVirtKey = (int)(wParam);
	LRESULT droppedstate;
	int ProfileId;

	result=0;
	bHandled=FALSE;

	droppedstate=::SendMessage(m_hWnd,CB_GETDROPPEDSTATE,0,0);

	switch(nVirtKey)
	{
	case VK_ESCAPE:
		::SendMessage(m_hWnd,CB_SHOWDROPDOWN,FALSE,0);
		break;

	case VK_RIGHT:
		if(droppedstate)
		{
			bHighlightSelection=false;
			m_pToolbar->SetChooseProfile(false);
			ProfileId=m_pToolbar->GetProfileId();
			ProfileId=m_pToolbar->m_Profiles.GetNextProfileId(ProfileId);
			m_pToolbar->SelectProfile(ProfileId);
			::SendMessage(m_hWnd,CB_SHOWDROPDOWN,TRUE,0);
			bHandled=TRUE;
		}
		else
		{
			::SendMessage(m_hWnd,CB_SHOWDROPDOWN,TRUE,0);
			result=-1;
		}
		break;
	case VK_LEFT:
		if(droppedstate)
		{
			bHighlightSelection=false;
			m_pToolbar->SetChooseProfile(false);
			ProfileId=m_pToolbar->GetProfileId();
			ProfileId=m_pToolbar->m_Profiles.GetPreviousProfileId(ProfileId);
			m_pToolbar->SelectProfile(ProfileId);
			::SendMessage(m_hWnd,CB_SHOWDROPDOWN,TRUE,0);
			bHandled=TRUE;
		}
		else
		{
			::SendMessage(m_hWnd,CB_SHOWDROPDOWN,TRUE,0);
			result=-1;
		}
		break;
	case VK_UP:
	case VK_DOWN:
		if(!droppedstate) 
		{
			::SendMessage(m_hWnd,CB_SHOWDROPDOWN,TRUE,0);
			result=-1;
		}
		break;
	default:
		if((nVirtKey>=L'0' && nVirtKey<=L'9') || (nVirtKey>=VK_NUMPAD0 && nVirtKey<=VK_NUMPAD9))
		{
			int digit;

			if(nVirtKey<=L'9') digit=nVirtKey-L'0';
			else digit=nVirtKey-VK_NUMPAD0;
			if(digit==0) digit=10;
			ProfileId=m_pToolbar->m_Profiles.IndexToProfileId(digit-1);
			if(ProfileId!=-1 && ProfileId!=m_pToolbar->GetProfileId())
			{
				bHighlightSelection=false;
				m_pToolbar->SetChooseProfile(false);				
				m_pToolbar->SelectProfile(ProfileId);
				::SendMessage(m_hWnd,CB_SHOWDROPDOWN,TRUE,0);
				bHandled=TRUE;
			}
		}
		else if(!droppedstate && nVirtKey>32) ::SendMessage(m_hWnd,CB_SHOWDROPDOWN,TRUE,0);
	}

	return result;
}

LRESULT CComboEngine::OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int nVirtKey = (int)(wParam);
	LRESULT droppedstate;
	
	if (VK_F4 == nVirtKey)
	{
		droppedstate=::SendMessage(m_hWnd,CB_GETDROPPEDSTATE,0,0);
		if(droppedstate) ::SendMessage(m_hWnd,CB_SHOWDROPDOWN,FALSE,0);
		else ::SendMessage(m_hWnd,CB_SHOWDROPDOWN,TRUE,0);
		bHandled=TRUE;
		return 0;
	}
	bHandled=FALSE;
	return 0;
}