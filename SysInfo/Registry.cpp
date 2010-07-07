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

// Defines for retrieving Windows registration
#define WIN_REGISTRATION_KEY					_T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion")
#define WIN_REGISTRATION_PRODUCT_ID_VALUE		_T( "ProductId")
#define WIN_REGISTRATION_DIGITAL_ID_VALUE		_T( "DigitalProductId");
#define WIN_REGISTRATION_COMPANY_VALUE			_T( "RegisteredOrganization")
#define WIN_REGISTRATION_OWNER_VALUE			_T( "RegisteredOwner")

#define NT_REGISTRATION_KEY						_T( "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion")
#define NT_REGISTRATION_PRODUCT_ID_VALUE		_T( "ProductId")
#define NT_REGISTRATION_DIGITAL_ID_VALUE		_T( "DigitalProductId");
#define NT_REGISTRATION_COMPANY_VALUE			_T( "RegisteredOrganization")
#define NT_REGISTRATION_OWNER_VALUE				_T( "RegisteredOwner")

// Defines for retrieving logon username from 9X/Me registry
#define WIN_LOGON_USER_KEY						_T( "SYSTEM\\CurrentControlSet\\Control")
#define WIN_LOGON_USER_VALUE					_T( "Current User")

#define NT_LOGON_USER_KEY						_T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer")
#define NT_LOGON_USER_VALUE						_T( "Logon User Name")

// Defines for retrieving last user who'd been logged in
#define NT_LASTLOGGEDUSER_USER_KEY				_T( "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon")
#define NT_LASTLOGGEDUSER_USER_VALUE			_T( "DefaultUserName")

// Defines for retrieving last user who'd been logged in on Vista
#define VISTA_LASTLOGGEDUSER_USER_KEY			_T( "Volatile Environment")
#define VISTA_LASTLOGGEDUSER_USER_VALUE			_T( "USERNAME" )

// Defines for retrieving computer description from 9X/Me registry
#define WIN_COMPUTER_DESCRIPTION_KEY			_T( "SYSTEM\\CurrentControlSet\\Services\\VxD\\VNETSUP")
#define WIN_COMPUTER_DESCRIPTION_VALUE			_T( "Comment")

// Defines for retrieving computer description from NT registry
#define NT_COMPUTER_DESCRIPTION_KEY				_T( "SYSTEM\\CurrentControlSet\\Services\\lanmanserver\\Parameters")
#define NT_COMPUTER_DESCRIPTION_VALUE			_T( "srvcomment")

// Defines for retrieving domain or workgroup from 9X/Me registry
#define WIN_DOMAIN_KEY							_T( "Security\\Provider")
#define WIN_DOMAIN_VALUE						_T( "Container")
#define WIN_WORKGROUP_KEY						_T( "SYSTEM\\CurrentControlSet\\Services\\VxD\\VNETSUP")
#define WIN_WORKGROUP_VALUE						_T( "Workgroup")

// Defines for retrieving domain or workgroup from NT registry
#define NT_DOMAIN_KEY							_T( "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon")
#define NT_DOMAIN_VALUE							_T( "DefaultDomainName")
#define NT_WORKGROUP_KEY						_T( "SYSTEM\\CurrentControlSet\\Services\\VxD\\VNETSUP")
#define NT_WORKGROUP_VALUE						_T( "Workgroup")

// Defines for retrieving BIOS from 9X/Me registry
#define WIN_BIOS_KEY							_T( "ENUM\\Root\\*PNP0C01\\0000")
#define WIN_SYSTEM_MANUFACTURER_VALUE			_T( "Mfg")
#define WIN_SYSTEM_MODEL_VALUE					_T( "Model")
#define WIN_SYSTEM_SERIAL_VALUE					_T( "Serial")
#define WIN_BIOS_MACHINE_TYPE_VALUE				_T( "MachineType")
#define WIN_BIOS_MANUFACTURER_VALUE				_T( "BIOSName")
#define WIN_BIOS_VERSION_VALUE					_T( "BIOSVersion")
#define WIN_BIOS_DATE_VALUE						_T( "BIOSDate")

// Defines for retrieving BIOS from NT registry
#define NT_BIOS_KEY								_T( "HARDWARE\\Description\\System")
#define NT_SYSTEM_MANUFACTURER_VALUE			_T( "Manufacturer")
#define NT_SYSTEM_MODEL_VALUE					_T( "Model")
#define NT_SYSTEM_SERIAL_VALUE					_T( "Serial")
#define NT_BIOS_MACHINE_TYPE_VALUE				_T( "Identifier")
#define NT_BIOS_MANUFACTURER_VALUE				_T( "BIOSName")
#define NT_BIOS_VERSION_VALUE					_T( "SystemBiosVersion")
#define NT_BIOS_DATE_VALUE						_T( "SystemBiosDate")

// Defines for retrieving BIOS from Windows Update 9X/Me registry
#define WIN_WU_KEY								_T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\OemInfo")
#define WIN_WU_SYSTEM_MANUFACTURER_VALUE		_T( "SmbOem")
#define WIN_WU_SYSTEM_MODEL_VALUE				_T( "SmbProduct")
#define WIN_WU_ACPI_MANUFACTURER_VALUE			_T( "AcpiOem")
#define WIN_WU_ACPI_MODEL_VALUE					_T( "AcpiProduct")

// Defines for retrieving BIOS from Windows Update NT registry
#define NT_WU_KEY								_T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\OemInfo")
#define NT_WU_SYSTEM_MANUFACTURER_VALUE			_T( "WbemOem")
#define NT_WU_SYSTEM_MODEL_VALUE				_T( "WbemProduct")
#define NT_WU_ACPI_MANUFACTURER_VALUE			_T( "AcpiOem")
#define NT_WU_ACPI_MODEL_VALUE					_T( "AcpiProduct")

// Defines for retrieving processors from 9X/Me registry
#define WIN_PROCESSOR_KEY						_T( "HARDWARE\\Description\\System\\CentralProcessor")
#define WIN_PROCESSOR_NAME_VALUE				_T( "Identifier")
#define WIN_PROCESSOR_SPEED_VALUE				_T( "~MHz")

// Defines for retrieving processors from NT registry
#define NT_PROCESSOR_KEY						_T( "HARDWARE\\Description\\System\\CentralProcessor")
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

// Defines for retrieving Display from 9X/Me registry
#define WIN_DISPLAY_KEY							_T( "SYSTEM\\CurrentControlSet\\Services\\Class\\Display")
#define WIN_ADAPTER_NAME_VALUE					_T( "DriverDesc")
#define WIN_DEVICE_INFO_KEY						_T( "INFO")
#define WIN_ADAPTER_CHIP_VALUE					_T( "ChipType")
#define WIN_ADAPTER_MEMORY_VALUE				_T( "VideoMemory")

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

// Defines for retrieving Monitors from 9X/Me registry
#define WIN_MONITOR_KEY							_T( "ENUM\\Monitor\\DEFAULT_MONITOR")
#define WIN_MONITOR_MANUFACTURER_VALUE			_T( "Mfg")
#define WIN_MONITOR_DESCRIPTION_VALUE			_T( "DeviceDesc")
#define WIN_MONITOR_CAPTION_VALUE				_T( "DeviceDesc")
#define WIN_MONITOR_TYPE_VALUE					_T( "DeviceDesc")

// Defines for retrieving Monitors from NT registry
#define NT_MONITOR_KEY							_T( "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E96E-E325-11CE-BFC1-08002BE10318}")
#define NT_MONITOR_MANUFACTURER_VALUE			_T( "ProviderName")
#define NT_MONITOR_DESCRIPTION_VALUE			_T( "DriverDesc")
#define NT_MONITOR_CAPTION_VALUE				_T( "DriverDesc")
#define NT_MONITOR_TYPE_VALUE					_T( "DriverDesc")

// Defines for retrieving input devices from 9X/Me registry
#define WIN_INPUT_KEYBOARD_KEY					_T( "SYSTEM\\CurrentControlSet\\Services\\Class\\Keyboard")
#define WIN_INPUT_KEYBOARD_MANUFACTURER_VALUE	_T( "ProviderName")
#define WIN_INPUT_KEYBOARD_CAPTION_VALUE		_T( "DriverDesc")
#define WIN_INPUT_KEYBOARD_DESCRIPTION_VALUE	_T( "DriverDesc")
#define WIN_INPUT_KEYBOARD_TYPE_VALUE			_T( "")
#define WIN_INPUT_KEYBOARD_INTERFACE_VALUE		_T( "")

#define WIN_INPUT_POINTING_KEY					_T( "SYSTEM\\CurrentControlSet\\Services\\Class\\Mouse")
#define WIN_INPUT_POINTING_MANUFACTURER_VALUE	_T( "ProviderName")
#define WIN_INPUT_POINTING_CAPTION_VALUE		_T( "DriverDesc")
#define WIN_INPUT_POINTING_DESCRIPTION_VALUE	_T( "DriverDesc")
#define WIN_INPUT_POINTING_TYPE_VALUE			_T( "")
#define WIN_INPUT_POINTING_INTERFACE_VALUE		_T( "")

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

// Defines for retrieving system ports from 9X/Me registry
#define WIN_SYSTEM_PORT_CLASS_PARALLEL			0
#define WIN_SYSTEM_PORT_CLASS_SERIAL			1
#define WIN_SYSTEM_PORT_KEY						_T( "SYSTEM\\CurrentControlSet\\Services\\Class\\Ports")
#define WIN_SYSTEM_PORT_NAME_VALUE				_T( "DriverDesc")
#define WIN_SYSTEM_PORT_CAPTION_VALUE			_T( "DriverDesc")
#define WIN_SYSTEM_PORT_DESCRIPTION_VALUE		_T( "DriverDesc")
#define WIN_SYSTEM_PORT_TYPE_VALUE				_T( "PortSubClass")

// Defines for retrieving system ports from NT registry
#define NT_SYSTEM_PORT_CLASS_PARALLEL			0
#define NT_SYSTEM_PORT_CLASS_SERIAL				1
#define NT_SYSTEM_PORT_KEY						_T( "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E978-E325-11CE-BFC1-08002BE10318}")
#define NT_SYSTEM_PORT_NAME_VALUE				_T( "DriverDesc")
#define NT_SYSTEM_PORT_CAPTION_VALUE			_T( "DriverDesc")
#define NT_SYSTEM_PORT_DESCRIPTION_VALUE		_T( "DriverDesc")
#define NT_SYSTEM_PORT_TYPE_VALUE				_T( "PortSubClass")

// Defines for retrieving system controlers from 9X/Me registry
#define WIN_CONTROLER_FLOPPY_KEY				_T( "SYSTEM\\CurrentControlSet\\Services\\Class\\fdc")
#define WIN_CONTROLER_FLOPPY_MANUFACTURER_VALUE _T( "ProviderName")
#define WIN_CONTROLER_FLOPPY_NAME_VALUE			_T( "DriverDesc")
#define WIN_CONTROLER_FLOPPY_CAPTION_VALUE		_T( "DriverDesc")
#define WIN_CONTROLER_FLOPPY_DESCRIPTION_VALUE	_T( "DriverDesc")
#define WIN_CONTROLER_FLOPPY_VERSION_VALUE		_T( "DriverDate")

#define WIN_CONTROLER_IDE_KEY					_T( "SYSTEM\\CurrentControlSet\\Services\\Class\\hdc")
#define WIN_CONTROLER_IDE_MANUFACTURER_VALUE	_T( "ProviderName")
#define WIN_CONTROLER_IDE_NAME_VALUE			_T( "DriverDesc")
#define WIN_CONTROLER_IDE_CAPTION_VALUE			_T( "DriverDesc")
#define WIN_CONTROLER_IDE_DESCRIPTION_VALUE		_T( "DriverDesc")
#define WIN_CONTROLER_IDE_VERSION_VALUE			_T( "DriverDate")

#define WIN_CONTROLER_SCSI_KEY					_T( "SYSTEM\\CurrentControlSet\\Services\\Class\\SCSIAdapter")
#define WIN_CONTROLER_SCSI_MANUFACTURER_VALUE	_T( "ProviderName")
#define WIN_CONTROLER_SCSI_NAME_VALUE			_T( "DriverDesc")
#define WIN_CONTROLER_SCSI_CAPTION_VALUE		_T( "DriverDesc")
#define WIN_CONTROLER_SCSI_DESCRIPTION_VALUE	_T( "DriverDesc")
#define WIN_CONTROLER_SCSI_VERSION_VALUE		_T( "DriverDate")

#define WIN_CONTROLER_INFRARED_KEY				_T( "SYSTEM\\CurrentControlSet\\Services\\Class\\Infrared")
#define WIN_CONTROLER_INFRARED_MANUFACTURER_VALUE _T( "ProviderName")
#define WIN_CONTROLER_INFRARED_NAME_VALUE		_T( "DriverDesc")
#define WIN_CONTROLER_INFRARED_CAPTION_VALUE	_T( "DriverDesc")
#define WIN_CONTROLER_INFRARED_DESCRIPTION_VALUE _T( "DriverDesc")
#define WIN_CONTROLER_INFRARED_VERSION_VALUE	_T( "DriverDate")

#define WIN_CONTROLER_USB_KEY					_T( "SYSTEM\\CurrentControlSet\\Services\\Class\\USB")
#define WIN_CONTROLER_USB_MANUFACTURER_VALUE	_T( "ProviderName")
#define WIN_CONTROLER_USB_NAME_VALUE			_T( "DriverDesc")
#define WIN_CONTROLER_USB_CAPTION_VALUE			_T( "DriverDesc")
#define WIN_CONTROLER_USB_DESCRIPTION_VALUE		_T( "DriverDesc")
#define WIN_CONTROLER_USB_VERSION_VALUE			_T( "DriverDate")

#define WIN_CONTROLER_IEEE1394_KEY				_T( "SYSTEM\\CurrentControlSet\\Services\\Class\\1394")
#define WIN_CONTROLER_IEEE1394_MANUFACTURER_VALUE _T( "ProviderName")
#define WIN_CONTROLER_IEEE1394_NAME_VALUE		_T( "DriverDesc")
#define WIN_CONTROLER_IEEE1394_CAPTION_VALUE	_T( "DriverDesc")
#define WIN_CONTROLER_IEEE1394_DESCRIPTION_VALUE _T( "DriverDesc")
#define WIN_CONTROLER_IEEE1394_VERSION_VALUE	_T( "DriverDate")

#define WIN_CONTROLER_PCMCIA_KEY				_T( "SYSTEM\\CurrentControlSet\\Services\\Class\\PCMCIA")
#define WIN_CONTROLER_PCMCIA_MANUFACTURER_VALUE _T( "ProviderName")
#define WIN_CONTROLER_PCMCIA_NAME_VALUE			_T( "DriverDesc")
#define WIN_CONTROLER_PCMCIA_CAPTION_VALUE		_T( "DriverDesc")
#define WIN_CONTROLER_PCMCIA_DESCRIPTION_VALUE	_T( "DriverDesc")
#define WIN_CONTROLER_PCMCIA_VERSION_VALUE		_T( "DriverDate")

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

// Defines for retrieving storage peripherals from 9X/Me registry
#define WIN_STORAGE_FLOPPY_KEY					_T( "ENUM\\FLOP")
#define WIN_STORAGE_FLOPPY_MANUFACTURER_VALUE	_T( "Manufacturer")
#define WIN_STORAGE_FLOPPY_MODEL_VALUE			_T( "ProductId")
#define WIN_STORAGE_FLOPPY_NAME_VALUE			_T( "DeviceDesc")
#define WIN_STORAGE_FLOPPY_DESCRIPTION_VALUE	_T( "Mfg")
#define WIN_STORAGE_FLOPPY_TYPE_VALUE			_T( "Class")

#define WIN_STORAGE_DISK_KEY					_T( "ENUM\\ESDI")
#define WIN_STORAGE_DISK_MANUFACTURER_VALUE		_T( "Manufacturer")
#define WIN_STORAGE_DISK_MODEL_VALUE			_T( "ProductId")
#define WIN_STORAGE_DISK_NAME_VALUE				_T( "DeviceDesc")
#define WIN_STORAGE_DISK_DESCRIPTION_VALUE		_T( "Mfg")
#define WIN_STORAGE_DISK_TYPE_VALUE				_T( "Class")

#define WIN_STORAGE_OTHER_KEY					_T( "ENUM\\SCSI")
#define WIN_STORAGE_OTHER_MANUFACTURER_VALUE	_T( "Manufacturer")
#define WIN_STORAGE_OTHER_MODEL_VALUE			_T( "ProductId")
#define WIN_STORAGE_OTHER_NAME_VALUE			_T( "DeviceDesc")
#define WIN_STORAGE_OTHER_DESCRIPTION_VALUE		_T( "Mfg")
#define WIN_STORAGE_OTHER_TYPE_VALUE			_T( "Class")

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

// Defines for retrieving Sound devices from 9X/Me registry
#define WIN_SOUND_KEY							_T( "SYSTEM\\CurrentControlSet\\Services\\Class\\MEDIA")
#define WIN_SOUND_MANUFACTURER_VALUE			_T( "ProviderName")
#define WIN_SOUND_NAME_VALUE					_T( "DriverDesc")
#define WIN_SOUND_DESCRIPTION_VALUE				_T( "DriverDesc")
#define WIN_SOUND_DRIVER						_T( "Drivers")
#define WIN_SOUND_DRIVER_CLASS					_T( "SubClasses")

// Defines for retrieving Sound devices from NT registry
#define NT_SOUND_KEY							_T( "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E96C-E325-11CE-BFC1-08002BE10318}")
#define NT_SOUND_MANUFACTURER_VALUE				_T( "ProviderName")
#define NT_SOUND_NAME_VALUE						_T( "DriverDesc")
#define NT_SOUND_DESCRIPTION_VALUE				_T( "DriverDesc")
#define NT_SOUND_DRIVER							_T( "Drivers")
#define NT_SOUND_DRIVER_CLASS					_T( "SubClasses")

// Defines for retrieving Modems from 9X/Me registry
#define WIN_MODEM_KEY							_T( "SYSTEM\\CurrentControlSet\\Services\\Class\\Modem")
#define WIN_MODEM_MODEL_VALUE					_T( "Model")
#define WIN_MODEM_NAME_VALUE					_T( "DriverDesc")
#define WIN_MODEM_DESCRIPTION_VALUE				_T( "DriverDesc")
#define WIN_MODEM_TYPE_VALUE					_T( "AttachedTo")

// Defines for retrieving Modems from NT registry
#define NT_MODEM_KEY							_T( "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E96D-E325-11CE-BFC1-08002BE10318}")
#define NT_MODEM_MODEL_VALUE					_T( "Model")
#define NT_MODEM_NAME_VALUE						_T( "DriverDesc")
#define NT_MODEM_DESCRIPTION_VALUE				_T( "DriverDesc")
#define NT_MODEM_TYPE_VALUE						_T( "AttachedTo")

// Defines for retrieving if it is notbook from 9X/Me registry
#define WIN_NOTEBOOK_KEY						_T( "ENUM\\VPOWERD\\BATTERY")

// Defines for retrieving if it is notbook from NT registry
#define NT_NOTEBOOK_KEY							_T( "SYSTEM\\CurrentControlSet\\Control\\Class\\{72631E54-78A4-11D0-BCF7-00AA00B7B32A}")

// Defines for retrieving installed apps from 9X/Me registry
#define WIN_APPS_KEY							_T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall")
#define WIN_APPS_VENDOR_VALUE					_T( "Publisher")
#define WIN_APPS_NAME_VALUE						_T( "DisplayName")
#define WIN_APPS_VERSION_VALUE					_T( "DisplayVersion")
#define WIN_APPS_FOLDER_VALUE					_T( "InstallLocation")
#define WIN_APPS_COMMENTS_VALUE					_T( "Comments")
#define WIN_APPS_UNINSTALL_VALUE				_T( "UninstallString")
#define WIN_APPS_QUIETUNINSTALL_VALUE			_T( "QuietUninstallString")
#define WIN_APPS_MODIFY_VALUE					_T( "ModifyPath")
#define WIN_APPS_LANGUAGE_VALUE					_T( "Language")

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

// Defines for validating detected components under HKEY_DYN_DATA for 9X/Me
#define WIN_CONFIG_MANAGER_KEY					_T( "Config Manager\\Enum")
#define WIN_CONFIG_MANAGER_COMPONENT_VALUE		_T( "HardWareKey")
#define WIN_CONFIG_MANAGER_STATUS_VALUE			_T( "Status")
#define WIN_CONFIG_MANAGER_STATUS_OK

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
	}
	else
	{
		// Failed so assume NT based, but not XP or later
		m_dwPlatformId = VER_PLATFORM_WIN32_NT;
		m_bIsXP = FALSE;
	}
	m_hKey = NULL;
	m_bRemoteRegistry = FALSE;
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

	csData.Empty();
	if ((hKey == NULL) || (lpstrValue == NULL))
		return ERROR_INVALID_HANDLE;

	// First call to get size of data
	lResult = RegQueryValueEx( hKey, lpstrValue, NULL, NULL, lpData, &dwSize);
	if ((lResult == ERROR_MORE_DATA) || (lResult == ERROR_SUCCESS))
	{
		// Now, allocate buffer to receive data
		if ((lpData = (LPBYTE) malloc ((dwSize+2)*sizeof( BYTE))) == NULL)
			return ERROR_NOT_ENOUGH_MEMORY;
		// Get data
		if ((lResult = RegQueryValueEx( hKey, lpstrValue, NULL, &dwType, lpData, &dwSize)) != ERROR_SUCCESS)
		{
			free( lpData);
			return ERROR_INVALID_DATA;
		}
		// Ensure string is null terminated
		lpData[dwSize]=0;
		lpData[dwSize+1]=0;
		switch (dwType)
		{
		case REG_DWORD:
			csData.Format( _T( "%I32u"), lpData);
			break;
		case REG_QWORD:
			csData.Format( _T( "%I64u"), lpData);
			break;
		case REG_SZ:
		case REG_EXPAND_SZ:
			csData.Format( _T( "%s"), lpData);
			break;
		case REG_MULTI_SZ:
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
	AddLog( _T( "Registry Connect: Trying to connect to HKEY_LOCAL_MACHINE on device <%s>..."), lpstrDevice == NULL ? _T( "Localhost") : lpstrDevice);
	if (lpstrDevice != NULL)
	{
		// Connect to remote registry
		csDevice.Format( _T( "\\\\%s"), lpstrDevice);
		if (RegConnectRegistry( csDevice, HKEY_LOCAL_MACHINE, &m_hKey) != ERROR_SUCCESS)
		{
			AddLog( _T( "Failed in call to <RegConnectRegistry> function for %s\\HKLM !\n"),
					csDevice);
			return FALSE;
		}
		m_bRemoteRegistry = TRUE;
		AddLog( _T( "OK.\n"));
		return TRUE;
	}
	// connect to local registry
	m_hKey = HKEY_LOCAL_MACHINE;
	AddLog( _T( "OK.\n"));
	return TRUE;
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
	case VER_PLATFORM_WIN32_WINDOWS:
		// Windows 9X/Me
		return GetBiosInfo9X( pMyBios);
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetBiosInfoNT( pMyBios);
	default:
		// Unknown
		pMyBios->Set( NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE
					  NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE);
		AddLog( _T( "Registry GetBiosInfo...Failed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::GetBiosInfo9X( CBios *pMyBios)
{
	HKEY	hKey = NULL;
	CString	csValue;
	LONG	lResult;
	BOOL	bManufacturer = FALSE,
			bModel = FALSE;

	AddLog( _T( "Registry 9X/Me GetBiosInfo...\n"));
	// Windows 9X/Me
	lResult = RegOpenKeyEx( m_hKey, WIN_BIOS_KEY, 0, KEY_READ, &hKey);
	if (lResult == ERROR_SUCCESS)
	{
		// Get System manufacturer
		lResult = GetValue( hKey, WIN_SYSTEM_MANUFACTURER_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					WIN_BIOS_KEY, WIN_SYSTEM_MANUFACTURER_VALUE);
			pMyBios->SetSystemManufacturer( NOT_AVAILABLE);
			bManufacturer = FALSE;;
		}
		else
		{
			pMyBios->SetSystemManufacturer( csValue);
			bManufacturer = TRUE;
		}
		// Get System model
		lResult = GetValue( hKey, WIN_SYSTEM_MODEL_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					WIN_BIOS_KEY, WIN_SYSTEM_MODEL_VALUE);
			pMyBios->SetSystemModel( NOT_AVAILABLE);
			bModel = FALSE;
		}
		else
		{
			pMyBios->SetSystemModel( csValue);
			bModel = TRUE;
		}
		// Get System serial number
		lResult = GetValue( hKey, WIN_SYSTEM_SERIAL_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					WIN_BIOS_KEY, WIN_SYSTEM_SERIAL_VALUE);
			pMyBios->SetSystemSerialNumber( NOT_AVAILABLE);
		}
		else
		{
			pMyBios->SetSystemSerialNumber( csValue);
		}
		// Get BIOS manufacturer
		lResult = GetValue( hKey, WIN_BIOS_MANUFACTURER_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					WIN_BIOS_KEY, WIN_BIOS_MANUFACTURER_VALUE);
			pMyBios->SetBiosManufacturer( NOT_AVAILABLE);
		}
		else
		{
			pMyBios->SetBiosManufacturer( csValue);
		}
		// Get BIOS version
		lResult = GetValue( hKey, WIN_BIOS_VERSION_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					WIN_BIOS_KEY, WIN_BIOS_VERSION_VALUE);
			pMyBios->SetBiosVersion( NOT_AVAILABLE);
		}
		else
		{
			pMyBios->SetBiosVersion( csValue);
		}
		// Get BIOS date
		lResult = GetValue( hKey, WIN_BIOS_DATE_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					WIN_BIOS_KEY, WIN_BIOS_DATE_VALUE);
			pMyBios->SetBiosDate( NOT_AVAILABLE);
		}
		else
		{
			pMyBios->SetBiosDate( csValue);
		}
		// Get machine type
		lResult = GetValue( hKey, WIN_BIOS_MACHINE_TYPE_VALUE, csValue);
		if (lResult != ERROR_SUCCESS)
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
					WIN_BIOS_KEY, WIN_BIOS_MACHINE_TYPE_VALUE);
			pMyBios->SetMachineType( NOT_AVAILABLE);
		}
		else
		{
			pMyBios->SetMachineType( csValue);
		}
		RegCloseKey( hKey);
	}
	else
	{
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				WIN_BIOS_KEY);
		pMyBios->Set( NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE
					  NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE);
	}
	if ((!bManufacturer) || (!bModel))
	{
		// Try to get System Manufacturer and Model from Windows Update Registry key
		lResult = RegOpenKeyEx( m_hKey, WIN_WU_KEY, 0, KEY_READ, &hKey);
		if (lResult != ERROR_SUCCESS)
		{
			// Get System manufacturer
			if (!bManufacturer)
			{
				lResult = GetValue( hKey, WIN_WU_SYSTEM_MANUFACTURER_VALUE, csValue);
				if (lResult != ERROR_SUCCESS)
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
							WIN_WU_KEY, WIN_WU_SYSTEM_MANUFACTURER_VALUE);
					pMyBios->SetSystemManufacturer( NOT_AVAILABLE);
					bManufacturer = FALSE;;
				}
				else
				{
					pMyBios->SetSystemManufacturer( csValue);
					if (csValue.CompareNoCase( _T( "System Manufacturer")) == 0)
						// Default WU value when not available
						bManufacturer = FALSE;
					else
						// Not default value
						bManufacturer = TRUE;
				}
			}
			// Get System model
			if (!bModel)
			{
				lResult = GetValue( hKey, WIN_WU_SYSTEM_MODEL_VALUE, csValue);
				if (lResult != ERROR_SUCCESS)
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
							WIN_WU_KEY, WIN_WU_SYSTEM_MODEL_VALUE);
					pMyBios->SetSystemModel( NOT_AVAILABLE);
					bModel = FALSE;
				}
				else
				{
					pMyBios->SetSystemModel( csValue);
					if (csValue.CompareNoCase( _T( "System Name")) == 0)
						// Default WU value when not available
						bModel = FALSE;
					else
						// Not default value
						bModel = TRUE;
				}
			}
			// Get ACPI manufacturer
			if (!bManufacturer)
			{
				lResult = GetValue( hKey, WIN_WU_ACPI_MANUFACTURER_VALUE, csValue);
				if (lResult != ERROR_SUCCESS)
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
							WIN_WU_KEY, WIN_WU_ACPI_MANUFACTURER_VALUE);
					pMyBios->SetSystemManufacturer( NOT_AVAILABLE);
					bManufacturer = FALSE;;
				}
				else
				{
					pMyBios->SetSystemManufacturer( csValue);
					bManufacturer = TRUE;
				}
			}
			// Get ACPI model
			if (!bModel)
			{
				lResult = GetValue( hKey, WIN_WU_ACPI_MODEL_VALUE, csValue);
				if (lResult != ERROR_SUCCESS)
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
							WIN_WU_KEY, WIN_WU_ACPI_MODEL_VALUE);
					pMyBios->SetSystemModel( NOT_AVAILABLE);
					bModel = FALSE;
				}
				else
				{
					pMyBios->SetSystemModel( csValue);
					bModel = TRUE;
				}
			}
			RegCloseKey( hKey);
		}
		else
		{
			AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
					WIN_WU_KEY);
		}
	}
	AddLog( _T( "Registry 9X/Me GetBiosInfo: OK.\n"));
	return TRUE;
}

BOOL CRegistry::GetBiosInfoNT( CBios *pMyBios)
{
	HKEY	hKey = NULL;
	CString	csValue;
	LONG	lResult;
	BOOL	bManufacturer = FALSE,
			bModel = FALSE;

	AddLog( _T( "Registry NT GetBiosInfo...\n"));
	// Windows NT
	lResult = RegOpenKeyEx( m_hKey, NT_BIOS_KEY, 0, KEY_READ, &hKey);
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
		RegCloseKey( hKey);
	}
	else
	{
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_BIOS_KEY);
		pMyBios->Set( NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE
					  NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE);
	}
	if ((!bManufacturer) || (!bModel))
	{
		// Try to get System Manufacturer and Model from Windows Update Registry key
		lResult = RegOpenKeyEx( m_hKey, NT_WU_KEY, 0, KEY_READ, &hKey);
		if (lResult == ERROR_SUCCESS)
		{
			// Get System manufacturer
			if (!bManufacturer)
			{
				lResult = GetValue( hKey, NT_WU_SYSTEM_MANUFACTURER_VALUE, csValue);
				if (lResult != ERROR_SUCCESS)
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
							NT_WU_KEY, NT_WU_SYSTEM_MANUFACTURER_VALUE);
					pMyBios->SetSystemManufacturer( NOT_AVAILABLE);
					bManufacturer = FALSE;
				}
				else
				{
					pMyBios->SetSystemManufacturer( csValue);
					if (csValue.CompareNoCase( _T( "System Manufacturer")) == 0)
						// Default WU value when not available
						bManufacturer = FALSE;
					else
						// Not default value
						bManufacturer = TRUE;
				}
			}
			// Get System model
			if (!bModel)
			{
				lResult = GetValue( hKey, NT_WU_SYSTEM_MODEL_VALUE, csValue);
				if (lResult != ERROR_SUCCESS)
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
							NT_WU_KEY, NT_WU_SYSTEM_MODEL_VALUE);
					pMyBios->SetSystemModel( NOT_AVAILABLE);
					bModel = FALSE;
				}
				else
				{
					pMyBios->SetSystemModel( csValue);
					if (csValue.CompareNoCase( _T( "System Name")) == 0)
						// Default WU value when not available
						bModel = FALSE;
					else
						// Not default value
						bModel = TRUE;
				}
			}
			// Get ACPI manufacturer
			if (!bManufacturer)
			{
				lResult = GetValue( hKey, NT_WU_ACPI_MANUFACTURER_VALUE, csValue);
				if (lResult != ERROR_SUCCESS)
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
							NT_WU_KEY, NT_WU_ACPI_MANUFACTURER_VALUE);
					pMyBios->SetSystemManufacturer( NOT_AVAILABLE);
					bManufacturer = FALSE;
				}
				else
				{
					pMyBios->SetSystemManufacturer( csValue);
					bManufacturer = TRUE;
				}
			}
			// Get ACPI model
			if (!bModel)
			{
				lResult = GetValue( hKey, NT_WU_ACPI_MODEL_VALUE, csValue);
				if (lResult != ERROR_SUCCESS)
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
							NT_WU_KEY, NT_WU_ACPI_MODEL_VALUE);
					pMyBios->SetSystemModel( NOT_AVAILABLE);
					bModel = FALSE;
				}
				else
				{
					pMyBios->SetSystemModel( csValue);
					bModel = TRUE;
				}
			}
			RegCloseKey( hKey);
		}
		else
		{
			AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
					NT_WU_KEY);
		}
	}
	AddLog( _T( "Registry NT GetBiosInfo: OK.\n"));
	return TRUE;
}

BOOL CRegistry::IsNotebook()
{
	// Try to get Battery Informations info from the registry
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_WINDOWS:
		// Windows 9X/Me
		return IsNotebook9X();
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return IsNotebookNT();
	default:
		// Unknown
		AddLog( _T( "Registry IsNotebook...Failed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::IsNotebook9X()
{
	HKEY			hKeyEnum;
	TCHAR			szDeviceName[256];
	DWORD			dwLength,
					dwIndexEnum = 0;
	LONG			lResult;
	FILETIME		MyFileTime;

	AddLog( _T( "Registry 9X/Me IsNotebook...\n"));
	// Windows 9X/Me => Open the battery key
	if (RegOpenKeyEx( m_hKey, WIN_NOTEBOOK_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
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
			AddLog( _T( "Registry 9X/Me IsNotebook: Failed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					WIN_NOTEBOOK_KEY);
		else
			AddLog( _T( "Registry 9X/Me IsNotebook: OK (%lu objects).\n"), dwIndexEnum);
	}
	else
		AddLog( _T( "Registry 9X/Me IsNotebook: Failed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				WIN_NOTEBOOK_KEY);
	return (dwIndexEnum > 0);
}

BOOL CRegistry::IsNotebookNT()
{
	HKEY			hKeyEnum;
	TCHAR			szDeviceName[256];
	DWORD			dwLength,
					dwIndexEnum = 0;
	LONG			lResult;
	FILETIME		MyFileTime;

	AddLog( _T( "Registry NT IsNotebook...\n"));
	// Windows NT => Open the battery key
	if (RegOpenKeyEx( m_hKey, NT_NOTEBOOK_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
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
			AddLog( _T( "Registry NT IsNotebook: Failed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_NOTEBOOK_KEY);
		else
			AddLog( _T( "Registry NT IsNotebook: OK (%lu objects).\n"), dwIndexEnum);
	}
	else
		AddLog( _T( "Registry NT IsNotebook: Failed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_NOTEBOOK_KEY);
	return (dwIndexEnum > 0);
}

DWORD CRegistry::GetProcessors(CString &csProcType, CString &csProcSpeed)
{
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_WINDOWS:
		// Windows 9X/Me
		return GetProcessors9X( csProcType, csProcSpeed);
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetProcessorsNT( csProcType, csProcSpeed);
	default:
		// Unknown
		csProcType = NOT_AVAILABLE;
		csProcSpeed = NOT_AVAILABLE;
		AddLog( _T( "Registry GetProcessors...Failed because unsupported or unrecognized OS !\n"));
		return 0;
	}
	return 0;
}

DWORD CRegistry::GetProcessors9X(CString &csProcType, CString &csProcSpeed)
{
	HKEY			hKeyEnum,
					hKeyObject;
	CString			csSubKey,
					csBuffer;
	TCHAR			szDeviceName[256];
	DWORD			dwLength,
					dwIndexEnum = 0,
					dwIndex = 0;
	LONG			lResult;
	FILETIME		MyFileTime;
	BOOL			bHaveToStore;

	AddLog( _T( "Registry 9X/Me GetProcessors...\n"));
	// Windows 9X => Open the processor key
	if (RegOpenKeyEx( m_hKey, WIN_PROCESSOR_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szDeviceName, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szDeviceName[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), WIN_PROCESSOR_KEY, szDeviceName);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
			{
				csProcType = NOT_AVAILABLE; 
				csProcSpeed = NOT_AVAILABLE;
				// Read the processor name
				if (GetValue( hKeyObject, WIN_PROCESSOR_NAME_VALUE, csProcType) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_PROCESSOR_NAME_VALUE);
					csProcType = NOT_AVAILABLE;
				}
				// Read the speed in MHz
				if (GetValue( hKeyObject, WIN_PROCESSOR_SPEED_VALUE, csProcSpeed) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_PROCESSOR_SPEED_VALUE);
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
			AddLog( _T( "Registry 9X/Me GetProcessors: Failed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					WIN_PROCESSOR_KEY);
		else
			AddLog( _T( "Registry 9X/Me GetProcessors: OK (%s %s MHz, %lu objects).\n"),
					csProcType, csProcSpeed, dwIndex);
	}
	else
		AddLog( _T( "Registry 9X/Me GetProcessors: Failed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				WIN_PROCESSOR_KEY);
	AddLog( _T( "Registry 9X/Me GetProcessors: %lu processor(s) found.\n"), dwIndex);
	return dwIndex;
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

	AddLog( _T( "Registry NT GetProcessors...\n"));
	// Windows NT => Open the processor key
	if (RegOpenKeyEx( m_hKey, NT_PROCESSOR_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szDeviceName, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szDeviceName[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_PROCESSOR_KEY, szDeviceName);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
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
			AddLog( _T( "Registry NT GetProcessors: Failed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_PROCESSOR_KEY);
		else
			AddLog( _T( "Registry NT GetProcessors: OK (%s %s MHz, %lu objects).\n"),
					csProcType, csProcSpeed, dwIndex);
	}
	else
		AddLog( _T( "Registry NT GetProcessors: Failed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_PROCESSOR_KEY);
	AddLog( _T( "Registry NT GetProcessors: %lu processor(s) found.\n"), dwIndex);
	return dwIndex;
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

	AddLog( _T( "Registry GetPrinters...\n"));
	// Reset object list content
	while (!(pList->GetCount() == 0))
		pList->RemoveHead();
	// Open the Printer key
	if (RegOpenKeyEx( m_hKey, PRINTERS_ENUM_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
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
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyDevice) == ERROR_SUCCESS)
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
			AddLog( _T( "Registry GetPrinters: Failed in call to <RegEnumKeyEx> function to find Printer subkey.\n"));
		else
			AddLog( _T( "Registry GetPrinters: OK (%u objects).\n"), uIndex);
	}
	else
		AddLog( _T( "Registry GetPrinters: Failed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"), PRINTERS_ENUM_KEY);
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
	case VER_PLATFORM_WIN32_WINDOWS:
		// Windows 9X/Me
		return GetVideoAdapters9X( pList);
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		if (m_bIsXP)
			return GetVideoAdaptersXP( pList);
		return GetVideoAdaptersNT_2K( pList);
	default:
		// Unknown
		AddLog( _T( "Registry GetVideoAdapters...Failed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::GetVideoAdapters9X(CVideoAdapterList *pList)
{
	HKEY			hKeyEnum,
					hKeyObject,
					hKeyProperty;
	CString			csSubKey,
					csAdapterName,
					csAdapterChip,
					csResolution;
	TCHAR			szDeviceName[256];
	DWORD			dwLength,
					dwMemory,
					dwIndexEnum = 0;
	LONG			lResult,
					lAdapterMemory;
	FILETIME		MyFileTime;
	CVideoAdapter	myObject;
	BOOL			bHaveToStore;
	UINT			uIndex = 0;

	ASSERT( pList);

	AddLog( _T( "Registry 9X/Me GetVideoAdapters...\n"));
	// Windows 9X => Open the Display key
	if (RegOpenKeyEx( m_hKey, WIN_DISPLAY_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices (have an INFO value)
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szDeviceName, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szDeviceName[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), WIN_DISPLAY_KEY, szDeviceName);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
			{
				csAdapterName = NOT_AVAILABLE;
				csAdapterChip = NOT_AVAILABLE;
				lAdapterMemory = -1;
				// OK => Read the Graphic Adapter description
				if (GetValue( hKeyObject, WIN_ADAPTER_NAME_VALUE, csAdapterName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_ADAPTER_NAME_VALUE);
					csAdapterName = NOT_AVAILABLE;
				}
				// Open the INFO key from Display Device
				csSubKey.Format( _T( "%s\\%s\\%s"), WIN_DISPLAY_KEY, szDeviceName, WIN_DEVICE_INFO_KEY);
				if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyProperty) == ERROR_SUCCESS)
				{
					// Read the Graphic Adapter Chipset
					if (GetValue( hKeyProperty, WIN_ADAPTER_CHIP_VALUE, csAdapterChip) == ERROR_SUCCESS)
					{
						bHaveToStore = TRUE;
					}
					else
					{
						AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
										   csSubKey, WIN_ADAPTER_CHIP_VALUE);
						csAdapterChip = NOT_AVAILABLE;
					}
					// Read the Graphic Adapter Memory size
					if (GetValue( hKeyProperty, WIN_ADAPTER_MEMORY_VALUE, &dwMemory) == ERROR_SUCCESS)
						lAdapterMemory = dwMemory/ONE_MEGABYTE;
					else
					{
						AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
										   csSubKey, WIN_ADAPTER_MEMORY_VALUE);
						lAdapterMemory = -1;
					}
					RegCloseKey( hKeyProperty);
				}
				else
					AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
									   csSubKey);
				RegCloseKey( hKeyObject);
				// Get screen resolution
				if (!GetScreenResolution( csResolution))
					AddLog( _T( "\tFailed in call to <GetSystemMetrics> function for SM_CXSCREEN and SM_CYSCREEN !\n"));
				myObject.SetName( csAdapterName);
				myObject.SetChipset( csAdapterChip);
				myObject.SetMemory( lAdapterMemory);
				myObject.SetScreenResolution( csResolution);
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
			AddLog( _T( "Registry 9X/Me GetVideoAdapters: Failed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					WIN_DISPLAY_KEY);
		else
			AddLog( _T( "Registry 9X/Me GetVideoAdapters: OK (%u objects).\n"), uIndex);
	}
	else
		AddLog( _T( "Registry 9X/Me GetVideoAdapters: Failed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				WIN_DISPLAY_KEY);
	return !pList->IsEmpty();
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

	AddLog( _T( "Registry NT GetVideoAdapters...\n"));
	// Windows NT =>  Browse the active service keys to find the Display service
	// Enumerate service groups under HKLM\System\CurrentControlSet\Enum
	if (RegOpenKeyEx( m_hKey, NT_ENUM_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szGroupName, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each group, enumerate service keys
			szGroupName[dwLength] = 0;
			csSubKey.Format( _T( "%s\\%s"), NT_ENUM_KEY, szGroupName);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyGroup) == ERROR_SUCCESS)
			{
				dwLength = 255;
				dwIndexGroup = 0;
				while ((lResult = RegEnumKeyEx( hKeyGroup, dwIndexGroup, szDeviceName, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
				{
					// For each service, enumerate propertie keys
					szDeviceName[dwLength] = 0;
					csSubKey.Format( _T( "%s\\%s\\%s"), NT_ENUM_KEY, szGroupName, szDeviceName);
					if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
					{
						dwLength = 255;
						dwIndexProperties = 0;
						while ((lResult = RegEnumKeyEx( hKeyObject, dwIndexProperties, szPropertyName, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
						{
							// If a display class key, read the associated service
							szPropertyName[dwLength] = 0;
							csSubKey.Format( _T( "%s\\%s\\%s\\%s"), NT_ENUM_KEY, szGroupName, szDeviceName, szPropertyName);
							if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyProperty) == ERROR_SUCCESS)
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
								if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyProperty) == ERROR_SUCCESS)
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
			AddLog( _T( "Registry NT GetVideoAdapters: Failed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
				    NT_ENUM_KEY);
		else
			AddLog( _T( "Registry NT GetVideoAdapters: OK (%u objects).\n"), uIndex);
	} // if RegOpenKey enum Key
	else
		AddLog( _T( "Registry NT GetVideoAdapters: Failed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
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

	AddLog( _T( "Registry XP GetVideoAdapters...\n"));
	// Windows XP =>  Browse the active service keys to find the Display service
	// Enumerate device groups under HKLM\System\CurrentControlSet\Control\Video
	if (RegOpenKeyEx( m_hKey, XP_ENUM_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szGroupName, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each group, enumerate device keys
			szGroupName[dwLength] = 0;
			csSubKey.Format( _T( "%s\\%s"), XP_ENUM_KEY, szGroupName);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyGroup) == ERROR_SUCCESS)
			{
				dwLength = 255;
				dwIndexGroup = 0;
				while ((lResult = RegEnumKeyEx( hKeyGroup, dwIndexGroup, szDeviceName, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
				{
					// For each device, get propertie keys
					szDeviceName[dwLength] = 0;
					csSubKey.Format( _T( "%s\\%s\\%s"), XP_ENUM_KEY, szGroupName, szDeviceName);
					if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyProperty) == ERROR_SUCCESS)
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
			AddLog( _T( "Registry XP GetVideoAdapters: Failed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
				   NT_ENUM_KEY);
			return FALSE;
		}
		AddLog( _T( "Registry XP GetVideoAdapters: OK (%u objects).\n"), uIndex);
		return TRUE;
	} // if RegOpenKey enum Key
	AddLog( _T( "Registry XP GetVideoAdapters: Failed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"), NT_ENUM_KEY);
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
	case VER_PLATFORM_WIN32_WINDOWS:
		// Windows 9X/Me
		return GetSoundDevices9X( pList);
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetSoundDevicesNT( pList);
	default:
		// Unknown
		AddLog( _T( "Registry GetSoundDevices...Failed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::GetSoundDevices9X(CSoundDeviceList *pList)
{
	HKEY			hKeyEnum,
					hKeyObject,
					hKeyDriver;
	CString			csSubKey,
					csDeviceName,
					csProviderName,
					csDescription;
	TCHAR			szKey[256];
	DWORD			dwLength,
					dwIndexEnum = 0;
	LONG			lResult;
	FILETIME		MyFileTime;
	CSoundDevice	myObject;
	BOOL			bHaveToStore;
	UINT			uIndex = 0;

	ASSERT( pList);

	AddLog( _T( "Registry 9X/Me GetSoundDevices...\n"));
	// Windows 9X => Open the sound key
	if (RegOpenKeyEx( m_hKey, WIN_SOUND_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), WIN_SOUND_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
			{
				csProviderName = NOT_AVAILABLE;
				csDeviceName = NOT_AVAILABLE; 
				csDescription = NOT_AVAILABLE; 
				// OK => Read the provider
				if (GetValue( hKeyObject, WIN_SOUND_MANUFACTURER_VALUE, csProviderName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_SOUND_MANUFACTURER_VALUE);
					csProviderName = NOT_AVAILABLE;
				}
				// Read the name
				if (GetValue( hKeyObject, WIN_SOUND_NAME_VALUE, csDeviceName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_SOUND_NAME_VALUE);
					csDeviceName = NOT_AVAILABLE; 
				}
				// Read the description
				if (GetValue( hKeyObject, WIN_SOUND_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_SOUND_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				RegCloseKey( hKeyObject);
				myObject.SetManufacturer( csProviderName);
				myObject.SetName( csDeviceName);
				myObject.SetDescription( csDescription);
				// Check if there is a Drivers sub key to validate
				csSubKey += _T( "\\");
				csSubKey += WIN_SOUND_DRIVER;
				if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyDriver) == ERROR_SUCCESS)
				{
					// OK => Try to read the SubClasses value
					if (GetValue( hKeyDriver, WIN_SOUND_DRIVER_CLASS, csProviderName) != ERROR_SUCCESS)
						// No value => skip
						bHaveToStore = FALSE;
					RegCloseKey( hKeyDriver);
				}
				else
					// No Driver sub key => skip
					bHaveToStore = FALSE;
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
			AddLog( _T( "Registry 9X/Me GetSoundDevices: Failed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					WIN_SOUND_KEY);
		else
			AddLog( _T( "Registry 9X/Me GetSoundDevices: OK (%u objects).\n"), uIndex);
	}
	else
		AddLog( _T( "Registry 9X/Me GetSoundDevices: Failed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				WIN_SOUND_KEY);
	return !pList->IsEmpty();
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

	AddLog( _T( "Registry NT GetSoundDevices...\n"));
	// Windows NT => Open the sound key
	if (RegOpenKeyEx( m_hKey, NT_SOUND_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_SOUND_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
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
				if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyDriver) == ERROR_SUCCESS)
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
			AddLog( _T( "Registry NT GetSoundDevices: Failed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_SOUND_KEY);
		else
			AddLog( _T( "Registry NT GetSoundDevices: OK (%u objects).\n"), uIndex);
	}
	else
		AddLog( _T( "Registry NT GetSoundDevices: Failed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
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
	case VER_PLATFORM_WIN32_WINDOWS:
		// Windows 9X/Me
		return GetModems9X( pList);
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetModemsNT( pList);
	default:
		// Unknown
		AddLog( _T( "Registry GetModems...Failed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::GetModems9X(CModemList *pList)
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

	AddLog( _T( "Registry 9X/Me GetModems...\n"));
	// Windows 9X => Open the modem key
	if (RegOpenKeyEx( m_hKey, WIN_MODEM_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), WIN_MODEM_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
			{
				csType = NOT_AVAILABLE; 
				csDeviceName = NOT_AVAILABLE; 
				csModel = NOT_AVAILABLE;
				csDescription = NOT_AVAILABLE; 
				// Read the type, i.e the COM port attached to
				if (GetValue( hKeyObject, WIN_MODEM_TYPE_VALUE, csType) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_MODEM_TYPE_VALUE);
					csType = NOT_AVAILABLE; 
				}
				// Read the name
				if (GetValue( hKeyObject, WIN_MODEM_NAME_VALUE, csDeviceName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_MODEM_NAME_VALUE);
					csDeviceName = NOT_AVAILABLE; 
				}
				// Read the model
				if (GetValue( hKeyObject, WIN_MODEM_MODEL_VALUE, csModel) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_MODEM_MODEL_VALUE);
					csModel = NOT_AVAILABLE;
				}
				// Read the description
				if (GetValue( hKeyObject, WIN_MODEM_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_MODEM_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				RegCloseKey( hKeyObject);
				myObject.SetType( csType);
				myObject.SetName( csDeviceName);
				myObject.SetModel( csModel);
				myObject.SetDescription( csDescription);
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
			AddLog( _T( "Registry 9X/Me GetModems: Failed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					WIN_MODEM_KEY);
		else
			AddLog( _T( "Registry 9X/Me GetModems: OK (%u objects).\n"), uIndex);
	}
	else
		AddLog( _T( "Registry 9X/Me GetModems: Failed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				WIN_MODEM_KEY);
	return !pList->IsEmpty();
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

	AddLog( _T( "Registry NT GetModems...\n"));
	// Windows NT => Open the modem key
	if (RegOpenKeyEx( m_hKey, NT_MODEM_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_MODEM_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
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
			AddLog( _T( "Registry NT GetModems: Failed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_MODEM_KEY);
		else
			AddLog( _T( "Registry NT GetModems: OK (%u objects).\n"), uIndex);
	}
	else
		AddLog( _T( "Registry NT GetModems: Failed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
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
	case VER_PLATFORM_WIN32_WINDOWS:
		// Windows 9X/Me
		return GetMonitors9X( pList);
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetMonitorsNT( pList);
	default:
		// Unknown
		AddLog( _T( "Registry GetMonitors...Failed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::GetMonitors9X(CMonitorList *pList)
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

	AddLog( _T( "Registry 9X/Me GetMonitors...\n"));
	// Windows 9X => Open the monitor key
	if (RegOpenKeyEx( m_hKey, WIN_MONITOR_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), WIN_MONITOR_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
			{
				csManufacturer = NOT_AVAILABLE; 
				csModel = NOT_AVAILABLE;
				csCaption = NOT_AVAILABLE; 
				csDescription = NOT_AVAILABLE; 
				// Read the manufactuer
				if (GetValue( hKeyObject, WIN_MONITOR_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_MONITOR_MANUFACTURER_VALUE);
					csManufacturer = NOT_AVAILABLE; 
				}
				// Read the model
				if (GetValue( hKeyObject, WIN_MONITOR_TYPE_VALUE, csModel) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_MONITOR_TYPE_VALUE);
					csModel = NOT_AVAILABLE;
				}
				// Read the caption
				if (GetValue( hKeyObject, WIN_MONITOR_CAPTION_VALUE, csCaption) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_MONITOR_CAPTION_VALUE);
					csCaption = NOT_AVAILABLE; 
				}
				// Read the description
				if (GetValue( hKeyObject, WIN_MONITOR_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_MONITOR_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				RegCloseKey( hKeyObject);
				myObject.SetManufacturer( csManufacturer);
				myObject.SetType( csModel);
				myObject.SetCaption( csCaption);
				myObject.SetDescription( csDescription);
				// Add the device to the adapter list
				// ValidateComponent9X( csSubKey);
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
			AddLog( _T( "Registry 9X/Me GetMonitors: Failed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					WIN_MONITOR_KEY);
		else
			AddLog( _T( "Registry 9X/Me GetMonitors: OK (%u objects).\n"), uIndex);
	}
	else
		AddLog( _T( "Registry 9X/Me GetMonitors: Failed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				WIN_MONITOR_KEY);
	return !pList->IsEmpty();
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

	AddLog( _T( "Registry NT GetMonitors...\n"));
	// Windows 9X => Open the monitor key
	if (RegOpenKeyEx( m_hKey, NT_MONITOR_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_MONITOR_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
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
			AddLog( _T( "Registry NT GetMonitors: Failed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_MONITOR_KEY);
		else
			AddLog( _T( "Registry NT GetMonitors: OK (%u objects).\n"), uIndex);
	}
	else
		AddLog( _T( "Registry NT GetMonitors: Failed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
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
	case VER_PLATFORM_WIN32_WINDOWS:
		// Windows 9X/Me
		return GetSystemControllers9X( pList);
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetSystemControllersNT( pList);
	default:
		// Unknown
		AddLog( _T( "Registry GetSystemControllers...Failed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::GetSystemControllers9X(CSystemControllerList *pList)
{
	HKEY			hKeyEnum,
					hKeyObject;
	CString			csSubKey,
					csManufacturer,
					csName,
					csCaption,
					csDescription,
					csVersion;
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

	AddLog( _T( "Registry 9X/Me GetSystemControllers: Trying to find Floppy Controllers in HKLM\\%s...\n"),
			WIN_CONTROLER_FLOPPY_KEY);
	// Windows 9X => Open the floppy controler key
	if (RegOpenKeyEx( m_hKey, WIN_CONTROLER_FLOPPY_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), WIN_CONTROLER_FLOPPY_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
			{
				csManufacturer = NOT_AVAILABLE; 
				csName = NOT_AVAILABLE;
				csCaption = NOT_AVAILABLE; 
				csDescription = NOT_AVAILABLE; 
				csVersion = NOT_AVAILABLE;
				// Read the manufactuer
				if (GetValue( hKeyObject, WIN_CONTROLER_FLOPPY_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_FLOPPY_MANUFACTURER_VALUE);
					csManufacturer = NOT_AVAILABLE;
				}
				// Read the name
				if (GetValue( hKeyObject, WIN_CONTROLER_FLOPPY_NAME_VALUE, csName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_FLOPPY_NAME_VALUE);
					csName = NOT_AVAILABLE;
				}
				// Read the caption
				if (GetValue( hKeyObject, WIN_CONTROLER_FLOPPY_CAPTION_VALUE, csCaption) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_FLOPPY_CAPTION_VALUE);
					csCaption = NOT_AVAILABLE; 
				}
				// Read the description
				if (GetValue( hKeyObject, WIN_CONTROLER_FLOPPY_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_FLOPPY_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				// Read the verion
				if (GetValue( hKeyObject, WIN_CONTROLER_FLOPPY_VERSION_VALUE, csVersion) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_FLOPPY_VERSION_VALUE);
					csVersion = NOT_AVAILABLE;
				}
				RegCloseKey( hKeyObject);
				myObject.SetType( SYSTEM_CONTROLER_FLOPPY);
				myObject.SetManufacturer( csManufacturer);
				myObject.SetName( csName);
				myObject.SetHardwareVersion( csVersion);
				myObject.SetCaption( csCaption);
				myObject.SetDescription( csDescription);
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
					WIN_CONTROLER_FLOPPY_KEY);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				WIN_CONTROLER_FLOPPY_KEY);
	if (uIndex > 0)
	{
		uTotalIndex += uIndex;
		AddLog( _T( "Registry 9X/Me GetSystemControllers: Find Floppy Controllers OK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "Registry 9X/Me GetSystemControllers: Find Floppy Controllers failed because no valid object !\n"));
	// Windows 9X => Open the IDE controler key
	uIndex = 0;
	dwIndexEnum = 0;
	AddLog( _T( "Registry 9X/Me GetSystemControllers: Trying to find IDE Controllers in HKLM\\%s...\n"),
			WIN_CONTROLER_IDE_KEY);
	if (RegOpenKeyEx( m_hKey, WIN_CONTROLER_IDE_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), WIN_CONTROLER_IDE_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
			{
				csManufacturer = NOT_AVAILABLE; 
				csName = NOT_AVAILABLE;
				csCaption = NOT_AVAILABLE; 
				csDescription = NOT_AVAILABLE; 
				csVersion = NOT_AVAILABLE;
				// Read the manufactuer
				if (GetValue( hKeyObject, WIN_CONTROLER_IDE_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_IDE_MANUFACTURER_VALUE);
					csManufacturer = NOT_AVAILABLE; 
				}
				// Read the name
				if (GetValue( hKeyObject, WIN_CONTROLER_IDE_NAME_VALUE, csName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_IDE_NAME_VALUE);
					csName = NOT_AVAILABLE;
				}
				// Read the caption
				if (GetValue( hKeyObject, WIN_CONTROLER_IDE_CAPTION_VALUE, csCaption) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_IDE_CAPTION_VALUE);
					csCaption = NOT_AVAILABLE; 
				}
				// Read the description
				if (GetValue( hKeyObject, WIN_CONTROLER_IDE_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_IDE_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				// Read the verion
				if (GetValue( hKeyObject, WIN_CONTROLER_IDE_VERSION_VALUE, csVersion) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_IDE_VERSION_VALUE);
					csVersion = NOT_AVAILABLE;
				}
				RegCloseKey( hKeyObject);
				myObject.SetType( SYSTEM_CONTROLER_IDE);
				myObject.SetManufacturer( csManufacturer);
				myObject.SetName( csName);
				myObject.SetHardwareVersion( csVersion);
				myObject.SetCaption( csCaption);
				myObject.SetDescription( csDescription);
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
					WIN_CONTROLER_IDE_KEY);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				WIN_CONTROLER_IDE_KEY);
	if (uIndex > 0)
	{
		uTotalIndex += uIndex;
		AddLog( _T( "Registry 9X/Me GetSystemControllers: Find IDE Controllers OK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "Registry 9X/Me GetSystemControllers: Find IDE Controllers failed because no valid object !\n"));
	// Windows 9X => Open the SCSI controler key
	uIndex = 0;
	dwIndexEnum = 0;
	AddLog( _T( "Registry 9X/Me GetSystemControllers: Trying to find SCSI Controllers in HKLM\\%s...\n"),
			WIN_CONTROLER_SCSI_KEY);
	if (RegOpenKeyEx( m_hKey, WIN_CONTROLER_SCSI_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), WIN_CONTROLER_SCSI_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
			{
				csManufacturer = NOT_AVAILABLE; 
				csName = NOT_AVAILABLE;
				csCaption = NOT_AVAILABLE; 
				csDescription = NOT_AVAILABLE; 
				csVersion = NOT_AVAILABLE;
				// Read the manufactuer
				if (GetValue( hKeyObject, WIN_CONTROLER_SCSI_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_SCSI_MANUFACTURER_VALUE);
					csManufacturer = NOT_AVAILABLE; 
				}
				// Read the name
				if (GetValue( hKeyObject, WIN_CONTROLER_SCSI_NAME_VALUE, csName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_SCSI_NAME_VALUE);
					csName = NOT_AVAILABLE;
				}
				// Read the caption
				if (GetValue( hKeyObject, WIN_CONTROLER_SCSI_CAPTION_VALUE, csCaption) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_SCSI_CAPTION_VALUE);
					csCaption = NOT_AVAILABLE; 
				}
				// Read the description
				if (GetValue( hKeyObject, WIN_CONTROLER_SCSI_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_SCSI_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				// Read the verion
				if (GetValue( hKeyObject, WIN_CONTROLER_SCSI_VERSION_VALUE, csVersion) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_SCSI_VERSION_VALUE);
					csVersion = NOT_AVAILABLE;
				}
				RegCloseKey( hKeyObject);
				myObject.SetType( SYSTEM_CONTROLER_SCSI);
				myObject.SetManufacturer( csManufacturer);
				myObject.SetName( csName);
				myObject.SetHardwareVersion( csVersion);
				myObject.SetCaption( csCaption);
				myObject.SetDescription( csDescription);
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
					WIN_CONTROLER_SCSI_KEY);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				WIN_CONTROLER_SCSI_KEY);
	if (uIndex > 0)
	{
		uTotalIndex += uIndex;
		AddLog( _T( "Registry 9X/Me GetSystemControllers: Find SCSI Controllers OK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "Registry 9X/Me GetSystemControllers: Find SCSI Controllers failed because no valid object !\n"));
	// Windows 9X => Open the INFRARED controler key
	uIndex = 0;
	dwIndexEnum = 0;
	AddLog( _T( "Registry 9X/Me GetSystemControllers: Trying to find InfraRed Controllers in HKLM\\%s...\n"),
			WIN_CONTROLER_INFRARED_KEY);
	if (RegOpenKeyEx( m_hKey, WIN_CONTROLER_INFRARED_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), WIN_CONTROLER_INFRARED_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
			{
				csManufacturer = NOT_AVAILABLE; 
				csName = NOT_AVAILABLE;
				csCaption = NOT_AVAILABLE; 
				csDescription = NOT_AVAILABLE; 
				csVersion = NOT_AVAILABLE;
				// Read the manufactuer
				if (GetValue( hKeyObject, WIN_CONTROLER_INFRARED_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_INFRARED_MANUFACTURER_VALUE);
					csManufacturer = NOT_AVAILABLE; 
				}
				// Read the name
				if (GetValue( hKeyObject, WIN_CONTROLER_INFRARED_NAME_VALUE, csName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_INFRARED_NAME_VALUE);
					csName = NOT_AVAILABLE;
				}
				// Read the caption
				if (GetValue( hKeyObject, WIN_CONTROLER_INFRARED_CAPTION_VALUE, csCaption) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_INFRARED_CAPTION_VALUE);
					csCaption = NOT_AVAILABLE; 
				}
				// Read the description
				if (GetValue( hKeyObject, WIN_CONTROLER_INFRARED_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_INFRARED_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				// Read the verion
				if (GetValue( hKeyObject, WIN_CONTROLER_INFRARED_VERSION_VALUE, csVersion) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_INFRARED_VERSION_VALUE);
					csVersion = NOT_AVAILABLE;
				}
				RegCloseKey( hKeyObject);
				myObject.SetType( SYSTEM_CONTROLER_INFRARED);
				myObject.SetManufacturer( csManufacturer);
				myObject.SetName( csName);
				myObject.SetHardwareVersion( csVersion);
				myObject.SetCaption( csCaption);
				myObject.SetDescription( csDescription);
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
					WIN_CONTROLER_INFRARED_KEY);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				WIN_CONTROLER_INFRARED_KEY);
	if (uIndex > 0)
	{
		uTotalIndex += uIndex;
		AddLog( _T( "Registry 9X/Me GetSystemControllers: Find InfraRed Controllers OK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "Registry 9X/Me GetSystemControllers: Find InfraRed Controllers failed because no valid object !\n"));
	// Windows 9X => Open the USB controler key
	uIndex = 0;
	dwIndexEnum = 0;
	AddLog( _T( "Registry 9X/Me GetSystemControllers: Trying to find USB Controllers in HKLM\\%s...\n"),
			WIN_CONTROLER_USB_KEY);
	if (RegOpenKeyEx( m_hKey, WIN_CONTROLER_USB_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), WIN_CONTROLER_USB_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
			{
				csManufacturer = NOT_AVAILABLE; 
				csName = NOT_AVAILABLE;
				csCaption = NOT_AVAILABLE; 
				csDescription = NOT_AVAILABLE; 
				csVersion = NOT_AVAILABLE;
				// Read the manufactuer
				if (GetValue( hKeyObject, WIN_CONTROLER_USB_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_USB_MANUFACTURER_VALUE);
					csManufacturer = NOT_AVAILABLE; 
				}
				// Read the name
				if (GetValue( hKeyObject, WIN_CONTROLER_USB_NAME_VALUE, csName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_USB_NAME_VALUE);
					csName = NOT_AVAILABLE;
				}
				// Read the caption
				if (GetValue( hKeyObject, WIN_CONTROLER_USB_CAPTION_VALUE, csCaption) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_USB_CAPTION_VALUE);
					csCaption = NOT_AVAILABLE; 
				}
				// Read the description
				if (GetValue( hKeyObject, WIN_CONTROLER_USB_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_USB_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				// Read the verion
				if (GetValue( hKeyObject, WIN_CONTROLER_USB_VERSION_VALUE, csVersion) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_USB_VERSION_VALUE);
					csVersion = NOT_AVAILABLE;
				}
				RegCloseKey( hKeyObject);
				myObject.SetType( SYSTEM_CONTROLER_USB);
				myObject.SetManufacturer( csManufacturer);
				myObject.SetName( csName);
				myObject.SetHardwareVersion( csVersion);
				myObject.SetCaption( csCaption);
				myObject.SetDescription( csDescription);
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
					WIN_CONTROLER_USB_KEY);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				WIN_CONTROLER_USB_KEY);
	if (uIndex > 0)
	{
		uTotalIndex += uIndex;
		AddLog( _T( "Registry 9X/Me GetSystemControllers: Find USB Controllers OK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "Registry 9X/Me GetSystemControllers: Find USB Controllers failed because no valid object !\n"));
	// Windows 9X => Open the IEEE1394 controler key
	uIndex = 0;
	dwIndexEnum = 0;
	AddLog( _T( "Registry 9X/Me GetSystemControllers: Trying to find IEEE1394 Controllers in HKLM\\%s...\n"),
			WIN_CONTROLER_IEEE1394_KEY);
	if (RegOpenKeyEx( m_hKey, WIN_CONTROLER_IEEE1394_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), WIN_CONTROLER_IEEE1394_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
			{
				csManufacturer = NOT_AVAILABLE; 
				csName = NOT_AVAILABLE;
				csCaption = NOT_AVAILABLE; 
				csDescription = NOT_AVAILABLE; 
				csVersion = NOT_AVAILABLE;
				// Read the manufactuer
				if (GetValue( hKeyObject, WIN_CONTROLER_IEEE1394_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_IEEE1394_MANUFACTURER_VALUE);
					csManufacturer = NOT_AVAILABLE; 
				}
				// Read the name
				if (GetValue( hKeyObject, WIN_CONTROLER_IEEE1394_NAME_VALUE, csName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_IEEE1394_NAME_VALUE);
					csName = NOT_AVAILABLE;
				}
				// Read the caption
				if (GetValue( hKeyObject, WIN_CONTROLER_IEEE1394_CAPTION_VALUE, csCaption) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_IEEE1394_CAPTION_VALUE);
					csCaption = NOT_AVAILABLE; 
				}
				// Read the description
				if (GetValue( hKeyObject, WIN_CONTROLER_IEEE1394_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_IEEE1394_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				// Read the verion
				if (GetValue( hKeyObject, WIN_CONTROLER_IEEE1394_VERSION_VALUE, csVersion) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_IEEE1394_VERSION_VALUE);
					csVersion = NOT_AVAILABLE;
				}
				RegCloseKey( hKeyObject);
				myObject.SetType( SYSTEM_CONTROLER_IEEE1394);
				myObject.SetManufacturer( csManufacturer);
				myObject.SetName( csName);
				myObject.SetHardwareVersion( csVersion);
				myObject.SetCaption( csCaption);
				myObject.SetDescription( csDescription);
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
					WIN_CONTROLER_IEEE1394_KEY);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				WIN_CONTROLER_IEEE1394_KEY);
	if (uIndex > 0)
	{
		uTotalIndex += uIndex;
		AddLog( _T( "Registry 9X/Me GetSystemControllers: Find IEEE1394 Controllers OK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "Registry 9X/Me GetSystemControllers: Find IEEE1394 Controllers failed because no valid object !\n"));
	// Windows 9X => Open the PCMCIA controler key
	uIndex = 0;
	dwIndexEnum = 0;
	AddLog( _T( "Registry 9X/Me GetSystemControllers: Trying to find PCMCIA Controllers in HKLM\\%s...\n"),
			WIN_CONTROLER_PCMCIA_KEY);
	if (RegOpenKeyEx( m_hKey, WIN_CONTROLER_PCMCIA_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), WIN_CONTROLER_PCMCIA_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
			{
				csManufacturer = NOT_AVAILABLE; 
				csName = NOT_AVAILABLE;
				csCaption = NOT_AVAILABLE; 
				csDescription = NOT_AVAILABLE; 
				csVersion = NOT_AVAILABLE;
				// Read the manufactuer
				if (GetValue( hKeyObject, WIN_CONTROLER_PCMCIA_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_PCMCIA_MANUFACTURER_VALUE);
					csManufacturer = NOT_AVAILABLE; 
				}
				// Read the name
				if (GetValue( hKeyObject, WIN_CONTROLER_PCMCIA_NAME_VALUE, csName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_PCMCIA_NAME_VALUE);
					csName = NOT_AVAILABLE;
				}
				// Read the caption
				if (GetValue( hKeyObject, WIN_CONTROLER_PCMCIA_CAPTION_VALUE, csCaption) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_PCMCIA_CAPTION_VALUE);
					csCaption = NOT_AVAILABLE; 
				}
				// Read the description
				if (GetValue( hKeyObject, WIN_CONTROLER_PCMCIA_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_PCMCIA_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				// Read the verion
				if (GetValue( hKeyObject, WIN_CONTROLER_PCMCIA_VERSION_VALUE, csVersion) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_CONTROLER_PCMCIA_VERSION_VALUE);
					csVersion = NOT_AVAILABLE;
				}
				RegCloseKey( hKeyObject);
				myObject.SetType( SYSTEM_CONTROLER_PCMCIA);
				myObject.SetManufacturer( csManufacturer);
				myObject.SetName( csName);
				myObject.SetHardwareVersion( csVersion);
				myObject.SetCaption( csCaption);
				myObject.SetDescription( csDescription);
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
					WIN_CONTROLER_PCMCIA_KEY);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				WIN_CONTROLER_PCMCIA_KEY);
	if (uIndex > 0)
	{
		uTotalIndex += uIndex;
		AddLog( _T( "Registry 9X/Me GetSystemControllers: Find PCMCIA Controllers OK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "Registry 9X/Me GetSystemControllers: Find PCMCIA Controllers failed because no valid object !\n"));
	if (uTotalIndex > 0)
	{
		AddLog( _T( "Registry 9X/Me GetSystemControllers: OK (%u objects).\n"), uTotalIndex);
		return TRUE;
	}
	AddLog( _T( "Registry 9X/Me GetSystemControllers: Failed because no controler found.\n"));
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
	if (RegOpenKeyEx( m_hKey, NT_CONTROLER_FLOPPY_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_CONTROLER_FLOPPY_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
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
		AddLog( _T( "Registry NT GetSystemControllers: Find Floppy Controllers OK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "Registry NT GetSystemControllers: Find Floppy Controllers failed because no valid object !\n"));
	// Windows NT => Open the IDE controler key
	uIndex = 0;
	dwIndexEnum = 0;
	AddLog( _T( "Registry NT GetSystemControllers: Trying to find IDE Controllers in HKLM\\%s...\n"),
			NT_CONTROLER_IDE_KEY);
	if (RegOpenKeyEx( m_hKey, NT_CONTROLER_IDE_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_CONTROLER_IDE_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
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
		AddLog( _T( "Registry NT GetSystemControllers: Find IDE Controllers OK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "Registry NT GetSystemControllers: Find IDE Controllers failed because no valid object !\n"));
	// Windows NT => Open the SCSI controler key
	uIndex = 0;
	dwIndexEnum = 0;
	AddLog( _T( "Registry NT GetSystemControllers: Trying to find SCSI Controllers in HKLM\\%s...\n"),
			NT_CONTROLER_SCSI_KEY);
	if (RegOpenKeyEx( m_hKey, NT_CONTROLER_SCSI_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_CONTROLER_SCSI_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
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
		AddLog( _T( "Registry NT GetSystemControllers: Find SCSI Controllers OK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "Registry NT GetSystemControllers: Find SCSI Controllers failed because no valid object !\n"));
	// Windows NT => Open the InfraRed controler key
	uIndex = 0;
	dwIndexEnum = 0;
	AddLog( _T( "Registry NT GetSystemControllers: Trying to find InfraRed Controllers in HKLM\\%s...\n"),
			NT_CONTROLER_INFRARED_KEY);
	if (RegOpenKeyEx( m_hKey, NT_CONTROLER_INFRARED_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_CONTROLER_INFRARED_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
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
		AddLog( _T( "Registry NT GetSystemControllers: Find InfraRed Controllers OK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "Registry NT GetSystemControllers: Find InfraRed Controllers failed because no valid object !\n"));
	// Windows NT => Open the USB controler key
	uIndex = 0;
	dwIndexEnum = 0;
	AddLog( _T( "Registry NT GetSystemControllers: Trying to find USB Controllers in HKLM\\%s...\n"),
			NT_CONTROLER_USB_KEY);
	if (RegOpenKeyEx( m_hKey, NT_CONTROLER_USB_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_CONTROLER_USB_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
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
		AddLog( _T( "Registry NT GetSystemControllers: Find USB Controllers OK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "Registry NT GetSystemControllers: Find USB Controllers failed because no valid object !\n"));
	// Windows NT => Open the IEEE1394 controler key
	uIndex = 0;
	dwIndexEnum = 0;
	AddLog( _T( "Registry NT GetSystemControllers: Trying to find IEEE1394 Controllers in HKLM\\%s...\n"),
			NT_CONTROLER_IEEE1394_KEY);
	if (RegOpenKeyEx( m_hKey, NT_CONTROLER_IEEE1394_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_CONTROLER_IEEE1394_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
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
		AddLog( _T( "Registry NT GetSystemControllers: Find IEEE1394 Controllers OK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "Registry NT GetSystemControllers: Find IEEE1394 Controllers failed because no valid object !\n"));
	// Windows NT => Open the PCMCIA controler key
	uIndex = 0;
	dwIndexEnum = 0;
	AddLog( _T( "Registry NT GetSystemControllers: Trying to find PCMCIA Controllers in HKLM\\%s...\n"),
			NT_CONTROLER_PCMCIA_KEY);
	if (RegOpenKeyEx( m_hKey, NT_CONTROLER_PCMCIA_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_CONTROLER_PCMCIA_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
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
		AddLog( _T( "Registry NT GetSystemControllers: Find PCMCIA Controllers OK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "Registry NT GetSystemControllers: Find PCMCIA Controllers failed because no valid object !\n"));
	if (uTotalIndex > 0)
	{
		AddLog( _T( "Registry NT GetSystemControllers: OK (%u objects).\n"), uTotalIndex);
		return TRUE;
	}
	AddLog( _T( "Registry NT GetSystemControllers: Failed because no controler found.\n"));
	return FALSE;
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
	case VER_PLATFORM_WIN32_WINDOWS:
		// Windows 9X/Me
		return GetStoragePeripherals9X( pList);
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetStoragePeripheralsNT( pList);
	default:
		// Unknown
		AddLog( _T( "Registry GetStoragePeripherals...Failed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::GetStoragePeripherals9X(CStoragePeripheralList *pList)
{
	HKEY			hKeyEnum,
					hKeySubEnum,
					hKeyObject;
	CString			csSubKey,
					csManufacturer,
					csModel,
					csName,
					csDescription,
					csType;
	TCHAR			szKey[256],
					szSubKey[256];
	DWORD			dwLength,
					dwIndexEnum = 0,
					dwIndexSubEnum;
	LONG			lResult;
	FILETIME		MyFileTime;
	CStoragePeripheral	myObject;
	BOOL			bHaveToStore;
	UINT			uIndex = 0,
					uTotalIndex = 0;

	ASSERT( pList);

	AddLog( _T( "Registry 9X/Me GetStoragePeripherals: Trying to find Floppy Drives in HKLM\\%s...\n"),
			WIN_STORAGE_FLOPPY_KEY);
	// Windows 9X => Open the floppy drive key
	if (RegOpenKeyEx( m_hKey, WIN_STORAGE_FLOPPY_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), WIN_STORAGE_FLOPPY_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeySubEnum) == ERROR_SUCCESS)
			{
				dwLength = 255;
				dwIndexSubEnum = 0;
				while ((lResult = RegEnumKeyEx( hKeySubEnum, dwIndexSubEnum, szSubKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
				{
					// For each object, Try to open the device key
					szSubKey[dwLength] = 0;
					bHaveToStore = FALSE;
					csSubKey.Format( _T( "%s\\%s\\%s"), WIN_STORAGE_FLOPPY_KEY, szKey, szSubKey);
					if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
					{
						csManufacturer = NOT_AVAILABLE; 
						csModel = NOT_AVAILABLE;
						csName = NOT_AVAILABLE; 
						csDescription = NOT_AVAILABLE; 
						csType = NOT_AVAILABLE; 
						// Read the manufactuer
						if (GetValue( hKeyObject, WIN_STORAGE_FLOPPY_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
						{
							bHaveToStore = TRUE;
						}
						else
						{
							AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									csSubKey, WIN_STORAGE_FLOPPY_MANUFACTURER_VALUE);
							csManufacturer = NOT_AVAILABLE; 
						}
						// Read the model
						if (GetValue( hKeyObject, WIN_STORAGE_FLOPPY_MODEL_VALUE, csModel) == ERROR_SUCCESS)
						{
							bHaveToStore = TRUE;
						}
						else
						{
							AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									csSubKey, WIN_STORAGE_FLOPPY_MODEL_VALUE);
							csModel = NOT_AVAILABLE;
						}
						// Read the name
						if (GetValue( hKeyObject, WIN_STORAGE_FLOPPY_NAME_VALUE, csName) == ERROR_SUCCESS)
						{
							bHaveToStore = TRUE;
						}
						else
						{
							AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									csSubKey, WIN_STORAGE_FLOPPY_NAME_VALUE);
							csName = NOT_AVAILABLE; 
						}
						// Read the description
						if (GetValue( hKeyObject, WIN_STORAGE_FLOPPY_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
						{
							bHaveToStore = TRUE;
						}
						else
						{
							AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									csSubKey, WIN_STORAGE_FLOPPY_DESCRIPTION_VALUE);
							csDescription = NOT_AVAILABLE; 
						}
						// Read the type
						if (GetValue( hKeyObject, WIN_STORAGE_FLOPPY_TYPE_VALUE, csType) == ERROR_SUCCESS)
						{
							bHaveToStore = TRUE;
						}
						else
						{
							AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									csSubKey, WIN_STORAGE_FLOPPY_TYPE_VALUE);
							csType = NOT_AVAILABLE; 
						}
						RegCloseKey( hKeyObject);
						myObject.SetType( csType);
						myObject.SetManufacturer( csManufacturer);
						myObject.SetName( csName);
						myObject.SetModel( csModel);
						myObject.SetDescription( csDescription);
						// Add the device to the adapter list
						// ValidateComponent9X( csSubKey);
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
					dwIndexSubEnum++;
				}
				RegCloseKey( hKeySubEnum);
				if (dwIndexSubEnum == 0)
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
					WIN_STORAGE_FLOPPY_KEY);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				WIN_STORAGE_FLOPPY_KEY);
	if (uIndex > 0)
	{
		uTotalIndex += uIndex;
		AddLog( _T( "Registry 9X/Me GetStoragePeripherals: Find Floppy Drives OK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "Registry 9X/Me GetStoragePeripherals: Find Floppy Drives failed because no valid object !\n"));
	// Windows 9X => Open the disk drive key
	AddLog( _T( "Registry 9X/Me GetStoragePeripherals: Trying to find Disk Drives in HKLM\\%s...\n"),
			WIN_STORAGE_DISK_KEY);
	uIndex = 0;
	dwIndexEnum = 0;
	if (RegOpenKeyEx( m_hKey, WIN_STORAGE_DISK_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), WIN_STORAGE_DISK_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeySubEnum) == ERROR_SUCCESS)
			{
				dwLength = 255;
				dwIndexSubEnum = 0;
				while ((lResult = RegEnumKeyEx( hKeySubEnum, dwIndexSubEnum, szSubKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
				{
					// For each object, Try to open the device key
					szSubKey[dwLength] = 0;
					bHaveToStore = FALSE;
					csSubKey.Format( _T( "%s\\%s\\%s"), WIN_STORAGE_DISK_KEY, szKey, szSubKey);
					if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
					{
						csManufacturer = NOT_AVAILABLE; 
						csModel = NOT_AVAILABLE;
						csName = NOT_AVAILABLE; 
						csDescription = NOT_AVAILABLE; 
						csType = NOT_AVAILABLE; 
						// Read the manufactuer
						if (GetValue( hKeyObject, WIN_STORAGE_DISK_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
						{
							bHaveToStore = TRUE;
						}
						else
						{
							AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									csSubKey, WIN_STORAGE_DISK_MANUFACTURER_VALUE);
							csManufacturer = NOT_AVAILABLE; 
						}
						// Read the model
						if (GetValue( hKeyObject, WIN_STORAGE_DISK_MODEL_VALUE, csModel) == ERROR_SUCCESS)
						{
							bHaveToStore = TRUE;
						}
						else
						{
							AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									csSubKey, WIN_STORAGE_DISK_MODEL_VALUE);
							csModel = NOT_AVAILABLE;
						}
						// Read the name
						if (GetValue( hKeyObject, WIN_STORAGE_DISK_NAME_VALUE, csName) == ERROR_SUCCESS)
						{
							bHaveToStore = TRUE;
						}
						else
						{
							AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									csSubKey, WIN_STORAGE_DISK_NAME_VALUE);
							csName = NOT_AVAILABLE; 
						}
						// Read the description
						if (GetValue( hKeyObject, WIN_STORAGE_DISK_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
						{
							bHaveToStore = TRUE;
						}
						else
						{
							AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									csSubKey, WIN_STORAGE_DISK_DESCRIPTION_VALUE);
							csDescription = NOT_AVAILABLE; 
						}
						// Read the type
						if (GetValue( hKeyObject, WIN_STORAGE_DISK_TYPE_VALUE, csType) == ERROR_SUCCESS)
						{
							bHaveToStore = TRUE;
						}
						else
						{
							AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									csSubKey, WIN_STORAGE_DISK_TYPE_VALUE);
							csType = NOT_AVAILABLE;
						}
						RegCloseKey( hKeyObject);
						myObject.SetType( csType);
						myObject.SetManufacturer( csManufacturer);
						myObject.SetName( csName);
						myObject.SetModel( csModel);
						myObject.SetDescription( csDescription);
						// Add the device to the adapter list
						// ValidateComponent9X( csSubKey);
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
					dwIndexSubEnum++;
				}
				RegCloseKey( hKeySubEnum);
				if (dwIndexSubEnum == 0)
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
					WIN_STORAGE_DISK_KEY);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				WIN_STORAGE_DISK_KEY);
	if (uIndex > 0)
	{
		uTotalIndex += uIndex;
		AddLog( _T( "Registry 9X/Me GetStoragePeripherals: Find Disk Drives OK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "Registry 9X/Me GetStoragePeripherals: Find Disk Drives failed because no valid object !\n"));
	// Windows 9X => Open the other drive key
	AddLog( _T( "Registry 9X/Me GetStoragePeripherals: Trying to find Other Drives in HKLM\\%s...\n"),
			WIN_STORAGE_OTHER_KEY);
	uIndex = 0;
	dwIndexEnum = 0;
	if (RegOpenKeyEx( m_hKey, WIN_STORAGE_OTHER_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), WIN_STORAGE_OTHER_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeySubEnum) == ERROR_SUCCESS)
			{
				dwLength = 255;
				dwIndexSubEnum = 0;
				while ((lResult = RegEnumKeyEx( hKeySubEnum, dwIndexSubEnum, szSubKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
				{
					// For each object, Try to open the device key
					szSubKey[dwLength] = 0;
					bHaveToStore = FALSE;
					csSubKey.Format( _T( "%s\\%s\\%s"), WIN_STORAGE_OTHER_KEY, szKey, szSubKey);
					if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
					{
						csManufacturer = NOT_AVAILABLE; 
						csModel = NOT_AVAILABLE;
						csName = NOT_AVAILABLE; 
						csDescription = NOT_AVAILABLE; 
						csType = NOT_AVAILABLE; 
						// Read the manufactuer
						if (GetValue( hKeyObject, WIN_STORAGE_OTHER_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
						{
							bHaveToStore = TRUE;
						}
						else
						{
							AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									csSubKey, WIN_STORAGE_OTHER_MANUFACTURER_VALUE);
							csManufacturer = NOT_AVAILABLE; 
						}
						// Read the model
						if (GetValue( hKeyObject, WIN_STORAGE_OTHER_MODEL_VALUE, csModel) == ERROR_SUCCESS)
						{
							bHaveToStore = TRUE;
						}
						else
						{
							AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									csSubKey, WIN_STORAGE_OTHER_MODEL_VALUE);
							csModel = NOT_AVAILABLE;
						}
						// Read the name
						if (GetValue( hKeyObject, WIN_STORAGE_OTHER_NAME_VALUE, csName) == ERROR_SUCCESS)
						{
							bHaveToStore = TRUE;
						}
						else
						{
							AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									csSubKey, WIN_STORAGE_OTHER_NAME_VALUE);
							csName = NOT_AVAILABLE; 
						}
						// Read the description
						if (GetValue( hKeyObject, WIN_STORAGE_OTHER_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
						{
							bHaveToStore = TRUE;
						}
						else
						{
							AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									csSubKey, WIN_STORAGE_OTHER_DESCRIPTION_VALUE);
							csDescription = NOT_AVAILABLE; 
						}
						// Read the type
						if (GetValue( hKeyObject, WIN_STORAGE_OTHER_TYPE_VALUE, csType) == ERROR_SUCCESS)
						{
							bHaveToStore = TRUE;
						}
						else
						{
							AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									csSubKey, WIN_STORAGE_OTHER_TYPE_VALUE);
							csType = NOT_AVAILABLE; 
						}
						RegCloseKey( hKeyObject);
						myObject.SetType( csType);
						myObject.SetManufacturer( csManufacturer);
						myObject.SetName( csName);
						myObject.SetModel( csModel);
						myObject.SetDescription( csDescription);
						// Add the device to the adapter list
						// ValidateComponent9X( csSubKey);
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
					dwIndexSubEnum++;
				}
				RegCloseKey( hKeySubEnum);
				if (dwIndexSubEnum == 0)
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
					WIN_STORAGE_OTHER_KEY);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				WIN_STORAGE_OTHER_KEY);
	if (uIndex > 0)
	{
		uTotalIndex += uIndex;
		AddLog( _T( "Registry 9X/Me GetStoragePeripherals: Find Other Drives OK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "Registry 9X/Me GetStoragePeripherals: Find Other Drives failed because no valid object !\n"));
	if (uTotalIndex > 0)
	{
		AddLog( _T( "Registry 9X/Me GetStoragePeripherals: OK (%u objects).\n"), uTotalIndex);
		return TRUE;
	}
	AddLog( _T( "Registry 9X/Me GetStoragePeripherals: Failed because no storage peripherals found.\n"));
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
	if (RegOpenKeyEx( m_hKey, NT_STORAGE_FLOPPY_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_STORAGE_FLOPPY_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyEnumBus) == ERROR_SUCCESS)
			{
				dwLength = 255;
				dwIndexEnumBus = 0;
				while ((lResult = RegEnumKeyEx( hKeyEnumBus, dwIndexEnumBus, szSubKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
				{
					// For each object, Try to open the device key
					szSubKey[dwLength] = 0;
					bHaveToStore = FALSE;
					csSubKey.Format( _T( "%s\\%s\\%s"), NT_STORAGE_FLOPPY_KEY, szKey, szSubKey);
					if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
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
		AddLog( _T( "Registry NT GetStoragePeripherals: Find Floppy Drives OK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "Registry NT GetStoragePeripherals: Find Floppy Drives failed because no valid object !\n"));

	AddLog( _T( "Registry NT GetStoragePeripherals: Trying to find Other Drives in HKLM\\%s...\n"),
			NT_STORAGE_OTHER_KEY);
	// Windows NT => Open the other drive key
	uIndex = 0;
	if (RegOpenKeyEx( m_hKey, NT_STORAGE_OTHER_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
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
			if (RegOpenKeyEx( m_hKey, csSubKeyBus, 0, KEY_READ, &hKeyEnumBus) == ERROR_SUCCESS)
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
					if (RegOpenKeyEx( m_hKey, csSubKeyTarget, 0, KEY_READ, &hKeyEnumTarget) == ERROR_SUCCESS)
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
							if (RegOpenKeyEx( m_hKey, csSubKeyLogical, 0, KEY_READ, &hKeyEnumLogical) == ERROR_SUCCESS)
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
									if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
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
		AddLog( _T( "Registry NT GetStoragePeripherals: Find Other Drives OK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "Registry NT GetStoragePeripherals: Find Other Drives failed because no valid object !\n"));

	if (uTotalIndex > 0)
	{
		AddLog( _T( "Registry NT GetStoragePeripherals: OK (%u objects).\n"), uTotalIndex);
		return TRUE;
	}
	AddLog( _T( "Registry NT GetStoragePeripherals: Failed because no storage peripherals found.\n"));
	return FALSE;
}

BOOL CRegistry::GetRegistryApplications(CSoftwareList *pList, BOOL hkcu)
{
	ASSERT( pList);

	// Reset object list content
	while (!(pList->GetCount() == 0))
		pList->RemoveHead();
	// Try to get BIOS Informations info from the registry
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_WINDOWS:
		// Windows 9X/Me
		if( hkcu )
			GetRegistryApplications9X( pList, HKEY_CURRENT_USER);
		return GetRegistryApplications9X( pList, HKEY_LOCAL_MACHINE);

	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		if( hkcu )
			GetRegistryApplicationsNT( pList, HKEY_CURRENT_USER);
		return GetRegistryApplicationsNT( pList, HKEY_LOCAL_MACHINE);
	default:
		// Unknown
		AddLog( _T( "Registry GetRegistryApplications...Failed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::GetRegistryApplications9X( CSoftwareList *pList, HKEY hHive)
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
					csUninstall;
	TCHAR			szGUID[256];
	DWORD			dwLength,
					dwLanguage,
					dwIndexEnum = 0;
	LONG			lResult;
	FILETIME		MyFileTime;
	CSoftware		cApp;
	BOOL			bHaveToStore;
	UINT			uIndex = 0;

	ASSERT( pList);

	CString csCurHive = (hHive==HKEY_LOCAL_MACHINE ? _T( "HKLM") : _T( "HKCU")) ;
	AddLog( _T( "Registry 9X GetRegistryApplications READING hive %s  ... \n"),csCurHive);
	// Windows 9X => Open the Apps key
	if (RegOpenKeyEx( hHive, WIN_APPS_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find installed apps
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szGUID, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szGUID[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), WIN_APPS_KEY, szGUID);
			if (RegOpenKeyEx( hHive, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
			{
				csPublisher = NOT_AVAILABLE;
				csName = NOT_AVAILABLE;
				csVersion = NOT_AVAILABLE;
				csFolder = NOT_AVAILABLE;
				csComments = NOT_AVAILABLE;
				csLanguage = NOT_AVAILABLE;
				csUninstall = NOT_AVAILABLE;
				// Read the Publisher
				if (GetValue( hKeyObject, WIN_APPS_VENDOR_VALUE, csPublisher) == ERROR_SUCCESS)
				{
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for %s\\%s\\%s !\n"),csCurHive,
									   csSubKey, WIN_APPS_VENDOR_VALUE);
					csPublisher = NOT_AVAILABLE;
				}
				// Read the name
				if (GetValue( hKeyObject, WIN_APPS_NAME_VALUE, csName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for %s\\%s\\%s !\n"),csCurHive,
									   csSubKey, WIN_APPS_NAME_VALUE);
					csName = NOT_AVAILABLE;
				}
				// Read the version
				if (GetValue( hKeyObject, WIN_APPS_VERSION_VALUE, csVersion) == ERROR_SUCCESS)
				{
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for %s\\%s\\%s !\n"),csCurHive,
									   csSubKey, WIN_APPS_VERSION_VALUE);
					csVersion = NOT_AVAILABLE;
				}
				// Read the folder
				if (GetValue( hKeyObject, WIN_APPS_FOLDER_VALUE, csFolder) == ERROR_SUCCESS)
				{
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for %s\\%s\\%s !\n"),csCurHive,
									   csSubKey, WIN_APPS_FOLDER_VALUE);
					csFolder = NOT_AVAILABLE;
				}
				// Read the comment
				if (GetValue( hKeyObject, WIN_APPS_COMMENTS_VALUE, csComments) == ERROR_SUCCESS)
				{
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for %s\\%s\\%s !\n"),csCurHive,
									   csSubKey, WIN_APPS_COMMENTS_VALUE);
					csComments = NOT_AVAILABLE;
				}
				// Read the language, this gives the language of the installation tool not of the application itself!
				if (GetValue( hKeyObject, WIN_APPS_LANGUAGE_VALUE, &dwLanguage) == ERROR_SUCCESS)
				{
				    DWORD dwSize = 256;
					TCHAR lpstrData[256];
					DWORD dwResult;

					dwResult = VerLanguageName( dwLanguage, lpstrData, dwSize);
					if ((dwResult != 0) && (dwResult < dwSize - 1))
					{
						lpstrData[dwSize]=0;
						csLanguage = lpstrData;
					}
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for %s\\%s\\%s !\n"),csCurHive,
									   csSubKey, WIN_APPS_LANGUAGE_VALUE);
					csLanguage = NOT_AVAILABLE;
				}
				// Read the uninstall string
				if ((GetValue( hKeyObject, WIN_APPS_UNINSTALL_VALUE, csUninstall) != ERROR_SUCCESS) &&
					(GetValue( hKeyObject, WIN_APPS_QUIETUNINSTALL_VALUE, csUninstall) != ERROR_SUCCESS) &&
					(GetValue( hKeyObject, WIN_APPS_MODIFY_VALUE, csUninstall) != ERROR_SUCCESS))
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for %s\\%s\\%s !\n"),csCurHive,
									   csSubKey, WIN_APPS_UNINSTALL_VALUE);
					bHaveToStore = FALSE;
				}
				RegCloseKey( hKeyObject);
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
			AddLog( _T( "Registry 9X/Me GetRegistryApplications: Failed in call to <RegEnumKeyEx> function to find subkey of %s\\%s.\n"),csCurHive,
					WIN_APPS_KEY);
		else
			AddLog( _T( "Registry 9X/Me GetRegistryApplications: OK (%u objects).\n"), uIndex);
	}
	else
		AddLog( _T( "Registry 9X/Me GetRegistryApplications: Failed in call to <RegOpenKeyEx> function for %s\\%s !\n"),csCurHive,
				WIN_APPS_KEY);
	return !pList->IsEmpty();
}

BOOL CRegistry::GetRegistryApplicationsNT(CSoftwareList *pList, HKEY hHive)
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
					csUninstall;
	TCHAR			szGUID[256];
	DWORD			dwLength,
					dwLanguage,
					dwIndexEnum = 0;
	LONG			lResult;
	FILETIME		MyFileTime;
	CSoftware		cApp;
	BOOL			bHaveToStore;
	UINT			uIndex = 0;

	ASSERT( pList);

	CString csCurHive = (hHive==HKEY_LOCAL_MACHINE ? _T( "HKLM") : _T( "HKCU")) ;
	AddLog( _T( "Registry NT GetRegistryApplications READING hive %s  ... \n"),csCurHive);	
	// Windows NT => Open the Apps key
	if (RegOpenKeyEx( hHive, NT_APPS_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find installed apps
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szGUID, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szGUID[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_APPS_KEY, szGUID);
			if (RegOpenKeyEx( hHive, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
			{
				csPublisher = NOT_AVAILABLE;
				csName = NOT_AVAILABLE;
				csVersion = NOT_AVAILABLE;
				csFolder = NOT_AVAILABLE;
				csComments = NOT_AVAILABLE;
				csLanguage = NOT_AVAILABLE;
				csUninstall = NOT_AVAILABLE;
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
						lpstrData[dwSize]=0;
						csLanguage = lpstrData;
					}
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for %s\\%s\\%s !\n"),csCurHive,
									   csSubKey, NT_APPS_LANGUAGE_VALUE);
					csLanguage = NOT_AVAILABLE;
				}
				// Read the uninstall string
				if ((GetValue( hKeyObject, NT_APPS_UNINSTALL_VALUE, csUninstall) != ERROR_SUCCESS) &&
					(GetValue( hKeyObject, NT_APPS_QUIETUNINSTALL_VALUE, csUninstall) != ERROR_SUCCESS) &&
					(GetValue( hKeyObject, NT_APPS_MODIFY_VALUE, csUninstall) != ERROR_SUCCESS))
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for %s\\%s\\%s !\n"),csCurHive,
									   csSubKey, NT_APPS_UNINSTALL_VALUE);
					bHaveToStore = FALSE;
				}
				RegCloseKey( hKeyObject);
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
			AddLog( _T( "Registry NT GetRegistryApplications: Failed in call to <RegEnumKeyEx> function to find subkey of %s\\%s.\n"),csCurHive,
					NT_APPS_KEY);
		else
			AddLog( _T( "Registry NT GetRegistryApplications: OK (%u objects).\n"), uIndex);
	}
	else
		AddLog( _T( "Registry NT GetRegistryApplications: Failed in call to <RegOpenKeyEx> function for %s\\%s !\n"),csCurHive,
				NT_APPS_KEY);
	return !pList->IsEmpty();
}

BOOL CRegistry::GetDeviceDescription( CString &csDescription)
{
	// Try to get Modems Informations info from the registry
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_WINDOWS:
		// Windows 9X/Me
		return GetDeviceDescription9X( csDescription);
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetDeviceDescriptionNT( csDescription);
	default:
		// Unknown
		AddLog( _T( "Registry GetDeviceDescription...Failed because unsupported or unrecognized OS !\n"));
		return NULL;
	}
	return NULL;
}

BOOL CRegistry::GetDeviceDescription9X( CString &csDescription)
{
	HKEY		 hKey = NULL;

	csDescription = NOT_AVAILABLE;
	AddLog( _T( "Registry 9X/Me GetDeviceDescription..."));
	if (RegOpenKeyEx( m_hKey, WIN_COMPUTER_DESCRIPTION_KEY, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		AddLog( _T( "Failed in call to <RegOpenKey> function for HKLM\\%s !\n"),
				WIN_COMPUTER_DESCRIPTION_KEY);
		return FALSE;
	}

	// Get description.
	if (GetValue( hKey, WIN_COMPUTER_DESCRIPTION_VALUE, csDescription) != ERROR_SUCCESS)
	{
		AddLog( _T( "Failed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
				WIN_COMPUTER_DESCRIPTION_KEY,
				WIN_COMPUTER_DESCRIPTION_VALUE);
		RegCloseKey( hKey);
		return FALSE;
	}
	RegCloseKey( hKey);
	AddLog( _T( "OK (%s).\n"), csDescription);
	return TRUE;
}

BOOL CRegistry::GetDeviceDescriptionNT( CString &csDescription)
{
	HKEY		 hKey = NULL;

	csDescription = NOT_AVAILABLE;
	AddLog( _T( "Registry NT GetDeviceDescription..."));
	if (RegOpenKeyEx( m_hKey, NT_COMPUTER_DESCRIPTION_KEY, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		AddLog( _T( "Failed in call to <RegOpenKey> function for HKLM\\%s !\n"),
				NT_COMPUTER_DESCRIPTION_KEY);
		return FALSE;
	}

	// Get description.
	if (GetValue( hKey, NT_COMPUTER_DESCRIPTION_VALUE, csDescription) != ERROR_SUCCESS)
	{
		AddLog( _T( "Failed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
				NT_COMPUTER_DESCRIPTION_KEY,
				NT_COMPUTER_DESCRIPTION_VALUE);
		RegCloseKey( hKey);
		return FALSE;
	}
	RegCloseKey( hKey);
	AddLog( _T( "OK (%s).\n"), csDescription);
	return TRUE;
}

BOOL CRegistry::GetDomainOrWorkgroup(CString &csDomain)
{
	// Try to get Modems Informations info from the registry
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_WINDOWS:
		// Windows 9X/Me
		return GetDomainOrWorkgroup9X( csDomain);
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetDomainOrWorkgroupNT( csDomain);
	default:
		// Unknown
		csDomain = NOT_AVAILABLE;
		AddLog( _T( "Registry GetDomainOrWorkgroup...Failed because unsupported or unrecognized OS !\n"));
		return NULL;
	}
	return NULL;
}

BOOL CRegistry::GetDomainOrWorkgroup9X(CString &csDomain)
{
	HKEY  hKey = NULL;

	csDomain = NOT_AVAILABLE;
	AddLog( _T( "Registry 9X/Me GetDomainOrWorkgroup: Trying to get Domain name..."));
	// Try to get domain name
	if (RegOpenKeyEx( m_hKey, WIN_DOMAIN_KEY, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		// Get domain.
		if (GetValue( hKey, WIN_DOMAIN_VALUE, csDomain) == ERROR_SUCCESS)
		{
			RegCloseKey( hKey);
			AddLog( _T( "OK (%s).\n"), csDomain);
			return TRUE;
		}
		AddLog( _T( "Failed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
				WIN_DOMAIN_KEY,
				WIN_DOMAIN_VALUE);
		RegCloseKey( hKey);
	}
	else
		AddLog( _T( "Failed in call to <RegOpenKey> function for HKLM\\%s !\n"),
				WIN_DOMAIN_KEY);
	// Failed to get domain name => Try to get workgroup
	AddLog( _T( "Registry 9X/Me GetDomainOrWorkgroup: Trying to get workgroup name..."));
	if (RegOpenKeyEx( m_hKey, WIN_WORKGROUP_KEY, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		AddLog( _T( "Failed in call to <RegOpenKey> function for HKLM\\%s !\n"),
				WIN_WORKGROUP_KEY);
		return FALSE;
	}
	// Get workgroup.
	if (GetValue( hKey, WIN_WORKGROUP_VALUE, csDomain) != ERROR_SUCCESS)
	{
		AddLog( _T( "Failed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
				WIN_WORKGROUP_KEY,
				WIN_WORKGROUP_VALUE);
		RegCloseKey( hKey);
		return FALSE;
	}
	RegCloseKey( hKey);
	AddLog( _T( "OK (%s).\n"), csDomain);
	return TRUE;
}

BOOL CRegistry::GetDomainOrWorkgroupNT(CString &csDomain)
{
	HKEY  hKey = NULL;

	csDomain = NOT_AVAILABLE;
	AddLog( _T( "Registry NT GetDomainOrWorkgroup..."));
	if (RegOpenKeyEx( m_hKey, NT_DOMAIN_KEY, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		AddLog( _T( "Failed in call to <RegOpenKey> function for HKLM\\%s !\n"),
				NT_DOMAIN_KEY);
		return FALSE;
	}

	// Get domain name or workgroup.
	if (GetValue( hKey, NT_DOMAIN_VALUE, csDomain) != ERROR_SUCCESS)
	{
		AddLog( _T( "Failed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
				NT_DOMAIN_KEY,
				NT_DOMAIN_VALUE);
		RegCloseKey( hKey);
		return FALSE;
	}
	RegCloseKey( hKey);
	AddLog( _T( "OK (%s).\n"), csDomain);
	return TRUE;
}

BOOL CRegistry::GetWindowsRegistration(CString &csCompany, CString &csOwner, CString &csProductID)
{
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_WINDOWS:
		// Windows 9X/Me
		return GetWindowsRegistration9X( csCompany, csOwner, csProductID);
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetWindowsRegistrationNT( csCompany, csOwner, csProductID);
	default:
		// Unknown
		csCompany = NOT_AVAILABLE;
		csOwner = NOT_AVAILABLE;
		csProductID = NOT_AVAILABLE;
		AddLog( _T( "Registry GetWindowsRegistration...Failed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::GetWindowsRegistration9X(CString &csCompany, CString &csOwner, CString &csProductID)
{
	HKEY			hKeyObject;
	CString			csSubKey;

	AddLog( _T( "Registry 9X/Me GetWindowsRegistration...\n"));
	// Windows 9X => Open the registration key
	if (RegOpenKeyEx( m_hKey, WIN_REGISTRATION_KEY, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
	{
		// Read the company name
		if (GetValue( hKeyObject, WIN_REGISTRATION_COMPANY_VALUE, csCompany) == ERROR_SUCCESS)
		{
		}
		else
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
							   WIN_REGISTRATION_KEY, WIN_REGISTRATION_COMPANY_VALUE);
			csCompany = NOT_AVAILABLE;
		}
		// Read the user
		if (GetValue( hKeyObject, WIN_REGISTRATION_OWNER_VALUE, csOwner) == ERROR_SUCCESS)
		{
		}
		else
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
							   WIN_REGISTRATION_KEY, WIN_REGISTRATION_OWNER_VALUE);
			csOwner = NOT_AVAILABLE;
		}
		// Read the productID
		if (GetValue( hKeyObject, WIN_REGISTRATION_PRODUCT_ID_VALUE, csProductID) == ERROR_SUCCESS)
		{
		}
		else
		{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
							   WIN_REGISTRATION_KEY, WIN_REGISTRATION_PRODUCT_ID_VALUE);
			csProductID = NOT_AVAILABLE;
		}
		RegCloseKey( hKeyObject);
		// Add the device to the adapter list
		StrForSQL( csCompany);
		StrForSQL( csOwner);
		StrForSQL( csProductID);
		AddLog( _T( "Registry 9X/Me GetWindowsRegistration: OK (%s %s %s).\n"),
				csCompany, csOwner, csProductID);
		return TRUE;
	}
	AddLog( _T( "Registry 9X/Me GetWindowsRegistration: Failed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
			WIN_REGISTRATION_KEY);
	return FALSE;
}


BOOL CRegistry::GetWindowsRegistrationNT(CString &csCompany, CString &csOwner, CString &csProductID)
{
	HKEY			hKeyObject;
	CString			csSubKey;

	AddLog( _T( "Registry NT GetWindowsRegistration...\n"));
	// Windows NT => Open the registration key
	if (RegOpenKeyEx( m_hKey, NT_REGISTRATION_KEY, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
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
		AddLog( _T( "Registry NT GetWindowsRegistration: OK (%s %s %s).\n"),
				csCompany, csOwner, csProductID);
		return TRUE;
	}
	AddLog( _T( "Registry NT GetWindowsRegistration: Failed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
			NT_REGISTRATION_KEY);
	return FALSE;
}



BOOL CRegistry::GetWindowsProductKey(CString &csProductKey)
{	
	char KeyChars[24] = {'B','C','D','F','G','H','J','K','M','P','Q','R','T','V','W','X','Y','2','3','4','6','7','8','9'};
	char Result[30];

	CString csKeyPath, csValueName;

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

	AddLog( _T( "Registry GetWindowsProductKey..."));

	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_WINDOWS:
		// Windows 9X/Me
		csKeyPath = WIN_REGISTRATION_KEY;
		csValueName = WIN_REGISTRATION_DIGITAL_ID_VALUE;
		break;
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		csKeyPath = NT_REGISTRATION_KEY;
		csValueName = NT_REGISTRATION_DIGITAL_ID_VALUE;
		break;
	default:
		// Unknown
		csProductKey = NOT_AVAILABLE;
		AddLog( _T( "Registry GetWindowsProductKey...Failed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}

	//Requete registry
	if( RegOpenKeyEx( HKEY_LOCAL_MACHINE, csKeyPath, 0, KEY_READ, &InfoKey ) == ERROR_SUCCESS )
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

			AddLog( _T( "OK (%s).\n"), csProductKey);

			return TRUE;
		}
		else{
			AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s !\n"), csValueName);
		}
	}
	else{
		AddLog( _T( "Registry GetWindowsProductKey: Failed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
			csKeyPath);
	}
	return FALSE;
}

BOOL CRegistry::GetLoggedOnUser(CString &csUser)
{
	// Try to get Modems Informations info from the registry
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_WINDOWS:
		// Windows 9X/Me
		return GetLoggedOnUser9X( csUser);
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetLoggedOnUserNT( csUser);
	default:
		// Unknown
		csUser = NOT_AVAILABLE;
		AddLog( _T( "Registry GetLoggedOnUser...Failed because unsupported or unrecognized OS !\n"));
		return NULL;
	}
	return NULL;
}

BOOL CRegistry::GetLoggedOnUser9X(CString &csUser)
{
	HKEY  hKey = NULL;

	csUser = NOT_AVAILABLE;
	AddLog( _T( "Registry 9X/Me GetLoggedOnUser: Trying to get Logon User Name..."));
	// Try to get logged on user
	if (RegOpenKeyEx( HKEY_LOCAL_MACHINE, WIN_LOGON_USER_KEY, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		// Get user name.
		if (GetValue( hKey, WIN_LOGON_USER_VALUE, csUser) == ERROR_SUCCESS)
		{
			RegCloseKey( hKey);
			AddLog( _T( "OK (%s).\n"), csUser);
			return TRUE;
		}
		AddLog( _T( "Failed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
				WIN_LOGON_USER_KEY,
				WIN_LOGON_USER_VALUE);
		RegCloseKey( hKey);
	}
	else
		AddLog( _T( "Failed in call to <RegOpenKey> function for HKLM\\%s !\n"),
				WIN_LOGON_USER_KEY);
	return FALSE;
}

BOOL CRegistry::GetLoggedOnUserNT(CString &csUser)
{
	HKEY  hKey = NULL;

	csUser = NOT_AVAILABLE;
	AddLog( _T( "Registry NT GetLoggedOnUser: Trying to get Logon User Name..."));
	// Try to get logged on user
	if (RegOpenKeyEx( HKEY_CURRENT_USER, NT_LOGON_USER_KEY, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		// Get user name.
		if (GetValue( hKey, NT_LOGON_USER_VALUE, csUser) == ERROR_SUCCESS)
		{
			RegCloseKey( hKey);
			AddLog( _T( "OK (%s).\n"), csUser);
			return TRUE;
		}
		AddLog( _T( "Failed in call to <RegQueryValueEx> function for HKCU\\%s\\%s !\n"),
				NT_LOGON_USER_KEY,
				NT_LOGON_USER_VALUE);
		RegCloseKey( hKey);
	}
	else
		AddLog( _T( "Failed in call to <RegOpenKey> function for HKCU\\%s !\n"),
				NT_LOGON_USER_KEY);
	return FALSE;
}

BOOL CRegistry::GetLastLoggedUser(CString &csLastLoggedUser)
{
	HKEY  hKey = NULL;
	LONG lResult;

  	AddLog( _T( "Registry NT GetLastLoggedUser: Trying to get the last user who'd been logged in..."));
	// Since Vista 
	if (RegOpenKeyEx( HKEY_CURRENT_USER, VISTA_LASTLOGGEDUSER_USER_KEY, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		lResult = GetValue( hKey, VISTA_LASTLOGGEDUSER_USER_VALUE, csLastLoggedUser);
		RegCloseKey( hKey);	
	}
	// Since Windows 2000, deprecated on Vista
	if (lResult != ERROR_SUCCESS) 
	{
		if (RegOpenKeyEx( HKEY_LOCAL_MACHINE, NT_LASTLOGGEDUSER_USER_KEY, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			lResult = GetValue( hKey, NT_LASTLOGGEDUSER_USER_VALUE, csLastLoggedUser);
			RegCloseKey( hKey);
		} 
		else 
		{
			AddLog( _T( "Failed in retrieve Last Logged User from registry!\n"));
			csLastLoggedUser = NOT_AVAILABLE;
			return FALSE;
		}
	}
	if (lResult == ERROR_SUCCESS) 
	{
		AddLog( _T( "OK (%s).\n"), csLastLoggedUser);
		return TRUE;
	}
	else 
	{
		AddLog( _T( "Failed in read Last Logged User from registry!\n"));
		csLastLoggedUser = NOT_AVAILABLE;
		return FALSE;
	}
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
		lResult = RegOpenKeyEx( HKEY_CLASSES_ROOT, lpstrSubKey, 0, KEY_READ, &hKey);
		break;
	case HKCU_TREE: // HKEY_CURRENT_USER
		AddLog( _T( "Registry GetRegistryValue (%s\\%s\\%s)..."),
			_T( "HKCU"),
			lpstrSubKey,
			lpstrValue);
		lResult = RegOpenKeyEx( HKEY_CURRENT_USER, lpstrSubKey, 0, KEY_READ, &hKey);
		break;
	case HKLM_TREE: // HKEY_LOCAL_MACHINE
		AddLog( _T( "Registry GetRegistryValue (%s\\%s\\%s)..."),
			_T( "HKLM"),
			lpstrSubKey,
			lpstrValue);
		lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, lpstrSubKey, 0, KEY_READ, &hKey);
		break;
	case HKU_TREE: // HKEY_USERS
		AddLog( _T( "Registry GetRegistryValue (%s\\%s\\%s)..."),
			_T( "HKU"),
			lpstrSubKey,
			lpstrValue);
		lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, lpstrSubKey, 0, KEY_READ, &hKey);
		break;
	case HKCC_TREE: // HKEY_CURRENT_CONFIG
		AddLog( _T( "Registry GetRegistryValue (%s\\%s\\%s)..."),
			_T( "HKCC"),
			lpstrSubKey,
			lpstrValue);
		lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, lpstrSubKey, 0, KEY_READ, &hKey);
		break;
	case HKDD_TRE: // HKEY_DYN_DATA (9X only)
		AddLog( _T( "Registry GetRegistryValue (%s\\%s\\%s)..."),
			_T( "HKDD"),
			lpstrSubKey,
			lpstrValue);
		lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, lpstrSubKey, 0, KEY_READ, &hKey);
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

BOOL CRegistry::GetInputDevices(CInputDeviceList *pList)
{
	ASSERT( pList);

	// Reset object list content
	while (!(pList->GetCount() == 0))
		pList->RemoveHead();
	// Try to get system Controllers Informations info from the registry
	switch( m_dwPlatformId)
	{
	case VER_PLATFORM_WIN32_WINDOWS:
		// Windows 9X/Me
		return GetInputDevices9X( pList);
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetInputDevicesNT( pList);
	default:
		// Unknown
		AddLog( _T( "Registry GetInputDevices...Failed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::GetInputDevices9X(CInputDeviceList *pList)
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

	// Windows 9X => Open the keyboard key
	uIndex = 0;
	dwIndexEnum = 0;
	AddLog( _T( "Registry 9X/Me GetInputDevices: Trying to find Keyboards in HKLM\\%s...\n"),
			WIN_INPUT_KEYBOARD_KEY);
	if (RegOpenKeyEx( m_hKey, WIN_INPUT_KEYBOARD_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), WIN_INPUT_KEYBOARD_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
			{
				csManufacturer = NOT_AVAILABLE; 
				csCaption = NOT_AVAILABLE; 
				csDescription = NOT_AVAILABLE; 
				csPointingType = NOT_AVAILABLE; 
				csPointingInterface = NOT_AVAILABLE; 
				// Read the manufactuer
				if (GetValue( hKeyObject, WIN_INPUT_KEYBOARD_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_INPUT_KEYBOARD_MANUFACTURER_VALUE);
					csManufacturer = NOT_AVAILABLE; 
				}
				// Read the caption
				if (GetValue( hKeyObject, WIN_INPUT_KEYBOARD_CAPTION_VALUE, csCaption) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_INPUT_KEYBOARD_CAPTION_VALUE);
					csCaption = NOT_AVAILABLE; 
				}
				// Read the description
				if (GetValue( hKeyObject, WIN_INPUT_KEYBOARD_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_INPUT_KEYBOARD_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				// Read the pointing type
				if (GetValue( hKeyObject, WIN_INPUT_KEYBOARD_TYPE_VALUE, csPointingType) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_INPUT_KEYBOARD_TYPE_VALUE);
					csPointingType = NOT_AVAILABLE; 
				}
				// Read the pointing interface
				if (GetValue( hKeyObject, WIN_INPUT_KEYBOARD_INTERFACE_VALUE, csPointingInterface) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_INPUT_KEYBOARD_INTERFACE_VALUE);
					csPointingInterface = NOT_AVAILABLE; 
				}
				RegCloseKey( hKeyObject);
				myObject.SetType( INPUT_DEVICE_KEYBOARD);
				myObject.SetManufacturer( csManufacturer);
				myObject.SetCaption( csCaption);
				myObject.SetDescription( csDescription);
				myObject.SetPointingType( csPointingType);
				myObject.SetPointingInterface( csPointingInterface);
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
					WIN_INPUT_KEYBOARD_KEY);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				WIN_INPUT_KEYBOARD_KEY);
	if (uIndex > 0)
	{
		uTotalIndex += uIndex;
		AddLog( _T( "Registry 9X/Me GetInputDevices: Find Keyboards OK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "Registry 9X/Me GetInputDevices: Find Keyboards failed because no valid object !\n"));
	// Windows 9X => Open the Pointing Devices key
	uIndex = 0;
	dwIndexEnum = 0;
	AddLog( _T( "Registry 9X/Me GetInputDevices: Trying to find Pointing Devices in HKLM\\%s...\n"),
			WIN_INPUT_POINTING_KEY);
	if (RegOpenKeyEx( m_hKey, WIN_INPUT_POINTING_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), WIN_INPUT_POINTING_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
			{
				csManufacturer = NOT_AVAILABLE; 
				csCaption = NOT_AVAILABLE; 
				csDescription = NOT_AVAILABLE; 
				csPointingType = NOT_AVAILABLE; 
				csPointingInterface = NOT_AVAILABLE; 
				// Read the manufactuer
				if (GetValue( hKeyObject, WIN_INPUT_KEYBOARD_MANUFACTURER_VALUE, csManufacturer) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_INPUT_KEYBOARD_MANUFACTURER_VALUE);
					csManufacturer = NOT_AVAILABLE;
				}
				// Read the caption
				if (GetValue( hKeyObject, WIN_INPUT_KEYBOARD_CAPTION_VALUE, csCaption) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_INPUT_KEYBOARD_CAPTION_VALUE);
					csCaption = NOT_AVAILABLE; 
				}
				// Read the description
				if (GetValue( hKeyObject, WIN_INPUT_KEYBOARD_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_INPUT_KEYBOARD_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				// Read the pointing type
				if (GetValue( hKeyObject, WIN_INPUT_KEYBOARD_TYPE_VALUE, csPointingType) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_INPUT_KEYBOARD_TYPE_VALUE);
					csPointingType = NOT_AVAILABLE; 
				}
				// Read the pointing interface
				if (GetValue( hKeyObject, WIN_INPUT_KEYBOARD_INTERFACE_VALUE, csPointingInterface) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_INPUT_KEYBOARD_INTERFACE_VALUE);
					csPointingInterface = NOT_AVAILABLE; 
				}
				RegCloseKey( hKeyObject);
				myObject.SetType( INPUT_DEVICE_POINTING);
				myObject.SetManufacturer( csManufacturer);
				myObject.SetCaption( csCaption);
				myObject.SetDescription( csDescription);
				myObject.SetPointingType( csPointingType);
				myObject.SetPointingInterface( csPointingInterface);
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
					WIN_INPUT_POINTING_KEY);
	}
	else
		AddLog( _T( "\tFailed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				WIN_INPUT_POINTING_KEY);
	if (uIndex > 0)
	{
		uTotalIndex += uIndex;
		AddLog( _T( "Registry 9X/Me GetInputDevices: Find Pointing Devices OK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "Registry 9X/Me GetInputDevices: Find Pointing Devices failed because no valid object !\n"));
	if (uTotalIndex > 0)
	{
		AddLog( _T( "Registry 9X/Me GetInputDevices: OK (%u objects).\n"), uTotalIndex);
		return TRUE;
	}
	AddLog( _T( "Registry 9X/Me GetInputDevices: Failed because no controler found.\n"));
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
	if (RegOpenKeyEx( m_hKey, NT_INPUT_KEYBOARD_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_INPUT_KEYBOARD_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
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
		AddLog( _T( "Registry NT GetInputDevices: Find Keyboards OK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "Registry NT GetInputDevices: Find Keyboards failed because no valid object !\n"));
	// Windows NT => Open the Pointing Devices key
	uIndex = 0;
	dwIndexEnum = 0;
	AddLog( _T( "Registry NT GetInputDevices: Trying to find Pointing Devices in HKLM\\%s...\n"),
			NT_INPUT_POINTING_KEY);
	if (RegOpenKeyEx( m_hKey, NT_INPUT_POINTING_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_INPUT_POINTING_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
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
		AddLog( _T( "Registry NT GetInputDevices: Find Pointing Devices OK (%u objects)\n"), uIndex);
	}
	else
		AddLog( _T( "Registry NT GetInputDevices: Find Pointing Devices failed because no valid object !\n"));
	if (uTotalIndex > 0)
	{
		AddLog( _T( "Registry NT GetInputDevices: OK (%u objects).\n"), uTotalIndex);
		return TRUE;
	}
	AddLog( _T( "Registry NT GetInputDevices: Failed because no controler found.\n"));
	return FALSE;
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
	case VER_PLATFORM_WIN32_WINDOWS:
		// Windows 9X/Me
		return GetSystemPorts9X( pList);
	case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000/XP/2003
		return GetSystemPortsNT( pList);
	default:
		// Unknown
		AddLog( _T( "Registry GetSystemPorts...Failed because unsupported or unrecognized OS !\n"));
		return FALSE;
	}
	return FALSE;
}

BOOL CRegistry::GetSystemPorts9X(CSystemPortList *pList)
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

	AddLog( _T( "Registry 9X/Me GetSystemPorts...\n"));
	// Windows 9X => Open the modem key
	if (RegOpenKeyEx( m_hKey, WIN_SYSTEM_PORT_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), WIN_SYSTEM_PORT_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
			{
				csType = NOT_AVAILABLE; 
				csName = NOT_AVAILABLE; 
				csCaption = NOT_AVAILABLE; 
				csDescription = NOT_AVAILABLE; 
				// Read the type, i.e the serial or parallel
				if (GetValue( hKeyObject, WIN_SYSTEM_PORT_TYPE_VALUE, &dwValue) == ERROR_SUCCESS)
				{
					switch (dwValue)
					{
					case WIN_SYSTEM_PORT_CLASS_SERIAL:
						csType = SYSTEM_PORT_SERIAL;
						break;
					case WIN_SYSTEM_PORT_CLASS_PARALLEL:
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
									   csSubKey, WIN_SYSTEM_PORT_TYPE_VALUE);
					csType = NOT_AVAILABLE;
				}
				// Read the name
				if (GetValue( hKeyObject, WIN_SYSTEM_PORT_NAME_VALUE, csName) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_SYSTEM_PORT_NAME_VALUE);
					csName = NOT_AVAILABLE; 
				}
				// Read the caption
				if (GetValue( hKeyObject, WIN_SYSTEM_PORT_CAPTION_VALUE, csCaption) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_SYSTEM_PORT_CAPTION_VALUE);
					csCaption = NOT_AVAILABLE; 
				}
				// Read the description
				if (GetValue( hKeyObject, WIN_SYSTEM_PORT_DESCRIPTION_VALUE, csDescription) == ERROR_SUCCESS)
				{
					bHaveToStore = TRUE;
				}
				else
				{
					AddLog( _T( "\tFailed in call to <RegQueryValueEx> function for HKLM\\%s\\%s !\n"),
									   csSubKey, WIN_SYSTEM_PORT_DESCRIPTION_VALUE);
					csDescription = NOT_AVAILABLE; 
				}
				RegCloseKey( hKeyObject);
				myObject.SetType( csType);
				myObject.SetName( csName);
				myObject.SetCaption( csCaption);
				myObject.SetDescription( csDescription);
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
			AddLog( _T( "Registry 9X/Me GetSystemPorts: Failed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					WIN_SYSTEM_PORT_KEY);
		else
			AddLog( _T( "Registry 9X/Me GetSystemPorts: OK (%u objects).\n"), uIndex);
	}
	else
		AddLog( _T( "Registry 9X/Me GetSystemPorts: Failed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				WIN_SYSTEM_PORT_KEY);
	return !pList->IsEmpty();
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
	if (RegOpenKeyEx( m_hKey, NT_SYSTEM_PORT_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find devices
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szKey, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the device key
			szKey[dwLength] = 0;
			bHaveToStore = FALSE;
			csSubKey.Format( _T( "%s\\%s"), NT_SYSTEM_PORT_KEY, szKey);
			if (RegOpenKeyEx( m_hKey, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
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
			AddLog( _T( "Registry NT GetSystemPorts: Failed in call to <RegEnumKeyEx> function to find subkey of HKLM\\%s.\n"),
					NT_SYSTEM_PORT_KEY);
		else
			AddLog( _T( "Registry NT GetSystemPorts: OK (%u objects).\n"), uIndex);
	}
	else
		AddLog( _T( "Registry NT GetSystemPorts: Failed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				NT_SYSTEM_PORT_KEY);
	return !pList->IsEmpty();
}

BOOL CRegistry::ValidateComponent9X(LPCTSTR lpstrComponentKey)
{
	HKEY			hKeyEnum,
					hKeyObject;
	CString			csSubKey,
					csComponentKey = lpstrComponentKey,
					csValue;
	TCHAR			szComponentName[256];
	DWORD			dwLength,
					dwValue,
					dwIndexEnum = 0;
	LONG			lResult;
	FILETIME		MyFileTime;

	AddLog( _T( "\tRegistry 9X/Me ValidateComponent...\n"));
	// Windows 9X => Open the Config Manager key
	if (RegOpenKeyEx( HKEY_DYN_DATA, WIN_CONFIG_MANAGER_KEY, 0, KEY_READ, &hKeyEnum) == ERROR_SUCCESS)
	{
		// Enum the devices subkeys to find components
		dwLength = 255;
		while ((lResult = RegEnumKeyEx( hKeyEnum, dwIndexEnum, szComponentName, &dwLength, 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS)
		{
			// For each object, Try to open the component key
			szComponentName[dwLength] = 0;
			csSubKey.Format( _T( "%s\\%s"), WIN_CONFIG_MANAGER_KEY, szComponentName);
			if (RegOpenKeyEx( HKEY_DYN_DATA, csSubKey, 0, KEY_READ, &hKeyObject) == ERROR_SUCCESS)
			{
				// OK => Read the component key
				if (GetValue( hKeyObject, WIN_CONFIG_MANAGER_COMPONENT_VALUE, csValue) != ERROR_SUCCESS)
				{
					AddLog( _T( "\t\tFailed in call to <RegQueryValueEx> function for HKDD\\%s\\%s !\n"),
									   csSubKey, WIN_CONFIG_MANAGER_COMPONENT_VALUE);
					RegCloseKey( hKeyObject);
					// Enum the next device
					dwLength = 255;
					dwIndexEnum++;
					continue;
				}
				// OK => check this the component we want
				if (csComponentKey.Find( csValue) == -1)
				{
					// Not the component
					RegCloseKey( hKeyObject);
					// Enum the next device
					dwLength = 255;
					dwIndexEnum++;
					continue;
				}
				// Read the status
				if (GetValue( hKeyObject, WIN_CONFIG_MANAGER_STATUS_VALUE, &dwValue) != ERROR_SUCCESS)
				{
					AddLog( _T( "\t\tFailed in call to <RegQueryValueEx> function for HKDD\\%s\\%s !\n"),
									   csSubKey, WIN_CONFIG_MANAGER_STATUS_VALUE);
					RegCloseKey( hKeyObject);
					// Enum the next device
					dwLength = 255;
					dwIndexEnum++;
					continue;
				}
				AddLog( _T( "\t\tStatus %s: %.04X\n"), csSubKey, dwValue);
				RegCloseKey( hKeyObject);
			}
			else
				AddLog( _T( "\t\tFailed in call to <RegOpenKeyEx> function for HKDD\\%s !\n"),
								   csSubKey);
			// Enum the next device
			dwLength = 255;
			dwIndexEnum++;
		}
		RegCloseKey( hKeyEnum);
		if (dwIndexEnum == 0)
			// No key found
			AddLog( _T( "\tRegistry 9X/Me ValidateComponent: Failed in call to <RegEnumKeyEx> function to find subkey of HKDD\\%s.\n"),
					WIN_CONFIG_MANAGER_KEY);
		else
			AddLog( _T( "\tRegistry 9X/Me ValidateComponent: OK.\n"));
	}
	else
		AddLog( _T( "\tRegistry 9X/Me ValidateComponent: Failed in call to <RegOpenKeyEx> function for HKLM\\%s !\n"),
				WIN_CONFIG_MANAGER_KEY);
	return FALSE;
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
		lResult = RegOpenKeyEx( HKEY_CLASSES_ROOT, lpstrSubKey, 0, KEY_READ, &hKey);
		break;
	case HKCU_TREE: // HKEY_CURRENT_USER
		AddLog( _T( "Registry GetRegistryMultipleValues (%s\\%s)...\n"),
			_T( "HKCU"),
			lpstrSubKey);
		lResult = RegOpenKeyEx( HKEY_CURRENT_USER, lpstrSubKey, 0, KEY_READ, &hKey);
		break;
	case HKLM_TREE: // HKEY_LOCAL_MACHINE
		AddLog( _T( "Registry GetRegistryMultipleValues (%s\\%s)...\n"),
			_T( "HKLM"),
			lpstrSubKey);
		lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, lpstrSubKey, 0, KEY_READ, &hKey);
		break;
	case HKU_TREE: // HKEY_USERS
		AddLog( _T( "Registry GetRegistryMultipleValues (%s\\%s)...\n"),
			_T( "HKU"),
			lpstrSubKey);
		lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, lpstrSubKey, 0, KEY_READ, &hKey);
		break;
	case HKCC_TREE: // HKEY_CURRENT_CONFIG
		AddLog( _T( "Registry GetRegistryMultipleValues (%s\\%s)...\n"),
			_T( "HKCC"),
			lpstrSubKey);
		lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, lpstrSubKey, 0, KEY_READ, &hKey);
		break;
	case HKDD_TRE: // HKEY_DYN_DATA (9X only)
		AddLog( _T( "Registry GetRegistryMultipleValues (%s\\%s)...\n"),
			_T( "HKDD"),
			lpstrSubKey);
		lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, lpstrSubKey, 0, KEY_READ, &hKey);
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
