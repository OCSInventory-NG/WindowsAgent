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
// informations to XML.
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
	// Standard contructor/destructor
	//////////////////////////////////
	CXMLInteract(){};
	CXMLInteract( CMarkup* );
	virtual ~CXMLInteract();

	//////////////////////////////////
	// Operations
	//////////////////////////////////
	// Write BIOS in XML document
	// Return TRUE if they are successfully updated
	BOOL UpdateBIOS( CBios &myBios);
	// Write CPU in the XML document
	// Return TRUE if they are successfully updated
	BOOL UpdateCPUs( CCpuList &myCpuList);
	// Write logical drives in the XML document
	// Return TRUE if they are successfully updated
	BOOL UpdateDrives( CLogicalDriveList &myDriveList);
	// Write input devices in the XML document
	// Return TRUE if they are successfully updated
	BOOL UpdateInputDevices( CInputDeviceList &myInputList);
	// Write hardware info in XML document
	// Return TRUE if they are successfully updated
	BOOL UpdateDeviceProperties( CDeviceProperties &pPC);
	// Write hardware info in XML document for NOTIFY mode
	// Return TRUE if they are successfully updated
	BOOL NotifyDeviceProperties( CDeviceProperties &pPC);
	// Write memory slots in XML document
	// Return TRUE if they are successfully updated
	BOOL UpdateMemorySlots( CMemorySlotList &myMemoryList);
	// Write modems in XML document
	// Return TRUE if they are successfully updated
	BOOL UpdateModems( CModemList &myModemList);
	// Write monitors in XML document
	// Return TRUE if they are successfully updated
	BOOL UpdateMonitors( CMonitorList &myMonitorList);
	// Write network adapters in XML document
	// Return TRUE if they are successfully updated
	BOOL UpdateNetworks( CNetworkAdapterList &myNetworkList);
	// Write network adapters in XML document for NOTIFY mode
	// Return TRUE if they are successfully updated
	BOOL NotifyNetworks( CNetworkAdapterList &myNetworkList);
	// Write printers in XML document
	// Return TRUE if they are successfully updated
	BOOL UpdatePrinters( CPrinterList &myPrinterList);
	// Write registry values in XML document
	// Return TRUE if they are successfully updated
	BOOL UpdateRegistryValues( CRegistryValueList &myRegistryList);
	// Write system ports in XML document
	// Return TRUE if they are successfully updated
	BOOL UpdateSystemPorts( CSystemPortList &myPortList);
	// Write system slots in XML document
	// Return TRUE if they are successfully updated
	BOOL UpdateSystemSlots( CSystemSlotList &mySlotList);
	// Write softwares in XML document
	// Return TRUE if they are successfully updated
	BOOL UpdateSoftwares( CSoftwareList &mySoftwareList);
	// Write sounds devices in XML document
	// Return TRUE if they are successfully updated
	BOOL UpdateSounds( CSoundDeviceList &mySoundList);
	// Write storages peripherals in XML document
	// Return TRUE if they are successfully updated
	BOOL UpdateStorages( CStoragePeripheralList &myStorageList);
	// Write system controllers in XML document
	// Return TRUE if they are successfully updated
	BOOL UpdateSystemControllers( CSystemControllerList &mySystemControllerList);
	// Write video adapters in XML document
	// Return TRUE if they are successfully updated
	BOOL UpdateVideos( CVideoAdapterList &myVideoList);
	// Write Administrative informations from file in XML document
	// Return TRUE if they are successfully updated
	BOOL UpdateAccountInfo( LPCTSTR lpstrAccountFile);

protected: // Methods
	// Read Administrative informations from old INI format and add them to XML
	BOOL UpdateAccountInfoOldFormat( CMarkup *pXml, LPCTSTR lpstrAccountFile);
protected: // Attributes
	CMarkup *m_pXml;				// Pointer to xml of the inventory
	DWORD m_dwBiosMode;				// Biosinfo mode
};

#endif // !defined(AFX_XMLINTERACT_H__02015424_D9B0_11D4_8F75_00600889DFA5__INCLUDED_)
