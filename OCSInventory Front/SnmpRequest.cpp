//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

#include "StdAfx.h"
#include "SnmpRequest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSnmpRequest::CSnmpRequest(BOOL)
{
	//CDeviceid *pDeviceid = getOcsDeviceID();

	setQuery(_T("SNMP"));
}

CSnmpRequest::~CSnmpRequest(void)
{
}

void CSnmpRequest::setSuccess()
{
}
