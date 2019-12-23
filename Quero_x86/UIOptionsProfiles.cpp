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
// UIOptionsProfiles.cpp: implementation of the CUIOptionsProfiles class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIOptionsProfiles.h"
#include "UISearchProfile.h"
#include "UISearchEngine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIOptionsProfiles::CUIOptionsProfiles(CQToolbar *pToolbar)
{
	m_pToolbar=pToolbar;

	ProfileCount=0;
	ProfilesSyncId=0;
	EngineCount=0;

	CurrentProfileIndex=MAX_PROFILES;

	m_hKeyProfiles=NULL;

	bResetProfiles=false;
}

CUIOptionsProfiles::~CUIOptionsProfiles()
{
	if(m_hKeyProfiles) RegCloseKey(m_hKeyProfiles);
	FreeProfiles();
}

void CUIOptionsProfiles::FreeProfiles()
{
	UINT i;

	for(i=0;i<ProfileCount;i++)
	{
		if(Profiles[i])
		{
			m_Profiles.FreeProfile(Profiles[i]);
			delete Profiles[i];
			Profiles[i]=NULL;
		}
	}
	ProfileCount=0;
	EngineCount=0;
}

LRESULT CUIOptionsProfiles::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_ProfilesListBox.SubclassWindow(GetDlgItem(IDC_PROFILES_LIST));
	m_EnginesListBox.SubclassWindow(GetDlgItem(IDC_ENGINES_LIST));

	m_ProfilesListBox.SetFont(m_pToolbar->hFont);
	m_EnginesListBox.SetFont(m_pToolbar->hFont);

	// Create the inplace edit control

	RECT rect;
	ZeroMemory(&rect,sizeof rect);
	m_Edit.Create(m_hWnd, rect, NULL, WS_CHILD|ES_AUTOHSCROLL|ES_LEFT|ES_NOHIDESEL,0,IDC_EDITBOX);
	m_Edit.SetFont(m_pToolbar->hFont);

	// Load profiles

	m_hKeyProfiles=m_Profiles.OpenProfiles(true,true);
	if(m_hKeyProfiles)
	{
		LoadProfiles(m_hKeyProfiles);
		Flags=m_Profiles.LoadFlags(m_hKeyProfiles,false);
	}

	return 1; // Let the system set the focus
}

void CUIOptionsProfiles::LoadProfiles(HKEY hKeyProfiles)
{
	UINT i;
	UINT DefaultProfileIndex;

	ProfileCount=0;
	if(hKeyProfiles)
	{
		m_Profiles.ProfileCount=m_Profiles.LoadList(hKeyProfiles,m_Profiles.ProfileList,MAX_PROFILES);
		m_Profiles.LoadDefaultProfileId(hKeyProfiles);

		DefaultProfileId=m_Profiles.DefaultProfileId;
		ProfileCount=m_Profiles.ProfileCount;

		i=0;
		while(i<ProfileCount)
		{
			ProfileList[i]=m_Profiles.ProfileList[i];
			Profiles[i]=new Profile;
			if(Profiles[i])
			{
				m_Profiles.InitProfile(Profiles[i]);
				m_Profiles.LoadProfile(hKeyProfiles,ProfileList[i],Profiles[i]);

				bSaveProfile[i]=false;
				SyncIds[i]=0;

				if(ProfileList[i]==DefaultProfileId) DefaultProfileIndex=i;
			}
			i++;
		}

		ProfilesSyncId=m_Profiles.LoadSyncId(hKeyProfiles);

		ShowProfile(DefaultProfileIndex,-1,true,true);
	}
}

BOOL CUIOptionsProfiles::OnSetActive()
{
	//DoDataExchange(FALSE);
	return TRUE;
}

void CUIOptionsProfiles::SetRedrawListBoxes(bool bRedraw)
{
	if(bRedraw)
	{
		m_ProfilesListBox.SetRedraw(TRUE);
		m_EnginesListBox.SetRedraw(TRUE);
		m_ProfilesListBox.RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_NOERASE|RDW_FRAME|RDW_UPDATENOW);
		m_EnginesListBox.RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_NOERASE|RDW_FRAME|RDW_UPDATENOW);
	}
	else
	{
		m_ProfilesListBox.SetRedraw(FALSE);
		m_EnginesListBox.SetRedraw(FALSE);
	}
}

LRESULT CUIOptionsProfiles::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT ProfileIndex;
	UINT EngineIndex;
	bool bFocusOnEngineListBox;
	BYTE OldDefaultId;
	SearchEngine *pEngine;
	TCHAR *pName;
	bool bRename;
	RECT rect;

	ProfileIndex=m_ProfilesListBox.GetCurSel();
	EngineIndex=m_EnginesListBox.GetCurSel();

	bFocusOnEngineListBox=(EngineIndex!=LB_ERR);

	switch(LOWORD(wParam))
	{
	case IDC_PROFILES_LIST:
		if(HIWORD(wParam)==LBN_SELCHANGE)
		{
			ShowProfile(ProfileIndex);
		}
		else if(HIWORD(wParam)==LBN_DBLCLK)
		{
			EditProfile(ProfileIndex);
		}
		break;

	case IDC_ENGINES_LIST:
		if(HIWORD(wParam)==LBN_DBLCLK)
		{
			EditEngine(EngineIndex);
		}
		break;

	case IDC_PROFILES_MOVE_UP:
		SetRedrawListBoxes(false);
		if(bFocusOnEngineListBox)
		{
			if(EngineIndex>=1 && EngineIndex<EngineCount && CurrentProfileIndex<ProfileCount)
			{
				SwapEngines(EngineIndex-1,EngineIndex);
				m_EnginesListBox.DeleteString(EngineIndex-1);
				m_EnginesListBox.InsertString(EngineIndex,GetEngineName(EngineIndex));
				m_EnginesListBox.SetTopIndex(EngineIndex>=5?EngineIndex-5:0);				
			}
		}
		else if(ProfileIndex>=1 && ProfileIndex<ProfileCount)
		{
			SwapProfiles(ProfileIndex-1,ProfileIndex);
			m_ProfilesListBox.DeleteString(ProfileIndex-1);
			m_ProfilesListBox.InsertString(ProfileIndex,GetProfileName(ProfileIndex));
			m_ProfilesListBox.SetTopIndex(ProfileIndex>=5?ProfileIndex-5:0);
		}
		SetRedrawListBoxes(true);
		break;

	case IDC_PROFILES_MOVE_DOWN:
		SetRedrawListBoxes(false);
		if(bFocusOnEngineListBox)
		{
			if(EngineIndex<(EngineCount-1) && CurrentProfileIndex<ProfileCount)
			{
				SwapEngines(EngineIndex,EngineIndex+1);
				m_EnginesListBox.DeleteString(EngineIndex+1);
				m_EnginesListBox.InsertString(EngineIndex,GetEngineName(EngineIndex));
				m_EnginesListBox.SetTopIndex(EngineIndex>=5?EngineIndex-5:0);
			}
		}
		else if(ProfileIndex<(ProfileCount-1))
		{
			SwapProfiles(ProfileIndex,ProfileIndex+1);			
			m_ProfilesListBox.DeleteString(ProfileIndex+1);
			m_ProfilesListBox.InsertString(ProfileIndex,GetProfileName(ProfileIndex));
			m_ProfilesListBox.SetTopIndex(ProfileIndex>=5?ProfileIndex-5:0);
		}
		SetRedrawListBoxes(true);
		break;

	case IDC_PROFILES_DELETE:
		if(bFocusOnEngineListBox)
		{
			if(EngineIndex<EngineCount)
			{
				DeleteEngine(EngineIndex);
				m_EnginesListBox.DeleteString(EngineIndex);
				if(EngineCount)
				{
					if(EngineIndex>=EngineCount) EngineIndex--;
					m_EnginesListBox.SetCurSel(EngineIndex);
				}
			}
		}
		else
		{
			if(ProfileIndex<ProfileCount)
			{
				DeleteProfile(ProfileIndex);
				m_ProfilesListBox.DeleteString(ProfileIndex);
				m_EnginesListBox.ResetContent();
				if(ProfileCount)
				{
					if(ProfileIndex>=ProfileCount) ProfileIndex--;
					m_ProfilesListBox.SetCurSel(ProfileIndex);
					ShowProfile(ProfileIndex);
				}
			}
		}
		break;

	case IDC_PROFILES_INSERT_SEPARATOR:
		if(CurrentProfileIndex<ProfileCount)
		{
			if(EngineCount+1<MAX_ENGINES)
			{
				if(EngineIndex>EngineCount || bFocusOnEngineListBox==false) EngineIndex=EngineCount;
				else EngineIndex++;
				InsertSeparator(EngineIndex);

				m_EnginesListBox.InsertString(EngineIndex,GetEngineName(EngineIndex));
				m_EnginesListBox.SetCurSel(EngineIndex);
			}
			else MessageBox(m_pToolbar->GetString(IDS_ERR_SE_OVERFLOW),L"Quero Toolbar",MB_ICONWARNING|MB_OK);
		}
		break;

	case IDC_PROFILES_SET_DEFAULT:
		SetRedrawListBoxes(false);
		if(bFocusOnEngineListBox)
		{
			pEngine=GetEngine(EngineIndex);
			if(pEngine)
			{
				OldDefaultId=Profiles[CurrentProfileIndex]->DefaultEngineId;
				Profiles[CurrentProfileIndex]->DefaultEngineId=pEngine->id;
				if(OldDefaultId!=pEngine->id)
				{
					ShowProfile(CurrentProfileIndex,EngineIndex,false,true);
					bSaveProfile[CurrentProfileIndex]=true;
				}
			}
		}
		else
		{
			if(ProfileIndex<ProfileCount)
			{
				OldDefaultId=DefaultProfileId;
				DefaultProfileId=Profiles[ProfileIndex]->id;
				if(OldDefaultId!=DefaultProfileId)
				{
					ShowProfile(CurrentProfileIndex,EngineIndex,true,true);
				}
			}
		}
		SetRedrawListBoxes(true);
		break;

	case IDC_PROFILES_RESET:
		Reset();
		break;

	case IDC_PROFILES_RENAME:
		if(m_Edit.IsWindowVisible())
		{
			Rename(bFocusOnEngineListBox,EngineIndex);
		}
		else
		{
			bRename=false;
			if(bFocusOnEngineListBox)
			{
				pEngine=GetEngine(EngineIndex);
				if(pEngine)
				{
					pName=pEngine->Name;
					m_EnginesListBox.GetItemRect(EngineIndex,&rect);
					m_Edit.SetParent(m_EnginesListBox.m_hWnd);
					m_Edit.LimitText(MAX_SEARCHENGINE_NAME_LEN-1);
					bRename=true;
				}
			}
			else if(ProfileIndex<ProfileCount && Profiles[ProfileIndex])
			{
				pName=Profiles[ProfileIndex]->Name;
				m_ProfilesListBox.GetItemRect(ProfileIndex,&rect);
				m_Edit.SetParent(m_ProfilesListBox.m_hWnd);
				m_Edit.LimitText(MAX_PROFILE_NAME_LEN-1);
				bRename=true;
			}
			if(bRename)
			{				
				m_Edit.MoveWindow(&rect);
				m_Edit.SetMargins(2,2);
				m_Edit.SetWindowText(pName?pName:L"");
				m_Edit.SetSel(0,-1,TRUE);
				m_Edit.ShowWindow(SW_SHOW);
				m_Edit.SetFocus();
			}
		}
		break;

	case IDC_EDITBOX:
		if(HIWORD(wParam)==EN_KILLFOCUS)
		{
			Rename(bFocusOnEngineListBox,EngineIndex);
		}
		break;

	case IDC_PROFILES_NEW:
		if(bFocusOnEngineListBox || (CurrentProfileIndex<ProfileCount && EngineCount==0))
		{
			if(EngineCount+1<MAX_ENGINES)
			{
				AddEngine(EngineIndex+1);
			}
			else MessageBox(m_pToolbar->GetString(IDS_ERR_SE_OVERFLOW),L"Quero Toolbar",MB_ICONWARNING|MB_OK);
		}
		else
		{
			if(ProfileCount<MAX_PROFILES)
			{
				AddProfile(ProfileIndex+1);
			}
			else MessageBox(m_pToolbar->GetString(IDS_ERR_PROFILES_OVERFLOW),L"Quero Toolbar",MB_ICONWARNING|MB_OK);
		}
		break;

	case IDC_PROFILES_EDIT:
		if(bFocusOnEngineListBox) EditEngine(EngineIndex);
		else EditProfile(ProfileIndex);
		break;
	}

	bHandled=FALSE;
	return 0;
}

LRESULT CUIOptionsProfiles::OnListBoxKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT result;
	bool bFocusOnEngineListBox;

	bFocusOnEngineListBox=((HWND)lParam==m_EnginesListBox.m_hWnd);

	result=0;

	switch(wParam)
	{
	case VK_DELETE:
		if(bFocusOnEngineListBox==false) m_EnginesListBox.SetCurSel(-1);
		PostMessage(WM_COMMAND,IDC_PROFILES_DELETE);
		break;
	case VK_F2:
		if(bFocusOnEngineListBox==false) m_EnginesListBox.SetCurSel(-1);
		PostMessage(WM_COMMAND,IDC_PROFILES_RENAME);
		break;
	case VK_LEFT:
	case VK_RIGHT:
		if(bFocusOnEngineListBox)
		{
			m_ProfilesListBox.SetFocus();
			m_EnginesListBox.SetCurSel(-1);
		}
		else
		{
			if(EngineCount) m_EnginesListBox.SetCurSel(0);
			m_EnginesListBox.SetFocus();
		}
		break;
	default:
		result=1;
	}
	
	return result;
}

BOOL CUIOptionsProfiles::OnKillActive()
{
	//DoDataExchange(TRUE);
	return TRUE;
}

SearchEngine* CUIOptionsProfiles::GetEngine(UINT EngineIndex)
{
	SearchEngine *pEngine;

	if(CurrentProfileIndex<ProfileCount && EngineIndex<EngineCount && EngineIndexList[EngineIndex]<Profiles[CurrentProfileIndex]->EngineCount)
		pEngine=&Profiles[CurrentProfileIndex]->Engines[EngineIndexList[EngineIndex]];
	else
		pEngine=NULL;

	return pEngine;
}

TCHAR* CUIOptionsProfiles::GetProfileName(UINT ProfileIndex)
{
	TCHAR *pName;
	int ProfileId;

	ProfileId=ProfileList[ProfileIndex];

	if(Profiles[ProfileIndex])
	{
		pName=Profiles[ProfileIndex]->Name;
		if(pName==NULL) pName=L"?";
	}
	else pName=L"?";

	if(ProfileId==DefaultProfileId)
	{
		StringCbCopy(Name,sizeof Name,pName);
		StringCbCat(Name,sizeof Name,m_pToolbar->GetString(IDS_PROFILES_DEFAULT));
		pName=Name;
	}

	return pName;
}

TCHAR* CUIOptionsProfiles::GetEngineName(UINT EngineIndex)
{
	TCHAR *pName;
	SearchEngine *pEngine;

	if(CurrentProfileIndex<ProfileCount && EngineIndex<EngineCount)
	{
		if(EngineIndexList[EngineIndex]==SEARCHENGINE_ID_SEPARATOR)
		{
			StringCbCopy(Name,sizeof Name,TXT_SEPARATOR);
			pName=Name;
		}
		else
		{
			pEngine=GetEngine(EngineIndex);
			if(pEngine)
			{
				if(pEngine->id==Profiles[CurrentProfileIndex]->DefaultEngineId)
				{
					StringCbCopy(Name,sizeof Name,pEngine->Name);
					StringCbCat(Name,sizeof Name,m_pToolbar->GetString(IDS_PROFILES_DEFAULT));
					pName=Name;
				}
				else pName=pEngine->Name;
			}
			else pName=L"?";			
		}
	}
	else pName=L"?";

	return pName;
}

void CUIOptionsProfiles::ShowProfile(UINT ProfileIndex,int EngineIndex,bool bUpdateProfileList,bool bForceUpdateEngineList)
{
	UINT i;

	SetRedrawListBoxes(false);
	if(ProfileIndex!=CurrentProfileIndex || bUpdateProfileList || bForceUpdateEngineList)
	{
		// Reorder current profile
		ReorderEngines();

		CurrentProfileIndex=ProfileIndex;

		//Update profile list
		if(bUpdateProfileList)
		{
			m_ProfilesListBox.ResetContent();
			i=0;
			while(i<ProfileCount)
			{
				m_ProfilesListBox.AddString(GetProfileName(i));
				i++;
			}
		}

		// Update engine list
		if(ProfileIndex<ProfileCount && Profiles[ProfileIndex])
		{			
			m_EnginesListBox.ResetContent();
			i=0;
			EngineCount=0;
			while(i<Profiles[ProfileIndex]->EngineCount)
			{
				EngineIndexList[EngineCount++]=i;
				m_EnginesListBox.AddString(GetEngineName(EngineCount-1));
				if(Profiles[ProfileIndex]->Engines[i].HasSeparator)
				{
					EngineIndexList[EngineCount++]=SEARCHENGINE_ID_SEPARATOR;
					m_EnginesListBox.AddString(TXT_SEPARATOR);
				}
				i++;
			}
		}
	}
	
	m_ProfilesListBox.SetCurSel(ProfileIndex);
	m_EnginesListBox.SetCurSel(EngineIndex);
	SetRedrawListBoxes(true);
}

void CUIOptionsProfiles::SwapProfiles(UINT ProfileIndex1,UINT ProfileIndex2)
{
	BYTE i;
	Profile *pProfile;
	bool b;

	i=ProfileList[ProfileIndex1];
	ProfileList[ProfileIndex1]=ProfileList[ProfileIndex2];
	ProfileList[ProfileIndex2]=i;

	pProfile=Profiles[ProfileIndex1];
	Profiles[ProfileIndex1]=Profiles[ProfileIndex2];
	Profiles[ProfileIndex2]=pProfile;

	b=bSaveProfile[ProfileIndex1];
	bSaveProfile[ProfileIndex1]=bSaveProfile[ProfileIndex2];
	bSaveProfile[ProfileIndex2]=b;

	if(ProfileIndex1==CurrentProfileIndex) CurrentProfileIndex=ProfileIndex2;
	else if(ProfileIndex2==CurrentProfileIndex) CurrentProfileIndex=ProfileIndex1;

	Flags|=PROFILES_LIST_MODIFIED;
}

void CUIOptionsProfiles::SwapEngines(UINT EngineIndex1,UINT EngineIndex2)
{
	BYTE i;

	i=EngineIndexList[EngineIndex1];
	EngineIndexList[EngineIndex1]=EngineIndexList[EngineIndex2];
	EngineIndexList[EngineIndex2]=i;

	if(CurrentProfileIndex<ProfileCount)
	{
		Profiles[CurrentProfileIndex]->Flags|=PROFILE_LIST_MODIFIED;
		bSaveProfile[CurrentProfileIndex]=true;
	}
}

void CUIOptionsProfiles::ReorderEngines()
{
	SearchEngine Engines[MAX_ENGINES];
	UINT i,j;

	if(CurrentProfileIndex<ProfileCount)
	{
		i=0;
		j=0;
		while(i<EngineCount)
		{
			if(EngineIndexList[i]==SEARCHENGINE_ID_SEPARATOR)
			{
				if(j>0) Engines[j-1].HasSeparator=true;
			}
			else
			{
				Engines[j]=Profiles[CurrentProfileIndex]->Engines[EngineIndexList[i]];
				Engines[j].HasSeparator=false;
				EngineIndexList[i]=j;
				j++;
			}
			i++;
		}

		i=0;
		while(i<j)
		{
			Profiles[CurrentProfileIndex]->Engines[i]=Engines[i];
			i++;
		}

		Profiles[CurrentProfileIndex]->EngineCount=j;
	}
}

void CUIOptionsProfiles::DeleteProfile(UINT ProfileIndex)
{
	UINT i;
	BYTE id;

	if(ProfileIndex<ProfileCount)
	{
		if(ProfileList[ProfileIndex]==DefaultProfileId) DefaultProfileId=-1;
		if(Profiles[ProfileIndex])
		{
			id=Profiles[ProfileIndex]->id+1;
			if(ProfilesSyncId<id && id<=PROFILE_CUSTOM_ID) ProfilesSyncId=id;
			m_Profiles.FreeProfile(Profiles[ProfileIndex]);
		}
		ProfileCount--;
		i=ProfileIndex;
		while(i<ProfileCount)
		{
			Profiles[i]=Profiles[i+1];
			ProfileList[i]=ProfileList[i+1];
			SyncIds[i]=SyncIds[i+1];
			bSaveProfile[i]=bSaveProfile[i+1];
			i++;
		}
		Flags|=PROFILES_LIST_MODIFIED;
		if(CurrentProfileIndex==ProfileIndex) CurrentProfileIndex=MAX_PROFILES;
	}
}

void CUIOptionsProfiles::DeleteEngine(UINT EngineIndex)
{
	UINT i;
	BYTE id;
	SearchEngine *pEngine;

	if(EngineIndex<EngineCount && CurrentProfileIndex<ProfileCount)
	{
		if(EngineIndexList[EngineIndex]!=SEARCHENGINE_ID_SEPARATOR)
		{
			pEngine=GetEngine(EngineIndex);
			if(pEngine)
			{
				id=pEngine->id+1;
				if(SyncIds[CurrentProfileIndex]<id && id<=SEARCHENGINE_CUSTOM_ID) SyncIds[CurrentProfileIndex]=id;
				m_Profiles.FreeEngine(pEngine);
			}
		}
		EngineCount--;
		i=EngineIndex;
		while(i<EngineCount)
		{
			EngineIndexList[i]=EngineIndexList[i+1];
			i++;
		}
		ReorderEngines();
		Profiles[CurrentProfileIndex]->Flags|=PROFILE_LIST_MODIFIED;
		bSaveProfile[CurrentProfileIndex]=true;
	}
}

void CUIOptionsProfiles::InsertSeparator(UINT EngineIndex)
{
	UINT i;

	if(EngineIndex<=EngineCount && EngineCount+1<MAX_ENGINES)
	{
		i=EngineCount;
		while(i>EngineIndex)
		{
			EngineIndexList[i]=EngineIndexList[i-1];
			i--;
		}
		EngineIndexList[EngineIndex]=SEARCHENGINE_ID_SEPARATOR;
		EngineCount++;
	}

	if(CurrentProfileIndex<ProfileCount) Profiles[CurrentProfileIndex]->Flags|=PROFILE_LIST_MODIFIED;
	bSaveProfile[CurrentProfileIndex]=true;
}

void CUIOptionsProfiles::Reset()
{
	HKEY hKeyProfiles;
	UINT i;

	m_ProfilesListBox.ResetContent();
	m_EnginesListBox.ResetContent();
	FreeProfiles();
	CurrentProfileIndex=MAX_PROFILES;
	DefaultProfileId=m_Profiles.DefaultProfileId;
	Flags=PROFILES_DEFAULT_FLAGS;
	ProfilesSyncId=0;

	hKeyProfiles=m_Profiles.OpenProfiles(false,false);
	if(hKeyProfiles)
	{
		LoadProfiles(hKeyProfiles);
		i=0;
		while(i<ProfileCount)
		{
			bSaveProfile[i]=true;
			i++;
		}
		RegCloseKey(hKeyProfiles);
	}

	bResetProfiles=true;
}

void CUIOptionsProfiles::Rename(bool bSearchEngine,UINT EngineIndex)
{
	BSTR bstrName=NULL;
	SearchEngine *pEngine;

	// Get the new name

	if(m_Edit.GetModify())
	{
		if(m_Edit.GetWindowText(Name,MAX_SEARCHENGINE_NAME_LEN))
		{
			CQToolbar::trim(Name);
			if(Name[0]) bstrName=SysAllocString(Name);
		}
	}

	// Replace the name

	if(bSearchEngine)
	{
		pEngine=GetEngine(EngineIndex);
		if(bstrName && pEngine && pEngine->Name)
		{
			if(StrCmp(bstrName,pEngine->Name))
			{
				SysFreeString(pEngine->Name);
				pEngine->Name=bstrName;
				pEngine->Flags|=SEARCHENGINE_CUSTOM;
				bstrName=NULL;
				bSaveProfile[CurrentProfileIndex]=true;
			}
		}
		m_EnginesListBox.SetFocus();
	}
	else
	{
		if(bstrName && CurrentProfileIndex<ProfileCount && Profiles[CurrentProfileIndex]->Name)
		{
			if(StrCmp(bstrName,Profiles[CurrentProfileIndex]->Name))
			{
				SysFreeString(Profiles[CurrentProfileIndex]->Name);
				Profiles[CurrentProfileIndex]->Name=bstrName;
				Profiles[CurrentProfileIndex]->Flags|=PROFILE_NAME_MODIFIED;
				bstrName=NULL;
				bSaveProfile[CurrentProfileIndex]=true;
			}
		}
		m_ProfilesListBox.SetFocus();
	}

	if(bstrName) SysFreeString(bstrName);
	else UpdateName(bSearchEngine,EngineIndex);

	m_Edit.ShowWindow(SW_HIDE);
}

void CUIOptionsProfiles::UpdateName(bool bSearchEngine,UINT EngineIndex)
{
	SetRedrawListBoxes(false);
	if(bSearchEngine)
	{
		if(EngineIndex<EngineCount)
		{
			m_EnginesListBox.DeleteString(EngineIndex);
			m_EnginesListBox.InsertString(EngineIndex,GetEngineName(EngineIndex));
			m_EnginesListBox.SetCurSel(EngineIndex);
		}
	}
	else
	{
		if(CurrentProfileIndex<ProfileCount)
		{
			m_ProfilesListBox.DeleteString(CurrentProfileIndex);
			m_ProfilesListBox.InsertString(CurrentProfileIndex,GetProfileName(CurrentProfileIndex));
			m_ProfilesListBox.SetCurSel(CurrentProfileIndex);
		}
	}
	SetRedrawListBoxes(true);
}

void CUIOptionsProfiles::AddProfile(UINT ProfileIndex)
{
	bool result=false;

	if(ProfileCount+1<MAX_PROFILES)
	{
		Profile *pNewProfile;
		UINT i;

		pNewProfile=new Profile;
		if(pNewProfile)
		{
			m_Profiles.InitProfile(pNewProfile);
			pNewProfile->id=m_Profiles.NewCustomProfileId(ProfileList,ProfileCount);
			pNewProfile->Flags|=PROFILE_CUSTOM;
			if(pNewProfile->id!=PROFILE_ID_ERROR)
			{
				CUISearchProfile UISearchProfile(pNewProfile,0,m_pToolbar);

				if(UISearchProfile.DoModal()==IDOK)
				{
					if(ProfileIndex>ProfileCount) ProfileIndex=ProfileCount;		
					i=ProfileCount;
					while(i>ProfileIndex)
					{
						Profiles[i]=Profiles[i-1];
						ProfileList[i]=ProfileList[i-1];
						SyncIds[i]=SyncIds[i-1];
						bSaveProfile[i]=bSaveProfile[i-1];
						i--;
					}

					Profiles[ProfileIndex]=pNewProfile;
					ProfileList[ProfileIndex]=pNewProfile->id;
					SyncIds[ProfileIndex]=0;
					bSaveProfile[ProfileIndex]=true;
					ProfileCount++;

					Flags|=PROFILES_LIST_MODIFIED;
					bSaveProfile[ProfileIndex]=true;

					m_ProfilesListBox.InsertString(ProfileIndex,GetProfileName(ProfileIndex));
					m_ProfilesListBox.SetCurSel(ProfileIndex);
					ShowProfile(ProfileIndex);
					result=true;
				}
			}
			
			if(result==false)
			{
				m_Profiles.FreeProfile(pNewProfile);
				delete pNewProfile;
			}
		}
	}
}

void CUIOptionsProfiles::AddEngine(UINT EngineIndex)
{
	bool result=false;

	if(CurrentProfileIndex<ProfileCount && Profiles[CurrentProfileIndex] && EngineCount+1<MAX_PROFILES)
	{
		SearchEngine NewEngine;
		UINT i;

		m_Profiles.InitEngine(&NewEngine);
		NewEngine.id=m_Profiles.NewCustomEngineId(Profiles[CurrentProfileIndex]);
		NewEngine.Flags|=SEARCHENGINE_CUSTOM;
		if(NewEngine.id!=SEARCHENGINE_ID_ERROR)
		{
			CUISearchEngine UISearchEngine(&NewEngine,0,m_pToolbar);

			if(UISearchEngine.DoModal()==IDOK)
			{
				if(EngineIndex>EngineCount) EngineIndex=EngineCount;
				i=EngineCount;
				while(i>EngineIndex)
				{
					EngineIndexList[i]=EngineIndexList[i-1];
					i--;
				}

				EngineIndexList[EngineIndex]=EngineCount;
				Profiles[CurrentProfileIndex]->Engines[EngineCount]=NewEngine;
				EngineCount++;
				ReorderEngines();

				Profiles[CurrentProfileIndex]->Flags|=PROFILE_LIST_MODIFIED;
				bSaveProfile[CurrentProfileIndex]=true;

				m_EnginesListBox.InsertString(EngineIndex,GetEngineName(EngineIndex));
				m_EnginesListBox.SetCurSel(EngineIndex);
				result=true;
			}
		}
		
		if(result==false) m_Profiles.FreeEngine(&NewEngine);
	}
}

void CUIOptionsProfiles::EditProfile(UINT ProfileIndex)
{
	if(ProfileIndex<ProfileCount && Profiles[ProfileIndex])
	{
		CUISearchProfile UISearchProfile(Profiles[ProfileIndex],IDS_EDIT_SEARCH_PROFILE,m_pToolbar);

		if(UISearchProfile.DoModal()==IDOK)
		{
			bSaveProfile[ProfileIndex]=true;
			UpdateName(false,0);
		}
	}
}

void CUIOptionsProfiles::EditEngine(UINT EngineIndex)
{
	SearchEngine *pEngine;

	pEngine=GetEngine(EngineIndex);
	if(pEngine)
	{
		CUISearchEngine UISearchEngine(pEngine,IDS_EDIT_SEARCH_ENGINE,m_pToolbar);

		if(UISearchEngine.DoModal()==IDOK)
		{
			bSaveProfile[CurrentProfileIndex]=true;
			UpdateName(true,EngineIndex);
		}
	}
}

bool CUIOptionsProfiles::SaveChanges()
{
	bool result;
	bool bSaveList;
	UINT i;
	HKEY hKey;
	BYTE SyncId;
	int ProfileId;

	result=false;

	// Delete profiles
	if(m_hKeyProfiles)
	{
		if(bResetProfiles)
		{
			RegCloseKey(m_hKeyProfiles);
			hKey=m_pToolbar->OpenQueroKey(HKEY_CURRENT_USER,NULL,true);
			if(hKey)
			{
				SHDeleteKey(hKey,L"Profiles");
				result=true;
				RegCloseKey(hKey);
			}
			m_hKeyProfiles=m_Profiles.OpenProfiles(true,true);
		}
		else
		{
			ProfileId=m_Profiles.First();
			while(ProfileId!=-1)
			{
				if(m_Profiles.IsInList(ProfileId,ProfileList,ProfileCount)==false)
				{
					m_Profiles.DeleteSubKey(m_hKeyProfiles,ProfileId);
					result=true;
				}
				ProfileId=m_Profiles.Next();
			}
		}
	}

	// Save profiles
	if(m_hKeyProfiles)
	{
		// Update current profile
		ReorderEngines();

		// Save profiles
		i=0;
		while(i<ProfileCount)
		{
			if(bSaveProfile[i] && Profiles[i])
			{
				// Save DefaultEngineId
				m_Profiles.SaveDefaultEngineId(Profiles[i],true);
				
				// Save profile
				m_Profiles.SaveProfile(m_hKeyProfiles,Profiles[i],false);
				
				// Update SyncId
				if(SyncIds[i])
				{
					hKey=m_Profiles.OpenSubKey(m_hKeyProfiles,ProfileList[i],true);
					if(hKey)
					{
						SyncId=m_Profiles.LoadSyncId(hKey);
						if(SyncIds[i]>SyncId) m_Profiles.SaveSyncId(hKey,SyncIds[i]);
						RegCloseKey(hKey);
					}
				}

				result=true;
			}
			i++;
		}

		// Update overall SyncId
		m_Profiles.SaveSyncId(m_hKeyProfiles,ProfilesSyncId);

		// Save profile list
		if(ProfileCount==m_Profiles.ProfileCount && bResetProfiles==false)
		{
			i=0;
			while(i<ProfileCount && ProfileList[i]==m_Profiles.ProfileList[i]) i++;
			bSaveList=(i<ProfileCount);
		}
		else bSaveList=true;
		if(bSaveList)
		{
			m_Profiles.SaveList(m_hKeyProfiles,ProfileList,ProfileCount);
			result=true;
		}

		// Save flags
		m_Profiles.SaveFlags(m_hKeyProfiles,Flags,PROFILES_DEFAULT_FLAGS);

		// Save DefaultProfileId
		m_Profiles.LoadProfileList();
		if(DefaultProfileId!=m_Profiles.DefaultProfileId) result=true;
		m_Profiles.SaveDefaultProfileId(DefaultProfileId,true);
	}

	return result;
}