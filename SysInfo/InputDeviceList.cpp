//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// InputDeviceList.cpp: implementation of the CInputDeviceList class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "InputDevice.h"
#include "InputDeviceList.h"
#include "OcsCrypto.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CInputDeviceList::CInputDeviceList()
{
}

CInputDeviceList::~CInputDeviceList()
{
}

LPCTSTR CInputDeviceList::GetHash()
{
	COcsCrypto	myHash;
	CInputDevice myObject;
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
		csToHash.Format( _T( "%s%s%s%s%s%s"), myObject.GetType(), myObject.GetManufacturer(),
						 myObject.GetCaption(), myObject.GetDescription(), myObject.GetPointingInterface(),
						 myObject.GetPointingType());
		myHash.HashUpdate( csToHash);
		bContinue = (pos != NULL);
		if (bContinue)
			myObject = GetNext( pos);
	}
	return myHash.HashFinal();
}
