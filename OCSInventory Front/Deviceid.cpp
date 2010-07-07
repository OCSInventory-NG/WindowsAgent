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
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
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
	CSysInfo				cSysinfo;
	CNetworkAdapter		cObject;
	CNetworkAdapterList cNetworkAdapterList;
	POSITION			position;
	BOOL				bContinue;
	CString				csMacConcat;

	cSysinfo.getNetworkAdapters( &cNetworkAdapterList );

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

/*****
 *
 *	Deviceid checkings and renewal if needed
 *
 ****/
void CDeviceid::checkDeviceid()
{
	CString csDeviceID, csFileDeviceID, csActualMac, csFileMac, csFileHname;
	TCHAR lpHostname[MAX_COMPUTERNAME_LENGTH + 1];
	CLog *pLogger = getOcsLogger();

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

	csFileHname		= csDeviceID.Left(csDeviceID.GetLength()-20);
	csFileDeviceID	= csDeviceID;

	if(csActualMac != csFileMac && lpHostname != csFileHname)
	{
		csDeviceID.Empty();
		pLogger->log( LOG_PRIORITY_NOTICE, _T( "DID_CHECK => Mac changed new:<%s> old:<%s>, hname changed new:<%s> old:<%s>"), 
			csActualMac, csFileMac, lpHostname, csFileHname ); 
	}
	else if( csActualMac != csFileMac || lpHostname != csFileHname )
	{
		m_csOldDeviceid = csDeviceID;
		csDeviceID.Empty();
		if( csActualMac != csFileMac )				
			pLogger->log( LOG_PRIORITY_NOTICE, _T( "DID_CHECK => Mac changed new:<%s> old:<%s>"), csActualMac, csFileMac);
		else
			pLogger->log(LOG_PRIORITY_NOTICE, _T( "DID_CHECK => hname changed new:<%s> old:<%s>"), lpHostname, csFileHname);
	}

	if (csDeviceID.IsEmpty())
	{
		// This is the first time we inventory the device => generate a new device unique ID
		pLogger->log( LOG_PRIORITY_NOTICE, _T( "DID_CHECK => Generating Unique ID for device <%s>"), lpHostname);
		m_csDeviceid = generate( lpHostname);
		writeDeviceid();
	}
	else
		m_csDeviceid = csDeviceID;
}

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

/* Store the deviceid and macs concatenated in a .dat file (compressed) */
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
	if ((pCb = CZip::deflate(  csContent)) == NULL)
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
