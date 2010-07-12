//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// EDID.cpp: implementation of the CEdid class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "SysInfo.h"
#include "DebugLog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// SetupAPI functions declaration
// Win 98/Me
HDEVINFO (WINAPI *lpfnSetupDiCreateDeviceInfoList) (const GUID * ClassGuid, HWND hwndParent);
// Win 2000 and +
HDEVINFO (WINAPI *lpfnSetupDiCreateDeviceInfoListEx) (const GUID * ClassGuid, HWND hwndParent, PCTSTR MachineName, PVOID Reserved);
// Win 98/Me
HDEVINFO (WINAPI *lpfnSetupDiGetClassDevs) (const GUID *ClassGuid, PCTSTR Enumerator, HWND hwndParent, DWORD Flags);
// Win 2000 and +
HDEVINFO (WINAPI *lpfnSetupDiGetClassDevsEx) (const GUID *ClassGuid, PCTSTR Enumerator, HWND hwndParent, DWORD Flags, HDEVINFO DeviceInfoSet, PCTSTR MachineName, PVOID Reserved);
BOOL (WINAPI *lpfnSetupDiEnumDeviceInfo) (HDEVINFO DeviceInfoSet, DWORD dwInstance, PSP_DEVINFO_DATA DeviceInfoData);
HKEY (WINAPI *lpfnSetupDiOpenDevRegKey) (HDEVINFO DeviceInfoSet, PSP_DEVINFO_DATA DeviceInfoData, DWORD Scope, DWORD HwProfile, DWORD KeyType, REGSAM samDesired);
BOOL (WINAPI *lpfnSetupDiDestroyDeviceInfoList) ( HDEVINFO DeviceInfoSet);



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEdid::CEdid()
{
	OSVERSIONINFO	osVersion;

	osVersion.dwOSVersionInfoSize = sizeof( OSVERSIONINFO);
	if (GetVersionEx( &osVersion))
		m_bIsWin2K = (osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
				  (osVersion.dwMajorVersion >= 5);
	else
		// Failed so assume 9X based
		m_bIsWin2K = FALSE;
	m_hDll = NULL;
	m_hDevInfo = INVALID_HANDLE_VALUE;
}

CEdid::~CEdid()
{
	Disconnect();
}

BOOL CEdid::Connect(LPCTSTR lpstrDevice)
{
	if (!m_bIsWin2K && (lpstrDevice != NULL))
	{
		AddLog( _T( "SetupAPI Connect: Trying to connect to SetupAPI on device <%s>...Failed because remote operation unsupported under this OS !\n"), lpstrDevice);
		return FALSE;
	}
	AddLog( _T( "SetupAPI Connect: Trying to connect to SetupAPI on device <%s>..."), lpstrDevice == NULL ? _T( "Localhost") : lpstrDevice);

	// Load the Winsock 32 bit DLL
	if ((m_hDll = LoadLibrary( _T( "setupapi.dll"))) == NULL)
	{
		// Tell the user that we could not find a usable WinSock DLL.                                  
		AddLog( _T( "Failed to load <setupapi.dll> (Windows 95 or NT4 ?) !\n"));
		return FALSE;
	}
	// Load the SetupDiCreateDeviceInfoList function
	if (m_bIsWin2K)
	{
		// Win 2K or higher
#ifdef UNICODE
		if ((*(FARPROC*)&lpfnSetupDiCreateDeviceInfoListEx = GetProcAddress( m_hDll, "SetupDiCreateDeviceInfoListExW")) == NULL)
#else
		if ((*(FARPROC*)&lpfnSetupDiCreateDeviceInfoListEx = GetProcAddress( m_hDll, "SetupDiCreateDeviceInfoListExA")) == NULL)
#endif
		{
			// Tell the user that we could not find a usable SetupAPI DLL.                                  
			AddLog( _T( "Failed to load <SetupDiCreateDeviceInfoListEx> function from setupapi.dll !\n"));
			FreeLibrary( m_hDll);
			m_hDll = NULL;
			return FALSE;
		}
	}
	else
	{
		// Win 98/Me
		if ((*(FARPROC*)&lpfnSetupDiCreateDeviceInfoList = GetProcAddress( m_hDll, "SetupDiCreateDeviceInfoList")) == NULL)
		{
			// Tell the user that we could not find a usable SetupAPI DLL.                                  
			AddLog( _T( "Failed to load <SetupDiCreateDeviceInfoList> function from setupapi.dll !\n"));
			FreeLibrary( m_hDll);
			m_hDll = NULL;
			return FALSE;
		}
	}
	// Load the SetupDiGetClassDevsEx function
	if (m_bIsWin2K)
	{
		// Win 2K or higher
#ifdef UNICODE
		if ((*(FARPROC*)&lpfnSetupDiGetClassDevsEx = GetProcAddress( m_hDll, "SetupDiGetClassDevsExW")) == NULL)
#else
		if ((*(FARPROC*)&lpfnSetupDiGetClassDevsEx = GetProcAddress( m_hDll, "SetupDiGetClassDevsExA")) == NULL)
#endif
		{
			// Tell the user that we could not find a usable SetupAPI DLL.                                  
			AddLog( _T( "Failed to load <SetupDiGetClassDevsEx> function from setupapi.dll !\n"));
			FreeLibrary( m_hDll);
			m_hDll = NULL;
			return FALSE;
		}
	}
	else
	{
		// Win 98/Me
#ifdef UNICODE
		if ((*(FARPROC*)&lpfnSetupDiGetClassDevs = GetProcAddress( m_hDll, "SetupDiGetClassDevsW")) == NULL)
#else
		if ((*(FARPROC*)&lpfnSetupDiGetClassDevs = GetProcAddress( m_hDll, "SetupDiGetClassDevsA")) == NULL)
#endif
		{
			// Tell the user that we could not find a usable SetupAPI DLL.                                  
			AddLog( _T( "Failed to load <SetupDiCreateDeviceInfoList> function from setupapi.dll !\n"));
			FreeLibrary( m_hDll);
			m_hDll = NULL;
			return FALSE;
		}
	}
	// Load the SetupDiEnumDeviceInfo function
	if ((*(FARPROC*)&lpfnSetupDiEnumDeviceInfo = GetProcAddress( m_hDll, "SetupDiEnumDeviceInfo")) == NULL)
	{
		// Tell the user that we could not find a usable SetupAPI DLL.                                  
		AddLog( _T( "Failed to load <SetupDiEnumDeviceInfo> function from setupapi.dll !\n"));
		FreeLibrary( m_hDll);
		m_hDll = NULL;
		return FALSE;
	}
	// Load the SetupDiOpenDevRegKey function
	if ((*(FARPROC*)&lpfnSetupDiOpenDevRegKey = GetProcAddress( m_hDll, "SetupDiOpenDevRegKey")) == NULL)
	{
		// Tell the user that we could not find a usable SetupAPI DLL.                                  
		AddLog( _T( "Failed to load <SetupDiOpenDevRegKey> function from setupapi.dll !\n"));
		FreeLibrary( m_hDll);
		m_hDll = NULL;
		return FALSE;
	}
	// Load the SetupDiDestroyDeviceInfoList function
	if ((*(FARPROC*)&lpfnSetupDiDestroyDeviceInfoList = GetProcAddress( m_hDll, "SetupDiDestroyDeviceInfoList")) == NULL)
	{
		// Tell the user that we could not find a usable SetupAPI DLL.                                  
		AddLog( _T( "Failed to load <SetupDiDestroyDeviceInfoList> function from setupapi.dll !\n"));
		FreeLibrary( m_hDll);
		m_hDll = NULL;
		return FALSE;
	}
    // Create a device information set that will be the container for 
    // the device interfaces.
	if (m_bIsWin2K)
		m_hDevInfo = lpfnSetupDiCreateDeviceInfoListEx( NULL, NULL, lpstrDevice, NULL);
	else
		m_hDevInfo = lpfnSetupDiCreateDeviceInfoList( NULL, NULL);
    if (m_hDevInfo == INVALID_HANDLE_VALUE)
    {
		// Tell the user that we could not find a usable SetupAPI DLL.                                  
		AddLog( _T( "Failed in call to <SetupDiCreateDeviceInfoList> function from setupapi.dll !\n"));
		FreeLibrary( m_hDll);
		m_hDll = NULL;
		return FALSE;
    }
	m_csDevice = lpstrDevice;
	AddLog( _T( "OK.\n"));
	return TRUE;
}

BOOL CEdid::Disconnect()
{
	if ((m_hDll != NULL) && (m_hDevInfo != INVALID_HANDLE_VALUE))
	{
		/* TODO: Sometimes crashes on some computers
		try
		{
			BOOL okDestroy = SetupDiDestroyDeviceInfoList( m_hDevInfo);
		}
		catch( CException * pE )
		{
			pE->Delete();
		}
		*/
	}
	m_hDevInfo = INVALID_HANDLE_VALUE;
	if (m_hDll != NULL)
	{
		FreeLibrary( m_hDll);
		AddLog( _T( "SetupAPI Disconnect: Disconnected from SetupAPI.\n"));
	}
	m_hDll = NULL;
	return TRUE;
}

BOOL CEdid::ParseEDID(LPBYTE lpByte, Standard_EDID &myEDID)
{
	EDIDRecord tmpEDID;  //données brutes

	// copie les données brutes dans la structure donnée brute
	memcpy( &tmpEDID, lpByte, sizeof(EDIDRecord));

	// transfert les données dans la structure préformatée
	myEDID.Checksum = tmpEDID.Checksum;

	//copie toutes les infos Chroma Information
	memcpy( &(myEDID.Chroma_Information_Green_X_Y_Red_X_Y), &(tmpEDID.Chroma_Information_Green_X_Y_Red_X_Y), 10*sizeof(BYTE));

	// copie les 4 Detailed_Timing_Descriptions
	memcpy( &(myEDID.Detailed_Timing_Description1), &(tmpEDID.Detailed_Timing_Description1), 72);

	// copie les autres valeurs
	myEDID.DPMS_Flags = tmpEDID.DPMS_Flags;
	myEDID.EDID_ID_Code = tmpEDID.EDID_ID_Code;
	myEDID.EDID_Revision = tmpEDID.EDID_Revision;
	myEDID.EDID_Version = tmpEDID.EDID_Version;
	myEDID.Established_Timings_1 = tmpEDID.Established_Timings_1;
	myEDID.Established_Timings_2 = tmpEDID.Established_Timings_2;
	myEDID.Gamma_Factor = tmpEDID.Gamma_Factor;
	myEDID.Manufacture_Year = int(tmpEDID.Manufacture_Year) + 1990;
	_tcsncpy( myEDID.Manufacturer_ID, GetManufacturerID(tmpEDID.Manufacturer_ID), 4);
	myEDID.Manufacturer_Reserved_Timing = tmpEDID.Manufacturer_Reserved_Timing;
	myEDID.Maximum_Horizontal_Size = tmpEDID.Maximum_Horizontal_Size;
	myEDID.Maximum_Vertical_Size = tmpEDID.Maximum_Vertical_Size;
	myEDID.Serial_Number = tmpEDID.Serial_Number;
	//myEDID.Standard_Timing_Identification = DecodeStandardTimingIdentification(tmpEDID.Standard_Timing_Identification);
	myEDID.Video_Input_Type = tmpEDID.Video_Input_Type;
	myEDID.Week_Number_Manufacture = tmpEDID.Week_Number_Manufacture;

	return TRUE;
}

DetailTiming CEdid::GetDetailledTimingDescriptionType(BYTE Detailed_Timing_Descript[])
{
	if ((Detailed_Timing_Descript[0] == 0) &&
		(Detailed_Timing_Descript[1] == 0) &&
		(Detailed_Timing_Descript[2] == 0))
	{
		switch (Detailed_Timing_Descript[3])
		{
		case 0xFF:
			return Serial_Number;
		case 0xFE:
			return Vendor_Name;
		case 0xFD:
			return Frequency_Range;
		case 0xFC:
			return Model_Name;
		}
	}
	return Detailed_Timing_Description;

}

LPCTSTR CEdid::GetManufacturerID(BYTE ID[2])
{
	int		littleEndianID, // ID en little-endian
			i = ID[0];
	BYTE	FirstLetter,
			SecondLetter,
			ThirdLetter; // les trois lettres de l'ID
	static CString strID;

	// transforme le big-endian en little-endian
	littleEndianID = ID[1] ;
	littleEndianID += i << 8;

	// trouve la valeur de chaque lettre (01=A, 02=B,...)
	ThirdLetter = littleEndianID & (1 + 2 + 4 + 8 + 16);
	SecondLetter = (littleEndianID & (32 + 64 + 128 + 256 + 512)) / 32;
	FirstLetter = (littleEndianID & (1024 + 2048 + 4096 + 8192 + 16384)) / 1024;
	// renvoie l'ID dans une chaine de 3 caracteres
	strID.Format( _T( "%c%c%c"), 64 + FirstLetter, 64 + SecondLetter, 64 + ThirdLetter);
	return strID;
}

LPCTSTR CEdid::GetManufacturerName(LPCTSTR lpstrID)
{
	if (_tcsicmp( lpstrID, _T( "ACR")) == 0)
		return _T("Acer, Inc.");
	else if (_tcsicmp( lpstrID, _T( "ACT")) == 0)
		return _T(  "Targa");
	else if (_tcsicmp( lpstrID, _T( "ADI")) == 0)
		return _T(  "ADI Corporation http://www.adi.com.tw");
	else if (_tcsicmp( lpstrID, _T( "AOC")) == 0)
		return _T(  "AOC International (USA) Ltd.");
	else if (_tcsicmp( lpstrID, _T( "API")) == 0)
		return _T(  "Acer America Corp.");
	else if (_tcsicmp( lpstrID, _T( "APP")) == 0)
		return _T(  "Apple Computer, Inc.");
	else if (_tcsicmp( lpstrID, _T( "ART")) == 0)
		return _T(  "ArtMedia");
	else if (_tcsicmp( lpstrID, _T( "AST")) == 0)
		return _T(  "AST Research");
	else if (_tcsicmp( lpstrID, _T( "CPL")) == 0)
		return _T(  "Compal Electronics, Inc. / ALFA");
	else if (_tcsicmp( lpstrID, _T( "CPQ")) == 0)
		return _T(  "COMPAQ Computer Corp.");
	else if (_tcsicmp( lpstrID, _T( "CTX")) == 0)
		return _T(  "CTX - Chuntex Electronic Co.");
	else if (_tcsicmp( lpstrID, _T( "DEC")) == 0)
		return _T(  "Digital Equipment Corporation");
	else if (_tcsicmp( lpstrID, _T( "DEL")) == 0)
		return _T(  "Dell Computer Corp.");
	else if (_tcsicmp( lpstrID, _T( "DPC")) == 0)
		return _T(  "Delta Electronics, Inc.");
	else if (_tcsicmp( lpstrID, _T( "DWE")) == 0)
		return _T(  "Daewoo Telecom Ltd");
	else if (_tcsicmp( lpstrID, _T( "ECS")) == 0)
		return _T(  "ELITEGROUP Computer Systems");
	else if (_tcsicmp( lpstrID, _T( "EIZ")) == 0)
		return _T(  "EIZO");
	else if (_tcsicmp( lpstrID, _T( "FCM")) == 0)
		return _T(  "Funai Electric Company of Taiwan");
	else if (_tcsicmp( lpstrID, _T( "FUS")) == 0)
		return _T(  "Fujitsu Siemens");
	else if (_tcsicmp( lpstrID, _T( "GSM")) == 0)
		return _T(  "LG Electronics Inc. (GoldStar Technology, Inc.)");
	else if (_tcsicmp( lpstrID, _T( "GWY")) == 0)
		return _T(  "Gateway 2000");
	else if (_tcsicmp( lpstrID, _T( "HEI")) == 0)
		return _T(  "Hyundai Electronics Industries Co., Ltd.");
	else if (_tcsicmp( lpstrID, _T( "HIT")) == 0)
		return _T(  "Hitachi");
	else if (_tcsicmp( lpstrID, _T( "HSL")) == 0)
		return _T(  "Hansol Electronics");
	else if (_tcsicmp( lpstrID, _T( "HTC")) == 0)
		return _T(  "Hitachi Ltd. / Nissei Sangyo America Ltd.");
	else if (_tcsicmp( lpstrID, _T( "HWP")) == 0)
		return _T(  "Hewlett Packard");
	else if (_tcsicmp( lpstrID, _T( "IBM")) == 0)
		return _T(  "IBM PC Company");
	else if (_tcsicmp( lpstrID, _T( "ICL")) == 0)
		return _T(  "Fujitsu ICL");
	else if (_tcsicmp( lpstrID, _T( "IVM")) == 0)
		return _T(  "Idek Iiyama North America, Inc.");
	else if (_tcsicmp( lpstrID, _T( "KFC")) == 0)
		return _T(  "KFC Computek");
	else if (_tcsicmp( lpstrID, _T( "LKM")) == 0)
		return _T(  "ADLAS / AZALEA");
	else if (_tcsicmp( lpstrID, _T( "LNK")) == 0)
		return _T(  "LINK Technologies, Inc.");
	else if (_tcsicmp( lpstrID, _T( "LTN")) == 0)
		return _T(  "Lite-On");
	else if (_tcsicmp( lpstrID, _T( "MAG")) == 0)
		return _T(  "MAG InnoVision");
	else if (_tcsicmp( lpstrID, _T( "MAX")) == 0)
		return _T(  "Maxdata Computer GmbH");
	else if (_tcsicmp( lpstrID, _T( "MEI")) == 0)
		return _T(  "Panasonic Comm. & Systems Co.");
	else if (_tcsicmp( lpstrID, _T( "MEL")) == 0)
		return _T(  "Mitsubishi Electronics");
	else if (_tcsicmp( lpstrID, _T( "MIR")) == 0)
		return _T(  "miro Computer Products AG");
	else if (_tcsicmp( lpstrID, _T( "MTC")) == 0)
		return _T(  "MITAC");
	else if (_tcsicmp( lpstrID, _T( "NAN")) == 0)
		return _T(  "NANAO");
	else if (_tcsicmp( lpstrID, _T( "NEC")) == 0)
		return _T(  "NEC Technologies, Inc.");
	else if (_tcsicmp( lpstrID, _T( "NOK")) == 0)
		return _T(  "Nokia");
	else if (_tcsicmp( lpstrID, _T( "OQI")) == 0)
		return _T(  "OPTIQUEST");
	else if (_tcsicmp( lpstrID, _T( "PBN")) == 0)
		return _T(  "Packard Bell");
	else if (_tcsicmp( lpstrID, _T( "PGS")) == 0)
		return _T(  "Princeton Graphic Systems");
	else if (_tcsicmp( lpstrID, _T( "PHL")) == 0)
		return _T(  "Philips Consumer Electronics Co.");
	else if (_tcsicmp( lpstrID, _T( "REL")) == 0)
		return _T(  "Relisys");
	else if (_tcsicmp( lpstrID, _T( "SAM")) == 0)
		return _T(  "Samsung");
	else if (_tcsicmp( lpstrID, _T( "SDI")) == 0)
		return _T(  "Samtron");
	else if (_tcsicmp( lpstrID, _T( "SEC")) == 0)
		return _T(  "Fujitsu Siemens");
	else if (_tcsicmp( lpstrID, _T( "SMI")) == 0)
		return _T(  "Smile");
	else if (_tcsicmp( lpstrID, _T( "SNI")) == 0)
		return _T(  "Siemens Nixdorf");
	else if (_tcsicmp( lpstrID, _T( "SNY")) == 0)
		return _T(  "Sony Corporation");
	else if (_tcsicmp( lpstrID, _T( "SPT")) == 0)
		return _T(  "Sceptre");
	else if (_tcsicmp( lpstrID, _T( "SRC")) == 0)
		return _T(  "Shamrock Technology");
	else if (_tcsicmp( lpstrID, _T( "STP")) == 0)
		return _T(  "Sceptre");
	else if (_tcsicmp( lpstrID, _T( "TAT")) == 0)
		return _T(  "Tatung Co. of America, Inc.");
	else if (_tcsicmp( lpstrID, _T( "TRL")) == 0)
		return _T(  "Royal Information Company");
	else if (_tcsicmp( lpstrID, _T( "TSB")) == 0)
		return _T(  "Toshiba, Inc.");
	else if (_tcsicmp( lpstrID, _T( "UNM")) == 0)
		return _T(  "Unisys Corporation");
	else if (_tcsicmp( lpstrID, _T( "VSC")) == 0)
		return _T(  "ViewSonic Corporation");
	else if (_tcsicmp( lpstrID, _T( "WTC")) == 0)
		return _T(  "Wen Technology");
	else if (_tcsicmp( lpstrID, _T( "ZCM")) == 0)
		return _T(  "Zenith Data Systems");
	else if (_tcsicmp( lpstrID, _T( "___")) == 0)
		return _T(  "Targa");
	m_csBuffer.Format( _T( "Unknown manufacturer code %s"), lpstrID);
	return m_csBuffer;
}

LPCTSTR CEdid::GetEdidText(BYTE lpByte[18])
{
	static TCHAR	szResult[15];
	int i;

	for (i=0; i<18;i++)
	{
		if (lpByte[i]==0) lpByte[i]=' ';
		if (lpByte[i]==10) lpByte[i]=0;
	}
	_tcsncpy( szResult, (LPCTSTR) (lpByte+4), 14);
	return szResult;
}

LPCTSTR CEdid::DecodeDPMSFlag(BYTE Flag)
{
	// 2 - 0
	// unused???
	// Display type
	if ((Flag & 8) == 8)
		m_csBuffer = _T( "RGB color");
	else
		m_csBuffer = _T( "non-RGB multicolor");
 
	// 4
	// unused???
	// Power support
/*	if ((Flag & 32) == 32)
		m_csBuffer += _T( ", Active Off supported");
	if ((Flag & 64) == 64)
		m_csBuffer += _T( ", Suspend supported");
	if ((Flag & 128) == 128)
		m_csBuffer += _T( ", Standby supported");
*/	return m_csBuffer;
}

BOOL CEdid::GetDisplayEDID(HDEVINFO hDeviceInfoSet, SP_DEVINFO_DATA *pDevInfoData, Standard_EDID &myEDID)
{
	HKEY	hKey;
	LPBYTE	lpByte = NULL;
	DWORD	dwType, dwSize = 0;
	LONG	lErr;

	if ((hKey = lpfnSetupDiOpenDevRegKey( hDeviceInfoSet, pDevInfoData, DICS_FLAG_GLOBAL, NULL, DIREG_DEV, KEY_QUERY_VALUE)) != NULL)
	{
		// Get size of byte array
		RegQueryValueEx( hKey, _T( "EDID"), NULL, &dwType, lpByte, &dwSize);
		if ((lpByte = (LPBYTE) malloc( (dwSize+1)*sizeof(BYTE))) == NULL)
		{
			AddLog( _T( "\tSetupAPI: malloc failed with error %ld.\n"), GetLastError());
			RegCloseKey( hKey);
			return FALSE;
		}
		if ((lErr = RegQueryValueEx( hKey, _T( "EDID"), NULL, &dwType, lpByte, &dwSize)) == ERROR_SUCCESS)
		{
			lpByte[dwSize]=0;
			
			ParseEDID( lpByte, myEDID);
			free( lpByte);
			RegCloseKey( hKey);
		}
		else
		{
			AddLog( _T( "\tSetupAPI: RegQueryValueEx failed with error %ld.\n"), lErr);
			free( lpByte);
			RegCloseKey( hKey);
			return FALSE;
		}
	}
	else
	{
		AddLog( _T( "\tSetupAPI: SetupDiOpenDevRegKey failed with error %ld.\n"), GetLastError());
		return FALSE;
	}
	return TRUE;
}

void CEdid::AcerHack (CMonitor *myMonitor, Standard_EDID *myRecord)
{
	TCHAR Buf1[32], Buf2[32], Buffer[32];

	AddLog( _T( "\tEDID : Monitor %s.%04X.%8X (%s)\n"), 
		myRecord->Manufacturer_ID, (DWORD)myRecord->EDID_ID_Code, (DWORD)myRecord->Serial_Number,
		myMonitor->GetSerial());
	
	if (!lstrcmpi(myRecord->Manufacturer_ID, _T( "ACR")))
	{
		// This an Acer monitor
		lstrcpyn(Buf1, myMonitor->GetSerial(), sizeof(Buf1));
		if (lstrlen( Buf1)==12) {
			// Heuristic confirm for
			// AL1916 (0xAD49), AL1923 (0x0783) B223W (0x0018 et 0x0020)
			// P243W  (0xADAF), X233H  (0x00A8)
			wsprintf( Buf2, _T( "%08x"), myRecord->Serial_Number);
			lstrcpyn( Buffer,    Buf1, 9);
			lstrcpyn( Buffer+8,  Buf2, 9);
			lstrcpyn(Buffer+16, Buf1+8, 5);

			AddLog( _T( "\tEDID Acer Fix: Change Serial Number to %s\n"), Buffer);
			myMonitor->SetSerial( Buffer);

		}
	}
}

LPCTSTR CEdid::GetDescription(Standard_EDID *myRecord)
{
	static TCHAR	szResult[64];

	// Standard OCS agent
	// wsprintf(szResult, "%d/%d", myRecord->Week_Number_Manufacture, myRecord->Manufacture_Year);

	// Gediff agent
	wsprintf(szResult, _T( "%s.%04X.%08X (%d/%d)"), 
		myRecord->Manufacturer_ID, (DWORD)myRecord->EDID_ID_Code, (DWORD)myRecord->Serial_Number,
		myRecord->Week_Number_Manufacture, myRecord->Manufacture_Year);

	return szResult;
}

BOOL CEdid::GetMonitors(CMonitorList *pMyList)
{
    HDEVINFO		hDeviceInfoSet;
	SP_DEVINFO_DATA spDeviceInfo;
	CString			csBuffer;
	DWORD			dwIndex = 0;
	Standard_EDID	myRecord;
	CMonitor		myMonitor;

    // Retrieve the device information set for the interface class.
	// First, try DISPLAY devices
	AddLog( _T( "SetupAPI: Enumerating DISPLAY devices...\n"));
	if (m_bIsWin2K)
		hDeviceInfoSet = lpfnSetupDiGetClassDevsEx( NULL, _T( "DISPLAY"), NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES, m_hDevInfo, m_csDevice.IsEmpty() ? NULL : LPCTSTR( m_csDevice), NULL);
	else
		hDeviceInfoSet = lpfnSetupDiGetClassDevs( NULL, _T( "DISPLAY"), NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);

	if (hDeviceInfoSet != INVALID_HANDLE_VALUE)
	{
		spDeviceInfo.cbSize = sizeof( SP_DEVINFO_DATA);
		while (lpfnSetupDiEnumDeviceInfo( hDeviceInfoSet, dwIndex, &spDeviceInfo))
		{
			myMonitor.Clear();
			if (GetDisplayEDID( hDeviceInfoSet, &spDeviceInfo, myRecord))
			{
				myMonitor.SetManufacturer( GetManufacturerName( myRecord.Manufacturer_ID));
				switch (GetDetailledTimingDescriptionType( myRecord.Detailed_Timing_Description1))
				{
				case Serial_Number:
					myMonitor.SetDescription( GetDescription(&myRecord));
					myMonitor.SetSerial(GetEdidText( myRecord.Detailed_Timing_Description1));
					break;
				case Model_Name:
					myMonitor.SetCaption( GetEdidText( myRecord.Detailed_Timing_Description1));
					break;
				}
				switch (GetDetailledTimingDescriptionType( myRecord.Detailed_Timing_Description2))
				{
				case Serial_Number:
					myMonitor.SetDescription( GetDescription(&myRecord));
					myMonitor.SetSerial(GetEdidText( myRecord.Detailed_Timing_Description2));
					break;
				case Model_Name:
					myMonitor.SetCaption( GetEdidText( myRecord.Detailed_Timing_Description2));
					break;
				}
				switch (GetDetailledTimingDescriptionType( myRecord.Detailed_Timing_Description3))
				{
				case Serial_Number:
					myMonitor.SetDescription( GetDescription(&myRecord));
					myMonitor.SetSerial(GetEdidText( myRecord.Detailed_Timing_Description3));
					break;
				case Model_Name:
					myMonitor.SetCaption( GetEdidText( myRecord.Detailed_Timing_Description3));
					break;
				}
				switch (GetDetailledTimingDescriptionType( myRecord.Detailed_Timing_Description4))
				{
				case Serial_Number:
					myMonitor.SetDescription( GetDescription(&myRecord));
					myMonitor.SetSerial(GetEdidText( myRecord.Detailed_Timing_Description4));
					break;
				case Model_Name:
					myMonitor.SetCaption( GetEdidText( myRecord.Detailed_Timing_Description4));
					break;
				}
				myMonitor.SetType( DecodeDPMSFlag( myRecord.DPMS_Flags));
				AcerHack(&myMonitor, &myRecord);
				pMyList->AddTail( myMonitor);
			}
			dwIndex++;
		}
		if (GetLastError() != ERROR_NO_MORE_ITEMS)
		{
			AddLog( _T( "\tSetupAPI: SetupDiEnumDeviceInterfaces failed with error %ld.\n"), GetLastError());
			lpfnSetupDiDestroyDeviceInfoList( hDeviceInfoSet);
			return FALSE;
		}
	}
	else
	{
			AddLog( _T( "\tSetupAPI: SetupDiGetClassDevs DISPLAY failed with error %ld.\n"), GetLastError());
	}
	lpfnSetupDiDestroyDeviceInfoList( hDeviceInfoSet);
	AddLog( _T( "SetupAPI: Enumerates DISPLAY devices finished (%ld objects)...\n"), dwIndex);
	// Next, try MONITOR devices
	dwIndex = 0;
	AddLog( _T( "SetupAPI: Enumerating MONITOR devices...\n"));
	if (m_bIsWin2K)
		hDeviceInfoSet = lpfnSetupDiGetClassDevsEx( NULL, _T( "MONITOR"), NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES, m_hDevInfo, m_csDevice.IsEmpty() ? NULL : LPCTSTR( m_csDevice), NULL);
	else
		hDeviceInfoSet = lpfnSetupDiGetClassDevs( NULL, _T( "MONITOR"), NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    if (hDeviceInfoSet != INVALID_HANDLE_VALUE)
	{
		spDeviceInfo.cbSize = sizeof( SP_DEVINFO_DATA);
		while (lpfnSetupDiEnumDeviceInfo( hDeviceInfoSet, dwIndex, &spDeviceInfo))
		{
			myMonitor.Clear();
			if (GetDisplayEDID( hDeviceInfoSet, &spDeviceInfo, myRecord))
			{
				myMonitor.SetManufacturer( GetManufacturerName( myRecord.Manufacturer_ID));
				switch (GetDetailledTimingDescriptionType( myRecord.Detailed_Timing_Description1))
				{
				case Serial_Number:
					myMonitor.SetDescription( GetDescription(&myRecord));
					myMonitor.SetSerial(GetEdidText( myRecord.Detailed_Timing_Description1));
					break;
				case Model_Name:
					myMonitor.SetCaption( GetEdidText( myRecord.Detailed_Timing_Description1));
					break;
				}
				switch (GetDetailledTimingDescriptionType( myRecord.Detailed_Timing_Description2))
				{
				case Serial_Number:
					myMonitor.SetDescription( GetDescription(&myRecord));
					myMonitor.SetSerial(GetEdidText( myRecord.Detailed_Timing_Description2));
					break;
				case Model_Name:
					myMonitor.SetCaption( GetEdidText( myRecord.Detailed_Timing_Description2));
					break;
				}
				switch (GetDetailledTimingDescriptionType( myRecord.Detailed_Timing_Description3))
				{
				case Serial_Number:
					myMonitor.SetDescription( GetDescription(&myRecord));
					myMonitor.SetSerial(GetEdidText( myRecord.Detailed_Timing_Description3));
					break;
				case Model_Name:
					myMonitor.SetCaption( GetEdidText( myRecord.Detailed_Timing_Description3));
					break;
				}
				switch (GetDetailledTimingDescriptionType( myRecord.Detailed_Timing_Description4))
				{
				case Serial_Number:
					myMonitor.SetDescription( GetDescription(&myRecord));
					myMonitor.SetSerial(GetEdidText( myRecord.Detailed_Timing_Description4));
					break;
				case Model_Name:
					myMonitor.SetCaption( GetEdidText( myRecord.Detailed_Timing_Description4));
					break;
				}
				myMonitor.SetType( DecodeDPMSFlag( myRecord.DPMS_Flags));
				AcerHack(&myMonitor, &myRecord);
				pMyList->AddTail( myMonitor);
			}
			dwIndex++;
		}
		if (GetLastError() != ERROR_NO_MORE_ITEMS)
		{
			AddLog( _T( "\tSetupAPI: SetupDiEnumDeviceInterfaces failed with error %ld.\n"), GetLastError());
			lpfnSetupDiDestroyDeviceInfoList( hDeviceInfoSet);
			return FALSE;
		}
	}
	else
	{
			AddLog( _T( "\tSetupAPI: SetupDiGetClassDevs MONITOR failed with error %ld.\n"), GetLastError());
	}

	if( hDeviceInfoSet != INVALID_HANDLE_VALUE )
		lpfnSetupDiDestroyDeviceInfoList( hDeviceInfoSet);
	AddLog( _T( "SetupAPI: Enumerates MONITOR devices finished (%ld objects)...\n"), dwIndex);
	return (pMyList->GetCount() > 0);
}

BOOL CEdid::IsConnected()
{
	return (m_hDevInfo != INVALID_HANDLE_VALUE);
}

