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
// QDebug.h: interface for the CQDebug class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QDEBUG_H__8311869E_0828_4FDC_9C7F_1EBADEFD3C31__INCLUDED_)
#define AFX_QDEBUG_H__8311869E_0828_4FDC_9C7F_1EBADEFD3C31__INCLUDED_

// enable/disable debug output
//#define QDEBUG

//#define QDEBUG_MESSAGEBOX
#define QDEBUG_CONSOLE
//#define QDEBUG_LOGFILE

#if defined QDEBUG

#define QDEBUG_CODE
#define QDEBUG_PRINT QueroDebug.print
#define QDEBUG_PRINTA QueroDebug.printA
#define QDEBUG_PRINTF QueroDebug.printf
#define QD(x) QueroDebug.print(x,L"")

class CQDebug
{
private:
	TCHAR text[16384];

	HANDLE hConsole;
	HANDLE hLogFile;

public:
	CQDebug();
	~CQDebug();

	void write_timestamp();
	void write_string(TCHAR *pString);
	void write_newline();

	void printf(TCHAR *event,TCHAR *fmt, ...);
	void print(TCHAR *event,TCHAR *details);
	void printA(TCHAR *event,CHAR *details);

};

extern class CQDebug QueroDebug;

#else
// Debugging output turned off

#define QDEBUG_CODE //
#define QDEBUG_PRINT //
#define QDEBUG_PRINTA //
#define QDEBUG_PRINTF //
#define QD(x) //

#endif

#endif // !defined(AFX_QDEBUG_H__8311869E_0828_4FDC_9C7F_1EBADEFD3C31__INCLUDED_)