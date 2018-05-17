//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// PrinterInfo.h: interface for the CPrinter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRINTER_H__EB59F013_89CE_11D5_B284_0040055338AF__INCLUDED_)
#define AFX_PRINTER_H__EB59F013_89CE_11D5_B284_0040055338AF__INCLUDED_

#pragma once

#include "SysInfoClasses.h"


class SYSINFO_API CPrinter  
{
public: // Methods
	//////////////////////////////////
	// Standard contructor/destructor
	//////////////////////////////////
	CPrinter();
	virtual ~CPrinter();

	void Clear();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	// Return the printer name
	LPCTSTR GetName();
	// Return the printer driver
	LPCTSTR GetDriver();
	// Return the printer port
	LPCTSTR GetPort();
	// Return the Print server name
	LPCTSTR GetServerName();
	// Return the Share name on print server
	LPCTSTR GetShareName();
	// Return the printer Resolution
	LPCTSTR GetResolution();
	// Return the printer comment
	LPCTSTR GetComment();
	// Return the printer port
	BOOL IsShared();
	// Return if printer is on network
	BOOL IsNetworkPrinter();

	//////////////////////////////////
	// Set attributes values
	//////////////////////////////////

	void SetName( LPCTSTR lpstrName);
	void SetDriver( LPCTSTR lpstrDriver);
	void SetPort( LPCTSTR lpstrPort);
	void SetServerName( LPCTSTR lpstrServerName);
	void SetShareName( LPCTSTR lpstrShareName);
	void SetResolution( LPCTSTR lpstrResolution);
	void SetComment( LPCTSTR lpstrComment);
	void SetShared( BOOL bShared = FALSE);
	void SetNetworkPrinter( BOOL bNetwork = FALSE);

	//////////////////////////////////
	// Comparison operators
	//////////////////////////////////

	int operator==( const CPrinter cObject) const;

protected: // Attributes
	CString m_csName;		// Printer name
	CString m_csDriver;		// Printer driver
	CString m_csPort;		// Printer port
	CString	m_csServerName;	// Print server name
	CString m_csShareName;	// Share name on print server
	CString m_csResolution; // Print resolution
	CString m_csComment;	// Comment on print server
	BOOL	m_bShared;		// Is printer shared
	BOOL	m_bNetwork;		// Is network printer

};
#endif // !defined(AFX_PRINTER_H__EB59F013_89CE_11D5_B284_0040055338AF__INCLUDED_)

