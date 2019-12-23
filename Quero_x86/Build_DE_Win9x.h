// Quero Build constants

#pragma once

// #define VERSION_EN
// #define VERSION_EN_WIN9X
// #define VERSION_EN_VISTA
// #define VERSION_EN_X64
// #define VERSION_DE
#define VERSION_DE_WIN9X
// #define VERSION_DE_VISTA
// #define VERSION_DE_X64

#define QUERO_BUILD 1
#define QUERO_LANGUAGE 1

#define COMPILE_FOR_WIN9X
// #define COMPILE_FOR_WINDOWS_VISTA
#define VERSION_BELOW_VISTA

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
