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
// Profiles.cpp: implementation of the CProfiles class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Profiles.h"
#include "QToolbar.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProfiles::CProfiles()
{
	ProfileCount=0;
	bNamesLoaded=false;
	InitProfile(&CurrentProfile);
	DefaultProfileId=-1;

	/* Test Case
	HKEY hKey=OpenProfiles(true,false);
	HKEY hKeyProfile=OpenSubKey(hKey,0,false);

	SearchEngine se;

	QD(L"LoadProfile");
	LoadProfile(hKey,0,&CurrentProfile);
	QDEBUG_PRINTF(L"EngineCount",L"%d",CurrentProfile.EngineCount);

	//SaveProfile(hKey,7,Engines,EngineCount,true);

	if(LoadEngine(hKeyProfile,0,&se))
	{
		QDEBUG_PRINT(L"se",se.Name);
		QDEBUG_PRINT(L"se",se.QueryURL);
		QDEBUG_PRINT(L"se",se.InterceptURL);
		QDEBUG_PRINTF(L"se",L"%d",se.ParamCount);
	}

	BSTR url;

	se.ParamCount=2;
	se.QueryURL=L"searchTerms={searchTerms}&p1={Quero:p1}&p2={Quero:p2}&year={Quero:Year}";
	se.ParamNames[0]=NULL;
	se.ParamNames[1]=NULL;
	se.ParamCount=0;
	InitParams(&se,se.QueryURL);
	//QDEBUG_PRINT(L"InitParams",se.ParamNames[0]);
	PrepareNavigation(L"test ä 123",&se,&url,NULL);
	QDEBUG_PRINT(L"PrepareNavigation",url);

	TCHAR terms[MAXURLLENGTH];
	InterceptSearchTerms(url,&se,terms);
	QDEBUG_PRINT(L"InterceptedSearchTerms",terms);

	RegCloseKey(hKeyProfile);
	RegCloseKey(hKey);
	// */
}

CProfiles::~CProfiles()
{
	FreeProfile(&CurrentProfile);
	FreeNames();
}

void CProfiles::InitProfile(Profile *pProfile)
{
	pProfile->Name=NULL;
	InitEngine(&pProfile->LuckySearch);
	pProfile->EngineCount=0;
}

void CProfiles::InitEngine(SearchEngine *pEngine)
{
	ZeroMemory(pEngine,sizeof SearchEngine);
	pEngine->id=SEARCHENGINE_ID_FREE;
	pEngine->Flags=SEARCHENGINE_DEFAULT_FLAGS;
}

void CProfiles::AssignEngine(SearchEngine *pEngineDest,SearchEngine *pEngineSrc)
{
	CopyMemory(pEngineDest,pEngineSrc,sizeof SearchEngine);
}

void CProfiles::AssignProfile(Profile *pProfileDest,Profile *pProfileSrc)
{
	CopyMemory(pProfileDest,pProfileSrc,sizeof Profile);
}

void CProfiles::CopyEngine(SearchEngine *pEngineDest,SearchEngine *pEngineSrc)
{
	UINT i;

	ZeroMemory(pEngineDest,sizeof SearchEngine);
	pEngineDest->id=pEngineSrc->id;
	pEngineDest->Flags=pEngineSrc->Flags;
	pEngineDest->HasSeparator=pEngineSrc->HasSeparator;
	pEngineDest->ParamCount=pEngineSrc->ParamCount;
	pEngineDest->iRequiresAddress=pEngineSrc->iRequiresAddress;
	pEngineDest->bRequiresKeywords=pEngineSrc->bRequiresKeywords;
	for(i=0;i<pEngineSrc->ParamCount;i++) if(pEngineSrc->ParamNames[i]) pEngineDest->ParamNames[i]=SysAllocString(pEngineSrc->ParamNames[i]);
	if(pEngineSrc->CountryParam) pEngineDest->CountryParam=SysAllocString(pEngineSrc->CountryParam);
	if(pEngineSrc->InterceptURL) pEngineDest->InterceptURL=SysAllocString(pEngineSrc->InterceptURL);
	if(pEngineSrc->LinkURL) pEngineDest->LinkURL=SysAllocString(pEngineSrc->LinkURL);
	if(pEngineSrc->Name) pEngineDest->Name=SysAllocString(pEngineSrc->Name);
	if(pEngineSrc->PostQuery) pEngineDest->PostQuery=SysAllocString(pEngineSrc->PostQuery);
	if(pEngineSrc->QueryURL) pEngineDest->QueryURL=SysAllocString(pEngineSrc->QueryURL);
	if(pEngineSrc->IconFile) pEngineDest->IconFile=SysAllocString(pEngineSrc->IconFile);
	if(pEngineSrc->hIcon) pEngineDest->hIcon=CopyIcon(pEngineSrc->hIcon);
}

void CProfiles::CopyProfile(Profile *pProfileDest,Profile *pProfileSrc)
{
	UINT i;

	pProfileDest->id=pProfileSrc->id;
	pProfileDest->Flags=pProfileSrc->Flags;
	pProfileDest->EngineCount=pProfileSrc->EngineCount;
	pProfileDest->DefaultEngineId=pProfileSrc->DefaultEngineId;
	if(pProfileSrc->Name) pProfileDest->Name=SysAllocString(pProfileSrc->Name);
	else pProfileDest->Name=NULL;
	CopyEngine(&pProfileDest->LuckySearch,&pProfileSrc->LuckySearch);
	for(i=0;i<pProfileSrc->EngineCount;i++) CopyEngine(&pProfileDest->Engines[i],&pProfileSrc->Engines[i]);
}

BYTE CProfiles::NewCustomProfileId(BYTE *pList,UINT ListSize)
{
	BYTE NewId;
	UINT i;

	NewId=PROFILE_CUSTOM_ID;
	i=0;
	while(i<ListSize)
	{
		if(pList[i]>=NewId)
		{
			NewId=pList[i];
			if(NewId<0xFF) NewId++;
		}
		i++;
	}

	return NewId;
}

BYTE CProfiles::NewCustomEngineId(Profile *pProfile)
{
	BYTE NewId;
	UINT i;

	NewId=SEARCHENGINE_CUSTOM_ID;

	i=0;
	while(i<pProfile->EngineCount)
	{
		if(pProfile->Engines[i].id>=NewId)
		{
			NewId=pProfile->Engines[i].id;
			if(NewId<0xFF) NewId++;
		}
		i++;
	}

	return NewId;
}

int CProfiles::First()
{
	LoadProfileList();
	EnumProfileIndex=0;
	return ProfileCount?ProfileList[0]:-1;
}

int CProfiles::Next()
{
	EnumProfileIndex++;
	return (EnumProfileIndex<ProfileCount)?ProfileList[EnumProfileIndex]:-1;
}

int CProfiles::GetNextProfileId(int ProfileId)
{
	int i;

	i=ProfileIdToIndex(ProfileId);
	if(i!=-1)
	{
		i=(i+1)%ProfileCount;
		i=ProfileList[i];
	}
	else if(ProfileCount) i=ProfileList[0];

	return i;
}

int CProfiles::GetPreviousProfileId(int ProfileId)
{
	int i;

	i=ProfileIdToIndex(ProfileId);
	if(i!=-1)
	{
		i--;
		if(i<0) i=ProfileCount-1;
		i=ProfileList[i];
	}
	else if(ProfileCount) i=ProfileList[0];

	return i;
}

int CProfiles::ProfileIdToIndex(int ProfileId)
{
	UINT i;

	i=0;
	while(i<ProfileCount && ProfileList[i]!=ProfileId) i++;

	return (i<ProfileCount)?i:-1;
}

int CProfiles::IndexToProfileId(int ProfileIndex)
{
	return (ProfileIndex>=0 && (UINT)ProfileIndex<ProfileCount)?ProfileList[ProfileIndex]:-1;
}

int CProfiles::EngineIdToIndex(int EngineId)
{
	int i=0;

	while((UINT)i<CurrentProfile.EngineCount && CurrentProfile.Engines[i].id!=EngineId) i++;
	if((UINT)i>=CurrentProfile.EngineCount)
	{
		if(CurrentProfile.EngineCount) i=0;
		else i=-1;
	}
	
	return i;
}

int CProfiles::IndexToEngineId(int EngineIndex)
{
	return (EngineIndex>=0 && (UINT)EngineIndex<CurrentProfile.EngineCount)?CurrentProfile.Engines[EngineIndex].id:-1;
}

bool CProfiles::LoadCurrentProfile(int ProfileId)
{
	bool result;
	HKEY hKey;

	hKey=OpenProfiles(true,false);
	if(hKey)
	{
		result=LoadProfile(hKey,ProfileId,&CurrentProfile);
		RegCloseKey(hKey);
	}
	else result=false;

	if(result==false) FreeProfile(&CurrentProfile);

	return result;
}

bool CProfiles::GetEngine(int ProfileId,int EngineId,SearchEngine *pEngine)
{
	bool result;
	HKEY hKey;
	HKEY hKeyProfile;

	result=false;
	hKey=OpenProfiles(true,false);
	if(hKey)
	{
		hKeyProfile=OpenSubKey(hKey,ProfileId,false);
		if(hKeyProfile)
		{
			result=LoadEngine(hKeyProfile,EngineId,pEngine);

			RegCloseKey(hKeyProfile);
		}

		RegCloseKey(hKey);
	}

	return result;
}

COLORREF CProfiles::GetEngineColor(SearchEngine *pEngine)
{
	return (pEngine->Flags&SEARCHENGINE_PARTIALSUPPORT)?PARTIALSUPPORTCOLOR:LinkColor;
}

HICON CProfiles::GetEngineIcon(SearchEngine *pEngine,HICON hDefaultIcon)
{
	HICON hIcon;

	hIcon=NULL;

	if(pEngine)
	{
		if(pEngine->hIcon)
		{
			hIcon=pEngine->hIcon;
		}
		else if(pEngine->IconFile)
		{
			pEngine->hIcon=(HICON)LoadImage(_Module.m_hInst,pEngine->IconFile,IMAGE_ICON,g_Scaled_IconSize,g_Scaled_IconSize,LR_DEFAULTCOLOR|LR_LOADFROMFILE);
			hIcon=pEngine->hIcon;
		}
		else if(g_Options&OPTION_DownloadFavIcon) // Try loading favicon.ico from cache
		{
			if(pEngine->IconCacheRetryTimer==0)
			{
				pEngine->hIcon=CFavIcon::LoadFavIconFromCache(pEngine->LinkURL,true);
				hIcon=pEngine->hIcon;
			}
			else pEngine->IconCacheRetryTimer--;
		}
	} // End pEngine!=NULL

	if(hIcon==NULL) hIcon=hDefaultIcon;

	return hIcon;
}

TCHAR* CProfiles::GetProfileName(int ProfileId)
{
	TCHAR *pProfileName;
	UINT i;

	pProfileName=NULL;
	if(ProfileId!=-1)
	{
		if(bNamesLoaded==false) LoadNames();

		if(bNamesLoaded)
		{
			i=0;
			while(i<ProfileCount)
			{
				if(ProfileList[i]==ProfileId)
				{
					pProfileName=ProfileNames[i];
					break;
				}
				i++;
			}
		}
	}

	return pProfileName;
}

bool CProfiles::GetLuckySearchEngine(int ProfileId,SearchEngine *pEngine)
{
	bool result;
	HKEY hKey;
	HKEY hKeyProfile;

	result=false;
	hKey=OpenProfiles(true,false);
	if(hKey)
	{
		hKeyProfile=OpenSubKey(hKey,ProfileId,false);
		if(hKeyProfile)
		{
			LoadLuckySearchEngine(hKeyProfile,pEngine);
			result=true;
			RegCloseKey(hKeyProfile);
		}
		RegCloseKey(hKey);
	}

	return result;
}

void CProfiles::LoadLuckySearchEngine(HKEY hKeyProfile,SearchEngine *pEngine)
{
	InitEngine(pEngine);
	pEngine->QueryURL=LoadString(hKeyProfile,L"Lucky",MAXURLLENGTH);
	if(pEngine->QueryURL==NULL) pEngine->QueryURL=SysAllocString(PROFILE_DEFAULT_LUCKYURL);
	InitParams(pEngine,pEngine->QueryURL);
	if(pEngine->ParamCount==0) pEngine->Flags=(pEngine->Flags&0xFFFF0000) | CODEPAGE_NO_ENCODING;
}

bool CProfiles::IsLuckyURL(TCHAR *pURL,TCHAR pInterceptedSearchTerms[MAXURLLENGTH])
{
	bool result;
	TCHAR *pLuckyURL;
	TCHAR *pOldURL;
	bool bParamPresent;

	result=false;
	if(CurrentProfile.LuckySearch.QueryURL)
	{
		pLuckyURL=CurrentProfile.LuckySearch.QueryURL;
		pOldURL=pURL;
		bParamPresent=false;
		while(*pURL && *pLuckyURL)
		{
			if(*pLuckyURL==L'{')
			{
				while(*pLuckyURL && *pLuckyURL!=L'}') pLuckyURL++;
				if(*pLuckyURL==L'}') pLuckyURL++;
				while(*pURL && *pURL!=L'&') pURL++;
				bParamPresent=true;
			}
			else if(*pLuckyURL==*pURL)
			{
				pLuckyURL++;
				pURL++;
			}
			else break;
		}
		if(*pURL==L'\0' && *pLuckyURL==L'\0' && bParamPresent)
		{
			CIDNA idna;
			TCHAR url[MAXURLLENGTH];
			size_t url_len;
			int HostEndIndex;

			// Check if LuckyURL is valid and not of the form "http://www.{searchTerms}.com/"
			StringCbCopy(url,sizeof url,CurrentProfile.LuckySearch.QueryURL);
			url_len=MAXURLLENGTH;
			if((idna.URLToUnicode(url,&url_len,NULL,&HostEndIndex,NULL)&IDNA_ILLEGAL)==0)
			{
				result=InterceptSearchTerms(pOldURL,true,&CurrentProfile.LuckySearch,pInterceptedSearchTerms);
			}
		}
	}

	return result;
}

HKEY CProfiles::OpenProfiles(bool bCurrentUser,bool bCreateKey)
{
	return CQToolbar::OpenQueroKey(bCurrentUser?HKEY_CURRENT_USER:HKEY_USERS,L"Profiles",bCreateKey);
}

HKEY CProfiles::OpenSubKey(HKEY hKey,int id,bool bCreateKey)
{
	HKEY hSubKey;
	TCHAR KeyName[MAX_SUBKEY_LEN];
	LONG result;

	if(id>=0)
	{
		StringCchPrintf(KeyName,MAX_SUBKEY_LEN,L"%d",id);

		if(bCreateKey)
		{
			result=RegCreateKeyEx(hKey,KeyName, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL, &hSubKey, NULL);
		}
		else
		{
			result=RegOpenKeyEx(hKey,KeyName, 0, KEY_READ, &hSubKey);
		}

		if(result!=ERROR_SUCCESS) hSubKey=NULL;
	}
	else hSubKey=NULL;

	return hSubKey;
}

bool CProfiles::DeleteSubKey(HKEY hKey,int id)
{
	TCHAR KeyName[MAX_SUBKEY_LEN];

	StringCchPrintf(KeyName,MAX_SUBKEY_LEN,L"%d",id);
	return SHDeleteKey(hKey,KeyName)==ERROR_SUCCESS;
}

UINT CProfiles::LoadList(HKEY hKey,BYTE *pList,UINT ListSize)
{
	DWORD n;

	n=ListSize;

	if(RegQueryValueEx(hKey,L"List",NULL,NULL,pList,&n)!=ERROR_SUCCESS) n=0;

	return n;
}

bool CProfiles::SaveList(HKEY hKey,BYTE *pList,UINT ListSize)
{
	return RegSetValueEx(hKey,L"List",NULL,REG_BINARY,pList,ListSize)==ERROR_SUCCESS;
}

bool CProfiles::LoadEngine(HKEY hKeyProfile,int EngineId,SearchEngine *pEngine)
{
	HKEY hKeyEngine;
	bool result=false;

	// Clear SE attributes
	InitEngine(pEngine);

	hKeyEngine=OpenSubKey(hKeyProfile,EngineId,false);
	if(hKeyEngine)
	{
		LONG RetCodeEnum;
		DWORD EnumIndex;
		TCHAR ValueName[MAX_SUBKEY_LEN];
		DWORD cValueName;
		DWORD type;
		TCHAR data[MAXURLLENGTH];
		DWORD size;
		CIDNA idna;
		size_t url_len;
		int HostEndIndex;
		int i;

		EnumIndex=0;

		// Enumerate Values
		do
		{
			cValueName=MAX_SUBKEY_LEN;
			size=sizeof data;

			#ifdef COMPILE_FOR_WIN9X
				char ValueNameA[MAX_SUBKEY_LEN];
				RetCodeEnum=RegEnumValueA(hKeyEngine,EnumIndex,ValueNameA,&cValueName,NULL,&type,(LPBYTE)data,&size);
				if(RetCodeEnum==ERROR_SUCCESS)
				{
					if(MultiByteToWideChar(CP_ACP,0,ValueNameA,-1,ValueName,MAX_SUBKEY_LEN)==0)
					{
						ValueName[0]=0;
					}
					if(type==REG_SZ)
					{
						TCHAR data2[MAXURLLENGTH];
						if(MultiByteToWideChar(CP_ACP,0,(char*)data,-1,data2,MAXURLLENGTH)==0) data2[0]=0;
						StringCbCopy(data,sizeof data,data2);
					}
				}
			#else
				RetCodeEnum=RegEnumValue(hKeyEngine,EnumIndex,ValueName,&cValueName,NULL,&type,(LPBYTE)data,&size);
			#endif

			if(RetCodeEnum==ERROR_SUCCESS)
			{
				i=0;
				while(i<SEARCHENGINE_VALUES_COUNT && StrCmpI(ValueName,SEARCHENGINE_VALUES[i])) i++;
				
				switch(i)
				{
				case SEARCHENGINE_VALUES_NAME:
					if(type==REG_SZ)
					{
						if(size>(MAX_SEARCHENGINE_NAME_LEN*sizeof TCHAR)) data[MAX_SEARCHENGINE_NAME_LEN]=0;
						pEngine->Name=SysAllocString(data);
					}
					break;
				case SEARCHENGINE_VALUES_QUERY:
					if(type==REG_SZ)
					{
						pEngine->QueryURL=SysAllocString(data);
						// Extract InterceptURL
						if(idna.IsInternetURL(data))
						{
							url_len=MAXURLLENGTH;
							if((idna.URLToUnicode(data,&url_len,NULL,&HostEndIndex,NULL)&IDNA_ILLEGAL)==0)
							{
								if(HostEndIndex<MAXURLLENGTH-1)
								{
									data[HostEndIndex]='/';
									data[HostEndIndex+1]=0;
									pEngine->InterceptURL=SysAllocString(data);
								}
							}
						}
					}
					break;
				case SEARCHENGINE_VALUES_LINK:
					if(type==REG_SZ) pEngine->LinkURL=SysAllocString(data);
					break;
				case SEARCHENGINE_VALUES_POST:
					if(type==REG_SZ) pEngine->PostQuery=SysAllocString(data);
					break;
				case SEARCHENGINE_VALUES_COUNTRYPARAM:
					if(type==REG_SZ) pEngine->CountryParam=SysAllocString(data);
					break;
				case SEARCHENGINE_VALUES_FLAGS:
					if(type==REG_DWORD) pEngine->Flags=*(DWORD*)data;
					break;
				case SEARCHENGINE_VALUES_ICON:
					if(type==REG_SZ) pEngine->IconFile=SysAllocString(data);
					break;
				}

				EnumIndex++;
			}
		} while(RetCodeEnum==ERROR_SUCCESS);

		pEngine->id=EngineId;

		if(pEngine->Name && pEngine->QueryURL)
		{
			TCHAR *pURL;

			// Init LinkURL
			
			if(pEngine->LinkURL==NULL && pEngine->InterceptURL) pEngine->LinkURL=SysAllocString(pEngine->InterceptURL);
			
			// Extract parameter names and count

			if(pEngine->PostQuery)
			{
				// Concatenate QueryURL and PostQuery
				StringCbCopy(data,sizeof data,pEngine->QueryURL);
				StringCbCat(data,sizeof data,L"&");
				StringCbCat(data,sizeof data,pEngine->PostQuery);
				pURL=data;
			}
			else pURL=pEngine->QueryURL;

			InitParams(pEngine,pURL);
		
			result=true;
		}
		else FreeEngine(pEngine);

		RegCloseKey(hKeyEngine);
	}

	return result;
}

void CProfiles::InitParams(SearchEngine *pEngine,TCHAR *pTemplate)
{
	TCHAR QueryParamName[MAXURLLENGTH];
	TCHAR TemplateParamName[MAXURLLENGTH];
	bool InsideTemplateParamName;
	bool InsideQueryParamName;
	TCHAR ch;
	int i,j;
	UINT k;
	UINT p;
	BSTR ParamNameSearchTerms;
	BSTR ParamNameAddress;

	InsideTemplateParamName=false;
	InsideQueryParamName=false;
	ParamNameSearchTerms=NULL;
	ParamNameAddress=NULL;
	QueryParamName[0]=0;
	ch=*pTemplate;
	while(ch)
	{
		if(InsideTemplateParamName)
		{
			if(ch==L'}')
			{
				if(j && TemplateParamName[j-1]==L'?') j--;
				TemplateParamName[j]=0;

				p=MAX_QUERY_PARAMETERS;
				if(!StrCmp(TemplateParamName,OPENSEARCH_PARAMS[OPENSEARCH_PARAMS_SEARCHTERMS]))
				{
					if(ParamNameSearchTerms==NULL) ParamNameSearchTerms=SysAllocString(QueryParamName);
					pEngine->bRequiresKeywords=true;
				}
				else if(!StrCmpN(TemplateParamName,L"Quero:",6))
				{
					if(j==8 && TemplateParamName[6]==L'p' && TemplateParamName[7]>=L'1' && TemplateParamName[7]<=L'9')
					{
						p=TemplateParamName[7]-L'1';
						pEngine->bRequiresKeywords=true;
					}
					else
					{
						k=QUERO_PARAMS_URL;
						while(k<QUERO_PARAMS_COUNT && StrCmp(TemplateParamName+6,QUERO_PARAMS[k])) k++;
						if(k<=QUERO_PARAMS_DOMAINNAME)
						{
							if(pEngine->iRequiresAddress < k)
							{
								pEngine->iRequiresAddress=k;
								if(ParamNameAddress) SysFreeString(ParamNameAddress);
								ParamNameAddress=SysAllocString(QueryParamName);
							}
						}
					}
				}

				if(p<MAX_QUERY_PARAMETERS)
				{
					if(QueryParamName[0] && pEngine->ParamNames[p]==NULL)
					{
						pEngine->ParamNames[p]=SysAllocString(QueryParamName);
					}
					if(pEngine->ParamCount<=p) pEngine->ParamCount=p+1; // ParamCount = Max (ParamCount,p+1)
				}

				InsideTemplateParamName=false;
			}
			else if(j<(MAXURLLENGTH-1)) TemplateParamName[j++]=ch;
		}
		else
		{
			switch(ch)
			{
			case L'{':
				if(InsideQueryParamName)
				{
					QueryParamName[0]=0;
					InsideQueryParamName=false;
				}
				j=0;
				InsideTemplateParamName=true;
				break;
			case L'&':
			case L'?':
				i=0;
				InsideQueryParamName=true;
				break;
			case L'=':
				if(InsideQueryParamName)
				{
					QueryParamName[i]=0;
					InsideQueryParamName=false;
				}
				break;
			default:
				if(InsideQueryParamName && i<(MAXURLLENGTH-1)) QueryParamName[i++]=ch;
				break;
			}
		}

		pTemplate++;
		ch=*pTemplate;
	}

	if(ParamNameSearchTerms)
	{
		if(pEngine->ParamCount) SysFreeString(ParamNameSearchTerms);
		else
		{
			pEngine->ParamNames[0]=ParamNameSearchTerms;
			pEngine->ParamCount=1;
		}
	}

	if(ParamNameAddress)
	{
		if(pEngine->ParamCount<MAX_QUERY_PARAMETERS)
		{
			pEngine->ParamNames[pEngine->ParamCount]=ParamNameAddress;
			pEngine->ParamCount++;
		}
		else SysFreeString(ParamNameAddress);
	}
}

bool CProfiles::SaveEngine(HKEY hKeyProfile,SearchEngine *pEngine,bool bOverwriteCustom)
{
	HKEY hKeyEngine;
	bool result=true;

	hKeyEngine=OpenSubKey(hKeyProfile,pEngine->id,true);
	if(hKeyEngine)
	{
		DWORD type;
		LPBYTE pData;
		DWORD size;
		int i;
		bool bSaveValue;

		// Check whether to update custom search engine
		if(bOverwriteCustom || (LoadFlags(hKeyEngine,true)&SEARCHENGINE_CUSTOM)==0)
		{
			for(i=0;i<SEARCHENGINE_VALUES_COUNT;i++)
			{
				bSaveValue=false;
				switch(i)
				{
				case SEARCHENGINE_VALUES_NAME:
					if(pEngine->Name)
					{
						type=REG_SZ;
						pData=(LPBYTE)pEngine->Name;
						size=SysStringByteLen(pEngine->Name)+sizeof TCHAR;
						bSaveValue=true;
					}
					break;
				case SEARCHENGINE_VALUES_QUERY:
					if(pEngine->QueryURL)
					{
						type=REG_SZ;
						pData=(LPBYTE)pEngine->QueryURL;
						size=SysStringByteLen(pEngine->QueryURL)+sizeof TCHAR;
						bSaveValue=true;
					}
					break;
				case SEARCHENGINE_VALUES_LINK:
					// Save Link URL ony if it is different from the InterceptURL
					if(pEngine->LinkURL && StrCmp(pEngine->LinkURL,pEngine->InterceptURL))
					{
						type=REG_SZ;
						pData=(LPBYTE)pEngine->LinkURL;
						size=SysStringByteLen(pEngine->LinkURL)+sizeof TCHAR;
						bSaveValue=true;
					}
					break;
				case SEARCHENGINE_VALUES_POST:
					if(pEngine->PostQuery)
					{
						type=REG_SZ;
						pData=(LPBYTE)pEngine->PostQuery;
						size=SysStringByteLen(pEngine->PostQuery)+sizeof TCHAR;
						bSaveValue=true;
					}
					break;
				case SEARCHENGINE_VALUES_COUNTRYPARAM:
					if(pEngine->CountryParam)
					{
						type=REG_SZ;
						pData=(LPBYTE)pEngine->CountryParam;
						size=SysStringByteLen(pEngine->CountryParam)+sizeof TCHAR;
						bSaveValue=true;
					}
					break;
				case SEARCHENGINE_VALUES_FLAGS:
					if(pEngine->Flags!=SEARCHENGINE_DEFAULT_FLAGS)
					{
						type=REG_DWORD;
						pData=(LPBYTE)&pEngine->Flags;
						size=sizeof DWORD;
						bSaveValue=true;
					}
					break;
				case SEARCHENGINE_VALUES_ICON:
					if(pEngine->IconFile)
					{
						type=REG_SZ;
						pData=(LPBYTE)pEngine->IconFile;
						size=SysStringByteLen(pEngine->IconFile)+sizeof TCHAR;
						bSaveValue=true;
					}
					break;
				}

				if(bSaveValue)
				{
					if(RegSetValueEx(hKeyEngine,SEARCHENGINE_VALUES[i],NULL,type,pData,size)!=ERROR_SUCCESS)
						result=false;
				}
				else RegDeleteValue(hKeyEngine,SEARCHENGINE_VALUES[i]);
			}
		}

		 // Remove legacy value
		RegDeleteValue(hKeyEngine,L"IconId");

		RegCloseKey(hKeyEngine);
	}
	else result=false;

	return result;
}

void CProfiles::FreeProfile(Profile *pProfile)
{
	UINT i;

	if(pProfile->Name)
	{
		SysFreeString(pProfile->Name);
		pProfile->Name=NULL;
	}
	FreeEngine(&pProfile->LuckySearch);
	for(i=0;i<pProfile->EngineCount;i++) FreeEngine(&pProfile->Engines[i]);
	pProfile->EngineCount=0;
}

void CProfiles::FreeEngine(SearchEngine *pEngine)
{
	UINT i;

	if(pEngine->id!=SEARCHENGINE_ID_FREE)
	{
		BSTR Strings[7]={pEngine->Name,pEngine->LinkURL,pEngine->QueryURL,pEngine->InterceptURL,pEngine->PostQuery,pEngine->CountryParam,pEngine->IconFile};
		
		for(i=0;i<7;i++) if(Strings[i]) SysFreeString(Strings[i]);

		for(i=0;i<pEngine->ParamCount;i++) if(pEngine->ParamNames[i]) SysFreeString(pEngine->ParamNames[i]);

		if(pEngine->hIcon) DestroyIcon(pEngine->hIcon);

		InitEngine(pEngine);
	}
}

BYTE CProfiles::LoadDefaultId(HKEY hKey)
{
	BYTE DefaultId;
	DWORD type;
	DWORD size;
	DWORD data;

	size=sizeof data;
	if(RegQueryValueEx(hKey,L"DefaultId",NULL,&type,(LPBYTE)&data,&size)==ERROR_SUCCESS && type==REG_DWORD)
		DefaultId=(BYTE)data;
	else
		DefaultId=PROFILE_ID_ERROR;

	return DefaultId;
}

bool CProfiles::SaveDefaultId(HKEY hKey,BYTE DefaultId)
{
	bool result;
	DWORD data;

	if(DefaultId!=PROFILE_ID_ERROR)
	{
		data=DefaultId;
		result=(RegSetValueEx(hKey,L"DefaultId",NULL,REG_DWORD,(LPBYTE)&data,sizeof data)==ERROR_SUCCESS);
	}
	else result=(RegDeleteValue(hKey,L"DefaultId")==ERROR_SUCCESS);

	return result;
}

BYTE CProfiles::LoadDefaultProfileId(HKEY hKeyProfiles)
{
	DefaultProfileId=LoadDefaultId(hKeyProfiles);
	if(DefaultProfileId==PROFILE_ID_ERROR) DefaultProfileId=IndexToProfileId(0);

	return DefaultProfileId;
}

bool CProfiles::SaveDefaultProfileId(BYTE DefaultProfileId,bool bCurrentUser)
{
	bool result=false;
	HKEY hKey;

	this->DefaultProfileId=DefaultProfileId;

	hKey=OpenProfiles(bCurrentUser,true);
	if(hKey)
	{
		if(ProfileCount && ProfileList[0]==DefaultProfileId) DefaultProfileId=PROFILE_ID_ERROR;
		result=SaveDefaultId(hKey,DefaultProfileId);
	}

	return result;
}

bool CProfiles::SaveDefaultEngineId(Profile *pProfile,bool bCurrentUser)
{
	bool result=false;
	HKEY hKey;
	HKEY hKeyProfile;

	hKey=OpenProfiles(bCurrentUser,true);
	if(hKey)
	{
		hKeyProfile=OpenSubKey(hKey,pProfile->id,true);
		if(hKeyProfile)
		{
			if(LoadDefaultId(hKeyProfile)!=pProfile->DefaultEngineId)
			{
				result=SaveDefaultEngineId(hKeyProfile,pProfile);
				pProfile->Flags|=PROFILE_DEFAULTENGINE_MODIFIED;
				SaveFlags(hKeyProfile,pProfile->Flags,PROFILE_DEFAULT_FLAGS);
			}
		}
	}

	return result;
}

bool CProfiles::SaveDefaultEngineId(HKEY hKeyProfile,Profile *pProfile)
{
	BYTE DefaultId;
	
	if(pProfile->EngineCount && pProfile->DefaultEngineId!=pProfile->Engines[0].id)
		DefaultId=pProfile->DefaultEngineId;
	else
		DefaultId=SEARCHENGINE_ID_ERROR;

	return SaveDefaultId(hKeyProfile,DefaultId);
}

bool CProfiles::LoadProfile(HKEY hKey,int ProfileId,Profile *pProfile)
{
	HKEY hKeyProfile;
	BYTE EngineList[MAX_ENGINES];
	int ListSize;
	int i;
	int n;
	bool result;

	// Free profile
	FreeProfile(pProfile);

	// Load profile
	hKeyProfile=OpenSubKey(hKey,ProfileId,false);
	if(hKeyProfile)
	{
		pProfile->Name=LoadString(hKeyProfile,L"Name",MAX_PROFILE_NAME_LEN);
		if(pProfile->Name)
		{
			pProfile->id=ProfileId;	
			LoadLuckySearchEngine(hKeyProfile,&pProfile->LuckySearch);
			pProfile->Flags=LoadFlags(hKeyProfile,false);

			ListSize=LoadList(hKeyProfile,EngineList,MAX_ENGINES);

			// Determine default engine
			pProfile->DefaultEngineId=LoadDefaultId(hKeyProfile);
			if(pProfile->DefaultEngineId==SEARCHENGINE_ID_ERROR)
			{
				if(ListSize) pProfile->DefaultEngineId=EngineList[0];
				else pProfile->DefaultEngineId=0;
			}

			// Load engines
			n=0;
			i=0;
			while(i<ListSize)
			{
				if(EngineList[i]!=SEARCHENGINE_ID_SEPARATOR)
				{
					if(LoadEngine(hKeyProfile,EngineList[i],&pProfile->Engines[n])) n++;
				}
				else if(n) pProfile->Engines[n-1].HasSeparator=true;
				
				i++;
			}

			pProfile->EngineCount=n;

			result=true;
		}
		else result=false;
		
		RegCloseKey(hKeyProfile);
	}
	else result=false;

	return result;
}

bool CProfiles::SaveProfile(HKEY hKey,Profile *pProfile,bool bMergeProfile)
{
	bool result=false;
	HKEY hKeyProfile;
	BYTE NewEngineList[MAX_ENGINES];
	BYTE OldEngineList[MAX_ENGINES];
	UINT NewListSize;
	UINT OldListSize;
	DWORD flags;
	UINT i;

	hKeyProfile=OpenSubKey(hKey,pProfile->id,true);
	if(hKeyProfile)
	{
		// Load current profile flags
		flags=LoadFlags(hKeyProfile,false);

		// Do not update custom profiles
		if(!(bMergeProfile && (flags&PROFILE_CUSTOM)))
		{
			// Load current profile list
			OldListSize=LoadList(hKeyProfile,OldEngineList,MAX_ENGINES);

			// Generate engine list
			i=0;
			NewListSize=0;
			while(i<pProfile->EngineCount && NewListSize<MAX_ENGINES)
			{
				NewEngineList[NewListSize]=pProfile->Engines[i].id;
				NewListSize++;
				if(pProfile->Engines[i].HasSeparator && NewListSize<MAX_ENGINES)
				{
					NewEngineList[NewListSize]=SEARCHENGINE_ID_SEPARATOR;
					NewListSize++;
				}
				i++;
			}

			// Merge engine list with old one
			if(bMergeProfile)
			{
				MergeLists(OldEngineList,OldListSize,NewEngineList,&NewListSize,flags,hKeyProfile,true);
			}
			else flags=0;

			// Delete old entries
			i=0;
			while(i<OldListSize)
			{
				if(OldEngineList[i]!=SEARCHENGINE_ID_SEPARATOR && !IsInList(OldEngineList[i],NewEngineList,NewListSize))
				{
					DeleteSubKey(hKeyProfile,OldEngineList[i]);
				}
				i++;
			}

			// Add/Update new entries
			i=0;
			while(i<pProfile->EngineCount)
			{
				if(IsInList(pProfile->Engines[i].id,NewEngineList,NewListSize))
				{
					SaveEngine(hKeyProfile,&pProfile->Engines[i],bMergeProfile==false);
				}
				i++;
			}

			// Save name
			if((flags&PROFILE_NAME_MODIFIED)==0) SaveString(hKeyProfile,L"Name",pProfile->Name);

			// Save list
			SaveList(hKeyProfile,NewEngineList,NewListSize);

			// Save LuckyURL
			if((flags&PROFILE_LUCKYURL_MODIFIED)==0)
			{
				if(pProfile->LuckySearch.QueryURL && StrCmp(pProfile->LuckySearch.QueryURL,PROFILE_DEFAULT_LUCKYURL))
					SaveString(hKeyProfile,L"Lucky",pProfile->LuckySearch.QueryURL);
				else RegDeleteValue(hKeyProfile,L"Lucky");
			}

			// Save default engine id
			if((flags&PROFILE_DEFAULTENGINE_MODIFIED)==0)
			{
				SaveDefaultEngineId(hKeyProfile,pProfile);
			}

			// Save flags
			SaveFlags(hKeyProfile,pProfile->Flags|flags,PROFILE_DEFAULT_FLAGS);
		}

		RegCloseKey(hKeyProfile);
	}

	return result;
}

bool CProfiles::UpdateUserProfiles()
{
	bool result=false;
	HKEY hKeyDefaultUser;
	HKEY hKeyCurrentUser;
	BYTE NewProfileList[MAX_PROFILES];
	BYTE OldProfileList[MAX_PROFILES];
	UINT NewListSize;
	UINT OldListSize;
	DWORD flags;
	UINT i;

	QDEBUG_PRINT(L"UpdateUserProfiles",L"start");

	hKeyDefaultUser=OpenProfiles(false,false);
	if(hKeyDefaultUser)
	{
		hKeyCurrentUser=OpenProfiles(true,true);
		if(hKeyCurrentUser)
		{
			// Do not update custom profiles
			flags=LoadFlags(hKeyCurrentUser,false);
			if((flags&PROFILES_CUSTOM)==0)
			{
				// Load profile lists
				OldListSize=LoadList(hKeyCurrentUser,OldProfileList,MAX_PROFILES);
				NewListSize=LoadList(hKeyDefaultUser,NewProfileList,MAX_PROFILES);

				// Merge profile list with old one
				MergeLists(OldProfileList,OldListSize,NewProfileList,&NewListSize,flags,hKeyCurrentUser,false);

				// Delete old entries
				i=0;
				while(i<OldListSize)
				{
					if(!IsInList(OldProfileList[i],NewProfileList,NewListSize))
					{
						DeleteSubKey(hKeyCurrentUser,OldProfileList[i]);
					}
					i++;
				}

				// Add/Update new entries
				i=0;
				while(i<NewListSize)
				{
					Profile profile;

					InitProfile(&profile);
					if(LoadProfile(hKeyDefaultUser,NewProfileList[i],&profile))
					{
						SaveProfile(hKeyCurrentUser,&profile,true);
						FreeProfile(&profile);
					}

					i++;
				}

				// Save list
				SaveList(hKeyCurrentUser,NewProfileList,NewListSize);

				// Save flags
				flags|=LoadFlags(hKeyDefaultUser,false);
				SaveFlags(hKeyCurrentUser,flags,PROFILES_DEFAULT_FLAGS);
			}
			RegCloseKey(hKeyCurrentUser);
		}
		RegCloseKey(hKeyDefaultUser);
	}

	QDEBUG_PRINT(L"UpdateUserProfiles",L"end");

	return result;
}

void CProfiles::MergeLists(BYTE *pOldList,UINT OldListSize,BYTE *pNewList,UINT *pNewListSize,int Flags,HKEY hKey,bool bSearchEngineList)
{
	BYTE MergedList[MAX_ENGINES>MAX_PROFILES?MAX_ENGINES:MAX_PROFILES];
	UINT MergedListSize;
	UINT MaxListSize;
	BYTE SyncId;
	HKEY hSubKey;
	bool bCopy;
	UINT i;

	if(Flags&PROFILES_LIST_MODIFIED) // PROFILES_LIST_MODIFIED==PROFILE_LIST_MODIFIED
	{
		MergedListSize=0;
		MaxListSize=bSearchEngineList?MAX_ENGINES:MAX_PROFILES;

		SyncId=LoadSyncId(hKey);
	
		// Copy existing entries, delete unmodified entries that have been removed
		i=0;
		while(i<OldListSize)
		{
			if(bSearchEngineList && pOldList[i]==SEARCHENGINE_ID_SEPARATOR)
			{
				bCopy=(MergedListSize==0 || MergedList[MergedListSize-1]!=SEARCHENGINE_ID_SEPARATOR);
			}
			else if(!IsInList(pOldList[i],pNewList,*pNewListSize))
			{
				hSubKey=OpenSubKey(hKey,pOldList[i],false);
				if(hSubKey)
				{
					bCopy=(LoadFlags(hSubKey,bSearchEngineList)&PROFILE_CUSTOM)!=0; // PROFILE_CUSTOM==SEARCHENGINE_CUSTOM
					RegCloseKey(hSubKey);
				}
				else bCopy=false;
			}
			else bCopy=true;

			if(bCopy) MergedList[MergedListSize++]=pOldList[i];
			i++;
		}

		// Add new entries to the end of the list
		i=0;
		while(i<*pNewListSize && MergedListSize<MaxListSize)
		{
			if(pNewList[i]>=SyncId && !IsInList(pNewList[i],MergedList,MergedListSize))
			{
				if(bSearchEngineList==false || pNewList[i]!=SEARCHENGINE_ID_SEPARATOR)
					MergedList[MergedListSize++]=pNewList[i];
			}
			i++;
		}

		// Update NewEngineList
		i=0;
		while(i<MergedListSize)
		{
			pNewList[i]=MergedList[i];
			i++;
		}
		*pNewListSize=MergedListSize;
	}
}

BYTE CProfiles::LoadSyncId(HKEY hKey)
{
	DWORD SyncId;
	DWORD size;

	size=sizeof DWORD;
	if(RegQueryValueEx(hKey,L"SyncId",NULL,NULL,(LPBYTE)&SyncId,&size)!=ERROR_SUCCESS) SyncId=0;

	return (BYTE)SyncId;
}

bool CProfiles::SaveSyncId(HKEY hKey,BYTE SyncId)
{
	DWORD dwSyncId;
	bool result;

	if(SyncId)
	{
		dwSyncId=SyncId;
		result=(RegSetValueEx(hKey,L"SyncId",NULL,REG_DWORD,(LPBYTE)&dwSyncId,sizeof dwSyncId)==ERROR_SUCCESS);
	}
	else result=(RegDeleteValue(hKey,L"SyncId")==ERROR_SUCCESS);

	return result;
}

DWORD CProfiles::LoadFlags(HKEY hKey,bool bSearchEngineFlags)
{
	DWORD Flags;
	DWORD size;

	size=sizeof DWORD;
	if(RegQueryValueEx(hKey,L"Flags",NULL,NULL,(LPBYTE)&Flags,&size)!=ERROR_SUCCESS)
		Flags=bSearchEngineFlags?SEARCHENGINE_DEFAULT_FLAGS:PROFILE_DEFAULT_FLAGS;

	return Flags;
}

bool CProfiles::SaveFlags(HKEY hKey,DWORD Flags,DWORD DefaultFlags)
{
	bool result;

	if(Flags!=DefaultFlags)
		result=(RegSetValueEx(hKey,L"Flags",NULL,REG_DWORD,(LPBYTE)&Flags,sizeof Flags)==ERROR_SUCCESS);
	else
		result=(RegDeleteValue(hKey,L"Flags")==ERROR_SUCCESS);

	return result;
}

bool CProfiles::IsInList(BYTE id,BYTE *pList,int ListSize)
{
	int i;

	i=0;
	while(i<ListSize && pList[i]!=id) i++;

	return (i<ListSize);
}

BSTR CProfiles::LoadString(HKEY hKey,TCHAR *pValueName,UINT MaxLen)
{
	BSTR bstrName;
	TCHAR data[MAXURLLENGTH];
	DWORD type;
	DWORD size;

	size=sizeof data;
	if(RegQueryValueEx(hKey,pValueName,NULL,&type,(LPBYTE)&data,&size)==ERROR_SUCCESS && type==REG_SZ)
	{
		if(MaxLen<MAXURLLENGTH) data[MaxLen]=0;
		bstrName=SysAllocString(data);
	}
	else bstrName=NULL;

	return bstrName;
}

bool CProfiles::SaveString(HKEY hKey,TCHAR *pValueName,BSTR bstrName)
{
	LONG result;

	if(bstrName) result=RegSetValueEx(hKey,pValueName,NULL,REG_SZ,(LPBYTE)bstrName,SysStringByteLen(bstrName)+sizeof TCHAR);
	else result=RegDeleteValue(hKey,pValueName);
	
	return result==ERROR_SUCCESS;
}

void CProfiles::LoadProfileList()
{
	HKEY hKeyProfiles;

	hKeyProfiles=OpenProfiles(true,false);
	if(hKeyProfiles)
	{
		ProfileCount=LoadList(hKeyProfiles,ProfileList,MAX_PROFILES);
		LoadDefaultProfileId(hKeyProfiles);

		RegCloseKey(hKeyProfiles);
	}
	bNamesLoaded=false;
}

void CProfiles::LoadNames()
{
	HKEY hKey;
	HKEY hKeyProfile;
	UINT i;

	if(bNamesLoaded) FreeNames();

	hKey=OpenProfiles(true,false);
	if(hKey)
	{
		i=0;
		while(i<ProfileCount)
		{
			hKeyProfile=OpenSubKey(hKey,ProfileList[i],false);
			if(hKeyProfile)
			{
				ProfileNames[i]=LoadString(hKeyProfile,L"Name",MAX_PROFILE_NAME_LEN);
				RegCloseKey(hKeyProfile);
			}
			else ProfileNames[i]=NULL;
			i++;
		}
		bNamesLoaded=true;
		RegCloseKey(hKey);
	}
}

void CProfiles::FreeNames()
{
	UINT i;

	if(bNamesLoaded)
	{
		i=0;
		while(i<ProfileCount)
		{
			if(ProfileNames[i])
			{
				SysFreeString(ProfileNames[i]);
				ProfileNames[i]=NULL;
			}
			i++;
		}
		bNamesLoaded=false;
	}
}

void CProfiles::PrepareNavigation(TCHAR *pQuery,SearchEngine *pEngine,BSTR *pbstrURL,char **ppPostData)
{
	TCHAR data[MAXURLLENGTH];
	TCHAR Params[MAX_QUERY_PARAMETERS][MAXURLLENGTH];
	UINT ParamCount;
	UINT param_count_without_adress;
	UINT i;
	size_t query_length;
	CIDNA idna;
	size_t size;
	int HostStartIndex;
	int HostEndIndex;
	int DomainStartIndex;
	TCHAR* pQueryEncoded;
	const TCHAR* pInputEncoding;
	TCHAR* pParamsEncoded[MAX_QUERY_PARAMETERS];
	TCHAR* pQueroParamsEncoded[QUERO_PARAMS_COUNT];
	TCHAR url[MAXURLLENGTH];

	url[0]=0;

	if(pQuery)
	{
		StrCchLen(pQuery,MAXURLLENGTH,query_length);

		param_count_without_adress=pEngine->ParamCount;

		// Extract address
		if(pEngine->iRequiresAddress)
		{
			if(pEngine->bRequiresKeywords)
			{
				param_count_without_adress--;
				i=(UINT)query_length;
				while(i>0)
				{
					i--;
					if(pQuery[i]==L' ' && pQuery[i+1]==L'@' && pQuery[i+2]==L' ')
					{
						StringCbCopy(url,sizeof url,pQuery+i+3);
						query_length=i;
						break;
					}
				}
			}
			else StringCbCopy(url,sizeof url,pQuery);
		}

		// Split parameters
		ParamCount=SplitParams(pQuery,query_length,Params,param_count_without_adress);

		i=0;
		while(i<N_ENCODINGS && LOWORD(pEngine->Flags)!=Encoding_CodePages[i]) i++;
		if(i<N_ENCODINGS) pInputEncoding=Encoding_CharSetNames[i];
		else pInputEncoding=L"";

		if(LOWORD(pEngine->Flags)==CODEPAGE_UTF8)
		{
			pQueryEncoded=idna.EncodeUTF8(pQuery,query_length);
			for(i=0;i<ParamCount;i++) pParamsEncoded[i]=idna.EncodeUTF8(Params[i]);
		}
		else
		{
			pQueryEncoded=idna.EncodeHex(pQuery,query_length,LOWORD(pEngine->Flags));
			for(i=0;i<ParamCount;i++) pParamsEncoded[i]=idna.EncodeHex(Params[i],LOWORD(pEngine->Flags));
		}
	}
	else // pQuery == NULL
	{
		ParamCount=0;
		pInputEncoding=NULL;
		pQueryEncoded=NULL;
	}

	// Initialize OpenSearch paramters

	const TCHAR* OpenSearchParams[COUNT_OPENSEARCH_PARAMS]={
		pQueryEncoded, // serchTerms
		NULL, // count
		NULL, // startIndex
		NULL, // startPage
		L"*", // language
		pInputEncoding, // inputEncoding
		pInputEncoding // outputEncoding
	};

	// Initialize Quero parameters

	ZeroMemory(pQueroParamsEncoded,sizeof pQueroParamsEncoded);
	if(pEngine->iRequiresAddress)
	{
		size=MAXURLLENGTH;
		if((idna.URLToUnicode(url,&size,&HostStartIndex,&HostEndIndex,&DomainStartIndex)&IDNA_ILLEGAL)==0)
		{
			if(LOWORD(pEngine->Flags)==CODEPAGE_UTF8)
			{
				pQueroParamsEncoded[QUERO_PARAMS_URL]=idna.EncodeUTF8(url);
				url[HostEndIndex]=0;
				pQueroParamsEncoded[QUERO_PARAMS_HOSTNAME]=idna.EncodeUTF8(url+HostStartIndex);
				pQueroParamsEncoded[QUERO_PARAMS_DOMAINNAME]=idna.EncodeUTF8(url+DomainStartIndex);
			}
			else
			{
				pQueroParamsEncoded[QUERO_PARAMS_URL]=idna.EncodeHex(url,LOWORD(pEngine->Flags));
				url[HostEndIndex]=0;
				pQueroParamsEncoded[QUERO_PARAMS_HOSTNAME]=idna.EncodeHex(url+HostStartIndex,LOWORD(pEngine->Flags));
				pQueroParamsEncoded[QUERO_PARAMS_DOMAINNAME]=idna.EncodeHex(url+DomainStartIndex,LOWORD(pEngine->Flags));
			}
		}
	}

	// Build Query

	BuildQuery(OpenSearchParams,pParamsEncoded,ParamCount,pQueroParamsEncoded,pEngine->QueryURL,data);
	if((g_Options&OPTION_SearchCountry) && pEngine->CountryParam)
	{
		StringCchCat(data,MAXURLLENGTH,L"&");
		StringCchCat(data,MAXURLLENGTH,pEngine->CountryParam);
	}
	if(pbstrURL) *pbstrURL=SysAllocString(data);

	if(ppPostData)
	{
		if(pEngine->PostQuery)
		{
			size_t len;

			BuildQuery(OpenSearchParams,pParamsEncoded,ParamCount,pQueroParamsEncoded,pEngine->PostQuery,data);
			StrCchLen(data,MAXURLLENGTH,len);
			if(len)
			{
				len++;
				*ppPostData=new char[len];
				if(*ppPostData) for(i=0;i<len;i++) (*ppPostData)[i]=(char)data[i];
			}
			else *ppPostData=NULL;
		}
		else *ppPostData=NULL;
	}

	// Free encoded strings

	if(pQueryEncoded) delete[] pQueryEncoded;
	for(i=0;i<ParamCount;i++) if(pParamsEncoded[i]) delete[] pParamsEncoded[i];
	for(i=0;i<QUERO_PARAMS_COUNT;i++) if(pQueroParamsEncoded[i]) delete[] pQueroParamsEncoded[i];
}

#define DELIMITER_LONGDASH 1 // " - "

UINT CProfiles::SplitParams(TCHAR *pQuery,size_t QueryLength,TCHAR Params[MAX_QUERY_PARAMETERS][MAXURLLENGTH],UINT ParamCount)
{
	TCHAR *pQueryEnd;
	UINT n;
	UINT i;
	TCHAR delimiter;
	TCHAR ch;
	TCHAR next;

	n=0;
	if(pQuery)
	{
		if(ParamCount>1)
		{
			// Delimiter precedence: " - ", " ", "-"
			if(StrStr(pQuery,L" - ")) delimiter=DELIMITER_LONGDASH;
			else if(StrStr(pQuery,L" ")) delimiter=L' ';
			else delimiter=L'-';
		}
		else delimiter=0;

		i=0;
		pQueryEnd=pQuery+QueryLength;
		ch=*pQuery;
		while(pQuery<pQueryEnd)
		{
			pQuery++;
			next=*pQuery;

			if((delimiter==DELIMITER_LONGDASH && ch==L' ' && next==L'-' && *(pQuery+1)==L' ') || ch==delimiter)
			{
				Params[n][i]=0;
				i=0;
				n++;
				if(n>=MAX_QUERY_PARAMETERS || n>=ParamCount) break;
				if(delimiter==DELIMITER_LONGDASH)
				{
					pQuery+=2;
					next=*pQuery;
				}
			}
			else if( !_istspace(ch) || ( (i || n) && next && !_istspace(next) && next!=delimiter ) )
			{
				Params[n][i]=ch;
				if(i<(MAXURLLENGTH-1)) i++;
			}

			ch=next;
		}
		
		if(i) Params[n++][i]=0;
	}

	return n;
}

void CProfiles::BuildQuery(const TCHAR *pOpenSearchParams[COUNT_OPENSEARCH_PARAMS],TCHAR *pQueryParams[],UINT ParamCount,TCHAR *pQueroParams[QUERO_PARAMS_COUNT],TCHAR *pTemplate,TCHAR *pOutput)
{
	TCHAR TemplateParamName[MAXURLLENGTH];
	bool InsideTemplateParamName;
	TCHAR ch;
	UINT cch;
	UINT i;
	SYSTEMTIME currentTime;
	const TCHAR *pValue;
	TCHAR Value[16];

	GetLocalTime(&currentTime);
	InsideTemplateParamName=false;
	i=0;
	cch=0;
	if(pTemplate && pOutput) ch=*pTemplate;
	else ch=0;
	while(ch && cch<(MAXURLLENGTH-1))
	{
		if(InsideTemplateParamName)
		{
			if(ch==L'}')
			{
				if(i && TemplateParamName[i-1]==L'?') i--;
				TemplateParamName[i]=0;

				if(!StrCmpN(TemplateParamName,L"Quero:",6))
				{
					if(i==8 && TemplateParamName[6]==L'p' && TemplateParamName[7]>=L'1' && TemplateParamName[7]<=L'9')
					{
						i=TemplateParamName[7]-L'1';
						if(i<ParamCount && pQueryParams) pValue=pQueryParams[i];
						else pValue=NULL;
					}
					else
					{
						i=0;
						while(i<QUERO_PARAMS_COUNT && StrCmp(TemplateParamName+6,QUERO_PARAMS[i])) i++;
						if(i<QUERO_PARAMS_SEC)
						{
							WORD currentTimeValues[QUERO_PARAMS_COUNT]={
								currentTime.wYear,
								currentTime.wMonth,
								currentTime.wDay,
								currentTime.wHour,
								currentTime.wMinute,
								currentTime.wSecond
							};

							StringCbPrintf(Value,sizeof Value,L"%02d",currentTimeValues[i]);
							pValue=Value;
						}
						else if(i<QUERO_PARAMS_COUNT)
						{
							pValue=pQueroParams[i];
						}
						else pValue=NULL;
					}
				}
				else
				{
					i=0;
					while(i<COUNT_OPENSEARCH_PARAMS && StrCmp(TemplateParamName,OPENSEARCH_PARAMS[i])) i++;
					if(i<COUNT_OPENSEARCH_PARAMS && pOpenSearchParams) pValue=pOpenSearchParams[i];
					else pValue=NULL;
				}

				if(pValue)
				{
					while(*pValue && cch<(MAXURLLENGTH-1))
					{
						pOutput[cch++]=*pValue;
						pValue++;
					}
				}

				InsideTemplateParamName=false;
			}
			else if(i<(MAXURLLENGTH-1)) TemplateParamName[i++]=ch;
		}
		else if(ch==L'{')
		{
			i=0;
			InsideTemplateParamName=true;
		}
		else pOutput[cch++]=ch;

		pTemplate++;
		ch=*pTemplate;
	}

	if(pOutput) pOutput[cch]=0;
}

bool CProfiles::InterceptSearchTerms(TCHAR *pQuery,bool bIsURL,SearchEngine *pEngine,TCHAR pInterceptedSearchTerms[MAXURLLENGTH])
{
	bool result;
	TCHAR QueryParamName[MAXURLLENGTH];
	TCHAR Params[MAX_QUERY_PARAMETERS][MAXURLLENGTH];
	UINT i;
	UINT ParamIndex;
	TCHAR ch;
	bool bInsideValue;
	bool bInsideQuery;
	CIDNA idna;

	for(i=0;i<pEngine->ParamCount;i++) Params[i][0]=0;

	// Parse parameters

	i=0;
	ParamIndex=MAX_QUERY_PARAMETERS;
	bInsideValue=false;
	bInsideQuery=!bIsURL;
	ch=*pQuery;
	while(ch)
	{
		if(bInsideValue)
		{
			if(ch==L'&' || ch==L'#')
			{
				bInsideValue=false;
				if(ParamIndex<MAX_QUERY_PARAMETERS) Params[ParamIndex][i]=0;
				i=0;
			}
			else if(ParamIndex<MAX_QUERY_PARAMETERS && i<(MAXURLLENGTH-1)) Params[ParamIndex][i++]=ch;			
		}
		else
		{
			switch(ch)
			{
			case L'?':
			case L'#': // ajax query
				i=0;
				bInsideQuery=true;
				break;
			case L'&':
				i=0;
				break;
			case L'=':
				if(bInsideQuery)
				{
					QueryParamName[i]=0;
					i=0;
					ParamIndex=MAX_QUERY_PARAMETERS;
					while(i<pEngine->ParamCount)
					{
						if(pEngine->ParamNames[i] && !StrCmp(QueryParamName,pEngine->ParamNames[i]))
						{
							ParamIndex=i;
							break;
						}
						i++;
					}
					i=0;
					bInsideValue=true;
				}
				break;
			default:
				if(i<(MAXURLLENGTH-1)) QueryParamName[i++]=ch;
			}
		}
		// Skip #!
		if(ch==L'#' && *(pQuery+1)==L'!') pQuery++;

		pQuery++;
		ch=*pQuery;
	}

	// Terminate parameter

	if(bInsideValue)
	{
		if(ParamIndex<MAX_QUERY_PARAMETERS) Params[ParamIndex][i]=0;
	}

	// Decode parameters

	result=false;
	i=0;
	while(i<pEngine->ParamCount)
	{
		if(LOWORD(pEngine->Flags)==CODEPAGE_UTF8) idna.DecodeUTF8(Params[i]);
		else idna.DecodeHex(Params[i],LOWORD(pEngine->Flags));

		CQToolbar::trim(Params[i]);
		if(Params[i][0]) result=true;

		i++;
	}

	// Concatenate paramters

	if(result)
	{
		i=0;
		pInterceptedSearchTerms[0]=0;
		while(i<pEngine->ParamCount)
		{
			StringCchCat(pInterceptedSearchTerms,MAXURLLENGTH,Params[i]);
			i++;
			if(i<pEngine->ParamCount)
			{
				if(pEngine->iRequiresAddress && (i+1)==pEngine->ParamCount)
				{
					if(Params[i][0]) StringCchCat(pInterceptedSearchTerms,MAXURLLENGTH,L" @ ");
				}
				else StringCchCat(pInterceptedSearchTerms,MAXURLLENGTH,L" - ");
			}
		}
	}

	return result;
}

bool CProfiles::InterceptSearch(TCHAR *pURL,TCHAR *pPostDataUnicode,int *pSearchEngineIndex,int *pSearchEngineId,TCHAR pInterceptedSearchTerms[MAXURLLENGTH])
{
	BSTR pInterceptURL;
	TCHAR *pInterceptData;
	bool bIsURL;
	bool result;
	UINT i;
	UINT length_InterceptURL;

	result=false;
	
	if(pPostDataUnicode && pPostDataUnicode[0])
	{
		pInterceptData=pPostDataUnicode;
		bIsURL=false;
	}
	else
	{
		pInterceptData=pURL;
		bIsURL=true;
	}

	i=0;
	while(i<CurrentProfile.EngineCount)
	{
		pInterceptURL=CurrentProfile.Engines[i].InterceptURL;
		if(pInterceptURL)
		{
			length_InterceptURL=SysStringLen(pInterceptURL);
			if(!StrCmpN(pURL,pInterceptURL,length_InterceptURL) || 
				(!StrCmpN(pURL,L"https://",8) && !StrCmpN(pInterceptURL,L"http://",7) && !StrCmpN(pURL+8,pInterceptURL+7,length_InterceptURL-7)) )
			{
				if(InterceptSearchTerms(pInterceptData,bIsURL,&CurrentProfile.Engines[i],pInterceptedSearchTerms))
				{
					*pSearchEngineIndex=i;
					*pSearchEngineId=CurrentProfile.Engines[i].id;
					result=true;
					break;
				}
			}
		}

		i++;
	}

	return result;
}