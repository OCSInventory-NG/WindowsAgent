//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Wmi.cpp: implementation of the CWmi class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DebugLog.h"
#include "SysInfo.h"
#include "DiskInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWmi::CWmi()
{
	m_bConnected = FALSE;
}

CWmi::~CWmi()
{
	Disconnect();
}

BOOL CWmi::Connect( LPCTSTR lpstrDevice)
{
	try
	{
		CString	csCimRoot;

		// Do not connect if already connected
		if (m_bConnected)
			return TRUE;
		// Load OcsWmi wrapper library
		AddLog( _T( "WMI Connect: Trying to connect to WMI namespace root\\cimv2 on device <%s>...\n"), 
			(lpstrDevice == NULL ? _T( "Localhost") : lpstrDevice));
		if (lpstrDevice == NULL)
			csCimRoot.Format( _T( "root\\CIMV2"));
		else
			csCimRoot.Format( _T( "\\\\%s\\root\\CIMV2"), lpstrDevice);
		if (!m_dllWMI.ConnectWMI( csCimRoot))
		{
			// Unable to connect to WMI => no WMI support
			AddLog( _T( "\tFailed because unable to connect to WMI namespace (0x%lX) !\n"), m_dllWMI.GetLastErrorWMI());
			return FALSE;
		}
		m_bConnected = TRUE;
		AddLog( _T( "\tOK\n"));
		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception.\n"));
		return FALSE;
	}
}

BOOL CWmi::Disconnect()
{
	if (m_bConnected)
	{
		m_dllWMI.DisconnectWMI();
		AddLog( _T( "WMI Disconnect: Disconnected from WMI namespace.\n"));
	}
	m_bConnected = FALSE;
	return TRUE;
}

BOOL CWmi::IsConnected()
{
	return m_bConnected;
}

BOOL CWmi::IsDeviceReallyConnected( DWORD dwStatus)
{
	switch (dwStatus)
	{
	case 3: // Enabled
	case 4: // Disabled
		// This a connected device
		return TRUE;
	case 1: // Other
	case 2: // Unknown
	case 5: // Not applicable
	default:
		// Device is not currently connected
		return FALSE;
	}
}

LPCTSTR CWmi::GetArchitecture( DWORD dwArch)
{
	switch (dwArch)
	{
	case 0x0:
		return _T( "x86");
	case 0x1:
		return _T( "MIPS");
	case 0x2:
		return _T( "Alpha");
	case 0x3:
		return _T( "PowerPC");
	case 0x6:
		return _T( "IA64");
	case 0x9:
		return _T( "x86_64");
	default:
		return NOT_AVAILABLE;
	}
}

LPCTSTR CWmi::GetVoltage( DWORD dwVolts)
{
	switch (dwVolts)
	{
	case 1:
		return _T( "5 volts");
	case 2:
		return _T( "3.3 volts");
	case 4:
		return _T( "2.9 volts");
	default:
		return NOT_AVAILABLE;
	}
}

LPCTSTR CWmi::GetCpuStatus( DWORD dwStatus)
{
	switch (dwStatus)
	{
	case 0:
		return _T( "Unknown");
	case 1:
		return _T( "CPU Enabled");
	case 2:
		return _T( "CPU Disabled by User via BIOS Setup");
	case 3:
		return _T( "CPU Disabled by BIOS (POST Error)");
	case 4:
		return _T( "CPU Is Idle");
	case 5:
	case 6:
		return _T( "Reserved");
	case 7:
		return _T( "Other");
	default:
		return NOT_AVAILABLE;
	}
}

BOOL CWmi::IsNotebook()
{
	CString csBuffer;
	UINT	uIndex = 0;

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI IsNotebook: Trying to find Win32_PortableBattery WMI objects...\n"));
	// Try to use PortableBattery object
	try
	{
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_PortableBattery")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "EstimatedRunTime"));
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "\tOK (%u objects). Is a Notebook.\n"), uIndex);
			return TRUE;
		}
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception!\n"));
	}
	AddLog( _T( "\tNo Win32_PortableBattery => not a Notebook\n"));
	return FALSE;
}

BOOL CWmi::GetBiosInfo(CBios *pMyBios)
{
	CString	csManufacturer,
			csModel,
			csSN,
			csRdate,
			csBiosVer,
			csChassisType,
			csAssetTag;
	UINT	uIndex = 0;
	BOOL	bResult = FALSE;

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return bResult;

	pMyBios->Set( NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE);
	// GET BIOS Informations
	if (GetBios( csManufacturer, csBiosVer, csRdate, csSN))
	{
		pMyBios->SetBiosManufacturer( csManufacturer);
		pMyBios->SetBiosDateFormat( csRdate);
		pMyBios->SetBiosVersion( csBiosVer);
		pMyBios->SetSystemSerialNumber( csSN);
		bResult = TRUE;
	}
	// Try to use win32 computer system object to get System Manufacturer, Model
	if (GetSystemInformation( csManufacturer, csModel))
	{
		pMyBios->SetSystemManufacturer( csManufacturer);
		pMyBios->SetSystemModel( csModel);
	}
	// Try to use system enclosure object to get System Manufacturer, Type, S/N and chassis type
	if (GetSystemEnclosure( csManufacturer, csChassisType, csSN, csAssetTag))
	{
		if (!pMyBios->IsValidSystemManufacturer())
			pMyBios->SetSystemManufacturer( csManufacturer);
		pMyBios->SetMachineType( csChassisType);
		if (!pMyBios->IsValidSystemSerialNumber())
			pMyBios->SetSystemSerialNumber( csSN);
		pMyBios->SetAssetTag( csAssetTag);
		bResult = TRUE;
	}
	if (GetBaseBoard( csManufacturer, csModel, csSN))
	{
		// System enclosure not available => use mother board object
		if (!pMyBios->IsValidSystemManufacturer())
			pMyBios->SetSystemManufacturer( csManufacturer);
		if (!pMyBios->IsValidSystemModel())
			pMyBios->SetSystemModel( csModel);
		if (!pMyBios->IsValidSystemSerialNumber())
			pMyBios->SetSystemSerialNumber( csSN);
		pMyBios->SetMotherboardManufacturer( csManufacturer);
		pMyBios->SetMotherboardModel( csModel);
		pMyBios->SetMotherboardSerialNumber( csSN);
	}
	return bResult;
}

BOOL CWmi::GetBios( CString &csVendor, CString &csVersion, CString &csDate, CString &csSN)
{
	UINT	uIndex = 0;

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;
	
	// GET BIOS Informations
	AddLog( _T( "WMI GetBiosInfo: Trying to find Win32_Bios WMI objects...\n"));
	try
	{
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_Bios")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				csVendor = m_dllWMI.GetClassObjectStringValue( _T( "Manufacturer"));
				csVersion = m_dllWMI.GetClassObjectStringValue( _T( "SMBIOSBIOSVersion"));
				if (csVersion.IsEmpty())
					csVersion = m_dllWMI.GetClassObjectStringValue( _T( "BIOSVersion"));
				if (csVersion.IsEmpty())
					csVersion = m_dllWMI.GetClassObjectStringValue( _T( "Version"));
				csSN = m_dllWMI.GetClassObjectStringValue( _T( "SerialNumber"));
				// TODO: Throw exception sometimes, why ?
				csDate = m_dllWMI.GetClassObjectStringValue( _T( "ReleaseDate"));
				AddLog( _T( "\t\t<Manufacturer: %s><Version: %s><Date: %s><S/N: %s>\n"), 
					csVendor, csVersion, csDate, csSN);
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "\tFailed because no Win32_Bios object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
		return FALSE;
	}
}

BOOL CWmi::GetSystemInformation( CString &csVendor, CString &csModel)
{
	UINT	uIndex = 0;

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;
	
	// GET BIOS Informations
	AddLog( _T( "WMI GetSystemInformation: Trying to find Win32_ComputerSystem WMI objects...\n"));
	try
	{
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_ComputerSystem")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				csVendor = m_dllWMI.GetClassObjectStringValue( _T( "Manufacturer"));
				csModel = m_dllWMI.GetClassObjectStringValue( _T( "Model"));
				AddLog( _T( "\t\t<Manufacturer: %s><Model: %s>\n"), 
					csVendor, csModel);
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "\tFailed because no Win32_ComputerSystem object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
		return FALSE;
	}
}

BOOL CWmi::GetBaseBoard( CString &csVendor, CString &csProduct, CString &csSN)
{
	UINT	uIndex = 0;

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;
	
	// GET BIOS Informations
	AddLog( _T( "WMI GetBaseBoard: Trying to find Win32_BaseBoard WMI objects...\n"));
	try
	{
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_BaseBoard")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				csVendor = m_dllWMI.GetClassObjectStringValue( _T( "Manufacturer"));
				csProduct = m_dllWMI.GetClassObjectStringValue( _T( "Model"));
				csSN = m_dllWMI.GetClassObjectStringValue( _T( "SerialNumber"));
				AddLog( _T( "\t\t<Manufacturer: %s><Model: %s><S/N: %s>\n"), 
					csVendor, csProduct, csSN);
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "\tFailed because no Win32_BaseBoard object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
		return FALSE;
	}
}

BOOL CWmi::GetSystemEnclosure( CString &csVendor, CString &csType, CString &csSN, CString &csAssetTag)
{
	UINT	uIndex = 0;

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;
	
	// GET BIOS Informations
	AddLog( _T( "WMI GetSystemEnclosure: Trying to find Win32_SystemEnclosure WMI objects...\n"));
	try
	{
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_SystemEnclosure")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				csVendor = m_dllWMI.GetClassObjectStringValue( _T( "Manufacturer"));
				csType = m_dllWMI.GetClassObjectStringValue( _T( "ChassisTypes"));			
				ParseChassisType( csType);
				csSN = m_dllWMI.GetClassObjectStringValue( _T( "SerialNumber"));
				csAssetTag = m_dllWMI.GetClassObjectStringValue( _T( "SMBIOSAssetTag"));
				AddLog( _T( "\t\t<Manufacturer: %s><Type: %s><S/N: %s><Asset Tag: %s>\n"), 
					csVendor, csType, csSN, csAssetTag);
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "\tFailed because no Win32_SystemEnclosure object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
		return FALSE;
	}
}

BOOL CWmi::GetStoragePeripherals(CStoragePeripheralList *pMyList)
{
	UINT				uIndex = 0,
						uTotalIndex = 0;
	CStoragePeripheral	myObject;
	CString				csBuffer;

	ASSERT( pMyList);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	// Get Floppy drives
	AddLog( _T( "WMI GetStoragePeripherals: Trying to find Win32_FloppyDrive WMI objects...\n"));
	// Reset object list content
	pMyList->RemoveAll();
	try
	{
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_FloppyDrive")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Manufacturer"));
				myObject.SetManufacturer( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Caption"));
				myObject.SetModel( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
				myObject.SetDescription( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Name"));
				myObject.SetName( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "MediaType"));
				myObject.SetType( csBuffer);
				myObject.SetSize( m_dllWMI.GetClassObjectU64Value( _T( "Size")) / ONE_MEGABYTE);
				pMyList->AddTail( myObject);
				AddLog( _T( "\t\t<Manufacturer: %s><Caption: %s><Description: %s><Name: %s><MediaType: %s><Size: %s>\n"), 
					myObject.GetManufacturer(), myObject.GetModel(), myObject.GetDescription(),
					myObject.GetName(), myObject.GetType(), myObject.GetSizeString());
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotalIndex += uIndex;
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "\tFailed because no Win32_FloppyDrive object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
	}
	// Get Disk drives
	AddLog( _T( "WMI GetStoragePeripherals: Trying to find Win32_DiskDrive WMI objects...\n"));
	try
	{
		uIndex = 0;
		DiskInfo&	di = DiskInfo::GetDiskInfo();
		LONG		lFixedDisk = di.LoadDiskInfo();

		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_DiskDrive")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				LONG lDiskIndex = m_dllWMI.GetClassObjectDwordValue( _T( "Index"));
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Name"));
				myObject.SetName( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Manufacturer"));
				myObject.SetManufacturer( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
				myObject.SetDescription( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "MediaType"));
				myObject.SetType( csBuffer);
				myObject.SetSize( m_dllWMI.GetClassObjectU64Value( _T( "Size")) / ONE_MEGABYTE);
				// Vista, 2008, Seven and higher only
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "SerialNumber"));
				if (!myObject.IsValidSN( csBuffer) && (lDiskIndex < lFixedDisk))
				{
					// 2000/XP/2003 => use DiskInfo (DeviceIoControl)
					csBuffer = di.SerialNumber( lDiskIndex);
					if (myObject.IsValidSN( csBuffer))
						myObject.SetSN( csBuffer);
					csBuffer = di.ModelNumber( lDiskIndex);
					if (myObject.IsValidModel( csBuffer))
						myObject.SetModel( csBuffer);
					csBuffer = di.RevisionNumber( lDiskIndex);
					if (myObject.IsValidFirmware( csBuffer))
						myObject.SetFirmware( csBuffer);
				}
				else
				{
					// Vista, 2008, Seven or higher => Use WMI
					if (myObject.IsValidSN( csBuffer))
						myObject.SetSN( csBuffer);
					csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Model"));
					if (myObject.IsValidModel( csBuffer))
						myObject.SetModel( csBuffer);
					csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "FirmwareRevision"));
					if (myObject.IsValidFirmware( csBuffer))
						myObject.SetFirmware( csBuffer);
				}
				pMyList->AddTail( myObject);
				AddLog( _T( "\t\t<Manufacturer: %s><Caption: %s><Description: %s><Name: %s><MediaType: %s><Size: %s><S/N: %s><FirmwareRevision: %s>\n"), 
					myObject.GetManufacturer(), myObject.GetModel(), myObject.GetDescription(),
					myObject.GetName(), myObject.GetType(), myObject.GetSizeString(), 
					myObject.GetSN(), myObject.GetFirmware());
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		di.Destroy();
		if (uIndex > 0)
		{
			uTotalIndex += uIndex;
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "\tFailed because no Win32_DiskDrive object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
	}
	// Get CD-Rom drives
	AddLog( _T( "WMI GetStoragePeripherals: Trying to find Win32_CDROMDrive WMI objects...\n"));
	try
	{
		uIndex = 0;
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_CDROMDrive")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Manufacturer"));
				myObject.SetManufacturer( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Caption"));
				myObject.SetModel( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
				myObject.SetDescription( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Name"));
				myObject.SetName( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "RevisionLevel"));
				myObject.SetFirmware( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "MediaType"));
				myObject.SetType( csBuffer);
				myObject.SetSize( m_dllWMI.GetClassObjectU64Value( _T( "Size")) / ONE_MEGABYTE);
				pMyList->AddTail( myObject);
				AddLog( _T( "\t\t<Manufacturer: %s><Caption: %s><Description: %s><Name: %s><MediaType: %s><Size: %s><S/N: %s><RevisionLevel: %s>\n"), 
					myObject.GetManufacturer(), myObject.GetModel(), myObject.GetDescription(),
					myObject.GetName(), myObject.GetType(), myObject.GetSizeString(), 
					myObject.GetSN(), myObject.GetFirmware());
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotalIndex += uIndex;
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "\tFailed because no Win32_CDROMDrive object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
	}
	// Get tape drives
	AddLog( _T( "WMI GetStoragePeripherals: Trying to find Win32_TapeDrive WMI objects...\n"));
	try
	{
		uIndex = 0;
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_TapeDrive")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Manufacturer"));
				myObject.SetManufacturer( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Caption"));
				if (csBuffer.IsEmpty())
					csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Id"));
				myObject.SetModel( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
				myObject.SetDescription( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Name"));
				myObject.SetName( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "MediaType"));
				myObject.SetType( csBuffer);
				myObject.SetSize( m_dllWMI.GetClassObjectU64Value( _T( "Size")) / ONE_MEGABYTE);
				pMyList->AddTail( myObject);
				AddLog( _T( "\t\t<Manufacturer: %s><Caption/Id: %s><Description: %s><Name: %s><MediaType: %s>\n"), 
					myObject.GetManufacturer(), myObject.GetModel(), myObject.GetDescription(),
					myObject.GetName(), myObject.GetType(), myObject.GetSizeString());
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotalIndex += uIndex;
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "\tFailed because no Win32_TapeDrive object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
	}
	if (uTotalIndex > 0)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CWmi::GetSystemControllers(CSystemControllerList *pMyList)
{
	UINT			uIndex = 0,
					uTotalIndex = 0;
	CSystemController myObject;
	CString			csBuffer;

	ASSERT( pMyList);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	// Get Floppy controller
	AddLog( _T( "WMI GetSystemControllers: Trying to find Win32_FloppyController WMI objects...\n"));
	// Reset object list content
	pMyList->RemoveAll();
	try
	{
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_FloppyController")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				myObject.SetType( SYSTEM_CONTROLER_FLOPPY);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Manufacturer"));
				myObject.SetManufacturer( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Caption"));
				myObject.SetCaption( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
				myObject.SetDescription( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Name"));
				myObject.SetName( csBuffer);
				myObject.SetHardwareVersion( NOT_AVAILABLE);
				pMyList->AddTail( myObject);
				AddLog( _T( "\t\t<Manufacturer: %s><Caption: %s><Description: %s><Name: %s><Type: %s><Version: %s>\n"), 
					myObject.GetManufacturer(), myObject.GetCaption(), myObject.GetDescription(),
					myObject.GetName(), myObject.GetType(), myObject.GetHardwareVersion());
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotalIndex += uIndex;
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "\tFailed because no Win32_FloppyController object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
	}
	// Get IDE controller
	AddLog( _T( "WMI GetSystemControllers: Trying to find Win32_IDEController WMI objects...\n"));
	try
	{
		uIndex = 0;
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_IDEController")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				myObject.SetType( SYSTEM_CONTROLER_IDE);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Manufacturer"));
				myObject.SetManufacturer( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Caption"));
				myObject.SetCaption( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
				myObject.SetDescription( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Name"));
				myObject.SetName( csBuffer);
				myObject.SetHardwareVersion( NOT_AVAILABLE);
				pMyList->AddTail( myObject);
				AddLog( _T( "\t\t<Manufacturer: %s><Caption: %s><Description: %s><Name: %s><Type: %s><Version: %s>\n"), 
					myObject.GetManufacturer(), myObject.GetCaption(), myObject.GetDescription(),
					myObject.GetName(), myObject.GetType(), myObject.GetHardwareVersion());
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotalIndex += uIndex;
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "\tFailed because no Win32_IDEController object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
	}
	// Get SCSI controller
	AddLog( _T( "WMI GetSystemControllers: Trying to find Win32_SCSIController WMI objects...\n"));
	try
	{
		uIndex = 0;
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_SCSIController")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				myObject.SetType( SYSTEM_CONTROLER_SCSI);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Manufacturer"));
				myObject.SetManufacturer( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Caption"));
				myObject.SetCaption( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
				myObject.SetDescription( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Name"));
				myObject.SetName( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "HardwareVersion"));
				myObject.SetHardwareVersion( csBuffer);
				pMyList->AddTail( myObject);
				AddLog( _T( "\t\t<Manufacturer: %s><Caption: %s><Description: %s><Name: %s><Type: %s><Version: %s>\n"), 
					myObject.GetManufacturer(), myObject.GetCaption(), myObject.GetDescription(),
					myObject.GetName(), myObject.GetType(), myObject.GetHardwareVersion());
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotalIndex += uIndex;
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "\tFailed because no Win32_SCSIController object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
	}
	// Get USB controller
	AddLog( _T( "WMI GetSystemControllers: Trying to find Win32_InfraredDevice WMI objects...\n"));
	try
	{
		uIndex = 0;
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_InfraredDevice")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				myObject.SetType( SYSTEM_CONTROLER_INFRARED);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Manufacturer"));
				myObject.SetManufacturer( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Caption"));
				myObject.SetCaption( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
				myObject.SetDescription( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Name"));
				myObject.SetName( csBuffer);
				myObject.SetHardwareVersion( NOT_AVAILABLE);
				pMyList->AddTail( myObject);
				AddLog( _T( "\t\t<Manufacturer: %s><Caption: %s><Description: %s><Name: %s><Type: %s><Version: %s>\n"), 
					myObject.GetManufacturer(), myObject.GetCaption(), myObject.GetDescription(),
					myObject.GetName(), myObject.GetType(), myObject.GetHardwareVersion());
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotalIndex += uIndex;
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "\tFailed because no Win32_InfraredDevice object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
	}
	// Get USB controller
	AddLog( _T( "WMI GetSystemControllers: Trying to find Win32_USBController WMI objects...\n"));
	try
	{
		uIndex = 0;
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_USBController")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				myObject.SetType( SYSTEM_CONTROLER_USB);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Manufacturer"));
				myObject.SetManufacturer( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Caption"));
				myObject.SetCaption( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
				myObject.SetDescription( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Name"));
				myObject.SetName( csBuffer);
				myObject.SetHardwareVersion( NOT_AVAILABLE);
				pMyList->AddTail( myObject);
				AddLog( _T( "\t\t<Manufacturer: %s><Caption: %s><Description: %s><Name: %s><Type: %s><Version: %s>\n"), 
					myObject.GetManufacturer(), myObject.GetCaption(), myObject.GetDescription(),
					myObject.GetName(), myObject.GetType(), myObject.GetHardwareVersion());
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotalIndex += uIndex;
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "\tFailed because no Win32_USBController object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
	}
	// Get IEEE1394 controller
	AddLog( _T( "WMI GetSystemControllers: Trying to find Win32_1394Controller WMI objects...\n"));
	try
	{
		uIndex = 0;
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_1394Controller")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				myObject.SetType( SYSTEM_CONTROLER_IEEE1394);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Manufacturer"));
				myObject.SetManufacturer( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Caption"));
				myObject.SetCaption( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
				myObject.SetDescription( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Name"));
				myObject.SetName( csBuffer);
				myObject.SetHardwareVersion( NOT_AVAILABLE);
				pMyList->AddTail( myObject);
				AddLog( _T( "\t\t<Manufacturer: %s><Caption: %s><Description: %s><Name: %s><Type: %s><Version: %s>\n"), 
					myObject.GetManufacturer(), myObject.GetCaption(), myObject.GetDescription(),
					myObject.GetName(), myObject.GetType(), myObject.GetHardwareVersion());
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotalIndex += uIndex;
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "\tFailed because no Win32_1394Controller object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
	}
	// Get PCMCIA controller
	AddLog( _T( "WMI GetSystemControllers: Trying to find Win32_PCMCIAController WMI objects...\n"));
	try
	{
		uIndex = 0;
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_PCMCIAController")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				myObject.SetType( SYSTEM_CONTROLER_PCMCIA);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Manufacturer"));
				myObject.SetManufacturer( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Caption"));
				myObject.SetCaption( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
				myObject.SetDescription( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Name"));
				myObject.SetName( csBuffer);
				myObject.SetHardwareVersion( NOT_AVAILABLE);
				pMyList->AddTail( myObject);
				AddLog( _T( "\t\t<Manufacturer: %s><Caption: %s><Description: %s><Name: %s><Type: %s><Version: %s>\n"), 
					myObject.GetManufacturer(), myObject.GetCaption(), myObject.GetDescription(),
					myObject.GetName(), myObject.GetType(), myObject.GetHardwareVersion());
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotalIndex += uIndex;
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "\tFailed because no Win32_PCMCIAController object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
	}
	if (uTotalIndex > 0)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CWmi::GetVideoAdapters(CVideoAdapterList *pMyList)
{
	ASSERT( pMyList);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetVideoAdapters: Trying to find Win32_VideoController WMI objects...\n"));
	// Reset object list content
	pMyList->RemoveAll();
	try
	{
		UINT			uIndex = 0;
		CVideoAdapter	myObject;
		CString			csBuffer;
		unsigned __int64 u64HorzRes,
						 u64VertRes;

		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_VideoController")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
				myObject.SetName( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "VideoProcessor"));
				myObject.SetChipset( csBuffer);
				csBuffer.Format( _T( "%lu"), m_dllWMI.GetClassObjectDwordValue( _T( "AdapterRAM")) / ONE_MEGABYTE);
				myObject.SetMemory( csBuffer);
				u64HorzRes = m_dllWMI.GetClassObjectU64Value( _T( "CurrentHorizontalResolution"));
				u64VertRes = m_dllWMI.GetClassObjectU64Value( _T( "CurrentVerticalResolution"));
				csBuffer.Format( _T("%I64u x %I64u"), u64HorzRes, u64VertRes);
				myObject.SetScreenResolution( csBuffer);
				pMyList->AddTail( myObject);
				AddLog( _T( "\t\t<Description: %s><VideoProcessor: %s><Memory: %s><Resolution: %s>\n"), 
					myObject.GetName(), myObject.GetChipset(), myObject.GetMemory(), myObject.GetScreenResolution());
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "\tFailed because no Win32_VideoController object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
		return FALSE;
	}
}

BOOL CWmi::GetNetworkAdapters(CNetworkAdapterList *pMyList)
{
	ASSERT( pMyList);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetNetworkAdapters: Trying to find Win32_NetworkAdapterSetting WMI objects..."));
	// Reset object list content
	pMyList->RemoveAll();
	try
	{
		CString			csBuffer,
						csMacAddress;
		UINT			uIndex = 0;
		LONG			lIfIndex;
		CNetworkAdapter	myObject;

		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_NetworkAdapterSetting")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				lIfIndex = m_dllWMI.GetRefElementClassObjectDwordValue( _T( "Element"), _T( "InterfaceIndex"));
				myObject.SetIfIndex( lIfIndex);
				csBuffer = m_dllWMI.GetRefElementClassObjectStringValue( _T( "Element"), _T( "AdapterType"));
				myObject.SetType( csBuffer);
				csBuffer = m_dllWMI.GetRefElementClassObjectStringValue( _T( "Element"), _T( "ProductName"));
				myObject.SetDescription( csBuffer);
				csBuffer = m_dllWMI.GetRefElementClassObjectStringValue( _T( "Element"), _T( "Speed"));
				myObject.SetSpeed( csBuffer);
				csMacAddress = m_dllWMI.GetRefElementClassObjectStringValue( _T( "Setting"), _T( "MACAddress"));
				myObject.SetMACAddress( csMacAddress);
				csBuffer = m_dllWMI.GetRefElementClassObjectStringValue( _T( "Setting"), _T( "IPAddress"));
				myObject.SetIPAddress( csBuffer);
				csBuffer = m_dllWMI.GetRefElementClassObjectStringValue( _T( "Setting"), _T( "IPSubnet"));
				myObject.SetIPNetMask( csBuffer);
				csBuffer = m_dllWMI.GetRefElementClassObjectStringValue( _T( "Setting"), _T( "DefaultIPGateway"));
				myObject.SetGateway( csBuffer);
				csBuffer = m_dllWMI.GetRefElementClassObjectStringValue( _T( "Setting"), _T( "DHCPServer"));
				myObject.SetDhcpServer( csBuffer);
				csBuffer = m_dllWMI.GetRefElementClassObjectStringValue(_T("Setting"), _T("MTU"));
				myObject.SetMtu(csBuffer);
				csBuffer = m_dllWMI.GetRefElementClassObjectStringValue( _T( "Element"), _T( "Status"));
				myObject.SetStatus( csBuffer);
				csBuffer = m_dllWMI.GetRefElementClassObjectStringValue( _T( "Element"), _T( "AdapterType"));
				myObject.SetTypeMIB( csBuffer);
				AddLog( _T( "\t\t<IfIndex: %ld><Type: %s><Description: %s><Speed: %s><MAC: %s><IP Address: %s><IP Subnet: %s><Gateway: %s><DHCP: %s><Status: %s><TypeMIB: %s><MTU: %s>\n"), 
					myObject.GetIfIndex(), myObject.GetType(), myObject.GetDescription(), myObject.GetSpeed(),
					myObject.GetMACAddress(), myObject.GetIPAddress(), myObject.GetIPNetMask(), myObject.GetGateway(), 
					myObject.GetDhcpServer(), myObject.GetOperationalStatus(), myObject.GetTypeMIB(), myObject.GetMtu());
				if (!csMacAddress.IsEmpty())
				{
					// Skip adapters without MAC Address
					pMyList->AddTail( myObject);
					uIndex ++;
				}
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "\tFailed because no Win32_NetworkAdapterSetting object with valid MAC Address !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
		return FALSE;
	}
}

BOOL CWmi::GetMonitors(CMonitorList *pMyList)
{
	ASSERT( pMyList);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetMonitors: Trying to find Win32_DesktopMonitor WMI objects...\n"));
	// Reset object list content
	pMyList->RemoveAll();
	try
	{
		UINT		uIndex = 0;
		CMonitor	myObject;
		CString		csBuffer;

		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_DesktopMonitor")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "MonitorManufacturer"));
				myObject.SetManufacturer( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
				myObject.SetDescription( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Caption"));
				myObject.SetCaption( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "MonitorType"));
				myObject.SetType( csBuffer);
				pMyList->AddTail( myObject);
				AddLog( _T( "\t\t<Manufacturer: %s><Caption: %s><Description: %s><Type: %s>\n"),  
					myObject.GetManufacturer(), myObject.GetCaption(), myObject.GetDescription(),
					myObject.GetType());
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "\tFailed because no Win32_DesktopMonitor object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
		return FALSE;
	}
}

BOOL CWmi::GetModems(CModemList *pMyList)
{
	ASSERT( pMyList);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetModems: Trying to find Win32_POTSModem WMI objects...\n"));
	// Reset object list content
	pMyList->RemoveAll();
	try
	{
		UINT	uIndex = 0;
		CModem	myObject;
		CString	csBuffer;

		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_POTSModem")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "DeviceType"));
				myObject.SetType( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Name"));
				myObject.SetName( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Model"));
				myObject.SetModel( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
				myObject.SetDescription( csBuffer);
				if (IsDeviceReallyConnected( m_dllWMI.GetClassObjectDwordValue( _T( "StatusInfo"))))
				{
					pMyList->AddTail( myObject);
					uIndex ++;
				}
				AddLog( _T( "\t\t<Type: %s><Name: %s><Model: %s><Description: %s><StatusInfo: %u>\n"), 
					myObject.GetType(), myObject.GetName(), myObject.GetModel(),
					myObject.GetDescription(), m_dllWMI.GetClassObjectDwordValue( _T( "StatusInfo")));
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "\tFailed because no Win32_POTSModem object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
		return FALSE;
	}
}

BOOL CWmi::GetSoundDevices(CSoundDeviceList *pMyList)
{
	ASSERT( pMyList);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetSoundDevices: Trying to find Win32_SoundDevice WMI objects...\n"));
	// Reset object list content
	pMyList->RemoveAll();
	try
	{
		CSoundDevice	myObject;
		UINT			uIndex = 0;
		CString			csBuffer;

		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_SoundDevice")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Manufacturer"));
				myObject.SetManufacturer( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "ProductName"));
				myObject.SetName( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
				myObject.SetDescription( csBuffer);
				if (IsDeviceReallyConnected( m_dllWMI.GetClassObjectDwordValue( _T( "StatusInfo"))))
				{
					pMyList->AddTail( myObject);
					uIndex ++;
				}
				AddLog( _T( "\t\t<Manufacturer: %s><Name: %s><Description: %s><StatusInfo: %u>\n"), 
					myObject.GetManufacturer(), myObject.GetName(), myObject.GetDescription(), m_dllWMI.GetClassObjectDwordValue( _T( "StatusInfo")));
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "\tFailed because no Win32_SoundDevice object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
		return FALSE;
	}
}

BOOL CWmi::GetPrinters(CPrinterList *pMyList)
{
	ASSERT( pMyList);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetPrinters: Trying to find Win32_Printer WMI objects...\n"));
	// Reset object list content
	pMyList->RemoveAll();
	try
	{
		UINT		uIndex = 0;
		CPrinter	myObject;
		CString		csBuffer;

		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_Printer")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Name"));
				myObject.SetName( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "DriverName"));
				myObject.SetDriver( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "PortName"));
				myObject.SetPort( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "ServerName"));
				myObject.SetServerName( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "ShareName"));
				myObject.SetShareName( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "HorizontalResolution"));
				if (!csBuffer.IsEmpty())
				{
					csBuffer.AppendFormat( _T( " x %s"), m_dllWMI.GetClassObjectStringValue( _T( "VerticalResolution")));
					myObject.SetResolution( csBuffer);
				}
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Comment"));
				myObject.SetComment( csBuffer);
				myObject.SetShared( m_dllWMI.GetClassObjectDwordValue( _T( "Shared")));
				myObject.SetNetworkPrinter( m_dllWMI.GetClassObjectDwordValue( _T( "Network")));
				pMyList->AddTail( myObject);
				AddLog( _T( "\t\t<Name: %s><Driver: %s><Port: %s><Server: %s><Share: %s><Resolution: %s><Comment: %s><Shared: %s><Network: %s>\n"), 
					myObject.GetName(), myObject.GetDriver(), myObject.GetPort(), myObject.GetServerName(), myObject.GetShareName(), 
					myObject.GetResolution(), myObject.GetComment(), myObject.IsShared()?_T("Yes"):_T("No"), myObject.IsNetworkPrinter()?_T("Yes"):_T("No"));
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "\tFailed because no Win32_Printer object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
		return FALSE;
	}
}

BOOL CWmi::GetSystemPorts(CSystemPortList *pMyList)
{
	UINT		uIndex,
				uTotal = 0;
	CSystemPort	myObject;
	CString		csBuffer;
	DWORD		dwValue;
	int			nIndex;

	ASSERT( pMyList);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	// Get Connector ports
	AddLog( _T( "WMI GetSystemPorts: Trying to find Win32_PortConnector WMI objects...\n"));
	try
	{
		uIndex = 0;
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_PortConnector")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "ExternalReferenceDesignator"));
				myObject.SetName( csBuffer);
				dwValue = m_dllWMI.GetClassObjectDwordValue( _T( "PortType"));
				myObject.SetType( dwValue);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "ConnectorType"));
				nIndex = csBuffer.Find( _T( ";"));
				if (nIndex > 0)
					// This is multi-valued 
					dwValue = _ttol( csBuffer.Left( nIndex));
				else
					// Single value
					dwValue = _ttol( csBuffer);
				myObject.SetConnectorType( dwValue);
				csBuffer.Format( _T( "%s %s"), myObject.GetName(), myObject.GetDescription());
				myObject.SetCaption( csBuffer);
				pMyList->AddTail( myObject);
				AddLog( _T( "\t\t<Name: %s><Type: %s><Caption: %s><Description: %s>\n"), 
					myObject.GetName(), myObject.GetType(), myObject.GetCaption(), myObject.GetDescription());
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotal += uIndex;
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "\tFailed because no Win32_PortConnector object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
	}
	if (uTotal == 0)
	{
		// No connector port object, try serial and parallel
		// Get serial ports
		AddLog( _T( "WMI GetSystemPorts: Trying to find Win32_SerialPort WMI objects...\n"));
		// Reset object list content
		pMyList->RemoveAll();
		try
		{
			uIndex = 0;
			if (m_dllWMI.BeginEnumClassObject( _T( "Win32_SerialPort")))
			{
				while (m_dllWMI.MoveNextEnumClassObject())
				{
					myObject.Clear();
					myObject.SetType( SYSTEM_PORT_SERIAL);
					csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Name"));
					myObject.SetName( csBuffer);
					csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Caption"));
					myObject.SetCaption( csBuffer);
					csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
					myObject.SetDescription( csBuffer);
					if (IsDeviceReallyConnected( m_dllWMI.GetClassObjectDwordValue( _T( "StatusInfo"))))
					{
						pMyList->AddTail( myObject);
						uIndex ++;
					}
					AddLog( _T( "\t\t<Name: %s><Type: %s><Caption: %s><Description: %s>\n"), 
						myObject.GetName(), myObject.GetType(), myObject.GetCaption(), myObject.GetDescription());
				}
				m_dllWMI.CloseEnumClassObject();
			}
			if (uIndex > 0)
			{
				uTotal += uIndex;
				AddLog( _T( "\tOK (%u objects)\n"), uIndex);
			}
			else
				AddLog( _T( "\tFailed because no Win32_SerialPort object !\n"));
		}
		catch (CException *pEx)
		{
			pEx->Delete();
			AddLog( _T( "\tFailed because unknown exception !\n"));
		}
		// Get parallel ports
		AddLog( _T( "WMI GetSystemPorts: Trying to find Win32_ParallelPort WMI objects...\n"));
		try
		{
			uIndex = 0;
			if (m_dllWMI.BeginEnumClassObject( _T( "Win32_ParallelPort")))
			{
				while (m_dllWMI.MoveNextEnumClassObject())
				{
					myObject.Clear();
					myObject.SetType( SYSTEM_PORT_PARALLEL);
					csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Name"));
					myObject.SetName( csBuffer);
					csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Caption"));
					myObject.SetCaption( csBuffer);
					csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
					myObject.SetDescription( csBuffer);
					if (IsDeviceReallyConnected( m_dllWMI.GetClassObjectDwordValue( _T( "StatusInfo"))))
					{
						pMyList->AddTail( myObject);
						uIndex ++;
					}
					AddLog( _T( "\t\t<Name: %s><Type: %s><Caption: %s><Description: %s>\n"), 
						myObject.GetName(), myObject.GetType(), myObject.GetCaption(), myObject.GetDescription());
				}
				m_dllWMI.CloseEnumClassObject();
			}
			if (uIndex > 0)
			{
				uTotal += uIndex;
				AddLog( _T( "\tOK (%u objects)\n"), uIndex);
			}
			else
				AddLog( _T( "\tFailed because no Win32_ParallelPort object !\n"));
		}
		catch (CException *pEx)
		{
			pEx->Delete();
			AddLog( _T( "\tFailed because unknown exception !\n"));
		}
	}
	if (uTotal > 0)
	{
		return TRUE;
	}
	return FALSE;
}

DWORD CWmi::GetProcessors(CString &csProcType, CString &csProcSpeed)
{
	static DWORD	dwNumber;
	DWORD			dwArch,
					dwCore;
	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetProcessors: Trying to find Win32_Processor WMI objects...\n"));
	dwNumber = 0;
	try
	{
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_Processor")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				csProcSpeed = m_dllWMI.GetClassObjectStringValue( _T( "MaxClockSpeed"));
				StrForSQL( csProcSpeed);
				csProcType = m_dllWMI.GetClassObjectStringValue( _T( "Name"));
				dwArch = m_dllWMI.GetClassObjectDwordValue( _T( "Architecture"));
				dwCore = m_dllWMI.GetClassObjectDwordValue( _T( "NumberOfCores"));
				// If value NumberOfCores is not available, assume at least 1 core
				if (dwCore == 0)
					dwCore = 1;
				csProcType.AppendFormat( _T( " [%lu cores %s]"), dwCore, GetArchitecture( dwArch));
				StrForSQL( csProcType);
				dwNumber ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (dwNumber > 0)
		{
			AddLog( _T( "\tOK (%lu x %s at %s)\n"), dwNumber, csProcType, csProcSpeed);
			return dwNumber;
		}
		AddLog( _T( "\tFailed because no Win32_Processor object !\n"));
		return 0;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
		return 0;
	}
}

DWORD CWmi::GetCPU( CCpuList *pMyList, CRegistry *pReg)
{
	static DWORD	dwNumber;
	DWORD			dwValue,
					dwNumberOfLogicalCPU = 0;
	CString         csValue;
	CCpu            myObject;

	ASSERT( pMyList);
	ASSERT( pReg);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetCPU: Trying to find Win32_Processor WMI objects...\n"));
	// Reset object list content
	pMyList->RemoveAll();
	dwNumber = 0;

	try
	{
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_Processor")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				csValue = m_dllWMI.GetClassObjectStringValue( _T( "SocketDesignation"));
				myObject.SetSocket( csValue);
				csValue = m_dllWMI.GetClassObjectStringValue( _T( "Manufacturer"));
				myObject.SetManufacturer( csValue);
				csValue = m_dllWMI.GetClassObjectStringValue( _T( "Name"));
				if (csValue.Find( _T( "Intel Pentium III Xeon")) != -1)
				{
					// Bogus WMI, use registry for Processor name
					AddLog( _T( "\tBogus CPU name <%s> detected => Using "), csValue);
					pReg->GetCPUName( dwNumberOfLogicalCPU, csValue);
				}
				myObject.SetName( csValue);
				dwValue = m_dllWMI.GetClassObjectDwordValue( _T( "Architecture"));
				myObject.SetArchitecture( GetArchitecture( dwValue));
				if ((dwValue = m_dllWMI.GetClassObjectDwordValue( _T( "NumberOfCores"))) == 0)
					// If value NumberOfCores is not available, assume at least 1 core
					dwValue = 1;
				myObject.SetNumberOfCores( dwValue);
				if ((dwValue = m_dllWMI.GetClassObjectDwordValue( _T( "NumberOfLogicalProcessors"))) == 0)
					// If value NumberOfLogicalProcessors is not available, assume at least 1 logical processor
					dwValue = 1;
				dwNumberOfLogicalCPU += dwValue;
				myObject.SetNumberOfLogicalProcessors( dwValue);
				dwValue = m_dllWMI.GetClassObjectDwordValue( _T( "CurrentClockSpeed"));
				myObject.SetCurrentSpeed( dwValue);
				dwValue = m_dllWMI.GetClassObjectDwordValue( _T( "MaxClockSpeed"));
				myObject.SetMaxSpeed( dwValue);
				dwValue = m_dllWMI.GetClassObjectDwordValue( _T( "L2CacheSize"));
				myObject.SetL2CacheSize( dwValue);
				dwValue = m_dllWMI.GetClassObjectDwordValue( _T( "AddressWidth"));
				myObject.SetAddressWith( dwValue);
				dwValue = m_dllWMI.GetClassObjectDwordValue( _T( "DataWidth"));
				myObject.SetDataWidth( dwValue);
				dwValue = m_dllWMI.GetClassObjectDwordValue( _T( "VoltageCaps"));
				myObject.SetVoltage( GetVoltage( dwValue));
				dwValue = m_dllWMI.GetClassObjectDwordValue( _T( "CpuStatus"));
				myObject.SetStatus( GetCpuStatus( dwValue));
				csValue.Empty();
				csValue = m_dllWMI.GetClassObjectStringValue( _T( "ProcessorId"));
				if (csValue.IsEmpty())
				{
					// Try UniqueId instead of ProcessorId
					csValue = m_dllWMI.GetClassObjectStringValue( _T( "UniqueId"));
				}
				myObject.SetSN( csValue);
				// Device is OK
				pMyList->AddTail( myObject);
				AddLog( _T( "\t\t<Manufacturer: %s><Name: %s><Socket: %s><Architecture: %s><NumberOfCores: %u><NumberOfLogicalProcessors: %u><CurrentClockSpeed: %u><MaxClockSpeed: %u><L2CacheSize: %u><AddressWidth: %u><DataWidth: %u><VoltageCaps: %s><CpuStatus: %s>\n"), 
					myObject.GetManufacturer(), myObject.GetName(), myObject.GetSocket(), myObject.GetArchitecture(),
					myObject.GetNumberOfCores(), myObject.GetNumberOfLogicalProcessors(), myObject.GetCurrentClockSpeed(),
					myObject.GetMaxClockSpeed(), myObject.GetL2CacheSize(), myObject.GetAddressWidth(), myObject.GetDataWidth(),
					myObject.GetVoltage(), myObject.GetStatus());
				dwNumber ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (dwNumber > 0)
		{
			AddLog( _T( "\tOK (%u objects)\n"), dwNumber);
			return dwNumber;
		}
		AddLog( _T( "\tFailed because no Win32_Processor object !\n"));
		return 0;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
		return 0;
	}
}

BOOL CWmi::GetOS(CString &csName, CString &csVersion, CString &csComment, CString &csDescription, CString &csInstallDate)
{
	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetOS: Trying to find Win32_OperatingSystem WMI objects...\n"));
	try
	{
		UINT	uIndex = 0;

		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_OperatingSystem")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				csName = m_dllWMI.GetClassObjectStringValue( _T( "Name"));
				csName = csName.Left( csName.Find( _T( "|"), 0));
				StrForSQL( csName);
				csVersion = m_dllWMI.GetClassObjectStringValue( _T( "Version"));
				StrForSQL( csVersion);
				csComment = m_dllWMI.GetClassObjectStringValue( _T( "CSDVersion"));
				StrForSQL( csComment);
				csDescription = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
				StrForSQL( csDescription);
				csInstallDate = m_dllWMI.GetClassObjectStringValue( _T( "InstallDate"));
				csInstallDate.Truncate( 8);
				StrForSQL( csInstallDate);
				AddLog( _T( "\t\t<OS: %s %s %s><Comments: %s>\n"), csName, csVersion, csComment, csDescription);
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "\tFailed because no Win32_OperatingSystem object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
		return FALSE;
	}
}

DWORD CWmi::GetAddressWidthOS()
{
	static DWORD	dwBits;
	DWORD			dwNumber = 0;
	// If not WMI connected => cannot do this
	if (!m_bConnected)
		// If value AddressWidth is not available, assume 32 bits
		return 32;

	AddLog( _T( "WMI GetAddressWidthOS: Trying to find Win32_Processor WMI objects...\n"));
	try
	{
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_Processor")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				dwBits = m_dllWMI.GetClassObjectDwordValue( _T( "AddressWidth"));
				// If value AddressWidth is not available, assume 32 bits
				if (dwBits == 0)
					dwBits = 32;
				AddLog( _T( "\t\tProcessor uses %u bits AddressWidth\n"), dwBits);
				dwNumber ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (dwNumber > 0)
			AddLog( _T( "\tOK (%u objects)\n"), dwNumber);
		else
		{
			AddLog( _T( "\tFailed because no Win32_Processor object, assuming 32 bits OS !\n"));
			dwBits = 32;
		}
		return dwBits;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception, assuming 32 bits OS !\n"));
		dwBits = 32;
		return dwBits;
	}
}


BOOL CWmi::GetWindowsRegistration( CString &csCompany, CString &csUser, CString &csSN)
{
	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetWindowsRegistration: Trying to find Win32_OperatingSystem WMI objects...\n"));
	try
	{
		UINT	uIndex = 0;

		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_OperatingSystem")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				csCompany = m_dllWMI.GetClassObjectStringValue( _T( "Organization"));
				StrForSQL( csCompany);
				csUser = m_dllWMI.GetClassObjectStringValue( _T( "RegisteredUser"));
				StrForSQL( csUser);
				csSN = m_dllWMI.GetClassObjectStringValue( _T( "SerialNumber"));
				StrForSQL( csSN);
				AddLog( _T( "\t\t<Organization: %s><User: %s><S/N: %s>\n"), csCompany, csUser, csSN);
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "\tFailed because no Win32_OperatingSystem object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
		return FALSE;
	}
}

BOOL CWmi::ParseChassisType(CString &csType)
{
	int		nPos = 0;
	CString csResult,
			csTemp,
			csData;

	if (csType.IsEmpty())
	{
		csType = NOT_AVAILABLE;
		return FALSE;
	}
	csData = csType;
	while ((nPos = csData.Find( _T( ";"))) != -1)
	{
		csTemp = csData.Left( nPos);
		if (!csTemp.IsEmpty())
		{
			csResult += CBios::ParseChassisType( _ttoi( csTemp));
			csResult += _T( ";");
		}
		csTemp = csData.Right( nPos+1);
		csData = csTemp;
	}
	if (!csData.IsEmpty())
	{
		csResult += CBios::ParseChassisType( _ttoi( csData));
	}
	csType = csResult;
	return TRUE;
}

BOOL CWmi::GetDomainOrWorkgroup( CString &csDomain)
{
	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	// Try to use win32 computer system object to get Domain
	AddLog( _T( "WMI GetDomainOrWorkgroup: Trying to find Win32_ComputerSystem WMI objects...\n"));
	try
	{
		UINT	uIndex = 0;
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_ComputerSystem")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				csDomain = m_dllWMI.GetClassObjectStringValue( _T( "Domain"));
				AddLog( _T( "\t\t<Domain: %s>\n"), csDomain);
				uIndex++;

			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "\tFailed because no Win32_ComputerSystem object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
	}
	return FALSE;
}


BOOL CWmi::GetLoggedOnUser( CString &csUser)
{
	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	// Try to use win32 logon session system object to get interactive session
	AddLog( _T( "WMI GetLoggedOnUser: Trying to find Win32_LoggedOnUser WMI objects...\n"));
	try
	{
		UINT	uIndex = 0;
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_LoggedOnUser")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				if (m_dllWMI.GetRefElementClassObjectDwordValue( _T( "Dependent"), _T( "LogonType")) == 2)
				{
					// This is an interactive session
					csUser = m_dllWMI.GetRefElementClassObjectStringValue( _T( "Antecedent"), _T( "Name"));
					AddLog( _T( "\t\t<User: %s>\n"), csUser);
					uIndex++;
				}
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "\tFailed because no Win32_LoggedOnUser object with interactive session found !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
	}
	return FALSE;
}

BOOL CWmi::GetUserDomain( CString &csDomain)
{
	BOOL	bResult = FALSE;

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return bResult;

	// Try to use win32 logon session system object to get interactive session
	AddLog( _T( "WMI GetUserDomain: Trying to find Win32_LoggedOnUser WMI objects...\n"));
	try
	{
		UINT	uIndex = 0;
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_LoggedOnUser")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				if (m_dllWMI.GetRefElementClassObjectDwordValue( _T( "Dependent"), _T( "LogonType")) == 2)
				{
					// This is an interactive session
					csDomain = m_dllWMI.GetRefElementClassObjectStringValue( _T( "Antecedent"), _T( "Domain"));
					AddLog( _T( "\t\t<Domain: %s>\n"), csDomain);
					uIndex++;
				}
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "\tFailed because no Win32_LoggedOnUser object with interactive session found !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
	}
	return FALSE;
}

BOOL CWmi::GetSystemSlots(CSystemSlotList *pMyList)
{
	ASSERT( pMyList);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetSystemSlots: Trying to find Win32_SystemSlot WMI objects...\n"));
	// Reset object list content
	pMyList->RemoveAll();
	try
	{
		CSystemSlot	myObject;
		UINT		uIndex = 0;
		CString		csBuffer;
		DWORD		dwValue;

		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_SystemSlot")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Name"));
				myObject.SetName( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
				myObject.SetDescription( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "SlotDesignation"));
				myObject.SetSlotDesignation( csBuffer);
				// TODO Validate working on ALL compuer
				dwValue = m_dllWMI.GetClassObjectDwordValue( _T( "CurrentUsage"));
				myObject.SetUsage( dwValue);
				// End TODO
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Status"));
				myObject.SetStatus( csBuffer);
				dwValue = m_dllWMI.GetClassObjectDwordValue( _T( "Shared"));
				myObject.SetShared( dwValue);
				// Device is OK
				pMyList->AddTail( myObject);
				AddLog( _T( "\t\t<Name: %s><Description: %s><Slot: %s><Usage: %s><Shared: %s>\n"), 
					myObject.GetName(), myObject.GetDescription(), myObject.GetSlotDesignation(), myObject.GetUsage(),
					myObject.IsShared()?_T("Yes"):_T("No"));
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "\tFailed because no Win32_SystemSlot object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
		return FALSE;
	}
}

BOOL CWmi::GetLogicalDrives( CLogicalDriveList *pMyList)
{
	ASSERT( pMyList);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetLogicalDrives: Trying to find Win32_LogicalDisk WMI objects...\n"));
	// Reset object list content
	pMyList->RemoveAll();
	try
	{
		UINT	uIndex = 0;
		unsigned __int64 u64Size;
		DWORD	dwType;
		CLogicalDrive	myObject;
		CString	csBuffer;
		BOOL	bIsLocalDrive;

		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_LogicalDisk")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Caption"));
				myObject.SetDriveLetter( csBuffer);
				dwType = m_dllWMI.GetClassObjectDwordValue( _T( "DriveType"));
				switch (dwType)
				{
				case DRIVE_REMOVABLE:
					myObject.SetFileSystem( NOT_AVAILABLE);
					myObject.SetVolumName( NOT_AVAILABLE);
					bIsLocalDrive = FALSE;
					break;
				case DRIVE_FIXED:
					csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Filesystem"));
					myObject.SetFileSystem( csBuffer);
					csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "VolumeName"));
					myObject.SetVolumName( csBuffer);
					bIsLocalDrive = TRUE;
					break;
				case DRIVE_REMOTE:
					csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Filesystem"));
					myObject.SetFileSystem( csBuffer);
					csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "ProviderName"));
					myObject.SetVolumName( csBuffer);
					bIsLocalDrive = FALSE;
					break;
				case DRIVE_CDROM:
					myObject.SetFileSystem( NOT_AVAILABLE);
					myObject.SetVolumName( NOT_AVAILABLE);
					bIsLocalDrive = FALSE;
					break;
				case DRIVE_RAMDISK:
					csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Filesystem"));
					myObject.SetFileSystem( csBuffer);
					csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "VolumeName"));
					myObject.SetVolumName( csBuffer);
					bIsLocalDrive = TRUE;
					break;
				case DRIVE_NO_ROOT_DIR:
				case DRIVE_UNKNOWN:
				default:
					myObject.SetFileSystem( NOT_AVAILABLE);
					myObject.SetVolumName( NOT_AVAILABLE);
					bIsLocalDrive = FALSE;
					break;
				}
				myObject.SetType( dwType);
				if (bIsLocalDrive)
				{
					u64Size = m_dllWMI.GetClassObjectU64Value( _T( "Size"));
					myObject.SetTotalSize( u64Size/ONE_MEGABYTE);
					u64Size = m_dllWMI.GetClassObjectU64Value( _T( "FreeSpace"));
					myObject.SetFreeSpace( u64Size/ONE_MEGABYTE);
				}
				else
				{
					myObject.SetTotalSize( 0);
					myObject.SetFreeSpace( 0);
				}

				pMyList->AddTail( myObject);
				AddLog( _T( "\t\t<Drive: %s><Type: %s><Filesystem: %s><Volum Name: %s><size: %ld MB><Free Space: %ld MB>\n"), 
					myObject.GetDriveLetter(), myObject.GetDriveType(), myObject.GetFileSystem(), myObject.GetVolumName(),
					myObject.GetTotalMB(), myObject.GetFreeMB());
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "\tFailed because no Win32_LogicalDisk object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
		return FALSE;
	}
}

BOOL CWmi::GetMemorySlots(CMemorySlotList *pMyList)
{
	ASSERT( pMyList);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetMemorySlots: Trying to find Win32_PhysicalMemory WMI objects...\n"));
	// Reset object list content
	pMyList->RemoveAll();
	try
	{
		CMemorySlot			myObject;
		UINT				uIndex = 0;
		CString				csBuffer, csCaption;
		DWORD				dwValue,
							dwUse,
							dwECC;
		unsigned __int64	u64Value;
		POSITION			pPosCur,
							pPosNext;

		// Checking Physical Memory objects for properties
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_PhysicalMemory")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Caption"));
				if (csBuffer.IsEmpty())
					csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
				myObject.SetCaption( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "BankLabel"));
				if (csBuffer.IsEmpty())
					csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "DeviceLocator"));
				myObject.SetDescription( csBuffer);
				u64Value = m_dllWMI.GetClassObjectU64Value(  _T( "Capacity"));
				csBuffer.Format( _T( "%I64u"), u64Value / ONE_MEGABYTE);
				myObject.SetCapacity( csBuffer);
				myObject.SetSlotNumber( uIndex+1);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Speed"));
				myObject.SetSpeed( csBuffer);
				dwValue = m_dllWMI.GetClassObjectDwordValue( _T( "MemoryType"));
				if (u64Value > 0)
					myObject.SetType( GetMemorySlotType( dwValue));
				else
					myObject.SetType( _T("Empty slot"));
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "SerialNumber"));
/*				// If S/N is empty, get Part number
				if (csBuffer.IsEmpty())
					csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "PartNumber"));
*/
				if ((u64Value > 0) && myObject.IsValidSN( csBuffer))
					myObject.SetSN( csBuffer);
				// Device is OK
				pMyList->AddTail( myObject);
				AddLog( _T( "\t\t<Caption: %s><Description: %s><Slot: %u><Capacity: %s><Speed: %s><Type: %s><S/N: %s>\n"), 
					myObject.GetCaption(), myObject.GetDescription(), myObject.GetSlotNumber(), myObject.GetCapacity(),
					myObject.GetSpeed(), myObject.GetType(), myObject.GetSN());
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex == 0)
		{
			AddLog( _T( "\tFailed because no Win32_PhysicalMemory object !\n"));
			return FALSE;
		}
		AddLog( _T( "\tOK (%u objects)\n"), uIndex);
		// Checking Physical Memory Array objects for completing properties
		AddLog( _T( "WMI GetMemorySlots: Trying to find Win32_PhysicalMemoryArray WMI objects...\n"));
		pPosNext = pMyList->GetHeadPosition();
		pPosCur = pMyList->GetHeadPosition();
		UINT nbFilled = uIndex;
		uIndex = 0;
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_PhysicalMemoryArray")))
		{
			// One Physical Memory Array object may include one or more Physical Memory objects
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				// The value MemoryDevices indicates the number of Physical Memory object in the array
				dwValue = m_dllWMI.GetClassObjectDwordValue( _T( "MemoryDevices"));
				while( nbFilled < dwValue ) {
					myObject.SetCaption( _T(""));
					myObject.SetDescription( _T(""));
					myObject.SetCapacity( _T("0"));
					myObject.SetSlotNumber( nbFilled+1 );
					myObject.SetSpeed( _T(""));
					myObject.SetType( _T("Empty slot"));
					pMyList->AddTail( myObject);
					nbFilled++;
				}
				dwUse = m_dllWMI.GetClassObjectDwordValue( _T( "Use"));
				dwECC = m_dllWMI.GetClassObjectDwordValue( _T( "MemoryErrorCorrection"));
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
				csCaption = m_dllWMI.GetClassObjectStringValue( _T( "Caption"));

				// Each properties of Physical Memory Array have to be set in MemorySlot object 
				// corresponding to current array
				if (pPosNext != NULL)
					myObject = pMyList->GetNext( pPosNext);
				while ((pPosCur != NULL) && (myObject.GetSlotNumber() <= (uIndex+dwValue)))
				{
					if (_tcslen( myObject.GetCaption()) == 0)
						myObject.SetCaption( csCaption);
					if (_tcslen( myObject.GetDescription()) == 0)
						myObject.SetDescription( csBuffer );					
					
					if (_tcslen( myObject.GetCaption()) == 0)
						myObject.SetCaption( csBuffer );
					if (_tcslen( myObject.GetDescription()) == 0)
						myObject.SetDescription( csCaption );

					myObject.SetUsage( dwUse);
					if( _tcscmp( myObject.GetType(), _T("Empty slot")) != 0 )
						myObject.SetTypeECC( dwECC);
					pMyList->SetAt( pPosCur, myObject);
					pPosCur = pPosNext;
					if (pPosNext != NULL)
						myObject = pMyList->GetNext( pPosNext);
				}
				uIndex += dwValue;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		AddLog( _T( "\tOK (%u objects updated)\n"), uIndex);
		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
		return FALSE;
	}
}

LPCTSTR CWmi::GetMemorySlotType( DWORD dwType)
{
	switch (dwType)
	{
	case 0:	return _T( "Unknown");
	case 1: return _T( "Other");
	case 2: return _T( "DRAM");
	case 3: return _T( "Synchronous DRAM");
	case 4: return _T( "Cache DRAM");
	case 5: return _T( "EDO");
	case 6: return _T( "EDRAM");
	case 7: return _T( "VRAM");
	case 8: return _T( "SRAM");
	case 9: return _T( "RAM");
	case 10: return _T( "ROM");
	case 11: return _T( "Flash");
	case 12: return _T( "EEPROM");
	case 13: return _T( "FEPROM");
	case 14: return _T( "EPROM");
	case 15: return _T( "CDRAM");
	case 16: return _T( "3DRAM");
	case 17: return _T( "SDRAM");
	case 18: return _T( "SGRAM");
	case 19: return _T( "RDRAM");
	case 20: return _T( "DDR");
	case 21: return _T( "DDR-2");
	default:
		return NOT_AVAILABLE;
	}
}

BOOL CWmi::GetInputDevices(CInputDeviceList *pMyList)
{
	UINT		uIndex,
				uTotal = 0;
	CInputDevice myObject;
	CString		csBuffer;
	DWORD		dwValue;

	ASSERT( pMyList);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	// Get keyboard
	AddLog( _T( "WMI GetInputDevices: Trying to find Win32_Keyboard WMI objects...\n"));
	// Reset object list content
	pMyList->RemoveAll();
	try
	{
		uIndex = 0;
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_Keyboard")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				myObject.SetType( INPUT_DEVICE_KEYBOARD);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Manufacturer"));
				myObject.SetManufacturer( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Caption"));
				myObject.SetCaption( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
				myObject.SetDescription( csBuffer);
				myObject.SetPointingType( NOT_AVAILABLE);
				myObject.SetPointingInterface( NOT_AVAILABLE);
				pMyList->AddTail( myObject);
				AddLog( _T( "\t\t<Type: %s><Manufacturer: %s><Caption: %s><Description: %s><Pointing Type: %s><Interface: %s>\n"), 
					myObject.GetType(), myObject.GetManufacturer(), myObject.GetCaption(), myObject.GetDescription(),
					myObject.GetPointingType(), myObject.GetPointingInterface());
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotal += uIndex;
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "\tFailed because no Win32_Keyboard object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
	}
	// Get pointing devices
	AddLog( _T( "WMI GetInputDevices: Trying to find Win32_PointingDevice WMI objects...\n"));
	try
	{
		uIndex = 0;
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_PointingDevice")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				myObject.SetType( INPUT_DEVICE_POINTING);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Manufacturer"));
				myObject.SetManufacturer( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Caption"));
				myObject.SetCaption( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
				myObject.SetDescription( csBuffer);
				dwValue = m_dllWMI.GetClassObjectDwordValue( _T( "PointingType"));
				myObject.SetPointingType( dwValue);
				dwValue = m_dllWMI.GetClassObjectDwordValue( _T( "DeviceInterface"));
				myObject.SetPointingInterface( dwValue);
				pMyList->AddTail( myObject);
				AddLog( _T( "\t\t<Type: %s><Manufacturer: %s><Caption: %s><Description: %s><Pointing Type: %s><Interface: %s>\n"), 
					myObject.GetType(), myObject.GetManufacturer(), myObject.GetCaption(), myObject.GetDescription(),
					myObject.GetPointingType(), myObject.GetPointingInterface());
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotal += uIndex;
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "\tFailed because no Win32_PointingDevice object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
	}
	// Get smartcard reader
	AddLog( _T( "WMI GetInputDevices: Trying to find Win32_PnPSignedDriver WMI objects for Smartcard Reader...\n"));
	try
	{
		uIndex = 0;
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_PnPSignedDriver")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Manufacturer"));
				myObject.SetManufacturer( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "DeviceName"));
				myObject.SetCaption( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
				myObject.SetDescription( csBuffer);
				myObject.SetPointingType( NOT_AVAILABLE);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "Location"));
				myObject.SetPointingInterface( csBuffer);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "DeviceClass"));
				if (csBuffer.CompareNoCase( _T( "SMARTCARDREADER")) == 0)
				{
					myObject.SetType( INPUT_DEVICE_SMARTCARD);
					pMyList->AddTail( myObject);
					uIndex ++;
				}
				AddLog( _T( "\t\t<Type: %s><Manufacturer: %s><Caption: %s><Description: %s><Pointing Type: %s><Interface: %s>\n"), 
					csBuffer, myObject.GetManufacturer(), myObject.GetCaption(), myObject.GetDescription(),
					myObject.GetPointingType(), myObject.GetPointingInterface());
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotal += uIndex;
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "\tFailed because no Win32_PnPSignedDriver Smartcard Reader object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
	}
	if (uTotal > 0)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CWmi::GetHotFixes( CSoftwareList *pMyList)
{
	ASSERT( pMyList);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetHotFixes: Trying to find Win32_QuickFixEngineering WMI objects...\n"));
	try
	{
		CSoftware	myObject;
		UINT		uIndex = 0;
		CString		csBuffer1, csBuffer2;

		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_QuickFixEngineering")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				myObject.Clear();
				// Publisher only Microsoft
				myObject.SetPublisher( MICROSOFT_CORP_STRING);
				// Set Name
				csBuffer1 = m_dllWMI.GetClassObjectStringValue( _T( "Description"));
				csBuffer2 = m_dllWMI.GetClassObjectStringValue( _T( "HotFixID"));
				csBuffer1.AppendFormat( _T( " %s"), csBuffer2);
				myObject.SetName( csBuffer1);
				// Set comments
				csBuffer1 = m_dllWMI.GetClassObjectStringValue( _T( "FixComments"));
				csBuffer2 = m_dllWMI.GetClassObjectStringValue( _T( "Caption"));
				csBuffer1.AppendFormat( _T( " (%s)"), csBuffer2);
				myObject.SetComments( csBuffer1);
				csBuffer1 = m_dllWMI.GetClassObjectStringValue( _T( "InstalledOn"));
				myObject.SetInstallDate( csBuffer1);
				// Software is OK
				pMyList->AddTail( myObject);
				AddLog( _T( "\t\t<Publisher: %s><Name: %s><Comments: %s><InstallDate: %s>\n"), 
					myObject.GetPublisher(), myObject.GetName(), myObject.GetComments(), myObject.GetInstallDate());
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "\tFailed because no Win32_QuickFixEngineering object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
		return FALSE;
	}
}


BOOL CWmi::GetUUID( CString &csUUID)
{
	UINT	uIndex = 0;

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;
	
	// GET BIOS Informations
	AddLog( _T( "WMI GetUUID: Trying to find Win32_ComputerSystemProduct WMI objects...\n"));
	try
	{
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_ComputerSystemProduct")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				csUUID = m_dllWMI.GetClassObjectStringValue( _T( "UUID"));
				AddLog( _T( "\t\t<UUID: %s>\n"), csUUID);
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "\tOK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "\tFailed because no Win32_ComputerSystemProduct object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "\tFailed because unknown exception !\n"));
		return FALSE;
	}
}
