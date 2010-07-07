//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// DownloadResponse.h: interface for the CDownloadResponse class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _INVENTORYRESPONSE_H_INCLUDED_
#define _INVENTORYRESPONSE_H_INCLUDED_

#pragma once

#include "OCSInventory Front Classes.h"
#include "ResponseAbstract.h"

class OCSINVENTORYFRONT_API CDownloadResponse :	public CResponseAbstract
{
public:
	CDownloadResponse(CByteArray*);
	~CDownloadResponse(void);
};

#endif // _INVENTORYRESPONSE_H_INCLUDED_
