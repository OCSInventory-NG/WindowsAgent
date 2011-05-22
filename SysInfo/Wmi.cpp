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
		AddLog( _T( "WMI Connect: Trying to connect to WMI namespace root\\cimv2 on device <%s>..."), 
			(lpstrDevice == NULL ? _T( "Localhost") : lpstrDevice));
		if (lpstrDevice == NULL)
			csCimRoot.Format( _T( "root\\CIMV2"));
		else
			csCimRoot.Format( _T( "\\\\%s\\root\\CIMV2"), lpstrDevice);
		if (!m_dllWMI.ConnectWMI( csCimRoot))
		{
			// Unable to connect to WMI => no WMI support
			AddLog( _T( "Failed because unable to connect to WMI namespace (0x%lX) !\n"), m_dllWMI.GetLastErrorWMI());
			return FALSE;
		}
		m_bConnected = TRUE;
		AddLog( _T( "OK.\n"));
		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception.\n"));
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
		return _T( "x64");
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

	AddLog( _T( "WMI IsNotebook: Trying to find Win32_PortableBattery WMI objects..."));
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
			AddLog( _T( "OK (%u objects). Is a Notebook.\n"), uIndex);
			return TRUE;
		}
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception!\n"));
	}
	AddLog( _T( "No Win32_PortableBattery => not a Notebook\n"));
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

	pMyBios->Set( NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE);
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
	AddLog( _T( "WMI GetBiosInfo: Trying to find Win32_Bios WMI objects..."));
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
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "OK (%s %s %s %s)\n"), csVendor, csVersion, csDate, csSN);
			return TRUE;
		}
		AddLog( _T( "Failed because no Win32_Bios object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
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
	AddLog( _T( "WMI GetSystemInformation: Trying to find Win32_ComputerSystem WMI objects..."));
	try
	{
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_ComputerSystem")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				csVendor = m_dllWMI.GetClassObjectStringValue( _T( "Manufacturer"));
				csModel = m_dllWMI.GetClassObjectStringValue( _T( "Model"));
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "OK (%s %s)\n"), csVendor, csModel);
			return TRUE;
		}
		AddLog( _T( "Failed because no Win32_ComputerSystem object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
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
	AddLog( _T( "WMI GetBaseBoard: Trying to find Win32_BaseBoard WMI objects..."));
	try
	{
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_BaseBoard")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				csVendor = m_dllWMI.GetClassObjectStringValue( _T( "Manufacturer"));
				csProduct = m_dllWMI.GetClassObjectStringValue( _T( "Model"));
				csSN = m_dllWMI.GetClassObjectStringValue( _T( "SerialNumber"));
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "OK (%s %s %S)\n"), csVendor, csProduct, csSN);
			return TRUE;
		}
		AddLog( _T( "Failed because no Win32_BaseBoard object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
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
	AddLog( _T( "WMI GetSystemEnclosure: Trying to find Win32_SystemEnclosure WMI objects..."));
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
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "OK (%s %s %s %s)\n"), csVendor, csType, csSN, csAssetTag);
			return TRUE;
		}
		AddLog( _T( "Failed because no Win32_SystemEnclosure object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
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
	AddLog( _T( "WMI GetStoragePeripherals: Trying to find Win32_FloppyDrive WMI objects..."));
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
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotalIndex += uIndex;
			AddLog( _T( "OK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "Failed because no Win32_FloppyDrive object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
	}
	// Get Disk drives
	AddLog( _T( "WMI GetStoragePeripherals: Trying to find Win32_DiskDrive WMI objects..."));
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
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		di.Destroy();
		if (uIndex > 0)
		{
			uTotalIndex += uIndex;
			AddLog( _T( "OK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "Failed because no Win32_DiskDrive object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
	}
	// Get CD-Rom drives
	AddLog( _T( "WMI GetStoragePeripherals: Trying to find Win32_CDROMDrive WMI objects..."));
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
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotalIndex += uIndex;
			AddLog( _T( "OK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "Failed because no Win32_CDROMDrive object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
	}
	// Get tape drives
	AddLog( _T( "WMI GetStoragePeripherals: Trying to find Win32_TapeDrive WMI objects..."));
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
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotalIndex += uIndex;
			AddLog( _T( "OK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "Failed because no Win32_TapeDrive object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
	}
	if (uTotalIndex > 0)
	{
		AddLog( _T( "WMI GetStoragePeripherals: OK\n"));
		return TRUE;
	}
	AddLog( _T( "WMI GetStoragePeripherals: Failed because no storage peripherals found !\n"));
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
	AddLog( _T( "WMI GetSystemControllers: Trying to find Win32_FloppyController WMI objects..."));
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
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotalIndex += uIndex;
			AddLog( _T( "OK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "Failed because no Win32_FloppyController object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
	}
	// Get IDE controller
	AddLog( _T( "WMI GetSystemControllers: Trying to find Win32_IDEController WMI objects..."));
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
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotalIndex += uIndex;
			AddLog( _T( "OK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "Failed because no Win32_IDEController object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
	}
	// Get SCSI controller
	AddLog( _T( "WMI GetSystemControllers: Trying to find Win32_SCSIController WMI objects..."));
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
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotalIndex += uIndex;
			AddLog( _T( "OK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "Failed because no Win32_SCSIController object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
	}
	// Get USB controller
	AddLog( _T( "WMI GetSystemControllers: Trying to find Win32_InfraredDevice WMI objects..."));
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
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotalIndex += uIndex;
			AddLog( _T( "OK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "Failed because no Win32_InfraredDevice object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
	}
	// Get USB controller
	AddLog( _T( "WMI GetSystemControllers: Trying to find Win32_USBController WMI objects..."));
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
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotalIndex += uIndex;
			AddLog( _T( "OK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "Failed because no Win32_USBController object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
	}
	// Get IEEE1394 controller
	AddLog( _T( "WMI GetSystemControllers: Trying to find Win32_1394Controller WMI objects..."));
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
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotalIndex += uIndex;
			AddLog( _T( "OK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "Failed because no Win32_1394Controller object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
	}
	// Get PCMCIA controller
	AddLog( _T( "WMI GetSystemControllers: Trying to find Win32_PCMCIAController WMI objects..."));
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
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotalIndex += uIndex;
			AddLog( _T( "OK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "Failed because no Win32_PCMCIAController object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
	}
	if (uTotalIndex > 0)
	{
		AddLog( _T( "WMI GetSystemControllers: OK\n"));
		return TRUE;
	}
	AddLog( _T( "WMI GetSystemControllers: Failed because no system controler found !\n"));
	return FALSE;
}

BOOL CWmi::GetVideoAdapters(CVideoAdapterList *pMyList)
{
	ASSERT( pMyList);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetVideoAdapters: Trying to find Win32_VideoController WMI objects..."));
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
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "OK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "Failed because no Win32_VideoController object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
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
				csBuffer = m_dllWMI.GetRefElementClassObjectStringValue( _T( "Element"), _T( "Status"));
				myObject.SetStatus( csBuffer);
				csBuffer = m_dllWMI.GetRefElementClassObjectStringValue( _T( "Element"), _T( "AdapterType"));
				myObject.SetTypeMIB( csBuffer);
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
			AddLog( _T( "OK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "Failed because no Win32_NetworkAdapterSetting object with valid MAC Address !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
		return FALSE;
	}
}

BOOL CWmi::GetMonitors(CMonitorList *pMyList)
{
	ASSERT( pMyList);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetMonitors: Trying to find Win32_DesktopMonitor WMI objects..."));
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
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "OK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "Failed because no Win32_DesktopMonitor object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
		return FALSE;
	}
}

BOOL CWmi::GetModems(CModemList *pMyList)
{
	ASSERT( pMyList);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetModems: Trying to find Win32_POTSModem WMI objects..."));
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
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "OK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "Failed because no Win32_POTSModem object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
		return FALSE;
	}
}

BOOL CWmi::GetSoundDevices(CSoundDeviceList *pMyList)
{
	ASSERT( pMyList);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetSoundDevices: Trying to find Win32_SoundDevice WMI objects..."));
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
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "OK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "Failed because no Win32_SoundDevice object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
		return FALSE;
	}
}

BOOL CWmi::GetPrinters(CPrinterList *pMyList)
{
	ASSERT( pMyList);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetPrinters: Trying to find Win32_Printer WMI objects..."));
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
				pMyList->AddTail( myObject);
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "OK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "Failed because no Win32_Printer object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
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
	AddLog( _T( "WMI GetSystemPorts: Trying to find Win32_PortConnector WMI objects..."));
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
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotal += uIndex;
			AddLog( _T( "OK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "Failed because no Win32_PortConnector object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
	}
	if (uTotal == 0)
	{
		// No connector port object, try serial and parallel
		// Get serial ports
		AddLog( _T( "WMI GetSystemPorts: Trying to find Win32_SerialPort WMI objects..."));
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
				}
				m_dllWMI.CloseEnumClassObject();
			}
			if (uIndex > 0)
			{
				uTotal += uIndex;
				AddLog( _T( "OK (%u objects)\n"), uIndex);
			}
			else
				AddLog( _T( "Failed because no Win32_SerialPort object !\n"));
		}
		catch (CException *pEx)
		{
			pEx->Delete();
			AddLog( _T( "Failed because unknown exception !\n"));
		}
		// Get parallel ports
		AddLog( _T( "WMI GetSystemPorts: Trying to find Win32_ParallelPort WMI objects..."));
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
				}
				m_dllWMI.CloseEnumClassObject();
			}
			if (uIndex > 0)
			{
				uTotal += uIndex;
				AddLog( _T( "OK (%u objects)\n"), uIndex);
			}
			else
				AddLog( _T( "Failed because no Win32_ParallelPort object !\n"));
		}
		catch (CException *pEx)
		{
			pEx->Delete();
			AddLog( _T( "Failed because unknown exception !\n"));
		}
	}
	if (uTotal > 0)
	{
		AddLog( _T( "WMI GetSystemPorts: OK\n"));
		return TRUE;
	}
	AddLog( _T( "WMI GetSystemPorts: Failed because no system port found !\n"));
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

	AddLog( _T( "WMI GetProcessors: Trying to find Win32_Processor WMI objects..."));
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
				csProcType.AppendFormat( _T( " [%lu core(s) %s]"), dwCore, GetArchitecture( dwArch));
				StrForSQL( csProcType);
				dwNumber ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (dwNumber > 0)
		{
			AddLog( _T( "%lu x %s at %s. OK\n"), dwNumber, csProcType, csProcSpeed);
			return dwNumber;
		}
		AddLog( _T( "Failed because no Win32_Processor object !\n"));
		return 0;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
		return 0;
	}
}

BOOL CWmi::GetOS(CString &csName, CString &csVersion, CString &csComment, CString &csDescription, CString &csInstallDate)
{
	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetOS: Trying to find Win32_OperatingSystem WMI objects..."));
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
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "OK (%s %s %s %s).\n"), csName, csVersion, csComment, csDescription);
			return TRUE;
		}
		AddLog( _T( "Failed because no Win32_OperatingSystem object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
		return FALSE;
	}
}

DWORD CWmi::GetAddressWidthOS()
{
	static DWORD	dwBits;
	DWORD			dwNumber = 0;
	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetAddressWidthOS: Trying to find Win32_Processor WMI objects..."));
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
				dwNumber ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (dwNumber > 0)
			AddLog( _T( "%u bits OS. OK\n"), dwBits);
		else
		{
			AddLog( _T( "Failed because no Win32_Processor object, assuming 32 bits OS !\n"));
			dwBits = 32;
		}
		return dwBits;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception, assuming 32 bits OS !\n"));
		dwBits = 32;
		return dwBits;
	}
}


BOOL CWmi::GetWindowsRegistration( CString &csCompany, CString &csUser, CString &csSN)
{
	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetWindowsRegistration: Trying to find Win32_OperatingSystem WMI objects..."));
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
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "OK (%s %s %s).\n"), csCompany, csUser, csSN);
			return TRUE;
		}
		AddLog( _T( "Failed because no Win32_OperatingSystem object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
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
		csResult += _T( ";");
	}
	csType = csResult;
	return TRUE;
}

BOOL CWmi::GetDomainOrWorkgroup( CString &csDomain)
{
	BOOL	bResult = FALSE;

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return bResult;

	// Try to use win32 computer system object to get System Manufacturer, Model
	AddLog( _T( "WMI GetDomainOrWorkgroup: Trying to find Win32_ComputerSystem WMI objects..."));
	try
	{
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_ComputerSystem")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				csDomain = m_dllWMI.GetClassObjectStringValue( _T( "Domain"));
			}
			m_dllWMI.CloseEnumClassObject();
			AddLog( _T( "OK (%s)\n"), csDomain);
			bResult = TRUE;
		}
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
	}
	return bResult;
}

BOOL CWmi::GetSystemSlots(CSystemSlotList *pMyList)
{
	ASSERT( pMyList);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetSystemSlots: Trying to find Win32_SystemSlot WMI objects..."));
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
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "OK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "Failed because no Win32_SystemSlot object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
		return FALSE;
	}
}

BOOL CWmi::GetLogicalDrives( CLogicalDriveList *pMyList)
{
	ASSERT( pMyList);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetLogicalDrives: Trying to find Win32_LogicalDisk WMI objects..."));
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
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "OK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "Failed because no Win32_LogicalDisk object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
		return FALSE;
	}
}

BOOL CWmi::GetMemorySlots(CMemorySlotList *pMyList)
{
	ASSERT( pMyList);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetMemorySlots: Trying to find Win32_PhysicalMemory WMI objects..."));
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
				myObject.SetType( dwValue);
				csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "SerialNumber"));
/*				// If S/N is empty, get Part number
				if (csBuffer.IsEmpty())
					csBuffer = m_dllWMI.GetClassObjectStringValue( _T( "PartNumber"));
*/
				if ((u64Value > 0) && myObject.IsValidSN( csBuffer))
					myObject.SetSN( csBuffer);
				// Device is OK
				pMyList->AddTail( myObject);
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex == 0)
		{
			AddLog( _T( "Failed because no Win32_PhysicalMemory object !\n"));
			return FALSE;
		}
		AddLog( _T( "OK (%u objects)\n"), uIndex);
		// Checking Physical Memory Array objects for completing properties
		AddLog( _T( "WMI GetMemorySlots: Trying to find Win32_PhysicalMemoryArray WMI objects..."));
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
		AddLog( _T( "OK (%u objects updated)\n"), uIndex);
		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
		return FALSE;
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
	AddLog( _T( "WMI GetInputDevices: Trying to find Win32_Keyboard WMI objects..."));
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
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotal += uIndex;
			AddLog( _T( "OK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "Failed because no Win32_Keyboard object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
	}
	// Get pointing devices
	AddLog( _T( "WMI GetInputDevices: Trying to find Win32_PointingDevice WMI objects..."));
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
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			uTotal += uIndex;
			AddLog( _T( "OK (%u objects)\n"), uIndex);
		}
		else
			AddLog( _T( "Failed because no Win32_PointingDevice object !\n"));
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
	}
	if (uTotal > 0)
	{
		AddLog( _T( "WMI GetInputDevices: OK\n"));
		return TRUE;
	}
	AddLog( _T( "WMI GetInputDevices: Failed because no input device found !\n"));
	return FALSE;
}

BOOL CWmi::GetHotFixes( CSoftwareList *pMyList)
{
	ASSERT( pMyList);

	// If not WMI connected => cannot do this
	if (!m_bConnected)
		return FALSE;

	AddLog( _T( "WMI GetHotFixes: Trying to find Win32_QuickFixEngineering WMI objects..."));
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
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
			AddLog( _T( "OK (%u objects)\n"), uIndex);
			return TRUE;
		}
		AddLog( _T( "Failed because no Win32_QuickFixEngineering object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
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
	AddLog( _T( "WMI GetUUID: Trying to find Win32_ComputerSystemProduct WMI objects..."));
	try
	{
		if (m_dllWMI.BeginEnumClassObject( _T( "Win32_ComputerSystemProduct")))
		{
			while (m_dllWMI.MoveNextEnumClassObject())
			{
				csUUID = m_dllWMI.GetClassObjectStringValue( _T( "UUID"));
				uIndex ++;
			}
			m_dllWMI.CloseEnumClassObject();
		}
		if (uIndex > 0)
		{
			AddLog( _T( "OK (%s)\n"), csUUID);
			return TRUE;
		}
		AddLog( _T( "Failed because no Win32_ComputerSystemProduct object !\n"));
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		AddLog( _T( "Failed because unknown exception !\n"));
		return FALSE;
	}
}
