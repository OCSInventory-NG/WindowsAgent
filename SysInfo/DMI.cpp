//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// DMI.cpp: implementation of the CDMI class.
//
// This code is derived from SMBIOS Peek code sample by wjfrancis
// http://www.codeproject.com/KB/system/SMBIOS_Peek.aspx
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DebugLog.h"
#include "SysInfo.h"
#include "wbemidl.h"

#define MAKEDWORD(a,b,c,d)	((d<<24)+(c<<16)+(b<<8)+(a))

CDMI::CDMI(void)
{
	m_pTables = NULL;
}

CDMI::~CDMI(void)
{
	Disconnect();
}


/////////////////////////////////////////////////////////////////////////////
// String 

LPCTSTR CDMI::DmiString(DmiHeader* dmi, UCHAR id)
{
	static CString cstr;
	char *p = (char *)dmi;

	p += dmi->Length;

	while(id > 1 && *p)
	{
		p += strlen(p);
		p++;
		id--;
	}
	// ASCII Filter
	for(DWORD i = 0; i < strlen(p); i++){
		if(p[i] < 32 || p[i] == 127){
			p[i]='.';
		}
	}
	cstr = p;
	return cstr;
}

LPCTSTR CDMI::DmiStringB(BYTE b)
{
	static CString cstr;
	cstr.Format( _T( "%d"), b);
	return cstr;
}

LPCTSTR CDMI::DmiStringW(WORD w)
{
	static CString cstr;
	cstr.Format( _T( "%d"), w);
	return cstr;
}

LPCTSTR CDMI::DmiStringD(DWORD d)
{
	static CString cstr;
	cstr.Format(  _T( "%d"), d);
	return cstr;
}

LPCTSTR CDMI::DmiStringBX(BYTE b, DWORD type)
{
	static CString cstr;
	switch (type)
	{
	case 0:
		cstr.Format( _T( "%02Xh (%d)"), b, b);
		break;
	case 1:
		cstr.Format( _T( "%02X"), b);
		break;
	case 2:
		cstr.Format( _T( "%02Xh"), b);
		break;
	}
	return cstr;
}

LPCTSTR CDMI::DmiStringWX(WORD w, DWORD type)
{
	static CString cstr;
	switch (type)
	{
	case 0:
		cstr.Format( _T( "%04Xh (%d)"), w, w);
		break;
	case 1:
		cstr.Format( _T( "%04X"), w);
		break;
	case 2:
		cstr.Format( _T( "%04Xh"), w);
		break;
	case 3:
		if(w >= 0xFFFE){
			cstr = NOT_AVAILABLE;
		}else{
			cstr.Format( _T( "%04Xh"),w);
		}
		break;
	}
	return cstr;
}

LPCTSTR CDMI::DmiStringDX(DWORD d, DWORD type)
{
	static CString cstr;
	switch (type)
	{
	case 0:
		cstr.Format( _T( "%08Xh (%d)"), d, d);
		break;
	case 1:
		cstr.Format( _T( "%08X"), d);
		break;
	case 2:
		cstr.Format( _T( "%08Xh"), d);
		break;
	}
	return cstr;
}

BOOL CDMI::CheckSum(const BYTE *buf, int length)
{
	BYTE sum = 0;
	
	for(int i = 0; i < length; i++){
		sum += buf[i];
	}
	return (sum==0);
}

BOOL CDMI::Connect()
{
	BOOL bRet = FALSE;
	HRESULT h_result;
	IWbemLocator* p_locator = 0;
    IWbemServices* p_service = 0;
    IEnumWbemClassObject* p_enumerator = NULL;

	// Initialize COM.
	AddLog( _T( "DMI: Trying to initialize COM...")); 
    if ((h_result =  CoInitializeEx(0, COINIT_MULTITHREADED)) < 0) 
	{
		AddLog( _T( "Failed in call to CoInitializeEx !\n"));
        return FALSE;
	}
    // Obtain the initial locator to Windows Management
    // on a particular host computer.
    h_result = CoCreateInstance(
        CLSID_WbemLocator,             
        0, 
        CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, (LPVOID *) &p_locator);
 
    if (h_result<0)
    {
  		AddLog( _T( "Failed in call to CoCreateInstance !\n"));
		CoUninitialize();
        return FALSE;
    }
    // Connect to the root\cimv2 namespace with the
    // current user and obtain pointer pSvc
    // to make IWbemServices calls.
    h_result = p_locator->ConnectServer(
        _T("ROOT\\WMI"),		 // WMI namespace
        NULL,                    // User name
        NULL,                    // User password
        0,                       // Locale
        NULL,                    // Security flags                 
        0,                       // Authority       
        0,                       // Context object
        &p_service                 // IWbemServices proxy
    );                              
    
    if (h_result<0)
    {
  		AddLog( _T( "Failed in call to ConnectServer !\n"));
        p_locator->Release();     
        CoUninitialize();
        return FALSE;
    }
    // Set the IWbemServices proxy so that impersonation
    // of the user (client) occurs.
    h_result = CoSetProxyBlanket(
       p_service,                      // the proxy to set
       RPC_C_AUTHN_WINNT,            // authentication service
       RPC_C_AUTHZ_NONE,             // authorization service
       NULL,                         // Server principal name
       RPC_C_AUTHN_LEVEL_CALL,       // authentication level
       RPC_C_IMP_LEVEL_IMPERSONATE,  // impersonation level
       NULL,                         // client identity 
       EOAC_NONE                     // proxy capabilities     
    );
    if (h_result<0)
    {
  		AddLog( _T( "Failed in call to CoSetProxyBlanket !\n"));
        p_service->Release();
        p_locator->Release();     
        CoUninitialize();
        return FALSE;
    }
	AddLog( _T( "OK\nDMI: Trying to get raw SMBios data...")); 
	// Enumerates MSSMBios_RawSMBiosTables objects
    h_result = p_service->CreateInstanceEnum( _T("MSSMBios_RawSMBiosTables"), 0, NULL, &p_enumerator);
    if (h_result<0)
    {
  		AddLog( _T( "Failed in call to CreateInstanceEnum( MSSMBios_RawSMBiosTables) !\n"));
        p_service->Release();
        p_locator->Release();     
        CoUninitialize();
        return FALSE;
    }
    do
    {
        IWbemClassObject* p_instance = NULL;
        ULONG dw_count = NULL;

        h_result = p_enumerator->Next(
            WBEM_INFINITE,
            1,
            &p_instance,
            &dw_count);      
		if (h_result>=0)
		{
			// This an object, ensure SMBios data are ok
			VARIANT variant_bios_data;
			VariantInit( &variant_bios_data);
			CIMTYPE type;

			// Try to get SMBios version
			h_result = p_instance->Get( _T("SmbiosMajorVersion"), 0, &variant_bios_data,&type,NULL);
			if (h_result <0)
			{
				// No SMBios version, skip
				VariantClear(&variant_bios_data);
				continue;
			}
			VariantInit(&variant_bios_data);
			h_result = p_instance->Get(_T("SmbiosMinorVersion"),0,&variant_bios_data,&type,NULL);
			if(h_result<0)
			{
				// No SMBios version, skip
				VariantClear( &variant_bios_data);
				continue;
			}
			// Try to get DMI tables
			VariantInit(&variant_bios_data);
			h_result = p_instance->Get(_T("SMBiosData"),0,&variant_bios_data,&type,NULL);
			if(h_result>=0)
			{
				if ( ( VT_UI1 | VT_ARRAY  ) != variant_bios_data.vt )
				{
					// No SMBios data inside
				}
				else
				{
					SAFEARRAY* p_array = NULL;
					p_array = V_ARRAY(&variant_bios_data);
					unsigned char* p_data = (unsigned char *)p_array->pvData;

					m_nStructureLength = p_array->rgsabound[0].cElements;
					if (m_pTables == NULL)
						free( m_pTables);
					if ((m_pTables = (UCHAR*) malloc( m_nStructureLength+2)) == NULL) 
					{
						AddLog( _T("Unable to allocate memory for raw SMBIOS data !\n"));
						p_service->Release();
						p_locator->Release();     
						CoUninitialize();
						return FALSE;
					}
					memset( m_pTables, 0, m_nStructureLength+2);
					// OK, we've found them
					memcpy( m_pTables, p_data, m_nStructureLength);
				}
			}
			VariantClear(&variant_bios_data);
			break;
		}
    } while (h_result == WBEM_S_NO_ERROR);
	
    p_service->Release();
    p_locator->Release();     
    CoUninitialize();
	if (m_pTables == NULL)
	{
		AddLog( _T( "No MSSMBios_RawSMBiosTables found !\n"));
		return FALSE;
	}
	AddLog( _T( "OK\n"));
	return TRUE;
}

BOOL CDMI::IsConnected()
{
	return (m_pTables != NULL);
}

BOOL CDMI::Disconnect()
{
	AddLog( _T( "DMI: Unload DMI tables...")); 
	if (m_pTables != NULL)
	{
		free( m_pTables);
		m_pTables = NULL;
	}
	AddLog( _T( "OK\n"));
	return TRUE;
}

int CDMI::GetTableLength( int nPosition)
{
	DmiHeader *dmi;
	int nLength = 0;

	// Ensure this is a valid table type
	dmi = (DmiHeader*) &m_pTables[ nPosition];
	if ((dmi->Type < 0) || (dmi->Type > 127))
		return 0; // error
	// Fixed table size is in DMI header, but we need to find variable table size
	// where strings are stored (ending with 2 '\0' characters)
	nLength = dmi->Length;
	while ((nPosition + nLength) < m_nStructureLength)
	{
		if ((m_pTables[ nPosition+nLength]==0) &&
			(m_pTables[ nPosition+nLength+1]==0))
		{
			return nLength+2;
		}
		nLength++;
	}
	// No variable part end found
	nLength = dmi->Length;
	return nLength;
}

UCHAR *CDMI::GetNextTable( UCHAR uType, BOOL bStart)
{
	static int nPosition; // To store position betwwen 2 calls
	int nIndex, nLength;
	DmiHeader *dmi;

	if (bStart)
		// Search from begining of tables
		nPosition = 0;

	while (nPosition < m_nStructureLength)
	{
		dmi = (DmiHeader*) &m_pTables[ nPosition];
		// Get table length
		nLength = GetTableLength( nPosition);
		// Save current table position 
		nIndex = nPosition;
		// Set position of next table
		nPosition += nLength;
		// Check current table type
		if (dmi->Type == uType)
		{
			// This is one table we search
			return &m_pTables[ nIndex];
		}
		// No more table to parse
		if (nLength == 0)
			break;
	}
	// Not found
	return NULL;
}


BOOL CDMI::GetBiosInfo( CBios *pMyBios)
{
	CString csVendor, 
			csVersion, 
			csDate,
			csManufacturer,
			csModel,
			csSN,
			csType,
			csAssetTag;

	pMyBios->Set( NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE, NOT_AVAILABLE);
	// First, get BIOS Info
	if (!GetBios( csVendor, csVersion, csDate))
		return FALSE;
	pMyBios->SetBiosManufacturer( csVendor);
	pMyBios->SetBiosVersion( csVersion);
	pMyBios->SetBiosDate( csDate);
	// Try general system info
	if (GetSystemInformation( csManufacturer, csModel, csSN))
	{
		pMyBios->SetSystemManufacturer( csManufacturer);
		pMyBios->SetSystemModel( csModel);
		pMyBios->SetSystemSerialNumber( csSN);
	}
	// Try system enclosure
	if (GetSystemEnclosure( csManufacturer, csType, csSN, csAssetTag))
	{
		if (!pMyBios->IsValidSystemManufacturer())
			pMyBios->SetSystemManufacturer( csManufacturer);
		if (!pMyBios->IsValidSystemSerialNumber())
			pMyBios->SetSystemSerialNumber( csSN);
		pMyBios->SetMachineType( csType);
		pMyBios->SetAssetTag( csAssetTag);
	}
	// Try baseboard
	if (GetBaseBoard( csManufacturer, csModel, csVersion, csSN, csAssetTag))
	{
		if (!pMyBios->IsValidAssetTag())
			pMyBios->SetAssetTag( csAssetTag);
		if (!pMyBios->IsValidSystemManufacturer())
			pMyBios->SetSystemManufacturer( csManufacturer);
		if (!pMyBios->IsValidSystemModel())
			pMyBios->SetSystemModel( csModel);
	}
	return TRUE;
}

BOOL CDMI::GetBios( CString &csVendor, CString &csVersion, CString &csDate)
{
	DmiHeader	*dmi;
	UCHAR		*pPointer;
	UINT		uCount = 0;

	if (m_pTables == NULL)
		return FALSE;
	AddLog( _T( "DMI GetBios: Trying to find DMI Structure type 0..."));
	// 00 BIOS Information
	pPointer = GetNextTable( DMI_BIOS_INFORMATION, TRUE);
	while( pPointer != NULL)
	{
		dmi = (DmiHeader*) pPointer;
		csVendor = DmiString(dmi, pPointer[0x04]);
		csVersion = DmiString(dmi, pPointer[0x05]);
		csDate = DmiString(dmi, pPointer[0x08]);
		uCount++;
		// next 
		pPointer = GetNextTable( DMI_BIOS_INFORMATION);
	}
	AddLog( _T( "OK (%u object(s)\n"), uCount);
	return (uCount > 0);
}

BOOL CDMI::GetSystemInformation( CString &csVendor, CString &csModel, CString &csSN)
{
	DmiHeader	*dmi;
	UCHAR		*pPointer;
	UINT		uCount = 0;
 
	if (m_pTables == NULL)
		return FALSE;
	AddLog( _T( "DMI GetSystemInformation: Trying to find DMI Structure type 1..."));
	// 01 System Information
	pPointer = GetNextTable( DMI_SYSTEM_INFORMATION, TRUE);
	while( pPointer != NULL)
	{
		dmi = (DmiHeader*) pPointer;
		csVendor = DmiString(dmi, pPointer[0x04]);
		csModel = DmiString(dmi, pPointer[0x05]);
		csSN = DmiString(dmi, pPointer[0x07]);
		uCount++;
		// next 
		pPointer = GetNextTable( DMI_SYSTEM_INFORMATION);
	}
	AddLog( _T( "OK (%u object(s)\n"), uCount);
	return (uCount > 0);
}

BOOL CDMI::GetBaseBoard( CString &csVendor, CString &csProduct, CString &csVersion, CString &csSN, CString &csAssetTag)
{
	DmiHeader	*dmi;
	UCHAR		*pPointer;
	UINT		uCount = 0;
 
	if (m_pTables == NULL)
		return FALSE;
	AddLog( _T( "DMI GetBaseBoard: Trying to find DMI Structure type 2..."));
	// 02 Base Board Information
	pPointer = GetNextTable( DMI_BASEBOARD_INFORMATION, TRUE);
	while( pPointer != NULL)
	{
		dmi = (DmiHeader*) pPointer;
		csVendor = DmiString(dmi, pPointer[0x04]);
		csProduct = DmiString(dmi, pPointer[0x05]);
		csVersion = DmiString(dmi, pPointer[0x06]);
		csSN = DmiString(dmi, pPointer[0x07]);
		if(pPointer[0x02] > 8)
		{
			// Asset tag is stored here
			csAssetTag = DmiString(dmi, pPointer[0x08]);
		}
		uCount++;
		// next 
		pPointer = GetNextTable( DMI_BASEBOARD_INFORMATION);
	}
	AddLog( _T( "OK (%u object(s)\n"), uCount);
	return (uCount > 0);
}

BOOL CDMI::GetSystemEnclosure( CString &csVendor, CString &csType, CString &csSN, CString &csAssetTag)
{
	DmiHeader	*dmi;
	UCHAR		*pPointer;
	UINT		uCount = 0;
 
	if (m_pTables == NULL)
		return FALSE;
	AddLog( _T( "DMI GetSystemEnclosure: Trying to find DMI Structure type 3..."));
	// 03 System Enclosure or Chassis
	pPointer = GetNextTable( DMI_SYSTEM_ENCLOSURE, TRUE);
	while( pPointer != NULL)
	{
		dmi = (DmiHeader*) pPointer;
		csVendor = DmiString(dmi, pPointer[0x04]);
		csSN = DmiString(dmi, pPointer[0x07]);
		csAssetTag = DmiString(dmi, pPointer[0x08]);
		csType = CBios::ParseChassisType( pPointer[0x05] & 0x7f);
		uCount++;
		// next 
		pPointer = GetNextTable( DMI_SYSTEM_ENCLOSURE);
	}
	AddLog( _T( "OK (%u object(s)\n"), uCount);
	return (uCount > 0);
}

BOOL CDMI::GetSystemPorts( CSystemPortList *pMyList)
{
	DmiHeader	*dmi;
	UCHAR		*pPointer;
	UINT		uCount = 0;
 	CSystemPort	myObject;
	CString		csBuffer;

	ASSERT( pMyList);

	if (m_pTables == NULL)
		return FALSE;

	AddLog( _T( "DMI GetSystemPorts: Trying to find DMI Structure type 8..."));
	// Reset object list content
	pMyList->RemoveAll();
	// 08 Port Connector Information
	pPointer = GetNextTable( DMI_SYSTEM_PORTS, TRUE);
	while( pPointer != NULL)
	{
		myObject.Clear();
		dmi = (DmiHeader*) pPointer;
		// External reference designator
		myObject.SetName( DmiString(dmi, pPointer[0x06]));
		// Port type
		myObject.SetType( pPointer[0x08]);
		// External connector type
		myObject.SetConnectorType( pPointer[0x07]);
		csBuffer.Format( _T( "%s %s"), myObject.GetName(), myObject.GetDescription());
		myObject.SetCaption( csBuffer);
		pMyList->AddTail( myObject);
		uCount++;
		// next 
		pPointer = GetNextTable( DMI_SYSTEM_PORTS);
	}
	AddLog( _T( "OK (%u object(s)\n"), uCount);
	return (uCount > 0);
}

BOOL CDMI::GetSystemSlots( CSystemSlotList *pMyList)
{
	DmiHeader	*dmi;
	UCHAR		*pPointer;
	UINT		uCount = 0;
 	CSystemSlot	myObject;
	CString		csBuffer,
				csWidth;

	ASSERT( pMyList);

	if (m_pTables == NULL)
		return FALSE;

	AddLog( _T( "DMI GetSystemSlots: Trying to find DMI Structure type 9..."));
	// Reset object list content
	pMyList->RemoveAll();
	// 09 System Slots
	pPointer = GetNextTable( DMI_SYSTEM_SLOTS, TRUE);
	while( pPointer != NULL)
	{
		myObject.Clear();
		dmi = (DmiHeader*) pPointer;
		// Slot designation
		myObject.SetSlotDesignation( DmiString(dmi, pPointer[0x04]));
		myObject.SetName( DmiString(dmi, pPointer[0x04]));
		// Slot type
		myObject.SetType( pPointer[0x05]);
		// Data Bus Width
		myObject.SetBusWidth( pPointer[0x06]);
		// Usage
		myObject.SetUsage( pPointer[0x07]);
		pMyList->AddTail( myObject);
		uCount++;
		// next 
		pPointer = GetNextTable( DMI_SYSTEM_SLOTS);
	}
	AddLog( _T( "OK (%u object(s)\n"), uCount);
	return (uCount > 0);
}

BOOL CDMI::GetMemorySlots( CMemorySlotList *pMyList)
{
	DmiHeader	*dmi;
	UCHAR		*pPointer;
	UINT		uCount = 0;
 	CMemorySlot	myObject;
	CString		csBuffer;
	DWORD		dwValue,
				dwUse,
				dwECC;
	POSITION	pPosCur,
				pPosNext;

	ASSERT( pMyList);

	if (m_pTables == NULL)
		return FALSE;

	AddLog( _T( "DMI GetMemorySlots: Trying to find DMI Structure type 17..."));
	// Reset object list content
	pMyList->RemoveAll();
	// 17 Memory Device
	pPointer = GetNextTable( DMI_MEMORY_DEVICE, TRUE);
	while( pPointer != NULL)
	{
		myObject.Clear();
		dmi = (DmiHeader*) pPointer;
		uCount++;
		myObject.SetSlotNumber( uCount);
		// Bank locator
		myObject.SetDescription( DmiString(dmi, pPointer[0x11]));
		if (_tcslen( myObject.GetDescription()) == 0)
			// Device Locator
			myObject.SetDescription( DmiString(dmi, pPointer[0x10]));
		// Size
		WORD wCapacity = MAKEWORD(pPointer[0x0C], pPointer[0x0D]);
		if (wCapacity == 0)
			csBuffer = _T( "0");
		else if (wCapacity == 0xFFFF)
			// Not available, wrong number
			csBuffer = NOT_AVAILABLE;
		else if ((wCapacity >> 15) & 0x1)
			// Size is in KB
			csBuffer = _T( "< 1");
		else
		{
			// Size is in MB
			csBuffer.Format( _T( "%u"), (wCapacity & 0x7FFF));
		}
		myObject.SetCapacity( csBuffer);
		// Type
		myObject.SetType( pPointer[0x12]);
		// Speed
		csBuffer.Format( _T( "%u"), MAKEWORD( pPointer[0x15], pPointer[0x16]));
		myObject.SetSpeed( csBuffer);
		// Serial Number
		csBuffer = DmiString(dmi, pPointer[0x18]);
/*		// If S/N is empty, get Part number
		if (csBuffer.IsEmpty())
			csBuffer = DmiString(dmi, pPointer[0x1A]);
*/		if (wCapacity > 0)
			myObject.SetSN( csBuffer);
		pMyList->AddTail( myObject);
		// next 
		pPointer = GetNextTable( DMI_MEMORY_DEVICE);
	}
	if (uCount == 0)
	{
		AddLog( _T( "Failed because no DMI Structure type 17 !\n"));
		return FALSE;
	}
	AddLog( _T( "OK (%u objects)\n"), uCount);
	// Add information from Memory Array
	AddLog( _T( "DMI GetMemorySlots: Trying to find DMI Structure type 16..."));
	pPosNext = pMyList->GetHeadPosition();
	pPosCur = pMyList->GetHeadPosition();
	UINT nbFilled = uCount;
	uCount = 0;
	// 16 Physical Memory Array
	pPointer = GetNextTable( DMI_MEMORY_ARRAY, TRUE);
	while( pPointer != NULL)
	{
		dmi = (DmiHeader*) pPointer;
		// Number of memory slot in this array
		dwValue = MAKEWORD( pPointer[0x0D], pPointer[0x0E]);
		while( nbFilled < dwValue ) {
			myObject.SetCaption( _T(""));
			myObject.SetDescription( _T(""));
			myObject.SetCapacity( _T("0"));
			myObject.SetSlotNumber( nbFilled+1 );
			myObject.SetSpeed( _T("N/A"));
			myObject.SetType( _T("Empty slot"));
			pMyList->AddTail( myObject);
			nbFilled++;
		}
		// Usage
		dwUse = pPointer[0x05];
		// ECC type
		dwECC = pPointer[0x06];
		// Each properties of Physical Memory Array have to be set in MemorySlot object 
		// corresponding to current array
		if (pPosNext != NULL)
			myObject = pMyList->GetNext( pPosNext);
		while ((pPosCur != NULL) && (myObject.GetSlotNumber() <= (uCount+dwValue)))
		{
			myObject.SetUsage( dwUse);
			myObject.SetCaption( myObject.GetUsage());
			if( _tcscmp( myObject.GetType(), _T("Empty slot")) != 0 )
				myObject.SetTypeECC( dwECC);
			pMyList->SetAt( pPosCur, myObject);
			pPosCur = pPosNext;
			if (pPosNext != NULL)
				myObject = pMyList->GetNext( pPosNext);
		}
		uCount += dwValue;
		// next 
		pPointer = GetNextTable( DMI_MEMORY_ARRAY);
	}
		AddLog( _T( "OK (%u objects updated)\n"), uCount);
	return (uCount > 0);
}
