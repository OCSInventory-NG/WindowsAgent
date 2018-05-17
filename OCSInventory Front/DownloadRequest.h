//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// DownloadRequest.h: interface for the CDownloadRequest class.
//
//////////////////////////////////////////////////////////////////////


#ifndef _INVENTORYREQUEST_H_INCLUDED_
#define _INVENTORYREQUEST_H_INCLUDED_

#pragma once

#include "OCSInventory Front Classes.h"
#include "RequestAbstract.h"

class OCSINVENTORYFRONT_API CDownloadRequest : public CRequestAbstract
{
public:
	CDownloadRequest(void);
	~CDownloadRequest(void);

	// Do some action when request successfully executed
	void setSuccess();
	// Set package execution result code and optionnally command ouput
	BOOL setPackageResult( LPCTSTR lpstrPackID, LPCTSTR lpstrCode, LPCTSTR lpstrOutput = NULL);
};

#endif //_INVENTORYREQUEST_H_INCLUDED_
