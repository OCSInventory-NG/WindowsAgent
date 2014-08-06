//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Registry.cpp: implementation of the CRegistry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SysInfo.h"
#include "DebugLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Defines for retrieving Windows registration
#define NT_REGISTRATION_KEY						_T( "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion")
#define NT_REGISTRATION_PRODUCT_ID_VALUE		_T( "ProductId")
#define NT_REGISTRATION_DIGITAL_ID_VALUE		_T( "DigitalProductId");
#define NT_REGISTRATION_COMPANY_VALUE			_T( "RegisteredOrganization")
#define NT_REGISTRATION_OWNER_VALUE				_T( "RegisteredOwner")

// Defines for retrieving logon username from NT/2000/XP registry
#define NT_LOGON_USER_KEY						_T( "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\WinLogon")
#define NT_LOGON_USER_VALUE						_T( "DefaultUserName")

// Defines for retrieving logon username from Vista+ registry
#define VISTA_LOGON_USER_KEY					_T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\LogonUI")
#define VISTA_LOGON_USER_VALUE					_T( "LastLoggedOnSAMUser")

// Defines for retrieving last user who'd been logged in
#define NT_LASTLOGGEDUSER_USER_KEY				_T( "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon")
#define NT_LASTLOGGEDUSER_USER_VALUE			_T( "DefaultUserName")

// Defines for retrieving last user who'd been logged in on Vista+
#define VISTA_LASTLOGGEDUSER_USER_KEY			_T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\LogonUI")
#define VISTA_LASTLOGGEDUSER_USER_VALUE			_T( "LastLoggedOnUser" )

// Defines for retrieving computer description from NT registry
#define NT_COMPUTER_DESCRIPTION_KEY				_T( "SYSTEM\\CurrentControlSet\\Services\\lanmanserver\\Parameters")
#define NT_COMPUTER_DESCRIPTION_VALUE			_T( "srvcomment")

// Defines for retrieving domain or workgroup from NT registry
#define NT_DOMAIN_KEY							_T( "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon")
#define NT_DOMAIN_VALUE							_T( "DefaultDomainName")

// Defines for retrieving BIOS from NT registry
#define NT_BIOS_KEY								_T( "HARDWARE\\Description\\System")
#define NT_SYSTEM_MANUFACTURER_VALUE			_T( "Manufacturer")
#define NT_SYSTEM_MODEL_VALUE					_T( "Model")
#define NT_SYSTEM_SERIAL_VALUE					_T( "Serial")
#define NT_BIOS_MACHINE_TYPE_VALUE				_T( "Identifier")
#define NT_BIOS_MANUFACTURER_VALUE				_T( "BIOSName")
#define NT_BIOS_VERSION_VALUE					_T( "SystemBiosVersion")
#define NT_BIOS_DATE_VALUE						_T( "SystemBiosDate")
#define NT_BASEBOARD_MANUFACTURER_VALUE			_T( "BaseBoardManufacturer")
#define NT_BASEBOARD_MODEL_VALUE				_T( "BaseBoardProduct")

// Defines for retrieving BIOS from Vista registry
#define VISTA_BIOS_KEY							_T( "HARDWARE\\Description\\System\\BIOS")
#define VISTA_SYSTEM_MANUFACTURER_VALUE			_T( "SystemManufacturer")
#define VISTA_SYSTEM_MODEL_VALUE				_T( "SystemProductName")
#define VISTA_SYSTEM_SERIAL_VALUE				_T( "SystemFamily")
#define VISTA_BIOS_MACHINE_TYPE_VALUE			_T( "SystemFamily")
#define VISTA_BIOS_MANUFACTURER_VALUE			_T( "BIOSVendor")
#define VISTA_BIOS_VERSION_VALUE				_T( "BIOSVersion")
#define VISTA_BIOS_DATE_VALUE					_T( "BIOSReleaseDate")
#define VISTA_BASEBOARD_MANUFACTURER_VALUE		_T( "BaseBoardManufacturer")
#define VISTA_BASEBOARD_MODEL_VALUE				_T( "BaseBoardProduct")

// Defines for retrieving processors from NT registry
#define NT_PROCESSOR_KEY						_T( "HARDWARE\\Description\\System\\CentralProcessor")
#define NT_PROCESSOR_MANUFACTURER_VALUE			_T( "VendorIdentifier")
#define NT_PROCESSOR_NAME_VALUE					_T( "Identifier")
#define NT_PROCESSOR_NAME_STRING_VALUE			_T( "ProcessorNameString")
#define NT_PROCESSOR_SPEED_VALUE				_T( "~MHz")

// Defines for retrieving Display from NT registry
#define NT_ENUM_KEY								_T( "SYSTEM\\CurrentControlSet\\Enum")
#define NT_ENUM_CLASS_VALUE						_T( "Class")
#define NT_ENUM_DISPLAY_VALUE					_T( "Display")
#define NT_ENUM_SERVICE_VALUE					_T( "Service")
#define NT_SERVICES_KEY							_T( "SYSTEM\\CurrentControlSet\\Services")
#define NT_SERVICE_DEVICE_KEY					_T( "Device0")
#define NT_ADAPTER_NAME_VALUE					_T( "HardwareInformation.AdapterString")
#define NT_ADAPTER_CHIP_VALUE					_T( "HardwareInformation.ChipType")
#define NT_ADAPTER_MEMORY_VALUE					_T( "HardwareInformation.MemorySize")

// Defines for retrieving Display from XP registry
#define XP_ENUM_KEY								_T( "SYSTEM\\CurrentControlSet\\Control\\Video")
#define XP_ADAPTER_NAME_VALUE					_T( "HardwareInformation.AdapterString")
#define XP_ADAPTER_CHIP_VALUE					_T( "HardwareInformation.ChipType")
#define XP_ADAPTER_MEMORY_VALUE					_T( "HardwareInformation.MemorySize")

// Defines for retrieving Printers from registry
#define PRINTERS_ENUM_KEY						_T( "SYSTEM\\CurrentControlSet\\Control\\Print\\Printers")
#define PRINTER_NAME_VALUE						_T( "Name")
#define PRINTER_DRIVER_VALUE					_T( "Printer Driver")
#define PRINTER_PORT_VALUE						_T( "Port")

// Defines for retrieving Monitors from NT registry
#define NT_MONITOR_KEY							_T( "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E96E-E325-11CE-BFC1-08002BE10318}")
#define NT_MONITOR_MANUFACTURER_VALUE			_T( "ProviderName")
#define NT_MONITOR_DESCRIPTION_VALUE			_T( "DriverDesc")
#define NT_MONITOR_CAPTION_VALUE				_T( "DriverDesc")
#define NT_MONITOR_TYPE_VALUE					_T( "DriverDesc")

// Defines for retrieving input devices from NT registry
#define NT_INPUT_KEYBOARD_KEY					_T( "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E96B-E325-11CE-BFC1-08002BE10318}")
#define NT_INPUT_KEYBOARD_MANUFACTURER_VALUE	_T( "ProviderName")
#define NT_INPUT_KEYBOARD_CAPTION_VALUE			_T( "DriverDesc")
#define NT_INPUT_KEYBOARD_DESCRIPTION_VALUE		_T( "DriverDesc")
#define NT_INPUT_KEYBOARD_TYPE_VALUE			_T( "")
#define NT_INPUT_KEYBOARD_INTERFACE_VALUE		_T( "LocationInformationOverride")

#define NT_INPUT_POINTING_KEY					_T( "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E96F-E325-11CE-BFC1-08002BE10318}")
#define NT_INPUT_POINTING_MANUFACTURER_VALUE	_T( "ProviderName")
#define NT_INPUT_POINTING_CAPTION_VALUE			_T( "DriverDesc")
#define NT_INPUT_POINTING_DESCRIPTION_VALUE		_T( "DriverDesc")
#define NT_INPUT_POINTING_TYPE_VALUE			_T( "")
#define NT_INPUT_POINTING_INTERFACE_VALUE		_T( "LocationInformationOverride")

// Defines for retrieving system ports from NT registry
#define NT_SYSTEM_PORT_CLASS_PARALLEL			0
#define NT_SYSTEM_PORT_CLASS_SERIAL				1
#define NT_SYSTEM_PORT_KEY						_T( "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E978-E325-11CE-BFC1-08002BE10318}")
#define NT_SYSTEM_PORT_NAME_VALUE				_T( "DriverDesc")
#define NT_SYSTEM_PORT_CAPTION_VALUE			_T( "DriverDesc")
#define NT_SYSTEM_PORT_DESCRIPTION_VALUE		_T( "DriverDesc")
#define NT_SYSTEM_PORT_TYPE_VALUE				_T( "PortSubClass")

// Defines for retrieving system controlers from NT registry
#define NT_CONTROLER_FLOPPY_KEY					_T( "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E969-E325-11CE-BFC1-08002BE10318}")
#define NT_CONTROLER_FLOPPY_MANUFACTURER_VALUE	_T( "ProviderName")
#define NT_CONTROLER_FLOPPY_NAME_VALUE			_T( "DriverDesc")
#define NT_CONTROLER_FLOPPY_CAPTION_VALUE		_T( "DriverDesc")
#define NT_CONTROLER_FLOPPY_DESCRIPTION_VALUE	_T( "DriverDesc")
#define NT_CONTROLER_FLOPPY_VERSION_VALUE		_T( "DriverVersion")

#define NT_CONTROLER_IDE_KEY					_T( "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E96A-E325-11CE-BFC1-08002BE10318}")
#define NT_CONTROLER_IDE_MANUFACTURER_VALUE		_T( "ProviderName")
#define NT_CONTROLER_IDE_NAME_VALUE				_T( "DriverDesc")
#define NT_CONTROLER_IDE_CAPTION_VALUE			_T( "DriverDesc")
#define NT_CONTROLER_IDE_DESCRIPTION_VALUE		_T( "DriverDesc")
#define NT_CONTROLER_IDE_VERSION_VALUE			_T( "DriverVersion")

#define NT_CONTROLER_SCSI_KEY					_T( "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E97B-E325-11CE-BFC1-08002BE10318}")
#define NT_CONTROLER_SCSI_MANUFACTURER_VALUE	_T( "ProviderName")
#define NT_CONTROLER_SCSI_NAME_VALUE			_T( "DriverDesc")
#define NT_CONTROLER_SCSI_CAPTION_VALUE			_T( "DriverDesc")
#define NT_CONTROLER_SCSI_DESCRIPTION_VALUE		_T( "DriverDesc")
#define NT_CONTROLER_SCSI_VERSION_VALUE			_T( "DriverVersion")

#define NT_CONTROLER_INFRARED_KEY				_T( "SYSTEM\\CurrentControlSet\\Control\\Class\\{6BDD1FC5-810F-11D0-BEC7-08002BE2092F}")
#define NT_CONTROLER_INFRARED_MANUFACTURER_VALUE _T( "ProviderName")
#define NT_CONTROLER_INFRARED_NAME_VALUE		_T( "DriverDesc")
#define NT_CONTROLER_INFRARED_CAPTION_VALUE		_T( "DriverDesc")
#define NT_CONTROLER_INFRARED_DESCRIPTION_VALUE	_T( "DriverDesc")
#define NT_CONTROLER_INFRARED_VERSION_VALUE		_T( "DriverVersion")
#define NT_CONTROLER_INFRARED_CONTROLER_VALUE	_T( "Controller")

#define NT_CONTROLER_USB_KEY					_T( "SYSTEM\\CurrentControlSet\\Control\\Class\\{36FC9E60-C465-11CF-8056-444553540000}")
#define NT_CONTROLER_USB_MANUFACTURER_VALUE		_T( "ProviderName")
#define NT_CONTROLER_USB_NAME_VALUE				_T( "DriverDesc")
#define NT_CONTROLER_USB_CAPTION_VALUE			_T( "DriverDesc")
#define NT_CONTROLER_USB_DESCRIPTION_VALUE		_T( "DriverDesc")
#define NT_CONTROLER_USB_VERSION_VALUE			_T( "DriverVersion")
#define NT_CONTROLER_USB_CONTROLER_VALUE		_T( "Controller")

#define NT_CONTROLER_IEEE1394_KEY				_T( "SYSTEM\\CurrentControlSet\\Control\\Class\\{6BDD1FC1-810F-11D0-BEC7-08002BE2092F}")
#define NT_CONTROLER_IEEE1394_MANUFACTURER_VALUE _T( "ProviderName")
#define NT_CONTROLER_IEEE1394_NAME_VALUE		_T( "DriverDesc")
#define NT_CONTROLER_IEEE1394_CAPTION_VALUE		_T( "DriverDesc")
#define NT_CONTROLER_IEEE1394_DESCRIPTION_VALUE _T( "DriverDesc")
#define NT_CONTROLER_IEEE1394_VERSION_VALUE		_T( "DriverVersion")

#define NT_CONTROLER_PCMCIA_KEY					_T( "SYSTEM\\CurrentControlSet\\Services\\Class\\PCMCIA")
#define NT_CONTROLER_PCMCIA_MANUFACTURER_VALUE	_T( "ProviderName")
#define NT_CONTROLER_PCMCIA_NAME_VALUE			_T( "DriverDesc")
#define NT_CONTROLER_PCMCIA_CAPTION_VALUE		_T( "DriverDesc")
#define NT_CONTROLER_PCMCIA_DESCRIPTION_VALUE	_T( "DriverDesc")
#define NT_CONTROLER_PCMCIA_VERSION_VALUE		_T( "DriverVersion")

// Defines for retrieving storage peripherals from NT registry
#define NT_STORAGE_FLOPPY_KEY					_T( "SYSTEM\\CurrentControlSet\\Enum\\FDC")
#define NT_STORAGE_FLOPPY_MANUFACTURER_VALUE	_T( "Mfg")
#define NT_STORAGE_FLOPPY_MODEL_VALUE			_T( "FriendlyName")
#define NT_STORAGE_FLOPPY_NAME_VALUE			_T( "DeviceDesc")
#define NT_STORAGE_FLOPPY_DESCRIPTION_VALUE		_T( "HardwareId")
#define NT_STORAGE_FLOPPY_TYPE_VALUE			_T( "Class")

#define NT_STORAGE_OTHER_KEY					_T( "HARDWARE\\DEVICEMAP\\Scsi")
#define NT_STORAGE_OTHER_MANUFACTURER_VALUE		_T( "Mfg")
#define NT_STORAGE_OTHER_MODEL_VALUE			_T( "Identifier")
#define NT_STORAGE_OTHER_NAME_VALUE				_T( "DeviceName")
#define NT_STORAGE_OTHER_DESCRIPTION_VALUE		_T( "Identifier")
#define NT_STORAGE_OTHER_TYPE_VALUE				_T( "Type")

// Defines for retrieving Sound devices from NT registry
#define NT_SOUND_KEY							_T( "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E96C-E325-11CE-BFC1-08002BE10318}")
#define NT_SOUND_MANUFACTURER_VALUE				_T( "ProviderName")
#define NT_SOUND_NAME_VALUE						_T( "DriverDesc")
#define NT_SOUND_DESCRIPTION_VALUE				_T( "DriverDesc")
#define NT_SOUND_DRIVER							_T( "Drivers")
#define NT_SOUND_DRIVER_CLASS					_T( "SubClasses")

// Defines for retrieving Modems from NT registry
#define NT_MODEM_KEY							_T( "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E96D-E325-11CE-BFC1-08002BE10318}")
#define NT_MODEM_MODEL_VALUE					_T( "Model")
#define NT_MODEM_NAME_VALUE						_T( "DriverDesc")
#define NT_MODEM_DESCRIPTION_VALUE				_T( "DriverDesc")
#define NT_MODEM_TYPE_VALUE						_T( "AttachedTo")

// Defines for retrieving if it is notbook from NT registry
#define NT_NOTEBOOK_KEY							_T( "SYSTEM\\CurrentControlSet\\Control\\Class\\{72631E54-78A4-11D0-BCF7-00AA00B7B32A}")

// Defines for retrieving installed apps from 9X/Me registry
#define NT_APPS_KEY								_T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall")
#define NT_APPS_VENDOR_VALUE					_T( "Publisher")
#define NT_APPS_NAME_VALUE						_T( "DisplayName")
#define NT_APPS_VERSION_VALUE					_T( "DisplayVersion")
#define NT_APPS_FOLDER_VALUE					_T( "InstallLocation")
#define NT_APPS_COMMENTS_VALUE					_T( "Comments")
#define NT_APPS_UNINSTALL_VALUE					_T( "UninstallString")
#define NT_APPS_QUIETUNINSTALL_VALUE			_T( "QuietUninstallString")
#define NT_APPS_MODIFY_VALUE					_T( "ModifyPath")
#define NT_APPS_LANGUAGE_VALUE					_T( "Language")
#define NT_APPS_INSTALLDATE_VALUE				_T( "InstallDate")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegistry::CRegistry()
{
	OSVERSIONINFO	osVersion;

	osVersion.dwOSVersionInfoSize = sizeof( OSVERSIONINFO);
	if (GetVersionEx( &osVersion))
	{
		m_dwPlatformId = osVersion.dwPlatformId;
		m_bIsXP = (osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
				  (((osVersion.dwMajorVersion == 5) && (osVersion.dwMinorVersion >= 1)) ||
				  (osVersion.dwMajorVersion > 5));
		m_bIsVista = (osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
				  	 (osVersion.dwMajorVersion >= 6);
	}
	else
	{
		// Failed so assume NT based, but not XP or later
		m_dwPlatformId = VER_PLATFORM_WIN32_NT;
		m_bIsXP = FALSE;
		m_bIsVista = FALSE;
	}
	m_hKey = NULL;
	m_bRemoteRegistry = FALSE;
	m_dwAddressWidth = 32;
}

CRegistry::~CRegistry()
{
	Disconnect();
}

LPCTSTR CRegistry::ParseMultiSZ(LPCTSTR lpstrCurrent)
{
	static LPCTSTR szRemainder = NULL;
	LPCTSTR szReturn = NULL;
	
	if (lpstrCurrent)
	{         
		szRemainder = lpstrCurrent;
    }
	else
		if (szRemainder == NULL)
		{
			return NULL;
	    }
	if (*szRemainder)
	{
		szReturn = szRemainder;
		while (*++szRemainder);
		szRemainder++;     
	}
	return szReturn;
}

LONG CRegistry::GetValue( HKEY hKey, LPCTSTR lpstrValue, DWORD *dwData)
{
	DWORD	dwType = REG_DWORD;
	DWORD	dwSize = sizeof( DWORD);

	if ((hKey == NULL) || (lpstrValue == NULL))
		return ERROR_INVALID_HANDLE;

	return RegQueryValueEx( hKey, lpstrValue, NULL, &dwType, (LPBYTE) dwData, &dwSize);
}

LONG CRegistry::GetValue( HKEY hKey, LPCTSTR lpstrValue, CString &csData)
{
	LPBYTE	lpData = NULL;
	DWORD	dwType = REG_NONE;
	DWORD	dwSize = 0;
	DWORD	dwCpt;
	LPCTSTR pSZ = NULL;
	CString	csTemp;
	LONG	lResult;
	DWORD	dwValue;
	unsigned __int64 qwValue;

	csData.Empty();
	if ((hKey == NULL) || (lpstrValue == NULL))
		return ERROR_INVALID_HANDLE;

	// First call to get size and type of data
	lResult = RegQueryValueEx( hKey, lpstrValue, NULL, &dwType, NULL, &dwSize);
	if ((lResult == ERROR_MORE_DATA) || (lResult == ERROR_SUCCESS))
	{
		// Allocate buffer to receive data
		if ((lpData = (LPBYTE) malloc ((dwSize+2)*sizeof( BYTE))) == NULL)
			return ERROR_NOT_ENOUGH_MEMORY;
		// Get data
		switch (dwType)
		{
		case REG_DWORD:
			dwSize = sizeof( DWORD);
			if ((lResult = RegQueryValueEx( hKey, lpstrValue, NULL, &dwType, (LPBYTE) &dwValue, &dwSize)) != ERROR_SUCCESS)
			{
				free( lpData);
				return ERROR_INVALID_DATA;
			}
			csData.Format( _T( "%I32u"), dwValue);
			break;
		case REG_QWORD:
			dwSize = sizeof( unsigned __int64);
			if ((lResult = RegQueryValueEx( hKey, lpstrValue, NULL, &dwType, (LPBYTE) &qwValue, &dwSize)) != ERROR_SUCCESS)
			{
				free( lpData);
				return ERROR_INVALID_DATA;
			}
			csData.Format( _T( "%I64u"), qwValue);
			break;
		case REG_SZ:
		case REG_EXPAND_SZ:
			if ((lResult = RegQueryValueEx( hKey, lpstrValue, NULL, &dwType, lpData, &dwSize)) != ERROR_SUCCESS)
			{
				free( lpData);
				return ERROR_INVALID_DATA;
			}
			// Ensure string is null terminated
			lpData[dwSize]=0;
			lpData[dwSize+1]=0;
			csData.Format( _T( "%s"), lpData);
			break;
		case REG_MULTI_SZ:
			if ((lResult = RegQueryValueEx( hKey, lpstrValue, NULL, &dwType, lpData, &dwSize)) != ERROR_SUCCESS)
			{
				free( lpData);
				return ERROR_INVALID_DATA;
			}
			// Ensure string is null terminated
			lpData[dwSize]=0;
			lpData[dwSize+1]=0;
			// Parse multistring registry value
			pSZ = ParseMultiSZ( (LPCTSTR) lpData);
			while (pSZ != NULL)
			{
				csData += pSZ;
				csData += _T( " ");
				pSZ = ParseMultiSZ();
			}
			break;
		case REG_BINARY:
			if ((lResult = RegQueryValueEx( hKey, lpstrValue, NULL, &dwType, lpData, &dwSize)) != ERROR_SUCCESS)
			{
				free( lpData);
				return ERROR_INVALID_DATA;
			}
			// Ensure string is null terminated
			lpData[dwSize]=0;
			lpData[dwSize+1]=0;
			for (dwCpt=0; dwCpt<dwSize; dwCpt++)
			{
				csTemp.Format( _T( "%.02X "), lpData[ dwCpt]);
				csData += csTemp;
			}
			break;
		case REG_NONE:
		default:
			free( lpData);
			return ERROR_BAD_FORMAT;
		}
		free( lpData);
		return ERROR_SUCCESS;
	} 
	// Unable to open value, not existing ?
	return ERROR_INVALID_DATA;
}

BOOL CRegistry::Connect(LPCTSTR lpstrDevice)
{
	CString	csDevice;

	// Do not connect if already connected
	if (m_hKey != NULL)
		return TRUE;
	AddLog( _T( "Registry Connect: Trying to connect to HKEY_LOCAL_MACHINE on device <%s>...\n"), lpstrDevice == NULL ? _T( "Localhost") : lpstrDevice);
	if (lpstrDevice != NULL)
	{
		// Connect to remote registry
		csDevice.Format( _T( "\\\\%s"), lpstrDevice);
		if (RegConnectRegistry( csDevice, HKEY_LOCAL_MACHINE, &m_hKey) != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegConnectRegistry> function for %s\\HKLM !\n"),
					csDevice);
			return FALSE;
		}
		m_bRemoteRegistry = TRUE;
		AddLog( _T( "\tOK\n"));
		return TRUE;
	}
	// connect to local registry
	m_hKey = HKEY_LOCAL_MACHINE;
	AddLog( _T( "\tOK\n"));
	return TRUE;
}

void CRegistry::SetAddressWidthOS( DWORD dwAddressWidth)
{
	m_dwAddressWidth = dwAddressWidth;
}

BOOL CRegistry::Disconnect()
{
	if (m_bRemoteRegistry)
	{
		AddLog( _T( "Registry Disconnect: Disconnected from HKEY_LOCAL_MACHINE.\n"));
		RegCloseKey( m_hKey);
	}
	m_hKey = NULL;
	m_bRemoteRegistry = FALSE;
	return TRUE;
}

BOOL CRegistry::GetBiosInfo( CBios *pMyBios)
{
	// Try to get BIOS Informations info from the registry
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		if (m_bIsVista && GetBiosInfoVista( pMyBios))
			return TRUE;
		// Windows NT/2000/XP/2003
		return GetBiosInfoNT( pMyBios);
	default:
		// Unknown
		pMyBios->Set( NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE,
					  NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE,
					  NOT_AVAILABLE, NOT_AVAILABLE);
		AddLog( _T( "Registry GetBiosInfo:\n\tFailed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::GetBiosInfoNT( CBios *pMyBios)
{
	HKEY	hKey = NULL;
	CString	csValue;
	LONG	lResult;
	BOOL	bManufacturer = FALSE,
			bModel = FALSE;

	AddLog( _T( "Registry NT GetBiosInfo: Trying to find BIOS in HKLM\\%s...\n"), NT_BIOS_KEY);
	// Windows NT
	lResult = RegOpenKeyEx( m_hKey, NT_BIOS_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKey);
	if (lResult == ERROR_SUCCESS)
	{
		// Get System manufacturer
		lResult = GetValue( hKey, NT_SYSTEM_MANUFACTURER_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					NT_BIOS_KEY, NT_SYSTEM_MANUFACTURER_VALUE);
			pMyBios->SetSystemManufacturer( NOT_AVAILABLE);
			bManufacturer = FALSE;
		}
		else
		{
			pMyBios->SetSystemManufacturer( csValue);
			bManufacturer = TRUE;
		}
		// Get System model
		lResult = GetValue( hKey, NT_SYSTEM_MODEL_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					NT_BIOS_KEY, NT_SYSTEM_MODEL_VALUE);
			pMyBios->SetSystemModel( NOT_AVAILABLE);
			bModel = FALSE;
		}
		else
		{
			pMyBios->SetSystemModel( csValue);
			bModel = TRUE;
		}
		// Get System serial number
		lResult = GetValue( hKey, NT_SYSTEM_SERIAL_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					NT_BIOS_KEY, NT_SYSTEM_SERIAL_VALUE);
			pMyBios->SetSystemSerialNumber( NOT_AVAILABLE);
		}
		else
		{
			pMyBios->SetSystemSerialNumber( csValue);
		}
		// Get BIOS manufacturer
		lResult = GetValue( hKey, NT_BIOS_MANUFACTURER_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					NT_BIOS_KEY, NT_BIOS_MANUFACTURER_VALUE);
			pMyBios->SetBiosManufacturer( NOT_AVAILABLE);
		}
		else
		{
			pMyBios->SetBiosManufacturer( csValue);
		}
		// Get BIOS version
		lResult = GetValue( hKey, NT_BIOS_VERSION_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					NT_BIOS_KEY, NT_BIOS_VERSION_VALUE);
			pMyBios->SetBiosVersion( NOT_AVAILABLE);
		}
		else
		{
			pMyBios->SetBiosVersion( csValue);
		}
		// Get BIOS date
		lResult = GetValue( hKey, NT_BIOS_DATE_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					NT_BIOS_KEY, NT_BIOS_DATE_VALUE);
			pMyBios->SetBiosDate( NOT_AVAILABLE);
		}
		else
		{
			pMyBios->SetBiosDate( csValue);
		}
		// Get Motherboard manufacturer
		lResult = GetValue( hKey, NT_BASEBOARD_MANUFACTURER_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					NT_BIOS_KEY, NT_BASEBOARD_MANUFACTURER_VALUE);
			pMyBios->SetMotherboardManufacturer( NOT_AVAILABLE);
			bManufacturer = FALSE;
		}
		else
		{
			pMyBios->SetMotherboardManufacturer( csValue);
			bManufacturer = TRUE;
		}
		// Get Motherboard model
		lResult = GetValue( hKey, NT_BASEBOARD_MODEL_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					NT_BIOS_KEY, NT_BASEBOARD_MODEL_VALUE);
			pMyBios->SetMotherboardModel( NOT_AVAILABLE);
			bModel = FALSE;
		}
		else
		{
			pMyBios->SetMotherboardModel( csValue);
			bModel = TRUE;
		}
		// Get machine type
		lResult = GetValue( hKey, NT_BIOS_MACHINE_TYPE_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					NT_BIOS_KEY, NT_BIOS_MACHINE_TYPE_VALUE);
			pMyBios->SetMachineType( NOT_AVAILABLE);
		}
		else
		{
			pMyBios->SetMachineType( csValue);
		}
		AddLog( _T( "\t\t<System Manufacturer: %s><System Model: %s><System S/N: %s><BIOS Manufacturer: %s><BIOS Version: %s><BIOS Date: %s><Baseboard Manufacturer: %s><Baseboard Model: %s><Type: %s>\n"), 
			pMyBios->GetSystemManufacturer(), pMyBios->GetSystemModel(), pMyBios->GetSystemSerialNumber(), pMyBios->GetBiosManufacturer(),
			pMyBios->GetBiosVersion(), pMyBios->GetBiosDate(), pMyBios->GetMotherboardManufacturer(), pMyBios->GetMotherboardModel(), 
			pMyBios->GetMachineType());
		RegCloseKey( hKey);
	}
	else
	{
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_BIOS_KEY);
		pMyBios->Set( NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE,
					  NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE,
					  NOT_AVAILABLE, NOT_AVAILABLE);
	}
	AddLog( _T( "\tOK.\n"));
	return TRUE;
}

BOOL CRegistry::GetBiosInfoVista( CBios *pMyBios)
{
	HKEY	hKey = NULL;
	CString	csValue;
	LONG	lResult;
	BOOL	bManufacturer = FALSE,
			bModel = FALSE;

	AddLog( _T( "Registry Vista GetBiosInfo: Trying to find BIOS in HKLM\\%s...\n"), NT_BIOS_KEY);
	// Windows NT
	lResult = RegOpenKeyEx( m_hKey, VISTA_BIOS_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKey);
	if (lResult == ERROR_SUCCESS)
	{
		// Get System manufacturer
		lResult = GetValue( hKey, VISTA_SYSTEM_MANUFACTURER_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					VISTA_BIOS_KEY, VISTA_SYSTEM_MANUFACTURER_VALUE);
			pMyBios->SetSystemManufacturer( NOT_AVAILABLE);
			bManufacturer = FALSE;
		}
		else
		{
			pMyBios->SetSystemManufacturer( csValue);
			bManufacturer = TRUE;
		}
		// Get System model
		lResult = GetValue( hKey, VISTA_SYSTEM_MODEL_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					VISTA_BIOS_KEY, VISTA_SYSTEM_MODEL_VALUE);
			pMyBios->SetSystemModel( NOT_AVAILABLE);
			bModel = FALSE;
		}
		else
		{
			pMyBios->SetSystemModel( csValue);
			bModel = TRUE;
		}
		// Get System serial number
		lResult = GetValue( hKey, VISTA_SYSTEM_SERIAL_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					VISTA_BIOS_KEY, VISTA_SYSTEM_SERIAL_VALUE);
			pMyBios->SetSystemSerialNumber( NOT_AVAILABLE);
		}
		else
		{
			pMyBios->SetSystemSerialNumber( csValue);
		}
		// Get BIOS manufacturer
		lResult = GetValue( hKey, NT_BIOS_MANUFACTURER_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					VISTA_BIOS_KEY, VISTA_BIOS_MANUFACTURER_VALUE);
			pMyBios->SetBiosManufacturer( NOT_AVAILABLE);
		}
		else
		{
			pMyBios->SetBiosManufacturer( csValue);
		}
		// Get BIOS version
		lResult = GetValue( hKey, VISTA_BIOS_VERSION_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					VISTA_BIOS_KEY, VISTA_BIOS_VERSION_VALUE);
			pMyBios->SetBiosVersion( NOT_AVAILABLE);
		}
		else
		{
			pMyBios->SetBiosVersion( csValue);
		}
		// Get BIOS date
		lResult = GetValue( hKey, VISTA_BIOS_DATE_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					VISTA_BIOS_KEY, VISTA_BIOS_DATE_VALUE);
			pMyBios->SetBiosDate( NOT_AVAILABLE);
		}
		else
		{
			pMyBios->SetBiosDate( csValue);
		}
		// Get Motherboard manufacturer
		lResult = GetValue( hKey, VISTA_BASEBOARD_MANUFACTURER_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					NT_BIOS_KEY, VISTA_BASEBOARD_MANUFACTURER_VALUE);
			pMyBios->SetMotherboardManufacturer( NOT_AVAILABLE);
			bManufacturer = FALSE;
		}
		else
		{
			pMyBios->SetMotherboardManufacturer( csValue);
			bManufacturer = TRUE;
		}
		// Get Motherboard model
		lResult = GetValue( hKey, VISTA_BASEBOARD_MODEL_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					NT_BIOS_KEY, VISTA_BASEBOARD_MODEL_VALUE);
			pMyBios->SetMotherboardModel( NOT_AVAILABLE);
			bModel = FALSE;
		}
		else
		{
			pMyBios->SetMotherboardModel( csValue);
			bModel = TRUE;
		}
		// Get machine type
		lResult = GetValue( hKey, VISTA_BIOS_MACHINE_TYPE_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					VISTA_BIOS_KEY, VISTA_BIOS_MACHINE_TYPE_VALUE);
			pMyBios->SetMachineType( NOT_AVAILABLE);
		}
		else
		{
			pMyBios->SetMachineType( csValue);
		}
		AddLog( _T( "\t\t<System Manufacturer: %s><System Model: %s><System S/N: %s><BIOS Manufacturer: %s><BIOS Version: %s><BIOS Date: %s><Baseboard Manufacturer: %s><Baseboard Model: %s><Type: %s>\n"), 
			pMyBios->GetSystemManufacturer(), pMyBios->GetSystemModel(), pMyBios->GetSystemSerialNumber(), pMyBios->GetBiosManufacturer(),
			pMyBios->GetBiosVersion(), pMyBios->GetBiosDate(), pMyBios->GetMotherboardManufacturer(), pMyBios->GetMotherboardModel(), 
			pMyBios->GetMachineType());
		RegCloseKey( hKey);
	}
	else
	{
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				VISTA_BIOS_KEY);
		pMyBios->Set( NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE,
					  NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE,
					  NOT_AVAILABLE, NOT_AVAILABLE);
	}
	AddLog( _T( "\tOK.\n"));
	return TRUE;
}

BOOL CRegistry::IsNotebook()
{
	// Try to get Battery Informations info from the registry
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return IsNotebookNT();
	default:
		// Unknown
		AddLog( _T( "Registry IsNotebook...\n\tFailed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::IsNotebookNT()
{
	HKEY			hKeyEnum;
	TCHAR			szDeviceName[256];
	DWORD			dwLength,
					dwIndexEnum = 0;
	LONG			lResult;
	FILETIME		MyFileTime;

	AddLog( _T( "Registry NT IsNotebook: Trying to find Battery in HKLM\\%s...\n"), NT_NOTEBOOK_KEY);
	// Windows NT => Open the battery key
	if (RegOpenKeyEx( m_hKey, NT_NOTEBOOK_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szDeviceName, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// There is at least one sub key
			// Enum the next device
			dwLength = 255;
			dwIndexEnum++;
		}
		RegCloseKey( hKeyEnum);
		if (dwIndexEnum == 0)
			// No key found
			AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_NOTEBOOK_KEY);
		else
			AddLog( _T( "\tOK (%lu objects).\n"), dwIndexEnum);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_NOTEBOOK_KEY);
	return (dwIndexEnum > 0);
}

DWORD CRegistry::GetProcessors(CString &csProcType, CString &csProcSpeed)
{
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetProcessorsNT( csProcType, csProcSpeed);
	default:
		// Unknown
		csProcType = NOT_AVAILABLE;
		csProcSpeed = NOT_AVAILABLE;
		AddLog( _T( "Registry GetProcessors...\n\tFailed because unsupported or unrecognized OS !\n"));
		return 0;
	}
	return 0;
}

DWORD CRegistry::GetProcessorsNT(CString &csProcType, CString &csProcSpeed)
{
	HKEY			hKeyEnum,
					hKeyObject;
	CString			csSubKey;
	TCHAR			szDeviceName[256];
	DWORD			dwLength,
					dwIndexEnum = 0,
					dwIndex = 0;
	LONG			lResult;
	FILETIME		MyFileTime;
	BOOL			bHaveToStore;

	AddLog( _T( "Registry NT GetProcessors: Trying to find Processors in HKLM\\%s\n"), NT_PROCESSOR_KEY);
	// Windows NT => Open the processor key
	if (RegOpenKeyEx( m_hKey, NT_PROCESSOR_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szDeviceName, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szDeviceName[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_PROCESSOR_KEY, szDeviceName);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyObject) == ERROR_SUCCESS)
			{
				csProcType = NOT_AVAILABLE; 
				csProcSpeed = NOT_AVAILABLE;
				// Read the processor name
				if (GetValue( hKeyObject, NT_PROCESSOR_NAME_STRING_VALUE, csProcType) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_PROCESSOR_NAME_STRING_VALUE);
					dwLength = 255;
					if (GetValue( hKeyObject, NT_PROCESSOR_NAME_VALUE, csProcType) == ERROR_SUCCESS)
					{
						bHaveToStore = TRUE;
					}
					else
					{
						AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
										   csSubKey, NT_PROCESSOR_NAME_VALUE);
						csProcType = NOT_AVAILABLE;
					}
				}
				// Read the speed in MHz
				if (GetValue( hKeyObject, NT_PROCESSOR_SPEED_VALUE, csProcSpeed) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_PROCESSOR_SPEED_VALUE);
					csProcSpeed = NOT_AVAILABLE;
				}
				RegCloseKey( hKeyObject);
				// Add the device to the adapter list
				if (bHaveToStore)
				{
					StrForSQL( csProcType);
					StrForSQL( csProcSpeed);
					dwIndex ++;
				}
				AddLog( _T( "\t\t<Type: %s><Speed: %s MHz>\n"), 
					csProcType, csProcSpeed);
			}
			else
				AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
								   csSubKey);
			// Enum the next device
			dwLength = 255;
			dwIndexEnum++;
		}
		RegCloseKey( hKeyEnum);
		if (dwIndexEnum == 0)
			// No key found
			AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_PROCESSOR_KEY);
		else
			AddLog( _T( "\tOK (%lu objects)\n"), dwIndex);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_PROCESSOR_KEY);
	AddLog( _T( "Registry NT GetProcessors: %lu processor(s) found.\n"), dwIndex);
	return dwIndex;
}

DWORD CRegistry::GetCPU( CCpuList *pMyList)
{
	HKEY			hKeyEnum,
					hKeyObject;
	CString			csSubKey, csManufacturer, csType, csSpeed;
	TCHAR			szDeviceName[256];
	DWORD			dwLength,
					dwIndexEnum = 0,
					dwIndex = 0;
	LONG			lResult;
	FILETIME		MyFileTime;
	BOOL			bHaveToStore;
	CCpu            myObject;

	AddLog( _T( "Registry NT GetCPU: Trying to find CPU in HKLM\\%s...\n"), NT_PROCESSOR_KEY);
	// Windows NT => Open the processor key
	if (RegOpenKeyEx( m_hKey, NT_PROCESSOR_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szDeviceName, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szDeviceName[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_PROCESSOR_KEY, szDeviceName);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyObject) == ERROR_SUCCESS)
			{
				csManufacturer = NOT_AVAILABLE; 
				csType = NOT_AVAILABLE; 
				csSpeed = NOT_AVAILABLE;
				// Read the manufacturer
				if (GetValue( hKeyObject, NT_PROCESSOR_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_PROCESSOR_SPEED_VALUE);
					csManufacturer = NOT_AVAILABLE;
				}
				// Read the processor name
				if (GetValue( hKeyObject, NT_PROCESSOR_NAME_STRING_VALUE, csType) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_PROCESSOR_NAME_STRING_VALUE);
					dwLength = 255;
					if (GetValue( hKeyObject, NT_PROCESSOR_NAME_VALUE, csType) == ERROR_SUCCESS)
					{
						bHaveToStore = TRUE;
					}
					else
					{
						AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
										   csSubKey, NT_PROCESSOR_NAME_VALUE);
						csType = NOT_AVAILABLE;
					}
				}
				// Read the speed in MHz
				if (GetValue( hKeyObject, NT_PROCESSOR_SPEED_VALUE, csSpeed) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_PROCESSOR_SPEED_VALUE);
					csSpeed = NOT_AVAILABLE;
				}
				RegCloseKey( hKeyObject);
				myObject.Clear();
				StrForSQL( csManufacturer);
				myObject.SetManufacturer( csManufacturer);
				StrForSQL( csType);
				myObject.SetName( csType);
				StrForSQL( csSpeed);
				myObject.SetMaxSpeed( _ttoi( csSpeed));
				// Assume current speed to max speed
				myObject.SetCurrentSpeed( _ttoi( csSpeed));
				// Assume x86 architecture
				myObject.SetArchitecture( _T( "x86"));
				// Assume 1 core and 1 logical CPU
				myObject.SetNumberOfCores( 1);
				myObject.SetNumberOfLogicalProcessors( 1);
				// Assume 32 bits OS and processor
				myObject.SetAddressWith( 32);
				myObject.SetDataWidth( 32);
				AddLog( _T( "\t\t<Manufacturer: %s><Name: %s><Socket: %s><Architecture: %s><NumberOfCores: %u><NumberOfLogicalProcessors: %u><CurrentClockSpeed: %u><MaxClockSpeed: %u><L2CacheSize: %u><AddressWidth: %u><DataWidth: %u><VoltageCaps: %s><CpuStatus: %s>\n"), 
					myObject.GetManufacturer(), myObject.GetName(), myObject.GetSocket(), myObject.GetArchitecture(),
					myObject.GetNumberOfCores(), myObject.GetNumberOfLogicalProcessors(), myObject.GetCurrentClockSpeed(),
					myObject.GetMaxClockSpeed(), myObject.GetL2CacheSize(), myObject.GetAddressWidth(), myObject.GetDataWidth(),
					myObject.GetVoltage(), myObject.GetStatus());
				// Add the device to the adapter list
				if (bHaveToStore)
				{
					pMyList->AddTail( myObject);
					dwIndex ++;
				}
			}
			else
				AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
								   csSubKey);
			// Enum the next device
			dwLength = 255;
			dwIndexEnum++;
		}
		RegCloseKey( hKeyEnum);
		if (dwIndexEnum == 0)
			// No key found
			AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_PROCESSOR_KEY);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_PROCESSOR_KEY);
	AddLog( _T( "\tOK( %lu objects)\n"), dwIndex);
	return dwIndex;
}

BOOL CRegistry::GetCPUName( UINT uCPUID, CString &csName)
{
	HKEY			hKeyObject;
	CString			csKey;

	AddLog( _T( "Registry NT GetCPUName for CPUID %u..."), uCPUID);
	// Windows NT => Open the processor key
	csKey.Format( _T( "%s\\%u"), NT_PROCESSOR_KEY, uCPUID);
	if (RegOpenKeyEx( m_hKey, csKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyObject) == ERROR_SUCCESS)
	{
		if (GetValue( hKeyObject, NT_PROCESSOR_NAME_STRING_VALUE, csName) == ERROR_SUCCESS)
		{
			RegCloseKey( hKeyObject);
			AddLog( _T( "OK (%s)\n"), csName);
			return TRUE;
		}
		RegCloseKey( hKeyObject);
		AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
						   csKey, NT_PROCESSOR_NAME_STRING_VALUE);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
						   csKey);
	return FALSE;
}

BOOL CRegistry::GetPrinters(CPrinterList *pList)
{
	HKEY			hKeyEnum,
					hKeyDevice;
	CString			csSubKey,
					csPrinterName,
					csDriver,
					csPort;
    TCHAR			szDeviceName[256];
	DWORD			dwLength,
					dwIndexEnum = 0;
	LONG			lResult;
	FILETIME		MyFileTime;
	BOOL			bHaveToStore;
	UINT			uIndex = 0;

	ASSERT( pList);

	AddLog( _T( "Registry GetPrinters: Trying to find Printer in HKLM\\%s...\n"), PRINTERS_ENUM_KEY);
	// Reset object list content
	while (!(pList->GetCount() == 0))
		pList->RemoveHead();
	// Open the Printer key
	if (RegOpenKeyEx( m_hKey, PRINTERS_ENUM_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find printers
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szDeviceName, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			CPrinter	cPrinter;

			// For each object, Try to open the device key
			szDeviceName[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), PRINTERS_ENUM_KEY, szDeviceName);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyDevice) == ERROR_SUCCESS)
			{

				// OK => Read the Printer description
				if (GetValue( hKeyDevice, PRINTER_NAME_VALUE, csPrinterName) == ERROR_SUCCESS)
				{
					// Store the printer
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, PRINTER_NAME_VALUE);
					csPrinterName = NOT_AVAILABLE;
				}
				// Read the Printer Driver
				if (GetValue( hKeyDevice, PRINTER_DRIVER_VALUE, csDriver) == ERROR_SUCCESS)
				{
					// Store the printer
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, PRINTER_DRIVER_VALUE);
					csDriver = NOT_AVAILABLE; 
				}
				// Read the Printer Port
				if (GetValue( hKeyDevice, PRINTER_PORT_VALUE, csPort) != ERROR_SUCCESS)
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, PRINTER_PORT_VALUE);
					csPort = NOT_AVAILABLE; 
				}
				RegCloseKey( hKeyDevice);
				cPrinter.SetName( csPrinterName);
				cPrinter.SetDriver( csDriver);
				cPrinter.SetPort( csPort);
				AddLog( _T( "\t\t<Name: %s><Driver: %s><Port: %s>\n"), 
					cPrinter.GetName(), cPrinter.GetDriver(), cPrinter.GetPort());
				// Add the device to the printer list if needed
				if (bHaveToStore)
				{
					pList->AddTail( cPrinter);
					uIndex ++;
				}
			}
			else
				AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
								   csSubKey);
			// Enum the next device
			dwIndexEnum++;
			dwLength = 255;
		}
		RegCloseKey( hKeyEnum);
		if (dwIndexEnum == 0)
			// No key found
			AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find Printer subkey.\n"));
		else
			AddLog( _T( "\tOK (%u objects).\n"), uIndex);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"), PRINTERS_ENUM_KEY);
	return TRUE;
}

BOOL CRegistry::GetVideoAdapters(CVideoAdapterList *pList)
{
	ASSERT( pList);

	// Reset object list content
	while (!(pList->GetCount() == 0))
		pList->RemoveHead();
	// Try to get Display Adapter info from the registry
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		if (m_bIsXP)
			return GetVideoAdaptersXP( pList);
		return GetVideoAdaptersNT_2K( pList);
	default:
		// Unknown
		AddLog( _T( "Registry GetVideoAdapters...\n\tFailed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::GetVideoAdaptersNT_2K(CVideoAdapterList *pList)
{
	HKEY			hKeyEnum,
					hKeyGroup,
					hKeyObject,
					hKeyProperty;
	CString			csSubKey,
					csAdapterName,
					csAdapterChip,
					csClassName,
					csServiceName,
					csResolution;
	TCHAR			szGroupName[256],
					szDeviceName[256],
					szPropertyName[256];
	DWORD			dwLength,
					dwMemory,
					dwIndexEnum = 0,
					dwIndexGroup = 0,
					dwIndexProperties = 0;
	LONG			lResult,
					lAdapterMemory;
	FILETIME		MyFileTime;
	CVideoAdapter	myObject;
	BOOL			bHaveToStore;
	UINT			uIndex = 0;

	ASSERT( pList);

	AddLog( _T( "Registry NT GetVideoAdapters: Trying to find Video Adapter in HKLM\\%s...\n"), NT_ENUM_KEY);
	// Windows NT =>  Browse the active service keys to find the Display service
	// Enumerate service groups under HKLM\System\CurrentControlSet\Enum
	if (RegOpenKeyEx( m_hKey, NT_ENUM_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnum) == ERROR_SUCCESS)
	{
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szGroupName, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each group, enumerate service keys
			szGroupName[dwLength] = 0;
			csSubKey.Format( _T( "%s\\%s"), NT_ENUM_KEY, szGroupName);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyGroup) == ERROR_SUCCESS)
			{
				dwLength = 255;
				dwIndexGroup = 0;
				while ((lResult = RegEnumKeyEx( hKeyGroup, dwIndexGroup, szDeviceName, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
				{
					// For each service, enumerate propertie keys
					szDeviceName[dwLength] = 0;
					csSubKey.Format( _T( "%s\\%s\\%s"), NT_ENUM_KEY, szGroupName, szDeviceName);
					if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyObject) == ERROR_SUCCESS)
					{
						dwLength = 255;
						dwIndexProperties = 0;
						while ((lResult = RegEnumKeyEx( hKeyObject, dwIndexProperties, szPropertyName, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
						{
							// If a display class key, read the associated service
							szPropertyName[dwLength] = 0;
							csSubKey.Format( _T( "%s\\%s\\%s\\%s"), NT_ENUM_KEY, szGroupName, szDeviceName, szPropertyName);
							if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyProperty) == ERROR_SUCCESS)
							{
								// Read the class
								if (GetValue( hKeyProperty, NT_ENUM_CLASS_VALUE, csClassName) != ERROR_SUCCESS)
								{
									// Cannot read the class name
									RegCloseKey( hKeyProperty);
									dwIndexProperties ++;
									dwLength = 255;
									continue;
								}
								else
									AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
													   csSubKey, NT_ENUM_CLASS_VALUE);
								if (csClassName.CompareNoCase( NT_ENUM_DISPLAY_VALUE) != 0)
								{
									// Not a display service
									dwIndexProperties ++;
									dwLength = 255;
									continue;
								}
								// We have found the display service => get the service name
								if (GetValue( hKeyProperty, NT_ENUM_SERVICE_VALUE, csServiceName) != ERROR_SUCCESS)
								{
									// Cannot read the associated service name
									RegCloseKey( hKeyProperty);
									dwIndexProperties ++;
									dwLength = 255;
									continue;
								}
								else
									AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
													   csSubKey, NT_ENUM_SERVICE_VALUE);
								// We know the service name => get the Graphic Adapter infos
								RegCloseKey( hKeyProperty);
								csSubKey.Format( _T( "%s\\%s\\%s"), NT_SERVICES_KEY, csServiceName, NT_SERVICE_DEVICE_KEY);
								csAdapterName = NOT_AVAILABLE;
								csAdapterChip = NOT_AVAILABLE;
								lAdapterMemory = -1;
								bHaveToStore = FALSE;
								if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyProperty) == ERROR_SUCCESS)
								{
									dwLength = 255;
									// Get the Graphic Adapter name
									if (GetValue( hKeyProperty, NT_ADAPTER_NAME_VALUE, csAdapterName) == ERROR_SUCCESS)
									{
										// The value is an array of string, each string only containing one character
										// Build a real string by copying each string to one
										CString csAdapter;
										csAdapter.Format( _T( "%S"), csAdapterName);
										csAdapterName = csAdapter;
										bHaveToStore = TRUE;
									}
									else
									{
										AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
														   csSubKey, NT_ADAPTER_NAME_VALUE);
										csAdapterName = NOT_AVAILABLE;
									}
									// Get the Graphic Adapter Chipset
									if (GetValue( hKeyProperty, NT_ADAPTER_CHIP_VALUE, csAdapterChip) == ERROR_SUCCESS)
									{
										// The value is an array of string, each string only containing one character
										// Build a real string by copying each string to one
										CString csChip;
										csChip.Format( _T( "%S"), csAdapterChip);
										csAdapterChip = csChip;
										bHaveToStore = TRUE;
									}
									else
									{
										AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
														   csSubKey, NT_ADAPTER_CHIP_VALUE);
										csAdapterChip = NOT_AVAILABLE;
									}
									// Get the Graphic Adapter Memory size
									if (GetValue( hKeyProperty, NT_ADAPTER_MEMORY_VALUE, &dwMemory) == ERROR_SUCCESS)
										lAdapterMemory = dwMemory/ONE_MEGABYTE;
									else
									{
										AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
														   csSubKey, NT_ADAPTER_MEMORY_VALUE);
										lAdapterMemory = -1;
									}
									// Get screen resolution
									GetScreenResolution( csResolution);
									myObject.SetName( csAdapterName);
									myObject.SetChipset( csAdapterChip);
									myObject.SetMemory( lAdapterMemory);
									myObject.SetScreenResolution( csResolution);
									AddLog( _T( "\t\t<Description: %s><VideoProcessor: %s><Memory: %s><Resolution: %s>\n"), 
										myObject.GetName(), myObject.GetChipset(), myObject.GetMemory(), myObject.GetScreenResolution());
									// Add the device to the adapter lis
									if (bHaveToStore)
									{
										pList->AddTail( myObject);
										uIndex ++;
									}
									RegCloseKey( hKeyProperty);
								} // if RegOpenKey NT_SERVICE_KEY
								else
									AddLog( _T( "\tFailed in call to <RegOpenKeyEx> for HKLM\\%s !\n"),
													   csSubKey);
							} // if RegOpenKey Property Key
							else
								AddLog( _T( "\tFailed in call to <RegOpenKeyEx> for HKLM\\%s !\n"),
												   csSubKey);
							dwIndexProperties++;
							dwLength = 255;
						} // while RegEnumKey Properties
						RegCloseKey( hKeyObject);
						if (dwIndexProperties == 0)
							// No key found
							AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
											   csSubKey);
					} // if RegOpenKey Object Key
					else
						AddLog( _T( "\t\tFailed in call to <RegOpenKeyEx> for HKLM\\%s !\n"),
										   csSubKey);
					dwIndexGroup ++;
					dwLength = 255;
				} // while RegEnumKey Group
				RegCloseKey( hKeyGroup);
				if (dwIndexGroup == 0)
					// No key found
					AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
									   csSubKey);
			} // if RegOpenKey Group Key
			else
				AddLog( _T( "\t\tFailed in call to <RegOpenKeyEx> for HKLM\\%s !\n"),
								   csSubKey);
			dwIndexEnum++;
			dwLength = 255;
		} // while RegEnumKey Enum
		RegCloseKey( hKeyEnum);
		if (dwIndexEnum == 0)
			// No key found
			AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
				    NT_ENUM_KEY);
		else
			AddLog( _T( "\tOK (%u objects).\n"), uIndex);
	} // if RegOpenKey enum Key
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_ENUM_KEY);
	return TRUE;
}

BOOL CRegistry::GetVideoAdaptersXP(CVideoAdapterList *pList)
{
	HKEY			hKeyEnum,
					hKeyGroup,
					hKeyProperty;
	CString			csSubKey,
					csAdapterName,
					csAdapterChip,
					csResolution;
	TCHAR			szGroupName[256],
					szDeviceName[256];
	DWORD			dwLength,
					dwMemory,
					dwIndexEnum = 0,
					dwIndexGroup = 0;
	LONG			lResult,
					lAdapterMemory;
	FILETIME		MyFileTime;
	CVideoAdapter	myObject;
	BOOL			bHaveToStore;
	UINT			uIndex = 0;

	ASSERT( pList);

	AddLog( _T( "Registry XP GetVideoAdapters: Trying to find Video Adapter in HKLM\\%s...\n"), XP_ENUM_KEY);
	// Windows XP =>  Browse the active service keys to find the Display service
	// Enumerate device groups under HKLM\System\CurrentControlSet\Control\Video
	if (RegOpenKeyEx( m_hKey, XP_ENUM_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnum) == ERROR_SUCCESS)
	{
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szGroupName, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each group, enumerate device keys
			szGroupName[dwLength] = 0;
			csSubKey.Format( _T( "%s\\%s"), XP_ENUM_KEY, szGroupName);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyGroup) == ERROR_SUCCESS)
			{
				dwLength = 255;
				dwIndexGroup = 0;
				while ((lResult = RegEnumKeyEx( hKeyGroup, dwIndexGroup, szDeviceName, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
				{
					// For each device, get propertie keys
					szDeviceName[dwLength] = 0;
					csSubKey.Format( _T( "%s\\%s\\%s"), XP_ENUM_KEY, szGroupName, szDeviceName);
					if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyProperty) == ERROR_SUCCESS)
					{
						bHaveToStore = FALSE;
						// Get the Graphic Adapter name
						if (GetValue( hKeyProperty, XP_ADAPTER_NAME_VALUE, csAdapterName) == ERROR_SUCCESS)
						{
							// The value is an array of string, each string only containing one character
							// Build a real string by copying each string to one
							CString csAdapter;
							csAdapter.Format( _T( "%S"), csAdapterName);
							csAdapterName = csAdapter;
							bHaveToStore = TRUE;
						}
						else
						{
							AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
											   csSubKey, XP_ADAPTER_NAME_VALUE);
							csAdapterName = NOT_AVAILABLE;
						}
						// Get the Graphic Adapter Chipset
						if (GetValue( hKeyProperty, XP_ADAPTER_CHIP_VALUE, csAdapterChip) == ERROR_SUCCESS)
						{
							// The value is an array of string, each string only containing one character
							// Build a real string by copying each string to one
							CString csChip;
							csChip.Format( _T( "%S"), csAdapterChip);
							csAdapterChip = csChip;
							bHaveToStore = TRUE;
						}
						else
						{
							AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
											   csSubKey, XP_ADAPTER_CHIP_VALUE);
							csAdapterChip = NOT_AVAILABLE;
						}
						// Get the Graphic Adapter Memory size
						if (GetValue( hKeyProperty, XP_ADAPTER_MEMORY_VALUE, &dwMemory) == ERROR_SUCCESS)
							lAdapterMemory = dwMemory/ONE_MEGABYTE;
						else
						{
							AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
											   csSubKey, XP_ADAPTER_MEMORY_VALUE);
							lAdapterMemory = -1;
						}
						// Get screen resolution
						GetScreenResolution( csResolution);
						myObject.SetName( csAdapterName);
						myObject.SetChipset( csAdapterChip);
						myObject.SetMemory( lAdapterMemory);
						myObject.SetScreenResolution( csResolution);
						AddLog( _T( "\t\t<Description: %s><VideoProcessor: %s><Memory: %s><Resolution: %s>\n"), 
							myObject.GetName(), myObject.GetChipset(), myObject.GetMemory(), myObject.GetScreenResolution());
						// Add the device to the adapter lis
						if (bHaveToStore)
						{
							pList->AddTail( myObject);
							uIndex ++;
						}
						RegCloseKey( hKeyProperty);
					} // if RegOpenKey Object Key
					else
						AddLog( _T( "\tFailed in call to <RegOpenKeyEx> for HKLM\\%s !\n"),
										   csSubKey);
					dwIndexGroup ++;
					dwLength = 255;
				} // while RegEnumKey Group
				RegCloseKey( hKeyGroup);
				if (dwIndexGroup == 0)
					// No key found
					AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
							csSubKey);
			} // if RegOpenKey Group Key
			else
				AddLog( _T( "\tFailed in call to <RegOpenKeyEx> for HKLM\\%s !\n"),
						csSubKey);
			dwIndexEnum++;
			dwLength = 255;
		} // while RegEnumKey Enum
		RegCloseKey( hKeyEnum);
		if (dwIndexEnum == 0)
		{
			// No key found
			AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
				   NT_ENUM_KEY);
			return FALSE;
		}
		AddLog( _T( "\tOK (%u objects).\n"), uIndex);
		return TRUE;
	} // if RegOpenKey enum Key
	AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"), NT_ENUM_KEY);
	return FALSE;
}

BOOL CRegistry::GetScreenResolution(CString &csResolution)
{
	int nScreenWidth,
		nScreenHeight;

	csResolution = NOT_AVAILABLE;
	if ((nScreenWidth = GetSystemMetrics(SM_CXSCREEN)) == 0)
		return FALSE;
	if ((nScreenHeight = GetSystemMetrics(SM_CYSCREEN)) == 0)
		return FALSE;
	csResolution.Format( _T( "%d x %d"), nScreenWidth, nScreenHeight);
	return TRUE;
}

BOOL CRegistry::GetSoundDevices(CSoundDeviceList *pList)
{
	ASSERT( pList);

	// Reset object list content
	while (!(pList->GetCount() == 0))
		pList->RemoveHead();
	// Try to get Sound Devices Informations info from the registry
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetSoundDevicesNT( pList);
	default:
		// Unknown
		AddLog( _T( "Registry GetSoundDevices...\n\tFailed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::GetSoundDevicesNT(CSoundDeviceList *pList)
{
	HKEY			hKeyEnum,
					hKeyObject,
					hKeyDriver;
	CString			csDeviceName,
					csProviderName,
					csDescription,
					csSubKey;
	TCHAR			szKey[256];
	DWORD			dwLength,
					dwIndexEnum = 0;
	LONG			lResult;
	FILETIME		MyFileTime;
	CSoundDevice	myObject;
	BOOL			bHaveToStore;
	UINT			uIndex = 0;

	ASSERT( pList);

	AddLog( _T( "Registry NT GetSoundDevices: Trying to find Sound Device in HKLM\\%s...\n"), NT_SOUND_KEY);
	// Windows NT => Open the sound key
	if (RegOpenKeyEx( m_hKey, NT_SOUND_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_SOUND_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyObject) == ERROR_SUCCESS)
			{
				csProviderName = NOT_AVAILABLE;
				csDeviceName = NOT_AVAILABLE;
				csDescription = NOT_AVAILABLE;
				// OK => Read the provider
				if (GetValue( hKeyObject, NT_SOUND_MANUFACTURER_VALUE, csProviderName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_SOUND_MANUFACTURER_VALUE);
					csProviderName = NOT_AVAILABLE;
				}
				// Read the name
				if (GetValue( hKeyObject, NT_SOUND_NAME_VALUE, csDeviceName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_SOUND_NAME_VALUE);
					csDeviceName = NOT_AVAILABLE;
				}
				// Read the description
				if (GetValue( hKeyObject, NT_SOUND_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_SOUND_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE;
				}
				RegCloseKey( hKeyObject);
				myObject.SetManufacturer( csProviderName);
				myObject.SetName( csDeviceName);
				myObject.SetDescription( csDescription);
				// Check if there is a Drivers sub key to validate
				csSubKey += _T( "\\");
				csSubKey += NT_SOUND_DRIVER;
				if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyDriver) == ERROR_SUCCESS)
				{
					// OK => Try to read the SubClasses value
					if (GetValue( hKeyDriver, NT_SOUND_DRIVER_CLASS, csProviderName) != ERROR_SUCCESS)
						// No value => skip
						bHaveToStore = FALSE;
					RegCloseKey( hKeyDriver);
				}
				else
					// No Driver sub key => skip
					bHaveToStore = FALSE;
				AddLog( _T( "\t\t<Manufacturer: %s><Name: %s><Description: %s><SubClass: %s>\n"), 
					myObject.GetManufacturer(), myObject.GetName(), myObject.GetDescription(), csProviderName);
				// Add the device to the adapter list
				if (bHaveToStore)
				{
					pList->AddTail( myObject);
					uIndex ++;
				}
			}
			else
				AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
								   csSubKey);
			// Enum the next device
			dwLength = 255;
			dwIndexEnum++;
		}
		RegCloseKey( hKeyEnum);
		if (dwIndexEnum == 0)
			// No key found
			AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_SOUND_KEY);
		else
			AddLog( _T( "\tOK (%u objects).\n"), uIndex);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_SOUND_KEY);
	return !pList->IsEmpty();
}

BOOL CRegistry::GetModems(CModemList *pList)
{
	ASSERT( pList);

	// Reset object list content
	while (!(pList->GetCount() == 0))
		pList->RemoveHead();
	// Try to get Modems Informations info from the registry
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetModemsNT( pList);
	default:
		// Unknown
		AddLog( _T( "Registry GetModems...\n\tFailed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::GetModemsNT(CModemList *pList)
{
	HKEY			hKeyEnum,
					hKeyObject;
	CString			csSubKey,
					csModel,
					csDescription,
					csDeviceName,
					csType;
	TCHAR			szKey[256];
	DWORD			dwLength,
					dwIndexEnum = 0;
	LONG			lResult;
	FILETIME		MyFileTime;
	CModem			myObject;
	BOOL			bHaveToStore;
	UINT			uIndex = 0;

	ASSERT( pList);

	AddLog( _T( "Registry NT GetModems: Trying to find Modem in HKLM\\%s...\n"), NT_MODEM_KEY);
	// Windows NT => Open the modem key
	if (RegOpenKeyEx( m_hKey, NT_MODEM_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_MODEM_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyObject) == ERROR_SUCCESS)
			{
				csType = NOT_AVAILABLE; 
				csDeviceName = NOT_AVAILABLE; 
				csModel = NOT_AVAILABLE;
				csDescription = NOT_AVAILABLE; 
				// Read the type, i.e the COM port attached to
				if (GetValue( hKeyObject, NT_MODEM_TYPE_VALUE, csType) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_MODEM_TYPE_VALUE);
					csType = NOT_AVAILABLE; 
				}
				// Read the name
				if (GetValue( hKeyObject, NT_MODEM_NAME_VALUE, csDeviceName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_MODEM_NAME_VALUE);
					csDeviceName = NOT_AVAILABLE; 
				}
				// Read the model
				if (GetValue( hKeyObject, NT_MODEM_MODEL_VALUE, csModel) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_MODEM_MODEL_VALUE);
					csModel = NOT_AVAILABLE;
				}
				// Read the description
				if (GetValue( hKeyObject, NT_MODEM_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_MODEM_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				RegCloseKey( hKeyObject);
				myObject.SetType( csType);
				myObject.SetName( csDeviceName);
				myObject.SetModel( csModel);
				myObject.SetDescription( csDescription);
				AddLog( _T( "\t\t<Type: %s><Name: %s><Model: %s><Description: %s>\n"), 
					myObject.GetType(), myObject.GetName(), myObject.GetModel(),
					myObject.GetDescription());
				// Add the device to the adapter list
				if (bHaveToStore)
				{
					pList->AddTail( myObject);
					uIndex ++;
				}
			}
			else
				AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
								   csSubKey);
			// Enum the next device
			dwLength = 255;
			dwIndexEnum++;
		}
		RegCloseKey( hKeyEnum);
		if (dwIndexEnum == 0)
			// No key found
			AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_MODEM_KEY);
		else
			AddLog( _T( "\tOK (%u objects).\n"), uIndex);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_MODEM_KEY);
	return !pList->IsEmpty();
}

BOOL CRegistry::GetMonitors(CMonitorList *pList)
{
	ASSERT( pList);

	// Reset object list content
	while (!(pList->GetCount() == 0))
		pList->RemoveHead();
	// Try to get Monitors Informations info from the registry
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetMonitorsNT( pList);
	default:
		// Unknown
		AddLog( _T( "Registry GetMonitors...\n\tFailed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::GetMonitorsNT(CMonitorList *pList)
{
	HKEY			hKeyEnum,
					hKeyObject;
	CString			csSubKey,
					csModel,
					csDescription,
					csManufacturer,
					csCaption;
	TCHAR			szKey[256];
	DWORD			dwLength,
					dwIndexEnum = 0;
	LONG			lResult;
	FILETIME		MyFileTime;
	CMonitor		myObject;
	BOOL			bHaveToStore;
	UINT			uIndex = 0;

	ASSERT( pList);

	AddLog( _T( "Registry NT GetMonitors: Trying to find Monitor in HKLM\\%s...\n"), NT_MONITOR_KEY);
	// Windows 9X => Open the monitor key
	if (RegOpenKeyEx( m_hKey, NT_MONITOR_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_MONITOR_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyObject) == ERROR_SUCCESS)
			{
				csManufacturer = NOT_AVAILABLE; 
				csModel = NOT_AVAILABLE;
				csCaption = NOT_AVAILABLE; 
				csDescription = NOT_AVAILABLE; 
				// Read the manufactuer
				if (GetValue( hKeyObject, NT_MONITOR_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_MONITOR_MANUFACTURER_VALUE);
					csManufacturer = NOT_AVAILABLE; 
				}
				// Read the model
				if (GetValue( hKeyObject, NT_MONITOR_TYPE_VALUE, csModel) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_MONITOR_TYPE_VALUE);
					csModel = NOT_AVAILABLE;
				}
				// Read the caption
				if (GetValue( hKeyObject, NT_MONITOR_CAPTION_VALUE, csCaption) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_MONITOR_CAPTION_VALUE);
					csCaption = NOT_AVAILABLE; 
				}
				// Read the description
				if (GetValue( hKeyObject, NT_MONITOR_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_MONITOR_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				RegCloseKey( hKeyObject);
				myObject.SetManufacturer( csManufacturer);
				myObject.SetType( csModel);
				myObject.SetCaption( csCaption);
				myObject.SetDescription( csDescription);
				AddLog( _T( "\t\t<Manufacturer: %s><Caption: %s><Description: %s><Name: %s><Type: %s>\n"), 
					myObject.GetManufacturer(), myObject.GetCaption(), myObject.GetDescription(),
					myObject.GetType());
				// Add the device to the adapter list
				if (bHaveToStore)
				{
					pList->AddTail( myObject);
					uIndex ++;
				}
			}
			else
				AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
								   csSubKey);
			// Enum the next device
			dwLength = 255;
			dwIndexEnum++;
		}
		RegCloseKey( hKeyEnum);
		if (dwIndexEnum == 0)
			// No key found
			AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_MONITOR_KEY);
		else
			AddLog( _T( "\tOK (%u objects).\n"), uIndex);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_MONITOR_KEY);
	return !pList->IsEmpty();
}

BOOL CRegistry::GetSystemControllers(CSystemControllerList *pList)
{
	ASSERT( pList);

	// Reset object list content
	while (!(pList->GetCount() == 0))
		pList->RemoveHead();
	// Try to get system Controllers Informations info from the registry
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetSystemControllersNT( pList);
	default:
		// Unknown
		AddLog( _T( "Registry GetSystemControllers...\n\tFailed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::GetSystemControllersNT(CSystemControllerList *pList)
{
	HKEY			hKeyEnum,
					hKeyObject;
	CString			csSubKey,
					csManufacturer,
					csName,
					csCaption,
					csDescription,
					csVersion,
					csValue;
	TCHAR			szKey[256];
	DWORD			dwLength,
					dwIndexEnum = 0;
	LONG			lResult;
	FILETIME		MyFileTime;
	CSystemController myObject;
	BOOL			bHaveToStore;
	UINT			uIndex = 0,
					uTotalIndex = 0;

	ASSERT( pList);

	AddLog( _T( "Registry NT GetSystemControllers: Trying to find Floppy Controllers in HKLM\\%s...\n"),
			NT_CONTROLER_FLOPPY_KEY);
	// Windows NT => Open the floppy controler key
	if (RegOpenKeyEx( m_hKey, NT_CONTROLER_FLOPPY_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_CONTROLER_FLOPPY_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyObject) == ERROR_SUCCESS)
			{
				csManufacturer = NOT_AVAILABLE; 
				csName = NOT_AVAILABLE;
				csCaption = NOT_AVAILABLE; 
				csDescription = NOT_AVAILABLE; 
				csVersion = NOT_AVAILABLE;
				// Read the manufactuer
				if (GetValue( hKeyObject, NT_CONTROLER_FLOPPY_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_FLOPPY_MANUFACTURER_VALUE);
					csManufacturer = NOT_AVAILABLE; 
				}
				// Read the name
				if (GetValue( hKeyObject, NT_CONTROLER_FLOPPY_NAME_VALUE, csName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_FLOPPY_NAME_VALUE);
					csName = NOT_AVAILABLE;
				}
				// Read the caption
				if (GetValue( hKeyObject, NT_CONTROLER_FLOPPY_CAPTION_VALUE, csCaption) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_FLOPPY_CAPTION_VALUE);
					csCaption = NOT_AVAILABLE; 
				}
				// Read the description
				if (GetValue( hKeyObject, NT_CONTROLER_FLOPPY_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_FLOPPY_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				// Read the verion
				if (GetValue( hKeyObject, NT_CONTROLER_FLOPPY_VERSION_VALUE, csVersion) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_FLOPPY_VERSION_VALUE);
					csVersion = NOT_AVAILABLE;
				}
				RegCloseKey( hKeyObject);
				myObject.SetType( SYSTEM_CONTROLER_FLOPPY);
				myObject.SetManufacturer( csManufacturer);
				myObject.SetName( csName);
				myObject.SetHardwareVersion( csVersion);
				myObject.SetCaption( csCaption);
				myObject.SetDescription( csDescription);
				AddLog( _T( "\t\t<Manufacturer: %s><Caption: %s><Description: %s><Name: %s><Type: %s><Version: %s>\n"), 
					myObject.GetManufacturer(), myObject.GetCaption(), myObject.GetDescription(),
					myObject.GetName(), myObject.GetType(), myObject.GetHardwareVersion());
				// Add the device to the adapter list
				if (bHaveToStore)
				{
					pList->AddTail( myObject);
					uIndex ++;
				}
			}
			else
				AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
								   csSubKey);
			// Enum the next device
			dwLength = 255;
			dwIndexEnum++;
		}
		RegCloseKey( hKeyEnum);
		if (dwIndexEnum == 0)
			// No key found
			AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_CONTROLER_FLOPPY_KEY);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_CONTROLER_FLOPPY_KEY);
	if (uIndex > 0)
	{
		uTotalIndex += uIndex;
		AddLog( _T( "\tOK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "\tFailed because no valid object !\n"));
	// Windows NT => Open the IDE controler key
	uIndex = 0;
	dwIndexEnum = 0;
	AddLog( _T( "Registry NT GetSystemControllers: Trying to find IDE Controllers in HKLM\\%s...\n"),
			NT_CONTROLER_IDE_KEY);
	if (RegOpenKeyEx( m_hKey, NT_CONTROLER_IDE_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_CONTROLER_IDE_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyObject) == ERROR_SUCCESS)
			{
				csManufacturer = NOT_AVAILABLE; 
				csName = NOT_AVAILABLE;
				csCaption = NOT_AVAILABLE; 
				csDescription = NOT_AVAILABLE; 
				csVersion = NOT_AVAILABLE;
				// Read the manufactuer
				if (GetValue( hKeyObject, NT_CONTROLER_IDE_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_IDE_MANUFACTURER_VALUE);
					csManufacturer = NOT_AVAILABLE; 
				}
				// Read the name
				if (GetValue( hKeyObject, NT_CONTROLER_IDE_NAME_VALUE, csName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_IDE_NAME_VALUE);
					csName = NOT_AVAILABLE;
				}
				// Read the caption
				if (GetValue( hKeyObject, NT_CONTROLER_IDE_CAPTION_VALUE, csCaption) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_IDE_CAPTION_VALUE);
					csCaption = NOT_AVAILABLE; 
				}
				// Read the description
				if (GetValue( hKeyObject, NT_CONTROLER_IDE_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_IDE_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				// Read the verion
				if (GetValue( hKeyObject, NT_CONTROLER_IDE_VERSION_VALUE, csVersion) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_IDE_VERSION_VALUE);
					csVersion = NOT_AVAILABLE;
				}
				RegCloseKey( hKeyObject);
				myObject.SetType( SYSTEM_CONTROLER_IDE);
				myObject.SetManufacturer( csManufacturer);
				myObject.SetName( csName);
				myObject.SetHardwareVersion( csVersion);
				myObject.SetCaption( csCaption);
				myObject.SetDescription( csDescription);
				AddLog( _T( "\t\t<Manufacturer: %s><Caption: %s><Description: %s><Name: %s><Type: %s><Version: %s>\n"), 
					myObject.GetManufacturer(), myObject.GetCaption(), myObject.GetDescription(),
					myObject.GetName(), myObject.GetType(), myObject.GetHardwareVersion());
				// Add the device to the adapter list
				if (bHaveToStore)
				{
					pList->AddTail( myObject);
					uIndex ++;
				}
			}
			else
				AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
								   csSubKey);
			// Enum the next device
			dwLength = 255;
			dwIndexEnum++;
		}
		RegCloseKey( hKeyEnum);
		if (dwIndexEnum == 0)
			// No key found
			AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_CONTROLER_IDE_KEY);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_CONTROLER_IDE_KEY);
	if (uIndex > 0)
	{
		uTotalIndex += uIndex;
		AddLog( _T( "\tOK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "\tFailed because no valid object !\n"));
	// Windows NT => Open the SCSI controler key
	uIndex = 0;
	dwIndexEnum = 0;
	AddLog( _T( "Registry NT GetSystemControllers: Trying to find SCSI Controllers in HKLM\\%s...\n"),
			NT_CONTROLER_SCSI_KEY);
	if (RegOpenKeyEx( m_hKey, NT_CONTROLER_SCSI_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_CONTROLER_SCSI_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyObject) == ERROR_SUCCESS)
			{
				csManufacturer = NOT_AVAILABLE; 
				csName = NOT_AVAILABLE;
				csCaption = NOT_AVAILABLE; 
				csDescription = NOT_AVAILABLE; 
				csVersion = NOT_AVAILABLE;
				// Read the manufactuer
				if (GetValue( hKeyObject, NT_CONTROLER_SCSI_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_SCSI_MANUFACTURER_VALUE);
					csManufacturer = NOT_AVAILABLE; 
				}
				// Read the name
				if (GetValue( hKeyObject, NT_CONTROLER_SCSI_NAME_VALUE, csName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_SCSI_NAME_VALUE);
					csName = NOT_AVAILABLE;
				}
				// Read the caption
				if (GetValue( hKeyObject, NT_CONTROLER_SCSI_CAPTION_VALUE, csCaption) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_SCSI_CAPTION_VALUE);
					csCaption = NOT_AVAILABLE; 
				}
				// Read the description
				if (GetValue( hKeyObject, NT_CONTROLER_SCSI_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_SCSI_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				// Read the verion
				if (GetValue( hKeyObject, NT_CONTROLER_SCSI_VERSION_VALUE, csVersion) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_SCSI_VERSION_VALUE);
					csVersion = NOT_AVAILABLE;
				}
				RegCloseKey( hKeyObject);
				myObject.SetType( SYSTEM_CONTROLER_SCSI);
				myObject.SetManufacturer( csManufacturer);
				myObject.SetName( csName);
				myObject.SetHardwareVersion( csVersion);
				myObject.SetCaption( csCaption);
				myObject.SetDescription( csDescription);
				AddLog( _T( "\t\t<Manufacturer: %s><Caption: %s><Description: %s><Name: %s><Type: %s><Version: %s>\n"), 
					myObject.GetManufacturer(), myObject.GetCaption(), myObject.GetDescription(),
					myObject.GetName(), myObject.GetType(), myObject.GetHardwareVersion());
				// Add the device to the adapter list
				if (bHaveToStore)
				{
					pList->AddTail( myObject);
					uIndex ++;
				}
			}
			else
				AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
								   csSubKey);
			// Enum the next device
			dwLength = 255;
			dwIndexEnum++;
		}
		RegCloseKey( hKeyEnum);
		if (dwIndexEnum == 0)
			// No key found
			AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_CONTROLER_SCSI_KEY);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_CONTROLER_SCSI_KEY);
	if (uIndex > 0)
	{
		uTotalIndex += uIndex;
		AddLog( _T( "\tOK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "\tFailed because no valid object !\n"));
	// Windows NT => Open the InfraRed controler key
	uIndex = 0;
	dwIndexEnum = 0;
	AddLog( _T( "Registry NT GetSystemControllers: Trying to find InfraRed Controllers in HKLM\\%s...\n"),
			NT_CONTROLER_INFRARED_KEY);
	if (RegOpenKeyEx( m_hKey, NT_CONTROLER_INFRARED_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_CONTROLER_INFRARED_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyObject) == ERROR_SUCCESS)
			{
				csManufacturer = NOT_AVAILABLE; 
				csName = NOT_AVAILABLE;
				csCaption = NOT_AVAILABLE; 
				csDescription = NOT_AVAILABLE; 
				csVersion = NOT_AVAILABLE;
				// Read the manufactuer
				if (GetValue( hKeyObject, NT_CONTROLER_INFRARED_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_INFRARED_MANUFACTURER_VALUE);
					csManufacturer = NOT_AVAILABLE; 
				}
				// Read the name
				if (GetValue( hKeyObject, NT_CONTROLER_INFRARED_NAME_VALUE, csName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_INFRARED_NAME_VALUE);
					csName = NOT_AVAILABLE;
				}
				// Read the caption
				if (GetValue( hKeyObject, NT_CONTROLER_INFRARED_CAPTION_VALUE, csCaption) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_INFRARED_CAPTION_VALUE);
					csCaption = NOT_AVAILABLE; 
				}
				// Read the description
				if (GetValue( hKeyObject, NT_CONTROLER_INFRARED_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_INFRARED_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				// Read the verion
				if (GetValue( hKeyObject, NT_CONTROLER_INFRARED_VERSION_VALUE, csVersion) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_INFRARED_VERSION_VALUE);
					csVersion = NOT_AVAILABLE;
				}
				// Ensure it is a controler
				if (GetValue( hKeyObject, NT_CONTROLER_INFRARED_CONTROLER_VALUE, csValue) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_INFRARED_CONTROLER_VALUE);
					bHaveToStore = FALSE;
				}
				RegCloseKey( hKeyObject);
				myObject.SetType( SYSTEM_CONTROLER_INFRARED);
				myObject.SetManufacturer( csManufacturer);
				myObject.SetName( csName);
				myObject.SetHardwareVersion( csVersion);
				myObject.SetCaption( csCaption);
				myObject.SetDescription( csDescription);
				AddLog( _T( "\t\t<Manufacturer: %s><Caption: %s><Description: %s><Name: %s><Type: %s><Version: %s>\n"), 
					myObject.GetManufacturer(), myObject.GetCaption(), myObject.GetDescription(),
					myObject.GetName(), myObject.GetType(), myObject.GetHardwareVersion());
				// Add the device to the adapter list
				if (bHaveToStore)
				{
					pList->AddTail( myObject);
					uIndex ++;
				}
			}
			else
				AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
								   csSubKey);
			// Enum the next device
			dwLength = 255;
			dwIndexEnum++;
		}
		RegCloseKey( hKeyEnum);
		if (dwIndexEnum == 0)
			// No key found
			AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_CONTROLER_INFRARED_KEY);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_CONTROLER_INFRARED_KEY);
	if (uIndex > 0)
	{
		uTotalIndex += uIndex;
		AddLog( _T( "\tOK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "\tFailed because no valid object !\n"));
	// Windows NT => Open the USB controler key
	uIndex = 0;
	dwIndexEnum = 0;
	AddLog( _T( "Registry NT GetSystemControllers: Trying to find USB Controllers in HKLM\\%s...\n"),
			NT_CONTROLER_USB_KEY);
	if (RegOpenKeyEx( m_hKey, NT_CONTROLER_USB_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_CONTROLER_USB_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyObject) == ERROR_SUCCESS)
			{
				csManufacturer = NOT_AVAILABLE; 
				csName = NOT_AVAILABLE;
				csCaption = NOT_AVAILABLE; 
				csDescription = NOT_AVAILABLE; 
				csVersion = NOT_AVAILABLE;
				// Read the manufactuer
				if (GetValue( hKeyObject, NT_CONTROLER_USB_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_USB_MANUFACTURER_VALUE);
					csManufacturer = NOT_AVAILABLE; 
				}
				// Read the name
				if (GetValue( hKeyObject, NT_CONTROLER_USB_NAME_VALUE, csName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_USB_NAME_VALUE);
					csName = NOT_AVAILABLE;
				}
				// Read the caption
				if (GetValue( hKeyObject, NT_CONTROLER_USB_CAPTION_VALUE, csCaption) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_USB_CAPTION_VALUE);
					csCaption = NOT_AVAILABLE; 
				}
				// Read the description
				if (GetValue( hKeyObject, NT_CONTROLER_USB_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_USB_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				// Read the verion
				if (GetValue( hKeyObject, NT_CONTROLER_USB_VERSION_VALUE, csVersion) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_USB_VERSION_VALUE);
					csVersion = NOT_AVAILABLE;
				}
				// Ensure it is a controler
				if (GetValue( hKeyObject, NT_CONTROLER_USB_CONTROLER_VALUE, csValue) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_USB_CONTROLER_VALUE);
					bHaveToStore = FALSE;
				}
				RegCloseKey( hKeyObject);
				myObject.SetType( SYSTEM_CONTROLER_USB);
				myObject.SetManufacturer( csManufacturer);
				myObject.SetName( csName);
				myObject.SetHardwareVersion( csVersion);
				myObject.SetCaption( csCaption);
				myObject.SetDescription( csDescription);
				AddLog( _T( "\t\t<Manufacturer: %s><Caption: %s><Description: %s><Name: %s><Type: %s><Version: %s>\n"), 
					myObject.GetManufacturer(), myObject.GetCaption(), myObject.GetDescription(),
					myObject.GetName(), myObject.GetType(), myObject.GetHardwareVersion());
				// Add the device to the adapter list
				if (bHaveToStore)
				{
					pList->AddTail( myObject);
					uIndex ++;
				}
			}
			else
				AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
								   csSubKey);
			// Enum the next device
			dwLength = 255;
			dwIndexEnum++;
		}
		RegCloseKey( hKeyEnum);
		if (dwIndexEnum == 0)
			// No key found
			AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_CONTROLER_USB_KEY);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_CONTROLER_USB_KEY);
	if (uIndex > 0)
	{
		uTotalIndex += uIndex;
		AddLog( _T( "\tOK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "\tFailed because no valid object !\n"));
	// Windows NT => Open the IEEE1394 controler key
	uIndex = 0;
	dwIndexEnum = 0;
	AddLog( _T( "Registry NT GetSystemControllers: Trying to find IEEE1394 Controllers in HKLM\\%s...\n"),
			NT_CONTROLER_IEEE1394_KEY);
	if (RegOpenKeyEx( m_hKey, NT_CONTROLER_IEEE1394_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_CONTROLER_IEEE1394_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyObject) == ERROR_SUCCESS)
			{
				csManufacturer = NOT_AVAILABLE; 
				csName = NOT_AVAILABLE;
				csCaption = NOT_AVAILABLE; 
				csDescription = NOT_AVAILABLE; 
				csVersion = NOT_AVAILABLE;
				// Read the manufactuer
				if (GetValue( hKeyObject, NT_CONTROLER_IEEE1394_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_IEEE1394_MANUFACTURER_VALUE);
					csManufacturer = NOT_AVAILABLE; 
				}
				// Read the name
				if (GetValue( hKeyObject, NT_CONTROLER_IEEE1394_NAME_VALUE, csName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_IEEE1394_NAME_VALUE);
					csName = NOT_AVAILABLE;
				}
				// Read the caption
				if (GetValue( hKeyObject, NT_CONTROLER_IEEE1394_CAPTION_VALUE, csCaption) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_IEEE1394_CAPTION_VALUE);
					csCaption = NOT_AVAILABLE; 
				}
				// Read the description
				if (GetValue( hKeyObject, NT_CONTROLER_IEEE1394_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_IEEE1394_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				// Read the verion
				if (GetValue( hKeyObject, NT_CONTROLER_IEEE1394_VERSION_VALUE, csVersion) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_IEEE1394_VERSION_VALUE);
					csVersion = NOT_AVAILABLE;
				}
				RegCloseKey( hKeyObject);
				myObject.SetType( SYSTEM_CONTROLER_IEEE1394);
				myObject.SetManufacturer( csManufacturer);
				myObject.SetName( csName);
				myObject.SetHardwareVersion( csVersion);
				myObject.SetCaption( csCaption);
				myObject.SetDescription( csDescription);
				AddLog( _T( "\t\t<Manufacturer: %s><Caption: %s><Description: %s><Name: %s><Type: %s><Version: %s>\n"), 
					myObject.GetManufacturer(), myObject.GetCaption(), myObject.GetDescription(),
					myObject.GetName(), myObject.GetType(), myObject.GetHardwareVersion());
				// Add the device to the adapter list
				if (bHaveToStore)
				{
					pList->AddTail( myObject);
					uIndex ++;
				}
			}
			else
				AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
								   csSubKey);
			// Enum the next device
			dwLength = 255;
			dwIndexEnum++;
		}
		RegCloseKey( hKeyEnum);
		if (dwIndexEnum == 0)
			// No key found
			AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_CONTROLER_IEEE1394_KEY);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_CONTROLER_IEEE1394_KEY);
	if (uIndex > 0)
	{
		uTotalIndex += uIndex;
		AddLog( _T( "\tOK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "\tFailed because no valid object !\n"));
	// Windows NT => Open the PCMCIA controler key
	uIndex = 0;
	dwIndexEnum = 0;
	AddLog( _T( "Registry NT GetSystemControllers: Trying to find PCMCIA Controllers in HKLM\\%s...\n"),
			NT_CONTROLER_PCMCIA_KEY);
	if (RegOpenKeyEx( m_hKey, NT_CONTROLER_PCMCIA_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_CONTROLER_PCMCIA_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyObject) == ERROR_SUCCESS)
			{
				csManufacturer = NOT_AVAILABLE; 
				csName = NOT_AVAILABLE;
				csCaption = NOT_AVAILABLE; 
				csDescription = NOT_AVAILABLE; 
				csVersion = NOT_AVAILABLE;
				// Read the manufactuer
				if (GetValue( hKeyObject, NT_CONTROLER_PCMCIA_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_PCMCIA_MANUFACTURER_VALUE);
					csManufacturer = NOT_AVAILABLE; 
				}
				// Read the name
				if (GetValue( hKeyObject, NT_CONTROLER_PCMCIA_NAME_VALUE, csName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_PCMCIA_NAME_VALUE);
					csName = NOT_AVAILABLE;
				}
				// Read the caption
				if (GetValue( hKeyObject, NT_CONTROLER_PCMCIA_CAPTION_VALUE, csCaption) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_PCMCIA_CAPTION_VALUE);
					csCaption = NOT_AVAILABLE; 
				}
				// Read the description
				if (GetValue( hKeyObject, NT_CONTROLER_PCMCIA_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_PCMCIA_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				// Read the verion
				if (GetValue( hKeyObject, NT_CONTROLER_PCMCIA_VERSION_VALUE, csVersion) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_CONTROLER_PCMCIA_VERSION_VALUE);
					csVersion = NOT_AVAILABLE;
				}
				RegCloseKey( hKeyObject);
				myObject.SetType( SYSTEM_CONTROLER_PCMCIA);
				myObject.SetManufacturer( csManufacturer);
				myObject.SetName( csName);
				myObject.SetHardwareVersion( csVersion);
				myObject.SetCaption( csCaption);
				myObject.SetDescription( csDescription);
				AddLog( _T( "\t\t<Manufacturer: %s><Caption: %s><Description: %s><Name: %s><Type: %s><Version: %s>\n"), 
					myObject.GetManufacturer(), myObject.GetCaption(), myObject.GetDescription(),
					myObject.GetName(), myObject.GetType(), myObject.GetHardwareVersion());
				// Add the device to the adapter list
				if (bHaveToStore)
				{
					pList->AddTail( myObject);
					uIndex ++;
				}
			}
			else
				AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
								   csSubKey);
			// Enum the next device
			dwLength = 255;
			dwIndexEnum++;
		}
		RegCloseKey( hKeyEnum);
		if (dwIndexEnum == 0)
			// No key found
			AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_CONTROLER_PCMCIA_KEY);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_CONTROLER_PCMCIA_KEY);
	if (uIndex > 0)
	{
		uTotalIndex += uIndex;
		AddLog( _T( "\tOK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "\tFailed because no valid object !\n"));
	return (uTotalIndex > 0);
}

BOOL CRegistry::GetStoragePeripherals(CStoragePeripheralList *pList)
{
	ASSERT( pList);

	// Reset object list content
	while (!(pList->GetCount() == 0))
		pList->RemoveHead();
	// Try to get Storage Peripherals Informations info from the registry
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetStoragePeripheralsNT( pList);
	default:
		// Unknown
		AddLog( _T( "Registry GetStoragePeripherals...\n\tFailed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::GetStoragePeripheralsNT(CStoragePeripheralList *pList)
{
	HKEY			hKeyEnum,
					hKeyEnumBus,
					hKeyEnumTarget,
					hKeyEnumLogical,
					hKeyObject;
	CString			csSubKeyBus,
					csSubKeyTarget,
					csSubKeyLogical,
					csSubKey,
					csManufacturer,
					csModel,
					csName,
					csDescription,
					csType;
	TCHAR			szKey[256],
					szSubKey[256];
	DWORD			dwLength,
					dwIndexEnum = 0,
					dwIndexEnumBus,
					dwIndexEnumTarget,
					dwIndexEnumLogical;
	LONG			lResult;
	FILETIME		MyFileTime;
	CStoragePeripheral	myObject;
	BOOL			bHaveToStore;
	UINT			uIndex = 0,
					uTotalIndex = 0;

	ASSERT( pList);

	AddLog( _T( "Registry NT GetStoragePeripherals: Trying to find Floppy Drives in HKLM\\%s...\n"),
			NT_STORAGE_FLOPPY_KEY);
	// Windows NT => Open the floppy drive key
	if (RegOpenKeyEx( m_hKey, NT_STORAGE_FLOPPY_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_STORAGE_FLOPPY_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnumBus) == ERROR_SUCCESS)
			{
				dwLength = 255;
				dwIndexEnumBus = 0;
				while ((lResult = RegEnumKeyEx( hKeyEnumBus, dwIndexEnumBus, szSubKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
				{
					// For each object, Try to open the device key
					szSubKey[dwLength] = 0;
					bHaveToStore = FALSE;
					csSubKey.Format( _T( "%s\\%s\\%s"), NT_STORAGE_FLOPPY_KEY, szKey, szSubKey);
					if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyObject) == ERROR_SUCCESS)
					{
						csManufacturer = NOT_AVAILABLE; 
						csModel = NOT_AVAILABLE;
						csName = NOT_AVAILABLE; 
						csDescription = NOT_AVAILABLE; 
						csType = NOT_AVAILABLE; 
						// Read the manufactuer
						if (GetValue( hKeyObject, NT_STORAGE_FLOPPY_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
						{
							bHaveToStore = TRUE;
						}
						else
						{
							AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									csSubKey, NT_STORAGE_FLOPPY_MANUFACTURER_VALUE);
							csManufacturer = NOT_AVAILABLE; 
						}
						// Read the model
						if (GetValue( hKeyObject, NT_STORAGE_FLOPPY_MODEL_VALUE, csModel) == ERROR_SUCCESS)
						{
							bHaveToStore = TRUE;
						}
						else
						{
							AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									csSubKey, NT_STORAGE_FLOPPY_MODEL_VALUE);
							csModel = NOT_AVAILABLE;
						}
						// Read the name
						if (GetValue( hKeyObject, NT_STORAGE_FLOPPY_NAME_VALUE, csName) == ERROR_SUCCESS)
						{
							bHaveToStore = TRUE;
						}
						else
						{
							AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									csSubKey, NT_STORAGE_FLOPPY_NAME_VALUE);
							csName = NOT_AVAILABLE; 
						}
						// Read the description
						if (GetValue( hKeyObject, NT_STORAGE_FLOPPY_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
						{
							bHaveToStore = TRUE;
						}
						else
						{
							AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									csSubKey, NT_STORAGE_FLOPPY_DESCRIPTION_VALUE);
							csDescription = NOT_AVAILABLE;
						}
						// Read the type
						if (GetValue( hKeyObject, NT_STORAGE_FLOPPY_TYPE_VALUE, csType) == ERROR_SUCCESS)
						{
							bHaveToStore = TRUE;
						}
						else
						{
							AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									csSubKey, NT_STORAGE_FLOPPY_TYPE_VALUE);
							csType = NOT_AVAILABLE; 
						}
						RegCloseKey( hKeyObject);
						myObject.SetType( csType);
						myObject.SetManufacturer( csManufacturer);
						myObject.SetName( csName);
						myObject.SetModel( csModel);
						myObject.SetDescription( csDescription);
						AddLog( _T( "\t\t<Manufacturer: %s><Caption: %s><Description: %s><Name: %s><MediaType: %s>\n"), 
							myObject.GetManufacturer(), myObject.GetModel(), myObject.GetDescription(),
							myObject.GetName(), myObject.GetType());
						// Add the device to the adapter list
						if (bHaveToStore)
						{
							pList->AddTail( myObject);
							uIndex ++;
						}
					}
					else
						AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
								csSubKey);
					// Enum the next device
					dwLength = 255;
					dwIndexEnumBus++;
				}
				RegCloseKey( hKeyEnumBus);
				if (dwIndexEnumBus == 0)
					// No key found
					AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
							csSubKey);
			}
			else
				AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
						csSubKey);
			// Enum the next device
			dwLength = 255;
			dwIndexEnum++;
		}
		RegCloseKey( hKeyEnum);
		if (dwIndexEnum == 0)
			// No key found
			AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_STORAGE_FLOPPY_KEY);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_STORAGE_FLOPPY_KEY);
	if (uIndex > 0)
	{
		uTotalIndex += uIndex;
		AddLog( _T( "\tOK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "\tFailed because no valid object !\n"));

	AddLog( _T( "Registry NT GetStoragePeripherals: Trying to find Other Drives in HKLM\\%s...\n"),
			NT_STORAGE_OTHER_KEY);
	// Windows NT => Open the other drive key
	uIndex = 0;
	if (RegOpenKeyEx( m_hKey, NT_STORAGE_OTHER_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find SCSI port
		dwLength = 255;
		dwIndexEnum = 0;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the SCSI bus key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKeyBus.Format( _T( "%s\\%s"), NT_STORAGE_OTHER_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKeyBus, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnumBus) == ERROR_SUCCESS)
			{
				// Enum the device subkeys to find SCSI target
				dwLength = 255;
				dwIndexEnumBus = 0;
				while ((lResult = RegEnumKeyEx( hKeyEnumBus, dwIndexEnumBus, szSubKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
				{
					// For each object, Try to open the SCSI traget key
					szSubKey[dwLength] = 0;
					bHaveToStore = FALSE;
					csSubKeyTarget.Format( _T( "%s\\%s"), csSubKeyBus, szSubKey);
					if (RegOpenKeyEx( m_hKey, csSubKeyTarget, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnumTarget) == ERROR_SUCCESS)
					{
						// Enum the device subkeys to find SCSI target
						dwLength = 255;
						dwIndexEnumTarget = 0;
						while ((lResult = RegEnumKeyEx( hKeyEnumTarget, dwIndexEnumTarget, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
						{
							// For each object, Try to open the SCSI traget key
							szKey[dwLength] = 0;
							bHaveToStore = FALSE;
							csSubKeyLogical.Format( _T( "%s\\%s"), csSubKeyTarget, szKey);
							if (RegOpenKeyEx( m_hKey, csSubKeyLogical, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnumLogical) == ERROR_SUCCESS)
							{
								// Enum the device subkeys to find SCSI logical unit
								dwLength = 255;
								dwIndexEnumLogical = 0;
								while ((lResult = RegEnumKeyEx( hKeyEnumLogical, dwIndexEnumLogical, szSubKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
								{
									// For each object, Try to open the SCSI logical unit key
									szSubKey[dwLength] = 0;
									bHaveToStore = FALSE;
									csSubKey.Format( _T( "%s\\%s"), csSubKeyLogical, szSubKey);
									if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyObject) == ERROR_SUCCESS)
									{
										csManufacturer = NOT_AVAILABLE; 
										csModel = NOT_AVAILABLE;
										csName = NOT_AVAILABLE; 
										csDescription = NOT_AVAILABLE; 
										csType = NOT_AVAILABLE; 
										// Read the manufactuer
										if (GetValue( hKeyObject, NT_STORAGE_OTHER_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
										{
											bHaveToStore = TRUE;
										}
										else
										{
											AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
													csSubKey, NT_STORAGE_OTHER_MANUFACTURER_VALUE);
											csManufacturer = NOT_AVAILABLE;
										}
										// Read the model
										if (GetValue( hKeyObject, NT_STORAGE_OTHER_MODEL_VALUE, csModel) == ERROR_SUCCESS)
										{
											bHaveToStore = TRUE;
										}
										else
										{
											AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
													csSubKey, NT_STORAGE_OTHER_MODEL_VALUE);
											csModel = NOT_AVAILABLE;
										}
										// Read the name
										if (GetValue( hKeyObject, NT_STORAGE_OTHER_NAME_VALUE, csName) == ERROR_SUCCESS)
										{
											bHaveToStore = TRUE;
										}
										else
										{
											AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
													csSubKey, NT_STORAGE_OTHER_NAME_VALUE);
											csName = NOT_AVAILABLE; 
										}
										// Read the description
										if (GetValue( hKeyObject, NT_STORAGE_OTHER_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
										{
											bHaveToStore = TRUE;
										}
										else
										{
											AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
													csSubKey, NT_STORAGE_OTHER_DESCRIPTION_VALUE);
											csDescription = NOT_AVAILABLE; 
										}
										// Read the type
										if (GetValue( hKeyObject, NT_STORAGE_OTHER_TYPE_VALUE, csType) == ERROR_SUCCESS)
										{
											bHaveToStore = TRUE;
										}
										else
										{
											AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
													csSubKey, NT_STORAGE_OTHER_TYPE_VALUE);
											csType = NOT_AVAILABLE; 
										}
										RegCloseKey( hKeyObject);
										myObject.SetType( csType);
										myObject.SetManufacturer( csManufacturer);
										myObject.SetName( csName);
										myObject.SetModel( csModel);
										myObject.SetDescription( csDescription);
										AddLog( _T( "\t\t<Manufacturer: %s><Caption: %s><Description: %s><Name: %s><MediaType: %s>\n"), 
											myObject.GetManufacturer(), myObject.GetModel(), myObject.GetDescription(),
											myObject.GetName(), myObject.GetType());
										// Add the device to the adapter list
										if (bHaveToStore)
										{
											pList->AddTail( myObject);
											uIndex ++;
										}
									} // If RegOpenKey Object
									else
										AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
												csSubKey);
									// Enum the next logical unit
									dwIndexEnumLogical++;
								} // While RegEnumKey Logical unit
								RegCloseKey( hKeyEnumLogical);
								if (dwIndexEnumLogical == 0)
									// No key found
									AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
											csSubKeyLogical);
							} // if RegOpenKey Logical unit
							else
								AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
										csSubKeyLogical);
							// Enum next target
							dwLength = 255;
							dwIndexEnumTarget++;
						} // While RegEnumKey Target
						RegCloseKey( hKeyEnumTarget);
						if (dwIndexEnumTarget == 0)
							// No key found
							AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
									csSubKeyTarget);
					} // If RegOpenKey Target
					else
						AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
								csSubKeyTarget);
					// Enum next bux
					dwLength = 255;
					dwIndexEnumBus++;
				} // While RegEnumKey Bus
				RegCloseKey( hKeyEnumBus);
				if (dwIndexEnumBus == 0)
					// No key found
					AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
							csSubKeyBus);
			} // If RegOpenKey Bus
			else
				AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
						csSubKeyBus);
			// Enum the next port
			dwLength = 255;
			dwIndexEnum++;
		} // While RegEnumKey Port
		RegCloseKey( hKeyEnum);
		if (dwIndexEnum == 0)
			// No key found
			AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_STORAGE_OTHER_KEY);
	} // If RegOpenKey Port
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_STORAGE_OTHER_KEY);
	if (uIndex > 0)
	{
		uTotalIndex += uIndex;
		AddLog( _T( "\tOK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "\tFailed because no valid object !\n"));

	return (uTotalIndex > 0);
}

BOOL CRegistry::GetRegistryApplications(CSoftwareList *pList, BOOL hkcu)
{
	ASSERT( pList);
	static BOOL bResult = TRUE;

	// Reset object list content
	while (!(pList->GetCount() == 0))
		pList->RemoveHead();
	// Try to get BIOS Informations info from the registry
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		if (hkcu)
		{
			// HKEY_CURRENT_USER => Don't care about result
			if (m_dwAddressWidth == 64)
			{
				// 64 bits OS
				GetRegistryApplicationsNT( pList, HKEY_CURRENT_USER, HIVE_WOW64_64KEY);
				GetRegistryApplicationsNT( pList, HKEY_CURRENT_USER, HIVE_WOW64_32KEY);
			}
			else
				// 32 bits OS
				GetRegistryApplicationsNT( pList, HKEY_CURRENT_USER, HIVE_WOW32_32KEY);
		}
		// HKEY_LOCAL_MACHINE
		if (m_dwAddressWidth == 64)
		{
			// 64 bits OS
			GetRegistryApplicationsNT( pList, HKEY_LOCAL_MACHINE, HIVE_WOW64_32KEY);
			bResult = GetRegistryApplicationsNT( pList, HKEY_LOCAL_MACHINE, HIVE_WOW64_64KEY);
		}
		else
			// 32 bits OS
			bResult = GetRegistryApplicationsNT( pList, HKEY_LOCAL_MACHINE, HIVE_WOW32_32KEY);
		return bResult;
	default:
		// Unknown
		AddLog( _T( "Registry GetRegistryApplications...\n\tFailed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::GetRegistryApplicationsNT(CSoftwareList *pList, HKEY hHive, UINT uHiveType)
{
	HKEY			hKeyEnum,
					hKeyObject;
	CString			csSubKey,
					csPublisher,
					csName,
					csVersion,
					csFolder,
					csComments,
					csLanguage,
					csUninstall,
					csInstallDate;
	TCHAR			szGUID[256];
	DWORD			dwLength,
					dwLanguage,
					dwIndexEnum = 0;
	REGSAM			regAccess;
	LONG			lResult;
	FILETIME		MyFileTime;
	CSoftware		cApp;
	BOOL			bHaveToStore;
	UINT			uIndex = 0;

	ASSERT( pList);

	CString csCurHive = (hHive==HKEY_LOCAL_MACHINE ? _T( "HKLM") : _T( "HKCU")) ;
	// Windows NT => Open the Apps key
	switch (uHiveType)
	{
	case HIVE_WOW64_64KEY:
		AddLog( _T( "Registry NT GetRegistryApplications: Reading 64 bits hive %s... \n"),csCurHive);	
		regAccess = KEY_READ|KEY_WOW64_64KEY;
		break;
	case HIVE_WOW64_32KEY:
		AddLog( _T( "Registry NT GetRegistryApplications: Reading 32 bits hive %s... \n"),csCurHive);	
		regAccess = KEY_READ|KEY_WOW64_32KEY;
		break;
	case HIVE_WOW32_32KEY:
	default:
		AddLog( _T( "Registry NT GetRegistryApplications: Reading hive %s... \n"),csCurHive);	
		regAccess = KEY_READ;
		break;
	}
	if (RegOpenKeyEx( hHive, NT_APPS_KEY, 0, regAccess, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find installed apps
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szGUID, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szGUID[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_APPS_KEY, szGUID);
			if (RegOpenKeyEx( hHive, csSubKey, 0, regAccess, &hKeyObject) == ERROR_SUCCESS)
			{
				csPublisher = NOT_AVAILABLE;
				csName = NOT_AVAILABLE;
				csVersion = NOT_AVAILABLE;
				csFolder = NOT_AVAILABLE;
				csComments = NOT_AVAILABLE;
				csLanguage = NOT_AVAILABLE;
				csUninstall = NOT_AVAILABLE;
				csInstallDate = _T( "0000/00/00");
				// Read the Publisher
				if (GetValue( hKeyObject, NT_APPS_VENDOR_VALUE, csPublisher) == ERROR_SUCCESS)
				{
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for %s\\%s\\%s !\n"),csCurHive,
									   csSubKey, NT_APPS_VENDOR_VALUE);
					csPublisher = NOT_AVAILABLE;
				}
				// Read the name
				if (GetValue( hKeyObject, NT_APPS_NAME_VALUE, csName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for %s\\%s\\%s !\n"),csCurHive,
									   csSubKey, NT_APPS_NAME_VALUE);
					csName = NOT_AVAILABLE;
				}
				// Read the version
				if (GetValue( hKeyObject, NT_APPS_VERSION_VALUE, csVersion) == ERROR_SUCCESS)
				{
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for %s\\%s\\%s !\n"),csCurHive,
									   csSubKey, NT_APPS_VERSION_VALUE);
					csVersion = NOT_AVAILABLE;
				}
				// Read the folder
				if (GetValue( hKeyObject, NT_APPS_FOLDER_VALUE, csFolder) == ERROR_SUCCESS)
				{
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for %s\\%s\\%s !\n"),csCurHive,
									   csSubKey, NT_APPS_FOLDER_VALUE);
					csFolder = NOT_AVAILABLE;
				}
				// Read the comment
				if (GetValue( hKeyObject, NT_APPS_COMMENTS_VALUE, csComments) == ERROR_SUCCESS)
				{
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for %s\\%s\\%s !\n"),csCurHive,
									   csSubKey, NT_APPS_COMMENTS_VALUE);
					csComments = NOT_AVAILABLE;
				}
				// Read the language, this gives the language of the installation tool not of the application itself!
				if (GetValue( hKeyObject, NT_APPS_LANGUAGE_VALUE, &dwLanguage) == ERROR_SUCCESS)
				{
				    DWORD dwSize = 256;
					TCHAR lpstrData[256];
					DWORD dwResult;

					dwResult = VerLanguageName( dwLanguage, lpstrData, dwSize);
					if ((dwResult != 0) && (dwResult < dwSize - 1))
					{
						lpstrData[dwResult]=0;
						csLanguage = lpstrData;
					}
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for %s\\%s\\%s !\n"),csCurHive,
									   csSubKey, NT_APPS_LANGUAGE_VALUE);
					csLanguage = NOT_AVAILABLE;
				}
				// Read the install date
				if (GetValue( hKeyObject, NT_APPS_INSTALLDATE_VALUE, csInstallDate) == ERROR_SUCCESS)
				{
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for %s\\%s\\%s !\n"),csCurHive,
									   csSubKey, NT_APPS_INSTALLDATE_VALUE);
					csInstallDate = _T( "0000/00/00");
				}
				// Read the uninstall string
/*				if ((GetValue( hKeyObject, NT_APPS_UNINSTALL_VALUE, csUninstall) != ERROR_SUCCESS) &&
					(GetValue( hKeyObject, NT_APPS_QUIETUNINSTALL_VALUE, csUninstall) != ERROR_SUCCESS) &&
					(GetValue( hKeyObject, NT_APPS_MODIFY_VALUE, csUninstall) != ERROR_SUCCESS))
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for %s\\%s\\%s !\n"),csCurHive,
									   csSubKey, NT_APPS_UNINSTALL_VALUE);
					bHaveToStore = FALSE;
				}
*/				RegCloseKey( hKeyObject);
				cApp.Clear();
				cApp.SetPublisher( csPublisher);
				cApp.SetName( csName);
				cApp.SetVersion( csVersion);
				cApp.SetFolder( csFolder);
				cApp.SetComments( csComments);
				cApp.SetFilename( NOT_AVAILABLE);
				cApp.SetFilesize( 0);
				cApp.SetFromRegistry( TRUE);
				cApp.SetGUID( szGUID);
				cApp.SetLanguage( csLanguage);
				cApp.SetInstallDate( csInstallDate, TRUE);
				if (uHiveType == HIVE_WOW64_64KEY)
					cApp.SetMemoryAddressWidth( 64);
				else
					cApp.SetMemoryAddressWidth( 32);
				AddLog( _T( "\t\t<Publisher: %s><Name: %s><Version: %s><Folder: %s><Comments: %s><GUID: %s><Language: %s><Install Date: %s><Architecture: %lu bits>\n"), 
					cApp.GetPublisher(), cApp.GetName(), cApp.GetVersion(),
					cApp.GetFolder(), cApp.GetComments(), cApp.GetGUID(),
					cApp.GetLanguage(), cApp.GetInstallDate(), cApp.GetMemoryAddressWidth());
				// Add the app to the apps list
				if (bHaveToStore)
				{
					pList->AddTail( cApp);
					uIndex ++;
				}
			}
			else
				AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for %s\\%s !\n"),csCurHive,
								   csSubKey);
			// Enum the next device
			dwLength = 255;
			dwIndexEnum++;
		}
		RegCloseKey( hKeyEnum);
		if (dwIndexEnum == 0)
			// No key found
			AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of %s\\%s.\n"),csCurHive,
					NT_APPS_KEY);
		else
			AddLog( _T( "\tOK (%u objects).\n"), uIndex);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for %s\\%s !\n"),csCurHive,
				NT_APPS_KEY);
	return !pList->IsEmpty();
}

BOOL CRegistry::GetDeviceDescription( CString &csDescription)
{
	// Try to get Modems Informations info from the registry
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetDeviceDescriptionNT( csDescription);
	default:
		// Unknown
		AddLog( _T( "Registry GetDeviceDescription...\n\tFailed because unsupported or unrecognized OS !\n"));
		return NULL;
	}
	return NULL;
}

BOOL CRegistry::GetDeviceDescriptionNT( CString &csDescription)
{
	HKEY		 hKey = NULL;

	csDescription = NOT_AVAILABLE;
	AddLog( _T( "Registry NT GetDeviceDescription...\n"));
	if (RegOpenKeyEx( m_hKey, NT_COMPUTER_DESCRIPTION_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKey) != ERROR_SUCCESS)
	{
		AddLog( _T( "\tFailed in call to <RegOpenKey> function for HKLM\\%s !\n"),
				NT_COMPUTER_DESCRIPTION_KEY);
		return FALSE;
	}

	// Get description.
	if (GetValue( hKey, NT_COMPUTER_DESCRIPTION_VALUE, csDescription) != ERROR_SUCCESS)
	{
		AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
				NT_COMPUTER_DESCRIPTION_KEY,
				NT_COMPUTER_DESCRIPTION_VALUE);
		RegCloseKey( hKey);
		return FALSE;
	}
	RegCloseKey( hKey);
	AddLog( _T( "\t\t<Description: %s>\n\tOK.\n"), csDescription);
	return TRUE;
}

BOOL CRegistry::GetDomainOrWorkgroup(CString &csDomain)
{
	// Try to get Modems Informations info from the registry
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetDomainOrWorkgroupNT( csDomain);
	default:
		// Unknown
		csDomain = NOT_AVAILABLE;
		AddLog( _T( "Registry GetDomainOrWorkgroup...\n\tFailed because unsupported or unrecognized OS !\n"));
		return NULL;
	}
	return NULL;
}

BOOL CRegistry::GetDomainOrWorkgroupNT(CString &csDomain)
{
	HKEY  hKey = NULL;

	csDomain = NOT_AVAILABLE;
	AddLog( _T( "Registry NT GetDomainOrWorkgroup...\n"));
	if (RegOpenKeyEx( m_hKey, NT_DOMAIN_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKey) != ERROR_SUCCESS)
	{
		AddLog( _T( "\tFailed in call to <RegOpenKey> function for HKLM\\%s !\n"),
				NT_DOMAIN_KEY);
		return FALSE;
	}

	// Get domain name or workgroup.
	if (GetValue( hKey, NT_DOMAIN_VALUE, csDomain) != ERROR_SUCCESS)
	{
		AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
				NT_DOMAIN_KEY,
				NT_DOMAIN_VALUE);
		RegCloseKey( hKey);
		return FALSE;
	}
	RegCloseKey( hKey);
	AddLog( _T( "\t\t<Domain: %s>\n\tOK\n"), csDomain);
	return TRUE;
}

BOOL CRegistry::GetWindowsRegistration(CString &csCompany, CString &csOwner, CString &csProductID)
{
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetWindowsRegistrationNT( csCompany, csOwner, csProductID);
	default:
		// Unknown
		csCompany = NOT_AVAILABLE;
		csOwner = NOT_AVAILABLE;
		csProductID = NOT_AVAILABLE;
		AddLog( _T( "Registry GetWindowsRegistration...\n\tFailed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::GetWindowsRegistrationNT(CString &csCompany, CString &csOwner, CString &csProductID)
{
	HKEY			hKeyObject;
	CString			csSubKey;

	AddLog( _T( "Registry NT GetWindowsRegistration...\n"));
	// Windows NT => Open the registration key
	if (RegOpenKeyEx( m_hKey, NT_REGISTRATION_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyObject) == ERROR_SUCCESS)
	{
		// Read the company name
		if (GetValue( hKeyObject, NT_REGISTRATION_COMPANY_VALUE, csCompany) == ERROR_SUCCESS)
		{
		}
		else
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
							   NT_REGISTRATION_KEY, NT_REGISTRATION_COMPANY_VALUE);
			csCompany = NOT_AVAILABLE;
		}
		// Read the user
		if (GetValue( hKeyObject, NT_REGISTRATION_OWNER_VALUE, csOwner) == ERROR_SUCCESS)
		{
		}
		else
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
							   NT_REGISTRATION_KEY, NT_REGISTRATION_OWNER_VALUE);
			csOwner = NOT_AVAILABLE;
		}
		// Read the productID
		if (GetValue( hKeyObject, NT_REGISTRATION_PRODUCT_ID_VALUE, csProductID) == ERROR_SUCCESS)
		{
		}
		else
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
							   NT_REGISTRATION_KEY, NT_REGISTRATION_PRODUCT_ID_VALUE);
			csProductID = NOT_AVAILABLE;
		}
		RegCloseKey( hKeyObject);
		// Add the device to the adapter list
		StrForSQL( csCompany);
		StrForSQL( csOwner);
		StrForSQL( csProductID);
		AddLog( _T( "\t\t<Organization: %s><User: %s><S/N: %s>\n\tOK\n"), csCompany, csOwner, csProductID);
		return TRUE;
	}
	AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
			NT_REGISTRATION_KEY);
	return FALSE;
}



BOOL CRegistry::GetWindowsProductKey(CString &csProductKey)
{	
	char KeyChars[24] = {'B','C','D','F','G','H','J','K','M','P','Q','R','T','V','W','X','Y','2','3','4','6','7','8','9'};
	char Result[30];

	CString csKeyPath, csValueName, csValue4Name;

	// Registry access variables
	HKEY InfoKey = NULL;
	DWORD InfoType = 0;

	// Decoding variables
	BYTE Data[0xA7];
	BYTE BinaryKey[15]; // Part of raw data contains the binary Product Key
	char DecodedKey[25]; // Decoded Product Key is placed here
	DWORD dwDataSize = sizeof( Data );
	UINT A;
	int i,j;

	AddLog( _T( "Registry GetWindowsProductKey...\n"));

	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		csKeyPath = NT_REGISTRATION_KEY;
		csValueName = NT_REGISTRATION_DIGITAL_ID_VALUE;
		break;
	default:
		// Unknown
		csProductKey = NOT_AVAILABLE;
		AddLog( _T( "\tFailed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}

	//Requete registry
	if( RegOpenKeyEx( m_hKey, csKeyPath, 0, KEY_READ|KEY_WOW64_64KEY, &InfoKey ) == ERROR_SUCCESS )
	{
		if( RegQueryValueEx(InfoKey, csValueName, NULL, &InfoType, Data, &dwDataSize) == ERROR_SUCCESS )
		{
			// We need 15 bytes from $35 in BinaryKey
			memcpy( BinaryKey, &Data[0x34], sizeof(BinaryKey));

			for( i=24; i>=0; i-- ){
				A = 0;
				// decoding the current symbol
				for( j=14; j>=0; j-- ){
					A = ((A<<8) + BinaryKey[j]);
					BinaryKey[j] = int(A / 24);
					A = A % 24;
				}
				DecodedKey[i] = KeyChars[A];
			}

			
			j=0;
			for( i=0; i<25; i++ ){
				strcpy(&Result[j],&DecodedKey[i]);
				j++;
				if ( ((i + 1) % 5 == 0) && (i < 24) ){
					strcpy(&Result[j], "-");
					j++;
				}
			}

			Result[29]='\0';
			csProductKey = Result;

			// Check Volume License Key
			if (csProductKey.CompareNoCase( _T( "BBBBB-BBBBB-BBBBB-BBBBB-BBBBB")) == 0)
				csProductKey = _T( "MAK or VLK key unavailable"); 

			AddLog( _T( "\t\t<Product key: %s>\n\tOK\n"), csProductKey);

			return TRUE;
		}
		else{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s !\n"), csValueName);
		}
	}
	else{
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
			csKeyPath);
	}
	return FALSE;
}

BOOL CRegistry::GetLoggedOnUser(CString &csUser)
{
	// Try to get Modems Informations info from the registry
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		if (m_bIsVista)
			// Vista/2008 or +
			return GetLoggedOnUserVista( csUser);
		// Windows NT/2000/XP/2003
		return GetLoggedOnUserNT( csUser);
	default:
		// Unknown
		csUser = NOT_AVAILABLE;
		AddLog( _T( "Registry GetLoggedOnUser...\n\tFailed because unsupported or unrecognized OS !\n"));
		return NULL;
	}
	return NULL;
}

BOOL CRegistry::GetLoggedOnUserNT(CString &csUser)
{
	HKEY  hKey = NULL;

	csUser = NOT_AVAILABLE;
	AddLog( _T( "Registry NT GetLoggedOnUser: Trying to get Logon User Name...\n"));
	// Try to get logged on user
	if (RegOpenKeyEx( m_hKey, NT_LOGON_USER_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS)
	{
		// Get user name.
		if (GetValue( hKey, NT_LOGON_USER_VALUE, csUser) == ERROR_SUCCESS)
		{
			RegCloseKey( hKey);
			AddLog( _T( "\t\t<User: %s>\n\tOK\n"), csUser);
			return TRUE;
		}
		AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKCU\\%s\\%s !\n"),
				NT_LOGON_USER_KEY,
				NT_LOGON_USER_VALUE);
		RegCloseKey( hKey);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKey> function for HKCU\\%s !\n"),
				NT_LOGON_USER_KEY);
	return FALSE;
}

BOOL CRegistry::GetLoggedOnUserVista(CString &csUser)
{
	HKEY  hKey = NULL;

	csUser = NOT_AVAILABLE;
	AddLog( _T( "Registry Vista GetLoggedOnUser: Trying to get Logon User Name...\n"));
	// Try to get logged on user
	if (RegOpenKeyEx( m_hKey, VISTA_LOGON_USER_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS)
	{
		// Get user name.
		if (GetValue( hKey, VISTA_LOGON_USER_VALUE, csUser) == ERROR_SUCCESS)
		{
			RegCloseKey( hKey);
			AddLog( _T( "\t\t<User: %s>\n\tOK\n"), csUser);
			return TRUE;
		}
		AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKCU\\%s\\%s !\n"),
				VISTA_LOGON_USER_KEY,
				VISTA_LOGON_USER_VALUE);
		RegCloseKey( hKey);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKey> function for HKCU\\%s !\n"),
				VISTA_LOGON_USER_KEY);
	return FALSE;
}


BOOL CRegistry::GetLastLoggedUser(CString &csLastLoggedUser)
{
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		if (m_bIsVista)
			// Windows Vista/Seven/8/2008/2012
			return GetLastLoggedUserVista( csLastLoggedUser);
		// Windows NT/2000/XP/2003
		return GetLastLoggedUserNT( csLastLoggedUser);
	default:
		// Unknown
		AddLog( _T( "Registry GetInputDevices...\n\tFailed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::GetLastLoggedUserNT(CString &csLastLoggedUser)
{
	HKEY  hKey = NULL;
	LONG lResult;

  	AddLog( _T( "Registry NT GetLastLoggedUser: Trying to get the last user who'd been logged in...\n"));
	// Since Windows 2000, deprecated on Vista
	if (RegOpenKeyEx( m_hKey, NT_LASTLOGGEDUSER_USER_KEY, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		AddLog( _T( "\tFailed in retrieve Last Logged User from registry!\n"));
		csLastLoggedUser = NOT_AVAILABLE;
		return FALSE;
	} 
	lResult = GetValue( hKey, NT_LASTLOGGEDUSER_USER_VALUE, csLastLoggedUser);
	RegCloseKey( hKey);
	if (lResult == ERROR_SUCCESS) 
	{
		AddLog( _T( "\t\t<Last logged: %s>\n\tOK\n"), csLastLoggedUser);
		return TRUE;
	}
	AddLog( _T( "\tFailed in read Last Logged User from registry!\n"));
	csLastLoggedUser = NOT_AVAILABLE;
	return FALSE;
}

BOOL CRegistry::GetLastLoggedUserVista(CString &csLastLoggedUser)
{
	HKEY  hKey = NULL;
	LONG lResult;

  	AddLog( _T( "Registry Vista GetLastLoggedUser: Trying to get the last user who'd been logged in...\n"));
	// Since Windows 2000, deprecated on Vista
	if (RegOpenKeyEx( m_hKey, VISTA_LASTLOGGEDUSER_USER_KEY, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		AddLog( _T( "\tFailed in retrieve Last Logged User from registry!\n"));
		csLastLoggedUser = NOT_AVAILABLE;
		return FALSE;
	} 
	lResult = GetValue( hKey, VISTA_LASTLOGGEDUSER_USER_VALUE, csLastLoggedUser);
	RegCloseKey( hKey);
	if (lResult == ERROR_SUCCESS) 
	{
		AddLog( _T( "\t\t<Last logged: %s>\n\tOK\n"), csLastLoggedUser);
		return TRUE;
	}
	AddLog( _T( "\tFailed in read Last Logged User from registry!\n"));
	csLastLoggedUser = NOT_AVAILABLE;
	return FALSE;
}

BOOL CRegistry::GetInputDevices(CInputDeviceList *pList)
{
	ASSERT( pList);

	// Reset object list content
	while (!(pList->GetCount() == 0))
		pList->RemoveHead();
	// Try to get system Controllers Informations info from the registry
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetInputDevicesNT( pList);
	default:
		// Unknown
		AddLog( _T( "Registry GetInputDevices...\n\tFailed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::GetInputDevicesNT(CInputDeviceList *pList)
{
	HKEY			hKeyEnum,
					hKeyObject;
	CString			csSubKey,
					csManufacturer,
					csCaption,
					csDescription,
					csPointingType,
					csPointingInterface;
	TCHAR			szKey[256];
	DWORD			dwLength,
					dwIndexEnum = 0;
	LONG			lResult;
	FILETIME		MyFileTime;
	CInputDevice	myObject;
	BOOL			bHaveToStore;
	UINT			uIndex = 0,
					uTotalIndex = 0;

	ASSERT( pList);

	// Windows NT => Open the keyboard key
	uIndex = 0;
	dwIndexEnum = 0;
	AddLog( _T( "Registry NT GetInputDevices: Trying to find Keyboards in HKLM\\%s...\n"),
			NT_INPUT_KEYBOARD_KEY);
	if (RegOpenKeyEx( m_hKey, NT_INPUT_KEYBOARD_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_INPUT_KEYBOARD_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyObject) == ERROR_SUCCESS)
			{
				csManufacturer = NOT_AVAILABLE; 
				csCaption = NOT_AVAILABLE; 
				csDescription = NOT_AVAILABLE; 
				csPointingType = NOT_AVAILABLE; 
				csPointingInterface = NOT_AVAILABLE; 
				// Read the manufactuer
				if (GetValue( hKeyObject, NT_INPUT_KEYBOARD_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_INPUT_KEYBOARD_MANUFACTURER_VALUE);
					csManufacturer = NOT_AVAILABLE; 
				}
				// Read the caption
				if (GetValue( hKeyObject, NT_INPUT_KEYBOARD_CAPTION_VALUE, csCaption) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_INPUT_KEYBOARD_CAPTION_VALUE);
					csCaption = NOT_AVAILABLE; 
				}
				// Read the description
				if (GetValue( hKeyObject, NT_INPUT_KEYBOARD_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_INPUT_KEYBOARD_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				// Read the pointing type
				if (GetValue( hKeyObject, NT_INPUT_KEYBOARD_TYPE_VALUE, csPointingType) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_INPUT_KEYBOARD_TYPE_VALUE);
					csPointingType = NOT_AVAILABLE; 
				}
				// Read the pointing interface
				if (GetValue( hKeyObject, NT_INPUT_KEYBOARD_INTERFACE_VALUE, csPointingInterface) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_INPUT_KEYBOARD_INTERFACE_VALUE);
					csPointingInterface = NOT_AVAILABLE; 
				}
				RegCloseKey( hKeyObject);
				myObject.SetType( INPUT_DEVICE_KEYBOARD);
				myObject.SetManufacturer( csManufacturer);
				myObject.SetCaption( csCaption);
				myObject.SetDescription( csDescription);
				myObject.SetPointingType( csPointingType);
				myObject.SetPointingInterface( csPointingInterface);
				AddLog( _T( "\t\t<Type: %s><Manufacturer: %s><Caption: %s><Description: %s><Pointing Type: %s><Interface: %s>\n"), 
					myObject.GetType(), myObject.GetManufacturer(), myObject.GetCaption(), myObject.GetDescription(),
					myObject.GetPointingType(), myObject.GetPointingInterface());
				// Add the device to the adapter list
				if (bHaveToStore)
				{
					pList->AddTail( myObject);
					uIndex ++;
				}
			}
			else
				AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
								   csSubKey);
			// Enum the next device
			dwLength = 255;
			dwIndexEnum++;
		}
		RegCloseKey( hKeyEnum);
		if (dwIndexEnum == 0)
			// No key found
			AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_INPUT_KEYBOARD_KEY);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_INPUT_KEYBOARD_KEY);
	if (uIndex > 0)
	{
		uTotalIndex += uIndex;
		AddLog( _T( "\tOK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "\tFailed because no valid object !\n"));
	// Windows NT => Open the Pointing Devices key
	uIndex = 0;
	dwIndexEnum = 0;
	AddLog( _T( "Registry NT GetInputDevices: Trying to find Pointing Devices in HKLM\\%s...\n"),
			NT_INPUT_POINTING_KEY);
	if (RegOpenKeyEx( m_hKey, NT_INPUT_POINTING_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_INPUT_POINTING_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyObject) == ERROR_SUCCESS)
			{
				csManufacturer = NOT_AVAILABLE; 
				csCaption = NOT_AVAILABLE; 
				csDescription = NOT_AVAILABLE; 
				csPointingType = NOT_AVAILABLE; 
				csPointingInterface = NOT_AVAILABLE; 
				// Read the manufactuer
				if (GetValue( hKeyObject, NT_INPUT_KEYBOARD_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_INPUT_KEYBOARD_MANUFACTURER_VALUE);
					csManufacturer = NOT_AVAILABLE;
				}
				// Read the caption
				if (GetValue( hKeyObject, NT_INPUT_KEYBOARD_CAPTION_VALUE, csCaption) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_INPUT_KEYBOARD_CAPTION_VALUE);
					csCaption = NOT_AVAILABLE; 
				}
				// Read the description
				if (GetValue( hKeyObject, NT_INPUT_KEYBOARD_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_INPUT_KEYBOARD_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				// Read the pointing type
				if (GetValue( hKeyObject, NT_INPUT_KEYBOARD_TYPE_VALUE, csPointingType) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_INPUT_KEYBOARD_TYPE_VALUE);
					csPointingType = NOT_AVAILABLE;
				}
				// Read the pointing interface
				if (GetValue( hKeyObject, NT_INPUT_KEYBOARD_INTERFACE_VALUE, csPointingInterface) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_INPUT_KEYBOARD_INTERFACE_VALUE);
					csPointingInterface = NOT_AVAILABLE; 
				}
				RegCloseKey( hKeyObject);
				myObject.SetType( INPUT_DEVICE_POINTING);
				myObject.SetManufacturer( csManufacturer);
				myObject.SetCaption( csCaption);
				myObject.SetDescription( csDescription);
				myObject.SetPointingType( csPointingType);
				myObject.SetPointingInterface( csPointingInterface);
				AddLog( _T( "\t\t<Type: %s><Manufacturer: %s><Caption: %s><Description: %s><Pointing Type: %s><Interface: %s>\n"), 
					myObject.GetType(), myObject.GetManufacturer(), myObject.GetCaption(), myObject.GetDescription(),
					myObject.GetPointingType(), myObject.GetPointingInterface());
				// Add the device to the adapter list
				if (bHaveToStore)
				{
					pList->AddTail( myObject);
					uIndex ++;
				}
			}
			else
				AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
								   csSubKey);
			// Enum the next device
			dwLength = 255;
			dwIndexEnum++;
		}
		RegCloseKey( hKeyEnum);
		if (dwIndexEnum == 0)
			// No key found
			AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_INPUT_POINTING_KEY);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_INPUT_POINTING_KEY);
	if (uIndex > 0)
	{
		uTotalIndex += uIndex;
		AddLog( _T( "\tOK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "\tFailed because no valid object !\n"));
	return (uTotalIndex > 0);
}

BOOL CRegistry::GetSystemPorts(CSystemPortList *pList)
{
	ASSERT( pList);

	// Reset object list content
	while (!(pList->GetCount() == 0))
		pList->RemoveHead();
	// Try to get system ports Informations info from the registry
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetSystemPortsNT( pList);
	default:
		// Unknown
		AddLog( _T( "Registry GetSystemPorts...\n\tFailed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::GetSystemPortsNT(CSystemPortList *pList)
{
	HKEY			hKeyEnum,
					hKeyObject;
	CString			csSubKey,
					csType,
					csName,
					csDescription,
					csCaption;
	TCHAR			szKey[256];
	DWORD			dwLength,
					dwValue,
					dwIndexEnum = 0;
	LONG			lResult;
	FILETIME		MyFileTime;
	CSystemPort		myObject;
	BOOL			bHaveToStore;
	UINT			uIndex = 0;

	ASSERT( pList);

	AddLog( _T( "Registry NT GetSystemPorts...\n"));
	// Windows 9X => Open the modem key
	if (RegOpenKeyEx( m_hKey, NT_SYSTEM_PORT_KEY, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_SYSTEM_PORT_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKeyObject) == ERROR_SUCCESS)
			{
				csType = NOT_AVAILABLE; 
				csName = NOT_AVAILABLE; 
				csCaption = NOT_AVAILABLE; 
				csDescription = NOT_AVAILABLE; 
				// Read the type, i.e the serial or parallel
				if (GetValue( hKeyObject, NT_SYSTEM_PORT_TYPE_VALUE, &dwValue) == ERROR_SUCCESS)
				{
					switch (dwValue)
					{
					case NT_SYSTEM_PORT_CLASS_SERIAL:
						csType = SYSTEM_PORT_SERIAL;
						break;
					case NT_SYSTEM_PORT_CLASS_PARALLEL:
						csType = SYSTEM_PORT_PARALLEL;
						break;
					default:
						csType = NOT_AVAILABLE;
						break;
					}
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_SYSTEM_PORT_TYPE_VALUE);
					csType = NOT_AVAILABLE;
				}
				// Read the name
				if (GetValue( hKeyObject, NT_SYSTEM_PORT_NAME_VALUE, csName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey,NT_SYSTEM_PORT_NAME_VALUE);
					csName = NOT_AVAILABLE; 
				}
				// Read the caption
				if (GetValue( hKeyObject, NT_SYSTEM_PORT_CAPTION_VALUE, csCaption) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_SYSTEM_PORT_CAPTION_VALUE);
					csCaption = NOT_AVAILABLE; 
				}
				// Read the description
				if (GetValue( hKeyObject, NT_SYSTEM_PORT_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, NT_SYSTEM_PORT_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				RegCloseKey( hKeyObject);
				myObject.SetType( csType);
				myObject.SetName( csName);
				myObject.SetCaption( csCaption);
				myObject.SetDescription( csDescription);
				AddLog( _T( "\t\t<Name: %s><Type: %s><Caption: %s><Description: %s>\n"), 
					myObject.GetName(), myObject.GetType(), myObject.GetCaption(), myObject.GetDescription());
				// Add the device to the adapter list
				if (bHaveToStore)
				{
					pList->AddTail( myObject);
					uIndex ++;
				}
			}
			else
				AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
								   csSubKey);
			// Enum the next device
			dwLength = 255;
			dwIndexEnum++;
		}
		RegCloseKey( hKeyEnum);
		if (dwIndexEnum == 0)
			// No key found
			AddLog( _T( "\tFailed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_SYSTEM_PORT_KEY);
		else
			AddLog( _T( "\tOK (%u objects).\n"), uIndex);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_SYSTEM_PORT_KEY);
	return !pList->IsEmpty();
}

BOOL CRegistry::GetRegistryValue( UINT uKeyTree, LPCTSTR lpstrSubKey, LPCTSTR lpstrValue, CString &csResult)
{
	HKEY	hKey = NULL;
	LONG	lResult;
	csResult = NOT_AVAILABLE;
	
	switch (uKeyTree)
	{
	case HKCR_TREE: // HKEY_CLASSES_ROOT
		AddLog( _T( "Registry GetRegistryValue (%s\\%s\\%s)..."),
			_T( "HKCR"),
			lpstrSubKey,
			lpstrValue);
		lResult = RegOpenKeyEx( HKEY_CLASSES_ROOT, lpstrSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKey);
		break;
	case HKCU_TREE: // HKEY_CURRENT_USER
		AddLog( _T( "Registry GetRegistryValue (%s\\%s\\%s)..."),
			_T( "HKCU"),
			lpstrSubKey,
			lpstrValue);
		lResult = RegOpenKeyEx( HKEY_CURRENT_USER, lpstrSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKey);
		break;
	case HKLM_TREE: // HKEY_LOCAL_MACHINE
		AddLog( _T( "Registry GetRegistryValue (%s\\%s\\%s)..."),
			_T( "HKLM"),
			lpstrSubKey,
			lpstrValue);
		lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, lpstrSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKey);
		break;
	case HKU_TREE: // HKEY_USERS
		AddLog( _T( "Registry GetRegistryValue (%s\\%s\\%s)..."),
			_T( "HKU"),
			lpstrSubKey,
			lpstrValue);
		lResult = RegOpenKeyEx( HKEY_USERS, lpstrSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKey);
		break;
	case HKCC_TREE: // HKEY_CURRENT_CONFIG
		AddLog( _T( "Registry GetRegistryValue (%s\\%s\\%s)..."),
			_T( "HKCC"),
			lpstrSubKey,
			lpstrValue);
		lResult = RegOpenKeyEx( HKEY_CURRENT_CONFIG, lpstrSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKey);
		break;
	default: // Error
		AddLog( _T( "Registry GetRegistryValue...Failed because unrecognized Registry Tree %u !\n"), uKeyTree);
		return FALSE;
	}
	if (lResult != ERROR_SUCCESS)
	{
		AddLog( _T( "Failed in call to <RegOpenKey> function for subkey %s of selected hive !\n"),
				lpstrSubKey);
		return FALSE;
	}
	// Get value
	if (GetValue( hKey, lpstrValue, csResult) != ERROR_SUCCESS)
	{
		AddLog( _T( "Failed in call to <RegQueryValueEx> function for value %s\\%s of selected hive (unable to get value)!\n"),
				lpstrSubKey,
				lpstrValue);
		RegCloseKey( hKey);
		csResult = NOT_AVAILABLE;
		return FALSE;
	}
	RegCloseKey( hKey);
	AddLog( _T( "OK (%s).\n"), csResult);
	return TRUE;
}

BOOL CRegistry::GetRegistryMultipleValues( LPCTSTR lpstrQueryName, UINT uKeyTree, LPCTSTR lpstrSubKey, CRegistryValueList *pMyList)
{
	TCHAR	szName[256];
    BYTE	*lpValue = NULL;
	HKEY	hKey = NULL;
	LONG	lResult;
	CString csResult,
			csValue;
	DWORD	dwIndex = 0,
			dwSizeName = 255;
	CRegistryValue myRegistry;

	ASSERT( pMyList);

	switch (uKeyTree)
	{
	case HKCR_TREE: // HKEY_CLASSES_ROOT
		AddLog( _T( "Registry GetRegistryMultipleValues (%s\\%s)...\n"),
			_T( "HKCR"),
			lpstrSubKey);
		lResult = RegOpenKeyEx( HKEY_CLASSES_ROOT, lpstrSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKey);
		break;
	case HKCU_TREE: // HKEY_CURRENT_USER
		AddLog( _T( "Registry GetRegistryMultipleValues (%s\\%s)...\n"),
			_T( "HKCU"),
			lpstrSubKey);
		lResult = RegOpenKeyEx( HKEY_CURRENT_USER, lpstrSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKey);
		break;
	case HKLM_TREE: // HKEY_LOCAL_MACHINE
		AddLog( _T( "Registry GetRegistryMultipleValues (%s\\%s)...\n"),
			_T( "HKLM"),
			lpstrSubKey);
		lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, lpstrSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKey);
		break;
	case HKU_TREE: // HKEY_USERS
		AddLog( _T( "Registry GetRegistryMultipleValues (%s\\%s)...\n"),
			_T( "HKU"),
			lpstrSubKey);
		lResult = RegOpenKeyEx( HKEY_USERS, lpstrSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKey);
		break;
	case HKCC_TREE: // HKEY_CURRENT_CONFIG
		AddLog( _T( "Registry GetRegistryMultipleValues (%s\\%s)...\n"),
			_T( "HKCC"),
			lpstrSubKey);
		lResult = RegOpenKeyEx( HKEY_CURRENT_CONFIG, lpstrSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hKey);
		break;
	default: // Error
		AddLog( _T( "Registry GetRegistryMultipleValues...Failed because unrecognized Registry Tree %u !\n"), uKeyTree);
		return FALSE;
	}
	if (lResult != ERROR_SUCCESS)
	{
		AddLog( _T( "\tFailed in call to <RegOpenKey> function for subkey %s of selected hive !\n"),
				lpstrSubKey);
		return FALSE;
	}
	while ((lResult = RegEnumValue( hKey, dwIndex, szName, &dwSizeName, NULL, NULL, NULL, NULL)) != ERROR_NO_MORE_ITEMS)
	{
		if ((lResult != ERROR_SUCCESS) && (lResult != ERROR_MORE_DATA) && (lResult != ERROR_NO_MORE_ITEMS))
		{
			// Error while reading the value
			AddLog( _T( "\tFailed in call to <RegEnumKey> function for value %lu in key %s of selected hive (unable to get value size) !\n"),
					dwIndex,
					lpstrSubKey);
		}
		else
		{
			csResult = NOT_AVAILABLE;
			szName[dwSizeName]=0;
			if (GetValue( hKey, szName, csValue) != ERROR_SUCCESS)
			{
				AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for value %s in key %s of selected hive !\n"),
						szName,
						lpstrSubKey);
			}
			else
			{
				csResult.Format( _T( "%s=%s"), szName, csValue);
				AddLog( _T("\tValue %lu: %s\n"), dwIndex, csValue);
				myRegistry.Set( lpstrQueryName, csResult);
				pMyList->AddTail( myRegistry);
			}
		}
		// Next
		dwSizeName=255;
		dwIndex++;
	}
	RegCloseKey( hKey);
	AddLog( _T( "Registry GetRegistryMultipleValues...OK (%u values read).\n"), dwIndex);
	return TRUE;
}
