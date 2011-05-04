//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOG_H__AEE32363_418A_45D1_8596_1E33C9930B48__INCLUDED_)
#define AFX_LOG_H__AEE32363_418A_45D1_8596_1E33C9930B48__INCLUDED_

#pragma once


#include "OCSInventory Front Classes.h"
#include "Singleton.h"
#include <string.h>

/* OCS Inventory NG log modes
   /DEBUG=0 => default mode 
   /DEBUG=1 => DEBUG mode, for more detailled logs
   /DEBUG=2 => TRACE mode, for full logs, including XML and SysInfo logs
*/

#define LOG_PRIORITY_ERROR			1	// This is a blocking error always shown
#define LOG_PRIORITY_NOTICE			2	// This a status message always shown
#define LOG_PRIORITY_DEBUG			3	// This is a status message shown only in DEBUG mode
#define LOG_PRIORITY_WARNING		4	// This is a warning message (non blocking) show only in DEBUG mode
#define LOG_PRIORITY_TRACE			5	// This is a trace message, used only in TRACE mode

class OCSINVENTORYFRONT_API CLog : public CSingleton<CLog>
{
	friend class CSingleton<CLog>;

public: // Methods

	// Standard destructor
	~CLog();

	// Gathers all messages and dispatch it
	void log(int, LPCTSTR, ...);
	// Set the current log priority
	void setLogLevel(DWORD);
	// Specify the application name to use it as log file name under Application data folder
	// Must be called before any call to "log" method if you which to use different log file
	// If set to NULL, then no log file is used
	void setApplication( LPCTSTR lpstrApp = NULL);


private: // Methods

	// Standard constructor
	CLog();

	// Create or open log file
	BOOL isOpen();
	// "Normal" log
	void log_notice( LPCTSTR lpstrFormat, va_list args);
	// Warning log, but can continue
	void log_warning( LPCTSTR lpstrFormat, va_list args);
	// Blocking error log
	void log_error( LPCTSTR lpstrFormat, va_list args);
	// Debug log
	void log_debug( LPCTSTR lpstrFormat, va_list args);
	// Trace log for full mode debug
	void log_trace( LPCTSTR lpstrFormat, va_list args);

private: // Atributes
	// Log filename
	CString m_csFileName;
	// The log file opened
	CStdioFile m_hLogFile;
	// Flag to indicate that log file is opened
	BOOL m_bLogOpened;
	// The current loglevel
	DWORD m_dwLoglevel;
};

#endif // !defined(AFX_LOG_H__AEE32363_418A_45D1_8596_1E33C9930B48__INCLUDED_)
