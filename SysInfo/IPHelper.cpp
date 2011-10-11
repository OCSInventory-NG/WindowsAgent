//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// IPHelper.cpp: implementation of the CIPHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "NetworkAdapter.h"
#include "NetworkAdapterList.h"
#include "debuglog.h"
#include "IPHelper.h"
#include <Winsock2.h>
#include <Snmp.h>
#include "snmpapi.h"
#include <iphlpapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPHelper::CIPHelper()
{

}

CIPHelper::~CIPHelper()
{

}

BOOL CIPHelper::GetNetworkAdapters(CNetworkAdapterList *pList)
{
    HINSTANCE			hDll;
	DWORD				(WINAPI *lpfnGetAdaptersInfo)( PIP_ADAPTER_INFO myInfo, ULONG *pLength);
	DWORD				(WINAPI *lpfnGetIfTable)( PMIB_IFTABLE pIfTable, PULONG pdwSize, BOOL bOrder);
	PMIB_IFTABLE		pIfTable;
	PMIB_IFROW			pIfEntry;
	PIP_ADAPTER_INFO	pAdapterTable, pAdapterInfo;
	ULONG				ulLength = 0;
	UINT				uIndex = 0;
	DWORD				dwIndex;
	CNetworkAdapter		cAdapter;
	PIP_ADDR_STRING		pAddressList;
	CString				csMAC,
						csAddress,
						csGateway,
						csDhcpServer,
						csBuffer;
	BYTE				pDescription[MAXLEN_IFDESCR+10];

	AddLog( _T( "IpHlpAPI GetNetworkAdapters...\n"));
	// Reset network adapter list content
	while (!(pList->GetCount() == 0))
		pList->RemoveHead();
    // Load the IpHlpAPI dll and get the addresses of necessary functions
    if ((hDll = LoadLibrary(_T( "iphlpapi.dll"))) < (HINSTANCE) HINSTANCE_ERROR) 
	{
		// Cannot load IpHlpAPI MIB
 		AddLog( _T( "IpHlpAPI GetNetworkAdapters: Failed because unable to load <iphlpapi.dll> !\n"));
		hDll = NULL;
        return FALSE;
    }
	if ((*(FARPROC*)&lpfnGetIfTable = GetProcAddress( hDll, "GetIfTable")) == NULL)
	{
		// Tell the user that we could not find a usable IpHlpAPI DLL.                                  
		FreeLibrary( hDll);
		AddLog( _T( "IpHlpAPI GetNetworkAdapters: Failed because unable to load <iphlpapi.dll> !\n"));
		return FALSE;
	}
	if ((*(FARPROC*)&lpfnGetAdaptersInfo = GetProcAddress( hDll, "GetAdaptersInfo")) == NULL)
	{
		// Tell the user that we could not find a usable IpHlpAPI DLL.                                  
		FreeLibrary( hDll);
		AddLog( _T( "IpHlpAPI GetNetworkAdapters: Failed because unable to load <iphlpapi.dll> !\n"));
		return FALSE;
	}
	// Call GetIfTable to get memory size
	AddLog( _T( "IpHlpAPI GetNetworkAdapters: Calling GetIfTable to determine network adapter properties..."));
	pIfTable = NULL;
	ulLength = 0;
	switch( lpfnGetIfTable( pIfTable, &ulLength, TRUE))
	{
	case NO_ERROR: // No error => no adapters
		FreeLibrary( hDll);
		AddLog( _T( "Failed because no network adapters !\n"));
		return FALSE;
	case ERROR_NOT_SUPPORTED: // Not supported
		FreeLibrary( hDll);
		AddLog( _T( "Failed because OS not support GetIfTable API function !\n" ));
		return FALSE;
	case ERROR_BUFFER_OVERFLOW: // We must allocate memory
	case ERROR_INSUFFICIENT_BUFFER:
		break;
	default:
		FreeLibrary( hDll);
		AddLog( _T( "Failed because unknown error !\n" ));
		return FALSE;
	}
	if ((pIfTable = (PMIB_IFTABLE) malloc( ulLength+1)) == NULL)
	{
		FreeLibrary( hDll);
		AddLog( _T( "Failed because memory error !\n" ));
		return FALSE;
	}
	// Recall GetIfTable
	switch( lpfnGetIfTable( pIfTable, &ulLength, TRUE))
	{
	case NO_ERROR: // No error
		break;
	case ERROR_NOT_SUPPORTED: // Not supported
		free( pIfTable);
		FreeLibrary( hDll);
		AddLog( _T( "Failed because OS not support GetIfTable API function !\n" ));
		return FALSE;
	case ERROR_BUFFER_OVERFLOW: // We have allocated needed memory, but not sufficient
	case ERROR_INSUFFICIENT_BUFFER:
		free( pIfTable);
		FreeLibrary( hDll);
		AddLog( _T( "Failed because memory error !\n" ));
		return FALSE;
	default:
		free( pIfTable);
		FreeLibrary( hDll);
		AddLog( _T( "Failed because unknown error !\n" ));
		return FALSE;
	}
/*
	// Dump IfTable
	AddLog("\n=== DEBUG : dump IfTable ===\n");
	for (dwIndex = 0; dwIndex < pIfTable->dwNumEntries; dwIndex ++)
	{
//		char wszName[MAX_INTERFACE_NAME_LEN];
//		WideCharToMultiByte(CP_ACP, 0, pIfTable->table[dwIndex].wszName, -1, wszName, MAX_INTERFACE_NAME_LEN, NULL, NULL);

		AddLog("+ Interface #%d\n", dwIndex);
//		AddLog("\twszName       = %s\n", wszName);
		AddLog("\tdwIndex       = %d\n", pIfTable->table[dwIndex].dwIndex);
		AddLog("\tdwType        = %d\n", pIfTable->table[dwIndex].dwType);
		AddLog("\tdwMtu         = %d\n", pIfTable->table[dwIndex].dwMtu);
		AddLog("\tdwSpeed       = %d\n", pIfTable->table[dwIndex].dwSpeed);
		AddLog("\tdwPhysAddrLen = %d\n", pIfTable->table[dwIndex].dwPhysAddrLen);
		AddLog("\tbPhysAddr     =");
		for (DWORD i=0 ; i<pIfTable->table[dwIndex].dwPhysAddrLen ; i++)
			AddLog(" %02x", pIfTable->table[dwIndex].bPhysAddr[i]);
		AddLog("\n");
		AddLog("\tbPhysAddr     = %d\n", pIfTable->table[dwIndex].dwPhysAddrLen);
		AddLog("\tdwAdminStatus = %d\n", pIfTable->table[dwIndex].dwAdminStatus);

		AddLog("\tdwOperStatus  = %d\n", pIfTable->table[dwIndex].dwOperStatus);
		AddLog("\tdwLastChange  = %d\n", pIfTable->table[dwIndex].dwLastChange);
		AddLog("\tbDescr        = %s\n", pIfTable->table[dwIndex].bDescr);
	}
*/
	// Call GetAdptersInfo with length to 0 to get size of required buffer
	AddLog( _T( "OK\nIpHlpAPI GetNetworkAdapters: Calling GetAdapterInfo to determine IP Infos..."));
	pAdapterTable = NULL;
	switch( lpfnGetAdaptersInfo( pAdapterTable, &ulLength))
	{
	case NO_ERROR: // No error => no adapters
	case ERROR_NO_DATA:
		free( pIfTable);
		FreeLibrary( hDll);
		AddLog( _T( "Failed because no network adapters !\n"));
		return FALSE;
	case ERROR_NOT_SUPPORTED: // Not supported
		free( pIfTable);
		FreeLibrary( hDll);
		AddLog( _T( "Failed because OS not support GetAdaptersInfo API function !\n" ));
		return FALSE;
	case ERROR_BUFFER_OVERFLOW: // We must allocate memory
		break;
	default:
		free( pIfTable);
		FreeLibrary( hDll);
		AddLog( _T( "Failed because unknown error !\n" ));
		return FALSE;
	}
	if ((pAdapterTable = (PIP_ADAPTER_INFO) malloc( ulLength+1)) == NULL)
	{
		free( pIfTable);
		FreeLibrary( hDll);
		AddLog( _T( "Failed because memory error !\n" ));
		return FALSE;
	}
	// Recall GetAdptersInfo
	switch( lpfnGetAdaptersInfo( pAdapterTable, &ulLength))
	{
	case 0: // No error
		break;
	case ERROR_NO_DATA: // No adapters
		free( pIfTable);
		free( pAdapterTable);
		FreeLibrary( hDll);
		AddLog( _T( "Failed because no network adapters !\n"));
		return FALSE;
	case ERROR_NOT_SUPPORTED: // Not supported
		free( pIfTable);
		free( pAdapterTable);
		FreeLibrary( hDll);
		AddLog( _T( "Failed because OS not support GetAdaptersInfo API function !\n" ));
		return FALSE;
	case ERROR_BUFFER_OVERFLOW: // We have allocated needed memory, but not sufficient
		free( pIfTable);
		free( pAdapterTable);
		FreeLibrary( hDll);
		AddLog( _T( "Failed because memory error !\n" ));
		return FALSE;
	default:
		free( pIfTable);
		free( pAdapterTable);
		FreeLibrary( hDll);
		AddLog( _T( "Failed because unknown error !\n" ));
		return FALSE;
	}
/*
	// Dump IfTable
	AddLog("\n=== DEBUG : dump AdaptersInfo ===\n");
	for (dwIndex=0, pAdapterInfo=pAdapterTable ; pAdapterInfo != NULL ; pAdapterInfo = pAdapterInfo->Next, dwIndex++)
	{
		AddLog("+ Adaptater #%d\n", dwIndex);
		AddLog("\tAdapterName   = %s\n", pAdapterInfo->AdapterName); 
		AddLog("\tDescription   = %s\n", pAdapterInfo->Description); 
		AddLog("\tAddressLength = %d\n", pAdapterInfo->AddressLength); 
		AddLog("\tAddress       =");
		for (DWORD i=0 ; i<pAdapterInfo->AddressLength ; i++)
			AddLog(" %02x", pAdapterInfo->Address[i]);
		AddLog("\n");
		AddLog("\tIndex         = %d\n", pAdapterInfo->Index); 
		AddLog("\tType          = %d\n", pAdapterInfo->Type); 
		AddLog("\tDhcpEnabled   = %d\n", pAdapterInfo->DhcpEnabled); 
	
	}
	AddLog("=== DEBUG : fin===\n");

*/
	// Call GetIfEntry for each interface
	for (dwIndex = 0; dwIndex < pIfTable->dwNumEntries; dwIndex ++)
		if (!IsLoopback( pIfTable->table[dwIndex].dwType))
	{
		pIfEntry = &(pIfTable->table[dwIndex]);
		// Get the Index
		cAdapter.SetIfIndex( pIfEntry->dwIndex);
		// Get the type
		cAdapter.SetType( GetAdapterType( pIfEntry->dwType));
		// Get the MIB type
		cAdapter.SetTypeMIB( GetIfType( pIfEntry->dwType));
		// Get the description
		memset( pDescription, 0, MAXLEN_IFDESCR+10);
		for (UINT uChar=0; (uChar < pIfEntry->dwDescrLen) && (uChar<MAXLEN_IFDESCR); uChar++)
			pDescription[uChar] = pIfEntry->bDescr[uChar];
		csAddress.Format( _T( "%S"), pDescription);
		cAdapter.SetDescription( csAddress);
		// Get MAC Address 
		csMAC.Format( _T("%02X:%02X:%02X:%02X:%02X:%02X"),
					   pIfEntry->bPhysAddr[0], pIfEntry->bPhysAddr[1],
					   pIfEntry->bPhysAddr[2], pIfEntry->bPhysAddr[3],
					   pIfEntry->bPhysAddr[4], pIfEntry->bPhysAddr[5]);
		cAdapter.SetMACAddress( csMAC);
		// Get the Speed
		cAdapter.SetSpeed( pIfEntry->dwSpeed);
		// Get the status
		cAdapter.SetIpHelperStatus( pIfEntry->dwOperStatus);
			
		// AddLog("+ IfTable => %s\n", csMAC);
		
		// Now parse the Adapter info
		for (pAdapterInfo=pAdapterTable ; pAdapterInfo != NULL ; pAdapterInfo = pAdapterInfo->Next)
			// Check the MAC Address (bad idea)
			//if (pIfEntry->dwPhysAddrLen==pAdapterInfo->AddressLength && !memcmp(pIfEntry->bPhysAddr, pAdapterInfo->Address, pAdapterInfo->AddressLength))
			// Check the AdapterIndex (better idea)
			if (pIfEntry->dwIndex == pAdapterInfo->Index)
			{
				// AddLog("++ AdapterTable : %02X:%02X:%02X:%02X:%02X:%02X\n", pAdapterInfo->Address[0], pAdapterInfo->Address[1], pAdapterInfo->Address[2], pAdapterInfo->Address[3], pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
				// Get Gateways
				pAddressList = &(pAdapterInfo->GatewayList);
				csGateway = _T("");
				do
				{
					csGateway += pAddressList->IpAddress.String;
					pAddressList = pAddressList->Next;
					if( pAddressList != NULL )
						csGateway += _T( ",");
				}
				while( pAddressList != NULL );
				cAdapter.SetGateway(csGateway);

				// Get DHCP server
				pAddressList = &(pAdapterInfo->DhcpServer);
				csDhcpServer = _T("");
				do
				{
					csDhcpServer += pAddressList->IpAddress.String;
					pAddressList = pAddressList->Next;
					if( pAddressList != NULL )
						csDhcpServer += _T( ",");
				}
				while( pAddressList != NULL );
				cAdapter.SetDhcpServer(csDhcpServer);

				// Get IP addresses and NetMasks
				pAddressList = &(pAdapterInfo->IpAddressList);
				do
				{
					in_addr ipa;
					ULONG   ipAdr, ipMsk, nbRez;		

					csBuffer = pAddressList->IpAddress.String;
					cAdapter.SetIPAddress( csBuffer);
					csBuffer = pAddressList->IpMask.String;
					cAdapter.SetIPNetMask( csBuffer);

					// Update network number if possible		
					if( inet_addr( pAddressList->IpAddress.String ) != INADDR_NONE ){
						ipAdr = ntohl(inet_addr ( pAddressList->IpAddress.String ));
						ipMsk = ntohl(inet_addr ( pAddressList->IpMask.String ));
						nbRez = ipAdr & ipMsk ;
	
						ipa.S_un.S_addr = htonl(nbRez);
						csBuffer = inet_ntoa(ipa);
						cAdapter.SetNetNumber( csBuffer);
					}

					// Create en Adapter entry for each Adapter/Address
					pList->AddTail( cAdapter);
					uIndex ++;

					pAddressList = pAddressList->Next;

				}
				while( pAddressList != NULL );

			// End If sameMac Address
			}
	// End For each interface
	}
	// Free memory
	free( pIfTable);
	free( pAdapterTable);

	// Unload library
	FreeLibrary( hDll);

	AddLog( _T( "OK\n"));
	if (uIndex > 0)
	{
		AddLog( _T( "IpHlpAPI GetNetworkAdapters: OK (%u objects).\n"), uIndex);
		return TRUE;
	}
	AddLog( _T( "IpHlpAPI GetNetworkAdapters: Failed because no network adapter object !\n"));
	return FALSE;
}
