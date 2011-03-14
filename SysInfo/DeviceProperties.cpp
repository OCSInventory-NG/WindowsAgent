//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// DeviceProperties.cpp: implementation of the CDeviceProperties class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sysinfo.h"
#include "OcsCrypto.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeviceProperties::CDeviceProperties()
{
	Clear();
}

CDeviceProperties::~CDeviceProperties()
{
	Clear();
}

void CDeviceProperties::Clear()
{
	COleDateTime oleTimeNow;

	m_csDeviceID.Empty();		// Device unique ID
	m_csDeviceName.Empty();		// Device netbios name
	m_csDomain.Empty();			// Domain or workgroup
	m_csUserDomain.Empty();			// Domain or workgroup
	m_csOSName.Empty();			// OS Name of the device (ex "Windows NT")
	m_csOSVersion.Empty();		// OS Version of the device (ex "4.0 Build 1381")
	m_csOSComment.Empty();		// OS Comment of the device (ex "Service Pack 6")
	m_uBitsOS = 32;				// Assume 32 bits OS
	m_csProcessorType.Empty();	// First Processor type of the device (ex "Intel Pentium II Model 1280 Stepping 2")
	m_csProcessorSpeed.Empty(); // Processor speed
	m_dwNumberOfProcessor = 0;	// Number of processor of the device
	m_ulPhysicalMemory = 0;		// Physical memory of the device
	m_ulSwapSize = 0;		// Page File Size of the device
	m_csIPAddress.Empty();		// IP Address of the device if available (ex "192.3.4.1" or "Unavailable")
	m_csExecutionDuration = _T( "00:00:00"); // Duration of the inventory check
	m_csLoggedOnUser.Empty();	// Logged on user when device has been checked
	m_csLastLoggedUser.Empty();	// Last user who'd been logged in
	m_csDescription.Empty();	// Device description extracted from OS
	m_csWinRegCompany.Empty();	// Windows registered company
	m_csWinRegOwner.Empty();	// Windows registered owner
	m_csWinRegProductID.Empty();// Windows registered product ID
	oleTimeNow.SetDate( 1970, 1, 1);
	m_csLastCheckDate = oleTimeNow.Format( _T( "%Y-%m-%d"));
	m_ulChecksum = 0;			// No changes detected
}

void CDeviceProperties::SetDeviceID(LPCTSTR lpstrDeviceID)
{
	m_csDeviceID = lpstrDeviceID;
	StrForSQL( m_csDeviceID);
}

void CDeviceProperties::SetDeviceName(LPCTSTR lpstrName)
{
	m_csDeviceName = lpstrName;
	StrForSQL( m_csDeviceName);
}

void CDeviceProperties::SetOS(LPCTSTR lpstrName, LPCTSTR lpstrVersion, LPCTSTR lpstrComment)
{
	m_csOSName = lpstrName;
	StrForSQL( m_csOSName);
	m_csOSVersion = lpstrVersion;
	StrForSQL( m_csOSVersion);
	m_csOSComment = lpstrComment;
	StrForSQL( m_csOSComment);
}

void CDeviceProperties::SetProcessor(LPCTSTR lpstrName, LPCTSTR lpstrSpeed, DWORD dwNumber)
{
	m_csProcessorType = lpstrName;
	StrForSQL( m_csProcessorType);
	m_csProcessorSpeed = lpstrSpeed;
	StrForSQL( m_csProcessorSpeed);
	m_dwNumberOfProcessor = dwNumber;
}

void CDeviceProperties::SetAddressWidthOS( UINT uBits)
{
	m_uBitsOS = uBits;
}

void CDeviceProperties::SetMemory(ULONG ulPhysical, ULONG ulPageFile)
{
	m_ulPhysicalMemory = ulPhysical;
	m_ulSwapSize = ulPageFile;
}

void CDeviceProperties::SetIPAddress(LPCTSTR lpstrIP)
{
	m_csIPAddress = lpstrIP;
	StrForSQL( m_csIPAddress);
}

void CDeviceProperties::SetLastCheckDate( LPCTSTR lpstrDate)
{
	m_csLastCheckDate = lpstrDate;
	StrForSQL( m_csLastCheckDate);
}

void CDeviceProperties::SetExecutionDuration( CTime &cBeginTime, CTime &cEndTime)
{
	CTimeSpan	cTimeExec;				// Time of execution

	// Format time execution
	cTimeExec = cEndTime - cBeginTime;
	// Format SQL query
	m_csExecutionDuration.Format( _T( "%.02d:%.02d:%.02d"), cTimeExec.GetHours(), cTimeExec.GetMinutes(), cTimeExec.GetSeconds());
}

void CDeviceProperties::SetExecutionDuration( LPCTSTR lpstrDuration)
{
	m_csExecutionDuration = lpstrDuration;
	StrForSQL( m_csExecutionDuration);
}

void CDeviceProperties::SetLoggedOnUser( LPCTSTR lpstrUser)
{
	m_csLoggedOnUser = lpstrUser;
	StrForSQL( m_csLoggedOnUser);
}

void CDeviceProperties::SetLastLoggedUser( LPCTSTR lpstrLastLoggedUser)
{
	m_csLastLoggedUser = lpstrLastLoggedUser;
	StrForSQL( m_csLastLoggedUser);
}


void CDeviceProperties::SetDescription( LPCTSTR lpstrDescription)
{
	m_csDescription = lpstrDescription;
	StrForSQL( m_csDescription);
}

void CDeviceProperties::SetDomainOrWorkgroup( LPCTSTR lpstrDomain)
{
	m_csDomain = lpstrDomain;
	StrForSQL( m_csDomain);
}

void CDeviceProperties::SetUserDomain( LPCTSTR lpstrUserDomain)
{
	m_csUserDomain = lpstrUserDomain;
	StrForSQL( m_csUserDomain);
}

void CDeviceProperties::SetWindowsProductKey( LPCTSTR lpstrWindowsKey)
{
	m_csWinProductKey = lpstrWindowsKey;
	StrForSQL( m_csWinProductKey );
}

void CDeviceProperties::SetWindowsRegistration( LPCTSTR lpstrCompany, LPCTSTR lpstrOwner, LPCTSTR lpstrProductID)
{
	m_csWinRegCompany = lpstrCompany;
	StrForSQL( m_csWinRegCompany);
	m_csWinRegOwner = lpstrOwner;
	StrForSQL( m_csWinRegOwner);
	m_csWinRegProductID = lpstrProductID;
	StrForSQL( m_csWinRegProductID);
}

void CDeviceProperties::SetChecksum( ULONG ulChecksum)
{
	m_ulChecksum = ulChecksum;
}

LPCTSTR CDeviceProperties::GetDeviceID()
{
	return m_csDeviceID;
}

LPCTSTR CDeviceProperties::GetDeviceName()
{
	return m_csDeviceName;
}

LPCTSTR CDeviceProperties::GetOSName()
{
	return m_csOSName;
}

LPCTSTR CDeviceProperties::GetOSVersion()
{
	return m_csOSVersion;
}

LPCTSTR CDeviceProperties::GetOSComment()
{
	return m_csOSComment;
}

BOOL CDeviceProperties::Is64bitsOS()
{
	return (m_uBitsOS == 64);
}

LPCTSTR CDeviceProperties::GetProcessorType()
{
	return m_csProcessorType;
}

LPCTSTR CDeviceProperties::GetProcessorSpeed()
{
	return m_csProcessorSpeed;
}

DWORD CDeviceProperties::GetNumberOfProcessors()
{
	return m_dwNumberOfProcessor;
}

ULONG CDeviceProperties::GetPhysicalMemory()
{
	return m_ulPhysicalMemory;
}

ULONG CDeviceProperties::GetPageFileSize()
{
	return m_ulSwapSize;
}

LPCTSTR CDeviceProperties::GetIPAddress()
{
	return m_csIPAddress;
}
LPCTSTR CDeviceProperties::GetExecutionDuration()
{
	return m_csExecutionDuration;
}

LPCTSTR CDeviceProperties::GetLastCheckDate()
{
	return m_csLastCheckDate;
}

LPCTSTR CDeviceProperties::GetLoggedOnUser()
{
	return m_csLoggedOnUser;
}

LPCTSTR CDeviceProperties::GetLastLoggedUser()
{
	return m_csLastLoggedUser;
}

LPCTSTR CDeviceProperties::GetDescription()
{
	return m_csDescription;
}

LPCTSTR CDeviceProperties::GetDomainOrWorkgroup()
{
	return m_csDomain;
}

LPCTSTR CDeviceProperties::GetUserDomain()
{
	return m_csUserDomain;
}

LPCTSTR CDeviceProperties::GetWindowsRegisteredCompany()
{
	return m_csWinRegCompany;
}

LPCTSTR CDeviceProperties::GetWindowsRegisteredOwner()
{
	return m_csWinRegOwner;
}

LPCTSTR CDeviceProperties::GetWindowsProductID()
{
	return m_csWinRegProductID;
}

LPCTSTR CDeviceProperties::GetWindowsProductKey()
{
	return m_csWinProductKey;
}

ULONG CDeviceProperties::GetChecksum()
{
	return m_ulChecksum;
}

LPCTSTR CDeviceProperties::GetHash()
{
	COcsCrypto	myHash;
	static CString		csToHash;

	if (!myHash.HashInit())
		return NULL;
	csToHash.Format( _T( "%s%s%s%s%s%s%s%u%lu%s%s%s"), m_csDeviceName, m_csDomain, m_csOSName,
					 m_csOSVersion, m_csOSComment, m_csProcessorType,
					 m_csProcessorSpeed,m_dwNumberOfProcessor,m_ulPhysicalMemory,
					 m_csWinRegCompany, m_csWinRegOwner, m_csWinRegProductID);
	myHash.HashUpdate( csToHash);
	return myHash.HashFinal();
}
