//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// XMLInteract.h: interface for the CXMLInteract class.
//
// Storage class using CStdioFile class to load from and store
// information to XML.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLINTERACT_H__02015424_D9B0_11D4_8F75_00600889DFA5__INCLUDED_)
#define AFX_XMLINTERACT_H__02015424_D9B0_11D4_8F75_00600889DFA5__INCLUDED_

#pragma once

#include "SysInfo.h"
#include "OcsUtils.h"
#include "Markup.h"
#include "Config.h"

class CXMLInteract 
{
public: // Methods
	//////////////////////////////////
	// Standard constructor/destructor
	//////////////////////////////////
	CXMLInteract(){};
	CXMLInteract( CMarkup* );
	virtual ~CXMLInteract();

	//////////////////////////////////
	// Operations
	//////////////////////////////////
	// Read or Write BIOS in XML document
	// Return TRUE if they are successfully updated
	BOOL ReadBIOS( CBios &myBios);
	BOOL UpdateBIOS( CBios &myBios);
	// Read or Write CPU in the XML document
	// Return TRUE if they are successfully updated
	BOOL ReadCPUs( CCpuList &myCpuList);
	BOOL UpdateCPUs( CCpuList &myCpuList);
	// Read or Write logical drives in the XML document
	// Return TRUE if they are successfully updated
	BOOL ReadDrives( CLogicalDriveList &myDriveList);
	BOOL UpdateDrives( CLogicalDriveList &myDriveList);
	// Read or Write input devices in the XML document
	// Return TRUE if they are successfully updated
	BOOL ReadInputDevices( CInputDeviceList &myInputList);
	BOOL UpdateInputDevices( CInputDeviceList &myInputList);
	// Read or Write hardware info in XML document
	// Return TRUE if they are successfully updated
	BOOL ReadDeviceProperties( CDeviceProperties &pPC);
	BOOL UpdateDeviceProperties( CDeviceProperties &pPC);
	// Write hardware info in XML document for NOTIFY mode
	// Return TRUE if they are successfully updated
	BOOL NotifyDeviceProperties( CDeviceProperties &pPC);
	// Read or Write memory slots in XML document
	// Return TRUE if they are successfully updated
	BOOL ReadMemorySlots( CMemorySlotList &myMemoryList);
	BOOL UpdateMemorySlots( CMemorySlotList &myMemoryList);
	// Read or Write modems in XML document
	// Return TRUE if they are successfully updated
	BOOL ReadModems( CModemList &myModemList);
	BOOL UpdateModems( CModemList &myModemList);
	// Read or Write monitors in XML document
	// Return TRUE if they are successfully updated
	BOOL ReadMonitors( CMonitorList &myMonitorList);
	BOOL UpdateMonitors( CMonitorList &myMonitorList);
	// Read or Write network adapters in XML document
	// Return TRUE if they are successfully updated
	BOOL ReadNetworks( CNetworkAdapterList &myNetworkList);
	BOOL UpdateNetworks( CNetworkAdapterList &myNetworkList);
	// Write network adapters in XML document for NOTIFY mode
	// Return TRUE if they are successfully updated
	BOOL NotifyNetworks( CNetworkAdapterList &myNetworkList);
	// Read or Write printers in XML document
	// Return TRUE if they are successfully updated
	BOOL ReadPrinters( CPrinterList &myPrinterList);
	BOOL UpdatePrinters( CPrinterList &myPrinterList);
	// Read or Write registry values in XML document
	// Return TRUE if they are successfully updated
	BOOL ReadRegistryValues( CRegistryValueList &myRegistryList);
	BOOL UpdateRegistryValues( CRegistryValueList &myRegistryList);
	// Write system ports in XML document
	// Return TRUE if they are successfully updated
	BOOL ReadSystemPorts( CSystemPortList &myPortList);
	BOOL UpdateSystemPorts( CSystemPortList &myPortList);
	// Read or Write system slots in XML document
	// Return TRUE if they are successfully updated
	BOOL ReadSystemSlots( CSystemSlotList &mySlotList);
	BOOL UpdateSystemSlots( CSystemSlotList &mySlotList);
	// Read or Write softwares in XML document
	// Return TRUE if they are successfully updated
	BOOL ReadSoftwares( CSoftwareList &mySoftwareList);
	BOOL UpdateSoftwares( CSoftwareList &mySoftwareList);
	// Read or Write sounds devices in XML document
	// Return TRUE if they are successfully updated
	BOOL ReadSounds( CSoundDeviceList &mySoundList);
	BOOL UpdateSounds( CSoundDeviceList &mySoundList);
	// Read or Write storages peripherals in XML document
	// Return TRUE if they are successfully updated
	BOOL ReadStorages( CStoragePeripheralList &myStorageList);
	BOOL UpdateStorages( CStoragePeripheralList &myStorageList);
	// Read or Write system controllers in XML document
	// Return TRUE if they are successfully updated
	BOOL ReadSystemControllers( CSystemControllerList &mySystemControllerList);
	BOOL UpdateSystemControllers( CSystemControllerList &mySystemControllerList);
	// Read or Write video adapters in XML document
	// Return TRUE if they are successfully updated
	BOOL ReadVideos( CVideoAdapterList &myVideoList);
	BOOL UpdateVideos( CVideoAdapterList &myVideoList);
	// Write Administrative information from file in XML document
	// Return TRUE if they are successfully updated
	BOOL UpdateAccountInfo( LPCTSTR lpstrAccountFile);

protected: // Methods
	// Read Administrative information from old INI format and add them to XML
	BOOL UpdateAccountInfoOldFormat( CMarkup *pXml, LPCTSTR lpstrAccountFile);
protected: // Attributes
	CMarkup *m_pXml;				// Pointer to xml of the inventory
	DWORD m_dwBiosMode;				// Biosinfo mode
};

#endif // !defined(AFX_XMLINTERACT_H__02015424_D9B0_11D4_8F75_00600889DFA5__INCLUDED_)
