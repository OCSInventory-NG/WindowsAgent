//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// AccessLogInfo.h: interface for the CAccessLogInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACCESSLOG_H__3A200601_4875_11D5_B231_0040055338AF__INCLUDED_)
#define AFX_ACCESSLOG_H__3A200601_4875_11D5_B231_0040055338AF__INCLUDED_

#pragma once

#include "SysInfoClasses.h"


class SYSINFO_API CAccessLog  
{
public: // Methods
	//////////////////////////////////
	// Standard constructor/destructor
	//////////////////////////////////
	CAccessLog();
	virtual ~CAccessLog();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	// Return the database record ID
	ULONG GetID();
	// Return the User ID
	LPCTSTR GetUserID();
	// Return the logon date
	LPCTSTR GetLogonDate();
	// Return the processes status
	LPCTSTR GetProcessesStatus();


	//////////////////////////////////
	// Set attributes values
	//////////////////////////////////

	void SetID( ULONG ulID);
	// Set the user ID
	void SetUserID( LPCTSTR lpstrUserID);
	// Set user's logon date from string
	void SetLogonDate( LPCTSTR lpstrDate);
	// Set user's logon date from date object
	void SetLogonDate( COleDateTime &dtDate);
	// Set process status string
	void SetProcessesStatus( LPCTSTR lpstrProcess);
	// Retrieve the informations for the current Device
	BOOL Retrieve( LPCTSTR lpstrUserID, LPCTSTR lpstrProcessesNames);

	//////////////////////////////////
	// Comparison operators
	//////////////////////////////////

	int CAccessLog::operator==( const CAccessLog cObject) const;

protected: // Methods
	// Return TRUE if no error
	BOOL CheckRunningProcesses( LPCTSTR lpstrProcessesNames, CString &csProcessesStatus);

protected: // Attributes
	ULONG	m_ulID;				// Database record ID
	CString m_csUserID;			// User identity
	CString m_csLogonDate;		// User logon date
	CString m_csProcessesStatus;// Result of the processes check status
};
#endif // !defined(AFX_ACCESSLOG_H__3A200601_4875_11D5_B231_0040055338AF__INCLUDED_)

