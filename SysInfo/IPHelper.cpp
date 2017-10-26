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
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

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
	DWORD				(WINAPI *lpfnGetAdaptersAddresses)(ULONG family, ULONG flags, PVOID reserved, PIP_ADAPTER_ADDRESSES myInfo, DWORD *size);
	DWORD				(WINAPI *lpfnGetIfTable2Ex)(MIB_IF_TABLE_LEVEL Level, PMIB_IF_TABLE2 *pIfTable);
	PMIB_IF_TABLE2		pIfTable;
	PMIB_IF_ROW2		pIfEntry;
	PMIB_IPADDRTABLE	pIPAddrTable;
	DWORD				dwSize = 0, size = 0, dwSizeBis = 0;
	DWORD				dwRetVal = 0;
	IN_ADDR				IPAddr, IPAddrBis, ipa;
	DWORD				ifIndex;
	PIP_ADAPTER_ADDRESSES	pAdresses, pAdressesBis, pAdapterAddr = NULL;
	PIP_ADAPTER_UNICAST_ADDRESS		pUnicast = NULL;
	SOCKET_ADDRESS		*pDhcp;
	PIP_ADAPTER_GATEWAY_ADDRESS		pGateway = NULL;
	ULONG				ulLength = 0;
	UINT				uIndex = 0;
	ULONG				dwIndex;
	CNetworkAdapter		cAdapter;
	CString				csMAC,
						csAddress,
						csSubnet,
						csSubnetNetwork,
						csAddressIp,
						csGateway,
						csDhcpServer,
						csBuffer;
	char				str[INET_ADDRSTRLEN],
						bufferstr[INET_ADDRSTRLEN],
						bufferRez[INET_ADDRSTRLEN];
	ULONG				ipAdr, 
						ipMsk, 
						nbRez;
	


	AddLog(_T("IpHlpAPI GetNetworkAdapters...\n"));
	// Reset network adapter list content
	while (!(pList->GetCount() == 0))
		pList->RemoveHead();
	// Load the IpHlpAPI dll and get the addresses of necessary functions
	if ((hDll = LoadLibrary(_T("iphlpapi.dll"))) < (HINSTANCE)HINSTANCE_ERROR)
	{
		// Cannot load IpHlpAPI MIB
		AddLog(_T("IpHlpAPI GetNetworkAdapters: Failed because unable to load <iphlpapi.dll> !\n"));
		hDll = NULL;
		return FALSE;
	}

	if ((*(FARPROC*)&lpfnGetIfTable2Ex = GetProcAddress(hDll, "GetIfTable2Ex")) == NULL)
	{
		// Tell the user that we could not find a usable IpHlpAPI DLL.                                  
		FreeLibrary(hDll);
		AddLog(_T("IpHlpAPI GetNetworkAdapters: Failed because unable to load <iphlpapi.dll> !\n"));
		return FALSE;
	}

	if ((*(FARPROC*)&lpfnGetAdaptersAddresses = GetProcAddress(hDll, "GetAdaptersAddresses")) == NULL)
	{
		// Tell the user that we could not find a usable IpHlpAPI DLL.                                  
		FreeLibrary(hDll);
		AddLog(_T("IpHlpAPI GetNetworkAdapters: Failed because unable to load <iphlpapi.dll> !\n"));
		return FALSE;
	}

	// Call GetIfTable to get memory size
	AddLog(_T("IpHlpAPI GetNetworkAdapters: Calling GetIfTable to determine network adapter properties..."));

	//call GetIfTable2Ex
	switch (lpfnGetIfTable2Ex(MibIfTableRaw, &pIfTable)) 
	{
	case NO_ERROR:
		break;
	case ERROR_NOT_SUPPORTED:
		FreeMibTable(pIfTable);
		FreeLibrary(hDll);
		AddLog(_T("Failed because OS not support GetIfTable2Ex API function !\n"));
		return FALSE;
	case ERROR_BUFFER_OVERFLOW:
	case ERROR_INSUFFICIENT_BUFFER:
		FreeMibTable(pIfTable);
		FreeLibrary(hDll);
		AddLog(_T("Failed because OS not support GetIfTable2Ex API function !\n"));
		return FALSE;
	default:
		FreeMibTable(pIfTable);
		FreeLibrary(hDll);
		AddLog(_T("Failed because unknown error !\n"));
		return FALSE;
	}

	// Call GetAdptersAddresses with length to 0 to get size of required buffer
	AddLog(_T("OK\nIpHlpAPI GetNetworkAdapters: Calling GetAdapterAddresses to determine IP Infos..."));
	pAdresses = NULL;
	dwSize = 0;

	switch (lpfnGetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL, pAdresses, &dwSize))
	{
	case NO_ERROR: // No error => no adapters
	case ERROR_NO_DATA:
		FreeMibTable(pIfTable);
		FreeLibrary(hDll);
		AddLog(_T("Failed because no network adapters !\n"));
		return FALSE;
	case ERROR_NOT_SUPPORTED: // Not supported
		FreeMibTable(pIfTable);
		FreeLibrary(hDll);
		AddLog(_T("Failed because OS not support GetAdapterAddresses API function !\n"));
		return FALSE;
	case ERROR_BUFFER_OVERFLOW: // We must allocate memory
		break;
	default:
		FreeMibTable(pIfTable);
		FreeLibrary(hDll);
		AddLog(_T("Failed because unknown error !\n"));
		return FALSE;
	}

	if ((pAdresses = (PIP_ADAPTER_ADDRESSES)malloc(dwSize + 1)) == NULL)
	{
		FreeMibTable(pIfTable);
		FreeLibrary(hDll);
		AddLog(_T("Failed because memory error !\n"));
		return FALSE;
	}

	// Recall GetAdptersAddresses
	switch (lpfnGetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL, pAdresses, &dwSize))
	{
	case 0: // No error
		break;
	case ERROR_NO_DATA: // No adapters
		FreeMibTable(pIfTable);
		free(pAdresses);
		FreeLibrary(hDll);
		AddLog(_T("Failed because no network adapters !\n"));
		return FALSE;
	case ERROR_NOT_SUPPORTED: // Not supported
		FreeMibTable(pIfTable);
		free(pAdresses);
		FreeLibrary(hDll);
		AddLog(_T("Failed because OS not support GetAdapterAddresses API function !\n"));
		return FALSE;
	case ERROR_BUFFER_OVERFLOW: // We have allocated needed memory, but not sufficient
		FreeMibTable(pIfTable);
		free(pAdresses);
		FreeLibrary(hDll);
		AddLog(_T("Failed because memory error !\n"));
		return FALSE;
	default:
		FreeMibTable(pIfTable);
		free(pAdresses);
		FreeLibrary(hDll);
		AddLog(_T("Failed because unknown error !\n"));
		return FALSE;
	}

	// Call GetAdptersAddresses with length to 0 to get size of required buffer
	AddLog(_T("OK\nIpHlpAPI GetNetworkAdapters: Calling GetAdapterAddresses to determine IP Gateway..."));
	pAdressesBis = NULL;

switch (lpfnGetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_GATEWAYS, NULL, pAdressesBis, &size))
{
case NO_ERROR: // No error => no adapters
case ERROR_NO_DATA:
	FreeMibTable(pIfTable);
	free(pAdresses);
	FreeLibrary(hDll);
	AddLog(_T("Failed because no network adapters !\n"));
	return FALSE;
case ERROR_NOT_SUPPORTED: // Not supported
	FreeMibTable(pIfTable);
	free(pAdresses);
	FreeLibrary(hDll);
	AddLog(_T("Failed because OS not support GetAdapterAddresses API function !\n"));
	return FALSE;
case ERROR_BUFFER_OVERFLOW: // We must allocate memory
	break;
default:
	FreeMibTable(pIfTable);
	free(pAdresses);
	FreeLibrary(hDll);
	AddLog(_T("Failed because unknown error !\n"));
	return FALSE;
}

if ((pAdressesBis = (PIP_ADAPTER_ADDRESSES)malloc(size + 1)) == NULL)
{
	FreeMibTable(pIfTable);
	free(pAdresses);
	FreeLibrary(hDll);
	AddLog(_T("Failed because memory error !\n"));
	return FALSE;
}

// Recall GetAdptersAddresses
switch (lpfnGetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_GATEWAYS, NULL, pAdressesBis, &size))
{
case 0: // No error
	break;
case ERROR_NO_DATA: // No adapters
	FreeMibTable(pIfTable);
	free(pAdressesBis);
	free(pAdresses);
	FreeLibrary(hDll);
	AddLog(_T("Failed because no network adapters !\n"));
	return FALSE;
case ERROR_NOT_SUPPORTED: // Not supported
	FreeMibTable(pIfTable);
	free(pAdressesBis);
	free(pAdresses);
	FreeLibrary(hDll);
	AddLog(_T("Failed because OS not support GetAdaptersInfo API function !\n"));
	return FALSE;
case ERROR_BUFFER_OVERFLOW: // We have allocated needed memory, but not sufficient
	FreeMibTable(pIfTable);
	free(pAdressesBis);
	free(pAdresses);
	FreeLibrary(hDll);
	AddLog(_T("Failed because memory error !\n"));
	return FALSE;
default:
	FreeMibTable(pIfTable);
	free(pAdressesBis);
	free(pAdresses);
	FreeLibrary(hDll);
	AddLog(_T("Failed because unknown error !\n"));
	return FALSE;
}

/*pIPAddrTable = (MIB_IPADDRTABLE *)malloc(sizeof(MIB_IPADDRTABLE));
if (pIPAddrTable == NULL)
{
	FreeMibTable(pIfTable);
	free(pAdresses);
	free(pAdressesBis);
	FreeLibrary(hDll);
	AddLog(_T("Failed because memory error !\n"));
	return FALSE;
}
else
{*/
	// Make an initial call to GetIpAddrTable to get the
	// necessary size into the dwSize variable
	if (GetIpAddrTable(pIPAddrTable, &dwSizeBis, 0) == ERROR_INSUFFICIENT_BUFFER)
	{
		free(pIPAddrTable);
		pIPAddrTable = (MIB_IPADDRTABLE *)malloc(dwSizeBis);
	}
	if (pIPAddrTable == NULL)
	{
		FreeMibTable(pIfTable);
		free(pAdresses);
		free(pAdressesBis);
		FreeLibrary(hDll);
		AddLog(_T("Memory allocation failed for GetIpAddrTable\n"));
		return FALSE;
	}
//}
if (GetIpAddrTable(pIPAddrTable, &dwSizeBis, 0) == NO_ERROR)
{

}
else
{
	if (pIPAddrTable)
		AddLog(_T("Call to GetIpAddrTable failed with error %d.\n", dwRetVal));
	free(pIPAddrTable);
	FreeMibTable(pIfTable);
	free(pAdresses);
	free(pAdressesBis);
	FreeLibrary(hDll);
	return FALSE;
}
	
	WSAData d;
	if (WSAStartup(MAKEWORD(2, 2), &d) != 0) 
	{
	}

	// Call GetIfTable2Ex for each interface
		for (dwIndex = 0; dwIndex < pIfTable->NumEntries; dwIndex++)
		{
			pIfEntry = (MIB_IF_ROW2 *)&(pIfTable->Table[dwIndex]);
			if (pIfEntry->Type != IF_TYPE_SOFTWARE_LOOPBACK)
			{
				//if Network card is desabled
				if (pIfEntry->OperStatus == IfOperStatusDown && pIfEntry->TransmitLinkSpeed != NULL)
				{
					// Get the Index
					cAdapter.SetIfIndex(pIfEntry->InterfaceIndex);
					// Get the type
					cAdapter.SetType(GetAdapterType(pIfEntry->Type));
					// Get the MIB type
					cAdapter.SetTypeMIB(GetIfType(pIfEntry->Type));
					// Get the description;
					cAdapter.SetDescription(pIfEntry->Description);
					// Get MAC Address 
					csMAC.Format(_T("%02X:%02X:%02X:%02X:%02X:%02X"),
						pIfEntry->PhysicalAddress[0], pIfEntry->PhysicalAddress[1],
						pIfEntry->PhysicalAddress[2], pIfEntry->PhysicalAddress[3],
						pIfEntry->PhysicalAddress[4], pIfEntry->PhysicalAddress[5]);
					cAdapter.SetMACAddress(csMAC);
					// Get the Speed
					cAdapter.SetSpeed(pIfEntry->TransmitLinkSpeed);
					// Get the status
					cAdapter.SetIpHelperStatus(pIfEntry->OperStatus);
					cAdapter.SetIPAddress(NULL);
					cAdapter.SetDhcpServer(NULL);
					cAdapter.SetGateway(NULL);
					cAdapter.SetIPNetMask(NULL);
					cAdapter.SetNetNumber(NULL);

					pList->AddTail(cAdapter);
					uIndex++;
				}
				else if (pIfEntry->OperStatus == IfOperStatusUp)
				{
					// Get the Index
					cAdapter.SetIfIndex(pIfEntry->InterfaceIndex);
					// Get the type
					cAdapter.SetType(GetAdapterType(pIfEntry->Type));
					// Get the MIB type
					cAdapter.SetTypeMIB(GetIfType(pIfEntry->Type));
					// Get the description
					cAdapter.SetDescription(pIfEntry->Description);
					// Get MAC Address 
					csMAC.Format(_T("%02X:%02X:%02X:%02X:%02X:%02X"),
						pIfEntry->PhysicalAddress[0], pIfEntry->PhysicalAddress[1],
						pIfEntry->PhysicalAddress[2], pIfEntry->PhysicalAddress[3],
						pIfEntry->PhysicalAddress[4], pIfEntry->PhysicalAddress[5]);
					cAdapter.SetMACAddress(csMAC);
					// Get the Speed
					cAdapter.SetSpeed(pIfEntry->TransmitLinkSpeed);
					// Get the status
					cAdapter.SetIpHelperStatus(pIfEntry->OperStatus);

					// Now parse the Adapter addresses
					for (pAdapterAddr = pAdresses; pAdapterAddr != NULL; pAdapterAddr = pAdapterAddr->Next)
					{
						if (pIfEntry->InterfaceIndex == pAdapterAddr->IfIndex)
						{
							if (pAdapterAddr->IfType != IF_TYPE_SOFTWARE_LOOPBACK)
							{
								// Get IP addresses
								for (pUnicast = pAdapterAddr->FirstUnicastAddress; pUnicast != NULL; pUnicast = pUnicast->Next)
								{
									if (pUnicast)
									{
										char buf2[BUFSIZ];
										int family = pUnicast->Address.lpSockaddr->sa_family;
										memset(buf2, 0, BUFSIZ);
										getnameinfo(pUnicast->Address.lpSockaddr, pUnicast->Address.iSockaddrLength, buf2, sizeof(buf2), NULL, 0, NI_NUMERICHOST);
										cAdapter.SetIPAddress(CA2W(buf2));
									}
								}

								//Get DHCP server address
								pDhcp = new SOCKET_ADDRESS;
								*pDhcp = pAdapterAddr->Dhcpv4Server;
								if (pDhcp)
								{
									char buf3[BUFSIZ];
									int family = pDhcp->lpSockaddr->sa_family;
									memset(buf3, 0, BUFSIZ);
									getnameinfo(pDhcp->lpSockaddr, pDhcp->iSockaddrLength, buf3, sizeof(buf3), NULL, 0, NI_NUMERICHOST);
									cAdapter.SetDhcpServer(CA2W(buf3));
									delete pDhcp;
								}
							}
						}
					}
					// End For each interface
					pAdapterAddr = NULL;

					for (pAdapterAddr = pAdressesBis; pAdapterAddr != NULL; pAdapterAddr = pAdapterAddr->Next)
					{
						if (pIfEntry->InterfaceIndex == pAdapterAddr->IfIndex)
						{
							//Get gateway
							for (pGateway = pAdapterAddr->FirstGatewayAddress; pGateway != NULL; pGateway = pGateway->Next)
							{
								if (pGateway)
								{
									char buf4[BUFSIZ];
									int family = pGateway->Address.lpSockaddr->sa_family;
									memset(buf4, 0, BUFSIZ);
									getnameinfo(pGateway->Address.lpSockaddr, pGateway->Address.iSockaddrLength, buf4, sizeof(buf4), NULL, 0, NI_NUMERICHOST);
									cAdapter.SetGateway(CA2W(buf4));
								}

								//Get subnet Mask
								
								// Make a second call to GetIpAddrTable to get the
								// actual data we want
								if (pIPAddrTable != NULL)
								{
									// Get NetMask
									ifIndex = pIPAddrTable->table[0].dwIndex;
									IPAddr.S_un.S_addr = (u_long)pIPAddrTable->table[0].dwMask;
									IPAddrBis.S_un.S_addr = (u_long)pIPAddrTable->table[0].dwAddr;
									csSubnet = inet_ntop(AF_INET, &IPAddr, str, INET_ADDRSTRLEN);
									csAddressIp = inet_ntop(AF_INET, &IPAddrBis, bufferstr, INET_ADDRSTRLEN);

									inet_pton(AF_INET, bufferstr, &ipAdr);
									inet_pton(AF_INET, str, &ipMsk);
									nbRez = htonl(ipAdr & ipMsk);

									ipa.S_un.S_addr = htonl(nbRez);
									csSubnetNetwork = inet_ntop(AF_INET, &ipa, bufferRez, INET_ADDRSTRLEN);
									cAdapter.SetNetNumber(csSubnetNetwork);
								}
								
								cAdapter.SetIPNetMask(csSubnet);

								pList->AddTail(cAdapter);
								uIndex++;
							}
						}
					}
				}
			}
		}
	
	
	// Free memory
	free(pIPAddrTable);
	FreeMibTable(pIfTable);
	free(pAdresses);
	free(pAdressesBis);
	pAdapterAddr = NULL;
	// Unload library
	FreeLibrary(hDll);

	WSACleanup();
	

	AddLog(_T("OK\n"));
	if (uIndex > 0)
	{
		AddLog(_T("IpHlpAPI GetNetworkAdapters: OK (%u objects).\n"), uIndex);
		return TRUE;
	}
	AddLog(_T("IpHlpAPI GetNetworkAdapters: Failed because no network adapter object !\n"));
	return FALSE;
}
