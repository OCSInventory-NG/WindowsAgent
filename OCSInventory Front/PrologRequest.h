//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// PrologRequest.h: interface for the CPrologRequest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROLOGREQUEST_H__86645FAD_D72A_4413_8276_428CE4B45C01__INCLUDED_)
#define AFX_PROLOGREQUEST_H__86645FAD_D72A_4413_8276_428CE4B45C01__INCLUDED_

#pragma once

#include "OCSInventory Front Classes.h"
#include "RequestAbstract.h"

class OCSINVENTORYFRONT_API CPrologRequest : public CRequestAbstract
{

public: // Methods

	// Constructor and destructor
	CPrologRequest();
	virtual ~CPrologRequest();

	// Do some action when request successfully executed
	void setSuccess();
};

#endif // !defined(AFX_PROLOGREQUEST_H__86645FAD_D72A_4413_8276_428CE4B45C01__INCLUDED_)
