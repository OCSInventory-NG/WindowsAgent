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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
	if (!m_csFileName.IsEmpty() && m_hLogFile.Open( m_csFileName, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite|CFile::typeText|CFile::shareDenyWrite))
	{
		m_bLogOpened = TRUE;
		m_hLogFile.SeekToEnd();
	}
	else
		m_bLogOpened = FALSE;
	return m_bLogOpened;
}

void CLog::log(int iPriority, LPCTSTR lpstrLog, ...)
{
	va_list args;
	va_start(args, lpstrLog);

	switch(iPriority)
	{
	case LOG_PRIORITY_ERROR:
			log_error( lpstrLog, args);
			break;
	case LOG_PRIORITY_NOTICE:
			log_notice( lpstrLog, args);
			break;
	case LOG_PRIORITY_DEBUG:
			log_debug( lpstrLog, args);
			break;
	case LOG_PRIORITY_WARNING:
			log_warning( lpstrLog, args);
			break;
	case LOG_PRIORITY_TRACE:
			log_trace( lpstrLog, args);
			break;
	default:
			CString csError;
			csError.Format( _T( "ERROR *** LOG: Unknown log code (%i)\n"), iPriority);
			m_hLogFile.WriteString( csError);
			m_hLogFile.Flush();
	}
	va_end(args);
}


void CLog::log_error( LPCTSTR lpstrFormat, va_list args)
{
	try
	{
		CString csLog, csMessage;

		if ((m_dwLoglevel < LOG_PRIORITY_ERROR) || !isOpen())
			return;
		csMessage.FormatV( lpstrFormat, args);
		csLog.Format( _T( "ERROR *** %s\n"), csMessage);
		m_hLogFile.WriteString( csLog);
		m_hLogFile.Flush();
	}
	catch (CException *pEx)
	{
		pEx->Delete();
	}
}

void CLog::log_notice( LPCTSTR lpstrFormat, va_list args)
{
	try
	{
		CString csLog, csMessage;

		if ((m_dwLoglevel < LOG_PRIORITY_NOTICE) || !isOpen())
			return;
		csMessage.FormatV( lpstrFormat, args);
		csLog.Format( _T( "%s\n"), csMessage);
		m_hLogFile.WriteString( csLog);
		m_hLogFile.Flush();
	}
	catch (CException *pEx)
	{
		pEx->Delete();
	}
}

void CLog::log_debug( LPCTSTR lpstrFormat, va_list args)
{
	try
	{
		CString csLog, csMessage;

		if ((m_dwLoglevel < LOG_PRIORITY_DEBUG) || !isOpen())
			return;
		csMessage.FormatV( lpstrFormat, args);
		csLog.Format( _T( "\t%s\n"), csMessage);
		m_hLogFile.WriteString( csLog);
		m_hLogFile.Flush();
	}
	catch (CException *pEx)
	{
		pEx->Delete();
	}
}

void CLog::log_warning( LPCTSTR lpstrFormat, va_list args)
{
	try
	{
		CString csLog, csMessage;

		if ((m_dwLoglevel < LOG_PRIORITY_DEBUG) || !isOpen())
			return;
		csMessage.FormatV( lpstrFormat, args);
		csLog.Format( _T( "\tWARNING *** %s\n"), csMessage);
		m_hLogFile.WriteString( csLog);
		m_hLogFile.Flush();
	}
	catch (CException *pEx)
	{
		pEx->Delete();
	}
}


void CLog::log_trace( LPCTSTR lpstrFormat, va_list args)
{
	try
	{
		CString csLog, csMessage;

		if ((getAgentConfig()->isDebugRequired() < OCS_DEBUG_MODE_TRACE) || !isOpen())
			return;
		m_hLogFile.WriteString( _T( "================= TRACE START ===============\n"));
		csMessage.FormatV( lpstrFormat, args);
		csLog.Format( _T( "%s\n"), csMessage);
		m_hLogFile.WriteString( csLog);
		m_hLogFile.WriteString( _T( "================= TRACE STOP ===============\n"));
		m_hLogFile.Flush();
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		m_hLogFile.WriteString( _T( "\n======= FAILED TO WRITE TRACE - STOP =======\n"));
		m_hLogFile.Flush();
	}
}

void CLog::setLogLevel(DWORD dwLogLevel)
{
	m_dwLoglevel = dwLogLevel;
}