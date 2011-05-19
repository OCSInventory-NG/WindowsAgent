//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Deviceid.cpp: implementation of the CDeviceid class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Deviceid.h"
#include "OcsUTils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeviceid::CDeviceid()
{
	checkDeviceid();
}

CDeviceid::~CDeviceid()
{

}

CString CDeviceid::getDeviceID()
{
	return m_csDeviceid;
}

CString CDeviceid::getOldDeviceID()
{
	return m_csOldDeviceid;
}

CString CDeviceid::getComputerName()
{
	return m_csHostName;
}

/* Generate a new deviceid */
CString CDeviceid::generate( LPCTSTR lpstrHostname)
{
	CString csDeviceID;
	COleDateTime myDate = COleDateTime::GetCurrentTime();

	csDeviceID.Format( _T( "%s-%s"), lpstrHostname, myDate.Format( _T( "%Y-%m-%d-%H-%M-%S")));
	return csDeviceID;
}

/* Concatenate MAC adresses in a CString and return it */
CString CDeviceid::getMacs()
{
	CSysInfo			cSysinfo;
	CNetworkAdapter		cObject;
	CNetworkAdapterList cNetworkAdapterList;
	POSITION			position;
	BOOL				bContinue;
	CString				csMacConcat;

	m_csMacConcat.Empty();
	if (!cSysinfo.getNetworkAdapters( &cNetworkAdapterList ))
		return m_csMacConcat;

	position = cNetworkAdapterList.GetHeadPosition();
	bContinue = (position != NULL);

	if (bContinue)
		cObject = cNetworkAdapterList.GetNext(position);

	while (bContinue)
	{
		bContinue = (position != NULL);
		csMacConcat += cObject.GetMACAddress();
		if (position != NULL)
			cObject = cNetworkAdapterList.GetNext( position );
	}
	m_csMacConcat = csMacConcat;
	return m_csMacConcat;
}

/* Load deviceid and MACs concatenated from the .dat file (compressed)  */
BOOL CDeviceid::loadDeviceid( CString &csDeviceID, CString &csMac)
{
	CString csContent;
	CByteArray *pCb;

	// Read .dat file
	csContent.Format( _T( "%s\\%s"), getDataFolder(), DEVICEID_DAT_FILE);
	if ((pCb = LoadFileToByteArray( csContent)) == NULL)
		return FALSE;

	// Uncompress content
	csContent = CZip::inflate( pCb );
	delete pCb;

	// First line is Device ID
	csDeviceID = csContent.Left( csContent.Find('\n') );
	// Second line is MAC addresses
	csMac = csContent.Right( csContent.GetLength()-csContent.Find('\n')-1);

	if( csContent.GetLength() == 0 )
		return FALSE;
	else
		return TRUE;
}

/* Store the deviceid and macs concatenated in the .dat file (compressed) */
BOOL CDeviceid::writeDeviceid()
{
	CString csContent;
	CByteArray *pCb = NULL;
	CLog *pLogger = getOcsLogger();

	pLogger->log( LOG_PRIORITY_DEBUG, _T( "DID_CHECK => writing DeviceID <%s> and MACs <%s> in file <%s\\%s>"), 
		m_csDeviceid, m_csMacConcat, getDataFolder(), DEVICEID_DAT_FILE);
	// Set Device ID into first line and MAC adresses into second line
	csContent.Format( _T( "%s\n%s"), m_csDeviceid, m_csMacConcat);
	// Compress buffer
	if ((pCb = CZip::deflate(  GetAnsiFromUnicode( csContent))) == NULL)
	{
		// Unable to compress
		pLogger->log( LOG_PRIORITY_WARNING, _T( "DID_CHECK => could not write file <%s\\%s>"), getDataFolder(), DEVICEID_DAT_FILE);
		return FALSE;
	}
	// Write buffer
	csContent.Format( _T( "%s\\%s"), getDataFolder(), DEVICEID_DAT_FILE);
	if (!WriteByteArrayToFile( pCb, csContent))
	{
		// Unable to write file
		pLogger->log( LOG_PRIORITY_WARNING, _T( "DID_CHECK => could not write file <%s\\%s>"), getDataFolder(), DEVICEID_DAT_FILE);
		delete pCb;
		return FALSE;
	}
	// All sucessfull
	delete pCb;
	return TRUE;
}

/* Parse MAC concatened into String Array */
UINT CDeviceid::parseMacs( CString &csMac, CStringArray &csMacArray)
{
	UINT uNumber = 0,
		 uIndex = 0;
	CString csOneMac;

	try
	{
		csMacArray.RemoveAll();
		while (csMac.Mid( uIndex, MAC_STRING_LENGTH).GetLength() > 0)
		{
			csOneMac = csMac.Mid( uIndex, MAC_STRING_LENGTH);
			if (csOneMac.GetLength() == MAC_STRING_LENGTH)
			{
				// This a MAC address
				csMacArray.Add( csOneMac);
				uNumber ++;
			}
			uIndex += MAC_STRING_LENGTH;
		}
		return uNumber;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return 0;
	}
}

/* Compares 2 string array of MAC */
BOOL CDeviceid::CompareMacs( CString &csRefList, CString &csActualList)
{
	CStringArray csRefArray, csActualArray;
	CString	csMac;
	BOOL	bFound;
	INT_PTR	uCount = 0,
			uRefIndex,
			uActualIndex,
			uChanged = 0;
	BOOL	bContinue = TRUE;

	// Parse reference list into array, and set his number of MAC addresses as reference count
	if ((uCount = parseMacs( csRefList, csRefArray)) == 0)
		return FALSE;
	// Parse new computed list into array
	if (parseMacs( csActualList, csActualArray) == 0)
		return FALSE;
	// For each MAC from computed list
	for (uActualIndex = 0; uActualIndex < csActualArray.GetCount(); uActualIndex ++)
	{
		csMac = csActualArray.GetAt( uActualIndex);
		bFound = FALSE; 
		// Try to find MAC from computed list into reference array
		for (uRefIndex = 0; uRefIndex < csRefArray.GetCount() && !bFound; uRefIndex ++)
		{
			if (csMac == csRefArray.GetAt( uRefIndex))
			{
				// We found it into reference array => remove it from reference array
				csRefArray.RemoveAt( uRefIndex);
				bFound = TRUE;
			}
		}
	}
	switch (csRefArray.GetCount())
	{
	case 0: 
		// All MAC from reference were found (all MAC are the same or there is only MAC added),
		// so no changes du to MAC address
		return FALSE;
	case 1: // One MAC from reference not found (only one changed)
		if (uCount == 1)
			// There was only one MAC in the reference, so changed
			return TRUE;
		// At least 2 or more MAC in the reference, so assuming no change du to MAC address
		return FALSE;
	case 2:
	default: // At least 2 MAC changed, assuming changes du to MAC address
		return TRUE;
	}
}

/*****
 *
 *	Deviceid checkings and renewal if needed
 *
 ****/
void CDeviceid::checkDeviceid()
{
	CString csDeviceID, csFileDeviceID, csActualMac, csFileMac, csFileHostname;
	TCHAR lpHostname[MAX_COMPUTERNAME_LENGTH + 1];
	CLog *pLogger = getOcsLogger();
	BOOL bMacChanged = FALSE;

	DWORD size = sizeof( lpHostname );
	GetComputerName( lpHostname,  &size);
	m_csHostName = lpHostname;

	// Load deviceid from .dat file
	if (!loadDeviceid( csDeviceID, csFileMac))
		pLogger->log( LOG_PRIORITY_WARNING, _T( "DID_CHECK => Failed to read file <%s>"), DEVICEID_DAT_FILE);
	else
		pLogger->log( LOG_PRIORITY_DEBUG, _T( "DID_CHECK => Read DeviceID <%s> and MACs <%s> in file <%s>"), csDeviceID, csFileMac, DEVICEID_DAT_FILE);
	// Get list of MC Addresses
	csActualMac = getMacs();

	csFileHostname	= csDeviceID.Left( csDeviceID.GetLength()-20);
	csFileDeviceID	= csDeviceID;

	// Compare reference to actual. There is changes if
	// - Hostname has changed
	// - There is only one MAC, and it has changed
	// - There is 2 or more MACs, and at least 2 has changed has changed
	bMacChanged = CompareMacs( csFileMac, csActualMac);
	if (bMacChanged && (m_csHostName != csFileHostname))
	{
		// Both MAC and hostname changes
		csDeviceID.Empty();
		pLogger->log( LOG_PRIORITY_NOTICE, _T( "DID_CHECK => MAC Address changed new:<%s> old:<%s>, Hostname changed new:<%s> old:<%s>"), 
			csActualMac, csFileMac, m_csHostName, csFileHostname ); 
	}
	else if (bMacChanged || (m_csHostName != csFileHostname))
	{
		m_csOldDeviceid = csDeviceID;
		csDeviceID.Empty();
		if (bMacChanged)				
			pLogger->log( LOG_PRIORITY_NOTICE, _T( "DID_CHECK => MAC Address changed new:<%s> old:<%s>"), csActualMac, csFileMac);
		else
			pLogger->log(LOG_PRIORITY_NOTICE, _T( "DID_CHECK => Hostname changed new:<%s> old:<%s>"), m_csHostName, csFileHostname);
	}

	if (csDeviceID.IsEmpty())
	{
		// This is the first time we inventory the device => generate a new device unique ID
		pLogger->log( LOG_PRIORITY_NOTICE, _T( "DID_CHECK => Generating new unique ID for device <%s>"), m_csHostName);
		m_csDeviceid = generate( m_csHostName);
	}
	else
		m_csDeviceid = csDeviceID;
	// Write file if something changes, even if DeviceID not regenerated (case of minor changes on MAC addresses)
	if (( csFileMac != csActualMac) || (m_csHostName != csFileHostname))
		writeDeviceid();
}

