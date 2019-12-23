# Microsoft Developer Studio Project File - Name="Quero" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Quero - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Quero.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Quero.mak" CFG="Quero - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Quero - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Quero - Win32 Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Quero - Win32 Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Quero - Win32 Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Quero - Win32 Unicode Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Quero - Win32 Unicode Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Quero - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "ISOLATION_AWARE_ENABLED" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0xc07 /d "_DEBUG"
# ADD RSC /l 0xc07 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib winmm.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# Begin Custom Build - Performing registration
OutDir=.\Debug
TargetPath=.\Debug\Quero.dll
InputPath=.\Debug\Quero.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Quero - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugU"
# PROP BASE Intermediate_Dir "DebugU"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugU"
# PROP Intermediate_Dir "DebugU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /D "_DEBUG" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "ISOLATION_AWARE_ENABLED" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0xc07 /d "_DEBUG"
# ADD RSC /l 0xc07 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib winmm.lib wininet.lib Usp10.lib shlwapi.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Performing registration
OutDir=.\DebugU
TargetPath=.\DebugU\Quero.dll
InputPath=.\DebugU\Quero.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Quero - Win32 Release MinSize"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMinSize"
# PROP BASE Intermediate_Dir "ReleaseMinSize"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMinSize"
# PROP Intermediate_Dir "ReleaseMinSize"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /O1 /D "NDEBUG" /D "_MBCS" /D "_ATL_DLL" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "ISOLATION_AWARE_ENABLED" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0xc07 /d "NDEBUG"
# ADD RSC /l 0xc07 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib winmm.lib /nologo /subsystem:windows /dll /machine:I386
# Begin Custom Build - Performing registration
OutDir=.\ReleaseMinSize
TargetPath=.\ReleaseMinSize\Quero.dll
InputPath=.\ReleaseMinSize\Quero.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Quero - Win32 Release MinDependency"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMinDependency"
# PROP BASE Intermediate_Dir "ReleaseMinDependency"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMinDependency"
# PROP Intermediate_Dir "ReleaseMinDependency"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /O1 /D "NDEBUG" /D "_MBCS" /D "_ATL_STATIC_REGISTRY" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "ISOLATION_AWARE_ENABLED" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0xc07 /d "NDEBUG"
# ADD RSC /l 0xc07 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib winmm.lib /nologo /subsystem:windows /dll /machine:I386
# Begin Custom Build - Performing registration
OutDir=.\ReleaseMinDependency
TargetPath=.\ReleaseMinDependency\Quero.dll
InputPath=.\ReleaseMinDependency\Quero.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Quero - Win32 Unicode Release MinSize"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUMinSize"
# PROP BASE Intermediate_Dir "ReleaseUMinSize"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUMinSize"
# PROP Intermediate_Dir "ReleaseUMinSize"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "_ATL_DLL" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /O1 /D "NDEBUG" /D "_UNICODE" /D "_ATL_DLL" /D "_WIN32" /D "_WINDOWS" /D "_USRDLL" /D "ISOLATION_AWARE_ENABLED" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0xc07 /d "NDEBUG"
# ADD RSC /l 0xc07 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib winmm.lib /nologo /subsystem:windows /dll /machine:I386
# Begin Custom Build - Performing registration
OutDir=.\ReleaseUMinSize
TargetPath=.\ReleaseUMinSize\Quero.dll
InputPath=.\ReleaseUMinSize\Quero.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Quero - Win32 Unicode Release MinDependency"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUMinDependency"
# PROP BASE Intermediate_Dir "ReleaseUMinDependency"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUMinDependency"
# PROP Intermediate_Dir "ReleaseUMinDependency"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /Zd /O1 /D "NDEBUG" /D "_UNICODE" /D "_ATL_STATIC_REGISTRY" /D "_WIN32" /D "_WINDOWS" /D "_USRDLL" /D "ISOLATION_AWARE_ENABLED" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0xc07 /d "NDEBUG"
# ADD RSC /l 0xc07 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib winmm.lib wininet.lib Usp10.lib shlwapi.lib /nologo /subsystem:windows /dll /map /machine:I386 /MAPINFO:LINES /MAPINFO:EXPORTS
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Performing registration
OutDir=.\ReleaseUMinDependency
TargetPath=.\ReleaseUMinDependency\Quero.dll
InputPath=.\ReleaseUMinDependency\Quero.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "Quero - Win32 Debug"
# Name "Quero - Win32 Unicode Debug"
# Name "Quero - Win32 Release MinSize"
# Name "Quero - Win32 Release MinDependency"
# Name "Quero - Win32 Unicode Release MinSize"
# Name "Quero - Win32 Unicode Release MinDependency"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AutoComplete.cpp
# End Source File
# Begin Source File

SOURCE=.\ButtonBar.cpp
# End Source File
# Begin Source File

SOURCE=.\ComboEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\ComboQuero.cpp
# End Source File
# Begin Source File

SOURCE=.\ContentFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\FavIcon.cpp
# End Source File
# Begin Source File

SOURCE=.\IDNA.cpp
# End Source File
# Begin Source File

SOURCE=.\LogoToolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\NavBar.cpp
# End Source File
# Begin Source File

SOURCE=.\OLEDataObject.cpp
# End Source File
# Begin Source File

SOURCE=.\OLEDropSource.cpp
# End Source File
# Begin Source File

SOURCE=.\OLEEnumFormat.cpp
# End Source File
# Begin Source File

SOURCE=.\Profiles.cpp
# End Source File
# Begin Source File

SOURCE=.\QDebug.cpp
# End Source File
# Begin Source File

SOURCE=.\QEditCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\QHTMLFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\QToolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\Quero.cpp
# End Source File
# Begin Source File

SOURCE=.\Quero.def
# End Source File
# Begin Source File

SOURCE=.\Quero.idl
# ADD MTL /tlb ".\Quero.tlb" /h "Quero.h" /iid "Quero_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\Quero.rc
# End Source File
# Begin Source File

SOURCE=.\QueroBand.cpp
# End Source File
# Begin Source File

SOURCE=.\RedirectRequest.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchAnimation.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Toolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolbarIcons.cpp
# End Source File
# Begin Source File

SOURCE=.\UIEditInplace.cpp
# End Source File
# Begin Source File

SOURCE=.\UIListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\UIOptionsAdBlocker.cpp
# End Source File
# Begin Source File

SOURCE=.\UIOptionsAdvanced.cpp
# End Source File
# Begin Source File

SOURCE=.\UIOptionsAppearance.cpp
# End Source File
# Begin Source File

SOURCE=.\UIOptionsProfiles.cpp
# End Source File
# Begin Source File

SOURCE=.\UIOptionsPropSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\UIOptionsSecurity.cpp
# End Source File
# Begin Source File

SOURCE=.\UIOptionsSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\UIResizeWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\UISearchEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\UISearchProfile.cpp
# End Source File
# Begin Source File

SOURCE=.\UIWarning.cpp
# End Source File
# Begin Source File

SOURCE=.\UIWhiteList.cpp
# End Source File
# Begin Source File

SOURCE=.\UIZoomFactor.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AutoComplete.h
# End Source File
# Begin Source File

SOURCE=.\ButtonBar.h
# End Source File
# Begin Source File

SOURCE=.\ComboEngine.h
# End Source File
# Begin Source File

SOURCE=.\ComboQuero.h
# End Source File
# Begin Source File

SOURCE=.\ContentFilter.h
# End Source File
# Begin Source File

SOURCE=.\FavIcon.h
# End Source File
# Begin Source File

SOURCE=.\IDNA.h
# End Source File
# Begin Source File

SOURCE=.\LogoToolbar.h
# End Source File
# Begin Source File

SOURCE=.\NavBar.h
# End Source File
# Begin Source File

SOURCE=.\OLEDataObject.h
# End Source File
# Begin Source File

SOURCE=.\OLEDropSource.h
# End Source File
# Begin Source File

SOURCE=.\OLEEnumFormat.h
# End Source File
# Begin Source File

SOURCE=.\Profiles.h
# End Source File
# Begin Source File

SOURCE=.\QDebug.h
# End Source File
# Begin Source File

SOURCE=.\QEditCtrl.h
# End Source File
# Begin Source File

SOURCE=.\QHTMLFilter.h
# End Source File
# Begin Source File

SOURCE=.\QToolbar.h
# End Source File
# Begin Source File

SOURCE=.\Quero.h
# End Source File
# Begin Source File

SOURCE=.\QueroBand.h
# End Source File
# Begin Source File

SOURCE=.\RedirectRequest.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SearchAnimation.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Toolbar.h
# End Source File
# Begin Source File

SOURCE=.\ToolbarIcons.h
# End Source File
# Begin Source File

SOURCE=.\UIEditInplace.h
# End Source File
# Begin Source File

SOURCE=.\UIListBox.h
# End Source File
# Begin Source File

SOURCE=.\UIOptionsAdBlocker.h
# End Source File
# Begin Source File

SOURCE=.\UIOptionsAdvanced.h
# End Source File
# Begin Source File

SOURCE=.\UIOptionsAppearance.h
# End Source File
# Begin Source File

SOURCE=.\UIOptionsProfiles.h
# End Source File
# Begin Source File

SOURCE=.\UIOptionsPropSheet.h
# End Source File
# Begin Source File

SOURCE=.\UIOptionsSecurity.h
# End Source File
# Begin Source File

SOURCE=.\UIOptionsSettings.h
# End Source File
# Begin Source File

SOURCE=.\UIResizeWindow.h
# End Source File
# Begin Source File

SOURCE=.\UISearchEngine.h
# End Source File
# Begin Source File

SOURCE=.\UISearchProfile.h
# End Source File
# Begin Source File

SOURCE=.\UIWarning.h
# End Source File
# Begin Source File

SOURCE=.\UIWhitelist.h
# End Source File
# Begin Source File

SOURCE=.\UIZoomFactor.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=E:\Data\Web\wwwquero\images\bar1.jpg
# End Source File
# Begin Source File

SOURCE=.\button_go.ico
# End Source File
# Begin Source File

SOURCE=.\button_go_hot.ico
# End Source File
# Begin Source File

SOURCE=.\button_quero.ico
# End Source File
# Begin Source File

SOURCE=.\button_quero_256.ico
# End Source File
# Begin Source File

SOURCE=.\button_quero_small.ico
# End Source File
# Begin Source File

SOURCE=E:\Data\Web\wwwquero\images\dot1.gif
# End Source File
# Begin Source File

SOURCE=.\engine1.ico
# End Source File
# Begin Source File

SOURCE=.\icologo1.ico
# End Source File
# Begin Source File

SOURCE=.\icologo1b.ico
# End Source File
# Begin Source File

SOURCE=.\icon_123.ico
# End Source File
# Begin Source File

SOURCE=.\icon_bin.ico
# End Source File
# Begin Source File

SOURCE=.\icon_book.ico
# End Source File
# Begin Source File

SOURCE=.\icon_chart.ico
# End Source File
# Begin Source File

SOURCE=.\icon_compass.ico
# End Source File
# Begin Source File

SOURCE=.\icon_engine.ico
# End Source File
# Begin Source File

SOURCE=.\icon_idn.ico
# End Source File
# Begin Source File

SOURCE=.\icon_lock.ico
# End Source File
# Begin Source File

SOURCE=.\icon_movie.ico
# End Source File
# Begin Source File

SOURCE=.\icon_price.ico
# End Source File
# Begin Source File

SOURCE=.\icon_science.ico
# End Source File
# Begin Source File

SOURCE=.\icon_telephone.ico
# End Source File
# Begin Source File

SOURCE=.\icon_trolley.ico
# End Source File
# Begin Source File

SOURCE=.\icon_tv.ico
# End Source File
# Begin Source File

SOURCE=.\icon_weather.ico
# End Source File
# Begin Source File

SOURCE=.\nav_back.ico
# End Source File
# Begin Source File

SOURCE=.\nav_back_disabled.ico
# End Source File
# Begin Source File

SOURCE=.\nav_back_hot.ico
# End Source File
# Begin Source File

SOURCE=.\nav_forward.ico
# End Source File
# Begin Source File

SOURCE=.\nav_forward_disabled.ico
# End Source File
# Begin Source File

SOURCE=.\nav_forward_hot.ico
# End Source File
# Begin Source File

SOURCE=.\nav_home.ico
# End Source File
# Begin Source File

SOURCE=.\nav_home_hot.ico
# End Source File
# Begin Source File

SOURCE=.\nav_refresh.ico
# End Source File
# Begin Source File

SOURCE=.\nav_refresh_hot.ico
# End Source File
# Begin Source File

SOURCE=.\nav_stop.ico
# End Source File
# Begin Source File

SOURCE=.\nav_stop_hot.ico
# End Source File
# Begin Source File

SOURCE=.\options_adblocker.ico
# End Source File
# Begin Source File

SOURCE=.\options_buttons.ico
# End Source File
# Begin Source File

SOURCE=.\options_highlight.ico
# End Source File
# Begin Source File

SOURCE=.\options_navigation.ico
# End Source File
# Begin Source File

SOURCE=.\options_search.ico
# End Source File
# Begin Source File

SOURCE=.\options_security.ico
# End Source File
# Begin Source File

SOURCE=.\options_typefind.ico
# End Source File
# Begin Source File

SOURCE=.\QHTMLFilter.rgs
# End Source File
# Begin Source File

SOURCE=E:\Data\Web\wwwquero\images\qtoolbar.gif
# End Source File
# Begin Source File

SOURCE=.\quero.ico
# End Source File
# Begin Source File

SOURCE=E:\Data\Web\wwwquero\images\quero1.gif
# End Source File
# Begin Source File

SOURCE=.\quero1.ico
# End Source File
# Begin Source File

SOURCE=.\quero1b.ico
# End Source File
# Begin Source File

SOURCE=.\quero2.ico
# End Source File
# Begin Source File

SOURCE=.\quero2b.ico
# End Source File
# Begin Source File

SOURCE=.\QueroBand.rgs
# End Source File
# Begin Source File

SOURCE=.\search.ico
# End Source File
# Begin Source File

SOURCE=.\search_hot.ico
# End Source File
# Begin Source File

SOURCE=.\search_notfound.ico
# End Source File
# Begin Source File

SOURCE=.\separator.bmp
# End Source File
# Begin Source File

SOURCE=.\shield.bmp
# End Source File
# Begin Source File

SOURCE=.\UISettingsPropSheet.rgs
# End Source File
# Begin Source File

SOURCE=.\url1.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\beep.wav
# End Source File
# Begin Source File

SOURCE=.\Quero.dll.manifest
# End Source File
# End Target
# End Project
