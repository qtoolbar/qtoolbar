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
// UIOptionsProfiles.h: interface for the CUIOptionsProfiles class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUIOPTIONSPROFILES_H__9252DE59_6FB4_4148_B272_309ADF768C38__INCLUDED_)
#define AFX_CUIOPTIONSPROFILES_H__9252DE59_6FB4_4148_B272_309ADF768C38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "QToolbar.h"
#include "UIListBox.h"
#include "UIEditInplace.h"

#define TXT_SEPARATOR L"---"

class CUIOptionsProfiles :
	public CPropertyPageImpl<CUIOptionsProfiles>
//	public CWinDataExchange<CUIOptionsProfiles>
{
public:
	CUIOptionsProfiles(CQToolbar *pToolbar);
	~CUIOptionsProfiles();

	enum { IDD = IDD_OPTIONS_PROFILES };

	BEGIN_MSG_MAP(CUIOptionsProfiles)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_LISTBOX_KEYDOWN, OnListBoxKeyDown)
		CHAIN_MSG_MAP(CPropertyPageImpl<CUIOptionsProfiles>)
	END_MSG_MAP()

	/*
	BEGIN_DDX_MAP(CUIOptionsProfiles)
		DDX_CHECK(IDC_CHKIDNWARNING, m_IDNWarning)
		DDX_CHECK(IDC_CHKMISSGLYPHSWARNING, m_MissGlyphsWarning)
		DDX_CHECK(IDC_CHKASCIIRULESWARNING, m_ASCIIRulesViolationWarning)
		DDX_CHECK(IDC_CHKHIGHLIGHTDOMAIN, m_HighlightDomain)
		DDX_CHECK(IDC_CHKDIGITASCIIINDICATOR, m_DigitAsciiIndicator)
	END_DDX_MAP()
	*/

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnListBoxKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	BOOL OnSetActive();
	BOOL OnKillActive();

	void SetRedrawListBoxes(bool bRedraw);
	
	void LoadProfiles(HKEY hKeyProfiles);
	bool SaveChanges();
	void FreeProfiles();
	void ShowProfile(UINT ProfileIndex,int EngineIndex=-1,bool bUpdateProfileList=false,bool bForceUpdateEngineList=false);
	SearchEngine* GetEngine(UINT EngineIndex);
	TCHAR* GetProfileName(UINT ProfileIndex);
	TCHAR* GetEngineName(UINT EngineIndex);
	void SwapProfiles(UINT ProfileIndex1,UINT ProfileIndex2);
	void SwapEngines(UINT EngineIndex1,UINT EngineIndex2);
	void ReorderEngines();
	void DeleteProfile(UINT ProfileIndex);
	void DeleteEngine(UINT EngineIndex);
	void InsertSeparator(UINT EngineIndex);
	void Reset();
	void Rename(bool bSearchEngine,UINT EngineIndex);
	void UpdateName(bool bSearchEngine,UINT EngineIndex);
	void AddProfile(UINT ProfileIndex);
	void AddEngine(UINT EngineIndex);
	void EditProfile(UINT ProfileIndex);
	void EditEngine(UINT EngineIndex);

	CUIListBox m_ProfilesListBox;
	CUIListBox m_EnginesListBox;
	CUIEditInplace m_Edit;

	CQToolbar *m_pToolbar;
	CProfiles m_Profiles;
	HKEY m_hKeyProfiles;
	TCHAR Name[MAX_PROFILE_NAME_LEN+MAX_SEARCHENGINE_NAME_LEN];
	
	Profile* Profiles[MAX_PROFILES];
	BYTE ProfileList[MAX_PROFILES];
	UINT ProfileCount;

	BYTE EngineIndexList[MAX_ENGINES];
	UINT EngineCount;

	DWORD Flags;
	bool bSaveProfile[MAX_PROFILES];
	BYTE SyncIds[MAX_PROFILES];
	BYTE ProfilesSyncId;

	UINT CurrentProfileIndex;
	int DefaultProfileId;

	bool bResetProfiles;
};

#endif // !defined(AFX_CUIOPTIONSPROFILES_H__9252DE59_6FB4_4148_B272_309ADF768C38__INCLUDED_)
