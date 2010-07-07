//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// InputDevice.cpp: implementation of the CInputDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InputDevice.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInputDevice::CInputDevice()
{
	Clear();
}

CInputDevice::~CInputDevice()
{
	Clear();
}

LPCTSTR CInputDevice::GetManufacturer()
{
	return m_csManufacturer;
}

LPCTSTR CInputDevice::GetType()
{
	return m_csType;
}

LPCTSTR CInputDevice::GetCaption()
{
	return m_csCaption;
}

LPCTSTR CInputDevice::GetDescription()
{
	return m_csDescription;
}

LPCTSTR CInputDevice::GetPointingType()
{
	return m_csPointingType;
}

LPCTSTR CInputDevice::GetPointingInterface()
{
	return m_csInterface;
}

void CInputDevice::SetManufacturer(LPCTSTR lpstrManufacturer)
{
	m_csManufacturer = lpstrManufacturer;
	StrForSQL( m_csManufacturer);
}

void CInputDevice::SetType(LPCTSTR lpstrType)
{
	m_csType = lpstrType;
	StrForSQL( m_csType);
}

void CInputDevice::SetCaption(LPCTSTR lpstrCaption)
{
	m_csCaption = lpstrCaption;
	StrForSQL( m_csCaption);
}

void CInputDevice::SetDescription(LPCTSTR lpstrDescription)
{
	m_csDescription = lpstrDescription;
	StrForSQL( m_csDescription);
}

void CInputDevice::SetPointingType(LPCTSTR lpstrPointingType)
{
	m_csPointingType = lpstrPointingType;
	StrForSQL( m_csPointingType);
}

void CInputDevice::SetPointingType(DWORD dwPointingType)
{
	switch (dwPointingType)
	{
	case 1: // Other
		m_csPointingType = _T( "Other");
		break;
	case 2: // Unknown
		m_csPointingType = NOT_AVAILABLE;
		break;
	case 3: // Mouse
		m_csPointingType = _T( "Mouse");
		break;
	case 4: // Track Ball
		m_csPointingType = _T( "Track Ball");
		break;
	case 5: // Track Point
		m_csPointingType = _T( "Track Point");
		break;
	case 6: // Glide Point
		m_csPointingType = _T( "Glide Point");
		break;
	case 7: // Touch Pad
		m_csPointingType = _T( "Touch Pad");
		break;
	case 8: // Touch Screen
		m_csPointingType = _T( "Touch Screen");
		break;
	case 9: // Mouse - Optical Sensor
		m_csPointingType = _T( "Mouse - Optical Sensor");
		break;
	default: // Unknown
		m_csPointingType.Format( _T( "%s (%lu)"), NOT_AVAILABLE, dwPointingType);
		break;
	}
}

void CInputDevice::SetPointingInterface(LPCTSTR lpstrInterface)
{
	m_csInterface = lpstrInterface;
	StrForSQL( m_csInterface);
}

void CInputDevice::SetPointingInterface(DWORD dwInterface)
{
	switch (dwInterface)
	{
	case 1: // Other
		m_csInterface = _T( "Other");
		break;
	case 2: // Unknown
		m_csInterface = NOT_AVAILABLE;
		break;
	case 3: // Serial
		m_csInterface = _T( "Serial");
		break;
	case 4: // PS/2
		m_csInterface = _T( "PS/2");
		break;
	case 5: // Infrared
		m_csInterface = _T( "Infrared");
		break;
	case 6: // HP-HIL
		m_csInterface = _T( "HP-HIL");
		break;
	case 7: // Bus mouse
		m_csInterface = _T( "Bus mouse");
		break;
	case 8: // ADB
		m_csInterface = _T( "Apple Desktop Bus");
		break;
	case 160: // Bus mouse DB-9
		m_csInterface = _T( "Bus mouse DB-9");
		break;
	case 161: // Bus mouse micro-DIN
		m_csInterface = _T( "Bus mouse micro-DIN");
		break;
	case 162: // USB
		m_csInterface = _T( "USB");
		break;
	default: // Unknwon
		m_csInterface.Format( _T( "%s (%lu)"), NOT_AVAILABLE, dwInterface);
		break;
	}
}

void CInputDevice::Clear()
{
	m_csType.Empty();			
	m_csManufacturer.Empty();	
	m_csCaption.Empty();		
	m_csDescription.Empty();	
	m_csPointingType.Empty();	
	m_csInterface.Empty();		
}

int CInputDevice::operator==( CInputDevice cObject) const
{
	return ((m_csType == cObject.GetType()) &&			
			(m_csManufacturer == cObject.GetManufacturer()) &&
			(m_csCaption == cObject.GetCaption()) &&	
			(m_csDescription == cObject.GetDescription()) &&
			(m_csPointingType == cObject.GetPointingType()) &&
			(m_csInterface == cObject.GetPointingInterface()));	
}
