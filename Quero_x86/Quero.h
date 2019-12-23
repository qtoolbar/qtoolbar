/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sat Dec 14 22:46:14 2013
 */
/* Compiler settings for D:\Data\quero\Quero_xp\Quero.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
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

#ifndef __Quero_h__
#define __Quero_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IQueroBand_FWD_DEFINED__
#define __IQueroBand_FWD_DEFINED__
typedef interface IQueroBand IQueroBand;
#endif 	/* __IQueroBand_FWD_DEFINED__ */


#ifndef __IQueroFilter_FWD_DEFINED__
#define __IQueroFilter_FWD_DEFINED__
typedef interface IQueroFilter IQueroFilter;
#endif 	/* __IQueroFilter_FWD_DEFINED__ */


#ifndef __QueroBand_FWD_DEFINED__
#define __QueroBand_FWD_DEFINED__

#ifdef __cplusplus
typedef class QueroBand QueroBand;
#else
typedef struct QueroBand QueroBand;
#endif /* __cplusplus */

#endif 	/* __QueroBand_FWD_DEFINED__ */


#ifndef __QueroFilter_FWD_DEFINED__
#define __QueroFilter_FWD_DEFINED__

#ifdef __cplusplus
typedef class QueroFilter QueroFilter;
#else
typedef struct QueroFilter QueroFilter;
#endif /* __cplusplus */

#endif 	/* __QueroFilter_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IQueroBand_INTERFACE_DEFINED__
#define __IQueroBand_INTERFACE_DEFINED__

/* interface IQueroBand */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IQueroBand;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3DBF9F47-B2FD-4A08-AF1E-653F5551217F")
    IQueroBand : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IQueroBandVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IQueroBand __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IQueroBand __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IQueroBand __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IQueroBand __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IQueroBand __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IQueroBand __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IQueroBand __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } IQueroBandVtbl;

    interface IQueroBand
    {
        CONST_VTBL struct IQueroBandVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IQueroBand_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IQueroBand_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IQueroBand_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IQueroBand_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IQueroBand_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IQueroBand_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IQueroBand_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IQueroBand_INTERFACE_DEFINED__ */


#ifndef __IQueroFilter_INTERFACE_DEFINED__
#define __IQueroFilter_INTERFACE_DEFINED__

/* interface IQueroFilter */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IQueroFilter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("057CD07D-8F03-47C5-809B-DF846BCEBB68")
    IQueroFilter : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IQueroFilterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IQueroFilter __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IQueroFilter __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IQueroFilter __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IQueroFilter __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IQueroFilter __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IQueroFilter __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IQueroFilter __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } IQueroFilterVtbl;

    interface IQueroFilter
    {
        CONST_VTBL struct IQueroFilterVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IQueroFilter_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IQueroFilter_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IQueroFilter_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IQueroFilter_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IQueroFilter_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IQueroFilter_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IQueroFilter_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IQueroFilter_INTERFACE_DEFINED__ */



#ifndef __QUEROLib_LIBRARY_DEFINED__
#define __QUEROLib_LIBRARY_DEFINED__

/* library QUEROLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_QUEROLib;

EXTERN_C const CLSID CLSID_QueroBand;

#ifdef __cplusplus

class DECLSPEC_UUID("A411D7F4-8D11-43EF-BDE4-AA921666388A")
QueroBand;
#endif

EXTERN_C const CLSID CLSID_QueroFilter;

#ifdef __cplusplus

class DECLSPEC_UUID("65AD9A7A-9E52-43D2-AA3D-02FBC9E535B8")
QueroFilter;
#endif
#endif /* __QUEROLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
