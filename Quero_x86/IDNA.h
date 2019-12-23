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

/*
   Portions of the source code:

   punycode.c from RFC 3492
   http://www.nicemice.net/idn/
   Adam M. Costello
   http://www.nicemice.net/amc/
*/

// Class CIDNA Internationalizing Domain Names in Applications

#ifndef _IDNA_H
#define _IDNA_H

#include <stdlib.h>
#include <windows.h>


typedef enum punycode_status {
	punycode_success,
	punycode_bad_input,      /* Input is invalid.                       */
	punycode_big_output,     /* Output would exceed the space provided. */
	punycode_overflow        /* Input needs wider integers to process.  */
} punycode_status;

// IDNA status

#define IDNA_UNMODIFIED 0x0
#define IDNA_MODIFIED 0x1
#define IDNA_NONASCII 0x2
#define IDNA_ASCIIRULES_VIOLATED 0x4 // U+0020..U+007E
#define IDNA_DOMAIN_VIOLATES_ASCIIRULES 0x8
#define IDNA_ILLEGAL 0x10

// Address types

#define ADDRESS_NONE 0x0
#define ADDRESS_URL 0x1
#define ADDRESS_URL_WITHOUT_SCHEME 0x2
#define ADDRESS_PATH 0x4
#define ADDRESS_DRIVE 0x8
#define ADDRESS_ABOUT 0x10
#define ADDRESS_JAVASCRIPT 0x20
#define ADDRESS_RESOURCE 0x40
#define ADDRESS_PIDL 0x80 // Pointer to an Item Identifier List
#define ADDRESS_NEWSGROUP 0x100
#define ADDRESS_MAILTO 0x200
#define ADDRESS_IPV4 0x400 // IP address in dotted decimal notation
#define ADDRESS_IPV6 0x800
#define ADDRESS_PORT_PRESENT 0x1000
#define ADDRESS_FILE_URL 0x2000
#define ADDRESS_HTTPS_URL 0x4000

#define NTLD 278 // Number of top level domains, thereof are 4 cyrillic mappings and 1 IDN ccTLD

// Scheme address type mapping

typedef struct SchemeAddressType
{
	TCHAR *Scheme;
	int Length;
	UINT Type;
} SchemeAddressType;

// Port numbers

#define PORTNUMBER_OMITTED -1
#define PORTNUMBER_DEFAULT 80

// Bootstring parameters for Punycode

enum punycode_parameter {
  base = 36, tmin = 1, tmax = 26,
  skew = 38, damp = 700,
  initial_bias = 72,
  initial_n = 0x80,
  delimiter = 0x2D
};

// Constants for algorithmic Hangul Syllable Normalization
enum hangul_constant {
	SBase = 0xAC00, LBase = 0x1100, VBase = 0x1161, TBase = 0x11A7,
	LCount = 19, VCount = 21, TCount = 28,
	NCount = VCount * TCount,   // 588
    SCount = LCount * NCount,   // 11172
	SEnd = SBase + SCount - 1,
	LEnd = LBase + LCount - 1,
	VEnd = VBase + LBase - 1,
	TEnd = TBase + TCount - 1
};

// Unicode Normalization Forms

#define NF_COMPATIBILITY_MASK 1
#define NF_COMPOSITION_MASK 2

#define NFD 0
#define NFC NF_COMPOSITION_MASK
#define NFKD NF_COMPATIBILITY_MASK
#define NFKC NF_COMPATIBILITY_MASK | NF_COMPOSITION_MASK

// Unicode table and mapping structures

#define MAPPINGSIZE 4
#define MAPPINGSIZELARGE 19

typedef struct UnicodeInterval
{
	BYTE Plane;
	WORD Begin;
	WORD End;
} UnicodeInterval;

typedef struct UnicodeMapping
{
	BYTE Plane; // lobyte=Plane of From codepoint; hibyte=Plane of To codepoint(s)
	WORD From;
	WORD To[MAPPINGSIZE];
} UnicodeMapping;

typedef struct UnicodeMappingLarge
{
	BYTE Plane; // lobyte=Plane of From codepoint; hibyte=Plane of To codepoint(s)
	WORD From;
	WORD To[MAPPINGSIZELARGE];
} UnicodeMappingLarge;

typedef struct UnicodeMappingDecomp
{
	BYTE Plane; // lobyte=Plane of From codepoint; hibyte=Plane of To codepoint(s)
	WORD From;
	WORD To[2];
} UnicodeMappingDecomp;

typedef struct UnicodeMappingSingleton
{
	BYTE Plane; // lobyte=Plane of From codepoint; hibyte=Plane of To codepoint(s)
	WORD From;
	WORD To;
} UnicodeMappingSingleton;

#define NUCCHARSETS 243
#define NUCCHARSETS_NAMES NUCCHARSETS+2
#define NUCIGNORED 12
#define NUCMAPPING 1370
#define NUCPROHIBITED 41
#define NUCBIDICATL 360
#define NUCBIDICATR_AL 34
#define NUCCLASSES 191
#define NUCDECOMP 572
#define NUCCOMPOSITES 489
#define NUCSINGLETONS 1117
#define NUCCOMPAT 2617
#define NUCCOMPATLARGE 86

#define CHARSET_HYPHEN 0
#define CHARSET_DIGITS 1
#define CHARSET_LETTERS 2
#define CHARSET_LATIN1 3
#define CHARSET_NONSTDASCII NUCCHARSETS
#define CHARSET_UNKNOWN NUCCHARSETS+1
        
#define ACE_PREFIX L"xn--"
#define ACE_PREFIX_LEN 4

#define MAX_HOST_LEN  255
#define MAX_HOST_LABELS  32
#define MAX_LABEL_LEN  63
#define MAX_SCHEME_LEN 15
#define MAX_USERPASSWORD_LEN 127

#define MAXURLLENGTH 2048
#define MAX_INT_LABEL_LEN (MAX_LABEL_LEN*3) // Internal label length (normalized label length can be smaller than source label length) with terminating zero
#define MAX_DECOMPOSITION_LEN 64

#define basic(cp) ((cp) < 0x80) // basic(cp) tests whether cp is a basic code point
#define delim(cp) ((cp) == delimiter) // delim(cp) tests whether cp is a delimiter
#define flagged(bcp) ((DWORD)(bcp) - 65 < 26) // tests whether a basic code point is flagged (uppercase).  The behavior is undefined if bcp is not a basic code point
 // #define MAXDWORD ((DWORD)-1)

#define IsUnicodeDot(cp) ((cp==0x2e) || (cp==0x3002) || (cp==0xff0f) || (cp==0xff61))

#define IsUTF16BMP(cp) ((cp)<0x10000 || (cp)>0x10FFFF) // EncodeUTF16 encodes given oodepoint into the Basic Multilingual Plane (BMP)

#define UNICODE_REPLACEMENT_CHARACTER 0xFFFD

// Hex Encoding

#define toHex(x) ((x) > 9 ? (((x)-10) + _T('A')) : ((x) + _T('0')))
#define fromHex(x) ((x) >= _T('A') ? ((x) - _T('A') + 10) : ((x) - _T('0')))

class CIDNA
{
public:
	CIDNA(void);
	~CIDNA(void);

	TCHAR* EncodeHex(TCHAR *sIn,UINT CodePage);
	TCHAR* EncodeHex(TCHAR *sIn,size_t nIn,UINT CodePage);
	TCHAR* DecodeHex(TCHAR *sIn,UINT CodePage);
	TCHAR* EncodeUTF8(TCHAR *szSource);
	TCHAR* EncodeUTF8(TCHAR *szSource,size_t len);
	TCHAR* DecodeUTF8(TCHAR *szSource);
	DWORD DecodeUTF16(TCHAR **sIn);
	static TCHAR* EncodeUTF16(DWORD cp,TCHAR **sOut);

/*
 * punycode_encode() converts Unicode to Punycode.  The input
 * is represented as an array of Unicode code points (not code
 * units; surrogate pairs are not allowed), and the output
 * will be represented as an array of ASCII code points.  The
 * output string is *not* null-terminated; it will contain
 * zeros if and only if the input contains zeros.  (Of course
 * the caller can leave room for a terminator and add one if
 * needed.)  The input_length is the number of code points in
 * the input.  The output_length is an in/out argument: the
 * caller passes in the maximum number of code points that it
 * can receive, and on successful return it will contain the
 * number of code points actually output.  The case_flags array
 * holds input_length boolean values, where nonzero suggests that
 * the corresponding Unicode character be forced to uppercase
 * after being decoded (if possible), and zero suggests that
 * it be forced to lowercase (if possible).  ASCII code points
 * are encoded literally, except that ASCII letters are forced
 * to uppercase or lowercase according to the corresponding
 * uppercase flags.  If case_flags is a null pointer then ASCII
 * letters are left as they are, and other code points are
 * treated as if their uppercase flags were zero.  The return
 * value can be any of the punycode_status values defined above
 * except punycode_bad_input; if not punycode_success, then
 * output_size and output might contain garbage.
 */
	punycode_status punycode_encode (size_t input_length,
									 const DWORD *input,
									 const BYTE *case_flags,
									 size_t *output_length,
									 TCHAR *output);

/*
 * punycode_decode() converts Punycode to Unicode.  The input is
 * represented as an array of ASCII code points, and the output
 * will be represented as an array of Unicode code points.  The
 * input_length is the number of code points in the input.  The
 * output_length is an in/out argument: the caller passes in
 * the maximum number of code points that it can receive, and
 * on successful return it will contain the actual number of
 * code points output.  The case_flags array needs room for at
 * least output_length values, or it can be a null pointer if the
 * case information is not needed.  A nonzero flag suggests that
 * the corresponding Unicode character be forced to uppercase
 * by the caller (if possible), while zero suggests that it be
 * forced to lowercase (if possible).  ASCII code points are
 * output already in the proper case, but their flags will be set
 * appropriately so that applying the flags would be harmless.
 * The return value can be any of the punycode_status values
 * defined above; if not punycode_success, then output_length,
 * output, and case_flags might contain garbage.  On success, the
 * decoder will never need to write an output_length greater than
 * input_length, because of how the encoding is defined.
 */
	punycode_status punycode_decode (size_t input_length,
									 const TCHAR *input,
									 size_t *output_length,
									 DWORD *output,
									 BYTE *case_flags);

	int URLToAscii(TCHAR *url,size_t *size,int *HostStartIndex,int *HostEndIndex,int *DomainStartIndex);
	int LabelToAscii(TCHAR *label,size_t *label_len);
	int URLToUnicode(TCHAR *url,size_t *size,int *HostStartIndex,int *HostEndIndex,int *DomainStartIndex);

	UINT IsAddress(TCHAR *uri);
	int IsIPv6reference(const TCHAR *pIPv6reference);
	bool IsOctet(TCHAR *label);
	bool IsHexWord(TCHAR *label);
	bool IsInternetURL(TCHAR *url);
	bool IsTLD(TCHAR* tld);

	// Nameprep membership and mapping functions
	
	int IsElementOfUCTable(DWORD cp,const UnicodeInterval* UCTable,short n);
	int GetMapping(DWORD cp,const UnicodeMapping* UCMapping,short n);
	int GetCaseMapping(DWORD cp);
	short GetCharSet(DWORD cp);
	const TCHAR *GetCharSetName(short CharSet);
	bool IsIgnoredChar(DWORD cp);
	bool IsProhibitedChar(DWORD cp);
	bool IsBidiCatL(DWORD cp);
	bool IsBidiCatR_AL(DWORD cp);

	// Unicode Database

	BYTE GetCanonicalClass(DWORD cp);
	DWORD GetComposite(DWORD cp1,DWORD cp2);
	DWORD GetSingletonMapping(DWORD cp);
	int GetDecompMapping(DWORD cp);
	int GetCompatMapping(DWORD cp);
	int GetCompatMappingLarge(DWORD cp);

	// Unicode Normalization
	bool Decompose(bool compatability,DWORD *label,size_t *len);
	bool Compose(DWORD *label,size_t *len);
	int GetRecursiveDecomposition(bool compatability,DWORD cp,DWORD *buffer);
	bool Normalize(char form,DWORD *label,size_t *len);

	// Nameprep
	bool Nameprep(TCHAR *utf16,DWORD *codepoints,size_t *len);

	// IP address
	bool IsIPv4;
	bool IsIPv6;

	// Has scheme
	inline bool HasScheme() { return SchemeLen!=0; }

private:
	DWORD decode_digit(TCHAR cp);
	TCHAR encode_digit(DWORD d, int flag);
	TCHAR encode_basic(DWORD bcp, int flag);
	DWORD adapt(DWORD delta, DWORD numpoints, int firsttime);

	bool split_labels(const TCHAR *url,bool bNoScheme=false);
	int GetDomainStartIndex(int *RegisteredDomainLabel);

	TCHAR buffer[MAX_INT_LABEL_LEN];
	
	TCHAR Labels[MAX_HOST_LABELS][MAX_INT_LABEL_LEN];
	size_t LabelLengths[MAX_HOST_LABELS];
	TCHAR Scheme[MAX_SCHEME_LEN+4]; // Includes ://
	size_t SchemeLen;
	TCHAR UserPassword[MAX_USERPASSWORD_LEN+1];
	size_t UserPasswordLen;
	TCHAR URLPath[MAXURLLENGTH];
	UINT nLabels;
	int PortNumber;
	bool IsFQDN; // Fully Qualified Domain Name (with trailing dot)
};

#endif