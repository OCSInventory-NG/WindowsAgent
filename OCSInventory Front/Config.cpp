//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Config.cpp: implementation of the CConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Config.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfig::CConfig()
{
	m_csVersion.Empty();
	// Load settings from config file
	load();
	// Initialize others settings not in config file
	m_bFastIp = FALSE;
	m_bForce = FALSE;
	m_bNoSoftware = FALSE;
	m_bUID = FALSE;
	m_bNotify = FALSE;
	m_csTag.Empty();
	m_csXml.Empty();
}

CConfig::~CConfig()
{
}

LPCTSTR CConfig::getVersion()
{
	return m_csVersion;
}

void CConfig::setVersion(LPCTSTR lpstrVersion)
{
	m_csVersion = lpstrVersion;
}

UINT CConfig::isDebugRequired()
{
	return m_uDebug;
}

BOOL CConfig::isFastIpRequired()
{
	return m_bFastIp;
}

BOOL CConfig::isForceInventoryRequired()
{
	return m_bForce;
}

BOOL CConfig::isHkcuRequired()
{
	return m_bHKCU;
}

BOOL CConfig::isLocalRequired()
{
	return !m_csLocal.IsEmpty();
}

LPCTSTR CConfig::getLocalInventoryFolder()
{
	return m_csLocal;
}

BOOL CConfig::isNoTagRequired()
{
	return m_bNoTag;
}

LPCTSTR CConfig::getTagText()
{
	return m_csTag;
}

BOOL CConfig::isTagTextProvided()
{
	return !m_csTag.IsEmpty();
}

BOOL CConfig::isNewUidRequired()
{
	return m_bUID;
}

BOOL CConfig::isNotifyRequired()
{
	return m_bNotify;
}

BOOL CConfig::isNoSoftwareRequired()
{
	return m_bNoSoftware;
}

LPCTSTR CConfig::getConfigFile()
{
	return m_csConfigFile;
}

BOOL CConfig::isIpDiscoverRequired()
{
	return !m_csIpDisc.IsEmpty();
}

LPCTSTR CConfig::getIpDiscoverNetwork()
{
	return m_csIpDisc;
}

BOOL CConfig::isIpDiscoverLatencyProvided()
{
	return !m_csIpDiscLat.IsEmpty();
}

LPCTSTR CConfig::getIpDiscoverLatency()
{
	return m_csIpDiscLat;
}

BOOL CConfig::isXmlRequired()
{
	return !m_csXml.IsEmpty();
}

LPCTSTR CConfig::getXmlFolder()
{
	return m_csXml;
}

LPCTSTR CConfig::getCommunicationProvider()
{
	return m_csComProvider;
}

BOOL CConfig::load( LPCTSTR lpstrFile)
{
	CString csBuffer;

	// First, set config file to load
	setConfigFile( lpstrFile);
	// Debug
	m_uDebug = (UINT)GetPrivateProfileInt( OCS_AGENT_SECTION, _T( "Debug"), FALSE, m_csConfigFile);
	// Local inventory mode, get path to folder to store .ocs file
	GetPrivateProfileString( OCS_AGENT_SECTION, _T( "Local"), _T( ""), m_csLocal.GetBuffer( 1024), 1024, m_csConfigFile);
	m_csLocal.ReleaseBuffer();
	//  Scanning HKEY_CURRENT_USER hive for printers and sofware
	m_bHKCU = (BOOL)GetPrivateProfileInt( OCS_AGENT_SECTION, _T( "HKCU"), FALSE, m_csConfigFile);
	//  Disable prompting user for TAG value
	m_bNoTag = (BOOL)GetPrivateProfileInt( OCS_AGENT_SECTION, _T( "NoTAG"), FALSE, m_csConfigFile);
	// Force IP Discover for the specified network
	GetPrivateProfileString( OCS_AGENT_SECTION, _T( "IpDisc"), _T( ""), m_csIpDisc.GetBuffer( 1024), 1024, m_csConfigFile);
	m_csIpDisc.ReleaseBuffer();
	// Communication provider to use
	GetPrivateProfileString( OCS_AGENT_SECTION, _T( "ComProvider"), OCS_DEFAULT_PROVIDER, csBuffer.GetBuffer( 1024), 1024, m_csConfigFile);
	csBuffer.ReleaseBuffer();
	setCommunicationProvider( csBuffer);
	return TRUE;
}

BOOL CConfig::save( LPCTSTR lpstrFile)
{
	CString csBuffer;
	BOOL	bResult;

	try
	{
		// First, set config file to load
		setConfigFile( lpstrFile);
		// Debug
		csBuffer.Format( _T( "%u"), m_uDebug);
		bResult = WritePrivateProfileString( OCS_AGENT_SECTION, _T( "Debug"), csBuffer, m_csConfigFile);
		// Local inventory mode, get path to folder to store .ocs file
		bResult = bResult && WritePrivateProfileString( OCS_AGENT_SECTION, _T( "Local"), m_csLocal, m_csConfigFile);
		//  Scanning HKEY_CURRENT_USER hive for printers and sofware
		csBuffer.Format( _T( "%u"), m_bHKCU);
		bResult = bResult && WritePrivateProfileString( OCS_AGENT_SECTION, _T( "HKCU"), csBuffer, m_csConfigFile);
		//  Disable prompting user for TAG value
		csBuffer.Format( _T( "%u"), m_bNoTag);
		bResult = bResult && WritePrivateProfileString( OCS_AGENT_SECTION, _T( "NoTAG"), csBuffer, m_csConfigFile);
		// Force IP Discover for the specified network
		bResult = bResult && WritePrivateProfileString( OCS_AGENT_SECTION, _T( "IpDisc"), m_csIpDisc, m_csConfigFile);
		// Communication provider to use
		CFile comFile( m_csComProvider, CFile::modeRead);
		bResult = bResult && WritePrivateProfileString( OCS_AGENT_SECTION, _T( "ComProvider"), comFile.GetFileName(), m_csConfigFile);
		return bResult;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return FALSE;
	}
}

void CConfig::setConfigFile(LPCTSTR lpstrFile)
{
	if (lpstrFile == NULL)
		m_csConfigFile.Format( _T( "%s\\%s"), getDataFolder(), OCS_CONFIG_FILENAME);
	else
		m_csConfigFile = lpstrFile;
}

void CConfig::setLocalInventory(LPCTSTR lpstrFolder)
{
	if ((lpstrFolder == NULL) || (_tcslen( lpstrFolder) == 0))
		m_csLocal = getDataFolder();
	else
		m_csLocal = lpstrFolder;
}

void CConfig::setTagText(LPCTSTR lpstrText)
{
	m_csTag = lpstrText;
}

void CConfig::setDebugRequired( UINT uDebug)
{
	m_uDebug = uDebug;
}

void CConfig::setFastIpRequired(BOOL bFast)
{
	m_bFastIp = bFast;
}

void CConfig::setForceInventoryRequired(BOOL bForce)
{
	m_bForce = bForce;
}

void CConfig::setHkcuRequired(BOOL bHKCU)
{
	m_bHKCU = bHKCU;
}

void CConfig::setIpDiscoverRequired(LPCTSTR lpstrNetwork)
{
	m_csIpDisc = lpstrNetwork;
}

void CConfig::setIpDiscoverLatency(LPCTSTR lpstrLatency)
{
	m_csIpDiscLat = lpstrLatency;
}

void CConfig::setNewUID(BOOL bNew)
{
	m_bUID = bNew;
}

void CConfig::setNotify(BOOL bNotify)
{
	m_bNotify = bNotify;
}

void CConfig::setNoSoftwareRequired(BOOL bNoSoftware)
{
	m_bNoSoftware = bNoSoftware;
}

void CConfig::setNoTagRequired(BOOL bNoTag)
{
	m_bNoTag = bNoTag;
}

void CConfig::setXmlFolder(LPCTSTR lpstrFolder)
{
	if ((lpstrFolder == NULL) || (_tcslen( lpstrFolder) == 0))
		m_csXml = getDataFolder();
	else
		m_csXml = lpstrFolder;
}

void CConfig::setCommunicationProvider( LPCTSTR lpstrDll)
{
	if (lpstrDll == NULL)
		m_csComProvider.Format( _T( "%s\\%s"), getInstallFolder(), OCS_DEFAULT_PROVIDER);
	else
		m_csComProvider.Format( _T( "%s\\%s"), getInstallFolder(), lpstrDll);
}

BOOL CConfig::writeAccountInfos( LPCTSTR lpstrName, LPCTSTR lpstrValue)
{
	HANDLE	hFile;
	CString	csAccountFile;

	// Write 
	csAccountFile.Format( _T( "%s\\%s"), getDataFolder(), OCS_ACCOUNTINFO_FILENAME);
	// Create the file with system and hidden attributes (if it does not exists)
	if ((hFile = CreateFile( csAccountFile, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN, NULL)) == INVALID_HANDLE_VALUE)
		return FALSE;
	CloseHandle( hFile);
	if (WritePrivateProfileString( OCS_AGENT_SECTION, lpstrName, lpstrValue, csAccountFile) == 0)
		return FALSE;
	return TRUE;
}
