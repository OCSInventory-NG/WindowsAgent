//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// SoundDevice.cpp: implementation of the CSoundDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SoundDevice.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSoundDevice::CSoundDevice()
{
	Clear();
}

CSoundDevice::~CSoundDevice()
{
	Clear();
}

LPCTSTR CSoundDevice::GetManufacturer()
{
	return m_csManufacturer;
}

LPCTSTR CSoundDevice::GetName()
{
	return m_csName;
}

LPCTSTR CSoundDevice::GetDescription()
{
	return m_csDescription;
}

void CSoundDevice::SetManufacturer(LPCTSTR lpstrManufacturer)
{
	m_csManufacturer = lpstrManufacturer;
	StrForSQL( m_csManufacturer);
}

void CSoundDevice::SetName(LPCTSTR lpstrName)
{
	m_csName = lpstrName;
	StrForSQL( m_csName);
}

void CSoundDevice::SetDescription(LPCTSTR lpstrDescription)
{
	m_csDescription = lpstrDescription;
	StrForSQL( m_csDescription);
}

void CSoundDevice::Clear()
{
	m_csManufacturer.Empty();
	m_csName.Empty();		
	m_csDescription.Empty();
}

int CSoundDevice::operator==(CSoundDevice cObject) const
{
	return ((m_csManufacturer == cObject.GetManufacturer()) &&
		(m_csName == cObject.GetName()) &&
		(m_csDescription == cObject.GetDescription()));
}
