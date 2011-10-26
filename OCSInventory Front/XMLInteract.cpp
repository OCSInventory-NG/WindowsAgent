//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// XMLInteract.cpp: implementation of the CXMLInteract class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "XMLInteract.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXMLInteract::CXMLInteract(CMarkup *pXml)
{
	m_pXml = pXml;
}

CXMLInteract::~CXMLInteract()
{
}

BOOL CXMLInteract::UpdateBIOS( CBios &myBios)
{
	m_pXml->AddElem( _T( "BIOS"));
		m_pXml->AddChildElem( _T( "SMANUFACTURER"), myBios.GetSystemManufacturer());
		m_pXml->AddChildElem( _T( "SMODEL"), myBios.GetSystemModel());
		m_pXml->AddChildElem( _T( "SSN"), myBios.GetSystemSerialNumber());
		m_pXml->AddChildElem( _T( "TYPE"), myBios.GetMachineType());
		m_pXml->AddChildElem( _T( "BMANUFACTURER"), myBios.GetBiosManufacturer());
		m_pXml->AddChildElem( _T( "BVERSION"), myBios.GetBiosVersion());
		m_pXml->AddChildElem( _T( "BDATE"), myBios.GetBiosDate());
		m_pXml->AddChildElem( _T( "ASSETTAG"), myBios.GetAssetTag());
	m_pXml->OutOfElem();
	return TRUE;
}

BOOL CXMLInteract::UpdateDrives( CLogicalDriveList &myDriveList)
{
	CLogicalDrive	cObject;
	POSITION		pos;
	BOOL			bContinue;

	try
	{
		pos = myDriveList.GetHeadPosition();
		bContinue = (pos != NULL);
		if (bContinue)
			// There is one record => get the first
			cObject = myDriveList.GetNext( pos);
		while (bContinue)
		{
			bContinue = (pos != NULL);
			m_pXml->AddElem( _T( "DRIVES"));
				m_pXml->AddChildElem( _T( "LETTER"), cObject.GetDriveLetter());
				m_pXml->AddChildElem( _T( "TYPE"), cObject.GetDriveType());
				m_pXml->AddChildElem( _T( "FILESYSTEM"), cObject.GetFileSystem());
				m_pXml->AddChildElem( _T( "TOTAL"), cObject.GetTotalMB());
				m_pXml->AddChildElem( _T( "FREE"), cObject.GetFreeMB());
				m_pXml->AddChildElem( _T( "NUMFILES"), cObject.GetFilesNumber());
				m_pXml->AddChildElem( _T( "VOLUMN"), cObject.GetVolumName());
			m_pXml->OutOfElem();		
			if (pos != NULL)
				cObject = myDriveList.GetNext( pos);
		}
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		return FALSE;
	}
	return TRUE;
}

BOOL CXMLInteract::UpdateDeviceProperties( CDeviceProperties &pPC)
{
	m_pXml->AddElem( _T( "HARDWARE"));
		m_pXml->AddChildElem( _T( "NAME"), pPC.GetDeviceName());
		m_pXml->AddChildElem( _T( "WORKGROUP"), pPC.GetDomainOrWorkgroup());
		m_pXml->AddChildElem( _T( "USERDOMAIN"), pPC.GetUserDomain());
		m_pXml->AddChildElem( _T( "OSNAME"), pPC.GetOSName());
		m_pXml->AddChildElem( _T( "OSVERSION"), pPC.GetOSVersion());
		m_pXml->AddChildElem( _T( "OSCOMMENTS"), pPC.GetOSComment());
		m_pXml->AddChildElem( _T( "ARCH"), pPC.Is64bitsOS() ? _T( "x86 64 bits") : _T( "x86 32 bits"));
		m_pXml->AddChildElem( _T( "PROCESSORT"), pPC.GetProcessorType());
		m_pXml->AddChildElem( _T( "PROCESSORS"), pPC.GetProcessorSpeed());
		m_pXml->AddChildElem( _T( "PROCESSORN"), pPC.GetNumberOfProcessors());
		m_pXml->AddChildElem( _T( "MEMORY"), pPC.GetPhysicalMemory());
		m_pXml->AddChildElem( _T( "SWAP"), pPC.GetPageFileSize());
		m_pXml->AddChildElem( _T( "IPADDR"), pPC.GetIPAddress());
		m_pXml->AddChildElem( _T( "ETIME"), pPC.GetExecutionDuration());
		m_pXml->AddChildElem( _T( "LASTDATE"), pPC.GetLastCheckDate());
		m_pXml->AddChildElem( _T( "USERID"), pPC.GetLoggedOnUser());
		m_pXml->AddChildElem( _T( "LASTLOGGEDUSER"),pPC.GetLastLoggedUser());
		m_pXml->AddChildElem( _T( "TYPE"), (long) 0);
		m_pXml->AddChildElem( _T( "DESCRIPTION"), pPC.GetDescription());
		m_pXml->AddChildElem( _T( "WINCOMPANY"), pPC.GetWindowsRegisteredCompany());
		m_pXml->AddChildElem( _T( "WINOWNER"), pPC.GetWindowsRegisteredOwner());
		m_pXml->AddChildElem( _T( "WINPRODID"), pPC.GetWindowsProductID());
		m_pXml->AddChildElem( _T( "WINPRODKEY"), pPC.GetWindowsProductKey());
		m_pXml->AddChildElem( _T( "UUID"), pPC.GetUUID());
		m_pXml->AddChildElem( _T( "VMSYSTEM"), pPC.GetVMSystem());
		m_pXml->AddChildElem( _T( "CHECKSUM"), pPC.GetChecksum());
	m_pXml->OutOfElem();
	return TRUE;
}

BOOL CXMLInteract::NotifyDeviceProperties( CDeviceProperties &pPC)
{
	m_pXml->AddElem( _T( "HARDWARE"));
		m_pXml->AddChildElem( _T( "NAME"), pPC.GetDeviceName());
		m_pXml->AddChildElem( _T( "WORKGROUP"), pPC.GetDomainOrWorkgroup());
		m_pXml->AddChildElem( _T( "USERDOMAIN"), pPC.GetUserDomain());
		m_pXml->AddChildElem( _T( "IPADDR"), pPC.GetIPAddress());
		m_pXml->AddChildElem( _T( "USERID"), pPC.GetLoggedOnUser());
		m_pXml->AddChildElem( _T( "LASTLOGGEDUSER"),pPC.GetLastLoggedUser());
	m_pXml->OutOfElem();
	return TRUE;
}

BOOL CXMLInteract::UpdateInputDevices( CInputDeviceList &myInputList)
{
	CInputDevice cObject;
	POSITION	pos;
	BOOL		bContinue;

	try
	{
		// Create new file
	
		pos = myInputList.GetHeadPosition();
		bContinue = (pos != NULL);
		if (bContinue)
			// There is one record => get the first
			cObject = myInputList.GetNext( pos);
		while (bContinue)
		{
			bContinue = (pos != NULL);
			m_pXml->AddElem( _T( "INPUTS"));
				m_pXml->AddChildElem( _T( "TYPE"), cObject.GetType());
				m_pXml->AddChildElem( _T( "MANUFACTURER"), cObject.GetManufacturer());
				m_pXml->AddChildElem( _T( "CAPTION"), cObject.GetCaption());
				m_pXml->AddChildElem( _T( "DESCRIPTION"), cObject.GetDescription());
				m_pXml->AddChildElem( _T( "INTERFACE"), cObject.GetPointingInterface());
				m_pXml->AddChildElem( _T( "POINTTYPE"), cObject.GetPointingType());
			m_pXml->OutOfElem();
			if (pos != NULL)
				cObject = myInputList.GetNext( pos);
		}
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		return FALSE;
	}
	return TRUE;
}

BOOL CXMLInteract::UpdateMemorySlots( CMemorySlotList &myMemoryList)
{
	CMemorySlot		cObject;
	POSITION		pos;
	BOOL			bContinue;

	try
	{
		// Insert new object records from List
		pos = myMemoryList.GetHeadPosition();
		bContinue = (pos != NULL);
		if (bContinue)
			// There is one record => get the first
			cObject = myMemoryList.GetNext( pos);
		while (bContinue)
		{
			bContinue = (pos != NULL);
			m_pXml->AddElem( _T( "MEMORIES"));
				m_pXml->AddChildElem( _T( "CAPTION"), cObject.GetCaption());
				m_pXml->AddChildElem( _T( "DESCRIPTION"), cObject.GetDescription());
				m_pXml->AddChildElem( _T( "CAPACITY"), cObject.GetCapacity());
				m_pXml->AddChildElem( _T( "PURPOSE"), cObject.GetUsage());
				m_pXml->AddChildElem( _T( "TYPE"), cObject.GetType());
				m_pXml->AddChildElem( _T( "SPEED"), cObject.GetSpeed());
				m_pXml->AddChildElem( _T( "NUMSLOTS"), cObject.GetSlotNumber());	
				m_pXml->AddChildElem( _T( "SERIALNUMBER"), cObject.GetSN());	
			m_pXml->OutOfElem();		
			if (pos != NULL)
				cObject = myMemoryList.GetNext( pos);
		}
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		return FALSE;
	}
	return TRUE;
}

BOOL CXMLInteract::UpdateModems( CModemList &myModemList)
{
	CModem			cObject;
	POSITION		pos;
	BOOL			bContinue;

	try
	{	
		// Insert new object records from List
		pos = myModemList.GetHeadPosition();
		bContinue = (pos != NULL);
		if (bContinue)
			// There is one record => get the first
			cObject = myModemList.GetNext( pos);
		while (bContinue)
		{
			bContinue = (pos != NULL);
			m_pXml->AddElem( _T( "MODEMS"));
				m_pXml->AddChildElem( _T( "NAME"), cObject.GetName());
				m_pXml->AddChildElem( _T( "MODEL"), cObject.GetModel());
				m_pXml->AddChildElem( _T( "DESCRIPTION"), cObject.GetDescription());
				m_pXml->AddChildElem( _T( "TYPE"), cObject.GetType());
			m_pXml->OutOfElem();
			if (pos != NULL)
				cObject = myModemList.GetNext( pos);
		}
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		return FALSE;
	}
	return TRUE;
}

BOOL CXMLInteract::UpdateMonitors( CMonitorList &myMonitorList)
{
	CMonitor		cObject;
	POSITION		pos;
	BOOL			bContinue;

	try
	{
		pos = myMonitorList.GetHeadPosition();
		bContinue = (pos != NULL);
		if (bContinue)
			// There is one record => get the first
			cObject = myMonitorList.GetNext( pos);
		while (bContinue)
		{
			bContinue = (pos != NULL);
			m_pXml->AddElem( _T( "MONITORS"));
				m_pXml->AddChildElem( _T( "MANUFACTURER"), cObject.GetManufacturer());
				m_pXml->AddChildElem( _T( "CAPTION"), cObject.GetCaption());
				m_pXml->AddChildElem( _T( "DESCRIPTION"), cObject.GetDescription());
				m_pXml->AddChildElem( _T( "TYPE"), cObject.GetType());
				m_pXml->AddChildElem( _T( "SERIAL"), cObject.GetSerial());
			m_pXml->OutOfElem();
			if (pos != NULL)
				cObject = myMonitorList.GetNext( pos);
		}
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		return FALSE;
	}
	return TRUE;
}

BOOL CXMLInteract::UpdateNetworks( CNetworkAdapterList &myNetworkList)
{
	CNetworkAdapter	cObject;
	POSITION		pos;
	BOOL			bContinue;

	try
	{
		pos = myNetworkList.GetHeadPosition();
		bContinue = (pos != NULL);
		if (bContinue)
			// There is one record => get the first
			cObject = myNetworkList.GetNext( pos);
		while (bContinue)
		{
			bContinue = (pos != NULL);			
			m_pXml->AddElem( _T( "NETWORKS"));
				m_pXml->AddChildElem( _T( "DESCRIPTION"), cObject.GetDescription());
				m_pXml->AddChildElem( _T( "TYPE"), cObject.GetType());
				m_pXml->AddChildElem( _T( "TYPEMIB"), cObject.GetTypeMIB());
				m_pXml->AddChildElem( _T( "SPEED"), cObject.GetSpeed());
				m_pXml->AddChildElem( _T( "MACADDR"), cObject.GetMACAddress());
				m_pXml->AddChildElem( _T( "STATUS"), cObject.GetOperationalStatus());
				m_pXml->AddChildElem( _T( "IPADDRESS"), cObject.GetIPAddress());
				m_pXml->AddChildElem( _T( "IPMASK"), cObject.GetIPNetMask());
				m_pXml->AddChildElem( _T( "IPGATEWAY"), cObject.GetGateway());
				m_pXml->AddChildElem( _T( "IPSUBNET"), cObject.GetNetNumber());
				m_pXml->AddChildElem( _T( "IPDHCP"), cObject.GetDhcpServer());
			m_pXml->OutOfElem();		
			if (pos != NULL)
				cObject = myNetworkList.GetNext( pos);
		}
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		return FALSE;
	}
	return TRUE;
}

BOOL CXMLInteract::NotifyNetworks( CNetworkAdapterList &myNetworkList)
{
	CNetworkAdapter	cObject;
	POSITION		pos;
	BOOL			bContinue;

	try
	{
		pos = myNetworkList.GetHeadPosition();
		bContinue = (pos != NULL);
		if (bContinue)
			// There is one record => get the first
			cObject = myNetworkList.GetNext( pos);
		while (bContinue)
		{
			bContinue = (pos != NULL);			
			m_pXml->AddElem( _T( "IFACE"));
				m_pXml->AddChildElem( _T( "MAC"), cObject.GetMACAddress());
				m_pXml->AddChildElem( _T( "IP"), cObject.GetIPAddress());
				m_pXml->AddChildElem( _T( "MASK"), cObject.GetIPNetMask());
				m_pXml->AddChildElem( _T( "GW"), cObject.GetGateway());
				m_pXml->AddChildElem( _T( "SUBNET"), cObject.GetNetNumber());
				m_pXml->AddChildElem( _T( "DHCP"), cObject.GetDhcpServer());
			m_pXml->OutOfElem();		
			if (pos != NULL)
				cObject = myNetworkList.GetNext( pos);
		}
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		return FALSE;
	}
	return TRUE;
}

BOOL CXMLInteract::UpdateSystemPorts( CSystemPortList &myPortList)
{
	CSystemPort	cObject;
	POSITION	pos;
	BOOL		bContinue;

	try
	{
		// Insert new object records from List
		pos = myPortList.GetHeadPosition();
		bContinue = (pos != NULL);
		if (bContinue)
			// There is one record => get the first
			cObject = myPortList.GetNext( pos);
		while (bContinue)
		{
			bContinue = (pos != NULL);
			m_pXml->AddElem( _T( "PORTS"));
				m_pXml->AddChildElem( _T( "TYPE"), cObject.GetType());
				m_pXml->AddChildElem( _T( "NAME"), cObject.GetName());
				m_pXml->AddChildElem( _T( "CAPTION"), cObject.GetCaption());
				m_pXml->AddChildElem( _T( "DESCRIPTION"), cObject.GetDescription());
			m_pXml->OutOfElem();
			if (pos != NULL)
				cObject = myPortList.GetNext( pos);
		}
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		return FALSE;
	}
	return TRUE;
}

BOOL CXMLInteract::UpdatePrinters( CPrinterList &myPrinterList)
{
	CPrinter	cObject;
	POSITION	pos;
	BOOL		bContinue;

	try
	{
		// Insert new object records from List
		pos = myPrinterList.GetHeadPosition();
		bContinue = (pos != NULL);
		if (bContinue)
			// There is one record => get the first
			cObject = myPrinterList.GetNext( pos);
		while (bContinue)
		{
			bContinue = (pos != NULL);
			m_pXml->AddElem( _T( "PRINTERS"));
				m_pXml->AddChildElem( _T( "NAME"), cObject.GetName());
				m_pXml->AddChildElem( _T( "DRIVER"), cObject.GetDriver());
				m_pXml->AddChildElem( _T( "PORT"), cObject.GetPort());
			m_pXml->OutOfElem();
			if (pos != NULL)
				cObject = myPrinterList.GetNext( pos);
		}
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		return FALSE;
	}
	return TRUE;
}

BOOL CXMLInteract::UpdateAccountInfo( LPCTSTR lpstrAccountFile)
{
	CMarkup		myXmlAccount;

	try
	{
		// Get size of Admininfo.conf file
		if (!fileExists( lpstrAccountFile))
			// If admin file does not exist, return TRUE because assuming first run or no admin infos
			return TRUE;
		if (!myXmlAccount.LoadFile( lpstrAccountFile))
		{
			// Old format admin infos file ???
			if (UpdateAccountInfoOldFormat( &myXmlAccount, lpstrAccountFile))
			{
				// Yes, so save in new format
				SetFileAttributes( lpstrAccountFile, FILE_ATTRIBUTE_NORMAL);
				myXmlAccount.SaveFile( lpstrAccountFile);
				SetFileAttributes( lpstrAccountFile, FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM);
			}
		}
		return m_pXml->AddXml( &myXmlAccount);;
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		return FALSE;
	}
}

BOOL CXMLInteract::UpdateAccountInfoOldFormat( CMarkup *pXml, LPCTSTR lpstrAccountFile)
{
	CFileStatus myFileStatus;
	TCHAR       *szKeys = NULL,
				szSeps[] = _T( "-"), 
				*szToken1 = NULL,
				*szToken2 = NULL;
	LONG        lSizeRead;

	try
	{
		// Get size of Admininfo.conf file
		if (CFile::GetStatus( lpstrAccountFile, myFileStatus))
		{
			// Allocate 2 buffers
			if ((szKeys = new TCHAR[ myFileStatus.m_size+1]) == NULL)
				return FALSE;
			if ((szToken2 = new TCHAR[myFileStatus.m_size+1]) == NULL)
			{
				delete szKeys;
				return FALSE;
			}
			// Get all key names from section in a multistring buffer, each string separated by \0
			lSizeRead = GetPrivateProfileString( OCS_AGENT_SECTION, NULL, _T(""), szKeys, myFileStatus.m_size, lpstrAccountFile);
			// Replace \0 by - in key buffer
			for (LONG i=0; i<myFileStatus.m_size; i++)
			{
				if (szKeys[i] == 0)
				{
					szKeys[i]='-';
				}
				else if (i>=lSizeRead)
				{
					szKeys[i]=0;
				}
			}
			// Parse key buffer to extract each key name
			szToken1 = _tcstok( szKeys, szSeps );
			while (szToken1 != NULL )
			{
				// Get value for this key
				GetPrivateProfileString( OCS_AGENT_SECTION, szToken1, _T(""), szToken2, myFileStatus.m_size, lpstrAccountFile);
				// Write it in XML
				pXml->AddElem( _T( "ACCOUNTINFO"));
					pXml->AddChildElem( _T( "KEYNAME"), szToken1);
					pXml->AddChildElem( _T( "KEYVALUE"), szToken2);
				pXml->OutOfElem();                            
				// Get next key name
				szToken1 = _tcstok( NULL, szSeps );
			}
			// free buffers
			delete szToken2;
			delete szKeys;
		}
		// Even if admin file does not exist, return TRUE
		return TRUE;
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		return FALSE;
	}
}

BOOL CXMLInteract::UpdateRegistryValues( CRegistryValueList &myRegistryList)
{
	CRegistryValue cObject;	
	POSITION	pos;
	BOOL		bContinue;

	try
	{
		// Insert new object records from List
		pos = myRegistryList.GetHeadPosition();
		bContinue = (pos != NULL);
		if (bContinue)
			// There is one record => get the first
			cObject = myRegistryList.GetNext( pos);

		BOOL didPresent=FALSE;
		while (bContinue)
		{
			bContinue = (pos != NULL);		
			m_pXml->AddElem( _T( "REGISTRY"));
				m_pXml->AddChildElem( _T( "NAME"), cObject.GetName());
				m_pXml->AddChildElem( _T( "REGVALUE"), cObject.GetValue());
			m_pXml->OutOfElem();
			if (pos != NULL)
				cObject = myRegistryList.GetNext( pos);
		}
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		return FALSE;
	}
	return TRUE;
}

BOOL CXMLInteract::UpdateSoftwares( CSoftwareList &mySoftwareList)
{
	CSoftware	cObject;
	POSITION	pos;
	BOOL		bContinue;

	try
	{
		// Insert new object records from List
		pos = mySoftwareList.GetHeadPosition();
		bContinue = (pos != NULL);
		if (bContinue)
			// There is one record => get the first
			cObject = mySoftwareList.GetNext( pos);
		while (bContinue)
		{
			bContinue = (pos != NULL);
			m_pXml->AddElem( _T( "SOFTWARES"));
				m_pXml->AddChildElem( _T( "PUBLISHER"), cObject.GetPublisher());
				m_pXml->AddChildElem( _T( "NAME"), cObject.GetName());
				m_pXml->AddChildElem( _T( "VERSION"), cObject.GetVersion());
				m_pXml->AddChildElem( _T( "FOLDER"), cObject.GetFolder());
				m_pXml->AddChildElem( _T( "COMMENTS"), cObject.GetComments());
				m_pXml->AddChildElem( _T( "FILENAME"), cObject.GetFilename());
				m_pXml->AddChildElem( _T( "FILESIZE"), cObject.GetFilesize());
				m_pXml->AddChildElem( _T( "GUID"), cObject.GetGUID());
				m_pXml->AddChildElem( _T( "LANGUAGE"), cObject.GetLanguage());
				m_pXml->AddChildElem( _T( "INSTALLDATE"), cObject.GetInstallDate());
				m_pXml->AddChildElem( _T( "BITSWIDTH"), cObject.GetMemoryAddressWidth());
				m_pXml->AddChildElem( _T( "SOURCE"), cObject.IsFromRegistry() ? 1 : 0);
			m_pXml->OutOfElem();
			if (pos != NULL)
				cObject = mySoftwareList.GetNext( pos);
		}
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		return FALSE;
	}
	return TRUE;
}

BOOL CXMLInteract::UpdateSounds( CSoundDeviceList &mySoundList)
{
	CSoundDevice	cObject;
	POSITION		pos;
	BOOL			bContinue;

	try
	{
		// Insert new object records from List
		pos = mySoundList.GetHeadPosition();
		bContinue = (pos != NULL);
		if (bContinue)
			// There is one record => get the first
			cObject = mySoundList.GetNext( pos);
		while (bContinue)
		{
			bContinue = (pos != NULL);
			m_pXml->AddElem( _T( "SOUNDS"));
				m_pXml->AddChildElem( _T( "MANUFACTURER"), cObject.GetManufacturer());
				m_pXml->AddChildElem( _T( "NAME"), cObject.GetName());
				m_pXml->AddChildElem( _T( "DESCRIPTION"), cObject.GetDescription());
			m_pXml->OutOfElem();
			if (pos != NULL)
				cObject = mySoundList.GetNext( pos);
		}
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		return FALSE;
	}
	return TRUE;
}

BOOL CXMLInteract::UpdateStorages( CStoragePeripheralList &myStorageList)
{
	CStoragePeripheral cObject;
	POSITION		pos;
	BOOL			bContinue;

	try
	{
		// Insert new object records from List
		pos = myStorageList.GetHeadPosition();
		bContinue = (pos != NULL);
		if (bContinue)
			// There is one record => get the first
			cObject = myStorageList.GetNext( pos);
		while (bContinue)
		{
			bContinue = (pos != NULL);
			m_pXml->AddElem( _T( "STORAGES"));
				m_pXml->AddChildElem( _T( "MANUFACTURER"), cObject.GetManufacturer());
				m_pXml->AddChildElem( _T( "NAME"), cObject.GetModel());
				m_pXml->AddChildElem( _T( "MODEL"), cObject.GetName());
				m_pXml->AddChildElem( _T( "DESCRIPTION"), cObject.GetDescription());
				m_pXml->AddChildElem( _T( "TYPE"), cObject.GetType());
				m_pXml->AddChildElem( _T( "DISKSIZE"), cObject.GetSizeString());
				m_pXml->AddChildElem( _T( "SERIALNUMBER"), cObject.GetSN());
				m_pXml->AddChildElem( _T( "FIRMWARE"), cObject.GetFirmware());
			m_pXml->OutOfElem();
			if (pos != NULL)
				cObject = myStorageList.GetNext( pos);
		}
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		return FALSE;
	}
	return TRUE;
}

BOOL CXMLInteract::UpdateSystemControllers( CSystemControllerList &mySystemControllerList)
{
	CSystemController cObject;
	POSITION		pos;
	BOOL			bContinue;

	try
	{
		// Insert new object records from List
		pos = mySystemControllerList.GetHeadPosition();
		bContinue = (pos != NULL);
		if (bContinue)
			// There is one record => get the first
			cObject = mySystemControllerList.GetNext( pos);
		while (bContinue)
		{
			bContinue = (pos != NULL);
			m_pXml->AddElem( _T( "CONTROLLERS"));
				m_pXml->AddChildElem( _T( "MANUFACTURER"), cObject.GetManufacturer());
				m_pXml->AddChildElem( _T( "NAME"), cObject.GetName());
				m_pXml->AddChildElem( _T( "CAPTION"), cObject.GetCaption());
				m_pXml->AddChildElem( _T( "DESCRIPTION"), cObject.GetDescription());
				m_pXml->AddChildElem( _T( "VERSION"), cObject.GetHardwareVersion());
				m_pXml->AddChildElem( _T( "TYPE"), cObject.GetType());
			m_pXml->OutOfElem();
			if (pos != NULL)
				cObject = mySystemControllerList.GetNext( pos);
		}
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		return FALSE;
	}
	return TRUE;
}

BOOL CXMLInteract::UpdateSystemSlots( CSystemSlotList &mySlotList)
{	
	CSystemSlot		cObject;
	POSITION		pos;
	BOOL			bContinue;

	try
	{
		// Insert new object records from List
		pos = mySlotList.GetHeadPosition();
		bContinue = (pos != NULL);
		if (bContinue)
			// There is one record => get the first
			cObject = mySlotList.GetNext( pos);
		while (bContinue)
		{
			bContinue = (pos != NULL);
			m_pXml->AddElem( _T( "SLOTS"));
				m_pXml->AddChildElem( _T( "NAME"), cObject.GetName());
				m_pXml->AddChildElem( _T( "DESCRIPTION"), cObject.GetDescription());
				m_pXml->AddChildElem( _T( "DESIGNATION"), cObject.GetSlotDesignation());
				m_pXml->AddChildElem( _T( "PURPOSE"), cObject.GetUsage());
				m_pXml->AddChildElem( _T( "STATUS"), cObject.GetStatus());
				m_pXml->AddChildElem( _T( "SHARED"), cObject.IsShared() ? 1 : 0);
			m_pXml->OutOfElem();
			if (pos != NULL)
				cObject = mySlotList.GetNext( pos);
		}
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		return FALSE;
	}
	return TRUE;
}

BOOL CXMLInteract::UpdateVideos( CVideoAdapterList &myVideoList)
{
	CVideoAdapter	cObject;
	POSITION		pos;
	BOOL			bContinue;

	try
	{
		// Insert new object records from List
		pos = myVideoList.GetHeadPosition();
		bContinue = (pos != NULL);
		if (bContinue)
			// There is one record => get the first
			cObject = myVideoList.GetNext( pos);
		while (bContinue)
		{
			bContinue = (pos != NULL);
			m_pXml->AddElem( _T( "VIDEOS"));
				m_pXml->AddChildElem( _T( "NAME"), cObject.GetName());
				m_pXml->AddChildElem( _T( "CHIPSET"), cObject.GetChipset());
				m_pXml->AddChildElem( _T( "MEMORY"), cObject.GetMemory());
				m_pXml->AddChildElem( _T( "RESOLUTION"), cObject.GetScreenResolution());
			m_pXml->OutOfElem();
			if (pos != NULL)
				cObject = myVideoList.GetNext( pos);
		}
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		return FALSE;
	}
	return TRUE;
}
