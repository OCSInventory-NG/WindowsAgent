//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// NetworkAdapter.cpp: implementation of the CNetworkAdapter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "NetworkAdapter.h"
#include <ipifcons.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetworkAdapter::CNetworkAdapter()
{
	Clear();
}

CNetworkAdapter::~CNetworkAdapter()
{
	Clear();
}

LPCTSTR CNetworkAdapter::GetNetNumber()
{
	return m_csNetNumber;
}

LPCTSTR CNetworkAdapter::GetType()
{
	return m_csType;
}

LPCTSTR CNetworkAdapter::GetDescription()
{
	return m_csDescription;
}

LPCTSTR CNetworkAdapter::GetSpeed()
{
	return m_csSpeed;
}

ULONG CNetworkAdapter::GetByteSpeed()
{
	return m_ulSpeed;
}

LPCTSTR CNetworkAdapter::GetMACAddress()
{
	return m_csHWAddr;
}

LPCTSTR CNetworkAdapter::GetIPAddress()
{
	return m_csIPAddr;
}

LPCTSTR CNetworkAdapter::GetTypeMIB()
{
	return m_csTypeMIB;
}

LPCTSTR CNetworkAdapter::GetOperationalStatus()
{
	return m_csOperStatus;
}

LPCTSTR CNetworkAdapter::GetIPNetMask()
{
	return m_csIPNetMask;
}

LPCTSTR CNetworkAdapter::GetGateway()
{
	return m_csGateway;
}

LPCTSTR CNetworkAdapter::GetDhcpServer()
{
	return m_csDhcpServer;
}

LPCTSTR CNetworkAdapter::GetMtu()
{
	return m_csMtu;
}

LONG CNetworkAdapter::GetIfIndex()
{
	return m_lIndex;
}

void CNetworkAdapter::SetNetNumber( LPCTSTR nbr )
{
	m_csNetNumber = nbr;
	StrForSQL( m_csNetNumber);
}

void CNetworkAdapter::SetDescription( CString lpstrDescription)
{
	m_csDescription = lpstrDescription;
	StrForSQL( m_csDescription);
}

void CNetworkAdapter::SetType( LPCTSTR lpstrType)
{
	m_csType = lpstrType;
	StrForSQL( m_csType);
}

void CNetworkAdapter::SetTypeMIB( LPCTSTR lpstrMIBType)
{
	m_csTypeMIB = lpstrMIBType;
	StrForSQL( m_csTypeMIB);
}

void CNetworkAdapter::SetSpeed( LPCTSTR lpstrSpeed)
{
	CString csBuffer = lpstrSpeed;

	m_csSpeed = lpstrSpeed;
	StrForSQL( m_csSpeed);
	// Recalculate byte speed
	if (csBuffer.Find( _T( " Gb/s")) >= 0)
	{
		csBuffer.TrimRight( _T( " Gb/s"));
		m_ulSpeed = _tcstoul( csBuffer, NULL, 10) * 1000000000;
		return;
	}
	if (csBuffer.Find( _T( " Mb/s")) >= 0)
	{
		csBuffer.TrimRight( _T( " Mb/s"));
		m_ulSpeed = _tcstoul( csBuffer, NULL, 10) * 1000000;
		return;
	}
	if (csBuffer.Find( _T( " Kb/s")) >= 0)
	{
		csBuffer.TrimRight( _T( " Kb/s"));
		m_ulSpeed = _tcstoul( csBuffer, NULL, 10) * 1000;
		return;
	}
	csBuffer.TrimRight( _T( " b/s"));
	m_ulSpeed = _tcstoul( csBuffer, NULL, 10);
}

void CNetworkAdapter::SetSpeed( ULONG lSpeed)
{
	m_ulSpeed = lSpeed;
	
	if (lSpeed >= 1000000000)
		m_csSpeed.Format( _T( "%lu Gb/s"), lSpeed / 1000000000);
	else
	{
		if (lSpeed >= 1000000)
			m_csSpeed.Format( _T( "%lu Mb/s"), lSpeed / 1000000);
		else
		{
			if (lSpeed >= 1000)
				m_csSpeed.Format( _T( "%lu Kb/s"), lSpeed / 1000);
			else
				m_csSpeed.Format( _T( "%lu b/s"), lSpeed);
		}
	}

}

void CNetworkAdapter::SetMACAddress( LPCTSTR lpstrMAC)
{
	m_csHWAddr = lpstrMAC;
	StrForSQL( m_csHWAddr);
}

void CNetworkAdapter::SetStatus( LPCTSTR lpstrStatus)
{
	m_csOperStatus = lpstrStatus;
	StrForSQL( m_csOperStatus);
}

void CNetworkAdapter::SetStatus( LONG lOperStatus)
{
	switch (lOperStatus)
	{
	case 1: // Up
		m_csOperStatus = _T( "Up");
		break;
	case 2: // Down
		m_csOperStatus = _T( "Down");
		break;
	case 3: // Testing
		m_csOperStatus = _T( "Testing");
		break;
	default: // Unknown
		m_csOperStatus = _T( "");
		break;
	}
}

void CNetworkAdapter::SetIpHelperStatus( LONG lOperStatus)
{
	switch (lOperStatus)
	{
	case IfOperStatusDormant:
	case IfOperStatusNotPresent:
	case IfOperStatusDown:
		m_csOperStatus = _T( "Down");
		break;
	case IfOperStatusTesting:
		m_csOperStatus = _T( "Testing");
		break;
	case IfOperStatusUp:
		m_csOperStatus = _T( "Up");
		break;
	default: // Unknown
		m_csOperStatus = _T( "");
		break;
	}
}

void CNetworkAdapter::SetIPAddress( LPCTSTR lpstrIP)
{
	m_csIPAddr = lpstrIP;
	StrForSQL( m_csIPAddr);
}

void CNetworkAdapter::SetIPNetMask( LPCTSTR lpstrNetMask)
{
	m_csIPNetMask = lpstrNetMask;
	StrForSQL( m_csIPNetMask);
}

void CNetworkAdapter::SetGateway( LPCTSTR lpstrGateway)
{
	m_csGateway = lpstrGateway;
	StrForSQL( m_csGateway);
}

void CNetworkAdapter::SetDhcpServer( LPCTSTR lpstrDhcpServer)
{
	m_csDhcpServer = lpstrDhcpServer;
	StrForSQL( m_csDhcpServer);
}

void CNetworkAdapter::SetMtu(LPCTSTR lpstrMtu)
{
	m_csMtu = lpstrMtu;
	StrForSQL(m_csMtu);
}

void CNetworkAdapter::SetIfIndex( LONG lIndex)
{
	m_lIndex = lIndex;
}

void CNetworkAdapter::Clear()
{
	m_csNetNumber.Empty();
	m_lIndex = -1;		
	m_csDescription.Empty();
	m_csType.Empty();		
	m_csTypeMIB.Empty();	
	m_csSpeed.Empty();		
	m_ulSpeed = 0;		
	m_csHWAddr.Empty();		
	m_csOperStatus.Empty();	
	m_csIPAddr.Empty();		
	m_csIPNetMask.Empty();	
	m_csGateway.Empty();	
	m_csDhcpServer.Empty();	
	m_csMtu.Empty();
}

int CNetworkAdapter::operator==(CNetworkAdapter cObject) const
{
	return ((m_csNetNumber == cObject.GetNetNumber()) &&
	(m_lIndex == cObject.GetIfIndex()) &&
	(m_csDescription == cObject.GetDescription()) &&
	(m_csType == cObject.GetType()) &&
	(m_csTypeMIB == cObject.GetTypeMIB()) &&
	(m_csSpeed == cObject.GetSpeed()) &&
	(m_ulSpeed == cObject.GetByteSpeed()) &&
	(m_csHWAddr == cObject.GetMACAddress()) &&	
	(m_csOperStatus == cObject.GetOperationalStatus()) &&
	(m_csIPAddr == cObject.GetIPAddress()) &&	
	(m_csIPNetMask == cObject.GetIPNetMask()) &&
	(m_csGateway == cObject.GetGateway()) &&
	(m_csDhcpServer == cObject.GetDhcpServer()) && 
	(m_csMtu == cObject.GetMtu()));
}
