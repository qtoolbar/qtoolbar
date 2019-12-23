# Quero Toolbar
Version 8

Navigation Bar and Ad Blocker project for Internet Explorer

Copyright 2004-2013 Viktor Krammer

Open source license
GNU General Public License v3

Website
http://www.quero.at/

Email
support at quero.at

/////////////////////////////////////////////////////////////////////

1 Requirements

2 Software components

3 Build instructions

4 Component registration

5 Registry keys

/////////////////////////////////////////////////////////////////////

1 Requirements

* MS Windows 2000 or higher (Windows 8 and x64 supported)
* MS Internet Explorer 5.5 or higher (IE11 supported)
* MS Visual Studio 2005 or higher
* MS Windows SDK 6.1 for Windows Vista and Windows Server 2008 or higher (recommended Windows SDK 7.1 for Windows 7)
* Windows Template Library WTL 8.0 or higher
  http://sourceforge.net/projects/wtl/
* Inno Setup 5.3 or higher
  http://www.jrsoftware.org/


2 Software components

* Quero.dll
  Internet Explorer add-on (BHO, IQueroBand, IQueroFilter)
  runs at Low IL under Windows Vista or higher
  (if protected mode is enabled)
* QueroBroker.exe
  COM server (IQueroBroker)
  Quero Broker process (required for Internet Explorer 7 and higher)
  runs at Medium IL under Windows Vista or higher (if UAC is enabled)
  manages all open IE windows and its associated Quero instances
  provides a communication channel between Quero instances
  performs elevated operations
* QueroBroker.dll
  Helper dll
  injected into IE frame processes
  implements keyboard and message hooks
* Installer
  based on Inno Setup


3 Build instructions

* Visual Studio 2005 solution files included
* Build different platform versions by replacing the Build.h
  with the desired platform Build file
* Build the German version by replacing
  Quero.rc with Quero_DE.rc
* Building the x64 version
  copy the source files into the Quero_x64 folder
  copy the _x64 Build file to Build.h


4 Component registration

* Register
  QueroBroker.exe /RegServer
  regsvr32 Quero.dll
* Unregister
  regsvr32 /u Quero.dll
  QueroBroker.exe /UnregServer


5 Registry keys

* Main settings under Windows 2000, XP
  [HKEY_CURRENT_USER\Software\Quero Toolbar]
* Main settings under Windows 7 and Windows Vista
  [HKEY_CURRENT_USER\Software\AppDataLow\Software\Quero Toolbar]
* Main settings under Windows 8
  [HKEY_CURRENT_USER\Software\Microsoft\Internet Explorer\InternetRegistry\Software\Quero Toolbar]
* Default search profile key
  [HKEY_USERS\.DEFAULT\Software\Quero Toolbar\Profiles]
