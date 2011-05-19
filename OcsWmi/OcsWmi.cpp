//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// OcsWmi.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "OcsWmi.h"
#include <comdef.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma comment(lib, "wbemuuid.lib")

// COcsWmiApp

BEGIN_MESSAGE_MAP(COcsWmiApp, CWinApp)
END_MESSAGE_MAP()


// COcsWmiApp construction

COcsWmiApp::COcsWmiApp()
{
}


// The one and only COcsWmiApp object
COcsWmiApp theApp;


// COcsWmiApp initialization

BOOL COcsWmiApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}



// This is the constructor of a class that has been exported.
// see OcsWmi.h for the class definition
COcsWmi::COcsWmi()
{ 
	m_pIWbemServices = NULL;
	m_pEnumClassObject = NULL;
	m_pClassObject = NULL;
}

COcsWmi::~COcsWmi()
{
	DisconnectWMI();
}

BOOL COcsWmi::ConnectWMI( LPCTSTR lpstrNameSpace)
{
	IWbemLocator *pIWbemLocator = NULL;

	ASSERT( lpstrNameSpace);

	try
	{
		// Step 1: --------------------------------------------------
		// Initialize COM. ------------------------------------------
		if (m_pIWbemServices)
		{
			// Already connected, release used objects, but do not unitialize COM
			if (m_pClassObject)
				m_pClassObject->Release();
			m_pClassObject = NULL;

			if (m_pEnumClassObject)
				m_pEnumClassObject->Release();
			m_pEnumClassObject = NULL;

			if (m_pIWbemServices)
				m_pIWbemServices->Release();
			m_pIWbemServices = NULL;
		}
		else
		{
			// Not connected, so intialize COM
			m_pIWbemServices = NULL;
			m_pEnumClassObject = NULL;
			m_pClassObject = NULL;

			m_hResult =  CoInitializeEx( 0, COINIT_MULTITHREADED); // Initialize COM.
			if (FAILED( m_hResult))
				return FALSE;
		}

		// Step 2: --------------------------------------------------
		// Set general COM security levels --------------------------
		m_hResult =  CoInitializeSecurity(	NULL, 
										-1,                          // COM authentication
										NULL,                        // Authentication services
										NULL,                        // Reserved
										RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
										RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
										NULL,                        // Authentication info
										EOAC_NONE,                   // Additional capabilities 
										NULL                         // Reserved
										);
		if (FAILED( m_hResult))
		{
			CoUninitialize();
			return FALSE;
		}

		// Step 3: ---------------------------------------------------
		// Obtain the initial locator to WMI -------------------------
		m_hResult = CoCreateInstance( CLSID_WbemLocator, 
									NULL, 
									CLSCTX_INPROC_SERVER,
									IID_IWbemLocator,
									(LPVOID *) &pIWbemLocator);
		if (FAILED( m_hResult))
		{
			CoUninitialize();
			return FALSE;
		}

		// Step 4: -----------------------------------------------------
		// Connect to WMI given namespace through the IWbemLocator::ConnectServer method
		m_hResult = pIWbemLocator->ConnectServer( _bstr_t( lpstrNameSpace),
										 NULL,  // User name. NULL = current user
										 NULL,	// User password. NULL = current
										 NULL,	// Locale. NULL indicates current
										 NULL,	// Security flags, NULL or WBEM_FLAG_CONNECT_USE_MAX_WAIT.
										 NULL,	// Authority (e.g. Kerberos, or domain for NTLM)
										 NULL,	// Context object 
										 &m_pIWbemServices);
		// Done with pIWbemLocator. 
		pIWbemLocator->Release(); 
		pIWbemLocator = NULL;
		if (FAILED( m_hResult))
		{
			CoUninitialize();
			return FALSE;
		}

		// Step 5: --------------------------------------------------
		// Set security levels on the proxy -------------------------
		m_hResult = CoSetProxyBlanket( m_pIWbemServices,                        // Indicates the proxy to set
							   RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
							   RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
							   NULL,                        // Server principal name 
							   RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
							   RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
							   NULL,                        // client identity
							   EOAC_NONE);                  // proxy capabilities 
		if (FAILED( m_hResult))
		{
			m_pIWbemServices->Release();
			CoUninitialize();
			return FALSE;
		}
		// Connected successfully to WMI given namespace
		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		if (m_pIWbemServices)
			m_pIWbemServices->Release();
		if (pIWbemLocator)
			pIWbemLocator->Release();
		m_hResult = WBEM_E_FAILED;
		CoUninitialize();
		return FALSE;
	}
}


BOOL COcsWmi::DisconnectWMI()
{
	try
	{
		if (m_pClassObject)
			m_pClassObject->Release();
		m_pClassObject = NULL;

		if (m_pEnumClassObject)
			m_pEnumClassObject->Release();
		m_pEnumClassObject = NULL;

		if (m_pIWbemServices)
			m_pIWbemServices->Release();
		m_pIWbemServices = NULL;
		CoUninitialize();
		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		m_hResult = WBEM_E_FAILED;
		return FALSE;
	}
}


BOOL COcsWmi::BeginEnumClassObject( LPCTSTR lpstrObject)
{
	ASSERT( m_pIWbemServices);
	ASSERT( lpstrObject);

	try
	{
		// Get the object class
		CString csQuery;
		csQuery.Format( _T( "SELECT * FROM %s"), lpstrObject);
		
		if (m_pEnumClassObject)
			m_pEnumClassObject->Release();
		m_pEnumClassObject = NULL;

		// Get the list of object instances.
		m_hResult = m_pIWbemServices->ExecQuery( _bstr_t( "WQL"), 	// Query language
											_bstr_t( csQuery),		// Query
											WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
											NULL,
											&m_pEnumClassObject);	// pointer to enumerator
		if (FAILED( m_hResult))
		{
#ifdef _DEBUG
			csQuery.Format( _T( "WMI Error code 0x%x"), m_hResult);
			AfxMessageBox( csQuery);
#endif
			return FALSE;
		}
		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		m_hResult = WBEM_E_FAILED;
		return FALSE;
	}
}


BOOL COcsWmi::MoveNextEnumClassObject()
{
	ASSERT( m_pEnumClassObject);

	try
	{
		ULONG uReturned = 1;

		if (m_pClassObject)
			m_pClassObject->Release();
		m_pClassObject = NULL;

		// Enumerate through the resultset.
		m_hResult = m_pEnumClassObject->Next( 30000,			// Timeout 30 s
											  1,				// Return just one storage device
											  &m_pClassObject,	// Pointer to storage device
											  &uReturned);		// Number obtained: one or zero
		if (SUCCEEDED( m_hResult) && (uReturned == 1))
			return TRUE;
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		m_hResult = WBEM_E_FAILED;
		return FALSE;
	}
}


BOOL COcsWmi::CloseEnumClassObject()
{
	try
	{
		if (m_pClassObject)
			m_pClassObject->Release();
		m_pClassObject = NULL;

		if (m_pEnumClassObject)
			m_pEnumClassObject->Release();
		m_pEnumClassObject = NULL;
		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		m_hResult = WBEM_E_FAILED;
		return FALSE;
	}
}


LPCTSTR COcsWmi::GetClassObjectStringValue(LPCTSTR lpstrProperty)
{
	ASSERT( m_pClassObject);
	ASSERT( lpstrProperty);

	try
	{
		VARIANT pVal;
		VariantInit(&pVal);
		CIMTYPE pType;

		VariantClear(&pVal);
		m_hResult = m_pClassObject->Get( _bstr_t( lpstrProperty), 0L, &pVal, &pType, NULL);
		if (SUCCEEDED( m_hResult))
		{
			if(pType == 101)
			{
					m_csResult = strCimValue( pVal, pType);
					return m_csResult;			
			}
			else
			{
				m_csResult = strCimArrayValue( pVal, pType);
				return m_csResult;
			}
		}
		return NULL;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		m_hResult = WBEM_E_FAILED;
		return NULL;
	}
}


DWORD COcsWmi::GetClassObjectDwordValue(LPCTSTR lpstrProperty)
{
	ASSERT( m_pClassObject);
	ASSERT( lpstrProperty);

	try
	{
		VARIANT pVal;
		VariantInit(&pVal);
		CIMTYPE pType;

		VariantClear(&pVal);
		m_hResult = m_pClassObject->Get( _bstr_t( lpstrProperty), 0L, &pVal, &pType, NULL);
		if (SUCCEEDED( m_hResult))
			return dwCimValue( pVal, pType);
		return 0;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		m_hResult = WBEM_E_FAILED;
		return 0;
	}
}


__int64 COcsWmi::GetClassObjectI64Value(LPCTSTR lpstrProperty)
{
	ASSERT( m_pClassObject);
	ASSERT( lpstrProperty);

	try
	{
		VARIANT pVal;
		VariantInit(&pVal);
		CIMTYPE pType;

		VariantClear(&pVal);
		m_hResult = m_pClassObject->Get( _bstr_t( lpstrProperty), 0L, &pVal, &pType, NULL);
		if (SUCCEEDED( m_hResult))
			return i64CimValue( pVal, pType);
		return 0;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		m_hResult = WBEM_E_FAILED;
		return 0;
	}
}


unsigned __int64 COcsWmi::GetClassObjectU64Value(LPCTSTR lpstrProperty)
{
	ASSERT( m_pClassObject);
	ASSERT( lpstrProperty);

	try
	{
		VARIANT pVal;
		VariantInit(&pVal);
		CIMTYPE pType;

		VariantClear(&pVal);
		m_hResult = m_pClassObject->Get( _bstr_t( lpstrProperty), 0L, &pVal, &pType, NULL);
		if (SUCCEEDED( m_hResult))
			return u64CimValue( pVal, pType);
		return 0;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		m_hResult = WBEM_E_FAILED;
		return 0;
	}
}


LPCTSTR COcsWmi::GetRefElementClassObjectStringValue(LPCTSTR lpstrRefElement, LPCTSTR lpstrProperty)
{
	ASSERT( m_pClassObject);
	ASSERT( lpstrRefElement);
	ASSERT( lpstrProperty);

	try
	{
		CString	csObject;
		VARIANT pVal;
		VariantInit(&pVal);
		CIMTYPE pType;
		IWbemClassObject *pClassObject;

		VariantClear(&pVal);
		m_hResult = m_pClassObject->Get( _bstr_t( lpstrRefElement), 0L, &pVal, &pType, NULL);
		if (FAILED( m_hResult))
			return NULL;
		csObject = strCimValue( pVal, pType);
		if (csObject.IsEmpty())
			return NULL;
		m_hResult = m_pIWbemServices->GetObject( _bstr_t( csObject),
												WBEM_FLAG_RETURN_WBEM_COMPLETE,
												NULL,
												&pClassObject,
												NULL);
		if (FAILED( m_hResult))
		{
			pClassObject->Release();
			return NULL;
		}
		VariantClear(&pVal);
		m_hResult = pClassObject->Get( _bstr_t( lpstrProperty), 0L, &pVal, &pType, NULL);
		if (FAILED( m_hResult))
		{
			pClassObject->Release();
			return NULL;
		}
		if(pType == 101)
			m_csResult = strCimValue( pVal, pType);
		else
			m_csResult = strCimArrayValue( pVal, pType);
		pClassObject->Release();
		return m_csResult;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		m_hResult = WBEM_E_FAILED;
		return NULL;
	}
}


DWORD COcsWmi::GetRefElementClassObjectDwordValue(LPCTSTR lpstrRefElement, LPCTSTR lpstrProperty)
{
	ASSERT( m_pClassObject);
	ASSERT( lpstrRefElement);
	ASSERT( lpstrProperty);

	try
	{
		CString	csObject;
		VARIANT pVal;
		VariantInit(&pVal);
		CIMTYPE pType;
		IWbemClassObject *pClassObject;
		static DWORD dwResult;

		VariantClear(&pVal);
		m_hResult = m_pClassObject->Get( _bstr_t( lpstrRefElement), 0L, &pVal, &pType, NULL);
		if (FAILED( m_hResult))
			return 0;
		csObject = strCimValue( pVal, pType);
		if (csObject.IsEmpty())
			return 0;
		m_hResult = m_pIWbemServices->GetObject( _bstr_t( csObject),
												WBEM_FLAG_RETURN_WBEM_COMPLETE,
												NULL,
												&pClassObject,
												NULL);
		if (FAILED( m_hResult))
		{
			pClassObject->Release();
			return 0;
		}
		VariantClear(&pVal);
		m_hResult = pClassObject->Get( _bstr_t( lpstrProperty), 0L, &pVal, &pType, NULL);
		if (FAILED( m_hResult))
			dwResult = 0;
		else
			dwResult = dwCimValue( pVal, pType);
		pClassObject->Release();
		return dwResult;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		m_hResult = WBEM_E_FAILED;
		return 0;
	}
}


__int64 COcsWmi::GetRefElementClassObjectI64Value(LPCTSTR lpstrRefElement, LPCTSTR lpstrProperty)
{
	ASSERT( m_pClassObject);
	ASSERT( lpstrRefElement);
	ASSERT( lpstrProperty);

	try
	{
		CString	csObject;
		VARIANT pVal;
		VariantInit(&pVal);
		CIMTYPE pType;
		IWbemClassObject *pClassObject;
		static __int64 i64Result;

		VariantClear(&pVal);
		m_hResult = m_pClassObject->Get( _bstr_t( lpstrRefElement), 0L, &pVal, &pType, NULL);
		if (FAILED( m_hResult))
			return 0;
		csObject = strCimValue( pVal, pType);
		if (csObject.IsEmpty())
			return 0;
		m_hResult = m_pIWbemServices->GetObject( _bstr_t( csObject),
												WBEM_FLAG_RETURN_WBEM_COMPLETE,
												NULL,
												&pClassObject,
												NULL);
		if (FAILED( m_hResult))
		{
			pClassObject->Release();
			return 0;
		}
		VariantClear(&pVal);
		m_hResult = pClassObject->Get( _bstr_t( lpstrProperty), 0L, &pVal, &pType, NULL);
		if (FAILED( m_hResult))
			i64Result = 0;
		else
			i64Result = i64CimValue( pVal, pType);
		pClassObject->Release();
		return i64Result;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		m_hResult = WBEM_E_FAILED;
		return 0;
	}
}


unsigned __int64 COcsWmi::GetRefElementClassObjectU64Value(LPCTSTR lpstrRefElement, LPCTSTR lpstrProperty)
{
	ASSERT( m_pClassObject);
	ASSERT( lpstrRefElement);
	ASSERT( lpstrProperty);
	try
	{
		CString	csObject;
		VARIANT pVal;
		VariantInit(&pVal);
		CIMTYPE pType;
		IWbemClassObject *pClassObject;
		static unsigned __int64 u64Result;


		VariantClear(&pVal);
		m_hResult = m_pClassObject->Get( _bstr_t( lpstrRefElement), 0L, &pVal, &pType, NULL);
		if (FAILED( m_hResult))
			return 0;
		csObject = strCimValue( pVal, pType);
		if (csObject.IsEmpty())
			return 0;
		m_hResult = m_pIWbemServices->GetObject( _bstr_t( csObject),
												WBEM_FLAG_RETURN_WBEM_COMPLETE,
												NULL,
												&pClassObject,
												NULL);
		if (FAILED( m_hResult))
		{
			pClassObject->Release();
			return 0;
		}
		VariantClear(&pVal);
		m_hResult = pClassObject->Get( _bstr_t( lpstrProperty), 0L, &pVal, &pType, NULL);
		if (FAILED( m_hResult))
			u64Result = 0;
		else
			u64Result = u64CimValue( pVal, pType);
		pClassObject->Release();
		return u64Result;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		m_hResult = WBEM_E_FAILED;
		return 0;
	}
}


HRESULT COcsWmi::GetLastErrorWMI()
{
	return m_hResult;
}


LPCTSTR COcsWmi::strCimValue(VARIANT &pVal, CIMTYPE &pType)
{
	COleDateTime pOleDate;
	static CString csResult;

	if ((pVal.vt == VT_NULL) || (pVal.vt == VT_EMPTY))
		csResult.Empty();
	else
		switch (pType)
		{
		case CIM_ILLEGAL:
			csResult = _T( "CIM_ILLEGAL");
			break;
		case CIM_EMPTY:
			csResult = _T( "");
			break;
		case CIM_SINT8:
			csResult.Format(_T( "%hd"),V_I1(&pVal));
			break;
		case CIM_UINT8:
			csResult.Format(_T( "%hu"),V_UI1(&pVal));
			break;
		case CIM_SINT16:
			csResult.Format(_T( "%d"),V_I2(&pVal));
			break;
		case CIM_UINT16:
			csResult.Format(_T( "%u"),V_UI2(&pVal));
			break;
		case CIM_SINT32:
			csResult.Format(_T( "%ld"),V_I4(&pVal));
			break;
		case CIM_UINT32:
			csResult.Format(_T( "%lu"),V_UI4(&pVal));
			break;
		case CIM_SINT64:
			csResult = V_BSTR(&pVal);
			break;
		case CIM_UINT64:
			csResult = V_BSTR(&pVal);
			break;
		case CIM_REAL32:
			csResult.Format(_T( "%e"),V_R4(&pVal));
			break;
		case CIM_REAL64:
			csResult.Format(_T( "%le"),V_R8(&pVal));
			break;
		case CIM_BOOLEAN:
			csResult = (V_BOOL(&pVal) ? _T("TRUE") : _T("FALSE"));
			break;
		case CIM_STRING:
			csResult = V_BSTR(&pVal);
			break;
		case CIM_DATETIME:
			pOleDate = COleDateTime( pVal);
			if (pOleDate.GetStatus() == COleDateTime::valid)
				csResult = pOleDate.Format( VAR_DATEVALUEONLY);
			else
				csResult = V_BSTR( &pVal);
			break;
		case CIM_REFERENCE:
			csResult = V_BSTR( &pVal);
			break;
		case CIM_CHAR16:
			csResult = V_BSTR(&pVal);
			break;
		case CIM_OBJECT:
			csResult = _T( "CIM_OBJECT");
			break;
		default:
			csResult = strVariantArray( pVal);
			break;
		}
	return csResult;
}


DWORD COcsWmi::dwCimValue(VARIANT &pVal, CIMTYPE &pType)
{
	if ((pVal.vt == VT_NULL) || (pVal.vt == VT_EMPTY))
		return 0;
	else
		switch (pType)
		{
		case CIM_SINT8:
			return V_I1(&pVal);
		case CIM_UINT8:
			return V_UI1(&pVal);
		case CIM_SINT16:
			return V_I2(&pVal);
		case CIM_UINT16:
			return V_UI2(&pVal);
		case CIM_SINT32:
			return V_I4(&pVal);
		case CIM_UINT32:
			return V_UI4(&pVal);
		case CIM_BOOLEAN:
			return (V_BOOL(&pVal) ? 1 : 0);
		default:
			return 0;
		}
}


__int64 COcsWmi::i64CimValue(VARIANT &pVal, CIMTYPE &pType)
{
	CString csResult;

	if ((pVal.vt == VT_NULL) || (pVal.vt == VT_EMPTY))
		return 0;
	else
		switch (pType)
		{
		case CIM_SINT8:
			return V_I1(&pVal);
		case CIM_UINT8:
			return V_UI1(&pVal);
		case CIM_SINT16:
			return V_I2(&pVal);
		case CIM_UINT16:
			return V_UI2(&pVal);
		case CIM_SINT32:
			return V_I4(&pVal);
		case CIM_UINT32:
			return V_UI4(&pVal);
		case CIM_SINT64:
			csResult = V_BSTR(&pVal);
			return _ttoi64( csResult);
		case CIM_UINT64:
			csResult = V_BSTR(&pVal);
			return _ttoi64( csResult);
		case CIM_BOOLEAN:
			return (V_BOOL(&pVal) ? 1 : 0);
		default:
			return 0;
		}
}


unsigned __int64 COcsWmi::u64CimValue(VARIANT &pVal, CIMTYPE &pType)
{
	CString csResult;

	if ((pVal.vt == VT_NULL) || (pVal.vt == VT_EMPTY))
		return 0;
	else
		switch (pType)
		{
		case CIM_SINT8:
			return V_I1(&pVal);
		case CIM_UINT8:
			return V_UI1(&pVal);
		case CIM_SINT16:
			return V_I2(&pVal);
		case CIM_UINT16:
			return V_UI2(&pVal);
		case CIM_SINT32:
			return V_I4(&pVal);
		case CIM_UINT32:
			return V_UI4(&pVal);
		case CIM_SINT64:
			csResult = V_BSTR(&pVal);
			return _ttoi64( csResult);
		case CIM_UINT64:
			csResult = V_BSTR(&pVal);
			return _ttoi64( csResult);
		case CIM_BOOLEAN:
			return (V_BOOL(&pVal) ? 1 : 0);
		default:
			return 0;
		}
}


LPCTSTR COcsWmi::strCimArrayValue(VARIANT &pVal, CIMTYPE &pType)
{
    LONG dwSLBound = 0;
    LONG dwSUBound = 0;
    VARIANT v;
	VariantInit(&v);
    LONG i;
	static CString strRet;
	HRESULT hr;
	CIMTYPE cimTypeWithoutArray;

    if(!V_ISARRAY(&pVal))
        return strCimValue( pVal, pType);

    //
    // Check that there is only one dimension in this array
    //

    if ((V_ARRAY(&pVal))->cDims != 1)
		return NULL;

	//
    // Check that there is atleast one element in this array
    //

    if ((V_ARRAY(&pVal))->rgsabound[0].cElements == 0)
		return NULL;

    //
    // We know that this is a valid single dimension array
    //

    hr = SafeArrayGetLBound( V_ARRAY(&pVal), 1, (long FAR *)&dwSLBound);
	if (FAILED( hr))
		return NULL;

    hr = SafeArrayGetUBound( V_ARRAY(&pVal), 1, (long FAR *)&dwSUBound);
    if (FAILED( hr))
		return NULL;

	// Calculate CIM type without the array flag
	cimTypeWithoutArray = pType ^ CIM_FLAG_ARRAY;

	// Parse the array
	strRet = _T( "");
    //VariantInit(&v);
/*    for (i = dwSLBound; i <= dwSUBound; i++)
	{
        hr = SafeArrayGetElement( V_ARRAY(&pVal), (long FAR *)&i, &v);
        if (FAILED(hr))
            continue;

        if (i < dwSUBound)
		{
			strRet += strCimValue( v, cimTypeWithoutArray);
			strRet += _T( ";");
		}
        else
		{
			strRet += strCimValue( v, cimTypeWithoutArray);
        }
        VariantClear(&v);
    }
*/
	v.vt=(unsigned short)cimTypeWithoutArray;
	DECIMAL_SETZERO(v.decVal);
	for (i = dwSLBound; i <= dwSUBound; i++)
	{
		  hr = SafeArrayGetElement( V_ARRAY(&pVal), (long FAR *)&i, &v.lVal);
		  if (FAILED(hr))
				continue;
		  strRet += strCimValue( v, cimTypeWithoutArray);
		  if (i < dwSUBound)
		  {
				strRet += _T( ";");
		  }
		  DECIMAL_SETZERO(v.decVal);
	}
    return(strRet);
}


LPCTSTR COcsWmi::strVariant( VARIANT pVal)
{
	static CString strRet;

	strRet = _T( "");
	switch(pVal.vt)
	{
		case VT_EMPTY:
		case VT_NULL:
			strRet = _T( "");
			break;
		case VT_I1:
			strRet.Format(_T( "%hd"),V_I2(&pVal));
			break;
		case VT_I2:
			strRet.Format(_T( "%d"),V_I2(&pVal));
			break;
		case VT_I4:
			strRet.Format(_T( "%ld"),V_I4(&pVal));
			break;
		case VT_I8:
			strRet.Format(_T( "%I64d"), V_I8(&pVal));
			break; 
		case VT_UI1:
			strRet.Format(_T( "%hu"),V_UI1(&pVal));
			break;
		case VT_UI2:
			strRet.Format(_T( "%u"),V_UI2(&pVal));
			break;
		case VT_UI4:
			strRet.Format(_T( "%lu"),V_UI4(&pVal));
			break;
		case VT_UI8:
			strRet.Format(_T( "%I64u"),V_UI8(&pVal));
			break;
		case VT_INT:
			strRet.Format(_T( "%d"),V_INT(&pVal));
			break;
		case VT_UINT:
			strRet.Format(_T( "%u"),V_UINT(&pVal));
			break;
		case VT_R4:
			strRet.Format(_T( "%e"),V_R4(&pVal));
			break;
		case VT_R8:
			strRet.Format(_T( "%le"),V_R8(&pVal));
			break;
		case VT_CY:
			strRet = COleCurrency(pVal).Format();
			break;
		case VT_DATE:
			strRet = COleDateTime(pVal).Format( VAR_DATEVALUEONLY);
			break;
		case VT_BSTR:
			strRet = V_BSTRT(&pVal);
			break;
		case VT_DISPATCH:
			strRet = _T( "VT_DISPATCH");
			break;
		case VT_ERROR:
			strRet = _T( "VT_ERROR");
			break;
		case VT_BOOL:
			return (V_BOOL(&pVal) ? _T("TRUE") : _T("FALSE"));
		case VT_VARIANT:
			strRet = _T( "VT_VARIANT");
			break;
		case VT_UNKNOWN:
			strRet = _T( "VT_UNKNOWN");
			break;
		case VT_VOID:
			strRet = _T( "VT_VOID");
			break;
		case VT_HRESULT:
			strRet = _T( "VT_HRESULT");
			break;
		case VT_PTR:
			strRet = _T( "VT_PTR");
			break;
		case VT_SAFEARRAY:
			strRet = _T( "VT_SAFEARRAY");
			break;
		case VT_CARRAY:
			strRet = _T( "VT_CARRAY");
			break;
		case VT_USERDEFINED:
			strRet = _T( "VT_USERDEFINED");
			break;
		case VT_LPSTR:
			strRet = _T( "VT_LPSTR");
			break;
		case VT_LPWSTR:
			strRet = _T( "VT_LPWSTR");
			break;
		case VT_FILETIME:
			strRet = _T( "VT_FILETIME");
			break;
		case VT_BLOB:
			strRet = _T( "VT_BLOB");
			break;
		case VT_STREAM:
			strRet = _T( "VT_STREAM");
			break;
		case VT_STORAGE:
			strRet = _T( "VT_STORAGE");
			break;
		case VT_STREAMED_OBJECT:
			strRet = _T( "VT_STREAMED_OBJECT");
			break;
		case VT_STORED_OBJECT:
			strRet = _T( "VT_STORED_OBJECT");
			break;
		case VT_BLOB_OBJECT:
			strRet = _T( "VT_BLOB_OBJECT");
			break;
		case VT_CF:
			strRet = _T( "VT_CF");
			break;
		case VT_CLSID:
			strRet = _T( "VT_CLSID");
			break;
	}
	return strRet;
}


LPCTSTR COcsWmi::strVariantArray( VARIANT var)
{
    LONG dwSLBound = 0;
    LONG dwSUBound = 0;
    VARIANT v;
	VariantInit(&v);
    LONG i;
	static CString strRet;
	HRESULT hr;

    if(!V_ISARRAY(&var))
        return strVariant( var);

    //
    // Check that there is only one dimension in this array
    //

    if ((V_ARRAY(&var))->cDims != 1)
		return NULL;

	//
    // Check that there is atleast one element in this array
    //

    if ((V_ARRAY(&var))->rgsabound[0].cElements == 0)
		return NULL;

    //
    // We know that this is a valid single dimension array
    //

    hr = SafeArrayGetLBound( V_ARRAY(&var), 1, (long FAR *)&dwSLBound);
	if (FAILED( hr))
		return NULL;

    hr = SafeArrayGetUBound( V_ARRAY(&var), 1, (long FAR *)&dwSUBound);
    if (FAILED( hr))
		return NULL;

	strRet = _T( "");
    // VariantInit(&v);
	DECIMAL_SETZERO(v.decVal);
    for (i = dwSLBound; i <= dwSUBound; i++)
	{
        hr = SafeArrayGetElement( V_ARRAY(&var), (long FAR *)&i, &v);
        if (FAILED(hr))
            continue;

        if (i < dwSUBound)
		{
			strRet += strVariant( v);
			strRet += _T( ";");
		}
        else
		{
			strRet += strVariant( v);
        }
        // VariantClear(&v);
		DECIMAL_SETZERO(v.decVal);
    }
    return(strRet);
}
