//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// SystemSlot.cpp: implementation of the CSystemSlot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SystemSlot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSystemSlot::CSystemSlot()
{
	Clear();
}

CSystemSlot::~CSystemSlot()
{
	Clear();
}

LPCTSTR CSystemSlot::GetName()
{
	return m_csName;
}

LPCTSTR CSystemSlot::GetDescription()
{
	return m_csDescription;
}

LPCTSTR CSystemSlot::GetSlotDesignation()
{
	return m_csDesignation;
}

LPCTSTR CSystemSlot::GetUsage()
{
	return m_csUsage;
}

LPCTSTR CSystemSlot::GetStatus()
{
	return m_csStatus;
}

BOOL CSystemSlot::IsShared()
{
	return m_bShared;
}

void CSystemSlot::SetName(LPCTSTR lpstrName)
{
	m_csName = lpstrName;
	StrForSQL( m_csName);
}

void CSystemSlot::SetType( DWORD dwType)
{
	// See DMTF Standard SMBIOS DSP 1034
	switch (dwType)
	{
	case 0x01: m_csDescription = _T( "Other");							break;
	case 0x02: m_csDescription = _T( "Unknown");						break;
	case 0x03: m_csDescription = _T( "ISA");							break;
	case 0x04: m_csDescription = _T( "MCA");							break;
	case 0x05: m_csDescription = _T( "EISA");							break;
	case 0x06: m_csDescription = _T( "PCI");							break;
	case 0x07: m_csDescription = _T( "PC Card (PCMCIA)");				break;
	case 0x08: m_csDescription = _T( "VL-VESA");						break;
	case 0x09: m_csDescription = _T( "Proprietary");					break;
	case 0x0A: m_csDescription = _T( "Processor Card Slot");			break;
	case 0x0B: m_csDescription = _T( "Proprietary Memory Card Slot");	break;
	case 0x0C: m_csDescription = _T( "I/O Riser Card Slot");			break;
	case 0x0D: m_csDescription = _T( "NuBus");							break;
	case 0x0E: m_csDescription = _T( "PCI - 66MHz Capable");			break;
	case 0x0F: m_csDescription = _T( "AGP");							break;
	case 0x10: m_csDescription = _T( "AGP 2X");							break;
	case 0x11: m_csDescription = _T( "AGP 4X");							break;
	case 0x12: m_csDescription = _T( "PCI-X");							break;
	case 0x13: m_csDescription = _T( "AGP 8X");							break;
	case 0xA0: m_csDescription = _T( "PC-98/C20");						break;
	case 0xA1: m_csDescription = _T( "PC-98/C24");						break;
	case 0xA2: m_csDescription = _T( "PC-98/E");						break;
	case 0xA3: m_csDescription = _T( "PC-98/Local Bus");				break;
	case 0xA4: m_csDescription = _T( "PC-98/Card");						break;
	case 0xA5: m_csDescription = _T( "PCI Express");					break;
	case 0xA6: m_csDescription = _T( "PCI Express x1");					break;
	case 0xA7: m_csDescription = _T( "PCI Express x2");					break;
	case 0xA8: m_csDescription = _T( "PCI Express x4");					break;
	case 0xA9: m_csDescription = _T( "PCI Express x8");					break;
	case 0xAA: m_csDescription = _T( "PCI Express x16");				break;
	case 0xAB: m_csDescription = _T( "PCI Express Gen 2");				break;
	case 0xAC: m_csDescription = _T( "PCI Express Gen 2 x1");			break;
	case 0xAD: m_csDescription = _T( "PCI Express Gen 2 x2");			break;
	case 0xAE: m_csDescription = _T( "PCI Express Gen 2 x4");			break;
	case 0xAF: m_csDescription = _T( "PCI Express Gen 2 x8");			break;
	case 0xB0: m_csDescription = _T( "PCI Express Gen 2 x16");			break;
	default: m_csDescription.Format( _T( "%s (%lu)"), NOT_AVAILABLE, dwType);break;
	}
	StrForSQL( m_csDescription);
}

void CSystemSlot::SetBusWidth( DWORD dwWidth)
{
	CString csWidth;

	// See DMTF Standard SMBIOS DSP 1034
	switch (dwWidth)
	{
	case 0x01: csWidth += _T( "Other");							break;
	case 0x02: csWidth += _T( "Unknown");						break;
	case 0x03: csWidth += _T( "8 bits");						break;
	case 0x04: csWidth += _T( "16 bits");						break;
	case 0x05: csWidth += _T( "32 bits");						break;
	case 0x06: csWidth += _T( "64 bits");						break;
	case 0x07: csWidth += _T( "128 bits");						break;
	case 0x08: csWidth += _T( "1x or x1");						break;
	case 0x09: csWidth += _T( "2x or x2");						break;
	case 0x0A: csWidth += _T( "4x or x4");						break;
	case 0x0B: csWidth += _T( "8x or x8");						break;
	case 0x0C: csWidth += _T( "12x or x12");					break;
	case 0x0D: csWidth += _T( "16x or x16");					break;
	case 0x0E: csWidth += _T( "32x or x32");					break;
	default: csWidth.Format( _T( "%s (%lu)"), NOT_AVAILABLE, dwWidth);break;
	}
	m_csDescription.AppendFormat( _T( " (data bus width %s)"), csWidth);
	StrForSQL( m_csDescription);
}

void CSystemSlot::SetDescription(LPCTSTR lpstrDescription)
{
	m_csDescription = lpstrDescription;
	StrForSQL( m_csDescription);
}

void CSystemSlot::SetSlotDesignation(LPCTSTR lpstrDesignation)
{
	m_csDesignation = lpstrDesignation;
	StrForSQL( m_csDesignation);
}

void CSystemSlot::SetUsage(DWORD dwUsage)
{
	switch (dwUsage)
	{
	case 0x01: m_csUsage = _T( "Other");	break;
	case 0x02: m_csUsage = _T( "Unknown");	break;
	case 0x03: m_csUsage = _T( "Available");break;
	case 0x04: m_csUsage = _T( "In Use");	break;
	default: m_csUsage.Format( _T( "%s (%lu)"), NOT_AVAILABLE, dwUsage);break;
	}
}

void CSystemSlot::SetUsage(LPCTSTR lpstrUsage)
{
	m_csUsage = lpstrUsage;
	StrForSQL( m_csUsage);
}

void CSystemSlot::SetStatus(LPCTSTR lpstrStatus)
{
	m_csStatus = lpstrStatus;
	StrForSQL( m_csStatus);
}

void CSystemSlot::SetShared( BOOL bShared)
{
	m_bShared = bShared;
}

void CSystemSlot::Clear()
{
	m_csName.Empty();		
	m_csDescription.Empty();
	m_csDesignation.Empty();
	m_csUsage.Empty();		
	m_csStatus.Empty();		
	m_bShared = FALSE;		
}

int CSystemSlot::operator==(CSystemSlot cObject) const
{
	return ((m_csName == cObject.GetName()) &&
		(m_csDescription == cObject.GetDescription()) &&
		(m_csDesignation == cObject.GetSlotDesignation()) &&
		(m_csUsage == cObject.GetUsage()) &&
		(m_csStatus == cObject.GetStatus()) &&
		(m_bShared == cObject.IsShared()));
}
