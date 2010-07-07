//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// MemorySlot.cpp: implementation of the CMemorySlot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemorySlot.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemorySlot::CMemorySlot()
{
	Clear();
}

CMemorySlot::~CMemorySlot()
{
	Clear();
}

LPCTSTR CMemorySlot::GetCaption()
{
	return m_csCaption;
}

LPCTSTR CMemorySlot::GetDescription()
{
	return m_csDescription;
}

LPCTSTR CMemorySlot::GetCapacity()
{
	return m_csCapacity;
}

LPCTSTR CMemorySlot::GetUsage()
{
	return m_csUsage;
}

LPCTSTR CMemorySlot::GetSpeed()
{
	return m_csSpeed;
}

LPCTSTR CMemorySlot::GetType()
{
	return m_csType;
}

UINT CMemorySlot::GetSlotNumber()
{
	return m_uSlotNumber;
}

void CMemorySlot::SetCaption(LPCTSTR lpstrCaption)
{
	m_csCaption = lpstrCaption;
	StrForSQL( m_csCaption);
}

void CMemorySlot::SetDescription(LPCTSTR lpstrDescription)
{
	m_csDescription = lpstrDescription;
	StrForSQL( m_csDescription);
}

void CMemorySlot::SetCapacity(LPCTSTR lpstrCapacity)
{
	if ((lpstrCapacity != NULL) &&
		(_tcslen( lpstrCapacity) > 0))
		m_csCapacity = lpstrCapacity;
	else
		m_csCapacity = NOT_AVAILABLE;
	StrForSQL( m_csCapacity);
}

void CMemorySlot::SetUsage(DWORD dwUsage)
{
	switch (dwUsage)
	{
	case 0x01:
		m_csUsage = _T( "Other");
		break;
	case 0x02:
		m_csUsage = _T( "Unknown");
		break;
	case 0x03:
		m_csUsage = _T( "System Memory");
		break;
	case 0x04:
		m_csUsage = _T( "Video Memory");
		break;
	case 0x05:
		m_csUsage = _T( "Flash Memory");
		break;
	case 0x06:
		m_csUsage = _T( "Non-volatile RAM");
		break;
	case 0x07:
		m_csUsage = _T( "Cache Memory");
		break;
	default:
		m_csUsage.Format( _T( "%s (%lu)"), NOT_AVAILABLE, dwUsage);
		break;
	}
}

void CMemorySlot::SetUsage(LPCTSTR lpstrUsage)
{
	m_csUsage = lpstrUsage;
	StrForSQL( m_csUsage);
}

void CMemorySlot::SetSpeed(LPCTSTR lpstrSpeed)
{
	if ((lpstrSpeed != NULL) &&
		(_tcslen( lpstrSpeed) > 0))
		m_csSpeed = lpstrSpeed;
	else
		m_csSpeed = NOT_AVAILABLE;
	StrForSQL( m_csSpeed);
}

void CMemorySlot::SetType(DWORD dwType)
{
	// Type of memory
	switch (dwType)
	{
	case 0x01:
		m_csType = _T( "Other");
		break;
	case 0x02:
		m_csType = _T( "Unknown");
		break;
	case 0x03:
		m_csType = _T( "DRAM");
		break;
	case 0x04:
		m_csType = _T( "EDRAM");
		break;
	case 0x05:
		m_csType = _T( "VDRAM");
		break;
	case 0x06:
		m_csType = _T( "SRAM");
		break;
	case 0x07:
		m_csType = _T( "RAM");
		break;
	case 0x08:
		m_csType = _T( "ROM");
		break;
	case 0x09:
		m_csType = _T( "FLASH");
		break;
	case 0x0A:
		m_csType = _T( "EEPROM");
		break;
	case 0x0B:
		m_csType = _T( "FEPROM");
		break;
	case 0x0C:
		m_csType = _T( "EPROM");
		break;
	case 0x0D:
		m_csType = _T( "CDRAM");
		break;
	case 0x0E:
		m_csType = _T( "3DRAM");
		break;
	case 0x0F:
		m_csType = _T( "SDRAM");
		break;
	case 0x10:
		m_csType = _T( "SGRAM");
		break;
	case 0x11:
		m_csType = _T( "RDRAM");
		break;
	case 0x12:
		m_csType = _T( "DDR");
		break;
	case 0x13:
		m_csType = _T( "DDR2");
		break;
	case 0x14:
		m_csType = _T( "DDR2 FB-DIMM");
		break;
	case 0x15:
	case 0x16:
	case 0x17:
		m_csType = _T( "Reserved");
		break;
	case 0x18:
		m_csType = _T( "DDR3");
		break;
	case 0x19:
		m_csType = _T( "FBD2");
		break;
	default:
		m_csType.Format( _T( "%s (%lu)"), NOT_AVAILABLE, dwType);
		break;
	}
}

void CMemorySlot::SetTypeECC( DWORD dwECC)
{
	CString csECC;

	// Error correction
	switch (dwECC)
	{
	case 0x01:
		csECC.Format( _T( " (Other ECC)"));
		break;
	case 0x02:
		csECC.Format( _T( " (Unknown ECC)"));
		break;
	case 0x03:
		csECC.Format( _T( " (No ECC)"));
		break;
	case 0x04:
		csECC.Format( _T( " (Parity ECC)"));
		break;
	case 0x05:
		csECC.Format( _T( " (Single-bit ECC)"));
		break;
	case 0x06:
		csECC.Format( _T( " (Multi-bit ECC)"));
		break;
	case 0x07:
		csECC.Format( _T( " (CRC ECC)"));
		break;
	default:
		csECC.Format( _T( " (Unknown ECC %lu)"), dwECC);
		break;
	}
	m_csDescription += csECC;
}

void CMemorySlot::SetType(LPCTSTR lpstrType)
{
	m_csType = lpstrType;
	StrForSQL( m_csType);
}

void CMemorySlot::SetSlotNumber(UINT uNumber)
{
	m_uSlotNumber = uNumber;
}

void CMemorySlot::Clear()
{
	m_csCaption.Empty();	
	m_csDescription.Empty();
	m_csCapacity.Empty();	
	m_csType.Empty();		
	m_csSpeed.Empty();		
	m_csUsage.Empty();		
	m_uSlotNumber = 0;	
}

int CMemorySlot::operator==(CMemorySlot cObject) const
{
	return ((m_csCaption == cObject.GetCaption()) &&
		(m_csDescription == cObject.GetDescription()) &&
		(m_csCapacity == cObject.GetCapacity()) &&
		(m_csType == cObject.GetType()) &&
		(m_csSpeed == cObject.GetSpeed()) &&
		(m_csUsage == cObject.GetUsage()) &&
		(m_uSlotNumber == cObject.GetSlotNumber()));
}
