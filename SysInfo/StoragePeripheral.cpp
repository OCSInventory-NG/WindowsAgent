//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// StoragePeripheral.cpp: implementation of the CStoragePeripheral class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StoragePeripheral.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStoragePeripheral::CStoragePeripheral()
{
	Clear();
}

CStoragePeripheral::~CStoragePeripheral()
{
	Clear();
}

LPCTSTR CStoragePeripheral::GetManufacturer()
{
	return m_csManufacturer;
}

LPCTSTR CStoragePeripheral::GetType()
{
	return m_csType;
}

LPCTSTR CStoragePeripheral::GetModel()
{
	return m_csModel;
}

LPCTSTR CStoragePeripheral::GetDescription()
{
	return m_csDescription;
}

LPCTSTR CStoragePeripheral::GetName()
{
	return m_csName;
}

unsigned __int64 CStoragePeripheral::GetSize()
{
	return m_u64Size;
}

LPCTSTR CStoragePeripheral::GetSizeString()
{
	static CString csBuffer;

	if (m_u64Size > 0)
		//csBuffer.Format( _T( "%I64u MB"), m_u64Size);
		csBuffer.Format( _T( "%I64u"), m_u64Size);
	else
		csBuffer.Empty();
	return csBuffer;
}

LPCTSTR CStoragePeripheral::GetSN()
{
	return m_csSN;
}

LPCTSTR CStoragePeripheral::GetFirmware()
{
	return m_csFirmware;
}

void CStoragePeripheral::SetManufacturer(LPCTSTR lpstrManufacturer)
{
	m_csManufacturer = lpstrManufacturer;
	StrForSQL( m_csManufacturer);
}

void CStoragePeripheral::SetType(LPCTSTR lpstrType)
{
	m_csType = lpstrType;
	StrForSQL( m_csType);
}

void CStoragePeripheral::SetName(LPCTSTR lpstrName)
{
	m_csName = lpstrName;
	StrForSQL( m_csName);
}

void CStoragePeripheral::SetModel(LPCTSTR lpstrModel)
{
	m_csModel = lpstrModel;
	StrForSQL( m_csModel);
}

void CStoragePeripheral::SetDescription(LPCTSTR lpstrDescription)
{
	m_csDescription = lpstrDescription;
	StrForSQL( m_csDescription);
}

void CStoragePeripheral::SetSize( unsigned __int64 u64Size)
{
	m_u64Size = u64Size;
}

void CStoragePeripheral::SetSN(LPCTSTR lpstrSN)
{
	m_csSN = lpstrSN;
	StrForSQL( m_csSN);
}

void CStoragePeripheral::SetFirmware( LPCTSTR lpstrRev)
{
	m_csFirmware = lpstrRev;
	StrForSQL( m_csFirmware);
}

void CStoragePeripheral::Clear()
{
	m_csType.Empty();			
	m_csManufacturer.Empty();	
	m_csName.Empty();			
	m_csModel.Empty();			
	m_csDescription.Empty();	
	m_csSN.Empty();
	m_csFirmware.Empty();
	m_u64Size = 0;			
}

int CStoragePeripheral::operator==(CStoragePeripheral cObject) const
{
	return ((m_csType == cObject.GetType()) &&
		(m_csManufacturer == cObject.GetManufacturer()) &&
		(m_csName == cObject.GetName()) &&
		(m_csModel == cObject.GetModel()) &&
		(m_csDescription == cObject.GetDescription()) &&
		(m_u64Size == cObject.GetSize()) &&
		(m_csSN == cObject.GetSN()) &&
		(m_csFirmware == cObject.GetFirmware()));
}
