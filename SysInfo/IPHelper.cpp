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
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <set>
#include <cstring>

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
	DWORD				dwSize = 0,
						size = 0,
						dwSizeBis = 0;
	IN_ADDR				IPAddr, 
						IPAddrBis, 
						ipa;
	PIP_ADAPTER_ADDRESSES	pAdresses, 
							pAdressesBis, 
							pAdapterAddr = NULL,
							pAdapterAddrBis = NULL;
	PIP_ADAPTER_UNICAST_ADDRESS		pUnicast = NULL;
	PIP_ADAPTER_PREFIX	pPrefix = NULL;
	SOCKET_ADDRESS		*pDhcp;
	PIP_ADAPTER_GATEWAY_ADDRESS		pGateway = NULL;
	ULONG				ulLength = 0,
						dwIndex,
						ipAdr,
						ipMsk,
						nbRez;
	UINT				uIndex = 0,
						ifIndex;
	CNetworkAdapter		cAdapter;
	CString				csMAC,
						csAddress,
						csSubnet,
						csSubnetNetwork,
						csAddressIp,
						csDescription,
						csPrefixLength;
	char				str[INET_ADDRSTRLEN],
						bufferstr[INET_ADDRSTRLEN],
						bufferRez[INET_ADDRSTRLEN];
	auto				family = NULL;


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
	pIfTable = NULL;
	pIfTable = (PMIB_IF_TABLE2)malloc(sizeof(PMIB_IF_TABLE2));
	if (pIfTable == NULL) {
		AddLog(_T("Error allocating memory needed to call GetIfTable\n"));
		return FALSE;
	}

	//call GetIfTable2Ex
	switch (lpfnGetIfTable2Ex(MibIfTableRaw, &pIfTable)) 
	{
	case NO_ERROR:
		break;
	case ERROR_NOT_SUPPORTED:
		FreeMibTable(pIfTable);
		FreeLibrary(hDll);
		AddLog(_T("Failed because OS does not support GetIfTable2Ex API function !\n"));
		return FALSE;
	case ERROR_BUFFER_OVERFLOW:
	case ERROR_INSUFFICIENT_BUFFER:
		FreeMibTable(pIfTable);
		FreeLibrary(hDll);
		AddLog(_T("Failed because OS does not support GetIfTable2Ex API function !\n"));
		return FALSE;
	default:
		FreeMibTable(pIfTable);
		FreeLibrary(hDll);
		AddLog(_T("Failed because unknown error !\n"));
		return FALSE;
	}

	// Call GetAdptersAddresses with length to 0 to get size of required buffer
	AddLog(_T("OK\nIpHlpAPI GetNetworkAdapters: Calling GetAdapterAddresses to determine IP Info..."));
	pAdresses = NULL;
	dwSize = 0;

	switch (lpfnGetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, pAdresses, &dwSize))
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
		AddLog(_T("Failed because OS does not support GetAdapterAddresses API function !\n"));
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
	switch (lpfnGetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, pAdresses, &dwSize))
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
		AddLog(_T("Failed because OS does not support GetAdapterAddresses API function !\n"));
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

	switch (lpfnGetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_GATEWAYS, NULL, pAdressesBis, &size))
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
		AddLog(_T("Failed because OS does not support GetAdapterAddresses API function !\n"));
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
	switch (lpfnGetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_GATEWAYS, NULL, pAdressesBis, &size))
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
		AddLog(_T("Failed because OS does not support GetAdaptersInfo API function !\n"));
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

	pIPAddrTable = (MIB_IPADDRTABLE *)malloc(sizeof(MIB_IPADDRTABLE));
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
	{
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
	}

	if (GetIpAddrTable(pIPAddrTable, &dwSizeBis, 0) == NO_ERROR)
	{

	}
	else
	{
		if (pIPAddrTable)
			AddLog(_T("Call to GetIpAddrTable failed.\n"));
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
			//if Network card is disabled
			if (pIfEntry->OperStatus == IfOperStatusDown)
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
									family = pUnicast->Address.lpSockaddr->sa_family;
									// Get the Index
									cAdapter.SetIfIndex(pIfEntry->InterfaceIndex);
									// Get the type
									cAdapter.SetType(GetAdapterType(pIfEntry->Type));
									// Get the MIB type
									cAdapter.SetTypeMIB(GetIfType(pIfEntry->Type));
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

									if (AF_INET == family)
									{
										csDescription = pIfEntry->Description;
										// Get the description
										cAdapter.SetDescription(csDescription);

										// IPV4
										char buf2[BUFSIZ];
										memset(buf2, 0, BUFSIZ);
										getnameinfo(pUnicast->Address.lpSockaddr, pUnicast->Address.iSockaddrLength, buf2, sizeof(buf2), NULL, 0, NI_NUMERICHOST);
										cAdapter.SetIPAddress(CA2W(buf2));

										//Get DHCP server address
										pDhcp = new SOCKET_ADDRESS;
										*pDhcp = pAdapterAddr->Dhcpv4Server;
										if (pDhcp)
										{
											char buf3[BUFSIZ];
											memset(buf3, 0, BUFSIZ);
											getnameinfo(pDhcp->lpSockaddr, pDhcp->iSockaddrLength, buf3, sizeof(buf3), NULL, 0, NI_NUMERICHOST);
											cAdapter.SetDhcpServer(CA2W(buf3));
											delete pDhcp;
										}

										// Now parse the Adapter addresses
										for (pAdapterAddrBis = pAdressesBis; pAdapterAddrBis != NULL; pAdapterAddrBis = pAdapterAddrBis->Next)
										{
											if (pIfEntry->InterfaceIndex == pAdapterAddrBis->IfIndex)
											{
												//Get gateway
												for (pGateway = pAdapterAddrBis->FirstGatewayAddress; pGateway != NULL; pGateway = pGateway->Next)
												{
													if (pGateway)
													{
														family = pGateway->Address.lpSockaddr->sa_family;
														if (AF_INET == family)
														{
															char buf4[BUFSIZ];
															memset(buf4, 0, BUFSIZ);
															getnameinfo(pGateway->Address.lpSockaddr, pGateway->Address.iSockaddrLength, buf4, sizeof(buf4), NULL, 0, NI_NUMERICHOST);
															cAdapter.SetGateway(CA2W(buf4));
															//Get subnet Mask
															// Make a second call to GetIpAddrTable to get the
															// actual data we want
															for (ifIndex = 0; ifIndex < (UINT)pIPAddrTable->dwNumEntries; ifIndex++)
															{
																if (pIfEntry->InterfaceIndex == pIPAddrTable->table[ifIndex].dwIndex)
																{
																	// Get NetMask
																	IPAddr.S_un.S_addr = (u_long)pIPAddrTable->table[ifIndex].dwMask;
																	IPAddrBis.S_un.S_addr = (u_long)pIPAddrTable->table[ifIndex].dwAddr;
																	csSubnet = inet_ntop(AF_INET, &IPAddr, str, INET_ADDRSTRLEN);
																	csAddressIp = inet_ntop(AF_INET, &IPAddrBis, bufferstr, INET_ADDRSTRLEN);

																	inet_pton(AF_INET, bufferstr, &ipAdr);
																	inet_pton(AF_INET, str, &ipMsk);
																	nbRez = htonl(ipAdr & ipMsk);

																	ipa.S_un.S_addr = htonl(nbRez);
																	csSubnetNetwork = inet_ntop(AF_INET, &ipa, bufferRez, INET_ADDRSTRLEN);
																	cAdapter.SetNetNumber(csSubnetNetwork);
																}
															}
															cAdapter.SetIPNetMask(csSubnet);
														}
													}
												}
											}
										}
										pAdapterAddrBis = NULL;
										pList->AddTail(cAdapter);
									}
									else if (AF_INET6 == family)
									{
										// IPv6
										SOCKADDR_IN6* ipv6 = reinterpret_cast<SOCKADDR_IN6*>(pUnicast->Address.lpSockaddr);
										char ipv6_buffer[INET6_ADDRSTRLEN] = { 0 };
										inet_ntop(AF_INET6, &(ipv6->sin6_addr), ipv6_buffer, INET6_ADDRSTRLEN);
										CString IPV6_buf = CA2W(ipv6_buffer);
										csAddressIp.Format(_T("%s"), IPV6_buf);

										if (csAddressIp.Mid(0, 2) != _T("fe"))
										{
											cAdapter.SetIPAddress(CA2W(ipv6_buffer));

											csDescription.Format(_T("%s %s"), pIfEntry->Description, _T("(IPV6)"));
											// Get the description
											cAdapter.SetDescription(csDescription);

											// IPv6
											SOCKADDR_IN6* ipv6 = reinterpret_cast<SOCKADDR_IN6*>(pUnicast->Address.lpSockaddr);
											char ipv6_buffer[INET6_ADDRSTRLEN] = { 0 };
											inet_ntop(AF_INET6, &(ipv6->sin6_addr), ipv6_buffer, INET6_ADDRSTRLEN);
											cAdapter.SetIPAddress(CA2W(ipv6_buffer));

											//Get prefix
											pPrefix = pAdapterAddr->FirstPrefix;

											if (pPrefix)
											{
												char buf_prefix[BUFSIZ];
												memset(buf_prefix, 0, BUFSIZ);
												getnameinfo(pPrefix->Address.lpSockaddr, pPrefix->Address.iSockaddrLength, buf_prefix, sizeof(buf_prefix), NULL, 0, NI_NUMERICHOST);
												cAdapter.SetNetNumber(CA2W(buf_prefix));

												csPrefixLength.Format(_T("%d"), pPrefix->PrefixLength);
												cAdapter.SetIPNetMask(csPrefixLength);
											}

											//Get DHCP server address
											pDhcp = new SOCKET_ADDRESS;
											*pDhcp = pAdapterAddr->Dhcpv6Server;
											if (pDhcp)
											{
												char buf3[BUFSIZ];
												memset(buf3, 0, BUFSIZ);
												getnameinfo(pDhcp->lpSockaddr, pDhcp->iSockaddrLength, buf3, sizeof(buf3), NULL, 0, NI_NUMERICHOST);
												cAdapter.SetDhcpServer(CA2W(buf3));
												delete pDhcp;
											}

											// Now parse the Adapter addresses
											for (pAdapterAddrBis = pAdressesBis; pAdapterAddrBis != NULL; pAdapterAddrBis = pAdapterAddrBis->Next)
											{
												if (pIfEntry->InterfaceIndex == pAdapterAddrBis->IfIndex)
												{
													//Get gateway
													for (pGateway = pAdapterAddrBis->FirstGatewayAddress; pGateway != NULL; pGateway = pGateway->Next)
													{
														if (pGateway)
														{
															family = pGateway->Address.lpSockaddr->sa_family;
															if (AF_INET6 == family)
															{
																SOCKADDR_IN6* gateway = reinterpret_cast<SOCKADDR_IN6*>(pGateway->Address.lpSockaddr);
																char gateway_buffer[INET6_ADDRSTRLEN] = { 0 };
																inet_ntop(AF_INET6, &(gateway->sin6_addr), gateway_buffer, INET6_ADDRSTRLEN);
																cAdapter.SetGateway(CA2W(gateway_buffer));
															}
														}
													}
												}
											}
											pAdapterAddrBis = NULL;
											pList->AddTail(cAdapter);
										}
									}
								}
							}
						}
					}
					uIndex++;
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
	pAdapterAddrBis = NULL;
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


BOOL CIPHelper::GetNetworkAdaptersJustMAC(CNetworkAdapterList *pList)
{
	HINSTANCE			hDll;
	DWORD(WINAPI *lpfnGetIfTable)(PMIB_IFTABLE pIfTable, PULONG pdwSize, BOOL bOrder);
	PMIB_IFTABLE		pIfTable;
	PMIB_IFROW			pIfEntry;
	ULONG				ulLength = 0;
	UINT				uIndex = 0;
	DWORD				dwIndex;
	CNetworkAdapter		cAdapter;
	CString				csMAC,
		csAddress,
		csGateway,
		csDhcpServer,
		csBuffer;

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

	std::set<CString> lCompareStr;

	// Call GetIfEntry for each interface
	for (dwIndex = 0; dwIndex < pIfTable->dwNumEntries; dwIndex++)
	{
		pIfEntry = &(pIfTable->table[dwIndex]);

		if (pIfEntry->dwType != IF_TYPE_ETHERNET_CSMACD || pIfEntry->dwPhysAddrLen == 0)
			continue;

		// Get MAC Address 
		csMAC.Format(_T("%02X:%02X:%02X:%02X:%02X:%02X"),
			pIfEntry->bPhysAddr[0], pIfEntry->bPhysAddr[1],
			pIfEntry->bPhysAddr[2], pIfEntry->bPhysAddr[3],
			pIfEntry->bPhysAddr[4], pIfEntry->bPhysAddr[5]);

		if (lCompareStr.find(csMAC) != lCompareStr.end())
			continue;

		cAdapter.SetMACAddress(csMAC);

		lCompareStr.insert(csMAC);

		pList->AddTail(cAdapter);
		uIndex++;
	}
	// Free memory
	free(pIfTable);

	// Unload library
	FreeLibrary(hDll);

	AddLog(_T("OK\n"));
	if (uIndex > 0)
	{
		AddLog(_T("IpHlpAPI GetNetworkAdapters: OK (%u objects).\n"), uIndex);
		return TRUE;
	}
	AddLog(_T("IpHlpAPI GetNetworkAdapters: Failed because no network adapter object !\n"));
	return FALSE;
}

LPCTSTR CIPHelper::GetAdapterType(UINT uType)
{
	if (IsEthernet(uType))
		return ETHERNET_ADAPTER;
	if (IsTokenRing(uType))
		return TOKENRING_ADAPTER;
	if (IsTokenBus(uType))
		return TOKENBUS_ADAPTER;
	if (IsATM(uType))
		return ATM_ADAPTER;
	if (IsDialup(uType))
		return DIALUP_ADAPTER;
	if (IsLoopback(uType))
		return LOOPBACK_ADAPTER;
	return OTHER_ADAPTER;
}

BOOL CIPHelper::IsEthernet(UINT uType)
{
	return ((uType == 6) || (uType == 7) || (uType == 26) ||
		(uType == 62) || (uType == 69) || (uType == 117));
}

BOOL CIPHelper::IsTokenRing(UINT uType)
{
	return ((uType == 9) || (uType == 115));
}

BOOL CIPHelper::IsTokenBus(UINT uType)
{
	return (uType == 8);
}

BOOL CIPHelper::IsISDN(UINT uType)
{
	return ((uType == 20) || (uType == 21) || (uType == 63) ||
		(uType == 75) || (uType == 76) || (uType == 77));
}

BOOL CIPHelper::IsATM(UINT uType)
{
	return ((uType == 37) || (uType == 49) || (uType == 107) ||
		(uType == 105) || (uType == 106) || (uType == 114) ||
		(uType == 134));
}

BOOL CIPHelper::IsLAN(UINT uType)
{
	return (IsEthernet(uType) || IsTokenBus(uType) || IsTokenRing(uType) ||
		(uType == 11) || (uType == 15) || (uType == 55) || (uType == 59) ||
		(uType == 60));
}

BOOL CIPHelper::IsDSL(UINT uType)
{
	return ((uType == 94) || (uType == 95) || (uType == 96) || (uType == 97) ||
		(uType == 143));
}

BOOL CIPHelper::IsDialup(UINT uType)
{
	return (IsISDN(uType) || IsDSL(uType) || (uType == 23) || (uType == 81) ||
		(uType == 82) || (uType == 108));
}

BOOL CIPHelper::IsLoopback(UINT uType)
{
	return (uType == 24);
}

LPCTSTR CIPHelper::GetIfType(UINT uType)
{
	// See http://www.iana.org/assignments/ianaiftype-mib
	switch (uType)
	{
		case 1: return _T("Other");
		case 2: return _T("regular1822");
		case 3: return _T("hdh1822");
		case 4: return _T("ddnX25");
		case 5: return _T("rfc877x25");
		case 6: return _T("ethernetCsmacd");
		case 7: return _T("iso88023Csmacd");
		case 8: return _T("iso88024TokenBus");
		case 9: return _T("iso88025TokenRing");
		case 10: return _T("iso88026Man");
		case 11: return _T("starLan");
		case 12: return _T("proteon10Mbit");
		case 13: return _T("proteon80Mbit");
		case 14: return _T("hyperchannel");
		case 15: return _T("fddi");
		case 16: return _T("lapb");
		case 17: return _T("sdlc");
		case 18: return _T("ds1");
		case 19: return _T("e1");
		case 20: return _T("basicISDN");
		case 21: return _T("primaryISDN");
		case 22: return _T("propPointToPointSerial");
		case 23: return _T("ppp");
		case 24: return _T("softwareLoopback");
		case 25: return _T("eon");
		case 26: return _T("ethernet-3Mbit");
		case 27: return _T("nsip");
		case 28: return _T("slip");
		case 29: return _T("ultra");
		case 30: return _T("ds3");
		case 31: return _T("sip");
		case 32: return _T("frame-relay");
		case 33: return _T("rs232");
		case 34: return _T("para");
		case 35: return _T("arcnet");
		case 36: return _T("arcnetPlus");
		case 37: return _T("atm");
		case 38: return _T("miox25");
		case 39: return _T("sonet");
		case 40: return _T("x25ple");
		case 41: return _T("iso88022llc");
		case 42: return _T("localTalk");
		case 43: return _T("smdsDxi");
		case 44: return _T("frameRelayService");
		case 45: return _T("v35");
		case 46: return _T("hssi");
		case 47: return _T("hippi");
		case 48: return _T("modem");
		case 49: return _T("aal5");
		case 50: return _T("sonetPath");
		case 51: return _T("sonetVT");
		case 52: return _T("smdsIcip");
		case 53: return _T("propVirtual");
		case 54: return _T("propMultiplexor");
		case 55: return _T("100BaseVG");
		case 56: return _T("Fibre Channel");
		case 57: return _T("HIPPI Interface");
		case 58: return _T("Obsolete for FrameRelay");
		case 59: return _T("ATM Emulation of 802.3 LAN");
		case 60: return _T("ATM Emulation of 802.5 LAN");
		case 61: return _T("ATM Emulation of a Circuit");
		case 62: return _T("FastEthernet (100BaseT)");
		case 63: return _T("ISDN & X.25");
		case 64: return _T("CCITT V.11/X.21");
		case 65: return _T("CCITT V.36");
		case 66: return _T("CCITT G703 at 64Kbps");
		case 67: return _T("Obsolete G702 see DS1-MIB");
		case 68: return _T("SNA QLLC");
		case 69: return _T("Full Duplex Fast Ethernet (100BaseFX)");
		case 70: return _T("Channel");
		case 71: return _T("Radio Spread Spectrum (802.11)");
		case 72: return _T("IBM System 360/370 OEMI Channel");
		case 73: return _T("IBM Enterprise Systems Connection");
		case 74: return _T("Data Link Switching");
		case 75: return _T("ISDN S/T Interface");
		case 76: return _T("ISDN U Interface");
		case 77: return _T("Link Access Protocol D (LAPD)");
		case 78: return _T("IP Switching Opjects");
		case 79: return _T("Remote Source Route Bridging");
		case 80: return _T("ATM Logical Port");
		case 81: return _T("AT&T DS0 Point (64 Kbps)");
		case 82: return _T("AT&T Group of DS0 on a single DS1");
		case 83: return _T("BiSync Protocol (BSC)");
		case 84: return _T("Asynchronous Protocol");
		case 85: return _T("Combat Net Radio");
		case 86: return _T("ISO 802.5r DTR");
		case 87: return _T("Ext Pos Loc Report Sys");
		case 88: return _T("Apple Talk Remote Access Protocol");
		case 89: return _T("Proprietary Connectionless Protocol");
		case 90: return _T("CCITT-ITU X.29 PAD Protocol");
		case 91: return _T("CCITT-ITU X.3 PAD Facility");
		case 92: return _T("MultiProtocol Connection over Frame/Relay");
		case 93: return _T("CCITT-ITU X213");
		case 94: return _T("Asymmetric Digital Subscriber Loop (ADSL)");
		case 95: return _T("Rate-Adapt Digital Subscriber Loop (RDSL)");
		case 96: return _T("Symmetric Digital Subscriber Loop (SDSL)");
		case 97: return _T("Very High Speed Digital Subscriber Loop (HDSL)");
		case 98: return _T("ISO 802.5 CRFP");
		case 99: return _T("Myricom Myrinet");
		case 100: return _T("Voice recEive and transMit (voiceEM)");
		case 101: return _T("Voice Foreign eXchange Office (voiceFXO)");
		case 102: return _T("Voice Foreign eXchange Station (voiceFXS)");
		case 103: return _T("Voice Encapsulation");
		case 104: return _T("Voice Over IP Encapsulation");
		case 105: return _T("ATM DXI");
		case 106: return _T("ATM FUNI");
		case 107: return _T("ATM IMA");
		case 108: return _T("PPP Multilink Bundle");
		case 109: return _T("IBM IP over CDLC");
		case 110: return _T("IBM Common Link Access to Workstation");
		case 111: return _T("IBM Stack to Stack");
		case 112: return _T("IBM Virtual IP Address (VIPA)");
		case 113: return _T("IBM Multi-Protocol Channel Support");
		case 114: return _T("IBM IP over ATM");
		case 115: return _T("ISO 802.5j Fiber Token Ring");
		case 116: return _T("IBM Twinaxial Data Link Control (TDLC)");
		case 117: return _T("Gigabit Ethernet");
		case 118: return _T("Higher Data Link Control (HDLC)");
		case 119: return _T("Link Access Protocol F (LAPF)");
		case 120: return _T("CCITT V.37");
		case 121: return _T("CCITT X.25 Multi-Link Protocol");
		case 122: return _T("CCITT X.25 Hunt Group");
		case 123: return _T("Transp HDLC");
		case 124: return _T("Interleave Channel");
		case 125: return _T("Fast Channel");
		case 126: return _T("IP (for APPN HPR in IP Networks)");
		case 127: return _T("CATV MAC Layer");
		case 128: return _T("CATV Downstream Interface");
		case 129: return _T("CATV Upstream Interface");
		case 130: return _T("Avalon Parallel Processor");
		case 131: return _T("Encapsulation Interface");
		case 132: return _T("Coffee Pot");
		case 133: return _T("Circuit Emulation Service");
		case 134: return _T("ATM Sub Interface");
		case 135: return _T("Layer 2 Virtual LAN using 802.1Q");
		case 136: return _T("Layer 3 Virtual LAN using IP");
		case 137: return _T("Layer 3 Virtual LAN using IPX");
		case 138: return _T("IP Over Power Lines");
		case 139: return _T("Multi-Media Mail over IP");
		case 140: return _T("Dynamic synchronous Transfer Mode (DTM)");
		case 141: return _T("Data Communications Network");
		case 142: return _T("IP Forwarding Interface");
		case 143: return _T("Multi-rate Symmetric DSL (MSDSL)");
		case 144: return _T("IEEE1394 High Perf Serial Bus");
		case 145: return _T("TV adapter type");
		case 162: return _T("Cisco Express Forwarding Interface");
		default: return _T("");
	}
}