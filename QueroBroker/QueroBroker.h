

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Sat Dec 14 23:02:55 2013
 */
/* Compiler settings for .\QueroBroker.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __QueroBroker_h__
#define __QueroBroker_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IQueroBroker_FWD_DEFINED__
#define __IQueroBroker_FWD_DEFINED__
typedef interface IQueroBroker IQueroBroker;
#endif 	/* __IQueroBroker_FWD_DEFINED__ */


#ifndef __QueroBroker_FWD_DEFINED__
#define __QueroBroker_FWD_DEFINED__

#ifdef __cplusplus
typedef class QueroBroker QueroBroker;
#else
typedef struct QueroBroker QueroBroker;
#endif /* __cplusplus */

#endif 	/* __QueroBroker_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IQueroBroker_INTERFACE_DEFINED__
#define __IQueroBroker_INTERFACE_DEFINED__

/* interface IQueroBroker */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IQueroBroker;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("81281C00-D843-45EA-B00B-FA4C5D177AED")
    IQueroBroker : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Hook_IEFrame( 
            /* [in] */ LONG hwnd_IEFrame,
            LONG hwnd_QToolbar,
            /* [in] */ UINT Options,
            /* [in] */ UINT Options2,
            /* [in] */ INT IE_MajorVersion) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Unhook_IEFrame( 
            /* [in] */ LONG hwnd_IEFrame) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetOptions( 
            /* [in] */ UINT Options,
            /* [in] */ UINT Options2,
            /* [in] */ UINT UpdateQueroInstances) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetDwmFrameTopMargin( 
            /* [in] */ LONG hwnd_IEFrame,
            /* [in] */ INT NewDwmFrameTopMargin) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDwmFrameTopMargin( 
            /* [in] */ LONG hwnd_IEFrame) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTitle( 
            /* [in] */ LONG hwnd_IEFrame,
            /* [in] */ BSTR bstrTitle) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetIcon( 
            /* [in] */ LONG hwnd_IEFrame,
            /* [in] */ LONG hIcon) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PostMessageToIE( 
            /* [in] */ LONG hwnd_IEFrame,
            /* [in] */ UINT uMsg,
            /* [in] */ WPARAM wParam,
            /* [in] */ LPARAM lParam) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsTabbedBrowsing( 
            /* [in] */ LONG hwnd_IEFrame) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetWindowPos( 
            /* [in] */ LONG hwnd_IEFrame,
            /* [in] */ LONG hwnd_InsertAfter,
            /* [in] */ INT x,
            /* [in] */ INT y,
            /* [in] */ INT cx,
            /* [in] */ INT cy,
            /* [in] */ UINT uFlags) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RegWrite_DWORD( 
            /* [in] */ LONG hwnd_IEFrame,
            /* [in] */ UINT ValueId,
            /* [in] */ DWORD dwValue,
            /* [in] */ BOOL bDeleteValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RegRead_DWORD( 
            /* [in] */ UINT ValueId,
            /* [out] */ DWORD *pValue) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IQueroBrokerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IQueroBroker * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IQueroBroker * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IQueroBroker * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IQueroBroker * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IQueroBroker * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IQueroBroker * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IQueroBroker * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Hook_IEFrame )( 
            IQueroBroker * This,
            /* [in] */ LONG hwnd_IEFrame,
            LONG hwnd_QToolbar,
            /* [in] */ UINT Options,
            /* [in] */ UINT Options2,
            /* [in] */ INT IE_MajorVersion);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Unhook_IEFrame )( 
            IQueroBroker * This,
            /* [in] */ LONG hwnd_IEFrame);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetOptions )( 
            IQueroBroker * This,
            /* [in] */ UINT Options,
            /* [in] */ UINT Options2,
            /* [in] */ UINT UpdateQueroInstances);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetDwmFrameTopMargin )( 
            IQueroBroker * This,
            /* [in] */ LONG hwnd_IEFrame,
            /* [in] */ INT NewDwmFrameTopMargin);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDwmFrameTopMargin )( 
            IQueroBroker * This,
            /* [in] */ LONG hwnd_IEFrame);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTitle )( 
            IQueroBroker * This,
            /* [in] */ LONG hwnd_IEFrame,
            /* [in] */ BSTR bstrTitle);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetIcon )( 
            IQueroBroker * This,
            /* [in] */ LONG hwnd_IEFrame,
            /* [in] */ LONG hIcon);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PostMessageToIE )( 
            IQueroBroker * This,
            /* [in] */ LONG hwnd_IEFrame,
            /* [in] */ UINT uMsg,
            /* [in] */ WPARAM wParam,
            /* [in] */ LPARAM lParam);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsTabbedBrowsing )( 
            IQueroBroker * This,
            /* [in] */ LONG hwnd_IEFrame);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetWindowPos )( 
            IQueroBroker * This,
            /* [in] */ LONG hwnd_IEFrame,
            /* [in] */ LONG hwnd_InsertAfter,
            /* [in] */ INT x,
            /* [in] */ INT y,
            /* [in] */ INT cx,
            /* [in] */ INT cy,
            /* [in] */ UINT uFlags);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RegWrite_DWORD )( 
            IQueroBroker * This,
            /* [in] */ LONG hwnd_IEFrame,
            /* [in] */ UINT ValueId,
            /* [in] */ DWORD dwValue,
            /* [in] */ BOOL bDeleteValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RegRead_DWORD )( 
            IQueroBroker * This,
            /* [in] */ UINT ValueId,
            /* [out] */ DWORD *pValue);
        
        END_INTERFACE
    } IQueroBrokerVtbl;

    interface IQueroBroker
    {
        CONST_VTBL struct IQueroBrokerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IQueroBroker_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IQueroBroker_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IQueroBroker_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IQueroBroker_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IQueroBroker_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IQueroBroker_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IQueroBroker_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IQueroBroker_Hook_IEFrame(This,hwnd_IEFrame,hwnd_QToolbar,Options,Options2,IE_MajorVersion)	\
    ( (This)->lpVtbl -> Hook_IEFrame(This,hwnd_IEFrame,hwnd_QToolbar,Options,Options2,IE_MajorVersion) ) 

#define IQueroBroker_Unhook_IEFrame(This,hwnd_IEFrame)	\
    ( (This)->lpVtbl -> Unhook_IEFrame(This,hwnd_IEFrame) ) 

#define IQueroBroker_SetOptions(This,Options,Options2,UpdateQueroInstances)	\
    ( (This)->lpVtbl -> SetOptions(This,Options,Options2,UpdateQueroInstances) ) 

#define IQueroBroker_SetDwmFrameTopMargin(This,hwnd_IEFrame,NewDwmFrameTopMargin)	\
    ( (This)->lpVtbl -> SetDwmFrameTopMargin(This,hwnd_IEFrame,NewDwmFrameTopMargin) ) 

#define IQueroBroker_GetDwmFrameTopMargin(This,hwnd_IEFrame)	\
    ( (This)->lpVtbl -> GetDwmFrameTopMargin(This,hwnd_IEFrame) ) 

#define IQueroBroker_SetTitle(This,hwnd_IEFrame,bstrTitle)	\
    ( (This)->lpVtbl -> SetTitle(This,hwnd_IEFrame,bstrTitle) ) 

#define IQueroBroker_SetIcon(This,hwnd_IEFrame,hIcon)	\
    ( (This)->lpVtbl -> SetIcon(This,hwnd_IEFrame,hIcon) ) 

#define IQueroBroker_PostMessageToIE(This,hwnd_IEFrame,uMsg,wParam,lParam)	\
    ( (This)->lpVtbl -> PostMessageToIE(This,hwnd_IEFrame,uMsg,wParam,lParam) ) 

#define IQueroBroker_IsTabbedBrowsing(This,hwnd_IEFrame)	\
    ( (This)->lpVtbl -> IsTabbedBrowsing(This,hwnd_IEFrame) ) 

#define IQueroBroker_SetWindowPos(This,hwnd_IEFrame,hwnd_InsertAfter,x,y,cx,cy,uFlags)	\
    ( (This)->lpVtbl -> SetWindowPos(This,hwnd_IEFrame,hwnd_InsertAfter,x,y,cx,cy,uFlags) ) 

#define IQueroBroker_RegWrite_DWORD(This,hwnd_IEFrame,ValueId,dwValue,bDeleteValue)	\
    ( (This)->lpVtbl -> RegWrite_DWORD(This,hwnd_IEFrame,ValueId,dwValue,bDeleteValue) ) 

#define IQueroBroker_RegRead_DWORD(This,ValueId,pValue)	\
    ( (This)->lpVtbl -> RegRead_DWORD(This,ValueId,pValue) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IQueroBroker_INTERFACE_DEFINED__ */



#ifndef __QueroBrokerLib_LIBRARY_DEFINED__
#define __QueroBrokerLib_LIBRARY_DEFINED__

/* library QueroBrokerLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_QueroBrokerLib;

EXTERN_C const CLSID CLSID_QueroBroker;

#ifdef __cplusplus

class DECLSPEC_UUID("5C97B747-C367-4BDF-AE2A-C1AB8EAED74B")
QueroBroker;
#endif
#endif /* __QueroBrokerLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


