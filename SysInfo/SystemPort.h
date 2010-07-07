//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// SystemPort.h: interface for the CSystemPort class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSTEMPORT_H__FC47C324_9B26_4C73_945F_5E7761E676F9__INCLUDED_)
#define AFX_SYSTEMPORT_H__FC47C324_9B26_4C73_945F_5E7761E676F9__INCLUDED_

#pragma once

#include "SysInfoClasses.h"

#define SYSTEM_PORT_SERIAL		_T( "Serial")
#define SYSTEM_PORT_PARALLEL	_T( "Parallel")

class SYSINFO_API CSystemPort  
{
public: // Methods
	//////////////////////////////////
	// Standard contructor/destructor
	//////////////////////////////////
	CSystemPort();
	virtual ~CSystemPort();

	void Clear();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	LPCTSTR GetDescription();
	LPCTSTR GetCaption();
	LPCTSTR GetName();
	LPCTSTR GetType();

	//////////////////////////////////
	// Set attributes values
	//////////////////////////////////

	void SetType( DWORD dwType);
	void SetType( LPCTSTR lpstrType);
	void SetName( LPCTSTR lpstrName);
	void SetCaption( LPCTSTR lpstrCaption);
	void SetDescription( LPCTSTR lpstrDescription);
	void SetConnectorType( DWORD dwType);

	//////////////////////////////////
	// Comparison operators
	//////////////////////////////////

	int operator==( const CSystemPort cObject) const;

protected: // Attributes
	CString m_csType;		// Port type (serial, parallel...)
	CString m_csName;		// Port name
	CString m_csCaption;	// Caption
	CString m_csDescription;// Description
};
#endif // !defined(AFX_SYSTEMPORT_H__FC47C324_9B26_4C73_945F_5E7761E676F9__INCLUDED_)

