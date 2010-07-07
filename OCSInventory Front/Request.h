//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Request.h: interface for the CRequest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REQUEST_H__3FDFE9E1_C1A0_43AF_BC61_1132F2E5F83F__INCLUDED_)
#define AFX_REQUEST_H__3FDFE9E1_C1A0_43AF_BC61_1132F2E5F83F__INCLUDED_

#pragma once

#include "OCSInventory Front Classes.h"
#include "RequestAbstract.h"

class OCSINVENTORYFRONT_API CRequest : public CRequestAbstract
{
public: // methods

	// Constructor and destructor
	CRequest();
	virtual ~CRequest();

	// Do some action when request successfully executed
	void setSuccess();
};

#endif // !defined(AFX_REQUEST_H__3FDFE9E1_C1A0_43AF_BC61_1132F2E5F83F__INCLUDED_)
