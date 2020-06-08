//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// CapacityAbstract.h: interface for the CCapacityAbstract class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAPACITYABSTRACT_H__A635C2F7_CFC5_4C49_9F2D_6EAB2046083E__INCLUDED_)
#define AFX_CAPACITYABSTRACT_H__A635C2F7_CFC5_4C49_9F2D_6EAB2046083E__INCLUDED_

#pragma once

#include "PrologRequest.h"
#include "PrologResponse.h"
#include "InventoryRequest.h"
#include "InventoryResponse.h"
#include "Log.h"
#include "OCSInventory Front.h"

class CCapacityAbstract  
{
public: // Methods

	// Standard constructor and destructor
	CCapacityAbstract();
	virtual ~CCapacityAbstract();
	// Setters
	void setProlog( CPrologRequest* );
	void setPrologResp( CPrologResponse* );
	void setInventory( CInventoryRequest* );
	void setInventoryResp( CInventoryResponse* );


protected: // Attributes
	// The logger object
	CLog *m_pLogger;
	// The prolog
	CPrologRequest *m_pProlog;
	// The current prolog response object
	CPrologResponse *m_pPrologResp;
	// The Inventory to feed
	CInventoryRequest *m_pInventory;
	// The inventory response for late call
	CInventoryResponse *m_pInventoryResp;
};

#endif // !defined(AFX_CAPACITYABSTRACT_H__A635C2F7_CFC5_4C49_9F2D_6EAB2046083E__INCLUDED_)
