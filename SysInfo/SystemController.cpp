//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// SystemController.cpp: implementation of the CSystemController class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SystemController.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSystemController::CSystemController()
{
	Clear();
}

CSystemController::~CSystemController()
{
	Clear();
}

LPCTSTR CSystemController::GetManufacturer()
{
	return m_csManufacturer;
}

LPCTSTR CSystemController::GetType()
{
	return m_csType;
}

LPCTSTR CSystemController::GetName()
{
	return m_csName;
}

LPCTSTR CSystemController::GetHardwareVersion()
{
	return m_csHardwareVersion;
}

LPCTSTR CSystemController::GetCaption()
{
	return m_csCaption;
}

LPCTSTR CSystemController::GetDescription()
{
	return m_csDescription;
}

void CSystemController::SetManufacturer(LPCTSTR lpstrManufacturer)
{
	m_csManufacturer = lpstrManufacturer;
	StrForSQL( m_csManufacturer);
}

void CSystemController::SetType(LPCTSTR lpstrType)
{
	m_csType = lpstrType;
	StrForSQL( m_csType);
}

void CSystemController::SetHardwareVersion(LPCTSTR lpstrVersion)
{
	m_csHardwareVersion = lpstrVersion;
	StrForSQL( m_csHardwareVersion);
}

void CSystemController::SetName(LPCTSTR lpstrName)
{
	m_csName = lpstrName;
	StrForSQL( m_csName);
}

void CSystemController::SetCaption(LPCTSTR lpstrCaption)
{
	m_csCaption = lpstrCaption;
	StrForSQL( m_csCaption);
}

void CSystemController::SetDescription(LPCTSTR lpstrDescription)
{
	m_csDescription = lpstrDescription;
	StrForSQL( m_csDescription);
}

void CSystemController::Clear()
{
	m_csType.Empty();			
	m_csManufacturer.Empty();	
	m_csName.Empty();			
	m_csHardwareVersion.Empty();
	m_csCaption.Empty();		
	m_csDescription.Empty();	
}

int CSystemController::operator==(CSystemController cObject) const
{
	return ((m_csType == cObject.GetType()) &&
		(m_csManufacturer == cObject.GetManufacturer()) &&
		(m_csName == cObject.GetName()) &&
		(m_csHardwareVersion == cObject.GetHardwareVersion()) &&
		(m_csCaption == cObject.GetCaption()) &&
		(m_csDescription == cObject.GetDescription()));
}
