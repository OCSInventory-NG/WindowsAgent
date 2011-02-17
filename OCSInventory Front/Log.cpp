//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Log.cpp: implementation of the CLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Config.h"
#include "Log.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLog::CLog()
{
	m_dwLoglevel = LOG_PRIORITY_NOTICE;
	m_bLogOpened = FALSE;
	m_hLogFile;
	// Disable use of log file by default
	setApplication();
}

CLog::~CLog()
{
	if (m_bLogOpened)
	{
		m_hLogFile.Close();
		m_bLogOpened = FALSE;
	}
}

void CLog::setApplication( LPCTSTR lpstrApp)
{
	if (m_bLogOpened)
	{
		// Already opened
		m_hLogFile.Close();
		m_bLogOpened = FALSE;
		return;
	}
	if (lpstrApp != NULL)
		m_csFileName.Format( _T( "%s\\%s.log"), getDataFolder(), lpstrApp);
	else
		m_csFileName.Empty();
}

BOOL CLog::isOpen()
{
	if (m_bLogOpened)
		// Already opened
		return TRUE;

	// Only try to open if filename ok
	if (!m_csFileName.IsEmpty() && m_hLogFile.Open( m_csFileName, CFile::modeWrite|CFile::typeText|CFile::shareDenyWrite))
		m_bLogOpened = TRUE;
	else
		m_bLogOpened = FALSE;
	m_hLogFile.SeekToEnd();
	return m_bLogOpened;
}

void CLog::log(int iPriority, LPCTSTR lpstrLog, ...)
{
	CString csMessage;
	va_list args;
	va_start(args, lpstrLog);
	csMessage.FormatV(lpstrLog, args);
	va_end(args);

	switch(iPriority)
	{
	case LOG_PRIORITY_ERROR:
			log_error( csMessage);
			break;
	case LOG_PRIORITY_NOTICE:
			log_notice( csMessage);
			break;
	case LOG_PRIORITY_DEBUG:
			log_debug( csMessage);
			break;
	case LOG_PRIORITY_WARNING:
			log_warning( csMessage);
			break;
	case LOG_PRIORITY_TRACE:
			log_trace( csMessage);
			break;
	default:
			CString csError;
			csError.Format( _T( "LOG: Unknown log code (%i)"), iPriority);
			log_error(csError);
	}
}


void CLog::log_error( LPCTSTR lpstrMessage)
{
	try
	{
		CString csLog;

		if ((m_dwLoglevel < LOG_PRIORITY_ERROR) || !isOpen())
			return;
		csLog.Format( _T( "ERROR *** %s\n"), lpstrMessage);
		m_hLogFile.WriteString( csLog);
		m_hLogFile.Flush();
	}
	catch (CException *pEx)
	{
		pEx->Delete();
	}
}

void CLog::log_notice( LPCTSTR lpstrMessage)
{
	try
	{
		CString csLog;

		if ((m_dwLoglevel < LOG_PRIORITY_NOTICE) || !isOpen())
			return;
		csLog.Format( _T( "%s\n"), lpstrMessage);
		m_hLogFile.WriteString( csLog);
		m_hLogFile.Flush();
	}
	catch (CException *pEx)
	{
		pEx->Delete();
	}
}

void CLog::log_debug( LPCTSTR lpstrMessage)
{
	try
	{
		CString csLog;

		if ((m_dwLoglevel < LOG_PRIORITY_DEBUG) || !isOpen())
			return;
		csLog.Format( _T( "\t%s\n"), lpstrMessage);
		m_hLogFile.WriteString( csLog);
		m_hLogFile.Flush();
	}
	catch (CException *pEx)
	{
		pEx->Delete();
	}
}

void CLog::log_warning( LPCTSTR lpstrMessage)
{
	try
	{
		CString csLog;

		if ((m_dwLoglevel < LOG_PRIORITY_DEBUG) || !isOpen())
			return;
		csLog.Format( _T( "\tWARNING *** %s\n"), lpstrMessage);
		m_hLogFile.WriteString( csLog);
		m_hLogFile.Flush();
	}
	catch (CException *pEx)
	{
		pEx->Delete();
	}
}


void CLog::log_trace( LPCTSTR lpstrMessage)
{
	try
	{
		CString csLog;

		if ((getAgentConfig()->isDebugRequired() < OCS_DEBUG_MODE_TRACE) || !isOpen())
			return;
		csLog.Format( _T( "================= TRACE START ===============\n%s\n================= TRACE STOP ===============\n"), lpstrMessage);
		m_hLogFile.WriteString( csLog);
		m_hLogFile.Flush();
	}
	catch (CException *pEx)
	{
		pEx->Delete();
	}
}

void CLog::setLogLevel(DWORD dwLogLevel)
{
	m_dwLoglevel = dwLogLevel;
}