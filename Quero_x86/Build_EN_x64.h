/*
   Quero Toolbar
   http://www.quero.at/
   Copyright 2009 Viktor Krammer

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
// Quero Build constants

#pragma once

// #define VERSION_EN
// #define VERSION_EN_WIN9X
// #define VERSION_EN_VISTA
#define VERSION_EN_X64
// #define VERSION_DE
// #define VERSION_DE_WIN9X
// #define VERSION_DE_VISTA
// #define VERSION_DE_X64

#define QUERO_BUILD 3
#define QUERO_LANGUAGE 0

// #define COMPILE_FOR_WIN9X
#define COMPILE_FOR_WINDOWS_VISTA
// #define VERSION_BELOW_VISTA

#define ENGLISH_VERSION
// #define GERMAN_VERSION

#ifndef WINVER
	#ifdef COMPILE_FOR_WINDOWS_VISTA
		#define WINVER 0x0600
		#define _WIN32_WINNT 0x0600
		#define _WIN32_IE 0x0700
	#else
		#ifdef COMPILE_FOR_WIN9X
			#define _WIN32_WINNT 0x0400
		#else
			#define _WIN32_WINNT 0x0500
		#endif
	#endif
#endif
