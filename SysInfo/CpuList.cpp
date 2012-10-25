//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// CpuList.cpp: implementation of the CCpuList class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Cpu.h"
#include "CpuList.h"
#include "OcsCrypto.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCpuList::CCpuList()
{
}

CCpuList::~CCpuList()
{
}

LPCTSTR CCpuList::GetHash()
{
	COcsCrypto	myHash;
	CCpu		myObject;
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
		csToHash.Format( _T( "%s%s%s%u%u%u%u%u%u%s"), myObject.GetManufacturer(), myObject.GetName(),
						 myObject.GetArchitecture(), myObject.GetNumberOfCores(),
						 myObject.GetNumberOfLogicalProcessors(), myObject.GetMaxClockSpeed(),
						 myObject.GetL2CacheSize(), myObject.GetAddressWidth(),
						 myObject.GetDataWidth(), myObject.GetVoltage());
		myHash.HashUpdate( csToHash);
		bContinue = (pos != NULL);
		if (bContinue)
			myObject = GetNext( pos);
	}
	return myHash.HashFinal();
}
