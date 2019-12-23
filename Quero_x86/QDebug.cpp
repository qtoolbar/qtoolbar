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
// QDebug.cpp: implementation of the CQDebug class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "QDebug.h"
#include <strsafe.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#if defined QDEBUG

class CQDebug QueroDebug;

CQDebug::CQDebug()
{
#if defined QDEBUG_CONSOLE
	AllocConsole();
	SetConsoleTitle(L"Quero Debug Console");
	hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
#endif

#if defined QDEBUG_LOGFILE
	TCHAR dir[MAX_PATH];
	TCHAR filename[MAX_PATH];
	TCHAR filepath[MAX_PATH];
	
	if(SHGetFolderPath(NULL,CSIDL_PERSONAL,NULL,SHGFP_TYPE_CURRENT,dir)==S_OK)
	{
		StringCbPrintf(filename,sizeof filename,L"Quero_Debug_Log_%d.txt",GetCurrentProcessId());
		PathCombine(filepath,dir,filename);

		hLogFile=CreateFile(filepath,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	}

#endif
}

CQDebug::~CQDebug()
{
#if defined QDEBUG_CONSOLE
	if(hConsole) FreeConsole();
#endif

#if defined QDEBUG_LOGFILE
	if(hLogFile) CloseHandle(hLogFile);
#endif
}

void CQDebug::write_string(TCHAR *pString)
{
	size_t nChars;
	
	if(StringCchLength(pString,STRSAFE_MAX_CCH,&nChars)!=S_OK) nChars=0;

#if defined QDEBUG_CONSOLE
	if(hConsole) WriteConsole(hConsole,pString,(DWORD)nChars,NULL,0);
#endif

#if defined QDEBUG_LOGFILE
	DWORD nBytesWritten;
	if(hLogFile) WriteFile(hLogFile,pString,(DWORD)nChars*sizeof TCHAR,&nBytesWritten,NULL);
#endif
}

void CQDebug::write_newline()
{
#if defined QDEBUG_CONSOLE
	if(hConsole) WriteConsole(hConsole,L"\n",1,NULL,0);
#endif

#if defined QDEBUG_LOGFILE
	DWORD nBytesWritten;
	if(hLogFile)
	{
		WriteFile(hLogFile,L"\r\n",2*sizeof TCHAR,&nBytesWritten,NULL);
		FlushFileBuffers(hLogFile);
	}
#endif
}

void CQDebug::write_timestamp()
{
	SYSTEMTIME st;
	TCHAR ts[32];

	GetLocalTime(&st);

	StringCbPrintf(ts,sizeof ts,L"%02d:%02d:%02d.%03d ",st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);

	write_string(ts);
}

void CQDebug::printf(TCHAR *event,TCHAR *fmt, ...)
{
	va_list arg;
	va_start(arg,fmt);

	StringCbVPrintf(text,sizeof text,fmt,arg);

	write_timestamp();
	write_string(event);
	write_string(L": ");
	write_string(text);
	write_newline();

#if defined QDEBUG_MESSAGEBOX
	MessageBox(NULL,text,event,MB_OK);
#endif

	va_end(arg);
}

void CQDebug::print(TCHAR *event,TCHAR *details)
{
	write_timestamp();
	write_string(event);
	write_string(L": ");
	write_string(details);
	write_newline();

#if defined QDEBUG_MESSAGEBOX
	MessageBox(NULL,details,event,MB_OK);
#endif
}

void CQDebug::printA(TCHAR *event,CHAR *details)
{
	MultiByteToWideChar(CP_ACP,0,(char*)details,-1,text,sizeof text/sizeof TCHAR);

	write_timestamp();
	write_string(event);
	write_string(L": ");
	write_string(text);
	write_newline();

#if defined QDEBUG_MESSAGEBOX
	MessageBox(NULL,text,event,MB_OK);
#endif
}

#endif