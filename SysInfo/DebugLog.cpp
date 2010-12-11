//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// DebugLog.cpp: implementation of the Logging machine
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DebugLog.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CStdioFile	m_fileSysInfoLog;
BOOL		m_bSysInfoLog = FALSE;

void OpenLog( LPCTSTR lpstrFolder)
{
	try
	{
		CString	csFileName;
		
		if (m_bSysInfoLog)
			m_fileSysInfoLog.Close();
		m_bSysInfoLog = FALSE;
		if (lpstrFolder == NULL)
			csFileName = _T( "SysInfo.log");
		else
			csFileName.Format( _T( "%s\\SysInfo.log"), lpstrFolder);
		if (!m_fileSysInfoLog.Open( csFileName, CFile::modeCreate|CFile::modeWrite|CFile::typeText|CFile::shareDenyWrite))
			// Unable to open file
			return;
		m_bSysInfoLog = TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		m_fileSysInfoLog.Abort();
		m_bSysInfoLog = FALSE;
	}
}

void AddLog(LPCTSTR lpstrLog,...)
{
	if (!m_bSysInfoLog)
		return;
	try
	{
		CString			csBuffer;
		va_list			argList;

		// Format arguments
		va_start(argList, lpstrLog);
		csBuffer.FormatV( lpstrLog, argList);
		va_end( argList);
		// Write string
		m_fileSysInfoLog.WriteString( csBuffer);
		m_fileSysInfoLog.Flush();
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		m_fileSysInfoLog.Abort();
	}
}

void CloseLog()
{
	try
	{
		if (m_bSysInfoLog)
			m_fileSysInfoLog.Close();
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		m_fileSysInfoLog.Abort();
	}
	m_bSysInfoLog = FALSE;
}
