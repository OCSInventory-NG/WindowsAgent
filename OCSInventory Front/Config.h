//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Config.h: interface for the CConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIG_H__2BEC077B_DAE8_408E_875C_79A2E06A33EB__INCLUDED_)
#define AFX_CONFIG_H__2BEC077B_DAE8_408E_875C_79A2E06A33EB__INCLUDED_

#pragma once

// Folder where Agent stores data, under CSIDL_COMMON_APPDATA
// (usually C:\Documents and settings\All users\Application data)
#define OCSNG_COMMON_APPDATA	_T( "\\OCS Inventory NG\\Agent")
// Default configuration file
#define OCS_CONFIG_FILENAME		_T( "ocsinventory.ini")
// Section for Agent parameters
#define OCS_AGENT_SECTION		_T( "OCS Inventory Agent")
// Command line option used to save settings
#define OCS_AGENT_SAVE_SETTINGS	_T( "save_conf")
// Debug mode
#define OCS_DEBUG_MODE_OFF		0
#define OCS_DEBUG_MODE_STANDARD	1
#define OCS_DEBUG_MODE_TRACE	2
#define	OCS_ACCOUNTINFO_FILENAME _T( "admininfo.conf")
// Section for Service parameters
#define	OCS_SERVICE_SECTION		_T( "OCS Inventory Service")
// Current delay between 2 agent launch
#define OCS_SERVICE_PROLOG_FREQ	 _T( "PROLOG_FREQ")
// OCS INVENTORY ON STARTUP FEATURE
#define	OCS_SERVICE_INVENTORY_ON_STARTUP _T( "INVENTORY_ON_STARTUP")
// Set to 0 to display Systray icon, otherwise systray icon hidden
#define OCS_SERVICE_HIDDEN		_T( "Hide")
// Default communication provider DLL, based on libcurl
#define OCS_DEFAULT_PROVIDER	_T( "ComHTTP.dll")
// Message to service for running inventory now
#define	OCS_SERVICE_CONTROL_RUN_INVENTORY_NOW	128
// Message to service for displaying inventory content using XSLT
#define OCS_SERVICE_CONTROL_SHOW_INVENTORY		129


class CConfig;

#include "OCSInventory Front Classes.h"
#include "Singleton.h"

class OCSINVENTORYFRONT_API CConfig : public CSingleton<CConfig>
{
	friend class CSingleton<CConfig>;

public: // Methods

	//Standards constructrs and destructors
	CConfig();
	virtual ~CConfig();

	// Reset default values
	void Clear();

	// Set Agent version
	void setVersion( LPCTSTR lpstrVersion);
	// Return Agent version
	LPCTSTR getVersion();

	// Load configuration from specified file (default agent config file used if NULL)
	BOOL load( LPCTSTR lpstrFile = NULL);
	// Save configuration to specified file (default agent config file used if NULL)
	BOOL save( LPCTSTR lpstrFile = NULL);

	// Get current config file (/CONF:"path_to_file")
	LPCTSTR getConfigFile();
	// Is debug mode enabled (/DEBUG)
	// (0 => disabled, 1 => light debug, 2=> trace all witjh sysinfo calls)
	UINT isDebugRequired();
	// Does agent create an XML file for results (/XML)
	BOOL isXmlRequired();
	// Get path to folder to store XML file (/XML[:"path_to_folder"])
	LPCTSTR getXmlFolder();
	// Is agent launched in local mode, wothout connection to server (/LOCAL)
	BOOL isLocalRequired();
	// In local mode, get path to folder to store .ocs file (/LOCAL[:"path_to_folder"])
	LPCTSTR getLocalInventoryFolder();
	// Is IpDiscover forced with /IPDISC
	BOOL isIpDiscoverRequired();
	// Get network to launch IpDiscover /IPDISC:network
	LPCTSTR getIpDiscoverNetwork();
	// Is IpDiscover latency forced with /IPDISC_LAT:ms
	BOOL isIpDiscoverLatencyProvided();
	// Get IpDiscover latency provided by /IPDISC_LAT:ms
	LPCTSTR getIpDiscoverLatency();
	// Does agent launches a fast IpDiscover scan, without taking care of network load, or not
	BOOL isFastIpRequired();
	// Does we have to generate a new Device Unique ID (/UID)
	BOOL isNewUidRequired();
	// Is NOTIFY mode required
	BOOL isNotifyRequired();
	// Does we disable software inventory (/NOSOFTWARE)
	BOOL isNoSoftwareRequired();
	// Does we disable software inventory (/NOTAG)
	BOOL isNoTagRequired();
	// What's the content of TAG (/TAG:"tag value")
	BOOL isTagTextProvided();
	LPCTSTR getTagText();
	// Does agent also scan HKEY_CURRENT_USER hive for printers and software (/HKCU)
	BOOL isHkcuRequired();
	// Is Inventory forced (/FORCE)
	BOOL isForceInventoryRequired();
	// Get Communication Provider DLL
	LPCTSTR getCommunicationProvider();

	// Enable pr not debugging mode
	void setDebugRequired( UINT uDebug = TRUE);
	// Set path to folder to store XML file (/XML[:"path_to_folder"])
	// if lpstrFolder NULL, useagent's setup folder
	void setXmlFolder( LPCTSTR lpstrFolder = NULL);
	// Enable local inventory mode, without contact to server
	// Set path to folder to store XML compressed file (/LOCAL[:"path_to_folder"])
	// if lpstrFolder NULL, use agent's setup folder
	void setLocalInventory( LPCTSTR lpstrFolder = NULL);
	// Does we disable software inventory (/NOSOFTWARE)
	void setNoSoftwareRequired( BOOL bNoSoftware = TRUE);
	// Does we disable prompting user for TAG (/NOTAG)
	void setNoTagRequired( BOOL bNoTag = TRUE);
	// Set the content of TAG (/TAG:"tag value")
	void setTagText( LPCTSTR lpstrText = NULL);
	// Force inventory to be sent (/FORCE)
	void setForceInventoryRequired( BOOL bForce = TRUE);
	// Does we have to generate a new Device Unique ID (/UID)
	void setNewUID( BOOL bNew = TRUE);
	// Enable agent to notify inventory change of important information
	void setNotify( BOOL bNotify = FALSE);
	// Set if IpDiscover force for network with /IPDISC:network
	void setIpDiscoverRequired(LPCTSTR lpstrNetwork = NULL);
	// Set if IpDiscover latency is specified
	void setIpDiscoverLatency(LPCTSTR lpstrLatency = NULL);
	// Does agent launches a fast IpDiscover scan, without taking care of network load, or not
	void setFastIpRequired( BOOL bFast = TRUE);
	// Set if agent also scan HKEY_CURRENT_USER hive for printers and software (/HKCU)
	void setHkcuRequired( BOOL bHKCU = TRUE);
	// Set Communication Provider DLL to use without extension .DLL
	void setCommunicationProvider( LPCTSTR lpstrDll);

	// Write Administrative info to admininfo.conf hidden file in agent install folder
	BOOL writeAccountInfos( LPCTSTR lpstrName, LPCTSTR lpstrValue);

private: //Methods
	void setConfigFile( LPCTSTR lpstrFile = NULL);

private: //Attributes
	BOOL	m_bForce;	// Is /FORCE enabled to force sending inventory
	UINT	m_uDebug;	// Enable debugging mode (0 => disabled, 1 => light debug, 2=> trace all witjh sysinfo calls) 
	CString	m_csLocal;	// Is /LOCAL:path_to_file enabled to create local XML compressed file (do not contact server)
	CString	m_csXml;	// Is /XML:path_to_file enabled to create local XML file
	BOOL	m_bNoTag;	// Is /NOTAG enabled, to prevent asking user for TAG value
	BOOL	m_bNoSoftware;// Is /NOSOFTWARE enabled, to prevent software inventory
	CString m_csTag;	// Is /TAG:"tag value" enabled (yes = not empty, no = empty)
	BOOL	m_bHKCU;	// Is /HKCU enabled, to also scan HKEY_CURRENT_USER for printers and softwares
	BOOL	m_bUID;		// Is /UID enabled, to force generating a new Device Unique ID
	BOOL	m_bNotify;	// Is /NOTIFY enabled, to force notifying mode because service detects inventory changes
	CString	m_csConfigFile; // Is /CONF:"path_to_file" enabled, to specify path to config file
	CString m_csIpDisc; // Is /IPDISC:"NetworkNumber" enabled to force IP Discover for the specified network
	CString m_csIpDiscLat; // Is /IPDISC_LAT:"number_of_ms" enabled to force IP Discover latency
	BOOL	m_bFastIp;	// Is /FASTIP enabled, to disable network load contriol settings in IP Discover
	CString m_csComProvider; // What's the communication provider to use to contact server ?
	// Agent version
	CString m_csVersion;
};

#endif // !defined(AFX_CONFIG_H__2BEC077B_DAE8_408E_875C_79A2E06A33EB__INCLUDED_)