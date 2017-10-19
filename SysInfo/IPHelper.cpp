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
	DWORD(WINAPI *lpfnGetAdaptersAddresses)(ULONG family, ULONG flags, PVOID reserved, PIP_ADAPTER_ADDRESSES myInfo, DWORD *size);
	DWORD(WINAPI *lpfnGetIfTable)(PMIB_IFTABLE pIfTable, PULONG pdwSize, BOOL bOrder);
	PMIB_IFTABLE		pIfTable;
	PMIB_IFROW			pIfEntry;
	PMIB_IPADDRTABLE	pIPAddrTable;
	DWORD				dwSize = 0;
	DWORD				dwRetVal = 0;
	IN_ADDR				IPAddr, IPAddrBis, ipa;
	DWORD				ifIndex;
	PIP_ADAPTER_ADDRESSES	pAdresses = NULL, pAdapterAddr = NULL;
	PIP_ADAPTER_UNICAST_ADDRESS		pUnicast = NULL;
	SOCKET_ADDRESS		*pDhcp;
	PIP_ADAPTER_GATEWAY_ADDRESS		pGateway = NULL;
	ULONG				ulLength = 0;
	UINT				uIndex = 0;
	DWORD				dwIndex;
	DWORD				rv = NULL, size = NULL;
	CNetworkAdapter		cAdapter;
	CString				csMAC,
						csAddress,
						csSubnet,
						csSubnetNetwork,
						csAddressIp,
						csGateway,
						csDhcpServer,
						csBuffer;
	BYTE				pDescription[MAXLEN_IFDESCR + 10];
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
	if ((*(FARPROC*)&lpfnGetIfTable = GetProcAddress(hDll, "GetIfTable")) == NULL)
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
	pIfTable = NULL;
	ulLength = 0;
	switch (lpfnGetIfTable(pIfTable, &ulLength, TRUE))
	{
	case NO_ERROR: // No error => no adapters
		FreeLibrary(hDll);
		AddLog(_T("Failed because no network adapters !\n"));
		return FALSE;
	case ERROR_NOT_SUPPORTED: // Not supported
		FreeLibrary(hDll);
		AddLog(_T("Failed because OS not support GetIfTable API function !\n"));
		return FALSE;
	case ERROR_BUFFER_OVERFLOW: // We must allocate memory
	case ERROR_INSUFFICIENT_BUFFER:
		break;
	default:
		FreeLibrary(hDll);
		AddLog(_T("Failed because unknown error !\n"));
		return FALSE;
	}
	if ((pIfTable = (PMIB_IFTABLE)malloc(ulLength + 1)) == NULL)
	{
		FreeLibrary(hDll);
		AddLog(_T("Failed because memory error !\n"));
		return FALSE;
	}
	// Recall GetIfTable
	switch (lpfnGetIfTable(pIfTable, &ulLength, TRUE))
	{
	case NO_ERROR: // No error
		break;
	case ERROR_NOT_SUPPORTED: // Not supported
		free(pIfTable);
		FreeLibrary(hDll);
		AddLog(_T("Failed because OS not support GetIfTable API function !\n"));
		return FALSE;
	case ERROR_BUFFER_OVERFLOW: // We have allocated needed memory, but not sufficient
	case ERROR_INSUFFICIENT_BUFFER:
		free(pIfTable);
		FreeLibrary(hDll);
		AddLog(_T("Failed because memory error !\n"));
		return FALSE;
	default:
		free(pIfTable);
		FreeLibrary(hDll);
		AddLog(_T("Failed because unknown error !\n"));
		return FALSE;
	}
	
	WSAData d;
	if (WSAStartup(MAKEWORD(2, 2), &d) != 0) {
	}

	// Call GetIfEntry for each interface
	for (dwIndex = 0; dwIndex < pIfTable->dwNumEntries; dwIndex++)
	{
		pIfEntry = (MIB_IFROW *)&(pIfTable->table[dwIndex]);

		// next entry if MAC empty
		if (pIfEntry->bPhysAddr[0] == 0 && pIfEntry->bPhysAddr[1] == 0 &&
			pIfEntry->bPhysAddr[2] == 0 && pIfEntry->bPhysAddr[3] == 0 &&
			pIfEntry->bPhysAddr[4] == 0 && pIfEntry->bPhysAddr[5] == 0)
			continue;

		if (pIfEntry->dwType != IF_TYPE_SOFTWARE_LOOPBACK)
		{
			//if Network card is desabled
			if (pIfEntry->dwOperStatus == IF_OPER_STATUS_NON_OPERATIONAL && pIfEntry->dwSpeed != NULL)
			{
				// Get the Index
				cAdapter.SetIfIndex(pIfEntry->dwIndex);
				// Get the type
				cAdapter.SetType(GetAdapterType(pIfEntry->dwType));
				// Get the MIB type
				cAdapter.SetTypeMIB(GetIfType(pIfEntry->dwType));
				// Get the description
				memset(pDescription, 0, MAXLEN_IFDESCR + 10);
				for (UINT uChar = 0; (uChar < pIfEntry->dwDescrLen) && (uChar < MAXLEN_IFDESCR); uChar++)
					pDescription[uChar] = pIfEntry->bDescr[uChar];
				csAddress.Format(_T("%S"), pDescription);
				if (pIfEntry->dwDescrLen <= csAddress.GetLength())
					csAddress.Truncate(pIfEntry->dwDescrLen);
				cAdapter.SetDescription(csAddress);
				// Get MAC Address 
				csMAC.Format(_T("%02X:%02X:%02X:%02X:%02X:%02X"),
					pIfEntry->bPhysAddr[0], pIfEntry->bPhysAddr[1],
					pIfEntry->bPhysAddr[2], pIfEntry->bPhysAddr[3],
					pIfEntry->bPhysAddr[4], pIfEntry->bPhysAddr[5]);
				cAdapter.SetMACAddress(csMAC);
				// Get the Speed
				cAdapter.SetSpeed(pIfEntry->dwSpeed);
				// Get the status
				cAdapter.SetIpHelperStatus(pIfEntry->dwOperStatus);
				cAdapter.SetIPAddress(NULL);
				cAdapter.SetDhcpServer(NULL);
				cAdapter.SetGateway(NULL);
				cAdapter.SetIPNetMask(NULL);
				cAdapter.SetNetNumber(NULL);
				
				pList->AddTail(cAdapter);
				uIndex++;
			}
			else if (pIfEntry->dwOperStatus == IF_OPER_STATUS_OPERATIONAL)
			{
				// Get the Index
				cAdapter.SetIfIndex(pIfEntry->dwIndex);
				// Get the type
				cAdapter.SetType(GetAdapterType(pIfEntry->dwType));
				// Get the MIB type
				cAdapter.SetTypeMIB(GetIfType(pIfEntry->dwType));
				// Get the description
				memset(pDescription, 0, MAXLEN_IFDESCR + 10);
				for (UINT uChar = 0; (uChar < pIfEntry->dwDescrLen) && (uChar < MAXLEN_IFDESCR); uChar++)
					pDescription[uChar] = pIfEntry->bDescr[uChar];
				csAddress.Format(_T("%S"), pDescription);
				if (pIfEntry->dwDescrLen <= csAddress.GetLength())
					csAddress.Truncate(pIfEntry->dwDescrLen);
				cAdapter.SetDescription(csAddress);
				// Get MAC Address 
				csMAC.Format(_T("%02X:%02X:%02X:%02X:%02X:%02X"),
					pIfEntry->bPhysAddr[0], pIfEntry->bPhysAddr[1],
					pIfEntry->bPhysAddr[2], pIfEntry->bPhysAddr[3],
					pIfEntry->bPhysAddr[4], pIfEntry->bPhysAddr[5]);
				cAdapter.SetMACAddress(csMAC);
				// Get the Speed
				cAdapter.SetSpeed(pIfEntry->dwSpeed);
				// Get the status
				cAdapter.SetIpHelperStatus(pIfEntry->dwOperStatus);

				//Call GetAdapterAddresses for prefix informations
				pAdresses = (PIP_ADAPTER_ADDRESSES)malloc(size);
				if (pAdresses)
				{
					rv = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &size);
					if (rv != ERROR_BUFFER_OVERFLOW) 
					{
						AddLog(_T("GetAdaptersAddresses() failed..."));
						return FALSE;
					}
					pAdresses = (PIP_ADAPTER_ADDRESSES)malloc(size);
					rv = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL, pAdresses, &size);
					if (rv != ERROR_SUCCESS) 
					{
						AddLog(_T("GetAdaptersAddresses() failed..."));
						free(pAdresses);
						return FALSE;
					}
				}
				// Now parse the Adapter addresses
				for (pAdapterAddr = pAdresses; pAdapterAddr != NULL; pAdapterAddr = pAdapterAddr->Next)
				{
					if (pIfEntry->dwIndex == pAdapterAddr->IfIndex)
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
							}
						}
					}
				}
				// End For each interface

				// Free memory
				free(pAdresses);

				pAdresses = NULL;
				pAdapterAddr = NULL;
				rv = NULL;
				size = NULL;

				//Make an other call for GetAdapterAddresses for get Gateway
				pAdresses = (PIP_ADAPTER_ADDRESSES)malloc(size);
				if (pAdresses)
				{
					rv = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_GATEWAYS, NULL, NULL, &size);
					if (rv != ERROR_BUFFER_OVERFLOW)
					{
						AddLog(_T("GetAdaptersAddresses() failed..."));
						return FALSE;
					}
					pAdresses = (PIP_ADAPTER_ADDRESSES)malloc(size);
					rv = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_GATEWAYS, NULL, pAdresses, &size);
					if (rv != ERROR_SUCCESS)
					{
						AddLog(_T("GetAdaptersAddresses() failed..."));
						free(pAdresses);
						return FALSE;
					}
				}

				for (pAdapterAddr = pAdresses; pAdapterAddr != NULL; pAdapterAddr = pAdapterAddr->Next)
				{
					if (pIfEntry->dwIndex == pAdapterAddr->IfIndex)
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
							pIPAddrTable = (MIB_IPADDRTABLE *)malloc(sizeof(MIB_IPADDRTABLE));
							if (pIPAddrTable == NULL)
							{
								return FALSE;
							}
							else
							{
								dwSize = 0;
								// Make an initial call to GetIpAddrTable to get the
								// necessary size into the dwSize variable
								if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) ==
									ERROR_INSUFFICIENT_BUFFER)
								{
									free(pIPAddrTable);
									pIPAddrTable = (MIB_IPADDRTABLE *)malloc(dwSize);

								}
								if (pIPAddrTable == NULL)
								{
									AddLog(_T("Memory allocation failed for GetIpAddrTable\n"));
									return FALSE;
								}
							}
							// Make a second call to GetIpAddrTable to get the
							// actual data we want
							if ((dwRetVal = GetIpAddrTable(pIPAddrTable, &dwSize, 0)) == NO_ERROR)
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
							else
							{
								if (pIPAddrTable)
									AddLog(_T("Call to GetIpAddrTable failed with error %d.\n", dwRetVal));
								free(pIPAddrTable);
								return FALSE;
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
	free(pIfTable);
	free(pAdresses);

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
