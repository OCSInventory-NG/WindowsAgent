//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// SystemPort.cpp: implementation of the CSystemPort class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SystemPort.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSystemPort::CSystemPort()
{
	Clear();
}

CSystemPort::~CSystemPort()
{
	Clear();
}

LPCTSTR CSystemPort::GetType()
{
	return m_csType;
}

LPCTSTR CSystemPort::GetName()
{
	return m_csName;
}

LPCTSTR CSystemPort::GetCaption()
{
	return m_csCaption;
}

LPCTSTR CSystemPort::GetDescription()
{
	return m_csDescription;
}

void CSystemPort::SetType( DWORD dwType)
{
	// See DMTF Standard SMBIOS DSP 1034
	switch( dwType)
	{
	case 0x00: m_csType = _T( "None");								break;
	case 0x01: m_csType = _T( "Parallel Port XT/AT Compatible");	break;
	case 0x02: m_csType = _T( "Parallel Port PS/2");				break;
	case 0x03: m_csType = _T( "Parallel Port ECP");					break;
	case 0x04: m_csType = _T( "Parallel Port EPP");					break;
	case 0x05: m_csType = _T( "Parallel Port ECP/EPP");				break;
	case 0x06: m_csType = _T( "Serial Port XT/AT Compatible");		break;
	case 0x07: m_csType = _T( "Serial Port 16450 Compatible");		break;
	case 0x08: m_csType = _T( "Serial Port 16550 Compatible");		break;
	case 0x09: m_csType = _T( "Serial Port 16550A Compatible");		break;
	case 0x0A: m_csType = _T( "SCSI Port");							break;
	case 0x0B: m_csType = _T( "MIDI Port");							break;
	case 0x0C: m_csType = _T( "Joy Stick Port");					break;
	case 0x0D: m_csType = _T( "Keyboard Port");						break;
	case 0x0E: m_csType = _T( "Mouse Port");						break;
	case 0x0F: m_csType = _T( "SSA SCSI");							break;
	case 0x10: m_csType = _T( "USB");								break;
	case 0x11: m_csType = _T( "FireWire (IEEE P1394)");				break;
	case 0x12: m_csType = _T( "PCMCIA Type II");					break;
	case 0x13: m_csType = _T( "PCMCIA Type II");					break;
	case 0x14: m_csType = _T( "PCMCIA Type III");					break;
	case 0x15: m_csType = _T( "Cardbus");							break;
	case 0x16: m_csType = _T( "Access Bus Port");					break;
	case 0x17: m_csType = _T( "SCSI II");							break;
	case 0x18: m_csType = _T( "SCSI Wide");							break;
	case 0x19: m_csType = _T( "PC-98");								break;
	case 0x1A: m_csType = _T( "PC-98-Hireso");						break;
	case 0x1B: m_csType = _T( "PC-H98");							break;
	case 0x1C: m_csType = _T( "Video Port");						break;
	case 0x1D: m_csType = _T( "Audio Port");						break;
	case 0x1E: m_csType = _T( "Modem Port");						break;
	case 0x1F: m_csType = _T( "Network Port");						break;
	case 0x20: m_csType = _T( "SATA");								break;
	case 0x21: m_csType = _T( "SAS");								break;
	case 0xA0: m_csType = _T( "8251 Compatible");					break;
	case 0xA1: m_csType = _T( "8251 FIFO Compatible");				break;
	case 0xFF: m_csType = _T( "Other");								break;
	default: m_csType.Format( _T( "%s (%lu)"), NOT_AVAILABLE, dwType);	break;
	}
	StrForSQL( m_csType);
}

void CSystemPort::SetType(LPCTSTR lpstrType)
{
	m_csType = lpstrType;
	StrForSQL( m_csType);
}

void CSystemPort::SetConnectorType( DWORD dwType)
{
	// See DMTF Standard SMBIOS DSP 1034
	switch( dwType)
	{
	case 0x00: m_csDescription = _T( "None");						break;
	case 0x01: m_csDescription = _T( "Centronics");					break;
	case 0x02: m_csDescription = _T( "Mini Centronics");			break;
	case 0x03: m_csDescription = _T( "Proprietary");				break;
	case 0x04: m_csDescription = _T( "DB-25 pin male");				break;
	case 0x05: m_csDescription = _T( "DB-25 pin female");			break;
	case 0x06: m_csDescription = _T( "DB-15 pin male");				break;
	case 0x07: m_csDescription = _T( "DB-15 pin female");			break;
	case 0x08: m_csDescription = _T( "DB-9 pin male");				break;
	case 0x09: m_csDescription = _T( "DB-9 pin female");			break;
	case 0x0A: m_csDescription = _T( "RJ-11");						break;
	case 0x0B: m_csDescription = _T( "RJ-45");						break;
	case 0x0C: m_csDescription = _T( "50 Pin MiniSCSI");			break;
	case 0x0D: m_csDescription = _T( "Mini-DIN");					break;
	case 0x0E: m_csDescription = _T( "Micro-DIN");					break;
	case 0x0F: m_csDescription = _T( "PS/2");						break;
	case 0x10: m_csDescription = _T( "Infrared");					break;
	case 0x11: m_csDescription = _T( "HP-HIL");						break;
	case 0x12: m_csDescription = _T( "Access Bus (USB)");			break;
	case 0x13: m_csDescription = _T( "SSA SCSI");					break;
	case 0x14: m_csDescription = _T( "Circular DIN-8 male");		break;
	case 0x15: m_csDescription = _T( "Circular DIN-8 female");		break;
	case 0x16: m_csDescription = _T( "On Board IDE");				break;
	case 0x17: m_csDescription = _T( "On Board Floppy");			break;
	case 0x18: m_csDescription = _T( "9 Pin Dual Inline (pin 10 cut)");break;
	case 0x19: m_csDescription = _T( "25 Pin Dual Inline (pin 26 cut)");break;
	case 0x1A: m_csDescription = _T( "50 Pin Dual Inline");			break;
	case 0x1B: m_csDescription = _T( "68 Pin Dual Inline");			break;
	case 0x1C: m_csDescription = _T( "On Board Sound Input from CD-ROM");break;
	case 0x1D: m_csDescription = _T( "Mini-Centronics Type-14");	break;
	case 0x1E: m_csDescription = _T( "Mini-Centronics Type-26");	break;
	case 0x1F: m_csDescription = _T( "Mini-jack (headphones)");		break;
	case 0x20: m_csDescription = _T( "BNC");						break;
	case 0x21: m_csDescription = _T( "1394");						break;
	case 0x22: m_csDescription = _T( "SAS/SATA Plug Receptacle");	break;
	case 0xA0: m_csDescription = _T( "PC-98");						break;
	case 0xA1: m_csDescription = _T( "PC-98Hireso");				break;
	case 0xA2: m_csDescription = _T( "PC-H98");						break;
	case 0xA3: m_csDescription = _T( "PC-98Note");					break;
	case 0xA4: m_csDescription = _T( "PC-98Full");					break;
	case 0xFF: m_csDescription = _T( "Other");						break;
	default: m_csDescription.Format( _T( "%s (%lu)"), NOT_AVAILABLE, dwType);break;
	}
	StrForSQL( m_csDescription);
}

void CSystemPort::SetName(LPCTSTR lpstrName)
{
	m_csName = lpstrName;
	StrForSQL( m_csName);
}

void CSystemPort::SetCaption(LPCTSTR lpstrCaption)
{
	m_csCaption = lpstrCaption;
	StrForSQL( m_csCaption);
}

void CSystemPort::SetDescription(LPCTSTR lpstrDescription)
{
	m_csDescription = lpstrDescription;
	StrForSQL( m_csDescription);
}

void CSystemPort::Clear()
{
	m_csType.Empty();		
	m_csName.Empty();		
	m_csCaption.Empty();	
	m_csDescription.Empty();
}

int CSystemPort::operator==(CSystemPort cObject) const
{
	return ((m_csType == cObject.GetType()) &&
		(m_csName == cObject.GetName()) &&
		(m_csCaption == cObject.GetCaption()) &&
		(m_csDescription == cObject.GetDescription()));
}
