//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Printer.cpp: implementation of the CPrinter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Printer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPrinter::CPrinter()
{
	Clear();
}

CPrinter::~CPrinter()
{
	Clear();
}

void CPrinter::SetName( LPCTSTR lpstrName)
{
	m_csName = lpstrName;
	StrForSQL( m_csName);
}

void CPrinter::SetDriver( LPCTSTR lpstrDriver)
{
	m_csDriver = lpstrDriver;
	StrForSQL( m_csDriver);
}

void CPrinter::SetPort( LPCTSTR lpstrPort)
{
	m_csPort = lpstrPort;
	StrForSQL( m_csPort);
}

void CPrinter::SetServerName( LPCTSTR lpstrServerName)
{
	m_csServerName = lpstrServerName;
	StrForSQL( m_csServerName);
}

void CPrinter::SetShareName( LPCTSTR lpstrShareName)
{
	m_csShareName = lpstrShareName;
	StrForSQL( m_csShareName);
}

void CPrinter::SetResolution( LPCTSTR lpstrResolution)
{
	m_csResolution = lpstrResolution;
	StrForSQL( m_csResolution);
}

void CPrinter::SetComment( LPCTSTR lpstrComment)
{
	m_csComment = lpstrComment;
	StrForSQL( m_csComment);
}

void CPrinter::SetShared( BOOL bShared)
{
	m_bShared = bShared;
}

void CPrinter::SetNetworkPrinter( BOOL bNetwork)
{
	m_bNetwork = bNetwork;
}

LPCTSTR CPrinter::GetName()
{
	return m_csName;
}

LPCTSTR CPrinter::GetDriver()
{
	return m_csDriver;
}

LPCTSTR CPrinter::GetPort()
{
	return m_csPort;
}

LPCTSTR CPrinter::GetServerName()
{
	return m_csServerName;
}

LPCTSTR CPrinter::GetShareName()
{
	return m_csShareName;
}

LPCTSTR CPrinter::GetResolution()
{
	return m_csResolution;
}

LPCTSTR CPrinter::GetComment()
{
	return m_csComment;
}

BOOL CPrinter::IsShared()
{
	return m_bShared;
}

BOOL CPrinter::IsNetworkPrinter()
{
	return m_bNetwork;
}

void CPrinter::Clear()
{
	m_csName.Empty();		
	m_csDriver.Empty();		
	m_csPort.Empty();	
	m_csServerName.Empty();
	m_csShareName.Empty();
	m_csResolution.Empty();
	m_csComment.Empty();
	m_bShared = FALSE;
	m_bNetwork = FALSE;
}

int CPrinter::operator==(CPrinter cObject) const
{
	return ((m_csName == cObject.GetName()) &&	
		(m_csDriver == cObject.GetDriver()) &&
		(m_csPort == cObject.GetPort()) &&
		(m_csServerName == cObject.GetServerName()) &&
		(m_csShareName == cObject.GetShareName()) &&
		(m_csResolution == cObject.GetResolution()) &&
		(m_csComment == cObject.GetComment()) &&
		m_bShared == cObject.IsShared() &&
		m_bShared == cObject.IsShared());
}
