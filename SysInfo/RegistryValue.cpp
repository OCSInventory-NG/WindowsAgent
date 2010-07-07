//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// RegistryValue.cpp: implementation of the CRegistryValue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RegistryValue.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegistryValue::CRegistryValue()
{
	Clear();
}

CRegistryValue::~CRegistryValue()
{
	Clear();
}

LPCTSTR CRegistryValue::GetName()
{
	return m_csName;
}

LPCTSTR CRegistryValue::GetValue()
{
	return m_csValue;
}

void CRegistryValue::SetName(LPCTSTR lpstrName)
{
	m_csName = lpstrName;
	StrForSQL( m_csName);
}

void CRegistryValue::SetValue(LPCTSTR lpstrValue)
{
	m_csValue = lpstrValue;
	StrForSQL( m_csValue);
}

void CRegistryValue::Set( LPCTSTR lpstrName, LPCTSTR lpstrValue)
{
	m_csName = lpstrName;
	StrForSQL( m_csName);
	m_csValue = lpstrValue;
	StrForSQL( m_csValue);
}

void CRegistryValue::Clear()
{
	m_csName.Empty();		
	m_csValue.Empty();		
}

int CRegistryValue::operator==(CRegistryValue cObject) const
{
	return ((m_csName == cObject.GetName()) &&
		(m_csValue == cObject.GetValue()));		
}
