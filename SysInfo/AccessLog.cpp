//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// AccessLog.cpp: implementation of the CAccessLog class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OcsProcess.h"
#include "AccessLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAccessLog::CAccessLog()
{
}

CAccessLog::~CAccessLog()
{
}

ULONG CAccessLog::GetID()
{
	return m_ulID;
}

LPCTSTR CAccessLog::GetUserID()
{
	return m_csUserID;
}

LPCTSTR CAccessLog::GetLogonDate()
{
	return (m_csLogonDate.IsEmpty() ? NULL : LPCTSTR( m_csLogonDate));
}

LPCTSTR CAccessLog::GetProcessesStatus()
{
	return m_csProcessesStatus;
}

void CAccessLog::SetID( ULONG ulID)
{
	m_ulID = ulID;
}

void CAccessLog::SetUserID( LPCTSTR lpstrUserID)
{
	m_csUserID = lpstrUserID;
	StrForSQL( m_csUserID);
}

void CAccessLog::SetLogonDate( LPCTSTR lpstrDate)
{
	m_csLogonDate = lpstrDate;
	StrForSQL( m_csLogonDate);
}

void CAccessLog::SetLogonDate( COleDateTime &dtDate)
{
	m_csLogonDate = dtDate.Format( _T( "%Y-%m-%d %H:%M:%S"));
	StrForSQL( m_csLogonDate);
}

void CAccessLog::SetProcessesStatus( LPCTSTR lpstrProcess)
{
	m_csProcessesStatus = lpstrProcess;
	StrForSQL( m_csProcessesStatus);
}

BOOL CAccessLog::CheckRunningProcesses(LPCTSTR lpstrProcessesNames, CString &csProcessesStatus)
{
	CString strHeadings = lpstrProcessesNames;
	int		nPos;
	// Search the lpstrProcessNames string for each process name, comma separated
	while ((nPos = strHeadings.Find(_T(","))) != -1)
	{
		CString strItem;
		strItem = strHeadings.Left(nPos);
		if (CProcess::IsProcessRunning( strItem))
			csProcessesStatus += strItem + _T( " Started, ");
		else
			csProcessesStatus += strItem + _T( " MISSING, ");
		strItem = strHeadings.Mid(nPos + 1);
		strHeadings = strItem;
	}
	if (CProcess::IsProcessRunning( strHeadings))
		csProcessesStatus += strHeadings + _T( " Started, ");
	else
		csProcessesStatus += strHeadings + _T( " MISSING, ");
	return TRUE;
}

BOOL CAccessLog::Retrieve( LPCTSTR lpstrUserID, LPCTSTR lpstrProcessesNames)
{
	COleDateTime	cTime = COleDateTime::GetCurrentTime();
	BOOL			bResult = FALSE;
	m_csUserID = lpstrUserID;
	StrForSQL( m_csUserID);
	m_csLogonDate = cTime.Format( _T( "%Y-%m-%d %H:%M:%S"));
	if (lpstrProcessesNames != NULL)
		bResult = CheckRunningProcesses( lpstrProcessesNames, m_csProcessesStatus);
	StrForSQL( m_csProcessesStatus);
	return bResult;
}

int CAccessLog::operator==(CAccessLog cObject) const
{
	return  (m_csUserID == cObject.GetUserID()) &&
			(m_csLogonDate == cObject.GetLogonDate()) &&
			(m_csProcessesStatus == cObject.GetProcessesStatus());
}
