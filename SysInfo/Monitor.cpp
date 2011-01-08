//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Monitor.cpp: implementation of the CMonitor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Monitor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonitor::CMonitor()
{

}

CMonitor::~CMonitor()
{

}

LPCTSTR CMonitor::GetManufacturer()
{
	return m_csManufacturer;
}

LPCTSTR CMonitor::GetType()
{
	return m_csType;
}

LPCTSTR CMonitor::GetCaption()
{
	return m_csCaption;
}

LPCTSTR CMonitor::GetDescription()
{
	return m_csDescription;
}

LPCTSTR CMonitor::GetSerial()
{
	return m_csSerial;
}

void CMonitor::SetManufacturer(LPCTSTR lpstrManufacturer)
{
	m_csManufacturer = lpstrManufacturer;
	StrForSQL( m_csManufacturer);
}

void CMonitor::SetType(LPCTSTR lpstrType)
{
	m_csType = lpstrType;
	StrForSQL( m_csType);
}

void CMonitor::SetCaption(LPCTSTR lpstrCaption)
{
	m_csCaption = lpstrCaption;
	StrForSQL( m_csCaption);
}

void CMonitor::SetDescription(LPCTSTR lpstrDescription)
{
	m_csDescription = lpstrDescription;
	StrForSQL( m_csDescription);
}

void CMonitor::SetSerial(LPCTSTR lpstrSerial)
{
	m_csSerial = lpstrSerial;
	StrForSQL( m_csSerial);
}

BOOL CMonitor::IsValidSerial( LPCTSTR lpstrSN)
{
	CString csSN;

	if (lpstrSN == NULL)
		csSN = m_csSerial;
	else
		csSN = lpstrSN;
	return (csSN.GetLength() > SYSINFO_MIN_SN_LENGTH);
}

void CMonitor::Clear()
{
	m_csManufacturer.Empty();
	m_csType.Empty();		
	m_csCaption.Empty();	
	m_csDescription.Empty();
	m_csSerial.Empty();
}

int CMonitor::operator==(CMonitor cObject) const
{
	return ((m_csManufacturer == cObject.GetManufacturer()) &&
		(m_csType == cObject.GetType()) &&
		(m_csCaption == cObject.GetCaption()) &&
		(m_csDescription == cObject.GetDescription()) &&
		(m_csSerial == cObject.GetSerial()));
}
