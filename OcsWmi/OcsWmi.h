//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// OcsWmi.h : main header file for the OcsWmi DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#include <wbemcli.h>        // WMI interface declarations

#ifdef OCSWMI_EXPORTS
#define OCSWMI_API __declspec(dllexport)
#else
#define OCSWMI_API __declspec(dllimport)
#endif

// COcsWmiApp
// See OcsWmi.cpp for the implementation of this class
//

class COcsWmiApp : public CWinApp
{
public:
	COcsWmiApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};


// This class is exported from the OcsWmi.dll
class OCSWMI_API COcsWmi
{
public:
	COcsWmi();
	virtual ~COcsWmi();

	// Connect/disconnect to WMI namespace
	BOOL ConnectWMI( LPCTSTR lpstrNameSpace);
	BOOL DisconnectWMI();

	// Enumerate objects from WMI (excuting WQL query "SELECT * FROM lpstrObject WHERE lpstrCondition")
	BOOL BeginEnumClassObject( LPCTSTR lpstrObject, LPCTSTR lpstrCondition = NULL);
	BOOL MoveNextEnumClassObject();
	BOOL CloseEnumClassObject();

	// Get Length of data in object
	INT_PTR GetClassObjectLength( LPCTSTR lpstrProperty);
	INT_PTR GetRefElementClassObjectLength( LPCTSTR lpstrRefElement, LPCTSTR lpstrProperty);

	// Get current enumerated object properties (or properties of an objet referenced by the current enumerated object)
	LPCTSTR GetClassObjectStringValue( LPCTSTR lpstrProperty);
	LPCTSTR GetRefElementClassObjectStringValue( LPCTSTR lpstrRefElement, LPCTSTR lpstrProperty);

	DWORD GetClassObjectDwordValue( LPCTSTR lpstrProperty);
	DWORD GetRefElementClassObjectDwordValue( LPCTSTR lpstrRefElement, LPCTSTR lpstrProperty);

	__int64 GetClassObjectI64Value( LPCTSTR lpstrProperty);
	__int64 GetRefElementClassObjectI64Value( LPCTSTR lpstrRefElement, LPCTSTR lpstrProperty);
	
	unsigned __int64 GetClassObjectU64Value( LPCTSTR lpstrProperty);
	unsigned __int64 GetRefElementClassObjectU64Value( LPCTSTR lpstrRefElement, LPCTSTR lpstrProperty);

	BOOL GetClassObjectVariantValue( LPCTSTR lpstrProperty, VARIANT &pVal);
	BOOL GetRefElementClassObjectVariantValue( LPCTSTR lpstrRefElement, LPCTSTR lpstrProperty, VARIANT &pVal);

	HRESULT GetLastErrorWMI();

protected: // Methods
	LPCTSTR strVariant( VARIANT pVal);
	LPCTSTR strVariantArray( VARIANT pVal);
	LPCTSTR strCimArrayValue( VARIANT &pVal, CIMTYPE &pType);
	LPCTSTR strCimValue( VARIANT &pVal, CIMTYPE &pType);
	DWORD dwCimValue( VARIANT &pVal, CIMTYPE &pType);
	__int64 i64CimValue( VARIANT &pVal, CIMTYPE &pType);
	unsigned __int64 u64CimValue( VARIANT &pVal, CIMTYPE &pType);

protected: // Attributes
	IWbemServices *m_pIWbemServices;
	IEnumWbemClassObject *m_pEnumClassObject;
	IWbemClassObject *m_pClassObject;
	HRESULT m_hResult;
	CString m_csResult;
};

