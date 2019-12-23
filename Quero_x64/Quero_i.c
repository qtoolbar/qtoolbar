

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Tue Nov 26 15:21:57 2013
 */
/* Compiler settings for .\Quero.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IQueroBand,0x3DBF9F47,0xB2FD,0x4A08,0xAF,0x1E,0x65,0x3F,0x55,0x51,0x21,0x7F);


MIDL_DEFINE_GUID(IID, IID_IQueroFilter,0x057CD07D,0x8F03,0x47C5,0x80,0x9B,0xDF,0x84,0x6B,0xCE,0xBB,0x68);


MIDL_DEFINE_GUID(IID, LIBID_QUEROLib,0x8A2092A7,0x55D2,0x433F,0xB7,0x00,0x4D,0x36,0x90,0x2B,0x39,0xB5);


MIDL_DEFINE_GUID(CLSID, CLSID_QueroBand,0xA411D7F4,0x8D11,0x43EF,0xBD,0xE4,0xAA,0x92,0x16,0x66,0x38,0x8A);


MIDL_DEFINE_GUID(CLSID, CLSID_QueroFilter,0x65AD9A7A,0x9E52,0x43D2,0xAA,0x3D,0x02,0xFB,0xC9,0xE5,0x35,0xB8);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



