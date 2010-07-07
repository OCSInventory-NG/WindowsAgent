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
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
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

void CPrinter::Clear()
{
	m_csName.Empty();		
	m_csDriver.Empty();		
	m_csPort.Empty();		
}

int CPrinter::operator==(CPrinter cObject) const
{
	return ((m_csName == cObject.GetName()) &&	
		(m_csDriver == cObject.GetDriver()) &&
		(m_csPort == cObject.GetPort()));
}
