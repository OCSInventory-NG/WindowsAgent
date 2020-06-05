//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// PrinterList.cpp: implementation of the CPrinterList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Printer.h"
#include "PrinterList.h"
#include "OcsCrypto.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPrinterList::CPrinterList()
{
}

CPrinterList::~CPrinterList()
{
}

LPCTSTR CPrinterList::GetHash()
{
	COcsCrypto	myHash;
	CPrinter	myObject;
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
		csToHash.Format( _T( "%s%s%s%s%s%s%s%u%u"), myObject.GetName(), myObject.GetDriver(),
						 myObject.GetPort(), myObject.GetServerName(), myObject.GetShareName(),
						 myObject.GetResolution(), myObject.GetComment(),
						 myObject.IsShared(), myObject.IsNetworkPrinter());
		myHash.HashUpdate( csToHash);
		bContinue = (pos != NULL);
		if (bContinue)
			myObject = GetNext( pos);
	}
	return myHash.HashFinal();
}
