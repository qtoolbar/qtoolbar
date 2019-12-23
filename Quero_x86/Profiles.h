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
// Profiles.h: interface for the CProfiles class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROFILES_H__044BDB3E_31DD_43A7_A33A_8B5DD987B98F__INCLUDED_)
#define AFX_PROFILES_H__044BDB3E_31DD_43A7_A33A_8B5DD987B98F__INCLUDED_

#include "resource.h"
#include "IDNA.h"

// Limits

#define MAX_ENGINES 64
#define MAX_PROFILES 64
#define MAX_QUERY_PARAMETERS 10
#define MAX_SUBKEY_LEN 32
#define MAX_PROFILE_NAME_LEN 40
#define MAX_SEARCHENGINE_NAME_LEN 40

// Ids

#define PROFILE_CUSTOM_ID 0x80
#define PROFILE_ID_ERROR 0xFF
#define SEARCHENGINE_CUSTOM_ID 0x80
#define SEARCHENGINE_ID_ERROR 0xFF
#define SEARCHENGINE_ID_FREE 0xFF
#define SEARCHENGINE_ID_SEPARATOR 0xFF

// Profiles Flags

#define PROFILES_LIST_MODIFIED 0x1
#define PROFILES_CUSTOM 0x10000

#define PROFILES_DEFAULT_FLAGS 0x0

// Profile Flags

#define PROFILE_LIST_MODIFIED PROFILES_LIST_MODIFIED // 0x1
#define PROFILE_NAME_MODIFIED 0x2
#define PROFILE_LUCKYURL_MODIFIED 0x4
#define PROFILE_DEFAULTENGINE_MODIFIED 0x8
#define PROFILE_CUSTOM 0x10000

#define PROFILE_DEFAULT_FLAGS 0x0

// Google LuckyURL

#define PROFILE_DEFAULT_LUCKYURL L"http://www.google.com/search?ie=UTF-8&btnI=&q={searchTerms}"

// Search Engine Registry Values

#define SEARCHENGINE_VALUES_COUNT 7
#define SEARCHENGINE_VALUES_NAME 0
#define SEARCHENGINE_VALUES_QUERY 1
#define SEARCHENGINE_VALUES_LINK 2
#define SEARCHENGINE_VALUES_POST 3
#define SEARCHENGINE_VALUES_COUNTRYPARAM 4
#define SEARCHENGINE_VALUES_FLAGS 5
#define SEARCHENGINE_VALUES_ICON 6
const static TCHAR* SEARCHENGINE_VALUES[SEARCHENGINE_VALUES_COUNT]={L"Name",L"Query",L"Link",L"Post",L"CountryParam",L"Flags",L"Icon"};

// Encodings

#define CODEPAGE_NO_ENCODING 0xFFFF // used for www.{searchTerms}.com template
#define CODEPAGE_UTF8 65001
#define CODEPAGE_Arabic 1256
#define CODEPAGE_Baltic 1257
#define CODEPAGE_Central_European 1250
#define CODEPAGE_Chinese_Simplified_GB2312 936
#define CODEPAGE_Chinese_Traditional_BIG5 950
#define CODEPAGE_Cyrillic 1251
#define CODEPAGE_Greek 1253
#define CODEPAGE_Hebrew 1255
#define CODEPAGE_Japanese 932
#define CODEPAGE_Korean 949
#define CODEPAGE_Thai 874
#define CODEPAGE_Turkish 1254
#define CODEPAGE_Vietnamese 1258
#define CODEPAGE_Western_European 1252

#define N_ENCODINGS 15
static const UINT Encoding_Names[N_ENCODINGS]={
	IDS_CODEPAGE_UTF8,
	IDS_CODEPAGE_Arabic,
	IDS_CODEPAGE_Baltic,
	IDS_CODEPAGE_Central_European,
	IDS_CODEPAGE_Chinese_Simplified_GB2312,
	IDS_CODEPAGE_Chinese_Traditional_BIG5,
	IDS_CODEPAGE_Cyrillic,
	IDS_CODEPAGE_Greek,
	IDS_CODEPAGE_Hebrew,
	IDS_CODEPAGE_Japanese,
	IDS_CODEPAGE_Korean,
	IDS_CODEPAGE_Thai,
	IDS_CODEPAGE_Turkish,
	IDS_CODEPAGE_Vietnamese,
	IDS_CODEPAGE_Western_European
};
static const UINT Encoding_CodePages[N_ENCODINGS]={
	CODEPAGE_UTF8,
	CODEPAGE_Arabic,
	CODEPAGE_Baltic,
	CODEPAGE_Central_European,
	CODEPAGE_Chinese_Simplified_GB2312,
	CODEPAGE_Chinese_Traditional_BIG5,
	CODEPAGE_Cyrillic,
	CODEPAGE_Greek,
	CODEPAGE_Hebrew,
	CODEPAGE_Japanese,
	CODEPAGE_Korean,
	CODEPAGE_Thai,
	CODEPAGE_Turkish,
	CODEPAGE_Vietnamese,
	CODEPAGE_Western_European
};
static const TCHAR* Encoding_CharSetNames[N_ENCODINGS]={ // IANA Charset Names http://www.iana.org/assignments/character-sets
	L"UTF-8",
	L"ISO-8859-6",
	L"windows-1257",
	L"ISO-8859-2",
	L"GBK",
	L"Big5",
	L"ISO-8859-5",
	L"ISO-8859-7",
	L"ISO-8859-8",
	L"Shift_JIS",
	L"EUC-KR",
	L"ISO-8859-11",
	L"ISO-8859-9",
	L"windows-1258",
	L"ISO-8859-1"
};

// Search Engine Flags (LOWORD is CodePage)

#define SEARCHENGINE_CUSTOM PROFILE_CUSTOM
#define SEARCHENGINE_PARTIALSUPPORT 0x20000

#define SEARCHENGINE_DEFAULT_FLAGS CODEPAGE_UTF8
#define SEARCHENGINE_DEFAULT_ICONID 4

// OpenSearch Template Parameter Names

#define COUNT_OPENSEARCH_PARAMS 7
#define OPENSEARCH_PARAMS_SEARCHTERMS 0
#define OPENSEARCH_PARAMS_COUNT 1
#define OPENSEARCH_PARAMS_STARTINDEX 2
#define OPENSEARCH_PARAMS_STARTPAGE 3
#define OPENSEARCH_PARAMS_LANGUAGE 4
#define OPENSEARCH_PARAMS_INPUTENCODING 5
#define OPENSEARCH_PARAMS_OUTPUTENCODING 6
const static TCHAR* OPENSEARCH_PARAMS[COUNT_OPENSEARCH_PARAMS]={L"searchTerms",L"count",L"startIndex",L"startPage",L"language",L"inputEncoding",L"outputEncoding"};

// Quero Parameter Names

#define QUERO_PARAMS_COUNT 9
#define QUERO_PARAMS_YEAR 0
#define QUERO_PARAMS_MONTH 1
#define QUERO_PARAMS_DAY 2
#define QUERO_PARAMS_HOUR 3
#define QUERO_PARAMS_MIN 4
#define QUERO_PARAMS_SEC 5
#define QUERO_PARAMS_URL 6
#define QUERO_PARAMS_HOSTNAME 7
#define QUERO_PARAMS_DOMAINNAME 8
const static TCHAR* QUERO_PARAMS[QUERO_PARAMS_COUNT]={L"Year",L"Month",L"Day",L"Hour",L"Min",L"Sec",L"url",L"hostname",L"domainname"};

// Search Engine structure

typedef struct SearchEngine {
	BYTE id;
	BSTR Name;
	BSTR LinkURL;
	BSTR QueryURL;
	BSTR InterceptURL;
	BSTR PostQuery;
	BSTR ParamNames[MAX_QUERY_PARAMETERS];
	UINT ParamCount;
	BSTR CountryParam;
	DWORD Flags;
	BSTR IconFile;
	BYTE IconCacheRetryTimer; // Try to load icon if timer reaches zero
	HICON hIcon;
	bool HasSeparator;
	UINT iRequiresAddress; // Id of most specific url template paramter present in QueryURL (Quero:url > Quero:hostname > Quero:domainname), or 0 if none
	bool bRequiresKeywords; // True if query template contains {searchTerms} or Quero:p1 ... p9
} SearchEngine;

// Profile structe

typedef struct Profile {
	BYTE id;
	BSTR Name;
	DWORD Flags;
	BYTE DefaultEngineId;
	UINT EngineCount;
	SearchEngine LuckySearch;
	SearchEngine Engines[MAX_ENGINES];
} Profile;


class CProfiles
{
private:
	COLORREF LinkColor;
	UINT EnumProfileIndex;

public:
	CProfiles();
	~CProfiles();

	bool LoadCurrentProfile(int ProfileId);

	bool GetEngine(int ProfileId,int EngineId,SearchEngine *pEngine);

	TCHAR* GetProfileName(int ProfileId);

	COLORREF GetEngineColor(SearchEngine *pEngine);
	HICON GetEngineIcon(SearchEngine *pEngine,HICON hDefaultIcon);

	int First();
	int Next();
	int GetNextProfileId(int ProfileId);
	int GetPreviousProfileId(int ProfileId);
	int ProfileIdToIndex(int ProfileId);
	int IndexToProfileId(int ProfileIndex);
	int EngineIdToIndex(int EngineId);
	int IndexToEngineId(int EngineIndex);

	bool IsLuckyURL(TCHAR *pURL,TCHAR pInterceptedSearchTerms[MAXURLLENGTH]);
	bool InterceptSearchTerms(TCHAR *pQuery,bool bIsURL,SearchEngine *pEngine,TCHAR pInterceptedSearchTerms[MAXURLLENGTH]);
	bool InterceptSearch(TCHAR *pURL,TCHAR *pPostDataUnicode,int *pSearchEngineIndex,int *pSearchEngineId,TCHAR pInterceptedSearchTerms[MAXURLLENGTH]);
	bool GetLuckySearchEngine(int ProfileId,SearchEngine *);
	void LoadLuckySearchEngine(HKEY hKeyProfile,SearchEngine *pEngine);

	inline void SetFontColor(COLORREF lc) { LinkColor=lc; }

	HKEY OpenProfiles(bool bCurrentUser,bool bCreateKey);
	HKEY OpenSubKey(HKEY hKey,int id,bool bCreateKey);
	bool DeleteSubKey(HKEY hKey,int id);

	void InitProfile(Profile *pProfile);
	void InitEngine(SearchEngine *pEngine);
	void InitParams(SearchEngine *pEngine,TCHAR *pTemplate); // Extract parameter names and count
	void AssignEngine(SearchEngine *pEngineDest,SearchEngine *pEngineSrc);
	void AssignProfile(Profile *pProfileDest,Profile *pProfileSrc);
	void CopyEngine(SearchEngine *pEngineDest,SearchEngine *pEngineSrc);
	void CopyProfile(Profile *pProfileDest,Profile *pProfileSrc);

	UINT LoadList(HKEY hKey,BYTE *pList,UINT ListSize);
	bool SaveList(HKEY hKey,BYTE *pList,UINT ListSize);
	bool LoadEngine(HKEY hKeyProfile,int EngineId,SearchEngine *pEngine);
	void FreeEngine(SearchEngine *pEngine);
	void FreeProfile(Profile *pProfile);
	bool SaveEngine(HKEY hKeyProfile,SearchEngine *pEngine,bool bOverwriteCustom);
	BYTE LoadDefaultId(HKEY hKey);
	BYTE LoadDefaultProfileId(HKEY hKeyProfiles);
	bool SaveDefaultId(HKEY hKey,BYTE DefaultId);
	bool SaveDefaultProfileId(BYTE DefaultProfileId,bool bCurrentUser);
	bool SaveDefaultEngineId(Profile *pProfile,bool bCurrentUser);
	bool SaveDefaultEngineId(HKEY hKeyProfile,Profile *pProfile);
	bool LoadProfile(HKEY hKey,int ProfileId,Profile *pProfile);
	bool SaveProfile(HKEY hKey,Profile *pProfile,bool bMergeProfile);
	bool UpdateUserProfiles();
	void MergeLists(BYTE *pOldList,UINT OldListSize,BYTE *pNewList,UINT *pNewListSize,int Flags,HKEY hKey,bool bSearchEngineList);
	BYTE LoadSyncId(HKEY hKey);
	bool SaveSyncId(HKEY hKey,BYTE SyncId);
	DWORD LoadFlags(HKEY hKey,bool bSearchEngineFlags);
	bool SaveFlags(HKEY hKey,DWORD Flags,DWORD DefaultFlags);
	bool IsInList(BYTE id,BYTE *pList,int ListSize);
	BSTR LoadString(HKEY hKey,TCHAR *pValueName,UINT MaxLen);
	bool SaveString(HKEY hKey,TCHAR *pValueName,BSTR bstrName);
	void LoadProfileList();
	void LoadNames();
	void FreeNames();

	BYTE NewCustomProfileId(BYTE *pList,UINT ListSize);
	BYTE NewCustomEngineId(Profile *pProfile);

	void PrepareNavigation(TCHAR *pQuery,SearchEngine *pEngine,BSTR *pbstrURL,char **ppPostData);
	UINT SplitParams(TCHAR *pQuery,size_t QueryLength,TCHAR Params[MAX_QUERY_PARAMETERS][MAXURLLENGTH],UINT ParamCount);
	void BuildQuery(const TCHAR *pOpenSearchParams[COUNT_OPENSEARCH_PARAMS],TCHAR *pQueryParams[],UINT ParamCount,TCHAR *pQueroParams[QUERO_PARAMS_COUNT],TCHAR *pTemplate,TCHAR *pOutput);

public:
	UINT ProfileCount;
	bool bNamesLoaded;
	BSTR ProfileNames[MAX_PROFILES];
	BYTE ProfileList[MAX_PROFILES];
	int DefaultProfileId;
	Profile CurrentProfile;
};

#endif // !defined(AFX_PROFILES_H__044BDB3E_31DD_43A7_A33A_8B5DD987B98F__INCLUDED_)
