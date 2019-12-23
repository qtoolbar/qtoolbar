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

// NavBar.cpp : Implementation of CNavBar

#include "stdafx.h"
#include "NavBar.h"
#include "QToolbar.h"
#include "resource.h"
#ifdef COMPILE_FOR_WINDOWS_VISTA
#include "MenuIcon.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// CNavBar

CToolbarIcons CNavBar::m_ToolbarIcons;

CNavBar::CNavBar()
{
	const UINT CommandIds[N_NAV_BUTTONS]={IDM_BACK,IDM_FORWARD,IDM_REFRESH,IDM_STOP,IDM_HOME};
	const UINT ButtonIds[N_NAV_BUTTONS]={BUTTON_BACK,BUTTON_FORWARD,BUTTON_REFRESH,BUTTON_STOP,BUTTON_HOME};
	UINT i;

	pButtonSizeX=&g_Scaled_NavButtonSize;
	pButtonSizeY=&g_Scaled_NavButtonSize;
	ButtonMargin=8;
	bSetTooltipNoPrefixStyle=true;
	i=0;
	while(i<N_NAV_BUTTONS)
	{
		this->CommandIds[i]=CommandIds[i];
		this->ButtonIds[i]=ButtonIds[i];
		ButtonStyles[i]=(BTNS_BUTTON | BTNS_AUTOSIZE);
		i++;
	}
	ButtonCount=N_NAV_BUTTONS;
	ButtonMask=BUTTON_BACK|BUTTON_FORWARD|BUTTON_REFRESH|BUTTON_STOP|BUTTON_HOME;
}

/*
CNavBar::~CNavBar()
{
	if(IsWindow()) DestroyWindow();
}*/

void CNavBar::LoadToolbarIcons()
{
	const WORD Icons[N_NAV_BUTTONS]={IDI_NAV_BACK,IDI_NAV_FORWARD,IDI_NAV_REFRESH,IDI_NAV_STOP,IDI_NAV_HOME};
	const WORD Icons_Hot[N_NAV_BUTTONS]={IDI_NAV_BACK_HOT,IDI_NAV_FORWARD_HOT,IDI_NAV_REFRESH_HOT,IDI_NAV_STOP_HOT,IDI_NAV_HOME_HOT};
	const WORD Icons_Disabled[2]={IDI_NAV_BACK_DISABLED,IDI_NAV_FORWARD_DISABLED};

	m_ToolbarIcons.Create(g_Scaled_NavButtonSize,g_Scaled_NavButtonSize,ILC_COLOR32|ILC_MASK,N_NAV_BUTTONS,CREATE_TB_IMAGELIST|CREATE_TB_HOTIMAGELIST|CREATE_TB_DISABLEDIMAGELIST);

	m_ToolbarIcons.LoadIcons(TB_IMAGELIST,Icons,N_NAV_BUTTONS);
	m_ToolbarIcons.LoadIcons(TB_HOTIMAGELIST,Icons_Hot,N_NAV_BUTTONS);
	m_ToolbarIcons.LoadIcons(TB_DISABLEDIMAGELIST,Icons_Disabled,2);
}

void CNavBar::SetImageLists()
{
	m_ToolbarIcons.SetImageList(m_hWnd,TB_SETIMAGELIST,TB_IMAGELIST);
	m_ToolbarIcons.SetImageList(m_hWnd,TB_SETHOTIMAGELIST,TB_HOTIMAGELIST);
	if(IsWindowsVistaOrLater()) m_ToolbarIcons.SetImageList(m_hWnd,TB_SETPRESSEDIMAGELIST,TB_HOTIMAGELIST);
	m_ToolbarIcons.SetImageList(m_hWnd,TB_SETDISABLEDIMAGELIST,TB_DISABLEDIMAGELIST);
}

void CNavBar::UpdatePosition()
{
	int toolbarheight;
	int space;
	int posy; 

	toolbarheight=m_pToolbar->GetToolbarHeight();
	if(toolbarheight)
	{
		space=toolbarheight-BarSize.cy;
		posy=space/2; // Default: 4 pixel offset

		//if(space && (space%2 || (BarSize.cy-g_Scaled_NavButtonSize)%2)) posy++; // Center icon if padding is uneven

		if(IsBelowWindows8() && g_IE_MajorVersion>=9 && (g_Options2&OPTION2_EnableAeroTheme) && posy>0) posy--;

		::MoveWindow(m_hWnd,0,posy,BarSize.cx,BarSize.cy,FALSE);
	}
}

/*
LRESULT CNavBar::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT OptionButtons;

	// Buttons with images and text
	SendMessage(m_hWnd, TB_SETEXTENDEDSTYLE, 0, (LPARAM)TBSTYLE_EX_MIXEDBUTTONS);
	// Sets the size of the TBBUTTON structure.
	SendMessage(m_hWnd, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
	// Set the maximum number of text rows and bitmap size.
	SendMessage(m_hWnd, TB_SETMAXTEXTROWS, 1, 0L);
	// Set Padding
	SendMessage(m_hWnd, TB_SETPADDING, 0, CalculatePadding());

	// Set the toolbar's image list
	m_ToolbarIcons.SetImageList(m_hWnd,TB_SETIMAGELIST,TB_IMAGELIST);
	m_ToolbarIcons.SetImageList(m_hWnd,TB_SETHOTIMAGELIST,TB_HOTIMAGELIST);
	if(IsWindowsVistaOrLater()) m_ToolbarIcons.SetImageList(m_hWnd,TB_SETPRESSEDIMAGELIST,TB_HOTIMAGELIST);
	m_ToolbarIcons.SetImageList(m_hWnd,TB_SETDISABLEDIMAGELIST,TB_DISABLEDIMAGELIST);

	// Add the button for the toolbar to the window
	TBBUTTON Buttons[N_NAV_BUTTONS];
	
	ZeroMemory((void*)&Buttons, sizeof(Buttons));

	OptionButtons=g_Buttons;

	const UINT CommandIds[N_NAV_BUTTONS]={IDM_BACK,IDM_FORWARD,IDM_REFRESH,IDM_STOP,IDM_HOME};
	const int ButtonIds[N_NAV_BUTTONS]={BUTTON_BACK,BUTTON_FORWARD,BUTTON_REFRESH,BUTTON_STOP,BUTTON_HOME};
	int i;

	for(i=0;i<N_NAV_BUTTONS;i++)
	{
		Buttons[i].idCommand = CommandIds[i];
		Buttons[i].fsState = (OptionButtons&ButtonIds[i])?TBSTATE_ENABLED:TBSTATE_ENABLED|TBSTATE_HIDDEN;
		Buttons[i].fsStyle = BTNS_BUTTON | BTNS_AUTOSIZE;
		Buttons[i].iBitmap = i;
	}
	
	::SendMessage(m_hWnd, TB_ADDBUTTONS, N_NAV_BUTTONS, (LPARAM)&Buttons);

	// Adjust toolbar size
	::SendMessage(m_hWnd, TB_GETMAXSIZE, 0, (LPARAM)&BarSize);
	if(IsBelowWindowsXP()) CalculateBarSize(OptionButtons);

	UpdatePosition();

	// Add TTS_NOPREFIX to the tooltips style
	HWND hWndTooltips;
	hWndTooltips=(HWND)SendMessage(m_hWnd, TB_GETTOOLTIPS , 0, 0);
	if(hWndTooltips) ::SetWindowLong(hWndTooltips,GWL_STYLE,::GetWindowLong(hWndTooltips,GWL_STYLE)|TTS_NOPREFIX);

	return 0;
}*/

/*
LPARAM CNavBar::CalculatePadding()
{
	int padding=m_pToolbar->GetToolbarHeight()-g_Scaled_NavButtonSize;

	if(padding>=8) padding-=4;

	return MAKELPARAM(0,padding);
}*/
/*
void CNavBar::OnHeightChange(int height)
{
	SendMessage(m_hWnd, TB_SETPADDING, 0, CalculatePadding());
	SendMessage(m_hWnd,  TB_SETBUTTONSIZE, 0, MAKELPARAM(g_Scaled_NavButtonSize,g_Scaled_NavButtonSize));

	::SendMessage(m_hWnd, TB_GETMAXSIZE, 0, (LPARAM)&BarSize);
	if(IsBelowWindowsXP()) CalculateBarSize(g_Buttons);

	UpdatePosition();
}*/
/*
inline void CNavBar::UpdatePosition()
{
	int space=m_pToolbar->GetToolbarHeight()-BarSize.cy;
	int posy=space/2; // Default: 4 pixel offset

	if(space && (space%2 || (BarSize.cy-g_Scaled_NavButtonSize)%2)) posy++; // Center icon if padding is uneven

	::MoveWindow(m_hWnd,0,posy,BarSize.cx,BarSize.cy,FALSE);
}*/
/*
LRESULT CNavBar::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RECT tbRect;

	::SendMessage(m_hWnd, TB_SETROWS, MAKEWPARAM(1, FALSE), (LPARAM)&tbRect);
	::SendMessage(m_hWnd, TB_AUTOSIZE, 0,0);

	return 0;
}*/

/*LRESULT CNavBar::OnBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::SendMessage(m_pToolbar->m_hWnd,WM_PAINT,(WPARAM)wParam,0);

	bHandled=TRUE;
	return -1;
}*/

LRESULT CNavBar::OnRightClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT point;
	RECT rect;
	int CommandId;
	HWND hWndTooltips;

	point.x=LOWORD(lParam);
	point.y=HIWORD(lParam);

	CommandId=PointToCommandId(&point);

	// Remove tooltip
	hWndTooltips=(HWND)SendMessage(m_hWnd,TB_GETTOOLTIPS,0,0);
	if(hWndTooltips)
	{
		MSG msg;
		msg.hwnd=m_hWnd;
		msg.message=uMsg;
		msg.wParam=wParam;
		msg.lParam=lParam;
		msg.pt.x=LOWORD(lParam);
		msg.pt.y=HIWORD(lParam);
		msg.time=GetMessageTime();
		SendMessage(hWndTooltips,TTM_RELAYEVENT,0,(LPARAM)&msg); // TTM_POP fades tooltip
	}

	if(uMsg==WM_RBUTTONUP)
	{
		switch(CommandId)
		{
		case IDM_BACK:
		case IDM_FORWARD:
			ShowTravelLog(CommandId==IDM_BACK);
			break;
		default:
			if((g_Buttons&BUTTON_QUERO)==0)
			{
				SetRectEmpty(&rect);
				MapWindowPoints(HWND_DESKTOP,&point,1);
				m_pToolbar->OnQueroButtonClick(TPM_LEFTALIGN,&point,&rect);
			}
			else bHandled=FALSE;
		}
	}

	return 0;
}

LRESULT CNavBar::OnMiddleClickUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT p;
	int CommandId;

	p.x=GET_X_LPARAM(lParam); 
	p.y=GET_Y_LPARAM(lParam);

	// Get button index
	CommandId=PointToCommandId(&p);
	switch(CommandId)
	{
	case IDM_BACK:
	case IDM_FORWARD:
		GoBackForward(CommandId==IDM_BACK,SHORTCUT_OPTION_MIDDLECLICK|SHORTCUT_OPTION_QUERO_TOOLBAR);
		break;
	default:
		bHandled=FALSE;
	}

	return 0;
}

void CNavBar::ShowTravelLog(bool bRelativeBack)
{
	HMENU hTravelLogMenu;
	TPMPARAMS tpm;
	HRESULT hr;
	RECT rect;
	ITravelLogStg *pTravelLog;
	ITravelLogEntry *pTravelLogEntries[MAX_TRAVEL_LOG_ENTRIES];
	TCHAR *pTitle;
	TCHAR *pURL;
	ULONG n,i,result;
#ifdef COMPILE_FOR_WINDOWS_VISTA
	CMenuIcon MenuIcons[MAX_TRAVEL_LOG_ENTRIES];
	HICON hIcon;
#endif

	hTravelLogMenu=CreatePopupMenu();
	if(hTravelLogMenu)
	{
		n=GetTravelLogEntries(bRelativeBack,MAX_TRAVEL_LOG_ENTRIES,pTravelLogEntries,&pTravelLog);

		i=0;
		while(i<n)
		{
			hr=pTravelLogEntries[i]->GetTitle(&pTitle);
			if(SUCCEEDED_OK(hr) && pTitle)
			{
				// Truncate Title
				size_t len;
				StrCchLen(pTitle,MAXURLLENGTH,len);
				if(len>(128+3)) StringCchCopy(pTitle+128,4,L"...");

				AppendMenu(hTravelLogMenu,MF_ENABLED|MF_STRING,i+1,pTitle);

#ifdef COMPILE_FOR_WINDOWS_VISTA
				// Add favicon if available
				hr=pTravelLogEntries[i]->GetURL(&pURL);
				if(SUCCEEDED_OK(hr) && pURL)
				{
					hIcon=CFavIcon::LoadFavIconFromCache(pURL,true);
					if(hIcon)
					{
						MenuIcons[i].AddIconToMenuItem(hIcon,NULL,hTravelLogMenu,i+1,MF_BYCOMMAND);
						DestroyIcon(hIcon);
					}
					CoTaskMemFree(pURL);
				}
#endif
				CoTaskMemFree(pTitle);
			}
			i++;
		}
		if(i) AppendMenu(hTravelLogMenu,MF_SEPARATOR,0,NULL);
		AppendMenu(hTravelLogMenu,MF_ENABLED|MF_STRING,i+1,m_pToolbar->GetString(IDS_HISTORY));
		if((g_Buttons&BUTTON_QUERO)==0) AppendMenu(hTravelLogMenu,MF_ENABLED|MF_STRING,i+2,L"Quero");

		GetClientRect(&rect);
		if(bRelativeBack==false) rect.left+=NAVBUTTON_TOTAL_WIDTH;
		MapWindowPoints(HWND_DESKTOP,&rect);

		tpm.cbSize=sizeof TPMPARAMS;
		tpm.rcExclude=rect;

		g_MiddleClick=false;
		result=TrackPopupMenuEx(hTravelLogMenu,TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL|TPM_RETURNCMD,rect.left,rect.bottom,m_pToolbar->m_hWnd,&tpm);
		if(result>0 && result<=n)
		{
			UINT newWinTab=m_pToolbar->GetNewWinTabKeyState(g_MiddleClick?SHORTCUT_OPTION_MIDDLECLICK|SHORTCUT_OPTION_QUERO_TOOLBAR:SHORTCUT_OPTION_QUERO_TOOLBAR,OPEN_SameWindow);

			if(newWinTab)
			{
				pURL=NULL;
				hr=pTravelLogEntries[result-1]->GetURL(&pURL);
				if(SUCCEEDED_OK(hr) && pURL)
				{
					m_pToolbar->Quero(pURL,TYPE_ADDRESS,QUERO_REDIRECT,newWinTab);
					CoTaskMemFree(pURL);
				}
			}
			else pTravelLog->TravelTo(pTravelLogEntries[result-1]);
		}
		else if(result==n+1)
		{
			m_pToolbar->PostCommandToIE(g_IE_MajorVersion>=7?IE_COMMAND_HISTORY_CENTER:IE_COMMAND_HISTORY,false);
		}
		else if(result==n+2)
		{
			m_pToolbar->PostMessage(WM_QUERO_SHOWOPTIONS);
		}

		if(n)
		{
			for(i=0;i<n;i++) pTravelLogEntries[i]->Release();
			pTravelLog->Release();
		}
		
		DestroyMenu(hTravelLogMenu);
	}
}

UINT CNavBar::GetTravelLogEntries(bool bRelativeBack,UINT MaxTravelLogEntries,ITravelLogEntry *pTravelLogEntries[MAX_TRAVEL_LOG_ENTRIES],ITravelLogStg **ppTravelLog)
{
	IWebBrowser2 *pWebBrowser;
	IServiceProvider *pServiceProvider;
	IEnumTravelLogEntry *pEnumTravelLog;
	ULONG n;
	HRESULT hr;

	n=0;

	pWebBrowser=m_pToolbar->GetBrowser();

	if(pWebBrowser)
	{
		hr=pWebBrowser->QueryInterface(IID_IServiceProvider,(LPVOID*)&pServiceProvider);
		if(SUCCEEDED_OK(hr))
		{
			hr=pServiceProvider->QueryService(IID_ITravelLogStg,IID_ITravelLogStg,(LPVOID*)ppTravelLog);
			if(SUCCEEDED_OK(hr))
			{
				hr=(*ppTravelLog)->EnumEntries(bRelativeBack?TLEF_RELATIVE_BACK:TLEF_RELATIVE_FORE,&pEnumTravelLog);
				if(SUCCEEDED_OK(hr))
				{
					hr=pEnumTravelLog->Next(MaxTravelLogEntries,pTravelLogEntries,&n);
					if(FAILED(hr)) n=0; // hr!=S_OK && hr!=S_FALSE

					pEnumTravelLog->Release();
				}
				if(n==0) (*ppTravelLog)->Release();
			}
			pServiceProvider->Release();
		}
	}
					
	return n;
}

void CNavBar::GoBackForward(bool bRelativeBack,UINT ShortcutOptions)
{
	ITravelLogEntry *pTravelLogEntry;
	ITravelLogStg *pTravelLog;
	TCHAR *pURL;
	UINT n;

	UINT newWinTab=m_pToolbar->GetNewWinTabKeyState(ShortcutOptions,OPEN_SameWindow);

	if(newWinTab)
	{
		n=GetTravelLogEntries(bRelativeBack,1,&pTravelLogEntry,&pTravelLog);
		if(n)
		{
			pURL=NULL;
			if(SUCCEEDED_OK(pTravelLogEntry->GetURL(&pURL)) && pURL)
			{
				m_pToolbar->Quero(pURL,TYPE_ADDRESS,QUERO_REDIRECT,newWinTab);
				CoTaskMemFree(pURL);
			}
			pTravelLogEntry->Release();
			pTravelLog->Release();
		}
	}
	else
	{
		if(bRelativeBack) m_pToolbar->GetBrowser()->GoBack();
		else m_pToolbar->GetBrowser()->GoForward();
	}
}

TCHAR* CNavBar::GetTravelLogTooltip(TCHAR *pTooltip,size_t cbTooltip,bool bRelativeBack)
{
	ITravelLogEntry *pTravelLogEntry;
	ITravelLogStg *pTravelLog;
	TCHAR *pTitle;
	UINT n;

	n=GetTravelLogEntries(bRelativeBack,1,&pTravelLogEntry,&pTravelLog);
	if(n)
	{
		pTitle=NULL;
		if(SUCCEEDED_OK(pTravelLogEntry->GetTitle(&pTitle)) && pTitle)
		{
			// Truncate Title
			size_t len;
			StrCchLen(pTitle,MAXURLLENGTH,len);
			if(len>(128+3)) StringCchCopy(pTitle+128,4,L"...");

			StringCbPrintf(pTooltip,cbTooltip,m_pToolbar->GetString(bRelativeBack?IDS_HINT_BACK_TO:IDS_HINT_FORWARD_TO),pTitle);
		
			CoTaskMemFree(pTitle);
		}
		else n=0;

		pTravelLogEntry->Release();
		pTravelLog->Release();
	}

	if(n==0) StringCbCopy(pTooltip,cbTooltip,m_pToolbar->GetString(bRelativeBack?IDS_HINT_BACK:IDS_HINT_FORWARD));

	return pTooltip;
}

/*
void CNavBar::EnableButton(int id,BOOL enable)
{
	::SendMessage(m_hWnd,TB_ENABLEBUTTON,id,MAKELONG(enable,0));
	if(IsWindowsVistaOrLater()) RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_NOERASE|RDW_UPDATENOW);
}

void CNavBar::ShowButtons(UINT Buttons)
{
	const UINT CommandIds[N_NAV_BUTTONS]={IDM_BACK,IDM_FORWARD,IDM_REFRESH,IDM_STOP,IDM_HOME};
	const int ButtonIds[N_NAV_BUTTONS]={BUTTON_BACK,BUTTON_FORWARD,BUTTON_REFRESH,BUTTON_STOP,BUTTON_HOME};
	int i;

	for(i=0;i<N_NAV_BUTTONS;i++) ::SendMessage(m_hWnd, TB_HIDEBUTTON, CommandIds[i], MAKELONG((Buttons&ButtonIds[i])==0,0));

	// Adjust toolbar size
	::SendMessage(m_hWnd, TB_GETMAXSIZE, 0, (LPARAM)&BarSize);
	if(IsBelowWindowsXP()) CalculateBarSize(Buttons);
	::MoveWindow(m_hWnd,0,4,BarSize.cx,BarSize.cy,FALSE);
}


// Workaround for Windows 2000 and below
void CNavBar::CalculateBarSize(UINT Buttons)
{
	int n;

	n=0;
	if(Buttons&BUTTON_BACK) n++;
	if(Buttons&BUTTON_FORWARD) n++;
	if(Buttons&BUTTON_REFRESH) n++;
	if(Buttons&BUTTON_STOP) n++;
	if(Buttons&BUTTON_HOME) n++;

	BarSize.cx=n*NAVBUTTON_TOTAL_WIDTH;
}

*/