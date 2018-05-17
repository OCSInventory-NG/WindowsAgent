//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// CMonitorList.cpp: implementation of the CMonitorList class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Monitor.h"
#include "MonitorList.h"
#include "OcsCrypto.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMonitorList::CMonitorList()
{
}

CMonitorList::~CMonitorList()
{
}

LPCTSTR CMonitorList::GetHash()
{
	COcsCrypto	myHash;
	CMonitor	myObject;
	POSITION	pos;
	BOOL		bContinue;
	CString		csToHash;

	if (GetCount() == 0)
		return NULL;
	if (!myHash.HashInit())
		return NULL;
	pos = GetHeadPosition();
	bContinue = (pos != NULL);
	if (bContinue)
		// There is one record => get the first
		myObject = GetNext( pos);
	while (bContinue)
	{
		csToHash.Format( _T( "%s%s%s%s%s"), myObject.GetManufacturer(), myObject.GetCaption(),
						 myObject.GetDescription(), myObject.GetType(), myObject.GetSerial());
		myHash.HashUpdate( csToHash);
		bContinue = (pos != NULL);
		if (bContinue)
			myObject = GetNext( pos);
	}
	return myHash.HashFinal();
}

BOOL CMonitorList::AddUniqueSerial( CMonitor &myMonitor)
{
	CMonitor	myObject;
	POSITION	pos;
	BOOL		bContinue;
	CString		csSN;

	csSN = myMonitor.GetSerial();
	if (!csSN.IsEmpty())
	{
		// There is a a valid serial number => try to see if not already registered
		pos = GetHeadPosition();
		bContinue = (pos != NULL);
		if (bContinue)
			// There is one record => get the first
			myObject = GetNext( pos);
		while (bContinue)
		{
			csSN = myObject.GetSerial();
			if (!csSN.IsEmpty() && (csSN.Compare( myMonitor.GetSerial()) == 0))
				// This serial is already registered => do not register this monitor
				return FALSE;
			bContinue = (pos != NULL);
			if (bContinue)
				myObject = GetNext( pos);
		}
	}
	// Register this monitor
	AddTail( myMonitor);
	return TRUE;
}