//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// InventoryRequest.cpp: implementation of the CInventoryRequest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InventoryRequest.h"
#include "FilePackageHistory.h"
#include "commonDownload.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInventoryRequest::CInventoryRequest( BOOL bNotify)
{
	m_bNotify = bNotify;

	if (!initInventory())
		// Can't get Device hardware and os => stop !
		exit( -1);
}

CInventoryRequest::~CInventoryRequest()
{
	delete m_pTheDB;
	delete m_pState;
	delete m_pPluginState;
	delete m_pSysInfo;
}

void CInventoryRequest::setSuccess()
{
	writeLastInventoryState();
}

BOOL CInventoryRequest::initInventory()
{
	CString csStateFile;

	if (m_bNotify)
		// Notify IP information changes
		setQuery( _T( "NOTIFY"), _T( "IP"));
	else
	{
		setQuery( _T( "INVENTORY"));

		/****	
		*
		* Package history inventory section
		*
		****/
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Loading Download history"));
		if (!loadDownloadHistory())
		{
			m_pLogger->log( LOG_PRIORITY_DEBUG, _T("INVENTORY => Failed opening Download Package history file <%s>"), LookupError( GetLastError()));
		}
	}
	/******************/

	if( m_pDeviceid->getOldDeviceID() != _T("") )
		m_cmXml.AddChildElem( _T("OLD_DEVICEID"),m_pDeviceid->getOldDeviceID());
			
	/****	
	*
	* Get the Device netbios Name
	*
	****/

	m_pTheDB	= new CXMLInteract(&m_cmXml);
	m_pState	= new COCSInventoryState;
	m_pPluginState	= new COCSInventoryState;
	m_pSysInfo	= new CSysInfo( getAgentConfig()->isDebugRequired() >= OCS_DEBUG_MODE_TRACE, getDataFolder());

	m_Device.SetDeviceName( m_pDeviceid->getComputerName());
	// Get DeviceId from ocsinventoryFront
	m_Device.SetDeviceID( m_pDeviceid->getDeviceID() );

	/*****
	 *
	 *	Main inventory function
	 *
	 ****/
	// Get Device info
	if (!runInventory())
		return FALSE;

	// Read last inventory state from XML file
	m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Reading last inventory state"));
	csStateFile.Format( _T("%s\\%s"), getDataFolder(), OCS_LAST_STATE_FILE);
	if (!m_pState->ReadFromFile( csStateFile, OCS_STATE_STANDARD_SECTION))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to load/parse inventory state from file <%s>"), csStateFile);
	if (!m_pPluginState->ReadFromFile( csStateFile, OCS_STATE_PLUGIN_SECTION))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to load/parse plugin state from file <%s>"), csStateFile);
	return TRUE;
}

BOOL CInventoryRequest::final()
{
	BOOL bSuccess = FALSE;
	CString	csFilename;

	if (m_bNotify)
	{
		// Notify mode => Only send minimum network informations
		getXmlPointerRequest();
		// Save inventory state only for Network adapters
		m_pState->SetNetworks( m_NetworkList.GetHash());
		m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "INVENTORY => Network adapters inventory state changed"));
		// Create XML only to notify network inventory changes
		bSuccess = m_pTheDB->NotifyDeviceProperties( m_Device);
		bSuccess = bSuccess && m_pTheDB->NotifyNetworks( m_NetworkList);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => XML Notify %u Network Adapter(s)"), m_NetworkList.GetCount());
	}
	else
	{
		// Standard inventory => Send full inventory information
		// Check state to see if there is something changed
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Checking last inventory state"));
		if (isStateChanged())
			m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "INVENTORY => Inventory changed since last run"));
		else
			m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "INVENTORY => No change since last inventory"));

		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Generating XML document with Device properties"));
		getXmlPointerContent();
		// Update BIOS file
		bSuccess = bSuccess && m_pTheDB->UpdateBIOS( m_BIOS);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => XML Update BIOS"));
		// Update CPUs
		bSuccess = m_pTheDB->UpdateCPUs( m_CpuList);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => XML Update %u CPU(s)"), m_CpuList.GetCount());
		// Update Memory slots
		bSuccess = m_pTheDB->UpdateMemorySlots( m_MemoryList);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => XML Update %u Logical Drive(s)"), m_MemoryList.GetCount());
		// Update Input Devices
		bSuccess = bSuccess && m_pTheDB->UpdateInputDevices( m_InputList);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => XML Update %u Input Device(s)"), m_InputList.GetCount());
		// Update System Ports
		bSuccess = bSuccess && m_pTheDB->UpdateSystemPorts( m_PortList);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => XML Update %u System Port(s)"), m_PortList.GetCount());
		// Update System Controllers
		bSuccess = bSuccess && m_pTheDB->UpdateSystemControllers( m_SystemControllerList);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => XML Update %u System Controler(s)"), m_SystemControllerList.GetCount());
		// Update System Slots
		bSuccess = bSuccess && m_pTheDB->UpdateSystemSlots( m_SlotList);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => XML Update %u System Slot(s)"), m_SlotList.GetCount());
		// Update Sounds
		bSuccess = bSuccess && m_pTheDB->UpdateSounds( m_SoundList);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => XML Update %u Sound Device(s)"), m_SoundList.GetCount());
		// Update Storages
		bSuccess = bSuccess && m_pTheDB->UpdateStorages( m_StorageList);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => XML Update %u Storage Peripheral(s)"), m_StorageList.GetCount());
		// Update Logical Drives
		bSuccess = bSuccess && m_pTheDB->UpdateDrives( m_DriveList);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => XML Update %u Logical Drive(s)"), m_DriveList.GetCount());
		// Update Modems
		bSuccess = bSuccess && m_pTheDB->UpdateModems( m_ModemList);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => XML Update %u Modem(s)"), m_ModemList.GetCount());
		// Update Networks
		bSuccess = bSuccess && m_pTheDB->UpdateNetworks( m_NetworkList);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => XML Update %u Network Adapter(s)"), m_NetworkList.GetCount());
		// Update Videos
		bSuccess = bSuccess && m_pTheDB->UpdateVideos( m_VideoList);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => XML Update %u Video Adapter(s)"), m_VideoList.GetCount());
		// Update Monitors
		bSuccess = bSuccess && m_pTheDB->UpdateMonitors( m_MonitorList);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => XML Update %u Monitor(s)"), m_MonitorList.GetCount());
		// Update Printers
		bSuccess = bSuccess && m_pTheDB->UpdatePrinters( m_PrinterList);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => XML Update %u Printer(s)"), m_PrinterList.GetCount());
		// Update Softwares
		if (!getAgentConfig()->isNoSoftwareRequired())
		{
			bSuccess = bSuccess && m_pTheDB->UpdateSoftwares( m_SoftwareList);
			m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => XML Update %u Software"), m_SoftwareList.GetCount());
		}
		// Update Registry values
		bSuccess = bSuccess && m_pTheDB->UpdateRegistryValues( m_RegistryList);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => XML Update %u Registry Value(s)"), m_RegistryList.GetCount());
		// Update Administrative Informations
		csFilename.Format( _T("%s\\%s"), getDataFolder(), OCS_ACCOUNTINFO_FILENAME);
		bSuccess = bSuccess && m_pTheDB->UpdateAccountInfo( csFilename);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => XML Update Administrative Information(s)"));
		// Update Hardware 
		bSuccess = bSuccess && m_pTheDB->UpdateDeviceProperties( m_Device);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => XML Update common Device properties"));
	}

	if (!bSuccess)
		m_pLogger->log( LOG_PRIORITY_ERROR, _T( "INVENTORY => XML Update Device properties failed"));
	bSuccess = bSuccess && CRequestAbstract::final();
	return bSuccess;
}

BOOL CInventoryRequest::writeLastInventoryState()
{
	// Write inventory state
	m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Writing new inventory state"));
	CString csFileName;
	csFileName.Format( _T( "%s\\%s"), getDataFolder(), OCS_LAST_STATE_FILE);
	if (!m_pState->WriteToFile( csFileName, OCS_STATE_STANDARD_SECTION))
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to write new inventory state to file <%s>"), csFileName);
		return FALSE;
	}
	if (!m_pPluginState->WriteToFile( csFileName, OCS_STATE_PLUGIN_SECTION))
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to write new plugin state to file <%s>"), csFileName);
		return FALSE;
	}
	return TRUE;

}

void CInventoryRequest::setTag(CString csTag)
{
	m_csTag = csTag;
}

BOOL CInventoryRequest::isStateChanged()
{
	CString				csHash, csPlugHash;
	ULONG				ulChecksum = 0;
	// BIOS informations from EXE/VBS plugin
	CBios				m_plugBIOS;
	// List of CPU infos from EXE/VBS plugin
	CCpuList			m_plugCpuList;
	// List of Input devices infos from EXE/VBS plugin
	CInputDeviceList	m_plugInputList;
	// List of Logical Drives (Volum name, File system, Total size, Free space...) from EXE/VBS plugin
	CLogicalDriveList	m_plugDriveList;
	// List of Memory slots from EXE/VBS plugin
	CMemorySlotList		m_plugMemoryList;
	// List of Modems from EXE/VBS plugin
	CModemList			m_plugModemList;
	// List of Monitors from EXE/VBS plugin
	CMonitorList		m_plugMonitorList;
	// List of Network adapters from EXE/VBS plugin
	CNetworkAdapterList m_plugNetworkList;
	// List of Printers from EXE/VBS plugin
	CPrinterList		m_plugPrinterList;
	// List of Device registry values infos from EXE/VBS plugin
	CRegistryValueList	m_plugRegistryList;
	// List of Software from EXE/VBS plugin
	CSoftwareList		m_plugSoftwareList;
	// List of Sound Devices from EXE/VBS plugin
	CSoundDeviceList	m_plugSoundList;
	// List of Storage devices (Floppy disk, IDE Disk, SCSI Disk, CD-Rom...) from EXE/VBS plugin
	CStoragePeripheralList m_plugStorageList;
	// List of System Controlers (Floppy, IDE, SCSI, USB, IEEE1394, PCMCIA, SATA) from EXE/VBS plugin
	CSystemControllerList m_plugSystemControllerList;
	// List of System Slots (PCI, AGP...) from EXE/VBS plugin
	CSystemSlotList		m_plugSlotList;
	// List of ports from EXE/VBS plugin
	CSystemPortList		m_plugPortList;
	// List of Video adapters from EXE/VBS plugin
	CVideoAdapterList	m_plugVideoList;

	// Checking if hardware changes
	csHash = m_Device.GetHash();
	if (csHash.CompareNoCase( m_pState->GetHardware()) != 0)
	{
		// Changed
		ulChecksum += OCS_CHECKSUM_HARDWARE;
		m_pState->SetHardware( csHash);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Hardware inventory state changed"));
	}
	// Checking if bios changes
	csHash = m_BIOS.GetHash();
	getXmlPointerContent();
	if (m_pTheDB->ReadBIOS( m_plugBIOS))
		csPlugHash = m_plugBIOS.GetHash();
	else
		csPlugHash.Empty();
	if ((csHash.CompareNoCase( m_pState->GetBios()) != 0) ||
		(csPlugHash.CompareNoCase( m_pPluginState->GetBios()) != 0))
	{
		// Changed
		ulChecksum += OCS_CHECKSUM_BIOS;
		m_pState->SetBios( csHash);
		m_pPluginState->SetBios( csPlugHash);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Bios inventory state changed"));
	}
	// Checking if memories changes
	csHash = m_MemoryList.GetHash();
	getXmlPointerContent();
	if (m_pTheDB->ReadMemorySlots( m_plugMemoryList))
		csPlugHash = m_plugMemoryList.GetHash();
	else
		csPlugHash.Empty();
	if ((csHash.CompareNoCase( m_pState->GetMemories()) != 0) ||
		(csPlugHash.CompareNoCase( m_pPluginState->GetMemories()) != 0))
	{
		// Changed
		ulChecksum += OCS_CHECKSUM_MEMORIES;
		m_pState->SetMemories( csHash);
		m_pPluginState->SetMemories( csPlugHash);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Memory slots inventory state changed"));
	}
	// Checking if slots changes
	csHash = m_SlotList.GetHash();
	getXmlPointerContent();
	if (m_pTheDB->ReadSystemSlots( m_plugSlotList))
		csPlugHash = m_plugSlotList.GetHash();
	else
		csPlugHash.Empty();
	if ((csHash.CompareNoCase( m_pState->GetSlots()) != 0) ||
		(csPlugHash.CompareNoCase( m_pPluginState->GetSlots()) != 0))
	{
		// Changed
		ulChecksum += OCS_CHECKSUM_SLOTS;
		m_pState->SetSlots( csHash);
		m_pPluginState->SetSlots( csPlugHash);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => System slots inventory state changed"));
	}
	// Checking if registry changes
	csHash = m_RegistryList.GetHash();
	getXmlPointerContent();
	if (m_pTheDB->ReadRegistryValues( m_plugRegistryList))
		csPlugHash = m_plugRegistryList.GetHash();
	else
		csPlugHash.Empty();
	if ((csHash.CompareNoCase( m_pState->GetRegistry()) != 0) ||
		(csPlugHash.CompareNoCase( m_pPluginState->GetRegistry()) != 0))
	{
		// Changed
		ulChecksum += OCS_CHECKSUM_REGISTRY;
		m_pState->SetRegistry( csHash);
		m_pPluginState->SetRegistry( csPlugHash);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Registry inventory state changed"));
	}
	// Checking if controllers changes
	csHash = m_SystemControllerList.GetHash();
	getXmlPointerContent();
	if (m_pTheDB->ReadSystemControllers( m_plugSystemControllerList))
		csPlugHash = m_plugSystemControllerList.GetHash();
	else
		csPlugHash.Empty();
	if ((csHash.CompareNoCase( m_pState->GetControllers()) != 0) ||
		(csPlugHash.CompareNoCase( m_pPluginState->GetControllers()) != 0))
	{
		// Changed
		ulChecksum += OCS_CHECKSUM_CONTROLLERS;
		m_pState->SetControllers( csHash);
		m_pPluginState->SetControllers( csPlugHash);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => System controllers inventory state changed"));
	}
	// Checking if monitors changes
	csHash = m_MonitorList.GetHash();
	getXmlPointerContent();
	if (m_pTheDB->ReadMonitors( m_plugMonitorList))
		csPlugHash = m_plugMonitorList.GetHash();
	else
		csPlugHash.Empty();
	if ((csHash.CompareNoCase( m_pState->GetMonitors()) != 0) ||
		(csPlugHash.CompareNoCase( m_pPluginState->GetMonitors()) != 0))
	{
		// Changed
		ulChecksum += OCS_CHECKSUM_MONITORS;
		m_pState->SetMonitors( csHash);
		m_pPluginState->SetMonitors( csPlugHash);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Monitors inventory state changed"));
	}
	// Checking if ports changes
	csHash = m_PortList.GetHash();
	getXmlPointerContent();
	if (m_pTheDB->ReadSystemPorts( m_plugPortList))
		csPlugHash = m_plugPortList.GetHash();
	else
		csPlugHash.Empty();
	if ((csHash.CompareNoCase( m_pState->GetPorts()) != 0) ||
		(csPlugHash.CompareNoCase( m_pPluginState->GetPorts()) != 0))
	{
		// Changed
		ulChecksum += OCS_CHECKSUM_PORTS;
		m_pState->SetPorts( csHash);
		m_pPluginState->SetPorts( csPlugHash);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => System ports inventory state changed"));
	}
	// Checking if storages changes
	csHash = m_StorageList.GetHash();
	getXmlPointerContent();
	if (m_pTheDB->ReadStorages( m_plugStorageList))
		csPlugHash = m_plugStorageList.GetHash();
	else
		csPlugHash.Empty();
	if ((csHash.CompareNoCase( m_pState->GetStorages()) != 0) ||
		(csPlugHash.CompareNoCase( m_pPluginState->GetStorages()) != 0))
	{
		// Changed
		ulChecksum += OCS_CHECKSUM_STORAGES;
		m_pState->SetStorages( csHash);
		m_pPluginState->SetStorages( csPlugHash);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Storage peripherals inventory state changed"));
	}
	// Checking if drives changes
	csHash = m_DriveList.GetHash();
	getXmlPointerContent();
	if (m_pTheDB->ReadDrives( m_plugDriveList))
		csPlugHash = m_plugDriveList.GetHash();
	else
		csPlugHash.Empty();
	if ((csHash.CompareNoCase( m_pState->GetDrives()) != 0) ||
		(csPlugHash.CompareNoCase( m_pPluginState->GetDrives()) != 0))
	{
		// Changed
		ulChecksum += OCS_CHECKSUM_DRIVES;
		m_pState->SetDrives( csHash);
		m_pPluginState->SetDrives( csPlugHash);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Logical drives inventory state changed"));
	}
	// Checking if inputs changes
	csHash = m_InputList.GetHash();
	getXmlPointerContent();
	if (m_pTheDB->ReadInputDevices( m_plugInputList))
		csPlugHash = m_plugInputList.GetHash();
	else
		csPlugHash.Empty();
	if ((csHash.CompareNoCase( m_pState->GetInputs()) != 0) ||
		(csPlugHash.CompareNoCase( m_pPluginState->GetInputs()) != 0))
	{
		// Changed
		ulChecksum += OCS_CHECKSUM_INPUTS;
		m_pState->SetInputs( csHash);
		m_pPluginState->SetInputs( csPlugHash);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Input peripherals inventory state changed"));
	}
	// Checking if modems changes
	csHash = m_ModemList.GetHash();
	getXmlPointerContent();
	if (m_pTheDB->ReadModems( m_plugModemList))
		csPlugHash = m_plugModemList.GetHash();
	else
		csPlugHash.Empty();
	if ((csHash.CompareNoCase( m_pState->GetModems()) != 0) ||
		(csPlugHash.CompareNoCase( m_pPluginState->GetModems()) != 0))
	{
		// Changed
		ulChecksum += OCS_CHECKSUM_MODEMS;
		m_pState->SetModems( csHash);
		m_pPluginState->SetModems( csPlugHash);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Modems inventory state changed"));
	}
	// Checking if networks changes
	csHash = m_NetworkList.GetHash();
	getXmlPointerContent();
	if (m_pTheDB->ReadNetworks( m_plugNetworkList))
		csPlugHash = m_plugNetworkList.GetHash();
	else
		csPlugHash.Empty();
	if ((csHash.CompareNoCase( m_pState->GetNetworks()) != 0) ||
		(csPlugHash.CompareNoCase( m_pPluginState->GetNetworks()) != 0))
	{
		// Changed
		ulChecksum += OCS_CHECKSUM_NETWORKS;
		m_pState->SetNetworks( csHash);
		m_pPluginState->SetNetworks( csPlugHash);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Network adapters inventory state changed"));
	}
	// Checking if printers changes
	csHash = m_PrinterList.GetHash();
	getXmlPointerContent();
	if (m_pTheDB->ReadPrinters( m_plugPrinterList))
		csPlugHash = m_plugPrinterList.GetHash();
	else
		csPlugHash.Empty();
	if ((csHash.CompareNoCase( m_pState->GetPrinters()) != 0) ||
		(csPlugHash.CompareNoCase( m_pPluginState->GetPrinters()) != 0))
	{
		// Changed
		ulChecksum += OCS_CHECKSUM_PRINTERS;
		m_pState->SetPrinters( csHash);
		m_pPluginState->SetPrinters( csPlugHash);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Printers inventory state changed"));
	}
	// Checking if sounds changes
	csHash = m_SoundList.GetHash();
	getXmlPointerContent();
	if (m_pTheDB->ReadSounds( m_plugSoundList))
		csPlugHash = m_plugSoundList.GetHash();
	else
		csPlugHash.Empty();
	if ((csHash.CompareNoCase( m_pState->GetSounds()) != 0) ||
		(csPlugHash.CompareNoCase( m_pPluginState->GetSounds()) != 0))
	{
		// Changed
		ulChecksum += OCS_CHECKSUM_SOUNDS;
		m_pState->SetSounds( csHash);
		m_pPluginState->SetSounds( csPlugHash);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Sound adapters inventory state changed"));
	}
	// Checking if videos changes
	csHash = m_VideoList.GetHash();
	getXmlPointerContent();
	if (m_pTheDB->ReadVideos( m_plugVideoList))
		csPlugHash = m_plugVideoList.GetHash();
	else
		csPlugHash.Empty();
	if ((csHash.CompareNoCase( m_pState->GetVideos()) != 0) ||
		(csPlugHash.CompareNoCase( m_pPluginState->GetVideos()) != 0))
	{
		// Changed
		ulChecksum += OCS_CHECKSUM_VIDEOS;
		m_pState->SetVideos( csHash);
		m_pPluginState->SetVideos( csPlugHash);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Video adapters inventory state changed"));
	}
	// Checking if softwares changes
	csHash = m_SoftwareList.GetHash();
	getXmlPointerContent();
	if (m_pTheDB->ReadSoftwares( m_plugSoftwareList))
		csPlugHash = m_plugSoftwareList.GetHash();
	else
		csPlugHash.Empty();
	if ((csHash.CompareNoCase( m_pState->GetSoftwares()) != 0) ||
		(csPlugHash.CompareNoCase( m_pPluginState->GetSoftwares()) != 0))
	{
		// Changed
		ulChecksum += OCS_CHECKSUM_SOFTWARES;
		m_pState->SetSoftwares( csHash);
		m_pPluginState->SetSoftwares( csPlugHash);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Software inventory state changed"));
	}
	// Checking if installed Virtual Machines changes
	csHash.Empty();
	if (csHash.CompareNoCase( m_pState->GetVirtualMachines()) != 0)
	{
		// Changed
		ulChecksum += OCS_CHECKSUM_VIRTUALMACHINES;
		m_pState->SetVirtualMachines( csHash);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => installed Virtual Machines inventory state changed"));
	}
	// Checking if CPUs changes
	csHash = m_CpuList.GetHash();
	getXmlPointerContent();
	if (m_pTheDB->ReadCPUs( m_plugCpuList))
		csPlugHash = m_plugCpuList.GetHash();
	else
		csPlugHash.Empty();
	if ((csHash.CompareNoCase( m_pState->GetCPUs()) != 0) ||
		(csPlugHash.CompareNoCase( m_pPluginState->GetCPUs()) != 0))
	{
		// Changed
		ulChecksum += OCS_CHECKSUM_CPUS;
		m_pState->SetCPUs( csHash);
		m_pPluginState->SetCPUs( csPlugHash);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => CPU inventory state changed"));
	}
	m_Device.SetChecksum( ulChecksum);
	// if change
	if (ulChecksum)
		return TRUE;
	return FALSE;
}

/* Add ocs packages to a dedicated section */
BOOL CInventoryRequest::loadDownloadHistory()
{
	CFilePackageHistory  fileHistory;
	CString		csBuffer;
	short		errCnt			= 0;
	BOOL		historyOpened	= FALSE;

	while( errCnt<3 )
	{
		if (fileHistory.Open( getPackageHistoryFilename()))
		{
			historyOpened = TRUE;
			break;
		}
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Cannot open Download Package history file, retrying in 500ms"));
		Sleep(500);
		errCnt++;
	}

	getXmlPointerContent();
	m_cmXml.AddElem( _T("DOWNLOAD"));
	m_cmXml.AddElem( _T("HISTORY"));

	if( historyOpened )
	{
		while( fileHistory.ReadPackage( csBuffer))
		{
			// Chomp the string
			m_pLogger->log( LOG_PRIORITY_DEBUG, _T("INVENTORY => Adding Download Package <%s> to report"), csBuffer);
			m_cmXml.AddElem( _T("PACKAGE"));
			m_cmXml.AddAttrib( _T("ID"), csBuffer);
			m_cmXml.OutOfElem();
		}
		fileHistory.Close();
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CInventoryRequest::runInventory()
{
	CString		cs1, cs2, cs3, cs4, cs5;
	DWORD		dwValue;
	ULONG		ulPhysicalMemory, ulSwapSize;
	BOOL		bRunBiosInfo = TRUE;
	CSoftware	cSoftOS;

	// Get logged on user
	if (!m_pSysInfo->getUserName( cs1))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve logged on user"));
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Logged on user ID is <%s>"), cs1);
	m_Device.SetLoggedOnUser( cs1);
	// Last logged on user
	m_pSysInfo->getLastLoggedUser( cs1);
	m_Device.SetLastLoggedUser( cs1);
	// Get OS informations and device type (windows station or windows server)
	if (!m_pSysInfo->getOS( cs1, cs2, cs3, cs4, cs5))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve Operating System"));
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Operating System is <%s %s %s>, description <%s>"), 
					cs1, cs2, cs3, cs4);
	// Fix for some Windows 2000 pro without OS name
	if ((cs1.GetLength() == 0) && (cs2== _T( "5.0.2195")))
		cs1 = _T( "Microsoft Windows 2000 Professional");
	m_Device.SetOS( cs1, cs2, cs3);
	m_Device.SetDescription (cs4);
	// Get OS Address width
	m_Device.SetAddressWidthOS( m_pSysInfo->getAddressWidthOS());
	m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Operating System uses %u bits memory address width"),  m_pSysInfo->getAddressWidthOS());
	// Prepare to also store OS information to software list
	cSoftOS.Set( MICROSOFT_CORP_STRING, cs1, cs2, NOT_AVAILABLE, cs3, NOT_AVAILABLE, 0, TRUE);
	cSoftOS.SetInstallDate( cs5, TRUE);
	cSoftOS.SetMemoryAddressWidth( m_pSysInfo->getAddressWidthOS());
	// Get NT Domain or Workgroup
	if (!m_pSysInfo->getDomainOrWorkgroup( cs1))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve computer domain or workgroup"));
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Computer domain or workgroup is <%s>"), 
					cs1);
	m_Device.SetDomainOrWorkgroup( cs1);
	// Get NT user Domain
	if (!m_pSysInfo->getUserDomain( cs1))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve user domain"));
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => User domain is <%s>"), 
					cs1);
	m_Device.SetUserDomain( cs1);
	// Get BIOS informations
	if (!m_pSysInfo->getBiosInfo( &m_BIOS))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve BIOS"));
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => System Manufacturer <%s>, System Model <%s>, System S/N <%s>, Bios Manufacturer <%s>, Bios Date <%s>, Bios Version <%s>"),
					  m_BIOS.GetSystemManufacturer(), m_BIOS.GetSystemModel(), m_BIOS.GetSystemSerialNumber(),
					  m_BIOS.GetBiosManufacturer(), m_BIOS.GetBiosDate(), m_BIOS.GetBiosVersion());
	// Get Processor infos (0 means error)
	if (!(dwValue = m_pSysInfo->getProcessors( cs1, cs2, &m_CpuList)))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve processor(s)"));
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => %lu processor(s) %s at %s MHz"),
					  dwValue, cs1, cs2);
	m_Device.SetProcessor( cs1, cs2, dwValue);
	// Get memory informations
	if (!m_pSysInfo->getMemory( &ulPhysicalMemory, &ulSwapSize))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve memory informations from OS"));
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => OS Memory %lu MB, OS Swap size %lu MB"),
					  ulPhysicalMemory, ulSwapSize);
	m_Device.SetMemory( ulPhysicalMemory, ulSwapSize);
	if (!m_pSysInfo->getMemorySlots( &m_MemoryList))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve memory slots"));
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => %d memory slot(s) found"),
					  m_MemoryList.GetCount());
	// Get Input Devices
	if (!m_pSysInfo->getInputDevices( &m_InputList))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve input devices"));
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => %d input device(s) found"),
					  m_InputList.GetCount());
	// Get System ports
	if (!m_pSysInfo->getSystemPorts( &m_PortList))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve system ports"));
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => %d system port(s) found"),
					  m_PortList.GetCount());
	// Get System Slots
	if (!m_pSysInfo->getSystemSlots( &m_SlotList))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve system slots"));
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => %d system slot(s) found"),
					  m_SlotList.GetCount());
	// Get System controlers
	if (!m_pSysInfo->getSystemControllers( &m_SystemControllerList))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve system controlers"));
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => %d system controler(s) found"),
					  m_SystemControllerList.GetCount());
	// Get Physical storage devices
	if (!m_pSysInfo->getStoragePeripherals( &m_StorageList))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve storage peripherals"));
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => %d storage peripheral(s) found"),
					  m_StorageList.GetCount());
	// Get Logical Drives
	if (!m_pSysInfo->getLogicalDrives( &m_DriveList))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve logical drives"));
	// Get Sound Devices
	if (!m_pSysInfo->getSoundDevices( &m_SoundList))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve sound devices"));
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => %d sound device(s) found"),
					  m_SoundList.GetCount());
	// Get Modems
	if (!m_pSysInfo->getModems( &m_ModemList))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve modems"));
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => %d modem(s) found"),
					  m_ModemList.GetCount());
	// Get network adapter(s) hardware and IP informations
	if (!m_pSysInfo->getNetworkAdapters( &m_NetworkList))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve network adapters"));
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => %d network adapter(s) found"),
					  m_NetworkList.GetCount());
	// Get Printer(s) informations
	if (!m_pSysInfo->getPrinters( &m_PrinterList))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve system printers"));
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => %d system printer(s) found"),
					  m_PrinterList.GetCount());
	// Get Video adapter(s) informations
	if (!m_pSysInfo->getVideoAdapters( &m_VideoList))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve video adapters"));
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => %d video adapter(s) found"),
					  m_VideoList.GetCount());
	if (!m_pSysInfo->getMonitors( &m_MonitorList))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve system monitors"));
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => %d system monitor(s) found"),
					  m_MonitorList.GetCount());
	// Get the primary local IP Address 
	m_Device.SetIPAddress( m_pSysInfo->getLocalIP());
	m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Default IPv4 address is <%s>"),
				  m_Device.GetIPAddress());
	// Get Windows registration infos
	if (!m_pSysInfo->getWindowsRegistration( cs1, cs2, cs3))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve system registration"));
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Registered company <%s>, registered owner <%s>, Product ID <%.08s...>"),
					  cs1, cs2, cs3);
	m_Device.SetWindowsRegistration( cs1, cs2, cs3);
	// Get Windows product key
	if (!m_pSysInfo->getWindowsProductKey( cs1))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve system product key"));
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Product key <%.08s...>"),
					  cs1);
	m_Device.SetWindowsProductKey( cs1);
	// Get Software list
	if (!m_pSysInfo->getInstalledApplications( &m_SoftwareList, getAgentConfig()->isHkcuRequired()))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve software from Registry"));
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => %d software found"),
						m_SoftwareList.GetCount());
	// Add OS to the list of detected software
	m_SoftwareList.AddTail( cSoftOS);
	// Get UUID
	if (!m_pSysInfo->getUUID( cs1))
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "INVENTORY => Failed to retrieve computer/VM UUID"));
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Computer/VM UUID is <%s>"), cs1);
	m_Device.SetUUID( cs1);
	// Check if we're running on VM
	CVMSystem myVM;
	if (!myVM.DetectVM( &m_BIOS, cs1) && !myVM.DetectVM( &m_SystemControllerList, cs1) && 
		!myVM.DetectVM( &m_StorageList, cs1) && !myVM.DetectVM( &m_VideoList, cs1))
	{
		cs1 = VMSYSTEM_PHYSYCAL;
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Computer seems to be physical host"));
	}
	else
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Computer seems to be %s virtual host"), cs1);
	m_Device.SetVMSystem( cs1);
	// Verify total system memory
	ULONG ulMemTotal = m_MemoryList.GetTotalMemory();
	if (ulMemTotal > 0)
	{	
		ULONG ulMemStart = m_Device.GetPhysicalMemory();
		ULONG ulMemToSet = ulMemStart;
		ULONG ulDiff = abs( (long)(ulMemTotal - ulMemStart));

		// Try to check if part of memory is used by video, either less than 32 MB or less than 10%
		if ((ulDiff < 32 ) || ((ulDiff*100/ulMemTotal) < 10)) 
		{			
			m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => System Memory set to %lu (instead of %lu)"), ulMemTotal, ulMemStart);
			ulMemToSet = ulMemTotal;
		}
		else
			m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "INVENTORY => Bogus summed Memory Slots, %lu is too far from %lu (keeping the last one)"), ulMemTotal, ulMemStart);
		
		m_Device.SetMemory( ulMemToSet, m_Device.GetPageFileSize());
	}
	return TRUE;
}

LONG CInventoryRequest::SearchFilesInDirectory(LPCTSTR lpstrDir, LPCTSTR lpstrExtensionToSearch, LPCTSTR lpstrFoldersToExclude)
{
	CString		csDir = lpstrDir;
	CFileFind	cFinder;
	BOOL		bWorking;
	CSoftware	cApp;
	CString		csPublisher,
				csName,
				csVersion,
				csComment,
				csLanguage;
	LONG		lNumberOfFiles = 0;

	// Search for all files and directory
	csDir += "*.*";
	bWorking = cFinder.FindFile( csDir);
	while (bWorking)
	{
		bWorking = cFinder.FindNextFile();
		if (!cFinder.IsDots())
		{
			// Not "." or ".." directory
			if (cFinder.IsDirectory())
			{
				// This is a directory => recursive search if needed
				if (IsExcludeFolder( cFinder.GetFilePath(), lpstrFoldersToExclude))
				{
					// Folder to exclude from search => skip
					m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "Skipping folder <%s>"), cFinder.GetFilePath());
				}
				else
				{
					csDir = cFinder.GetFilePath() + _T( "\\");
					SearchFilesInDirectory( csDir);
				}
			}
			else
			{
				// This a file => update total files number for this Device
				lNumberOfFiles ++;
				if (IsExtensionToSearch( cFinder.GetFileName(), lpstrExtensionToSearch))
				{
					// This is an apps to search
					if (!getFileVersion( cFinder.GetFilePath(), csPublisher, csName, csVersion, csComment, csLanguage))
						// Unable to get version infos => use filename
						csName = cFinder.GetFileName();
					StrForSQL( csName);
					if (csName.IsEmpty())
						// Version info do not contains app name => use filename
						csName = cFinder.GetFileName();
					cApp.Clear();
					cApp.Set( csPublisher, csName, csVersion, cFinder.GetRoot(), csComment, cFinder.GetFileName(), cFinder.GetLength());
					cApp.SetLanguage(csLanguage);
					m_SoftwareList.AddTail( cApp);
				}
			}
		}
	}
	return lNumberOfFiles;
}

BOOL CInventoryRequest::IsExtensionToSearch( LPCTSTR lpstrFilename, LPCTSTR lpstrExtension)
{
	CString	csFile = lpstrFilename,
			csExt,
			csData,
			csTemp,
			csBuffer = lpstrExtension;
	int		nPos;

	if ((lpstrExtension == NULL) || (_tcslen( lpstrExtension) == 0))
		return FALSE;
	csFile.MakeLower();
	if ((nPos = csFile.ReverseFind( '.')) == -1)
		return FALSE;
	csExt = csFile.Mid( nPos+1);
	while ((nPos = csBuffer.Find(_T( ","))) >= 0)
	{
		csData = csBuffer.Left( nPos);
		if (csExt.Compare( csData) == 0)
			return TRUE;
		csTemp = csBuffer.Mid( nPos + 1);
		csBuffer = csTemp;
	}
	if (csExt.Compare( csBuffer) == 0)
		return TRUE;
	return FALSE;
}

BOOL CInventoryRequest::IsExcludeFolder( LPCTSTR lpstrFolder, LPCTSTR lpstrFolderToSkip)
{
	CString	csFolder = lpstrFolder,
			csData,
			csTemp,
			csBuffer = lpstrFolderToSkip;
	int		nPos;


	if ((lpstrFolderToSkip == NULL) || (_tcslen( lpstrFolderToSkip) == 0))
		return FALSE;
	csFolder.MakeLower();
	while ((nPos = csBuffer.Find(_T( ","))) >= 0)
	{
		csData = csBuffer.Left( nPos);
		if (csFolder.Find( csData) >= 0)
			return TRUE;
		csTemp = csBuffer.Mid( nPos + 1);
		csBuffer = csTemp;
	}
	if (csFolder.Find( csBuffer) >= 0)
		return TRUE;
	return FALSE;
}