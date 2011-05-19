//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Modem.cpp: implementation of the CModem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Modem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModem::CModem()
{
	Clear();
}

CModem::~CModem()
{
	Clear();
}

LPCTSTR CModem::GetType()
{
	return m_csType;
}

LPCTSTR CModem::GetName()
{
	return m_csName;
}

LPCTSTR CModem::GetModel()
{
	return m_csModel;
}

LPCTSTR CModem::GetDescription()
{
	return m_csDescription;
}

void CModem::SetType(LPCTSTR lpstrType)
{
	m_csType = lpstrType;
	StrForSQL( m_csType);
}

void CModem::SetName(LPCTSTR lpstrName)
{
	m_csName = lpstrName;
	StrForSQL( m_csName);
}

void CModem::SetModel(LPCTSTR lpstrModel)
{
	m_csModel = lpstrModel;
	StrForSQL( m_csModel);
}

void CModem::SetDescription(LPCTSTR lpstrDescription)
{
	m_csDescription = lpstrDescription;
	StrForSQL( m_csDescription);
}

void CModem::Clear()
{
	m_csType.Empty();		
	m_csName.Empty();		
	m_csModel.Empty();		
	m_csDescription.Empty();
}

int CModem::operator==(CModem cObject) const
{
	return ((m_csType == cObject.GetType()) &&
		(m_csName == cObject.GetName()) &&		
		(m_csModel == cObject.GetModel()) &&
		(m_csDescription == cObject.GetDescription()));
}