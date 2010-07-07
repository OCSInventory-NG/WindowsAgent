//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// InventoryResponse.h: interface for the CInventoryResponse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INVENTORYRESPONSE_H__DA9F7D5E_5E2A_4603_AEF6_ECA0D372EB1F__INCLUDED_)
#define AFX_INVENTORYRESPONSE_H__DA9F7D5E_5E2A_4603_AEF6_ECA0D372EB1F__INCLUDED_

#pragma once

#include "OCSInventory Front Classes.h"
#include "Log.h"
#include "Config.h"
#include "ResponseAbstract.h"

class OCSINVENTORYFRONT_API CInventoryResponse : public CResponseAbstract  
{
public: // Methods

	// Standard constructor and destructor
	CInventoryResponse(CByteArray*);
	virtual ~CInventoryResponse();
};

#endif // !defined(AFX_INVENTORYRESPONSE_H__DA9F7D5E_5E2A_4603_AEF6_ECA0D372EB1F__INCLUDED_)
