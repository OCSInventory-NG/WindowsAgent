//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Monitor.h: interface for the CMonitor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MONITOR_H__1838430C_16FA_4069_A2C2_4B338DF7308C__INCLUDED_)
#define AFX_MONITOR_H__1838430C_16FA_4069_A2C2_4B338DF7308C__INCLUDED_

#pragma once

#include "SysInfoClasses.h"


class SYSINFO_API CMonitor  
{
public: // Methods
	//////////////////////////////////
	// Standard constructor/destructor
	//////////////////////////////////
	CMonitor();
	virtual ~CMonitor();

	void Clear();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	LPCTSTR GetDescription();
	LPCTSTR GetCaption();
	LPCTSTR GetType();
	LPCTSTR GetManufacturer();
	LPCTSTR GetSerial();

	//////////////////////////////////
	// Set attributes values
	//////////////////////////////////

	void SetDescription( LPCTSTR lpstrDescription);
	void SetCaption( LPCTSTR lpstrCaption);
	void SetType( LPCTSTR lpstrType);
	void SetManufacturer( LPCTSTR lpstrManufacturer);
	void SetSerial( LPCTSTR lpstrSerial);
	// Is provided serial number valid (check already stored SN if null)
	BOOL IsValidSerial( LPCTSTR lpstrSN = NULL);

	//////////////////////////////////
	// Comparison operators
	//////////////////////////////////

	int operator==( const CMonitor cObject) const;

protected: // Attributes
	CString m_csManufacturer;	// Monitor manufacturer
	CString m_csType;			// Monitor type
	CString m_csCaption;		// Caption
	CString m_csDescription;	// Description
	CString m_csSerial;			// Serial
};
#endif // !defined(AFX_MONITOR_H__1838430C_16FA_4069_A2C2_4B338DF7308C__INCLUDED_)

