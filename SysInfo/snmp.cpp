//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// SNMP.cpp: implementation of the CSNMP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Sysinfo.h"
#include "debuglog.h"
#include <Winsock2.h>
#include <Snmp.h>
#include "SnmpAPI.h"
#include "SNMP.h"
#include <ws2tcpip.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

LPCSTR GetAnsiFromTString(LPCTSTR a_wstrString)
{
   static char	szBuffer[1024*1024+1]; // 1MB buffer to handle string 
   
   if (_tcslen( a_wstrString) > 1024*1024)
	   AfxThrowMemoryException();
   strcpy_s( szBuffer, 1024*1024, CT2A(a_wstrString));
   return szBuffer;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSNMP::CSNMP()
{

}

CSNMP::~CSNMP()
{

}

BOOL CSNMP::GetNetworkAdapters(CNetworkAdapterList *pList)
{
	typedef BOOL(WINAPI * pSnmpExtensionInit) (
        IN DWORD dwTimeZeroReference,
        OUT HANDLE * hPollForTrapEvent,
        OUT AsnObjectIdentifier * supportedView);

	typedef BOOL(WINAPI * pSnmpExtensionTrap) (
        OUT AsnObjectIdentifier * enterprise,
        OUT AsnInteger * genericTrap,
        OUT AsnInteger * specificTrap,
        OUT AsnTimeticks * timeStamp,
        OUT RFC1157VarBindList * variableBindings);

	typedef BOOL(WINAPI * pSnmpExtensionQuery) (
        IN BYTE requestType,
        IN OUT RFC1157VarBindList * variableBindings,
        OUT AsnInteger * errorStatus,
        OUT AsnInteger * errorIndex);

	typedef BOOL(WINAPI * pSnmpExtensionInitEx) (
        OUT AsnObjectIdentifier * supportedView);
    
    HINSTANCE			hDll;
    HANDLE				PollForTrapEvent;
    int					ret,
    					nType;
	ULONG				speed;
    BOOL				found = FALSE;
	CString				csAddress,
						csBuffer;
	UINT				uIndex = 0;

    pSnmpExtensionInit	m_Init = NULL;
    pSnmpExtensionInitEx m_InitEx = NULL;
    pSnmpExtensionQuery m_Query = NULL;
    pSnmpExtensionTrap	m_Trap = NULL;
    AsnObjectIdentifier SupportedView;
    RFC1157VarBindList	varBindList;
    RFC1157VarBind		varBind[6];
    AsnInteger			errorStatus;
    AsnInteger			errorIndex;

	char				str[INET_ADDRSTRLEN];

	// Reset network adapter list content
	while (!(pList->GetCount() == 0))
		pList->RemoveHead();
	// Network adapter(s) description and MAC address in
	// <.iso.org.dod.internet.mgmt.mib-2.interfaces> MIB tree
	// Entry for number of network interface
    UINT OID_ifEntryNumber[] = { 
		1, 3, 6, 1, 2, 1, 2, 1
	};
	// Entry for network interface index
    UINT OID_ifEntryIndex[] = { 
		1, 3, 6, 1, 2, 1, 2, 2, 1, 1 
	};
	// Entry for network interface description
    UINT OID_ifDesc[] = { 
		1, 3, 6, 1, 2, 1, 2, 2, 1, 2 
	};
	// Entry for network interface type
    UINT OID_ifEntryType[] = { 
		1, 3, 6, 1, 2, 1, 2, 2, 1, 3 
	};
	// Entry for network interface speed
    UINT OID_ifSpeed[] = { 
		1, 3, 6, 1, 2, 1, 2, 2, 1, 5 
	};                          //, 1 ,5 };
	// Entry for network interface physical address
    UINT OID_ifMACAddr[] = { 
		1, 3, 6, 1, 2, 1, 2, 2, 1, 6 
	};
	// Entry for network interface operational status
    UINT OID_ifOperStatus[] = { 
		1, 3, 6, 1, 2, 1, 2, 2, 1, 8 
	};
	AsnObjectIdentifier MIB_ifMACAddr = {
		sizeof(OID_ifMACAddr) / sizeof(UINT), OID_ifMACAddr
	};
    AsnObjectIdentifier MIB_ifEntryType = {
        sizeof(OID_ifEntryType) / sizeof(UINT), OID_ifEntryType
    };
    AsnObjectIdentifier MIB_ifEntryNumber = {
        sizeof(OID_ifEntryNumber) / sizeof(UINT), OID_ifEntryNumber
    };
    AsnObjectIdentifier MIB_ifSpeed= {
        sizeof(OID_ifSpeed) / sizeof(UINT), OID_ifSpeed
    };
    AsnObjectIdentifier MIB_ifDesc= {
        sizeof(OID_ifDesc) / sizeof(UINT), OID_ifDesc
    };
    AsnObjectIdentifier MIB_ifIndex= {
        sizeof(OID_ifEntryIndex) / sizeof(UINT), OID_ifEntryIndex
    };
    AsnObjectIdentifier MIB_ifOperStatus= {
        sizeof(OID_ifOperStatus) / sizeof(UINT), OID_ifOperStatus
    };

	// Network adapter(s) IP address and Net Mask in
	// <.iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable> MIB tree
	// Entry for network interfaces IP Address
    UINT OID_ipAdEntAddr[] = { 
		1, 3, 6, 1, 2, 1, 4, 20, 1, 1 
	};
	// Entry for network interfaces index
    UINT OID_ipAdEntIfIndex[] = { 
		1, 3, 6, 1, 2, 1, 4, 20, 1, 2 
	};
	// Entry for network interfaces IP Net Mask
    UINT OID_ipAdEntNetMask[] = { 
		1, 3, 6, 1, 2, 1, 4, 20, 1, 3 
	};
	AsnObjectIdentifier MIB_ipAdEntAddr = {
		sizeof(OID_ipAdEntAddr) / sizeof(UINT), OID_ipAdEntAddr
	};
    AsnObjectIdentifier MIB_ipAdEntIfIndex = {
        sizeof(OID_ipAdEntIfIndex) / sizeof(UINT), OID_ipAdEntIfIndex
    };
    AsnObjectIdentifier MIB_ipAdEntNetMask = {
        sizeof(OID_ipAdEntNetMask) / sizeof(UINT), OID_ipAdEntNetMask
    };

    AsnObjectIdentifier MIB_NULL = {
        0, 0
    };

	// Didier LIROULET 2005-10-17 BEGIN

	// Network adapter(s) gateway in
	// <.iso.org.dod.internet.mgmt.mib-2.ip.ipRouteTable> MIB tree
	// Entry for IpRoute destination (0.0.0.0 for default route)
	UINT OID_ipRouteDest[] = {
		1, 3, 6, 1, 2, 1, 4, 21, 1, 1
	};
	// Entry for network interface index IpRoute applies
	UINT OID_ipRouteIfIndex[] = {
		1, 3, 6, 1, 2, 1, 4, 21, 1, 2
	};
	// Entry for IpRoute gateway
	UINT OID_ipRouteNextHop[] = {
		1, 3, 6, 1, 2, 1, 4, 21, 1, 7
	};
	// Entry for IpRoute type (2=invalid, 3=direct, 4=indirect)
	UINT OID_ipRouteType[] = {
		1, 3, 6, 1, 2, 1, 4, 21, 1, 8
	};
	AsnObjectIdentifier MIB_ipRouteDest = {
		sizeof(OID_ipRouteDest) / sizeof(UINT), OID_ipRouteDest
	};
    AsnObjectIdentifier MIB_ipRouteIfIndex = {
        sizeof(OID_ipRouteIfIndex) / sizeof(UINT), OID_ipRouteIfIndex
    };
    AsnObjectIdentifier MIB_ipRouteNextHop = {
        sizeof(OID_ipRouteNextHop) / sizeof(UINT), OID_ipRouteNextHop
    };
    AsnObjectIdentifier MIB_ipRouteType = {
        sizeof(OID_ipRouteType) / sizeof(UINT), OID_ipRouteType
    };
	// Didier LIROULET 2005-10-17 END

	AddLog( _T( "SNMP GetNetworkAdapters...\n"));
    // Load the MIB dll and get the addresses of necessary functions
    if ((hDll = LoadLibrary( _T( "inetmib1.dll"))) < (HINSTANCE) HINSTANCE_ERROR) 
	{
		// Cannot load SNMP MIB
 		AddLog( _T( "SNMP GetNetworkAdapters: Failed to load <inetmib1.dll> !\n"));
		hDll = NULL;
        return FALSE;
    }
    if ((m_Init = (pSnmpExtensionInit) GetProcAddress(hDll, "SnmpExtensionInit")) == NULL)
	{
		// Cannot load SNMP MIB
		AddLog( _T( "SNMP GetNetworkAdapters: Failed to load <SnmpExtensionInit> function from inetmib1.dll !\n"));
		FreeLibrary( hDll);
        hDll = NULL;
        return FALSE;
    }
    if ((m_InitEx = (pSnmpExtensionInitEx) GetProcAddress(hDll, "SnmpExtensionInitEx")) == NULL)
	{
		// Cannot load SNMP MIB
		AddLog( _T( "SNMP GetNetworkAdapters: Failed to load <SnmpExtensionInitEx> function from inetmib1.dll !\n"));
		FreeLibrary( hDll);
        hDll = NULL;
        return FALSE;
    }
    if ((m_Query = (pSnmpExtensionQuery) GetProcAddress(hDll, "SnmpExtensionQuery")) == NULL)
	{
		// Cannot load SNMP MIB
		AddLog( _T( "SNMP GetNetworkAdapters: Failed to load <SnmpExtensionQuery> function from inetmib1.dll !\n"));
		FreeLibrary( hDll);
        hDll = NULL;
        return FALSE;
    }
    if ((m_Trap = (pSnmpExtensionTrap) GetProcAddress(hDll, "SnmpExtensionTrap")) == NULL)
	{
		// Cannot load SNMP MIB
		FreeLibrary( hDll);
		AddLog( _T( "SNMP GetNetworkAdapters: Failed to load <SnmpExtensionTrap> function from inetmib1.dll !\n"));
        hDll = NULL;
        return FALSE;
    }

    m_Init(GetTickCount(), &PollForTrapEvent, &SupportedView);

    // Initialize the variable list to be retrieved by m_Query

	/////////////////////////////////////////////////////////////////
	// First, get  network interfaces from <.iso.org.dod.internet.mgmt.mib-2.interfaces> MIB tree
	//////////////////////////////////////////////////////////////////
    varBindList.list = varBind;
    varBind[0].name = MIB_NULL;
    varBind[1].name = MIB_NULL;
	varBind[2].name = MIB_NULL;
	varBind[3].name = MIB_NULL;
	varBind[4].name = MIB_NULL;
	varBind[5].name = MIB_NULL;

    // Copy in the OID to find the number of entries in the Interface table
    varBindList.len = 1;        // Only retrieving one item
    SNMP_oidcpy(&varBind[0].name, &MIB_ifEntryNumber);
    ret = m_Query(ASN_RFC1157_GETNEXTREQUEST, &varBindList, &errorStatus, &errorIndex);

    // Copy in the OID to retrieve interface properties in the Interface table
	varBindList.len = 6;
    // Copy in the OID of ifType, the type of interface
    SNMP_oidcpy(&varBind[0].name, &MIB_ifEntryType);

    // Copy in the OID of ifphysAddress, the address
    SNMP_oidcpy(&varBind[1].name, &MIB_ifMACAddr);

    // Copy in the OID of ifPhysSpeed, the speed
    SNMP_oidcpy(&varBind[2].name, &MIB_ifSpeed);

    // Copy in the OID of ifDesc, the adapter description
    SNMP_oidcpy(&varBind[3].name, &MIB_ifDesc);

    // Copy in the OID of ifIndex, the adapter index
    SNMP_oidcpy(&varBind[4].name, &MIB_ifIndex);

    /* Copy in the OID of ifOperStatus, the adapter operational status */
    SNMP_oidcpy(&varBind[5].name, &MIB_ifOperStatus);

    do {
        // Submit the query.  Responses will be loaded into varBindList.
        // We can expect this call to succeed a # of times corresponding
        // to the # of adapters reported to be in the system
        ret = m_Query(ASN_RFC1157_GETNEXTREQUEST, &varBindList, &errorStatus, &errorIndex); 
		if (!ret)
			ret = 1;
        else
            // Confirm that the proper type has been returned
            ret = SNMP_oidncmp(&varBind[0].name, &MIB_ifEntryType, MIB_ifEntryType.idLength); 
		if (!ret) 
		{
			CNetworkAdapter cAdapter;

			// Get type of adapter
            nType = varBind[0].value.asnValue.number;
			if (IsLoopback( nType))
			{
				// Loopback adapter => skip it
				continue;
			}
            // Confirm that we have an address here
            ret = SNMP_oidncmp(&varBind[1].name, &MIB_ifMACAddr, MIB_ifMACAddr.idLength);
			// Get adapter speed
			speed = varBind[2].value.asnValue.gauge;
            if ((!ret)
                && (varBind[1].value.asnValue.address.stream != NULL)) 
			{
				csAddress.Format( _T( "%02X:%02X:%02X:%02X:%02X:%02X"),
                        varBind[1].value.asnValue.address.stream[0],
                        varBind[1].value.asnValue.address.stream[1],
                        varBind[1].value.asnValue.address.stream[2],
                        varBind[1].value.asnValue.address.stream[3],
                        varBind[1].value.asnValue.address.stream[4],
                        varBind[1].value.asnValue.address.stream[5]);
				// We can save the info
				csBuffer.Empty();
				for ( UINT uChar=0; uChar < varBind[3].value.asnValue.string.length; uChar++)
					csBuffer.AppendFormat( _T( "%c"), varBind[3].value.asnValue.string.stream[uChar]);
				cAdapter.SetIfIndex( varBind[4].value.asnValue.number);
				cAdapter.SetDescription( csBuffer);
				cAdapter.SetType( GetAdapterType( nType));
				cAdapter.SetTypeMIB( GetIfType( nType));
				cAdapter.SetSpeed( speed);
				cAdapter.SetMACAddress( csAddress);
				cAdapter.SetStatus( varBind[5].value.asnValue.number);
				// IP info not available => we get them later
				cAdapter.SetIPAddress( _T(""));
				cAdapter.SetIPNetMask( _T(""));
				AddLog( _T( "\tSNMP IfAddrEntry index=%ld, Description=%s, Type=%s, MIB Type=%s, Speed=%lu B/s, @ MAC=%s, Status=%s\n"),
							cAdapter.GetIfIndex(),
							cAdapter.GetDescription(),
							cAdapter.GetType(),
							cAdapter.GetTypeMIB(),
							cAdapter.GetByteSpeed(),
							cAdapter.GetMACAddress(),
							cAdapter.GetOperationalStatus());
				// Add interface to list
				pList->AddTail( cAdapter);
				uIndex++;
			}
        }
    }
	while (!ret);
	// Stop only on an error.  An error will occur when we exhaust
	// the list of interfaces to be examined
    // Free the bindings
    SNMP_FreeVarBind(&varBind[0]);
    SNMP_FreeVarBind(&varBind[1]);
    SNMP_FreeVarBind(&varBind[2]);
    SNMP_FreeVarBind(&varBind[3]);
    SNMP_FreeVarBind(&varBind[4]);
    SNMP_FreeVarBind(&varBind[5]);

	//////////////////////////////////////////////////////////////////
	// Next, get network interfaces IP from <.iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable> MIB tree
	//////////////////////////////////////////////////////////////////
    varBind[0].name = MIB_NULL;
    varBind[1].name = MIB_NULL;
	varBind[2].name = MIB_NULL;

    // Copy in the OID to retrieve interface IP properties in the IPAddr table
	varBindList.len = 3;
    // Copy in the OID of ipAdEntIfIndex, the index of interface
    SNMP_oidcpy(&varBind[0].name, &MIB_ipAdEntIfIndex);

    // Copy in the OID of ipAdEntAddr, the IP address
    SNMP_oidcpy(&varBind[1].name, &MIB_ipAdEntAddr);

    // Copy in the OID of ipAdEntNetMask, the IP Net Mask
    SNMP_oidcpy(&varBind[2].name, &MIB_ipAdEntNetMask);

    do {
        // Submit the query.  Responses will be loaded into varBindList.
        // We can expect this call to succeed a # of times corresponding
        // to the # of adapters reported to be in the system
        ret = m_Query(ASN_RFC1157_GETNEXTREQUEST, &varBindList, &errorStatus, &errorIndex); 
		if (!ret)
			ret = 1;
        else
            // Confirm that the proper type has been returned
            ret = SNMP_oidncmp(&varBind[0].name, &MIB_ipAdEntIfIndex, MIB_ipAdEntIfIndex.idLength); 
		if (!ret) 
		{
			// Get IfIndex of adapter
            nType = varBind[0].value.asnValue.number;
            // Confirm that we have an address here
            ret = SNMP_oidncmp(&varBind[1].name, &MIB_ipAdEntAddr, MIB_ipAdEntAddr.idLength);
            if ((!ret)
                && (varBind[1].value.asnValue.address.stream != NULL) 
                && (varBind[2].value.asnValue.address.stream != NULL)) 
			{
				// We can save the info
				csAddress.Format( _T( "%d.%d.%d.%d"),
                        varBind[1].value.asnValue.address.stream[0],
                        varBind[1].value.asnValue.address.stream[1],
                        varBind[1].value.asnValue.address.stream[2],
                        varBind[1].value.asnValue.address.stream[3]);
				csBuffer.Format( _T( "%d.%d.%d.%d"),
                        varBind[2].value.asnValue.address.stream[0],
                        varBind[2].value.asnValue.address.stream[1],
                        varBind[2].value.asnValue.address.stream[2],
                        varBind[2].value.asnValue.address.stream[3]);
				AddLog( _T( "\tSNMP IfTableEntry index=%ld, @ IP=%s, Network Mask=%s\n"),
							varBind[0].value.asnValue.number,
							csAddress, csBuffer);

				// Update network number
				in_addr ipa;
				ULONG   ipAdr, ipMsk, nbRez;		
				CString csRez;
				
				inet_pton(AF_INET, GetAnsiFromTString(csAddress.GetBuffer()), &ipAdr);
				inet_pton(AF_INET, GetAnsiFromTString(csBuffer), &ipMsk);
				nbRez = htonl(ipAdr & ipMsk) ;

				ipa.S_un.S_addr = htonl(nbRez);
				csRez = inet_ntop(AF_INET, &ipa, str, INET_ADDRSTRLEN);

				pList->SetIpAddrEntry( varBind[0].value.asnValue.number,
									   csAddress, csBuffer, csRez);
			}
        }
    }
	while (!ret);
	// Stop only on an error.  An error will occur when we exhaust
	// the list of interfaces to be examined
    // Free the bindings
    SNMP_FreeVarBind(&varBind[0]);
    SNMP_FreeVarBind(&varBind[1]);
    SNMP_FreeVarBind(&varBind[2]);


	// Didier LIROULET 2005-10-17 BEGIN
	//////////////////////////////////////////////////////////////////
	// Next, get network interfaces gateway from <.iso.org.dod.internet.mgmt.mib-2.ip.ipRouteTable> MIB tree
	//////////////////////////////////////////////////////////////////
    varBind[0].name = MIB_NULL;
    varBind[1].name = MIB_NULL;
	varBind[2].name = MIB_NULL;
	varBind[3].name = MIB_NULL;

    // Copy in the OID to retrieve interface IP properties in the IpRoute table
	varBindList.len = 4;
    // Copy in the OID of ipRouteIfIndex, the index of interface
    SNMP_oidcpy(&varBind[0].name, &MIB_ipRouteIfIndex);

    // Copy in the OID of ipRouteNextHop, the gateway IP address
    SNMP_oidcpy(&varBind[1].name, &MIB_ipRouteNextHop);

    // Copy in the OID of ipRouteDest, the IP address to reach (0.0.0.0 for default)
    SNMP_oidcpy(&varBind[2].name, &MIB_ipRouteDest);

    // Copy in the OID of ipRouteType, the route type (2=invalid, 3=direct, 4=indirect)
    SNMP_oidcpy(&varBind[3].name, &MIB_ipRouteType);

    do {
        // Submit the query.  Responses will be loaded into varBindList.
        // We can expect this call to succeed a # of times corresponding
        // to the # of adapters reported to be in the system
        ret = m_Query(ASN_RFC1157_GETNEXTREQUEST, &varBindList, &errorStatus, &errorIndex); 
		if (!ret)
			ret = 1;
        else
            // Confirm that the proper type has been returned
            ret = SNMP_oidncmp(&varBind[0].name, &MIB_ipRouteIfIndex, MIB_ipRouteIfIndex.idLength); 
		if (!ret) 
		{
			// Get IfIndex of adapter
            nType = varBind[0].value.asnValue.number;
            // Confirm that we have an address here and that route is valid and
			// that destination address is 0.0.0.0
            ret = SNMP_oidncmp(&varBind[1].name, &MIB_ipRouteNextHop, MIB_ipRouteNextHop.idLength);
            if ((!ret)
                && (varBind[3].value.asnValue.number != 2)
				&& (varBind[2].value.asnValue.address.stream[0] == 0)
				&& (varBind[2].value.asnValue.address.stream[1] == 0)
				&& (varBind[2].value.asnValue.address.stream[2] == 0)
				&& (varBind[2].value.asnValue.address.stream[3] == 0)) 
			{
				// We can save the info
				csAddress.Format( _T( "%d.%d.%d.%d"),
                        varBind[1].value.asnValue.address.stream[0],
                        varBind[1].value.asnValue.address.stream[1],
                        varBind[1].value.asnValue.address.stream[2],
                        varBind[1].value.asnValue.address.stream[3]);
				AddLog( _T( "\tSNMP IfRouteEntry index=%ld, Gateway=%s\n"),
							varBind[0].value.asnValue.number,
							csAddress);
				pList->SetIpRouteEntry( varBind[0].value.asnValue.number,
									   csAddress);
			}
        }
    }
	while (!ret);
	// Stop only on an error.  An error will occur when we exhaust
	// the list of interfaces to be examined
    // Free the bindings
    SNMP_FreeVarBind(&varBind[0]);
    SNMP_FreeVarBind(&varBind[1]);
    SNMP_FreeVarBind(&varBind[2]);
    SNMP_FreeVarBind(&varBind[3]);
	// Didier LIROULET 2005-10-17 END

	// Unload the MIB dll library
	FreeLibrary( hDll);
	if (uIndex > 0)
	{
		AddLog( _T( "SNMP GetNetworkAdapters: OK (%u objects).\n"), uIndex);
		return TRUE;
	}
	AddLog( _T( "SNMP GetNetworkAdapters: Failed because no network adapter SNMP object !\n"));
	return FALSE;
}
