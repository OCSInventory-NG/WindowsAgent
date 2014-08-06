//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Bios.cpp: implementation of the CBios class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Bios.h"
#include "OcsCrypto.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBios::CBios()
{
	Clear();
}

CBios::~CBios()
{
}

LPCTSTR CBios::ParseChassisType( DWORD dwType)
{
	switch (dwType)
	{
	case 0x01:	return _T( "Other");
	case 0x02:	return _T( "Unknown");
	case 0x03:	return _T( "Desktop");	
	case 0x04:	return _T( "Low Profile Desktop");
	case 0x05:	return _T( "Pizza Box");
	case 0x06:	return _T( "Mini Tower");	
	case 0x07:	return _T( "Tower");
	case 0x08:	return _T( "Portable");
	case 0x09:	return _T( "LapTop");
	case 0x0A:	return _T( "Notebook");
	case 0x0B:	return _T( "Hand Held");
	case 0x0C:	return _T( "Docking Station");
	case 0x0D:	return _T( "All in One");
	case 0x0E:	return _T( "Sub Notebook");
	case 0x0F:	return _T( "Space-saving");	
	case 0x10:	return _T( "Lunch Box");
	case 0x11:	return _T( "Main Server Chassis");
	case 0x12:	return _T( "Expansion Chassis");
	case 0x13:	return _T( "SubChassis");
	case 0x14:	return _T( "Bus Expansion Chassis");
	case 0x15:	return _T( "Peripheral Chassis");
	case 0x16:	return _T( "RAID Chassis");
	case 0x17:	return _T( "Rack Mount Chassis");
	case 0x18:	return _T( "Sealed-case PC");
	case 0x19:	return _T( "Multi-system chassis");
	case 0x1A:	return _T( "CompactPCI");
	case 0x1B:	return _T( "AdvancedTCA");	
	case 0x1C:	return _T( "Blade");
	case 0x1D:	return _T( "Blade Enclosure");
	default: return NOT_AVAILABLE;
	}
}

void CBios::Set( LPCTSTR lpstrSystemManufacturer, LPCTSTR lpstrSystemModel, LPCTSTR lpstrSystemSerialNumber, LPCTSTR lpstrMachineType,
				 LPCTSTR lpstrBiosManufacturer, LPCTSTR lpstrBiosVersion, LPCTSTR lpstrBiosDate,
				 LPCTSTR lpstrMotherboardManufacturer, LPCTSTR lpstrMotherboardModel, LPCTSTR lpstrMotherboardSerialNumber)
{
	m_csSystemManufacturer = lpstrSystemManufacturer;
	StrForSQL( m_csSystemManufacturer);
	m_csSystemModel = lpstrSystemModel;
	StrForSQL( m_csSystemModel);
	m_csSystemSerialNumber = lpstrSystemSerialNumber;
	StrForSQL( m_csSystemSerialNumber);
	m_csMachineType = lpstrMachineType;
	StrForSQL( m_csMachineType);
	m_csBiosManufacturer = lpstrBiosManufacturer;
	StrForSQL( m_csBiosManufacturer);
	m_csBiosVersion = lpstrBiosVersion;
	StrForSQL( m_csBiosVersion);
	m_csBiosDate = lpstrBiosDate;
	StrForSQL( m_csBiosDate);
	m_csMotherboardManufacturer = lpstrMotherboardManufacturer;
	StrForSQL(m_csMotherboardManufacturer);
	m_csMotherboardModel = lpstrMotherboardModel;
	StrForSQL(m_csMotherboardModel);
	m_csMotherboardSerialNumber = lpstrMotherboardSerialNumber;
	StrForSQL(m_csMotherboardSerialNumber);
}

void CBios::SetSystemManufacturer(LPCTSTR lpstrSystemManufacturer)
{
	m_csSystemManufacturer = lpstrSystemManufacturer;
	StrForSQL( m_csSystemManufacturer);
}

void CBios::SetSystemModel(LPCTSTR lpstrSystemModel)
{
	m_csSystemModel = lpstrSystemModel;
	StrForSQL( m_csSystemModel);
}

void CBios::SetSystemSerialNumber(LPCTSTR lpstrSystemSerialNumber)
{
	m_csSystemSerialNumber = lpstrSystemSerialNumber;
	StrForSQL( m_csSystemSerialNumber);
}

void CBios::SetMotherboardManufacturer(LPCTSTR lpstrMotherboardManufacturer)
{
	m_csMotherboardManufacturer = lpstrMotherboardManufacturer;
	StrForSQL( m_csMotherboardManufacturer);
}

void CBios::SetMotherboardModel(LPCTSTR lpstrMotherboardModel)
{
	m_csMotherboardModel = lpstrMotherboardModel;
	StrForSQL( m_csMotherboardModel);
}

void CBios::SetMotherboardSerialNumber(LPCTSTR lpstrMotherboardSerialNumber)
{
	m_csMotherboardSerialNumber = lpstrMotherboardSerialNumber;
	StrForSQL( m_csMotherboardSerialNumber);
}

void CBios::SetMachineType(LPCTSTR lpstrMachineType)
{
	m_csMachineType = lpstrMachineType;
	StrForSQL( m_csMachineType);
}

void CBios::SetBiosManufacturer(LPCTSTR lpstrBiosManufacturer)
{
	m_csBiosManufacturer = lpstrBiosManufacturer;
	StrForSQL( m_csBiosManufacturer);
}

void CBios::SetBiosVersion(LPCTSTR lpstrBiosVersion)
{
	m_csBiosVersion = lpstrBiosVersion;
	StrForSQL( m_csBiosVersion);
}

void CBios::SetBiosDateFormat(LPCTSTR lpstrBiosDate)
{
	m_csBiosDate = lpstrBiosDate;
	// Format date with *** or 000
	if(m_csBiosDate.GetLength()>8)
		if(m_csBiosDate.GetAt(8)=='*'||m_csBiosDate.GetAt(8)=='0')
		{
			CString d,m,y;
			d+=m_csBiosDate[6]; d+=m_csBiosDate[7]; 
			m+=m_csBiosDate[4]; m+=m_csBiosDate[5]; 
			y+=m_csBiosDate[0]; y+=m_csBiosDate[1];
			y+=m_csBiosDate[2]; y+=m_csBiosDate[3];
			m_csBiosDate.Format( _T( "%s/%s/%s"),d,m,y);
		}
	StrForSQL( m_csBiosDate);
}

void CBios::SetBiosDate(LPCTSTR lpstrBiosDate)
{
	m_csBiosDate = lpstrBiosDate;
	StrForSQL( m_csBiosDate);
}

void CBios::SetAssetTag(LPCTSTR lpstrTag)
{
	m_csAssetTag = lpstrTag;
	StrForSQL( m_csAssetTag);
}

LPCTSTR CBios::GetSystemManufacturer()
{
	return m_csSystemManufacturer;
}

LPCTSTR CBios::GetSystemModel()
{
	return m_csSystemModel;
}

LPCTSTR CBios::GetSystemSerialNumber()
{
	return m_csSystemSerialNumber;
}

LPCTSTR CBios::GetMotherboardManufacturer()
{
	return m_csMotherboardManufacturer;
}

LPCTSTR CBios::GetMotherboardModel()
{
	return m_csMotherboardModel;
}

LPCTSTR CBios::GetMotherboardSerialNumber()
{
	return m_csMotherboardSerialNumber;
}

LPCTSTR CBios::GetMachineType()
{
	return m_csMachineType;
}

LPCTSTR CBios::GetBiosManufacturer()
{
	return m_csBiosManufacturer;
}

LPCTSTR CBios::GetBiosVersion()
{
	return m_csBiosVersion;
}

LPCTSTR CBios::GetBiosDate()
{
	return m_csBiosDate;
}

LPCTSTR CBios::GetAssetTag()
{
	return m_csAssetTag;
}

BOOL CBios::IsValidSystemManufacturer()
{
	return ((m_csSystemManufacturer.GetLength() > 0) && (m_csSystemManufacturer.Compare( NOT_AVAILABLE) != 0));
}

BOOL CBios::IsValidSystemModel()
{
	return ((m_csSystemModel.GetLength() > 0) && (m_csSystemModel.Compare( NOT_AVAILABLE) != 0));
}

BOOL CBios::IsValidSystemSerialNumber()
{
	return ((m_csSystemSerialNumber.GetLength() > SYSINFO_MIN_SN_LENGTH) && (m_csSystemSerialNumber.Compare( NOT_AVAILABLE) != 0));
}

BOOL CBios::IsValidAssetTag()
{
	return ((m_csAssetTag.GetLength() > 0) && (m_csAssetTag.Compare( NOT_AVAILABLE) != 0));
}

void CBios::Clear()
{
	m_csSystemManufacturer.Empty();
	m_csSystemModel.Empty();
	m_csSystemSerialNumber.Empty();
	m_csMachineType.Empty();
	m_csBiosManufacturer.Empty();
	m_csBiosVersion.Empty();
	m_csBiosDate.Empty();
	m_csAssetTag.Empty();
}

LPCTSTR CBios::GetHash()
{
	COcsCrypto	myHash;
	static CString		csToHash;

	if (m_csSystemManufacturer.IsEmpty() && m_csSystemModel.IsEmpty() &&
		m_csSystemSerialNumber.IsEmpty() && m_csMachineType.IsEmpty() &&
		m_csBiosManufacturer.IsEmpty() && m_csBiosVersion.IsEmpty() &&
		m_csBiosDate.IsEmpty() && m_csAssetTag.IsEmpty())
		return NULL;
	if (!myHash.HashInit())
		return NULL;
	csToHash.Format( _T( "%s%s%s%s%s%s%s%s"), m_csSystemManufacturer, m_csSystemModel,
					 m_csSystemSerialNumber, m_csBiosManufacturer, m_csBiosVersion,
					 m_csBiosDate,m_csMachineType,m_csAssetTag);
	myHash.HashUpdate( csToHash);
	return myHash.HashFinal();
}

int CBios::operator==(CBios cObject) const
{
	return	(m_csBiosDate == cObject.GetBiosDate()) &&
			(m_csBiosManufacturer == cObject.GetBiosManufacturer()) &&
			(m_csBiosVersion == cObject.GetBiosVersion()) &&
			(m_csMachineType == cObject.GetMachineType()) && 
			(m_csSystemManufacturer == cObject.GetSystemManufacturer()) &&
			(m_csSystemModel == cObject.GetSystemModel()) &&
			(m_csSystemSerialNumber == cObject.GetSystemSerialNumber()) &&
			(m_csAssetTag == cObject.GetAssetTag());
}
