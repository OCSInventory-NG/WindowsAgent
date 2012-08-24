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
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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
	EDIDRecord tmpEDID;  // EDID data structure

	// Copy byte to EDID data structure
	memcpy( &tmpEDID, lpByte, sizeof( EDIDRecord));

	// Copy some well non data to EDID data structure
	myEDID.Checksum = tmpEDID.Checksum;

	// Chroma Information
	memcpy( &(myEDID.Chroma_Information_Green_X_Y_Red_X_Y), &(tmpEDID.Chroma_Information_Green_X_Y_Red_X_Y), 10*sizeof(BYTE));

	// 4 Detailed_Timing_Descriptions
	memcpy( &(myEDID.Detailed_Timing_Description1), &(tmpEDID.Detailed_Timing_Description1), 72);

	// Others values
	myEDID.DPMS_Flags = tmpEDID.DPMS_Flags;
	myEDID.EDID_ID_Code = tmpEDID.EDID_ID_Code;
	myEDID.EDID_Revision = tmpEDID.EDID_Revision;
	myEDID.EDID_Version = tmpEDID.EDID_Version;
	myEDID.Established_Timings_1 = tmpEDID.Established_Timings_1;
	myEDID.Established_Timings_2 = tmpEDID.Established_Timings_2;
	myEDID.Gamma_Factor = tmpEDID.Gamma_Factor;
	myEDID.Manufacture_Year = int( tmpEDID.Manufacture_Year) + 1990;
	strncpy( myEDID.Manufacturer_ID, GetManufacturerID( tmpEDID.Manufacturer_ID), 4);
	myEDID.Manufacturer_Reserved_Timing = tmpEDID.Manufacturer_Reserved_Timing;
	myEDID.Maximum_Horizontal_Size = tmpEDID.Maximum_Horizontal_Size;
	myEDID.Maximum_Vertical_Size = tmpEDID.Maximum_Vertical_Size;
	myEDID.Serial_Number = tmpEDID.Serial_Number;
//	myEDID.Standard_Timing_Identification = DecodeStandardTimingIdentification(tmpEDID.Standard_Timing_Identification);
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

LPCSTR CEdid::GetManufacturerID(BYTE ID[2])
{
	int		littleEndianID, // ID en little-endian
			i = ID[0];
	BYTE	FirstLetter,
			SecondLetter,
			ThirdLetter; // les trois lettres de l'ID

	// transforme le big-endian en little-endian
	littleEndianID = ID[1] ;
	littleEndianID += i << 8;

	// trouve la valeur de chaque lettre (01=A, 02=B,...)
	ThirdLetter = littleEndianID & (1 + 2 + 4 + 8 + 16);
	SecondLetter = (littleEndianID & (32 + 64 + 128 + 256 + 512)) / 32;
	FirstLetter = (littleEndianID & (1024 + 2048 + 4096 + 8192 + 16384)) / 1024;
	// renvoie l'ID dans une chaine de 3 caracteres
	m_csBuffer.Format( "%c%c%c", 64 + FirstLetter, 64 + SecondLetter, 64 + ThirdLetter);
	return m_csBuffer;
}

LPCSTR CEdid::GetManufacturerName(LPCSTR lpstrID)
{
	if (_strnicmp( lpstrID, "ACR", 3) == 0)
		return "Acer, Inc.";
	else if (_strnicmp( lpstrID, "ACT", 3) == 0)
		return "Targa";
	else if (_strnicmp( lpstrID, "ACI", 3) == 0)
		return "Asus Computer Inc.";
	else if (_strnicmp( lpstrID, "ADI", 3) == 0)
		return "ADI Corporation http://www.adi.com.tw";
	else if (_strnicmp( lpstrID, "AOC", 3) == 0)
		return "AOC International (USA) Ltd.";
	else if (_strnicmp( lpstrID, "API", 3) == 0)
		return "Acer America Corp.";
	else if (_strnicmp( lpstrID, "APP", 3) == 0)
		return "Apple Computer, Inc.";
	else if (_strnicmp( lpstrID, "ART", 3) == 0)
		return "ArtMedia";
	else if (_strnicmp( lpstrID, "AST", 3) == 0)
		return "AST Research";
	else if (_strnicmp( lpstrID, "BNQ", 3) == 0)
		return "BenQ Corporation";
	else if (_strnicmp( lpstrID, "CPL", 3) == 0)
		return "Compal Electronics, Inc. / ALFA";
	else if (_strnicmp( lpstrID, "CPQ", 3) == 0)
		return "COMPAQ Computer Corp.";
	else if (_strnicmp( lpstrID, "CPT", 3) == 0)
		return "Chunghwa Picture Tubes, Ltd.";
	else if (_strnicmp( lpstrID, "CTX", 3) == 0)
		return "CTX - Chuntex Electronic Co.";
	else if (_strnicmp( lpstrID, "DEC", 3) == 0)
		return "Digital Equipment Corporation";
	else if (_strnicmp( lpstrID, "DEL", 3) == 0)
		return "Dell Computer Corp.";
	else if (_strnicmp( lpstrID, "DPC", 3) == 0)
		return "Delta Electronics, Inc.";
	else if (_strnicmp( lpstrID, "DWE", 3) == 0)
		return "Daewoo Telecom Ltd";
	else if (_strnicmp( lpstrID, "ECS", 3) == 0)
		return "ELITEGROUP Computer Systems";
	else if (_strnicmp( lpstrID, "EIZ", 3) == 0)
		return "EIZO";
	else if (_strnicmp( lpstrID, "ENC", 3) == 0)
		return "EIZO";
	else if (_strnicmp( lpstrID, "FCM", 3) == 0)
		return "Funai Electric Company of Taiwan";
	else if (_strnicmp( lpstrID, "FUS", 3) == 0)
		return "Fujitsu Siemens";
	else if (_strnicmp( lpstrID, "FUJ", 3) == 0)
		return "Fujitsu Siemens";
	else if (_strnicmp( lpstrID, "GSM", 3) == 0)
		return "LG Electronics Inc. (GoldStar Technology, Inc.)";
	else if (_strnicmp( lpstrID, "GWY", 3) == 0)
		return "Gateway 2000";
	else if (_strnicmp( lpstrID, "HEI", 3) == 0)
		return "Hyundai Electronics Industries Co., Ltd.";
	else if (_strnicmp( lpstrID, "HIT", 3) == 0)
		return "Hitachi";
	else if (_strnicmp( lpstrID, "HSD", 3) == 0)
		return "Hannspree Inc";
	else if (_strnicmp( lpstrID, "HSL", 3) == 0)
		return "Hansol Electronics";
	else if (_strnicmp( lpstrID, "HTC", 3) == 0)
		return "Hitachi Ltd. / Nissei Sangyo America Ltd.";
	else if (_strnicmp( lpstrID, "HWP", 3) == 0)
		return "Hewlett Packard";
	else if (_strnicmp( lpstrID, "IBM", 3) == 0)
		return "IBM PC Company";
	else if (_strnicmp( lpstrID, "ICL", 3) == 0)
		return "Fujitsu ICL";
	else if (_strnicmp( lpstrID, "IVM", 3) == 0)
		return "Idek Iiyama North America, Inc.";
	else if (_strnicmp( lpstrID, "KFC", 3) == 0)
		return "KFC Computek";
	else if (_strnicmp( lpstrID, "LEN", 3) == 0)
		return "Lenovo";
	else if (_strnicmp( lpstrID, "LGD", 3) == 0)
		return "Asus Computer Inc.";
	else if (_strnicmp( lpstrID, "LKM", 3) == 0)
		return "ADLAS / AZALEA";
	else if (_strnicmp( lpstrID, "LNK", 3) == 0)
		return "LINK Technologies, Inc.";
	else if (_strnicmp( lpstrID, "LTN", 3) == 0)
		return "Lite-On";
	else if (_strnicmp( lpstrID, "MAG", 3) == 0)
		return "MAG InnoVision";
	else if (_strnicmp( lpstrID, "MAX", 3) == 0)
		return "Maxdata Computer GmbH";
	else if (_strnicmp( lpstrID, "MEI", 3) == 0)
		return "Panasonic Comm. & Systems Co.";
	else if (_strnicmp( lpstrID, "MEL", 3) == 0)
		return "Mitsubishi Electronics";
	else if (_strnicmp( lpstrID, "MIR", 3) == 0)
		return "miro Computer Products AG";
	else if (_strnicmp( lpstrID, "MTC", 3) == 0)
		return "MITAC";
	else if (_strnicmp( lpstrID, "MS_", 3) == 0)
		return "Panasonic Comm. & Systems Co.";
	else if (_strnicmp( lpstrID, "NAN", 3) == 0)
		return "NANAO";
	else if (_strnicmp( lpstrID, "NEC", 3) == 0)
		return "NEC Technologies, Inc.";
	else if (_strnicmp( lpstrID, "NOK", 3) == 0)
		return "Nokia";
	else if (_strnicmp( lpstrID, "NVD", 3) == 0)
		return "Fujitsu Siemens";
	else if (_strnicmp( lpstrID, "OQI", 3) == 0)
		return "OPTIQUEST";
	else if (_strnicmp( lpstrID, "PBN", 3) == 0)
		return "Packard Bell";
	else if (_strnicmp( lpstrID, "PGS", 3) == 0)
		return "Princeton Graphic Systems";
	else if (_strnicmp( lpstrID, "PHL", 3) == 0)
		return "Philips Consumer Electronics Co.";
	else if (_strnicmp( lpstrID, "REL", 3) == 0)
		return "Relisys";
	else if (_strnicmp( lpstrID, "SAM", 3) == 0)
		return "Samsung";
	else if (_strnicmp( lpstrID, "SDI", 3) == 0)
		return "Samtron";
	else if (_strnicmp( lpstrID, "SEC", 3) == 0)
		return "Fujitsu Siemens";
	else if (_strnicmp( lpstrID, "SMI", 3) == 0)
		return "Smile";
	else if (_strnicmp( lpstrID, "SNI", 3) == 0)
		return "Siemens Nixdorf";
	else if (_strnicmp( lpstrID, "SNY", 3) == 0)
		return "Sony Corporation";
	else if (_strnicmp( lpstrID, "SPT", 3) == 0)
		return "Sceptre";
	else if (_strnicmp( lpstrID, "SRC", 3) == 0)
		return "Shamrock Technology";
	else if (_strnicmp( lpstrID, "STN", 3) == 0)
		return "Samtron";
	else if (_strnicmp( lpstrID, "STP", 3) == 0)
		return "Sceptre";
	else if (_strnicmp( lpstrID, "TAT", 3) == 0)
		return "Tatung Co. of America, Inc.";
	else if (_strnicmp( lpstrID, "TRL", 3) == 0)
		return "Royal Information Company";
	else if (_strnicmp( lpstrID, "TSB", 3) == 0)
		return "Toshiba, Inc.";
	else if (_strnicmp( lpstrID, "UNM", 3) == 0)
		return "Unisys Corporation";
	else if (_strnicmp( lpstrID, "VSC", 3) == 0)
		return "ViewSonic Corporation";
	else if (_strnicmp( lpstrID, "WTC", 3) == 0)
		return "Wen Technology";
	else if (_strnicmp( lpstrID, "ZCM", 3) == 0)
		return "Zenith Data Systems";
	else if (_strnicmp( lpstrID, "___", 3) == 0)
		return "Targa";
	m_csBuffer.Format( "Unknown manufacturer code %s", lpstrID);
	return m_csBuffer;
}

LPCSTR CEdid::GetEdidText(BYTE lpByte[18])
{
	char	szResult[15];
	size_t	i;

	for (i=0; i<18;i++)
	{
		if (lpByte[i]==0) lpByte[i]=' ';
		if (lpByte[i]==10) lpByte[i]=0;
	}
	memset( szResult, 0, 15);
	strncpy( szResult, (LPCSTR) (lpByte+DESCRIPTOR_DATA_OFFSET), 14);
	// Ignore space characters at begining
	for (i=0; i<strlen( szResult) && szResult[i]==' '; i++);
	m_csBuffer.Format( "%s", szResult+i);
	return m_csBuffer;
}

LPCSTR CEdid::DecodeDPMSFlag(BYTE Flag)
{
	// 2 - 0
	// unused???
	// Display type
	if ((Flag & 8) == 8)
		m_csBuffer = "RGB color";
	else
		m_csBuffer = "non-RGB multicolor";
 
	// 4
	// unused???
	// Power support
/*	if ((Flag & 32) == 32)
		m_csBuffer += ", Active Off supported";
	if ((Flag & 64) == 64)
		m_csBuffer += ", Suspend supported";
	if ((Flag & 128) == 128)
		m_csBuffer += ", Standby supported";
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

BOOL CEdid::AcerHack( CStringA &csSerial, Standard_EDID *myRecord)
{
	char szBuf1[32], szBuf2[32], szBuffer[32];

	if (_strnicmp( myRecord->Manufacturer_ID, "ACR", 3) == 0)
	{
		// This an Acer monitor
		strncpy( szBuf1, csSerial, sizeof( szBuf1));
		if (strlen( szBuf1)==12)
		{
			// Heuristic confirm for
			// AL1916 (0xAD49), AL1923 (0x0783) B223W (0x0018 et 0x0020)
			// P243W  (0xADAF), X233H  (0x00A8)
			sprintf( szBuf2, "%08X", myRecord->Serial_Number);
			strncpy( szBuffer, szBuf1, 9);
			strncpy( szBuffer+8, szBuf2, 9);
			strncpy( szBuffer+16, szBuf1+8, 5);
			csSerial = szBuffer;
			return TRUE;
		}
	}
	return FALSE;
}

LPCSTR CEdid::GetDescription(Standard_EDID *myRecord)
{
	m_csBuffer.Format( "%s.%04X.%08X (%d/%d)", 
		myRecord->Manufacturer_ID, (DWORD)myRecord->EDID_ID_Code, (DWORD)myRecord->Serial_Number,
		myRecord->Week_Number_Manufacture, myRecord->Manufacture_Year);

	return m_csBuffer;
}

BOOL CEdid::GetMonitors( CMonitorList *pMyList)
{
    HDEVINFO		hDeviceInfoSet;
	SP_DEVINFO_DATA spDeviceInfo;
	CString			csBuffer;
	DWORD			dwIndex = 0;
	Standard_EDID	myRecord;
	CMonitor		myMonitor;
	CStringA		csSerial,
					csCaption,
					csDescription;

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
			csCaption.Empty();
			csDescription.Empty();
			csSerial.Empty();
			if (GetDisplayEDID( hDeviceInfoSet, &spDeviceInfo, myRecord))
			{
				myMonitor.SetManufacturer( CA2T( GetManufacturerName( myRecord.Manufacturer_ID)));
				switch (GetDetailledTimingDescriptionType( myRecord.Detailed_Timing_Description1))
				{
				case Serial_Number:
					csDescription = GetDescription( &myRecord);
					csSerial = GetEdidText( myRecord.Detailed_Timing_Description1);
					break;
				case Model_Name:
					csCaption = GetEdidText( myRecord.Detailed_Timing_Description1);
					break;
				}
				switch (GetDetailledTimingDescriptionType( myRecord.Detailed_Timing_Description2))
				{
				case Serial_Number:
					csDescription = GetDescription( &myRecord);
					csSerial = GetEdidText( myRecord.Detailed_Timing_Description2);
					break;
				case Model_Name:
					csCaption = GetEdidText( myRecord.Detailed_Timing_Description2);
					break;
				}
				switch (GetDetailledTimingDescriptionType( myRecord.Detailed_Timing_Description3))
				{
				case Serial_Number:
					csDescription = GetDescription( &myRecord);
					csSerial = GetEdidText( myRecord.Detailed_Timing_Description3);
					break;
				case Model_Name:
					csCaption = GetEdidText( myRecord.Detailed_Timing_Description3);
					break;
				}
				switch (GetDetailledTimingDescriptionType( myRecord.Detailed_Timing_Description4))
				{
				case Serial_Number:
					csDescription = GetDescription( &myRecord);
					csSerial = GetEdidText( myRecord.Detailed_Timing_Description4);
					break;
				case Model_Name:
					csCaption = GetEdidText( myRecord.Detailed_Timing_Description4);
					break;
				}
				myMonitor.SetType( CA2T( DecodeDPMSFlag( myRecord.DPMS_Flags)));
				AddLog( _T( "\tEDID : Monitor %s, Serial "), CA2T( csDescription));
				AddLog( _T( "<%s>"), CA2T( csSerial));
				// Acer monitors have bogus serial
				if (AcerHack( csSerial, &myRecord))
					AddLog( _T( ", Acer Fix: Change Serial Number to <%s>\n"), CA2T( csSerial));
				else
					AddLog( _T( "\n"), CA2T( csSerial));
				myMonitor.SetCaption( CA2T( csCaption));
				myMonitor.SetDescription( CA2T( csDescription));
				if (myMonitor.IsValidSerial( CA2T( csSerial)))
					myMonitor.SetSerial( CA2T( csSerial));
				if (!pMyList->AddUniqueSerial( myMonitor))
					AddLog( _T( "\tEDID : Monitor serial <%s> already registered, so ignoring\n"), csSerial); 
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
	if( hDeviceInfoSet != INVALID_HANDLE_VALUE)
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
			csCaption.Empty();
			csDescription.Empty();
			csSerial.Empty();
			if (GetDisplayEDID( hDeviceInfoSet, &spDeviceInfo, myRecord))
			{
				myMonitor.SetManufacturer( CA2T( GetManufacturerName( myRecord.Manufacturer_ID)));
				switch (GetDetailledTimingDescriptionType( myRecord.Detailed_Timing_Description1))
				{
				case Serial_Number:
					csDescription = GetDescription( &myRecord);
					csSerial = GetEdidText( myRecord.Detailed_Timing_Description1);
					break;
				case Model_Name:
					csCaption = GetEdidText( myRecord.Detailed_Timing_Description1);
					break;
				}
				switch (GetDetailledTimingDescriptionType( myRecord.Detailed_Timing_Description2))
				{
				case Serial_Number:
					csDescription = GetDescription( &myRecord);
					csSerial = GetEdidText( myRecord.Detailed_Timing_Description2);
					break;
				case Model_Name:
					csCaption = GetEdidText( myRecord.Detailed_Timing_Description2);
					break;
				}
				switch (GetDetailledTimingDescriptionType( myRecord.Detailed_Timing_Description3))
				{
				case Serial_Number:
					csDescription = GetDescription( &myRecord);
					csSerial = GetEdidText( myRecord.Detailed_Timing_Description3);
					break;
				case Model_Name:
					csCaption = GetEdidText( myRecord.Detailed_Timing_Description3);
					break;
				}
				switch (GetDetailledTimingDescriptionType( myRecord.Detailed_Timing_Description4))
				{
				case Serial_Number:
					csDescription = GetDescription( &myRecord);
					csSerial = GetEdidText( myRecord.Detailed_Timing_Description4);
					break;
				case Model_Name:
					csCaption = GetEdidText( myRecord.Detailed_Timing_Description4);
					break;
				}
				myMonitor.SetType( CA2T( DecodeDPMSFlag( myRecord.DPMS_Flags)));
				AddLog( _T( "\tEDID : Monitor %s, Serial "), CA2T( csDescription));
				AddLog( _T( "<%s>"), CA2T( csSerial));
				// Acer monitors have bogus serial
				if (AcerHack( csSerial, &myRecord))
					AddLog( _T( ", Acer Fix: Change Serial Number to <%s>\n"), CA2T( csSerial));
				else
					AddLog( _T( "\n"), CA2T( csSerial));
				myMonitor.SetCaption( CA2T( csCaption));
				myMonitor.SetDescription( CA2T( csDescription));
				myMonitor.SetSerial( CA2T( csSerial));
				if (!pMyList->AddUniqueSerial( myMonitor))
					AddLog( _T( "\tEDID : Monitor serial <%s> already registered, so ignoring\n"), csSerial);
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

	if( hDeviceInfoSet != INVALID_HANDLE_VALUE)
		lpfnSetupDiDestroyDeviceInfoList( hDeviceInfoSet);
	AddLog( _T( "SetupAPI: Enumerates MONITOR devices finished (%ld objects)...\n"), dwIndex);
	return (pMyList->GetCount() > 0);
}

BOOL CEdid::IsConnected()
{
	return (m_hDevInfo != INVALID_HANDLE_VALUE);
}

